#ifndef QXPACK_IC_THREADPOOL_CXX
#define QXPACK_IC_THREADPOOL_CXX

#include <thread> // C++11
#include <vector> // C++11
#include <chrono> // C++11
#include <map>    // C++11
#include <new>    // C++11

#include "qxpack_ic_pimplprivtemp.hpp"
#include "qxpack_ic_tmrqueue.hxx"
#include "qxpack_ic_threadpool.hxx"
#include "qxpack_ic_semtemp.hpp"
#include "qxpack_ic_memcntr.hxx"
#include "qxpack_ic_heapvectortemp.hpp"
#include "qxpack_ic_variant.hxx"
#include "qxpack_ic_queuetemp.hpp"
#include "qxpack_ic_objcache.hxx"

#if defined( WIN32 )
#include <windows.h>
#endif

namespace QxPack {
namespace IcThreadPoolSpc {

// ////////////////////////////////////////////////////////////////////////////
//
//                 global
//
// ////////////////////////////////////////////////////////////////////////////
typedef std::chrono::steady_clock::time_point  IcTmrStamp;
static inline IcTmrStamp  gMakeTimeStampFromNow ( int mill )
{ return std::chrono::steady_clock::now() + std::chrono::milliseconds(mill); }

// ////////////////////////////////////////////////////////////////////////////
//
//                shared semaphore
//
// ////////////////////////////////////////////////////////////////////////////
class SharedSem : public IcPImplPrivTemp<SharedSem> {
private:  IcSemTemp m_sem;
public :
    explicit SharedSem( ) { qxpack_ic_incr_new_cntr(1);  }
    SharedSem( const SharedSem & ) {  }
    virtual ~SharedSem( ) override { qxpack_ic_decr_new_cntr(1); }
    inline IcSemTemp*  semObj() { return &m_sem; }
};


// ////////////////////////////////////////////////////////////////////////////
//
//                 command pipe [ thread safe ]
//
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// CmdItem used to store the command information that used in tread pool
// ============================================================================
class QXPACK_IC_HIDDEN  CmdItem QXPACK_IC_FINAL {
public :
    enum CmdID {
        CmdID_Nop = 0, CmdID_ExitWorker, CmdID_NewRunObj, CmdID_IdleThread,
        CmdID_ShrinkRunObj, CmdID_WaitForDoneSem
    };
private: IcVariant m_var[2];  CmdID  m_cmd;
public :
    explicit CmdItem () : m_cmd( CmdID_Nop ) { }

    CmdItem ( CmdID cid, const IcVariant &var_a, const IcVariant &var_b )
    { m_cmd = cid; m_var[0] = var_a; m_var[1] = var_b; }

    CmdItem ( const CmdItem &ot )
    { m_cmd = ot.m_cmd; m_var[0] = ot.m_var[0]; m_var[1] = ot.m_var[1]; }

    CmdItem & operator = ( const CmdItem &ot )
    {
      m_cmd = ot.m_cmd; m_var[0] = ot.m_var[0]; m_var[1] = ot.m_var[1];
      return *this;
    }  

    inline void   emptyVar( ) { m_var[0].makeNull(); m_var[1].makeNull(); }
    inline CmdID  cmdId() const            { return m_cmd; }
    inline const IcVariant &  varA() const { return m_var[0]; }
    inline const IcVariant &  varB() const { return m_var[1]; }
    inline void   setCmdId( CmdID c )      { m_cmd = c; }
    inline void   setVarA ( const IcVariant &v ) { m_var[0] = v; }
    inline void   setVarB ( const IcVariant &v ) { m_var[1] = v; }

    static void  makeNop       ( CmdItem & );
    static void  makeExitWorker( CmdItem & );
    static void  makeNewRunObj ( CmdItem &, IcRunnable*, int );
    static void  makeIdleThread( CmdItem &, void* );
    static void  makeShrinkRO  ( CmdItem & );
    static void  makeWaitForDoneSem ( CmdItem &, SharedSem & );
};

void   CmdItem :: makeExitWorker( CmdItem &i ) // static
{ i.setCmdId( CmdID_ExitWorker ); i.emptyVar(); }

void   CmdItem :: makeNewRunObj ( CmdItem &i, IcRunnable *r, int p ) // static
{ i.setCmdId( CmdID_NewRunObj ); i.setVarA( r ); i.setVarB( p ); }

void   CmdItem :: makeIdleThread( CmdItem &i, void *p )  // static
{ i.setCmdId( CmdID_IdleThread ); i.emptyVar(); i.setVarA( p ); }

void   CmdItem :: makeNop( CmdItem &i ) // static
{ i.setCmdId( CmdID_Nop ); i.emptyVar(); }

void   CmdItem :: makeShrinkRO( CmdItem &i ) // static
{ i.setCmdId( CmdID_ShrinkRunObj ); i.emptyVar(); }

void   CmdItem :: makeWaitForDoneSem( CmdItem &i, SharedSem &sem ) // static
{
    i.setCmdId( CmdID_WaitForDoneSem );
    i.emptyVar();
    SharedSem::addRef( &sem );
    i.setVarA( &sem );
}

// ============================================================================
// CmdPipe used to make a sequence of CmdItem
// ============================================================================
#define CMDPIPE_CacheShrinkGap  (3000)
#define CMDPIPE_CacheMaxNum     (16)

class QXPACK_IC_HIDDEN  CmdPipe : public IcPImplPrivTemp< CmdPipe > {
    typedef IcNodeQueueTemp<CmdItem> Que;

    // ========================================================================
    // the operator interface of cache
    // ========================================================================
    struct QXPACK_IC_HIDDEN CacheOpr : public IcObjCache::OprIF {
        virtual        ~CacheOpr()  override { }
        virtual void   deleteSelf() override { qxpack_ic_delete( this, CacheOpr ); }
        virtual int    verID()      override { return 0; }
        virtual void*  createObj () override { return qxpack_ic_new( Que::Node ); }
        virtual void   deinitObj ( void* ) override { }
        virtual void   initObj   ( void* ) override { }
        virtual void   deleteObj ( void* ptr ) override
        {
            qxpack_ic_debug("IcThreadPool: CmdPipe cache delete object:%x", ptr );
            qxpack_ic_delete( ptr, Que::Node );
        }
    };

public :
    typedef void (*ItemCallback)( CmdItem &, void* );

private:
    IcSemTemp     m_sem;   Que   m_cmd_que_ts;
    ItemCallback  m_cb;    void *m_cb_ctxt;
    IcObjCache    m_obj_cache_ts;

protected:
    // ========================================================================
    // allocate a usable node, prefer to use cache
    // ========================================================================
    CmdPipe::Que::Node*  allocNode( )
    {
        auto *n = reinterpret_cast<Que::Node*>( m_obj_cache_ts.alloc());
        return ( n != nullptr ? n : qxpack_ic_new( Que::Node ));
    }

    // ========================================================================
    // recycle the node into cache
    // ========================================================================
    inline void  recycleNode( Que::Node *n ) { m_obj_cache_ts.recycle( n ); }

public :
    explicit CmdPipe( ItemCallback cb, void * );
    CmdPipe ( const CmdPipe & );
    virtual ~CmdPipe () override;
    void  takeAndPost( const CmdItem & );
    bool  waitForCmd ( CmdItem &, int timeout_ms = -1 );
    bool  isEmpty()  { return m_cmd_que_ts.isEmpty(); }
};

CmdPipe :: CmdPipe ( ItemCallback cb, void*cb_ctxt )
    : m_cmd_que_ts ( []( Que::Node *n, void* ){ qxpack_ic_delete( n, Que::Node ); }, this ),
      m_obj_cache_ts( CMDPIPE_CacheMaxNum, CMDPIPE_CacheShrinkGap, qxpack_ic_new( CacheOpr ))
{   m_cb = cb; m_cb_ctxt = cb_ctxt; }

CmdPipe :: CmdPipe ( const CmdPipe & ) // this never ocurred!
    : m_cmd_que_ts ( []( Que::Node *n, void* ){ qxpack_ic_delete( n, Que::Node ); }, this ),
      m_obj_cache_ts( CMDPIPE_CacheMaxNum, CMDPIPE_CacheShrinkGap, qxpack_ic_new( CacheOpr ))
{   qxpack_ic_fatal("never ocurred!"); }

CmdPipe :: ~CmdPipe( )
{
    m_cmd_que_ts.blockDequeueUntil (
       []( Que::Node *n, void *ctxt ) {
           auto *t_this = reinterpret_cast<CmdPipe*>( ctxt );
           t_this->m_cb( n->dataRef(), t_this->m_cb_ctxt );
           qxpack_ic_delete( n, Que::Node );
       }, this,
       []( Que::Node*, void* )->bool { return true; }, nullptr
    );
}

void    CmdPipe :: takeAndPost( const CmdItem &i )
{
    m_cmd_que_ts.takeAndEnqueue( allocNode()->setData( i ) );
    m_sem.notify();
}

bool    CmdPipe :: waitForCmd ( CmdItem &i, int timeout_ms )
{
    bool ret = true;
    if ( timeout_ms > 0 ) {
         ret = m_sem.waitUntil( gMakeTimeStampFromNow( timeout_ms ));
    } else {
         m_sem.wait();
    }
    if ( ! ret ) { return ret; }

    Que::Node *n = m_cmd_que_ts.dequeue();
    if ( ( ret = ( n != nullptr )) ) {
        i = n->dataRef();
        n->setData( CmdItem() );
        recycleNode( n );
    }
    return ret;
}


// ////////////////////////////////////////////////////////////////////////////
//
//                      Runnable queue [ Thread Safe ]
//
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// the run object queue manager
// ============================================================================
class QXPACK_IC_HIDDEN  RunObjQue : public IcPImplPrivTemp<RunObjQue> {
public :
    struct QXPACK_IC_HIDDEN Item {
        IcRunnable* obj; int prior;
        explicit Item() : obj( nullptr ), prior(0) { }
        Item( IcRunnable* ptr, int p ) : obj(ptr), prior(p) { }
        inline bool isValid() const { return ( obj != nullptr ); }
    };

private :
    struct QXPACK_IC_HIDDEN GtCmp {  // big heap compare
        inline bool operator()( const Item &lhs , const Item &rhs ) const
        { return lhs.prior > rhs.prior; }
    };
    typedef IcHeapVector<Item,GtCmp> RunObjHeap;

private :
    RunObjHeap   m_que_ts;

public :
    typedef void (*ItemCallback)( Item &, void* );

    explicit RunObjQue();
    RunObjQue ( const RunObjQue & );
    ~RunObjQue();

    inline Item  deque()                { Item i; m_que_ts.removeAt(0,i); return i; }
    inline void  enque( const Item &i ) { m_que_ts.insert(i); }
    inline int   size()    { return m_que_ts.size();    }
    inline bool  isEmpty() { return m_que_ts.isEmpty(); }
    inline void  clear  ( ItemCallback cb, void *ctxt ) { m_que_ts.clear( cb, ctxt );   }

    void  tryShrink();
};

RunObjQue :: RunObjQue() : m_que_ts( nullptr, nullptr, 16, true )
{}

RunObjQue :: RunObjQue( const RunObjQue & ) :  m_que_ts( nullptr, nullptr, 16, true )
{ qxpack_ic_fatal("never ocurred!"); }

RunObjQue :: ~RunObjQue()
{ m_que_ts.clear( []( Item &i, void* ) { IcRunnable::dispose( i.obj ); }, this ); }

void    RunObjQue :: tryShrink()
{
    m_que_ts.shrinkIf (
        []( const std::vector<Item> &vec, void* )->int {
            const size_t nn = 32;
            size_t curr_sz  = vec.size();
            size_t cap_sz   = vec.capacity();
            size_t round_sz = ( curr_sz + nn - 1 ) / nn * nn;
            int shrink_sz =  int(( cap_sz > nn  &&  round_sz * 2 + nn < cap_sz ) ? cap_sz / 2 : -1 );
            if ( shrink_sz > 0 ) {
                qxpack_ic_debug("IcThreadPool: RunObjQue shrink %d cap. to %d", cap_sz, shrink_sz );
            } else {
                qxpack_ic_debug("IcThreadPool: RunObjQue need not to shrink cap.(%d)", cap_sz );
            }
            return shrink_sz;
        }, this
    );
}



// ////////////////////////////////////////////////////////////////////////////
//
//                 thread item
//
// ////////////////////////////////////////////////////////////////////////////
typedef IcRunnable*  IcRunnablePtr;
class QXPACK_IC_HIDDEN ThrItem {
private:
    IcSemTemp    m_sem;         // work sem. used to wakeup the worker thread
    std::thread  m_thr;         // thread of current item
    IcTmrStamp   m_tmr_stamp;   // while idle, the current time
    IcRunnablePtr  m_run_obj;   // current runable object pointer

    CmdPipe    *m_cmd_pipe_ts;
    RunObjQue  *m_ro_que_ts;
    bool  m_is_run;             // control the thread running.

protected:
    void   doWork_thr();
public :
    explicit ThrItem( CmdPipe *cp, RunObjQue *rp )
    {
        QXPACK_IC_ASSERT( cp != nullptr && rp != nullptr, "pointer are null!" );
        m_cmd_pipe_ts = cp; m_ro_que_ts = rp;
        m_run_obj = nullptr;  m_is_run = true;
        m_thr = std::thread( [this](){ this->doWork_thr(); } );
    }
    ~ThrItem()
    {
        markStopRun();
        wakeUp();
        m_thr.join();
    }
    inline IcTmrStamp &  timeStamp() { return m_tmr_stamp; }
    inline void  markStopRun( ) {  m_is_run = false; }
    inline void  setRunObj( IcRunnablePtr ptr ) { m_run_obj = ptr; }
    inline void  wakeUp() { m_sem.notify(); }
};
typedef ThrItem* ThrItemPtr;

// ============================================================================
// do the work
// ============================================================================
void                    ThrItem :: doWork_thr()
{
    volatile IcRunnablePtr *r_obj = & m_run_obj;
    volatile bool *is_run = & m_is_run;

    while ( *is_run ) {
        m_sem.wait();

        // --------------------------------------------------------------------
        // handle run objects until no object or require this thread ready to existed.
        // --------------------------------------------------------------------
        while ( *is_run && *r_obj != nullptr ) {
            if ( *r_obj != nullptr ) {
                (*r_obj)->run(); IcRunnable::dispose( *r_obj ); *r_obj = nullptr;
            }
            RunObjQue::Item i = m_ro_que_ts->deque();
            *r_obj = i.obj;
        }

        // --------------------------------------------------------------------
        // if no run object, tell manager this item is about to enter idle
        // --------------------------------------------------------------------
        if ( *is_run ) {
            CmdItem cmd;
            CmdItem::makeIdleThread( cmd, this );
            m_cmd_pipe_ts->takeAndPost( cmd );
        }
    }

    // NOTE: must re-check the runobject pointer, makesure process it!
    if ( *r_obj != nullptr ) {
        (*r_obj)->run(); IcRunnable::dispose( *r_obj ); *r_obj = nullptr;
    }
}

}


// ////////////////////////////////////////////////////////////////////////////
//
//                      Private Object
//
// ////////////////////////////////////////////////////////////////////////////
using namespace IcThreadPoolSpc;
#define THRPOOLWKR_ROQ_ShrinkGap  (2000)
#define THRPOOLWKR_EXPIRY_TM      (2000)
#define THRPOOLWKR_MAX_THR_CNTR   (4)
#define T_Worker( o ) T_ObjCast( IcThreadPoolWorker*, o )

class QXPACK_IC_HIDDEN  IcThreadPoolWorker {
    typedef std::pair<ThrItemPtr,IcTmrStamp>  ThrItemPair;
    typedef std::map <ThrItemPtr,IcTmrStamp>  ThrItemMap;
private:
    RunObjQue  m_ro_que_ts;           // store the IRunnable objects
    CmdPipe    m_cmd_pipe_ts;         // the command pipe that used to store orders.

    IcTmrQueue  *m_tmr_que_ts;

    ThrItemMap   m_idle_thr_map_thr, m_work_thr_map_thr; // two thread map used to remember thread items
    std::thread  m_thr_wkr;
    std::atomic<int>  m_expiry_tm_ts, m_max_thr_cntr_ts;

protected: 
    void  shedShrinkRunObjQue_ts ( );

    void  notifyAllWfd_thr( std::vector<void*> & );
    bool  tryDispatchRunObj_thr( IcRunnablePtr ptr, int prior );
    void  doWork_thr();
    void  cmdPipe_onClearCallback( CmdItem &, void* );

    void  cleanupIdleThrItem_thr();
    void  cleanupWorkThrItem_thr();

public :
    explicit IcThreadPoolWorker();
    ~IcThreadPoolWorker();

    inline int  expiryTime() const { return m_expiry_tm_ts.load   ( std::memory_order_seq_cst ); }
    inline int  maxThrCnt () const { return m_max_thr_cntr_ts.load( std::memory_order_seq_cst ); }
    inline RunObjQue &  runObjQueRef() { return m_ro_que_ts; }

    inline void postWfdCmd( SharedSem &sem )
    { CmdItem item; CmdItem::makeWaitForDoneSem( item, sem );   m_cmd_pipe_ts.takeAndPost( item ); }

    inline void postRunObj( IcRunnablePtr ptr, int prior )
    { CmdItem item; CmdItem::makeNewRunObj( item, ptr, prior ); m_cmd_pipe_ts.takeAndPost( item ); }

    void  setExpiryTime( int t ) { m_expiry_tm_ts.store( t < 100 ? 100 : t ); }
    void  setMaxThrCnt ( int c ) { m_max_thr_cntr_ts.store( c < 1 ? 1 : c ); }
};

// ============================================================================
// ctor
// ============================================================================
IcThreadPoolWorker :: IcThreadPoolWorker ( ) :
    m_cmd_pipe_ts( []( CmdItem &i, void *ctxt ){
        reinterpret_cast<IcThreadPoolWorker*>(ctxt)->cmdPipe_onClearCallback( i, ctxt ); }, this
    )
{
    m_max_thr_cntr_ts.store( THRPOOLWKR_MAX_THR_CNTR );
    m_expiry_tm_ts.store( THRPOOLWKR_EXPIRY_TM );

    m_thr_wkr    = std::thread( [this](){ this->doWork_thr(); } );
    m_tmr_que_ts = qxpack_ic_new( IcTmrQueue, IcTmrQueue::disposeObjCbQueName() );
    shedShrinkRunObjQue_ts();
}

// ============================================================================
// dtor
// ============================================================================
IcThreadPoolWorker :: ~IcThreadPoolWorker ( )
{
    qxpack_ic_delete( m_tmr_que_ts, IcTmrQueue );
    CmdItem item( CmdItem::CmdID_ExitWorker, IcVariant(), IcVariant() );
    m_cmd_pipe_ts.takeAndPost( item );   
    m_thr_wkr.join();
}

// ============================================================================
// [ insurance ]  clear all command in the command queue
// ============================================================================
void    IcThreadPoolWorker :: cmdPipe_onClearCallback( CmdItem &i, void* )
{
    switch( i.cmdId() ) {
    case CmdItem::CmdID_NewRunObj : {  // process the new run object, and dispose it
        auto *run_obj = reinterpret_cast<IcRunnablePtr>( i.varA().toVoidStar());
        if ( run_obj != nullptr ) { run_obj->run(); IcRunnable::dispose( run_obj ); }
    } break;

    case CmdItem::CmdID_IdleThread : { // delete the idle thread
        auto *thr = reinterpret_cast<ThrItemPtr>( i.varA().toVoidStar());
        if ( thr != nullptr ) { qxpack_ic_delete( thr, ThrItem ); }
    } break;

    case CmdItem::CmdID_WaitForDoneSem : { // sem that used to wait for all job done
        auto *sem = reinterpret_cast<SharedSem*>( i.varA().toVoidStar());
        sem->semObj()->notify();
        SharedSem::releaseRef( sem );
    } break;

    default : break;
    }
}

// ============================================================================
// [ thread-safe ] shedule a callback to solve the run object queue
// ============================================================================
void   IcThreadPoolWorker :: shedShrinkRunObjQue_ts ( )
{
    m_tmr_que_ts->singleShot (
        THRPOOLWKR_ROQ_ShrinkGap,
        []( qxic_uid_t, const IcVariant &p1, const IcVariant &, const IcVariant &, const IcVariant &, bool is_valid ) {
            if ( ! is_valid ) { return; }
            auto *t_this = reinterpret_cast<IcThreadPoolWorker*>(p1.toVoidStar());
            if ( t_this == nullptr ) { return; }
            CmdItem cmd; CmdItem::makeShrinkRO( cmd );
            t_this->m_cmd_pipe_ts.takeAndPost( cmd );
        }, IcVariant(this), IcVariant()
    );
}

// ============================================================================
// delete the idle work thread item
// ============================================================================
void    IcThreadPoolWorker :: cleanupIdleThrItem_thr()
{
    auto c_itr = m_idle_thr_map_thr.cbegin();
    while ( c_itr != m_idle_thr_map_thr.cend()) {
        auto *thr = c_itr->first; ++ c_itr;
        if ( thr != nullptr ) { qxpack_ic_delete( thr, ThrItem ); }
    }
    ThrItemMap tmp;
    m_idle_thr_map_thr.swap( tmp );
}

// ============================================================================
// delete the working thread item
// ============================================================================
void    IcThreadPoolWorker :: cleanupWorkThrItem_thr()
{
    // first stop the running
    auto c_itr = m_work_thr_map_thr.cbegin();
    while ( c_itr != m_work_thr_map_thr.cend() ) {
        auto *thr = c_itr->first; ++ c_itr;
        if ( thr != nullptr ) { thr->markStopRun(); thr->wakeUp(); }
    }

    // second try delete each one
    auto c_itr2 = m_work_thr_map_thr.cbegin();
    while ( c_itr2 != m_work_thr_map_thr.cend()) {
        auto *thr = c_itr2->first; ++ c_itr2;
        if ( thr != nullptr ) { qxpack_ic_delete( thr, ThrItem ); }
    }
    ThrItemMap tmp;
    m_work_thr_map_thr.swap( tmp );
}

// ============================================================================
//! \brief  try use a thread to run the run object
//! \note   pass ( nullptr, 0 ) to make solve run object in the queue.
//! \return true means has been solved a run object
// ============================================================================
bool    IcThreadPoolWorker :: tryDispatchRunObj_thr( IcRunnablePtr ptr, int prior )
{
    ThrItem *thr = nullptr;
    auto &idle_map = m_idle_thr_map_thr;
    auto &work_map = m_work_thr_map_thr;

    // ------------------------------------------------------------------------
    // check if exist run objects
    // ------------------------------------------------------------------------
    if ( m_ro_que_ts.isEmpty() && ptr == nullptr ) { return false; }

    // ------------------------------------------------------------------------
    // got a thread object ( maybe create thread )
    // ------------------------------------------------------------------------
    if ( ! idle_map.empty() ) {
        thr = idle_map.begin()->first;
        idle_map.erase( idle_map.begin() );

    } else if ( idle_map.size() + work_map.size() < size_t( m_max_thr_cntr_ts.load( std::memory_order_seq_cst ))) {
        thr = qxpack_ic_new( ThrItem, & m_cmd_pipe_ts, & m_ro_que_ts );
        qxpack_ic_debug("IcThreadPool: create thread item 0x%x", thr );
    } else {
        thr = nullptr;
    }

    // ------------------------------------------------------------------------
    // try pick a run object to solve.
    // ------------------------------------------------------------------------
    if ( thr != nullptr ) {
        if ( ptr == nullptr ) {
            auto ro = m_ro_que_ts.deque();
            ptr = ro.obj; prior = ro.prior;
        }

        if ( ptr != nullptr ) {
            work_map.insert( ThrItemPair( thr, gMakeTimeStampFromNow(0)));
            thr->setRunObj( ptr );
            thr->wakeUp();
        } else { // no object to solve, put this thread to idle map.
            idle_map.insert( ThrItemPair( thr, gMakeTimeStampFromNow(0)));
            thr = nullptr;
        }
    } else { // no thread to solve this run object
        if ( ptr != nullptr ) { m_ro_que_ts.enque( RunObjQue::Item( ptr, prior ) ); }
    }

    // final check we has been solved a run object.
    return ( thr != nullptr );
}

// ============================================================================
//! \brief notify all wait for done  semaphore
// ============================================================================
void     IcThreadPoolWorker :: notifyAllWfd_thr( std::vector<void*> &vec )
{  
    auto c_itr = vec.cbegin();
    while ( c_itr != vec.cend()) {
        auto *ss = reinterpret_cast<SharedSem*>( *( c_itr ++ ));
        if ( ss != nullptr ) {
            ss->semObj()->notify();
            SharedSem::releaseRef( ss );
        }
    }
    if ( vec.capacity() > 4 ) {
        std::vector<void*> tmp; tmp.reserve(4);
        vec.swap( tmp );
    } else {
        vec.clear();
    }
}

// ============================================================================
//                      do the work ( running in thread )
//
// ============================================================================
void    IcThreadPoolWorker :: doWork_thr( )
{
    enum Stage { Stage_Working, Stage_Exiting, Stage_Finish }
    volatile stage = Stage_Working;
    std::vector<void*> wfd_vec; wfd_vec.reserve(4);

    while( stage != Stage::Stage_Finish ) {
        // --------------------------------------------------------------------
        // waiting for a new command
        // --------------------------------------------------------------------
        CmdItem cmd_item;
        if ( ! m_cmd_pipe_ts.waitForCmd( cmd_item ) ) { continue; }

        // --------------------------------------------------------------------
        // process command
        // --------------------------------------------------------------------
        switch( cmd_item.cmdId()) {
        case CmdItem::CmdID_ExitWorker : {
            // user must be not insert new run object into command queue
            // this will make exit while processed all runnable objects
            stage = Stage_Exiting;
        } break;

        case CmdItem::CmdID_NewRunObj  : {
            auto *ro = reinterpret_cast<IcRunnablePtr>( cmd_item.varA().toVoidStar());
            int   pr = cmd_item.varB().toInt();
            tryDispatchRunObj_thr( ro, pr );
        } break;

        case CmdItem::CmdID_IdleThread : {
            auto *thr = reinterpret_cast<ThrItem*>( cmd_item.varA().toVoidStar());
            if ( m_work_thr_map_thr.size() + m_idle_thr_map_thr.size() < size_t( m_max_thr_cntr_ts.load( std::memory_order_seq_cst ))) {
                auto ro   = m_ro_que_ts.deque();
                if ( ro.isValid()) {
                    thr->setRunObj( ro.obj );
                    thr->wakeUp();  // the thread already in working map, need not to change.
                } else {
                    auto itr = m_work_thr_map_thr.find( thr );
                    if ( itr != m_work_thr_map_thr.end()) { m_work_thr_map_thr.erase( itr ); }
                    m_idle_thr_map_thr.insert( ThrItemPair( thr, gMakeTimeStampFromNow(0)));
                    m_tmr_que_ts->singleShot (
                        m_expiry_tm_ts.load( std::memory_order_seq_cst ),
                        []( qxic_uid_t, const IcVariant &p1, const IcVariant &p2, const IcVariant &, const IcVariant &, bool is_valid ) {
                            if ( ! is_valid ) { return; }
                            auto *wkr = reinterpret_cast<IcThreadPoolWorker*>(p1.toVoidStar());
                            auto *thr = reinterpret_cast<ThrItemPtr>(p2.toVoidStar());
                            if ( wkr == nullptr || thr == nullptr ) { return; }
                            auto itr = wkr->m_idle_thr_map_thr.find( thr );
                            if ( itr != wkr->m_idle_thr_map_thr.end()) {
                                qxpack_ic_debug("IcThreadPool: delete idle thread 0x%x", thr );
                                wkr->m_idle_thr_map_thr.erase( itr );
                                qxpack_ic_delete( thr, ThrItem );
                            }
                        }, IcVariant( this ), IcVariant( thr )
                    );
                }
            } else {
                // over the current maximum thread's count
                auto itr = m_work_thr_map_thr.find( thr );
                if ( itr != m_work_thr_map_thr.end()) { m_work_thr_map_thr.erase( itr ); }
                qxpack_ic_debug("IcThreadPool: delete excess idle thread 0x%x", thr );
                qxpack_ic_delete( thr, ThrItem );
            }
        } break;

        case CmdItem::CmdID_ShrinkRunObj : {
            m_ro_que_ts.tryShrink();
            shedShrinkRunObjQue_ts();
        } break;

        case CmdItem::CmdID_WaitForDoneSem : {
            auto *sem = reinterpret_cast<SharedSem*>( cmd_item.varA().toVoidStar());
            if ( sem != nullptr ) { wfd_vec.push_back( sem ); }
        } break;

        default: break;
        }

        // --------------------------------------------------------------------
        // try solve the run object that in the queue
        // --------------------------------------------------------------------
        tryDispatchRunObj_thr( nullptr, 0 );

        // --------------------------------------------------------------------
        // check if all jobs done
        // --------------------------------------------------------------------
        bool is_ro_que_empty = m_ro_que_ts.isEmpty();
        bool is_wk_thr_empty = m_work_thr_map_thr.empty();

        if ( is_ro_que_empty && is_wk_thr_empty ) { notifyAllWfd_thr( wfd_vec ); }

        // --------------------------------------------------------------------
        // is there no working threads and stage is Exiting?
        // --------------------------------------------------------------------
        if (
            stage == Stage::Stage_Exiting &&  // want to exit
            is_wk_thr_empty  &&  is_ro_que_empty &&
            m_cmd_pipe_ts.isEmpty()           // no other commands
        ) { stage = Stage::Stage_Finish; }
    }

    // final cleanup
    notifyAllWfd_thr( wfd_vec );
    cleanupWorkThrItem_thr();
    cleanupIdleThrItem_thr();
}



// ////////////////////////////////////////////////////////////////////////////
//  wrap API
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcThreadPool :: IcThreadPool ( void * )
{
    m_obj = qxpack_ic_new( IcThreadPoolWorker );
}

// ===========================================================================
// dtor
// ===========================================================================
IcThreadPool :: ~IcThreadPool ( )
{
    waitForDone(-1);
    qxpack_ic_delete( m_obj, IcThreadPoolWorker );
}

// ============================================================================
// remove all waiting run objects
// ============================================================================
void    IcThreadPool :: clear( )
{
    auto roq = T_Worker( m_obj )->runObjQueRef();

    std::vector<void*> ro_vec;
    roq.clear(
        []( RunObjQue::Item &i, void *ctxt ) {
            auto *vec = reinterpret_cast<std::vector<void*>*>(ctxt);
            vec->push_back( i.obj );
        }, &ro_vec
    );

    auto c_itr = ro_vec.cbegin();
    while ( c_itr != ro_vec.cend() ) {
        auto *ro = reinterpret_cast<IcRunnable*>( *( c_itr ++ ));
        if ( ro != nullptr ) { IcRunnable::dispose( ro ); }
    }
}

// =============================================================================
// expiry time of current settings
// =============================================================================
int     IcThreadPool :: expiryTimeout() const
{ return T_Worker( m_obj )->expiryTime(); }

// ============================================================================
//  current maximum thread count
// ============================================================================
int     IcThreadPool :: maxThreadCount() const
{ return T_Worker( m_obj )->maxThrCnt(); }

// ============================================================================
// set expiry time out, the time should be greater than 100
// ============================================================================
void    IcThreadPool :: setExpiryTimeout( int tm )
{ T_Worker( m_obj )->setExpiryTime( tm ); }

// ============================================================================
// set the maximum thread count
// ============================================================================
void    IcThreadPool :: setMaxThreadCount( int cnt )
{ T_Worker( m_obj )->setMaxThrCnt( cnt ); }

// ============================================================================
// wait for all run objects are done
// ============================================================================
bool    IcThreadPool :: waitForDone( int msec )
{
    auto *sem = new SharedSem;
    if ( msec > 0 ) {
       T_Worker( m_obj )->postWfdCmd( *sem );
       auto tm_pt = gMakeTimeStampFromNow( msec );
       bool ret = sem->semObj()->waitUntil( tm_pt );
       SharedSem::releaseRef( sem );
       return ret;
    } else {
       T_Worker( m_obj )->postWfdCmd( *sem );
       sem->semObj()->wait();
       SharedSem::releaseRef( sem );
       return true;
    }
}

// ============================================================================
// start the runnable object
// ============================================================================
void    IcThreadPool :: start( IcRunnable *r, int prior )
{ if ( r != nullptr ) { T_Worker( m_obj )->postRunObj( r, prior ); } }


}
#endif
