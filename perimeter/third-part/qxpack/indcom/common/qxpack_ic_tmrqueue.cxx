// ////////////////////////////////////////////////////////////////////////////
//! @brief the timer queue
//! @verbatim
//!      <author>         <date>          <memo.>
//!    night wing      20200910           add heap shrink strage
//! @endverbatim
//
// ////////////////////////////////////////////////////////////////////////////
#ifndef QXPACK_IC_TMRQUEUE_CXX
#define QXPACK_IC_TMRQUEUE_CXX

#include "qxpack_ic_tmrqueue.hxx"
#include "../common/qxpack_ic_memcntr.hxx"
#include "../common/qxpack_ic_heapvectortemp.hpp"
#include "../common/qxpack_ic_queuetemp.hpp"
#include "../common/qxpack_ic_semtemp.hpp"
#include "../common/qxpack_ic_global.hxx"
#include "../common/qxpack_ic_logging.hxx"

#include <mutex>
#include <atomic>
#include <chrono>
#include <thread>
#include <map>
#include <string>

namespace QxPack {

typedef std::chrono::steady_clock::time_point  IcTmrStamp; // to output the time point value, use

//std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
//std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end_s.time_since_epoch() ).count()

#define TMRQUEUE_ClearFreeNode_GapMS  (3000)

// ////////////////////////////////////////////////////////////////////////////
//
//                      global functions
//
// ////////////////////////////////////////////////////////////////////////////
static inline IcTmrStamp  gMakeTimeStampFromNow ( int mill )
{ return std::chrono::steady_clock::now() + std::chrono::milliseconds(mill); }

static void               gLockAtomFlag( std::atomic_flag &lck )
{
    IcLCG lgc;
    int busy_cnt = int( lgc.value() % 253 ) + 50;

    // --------------------------------------------------------------------
    // WARNING: by C++11 spec., test_and_set() return old value
    // HERE JUST loop while old value is true!
    // --------------------------------------------------------------------
    while ( lck.test_and_set( std::memory_order_seq_cst )) { // acquire lock
        if ( busy_cnt -- > 0 ) { continue; }
        std::this_thread::yield(); busy_cnt = int( lgc.value() % 253 ) + 50;
    }
}

// ////////////////////////////////////////////////////////////////////////////
//
//                      the timer descriptor item
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN  IcTmrItem {
public :
    enum Type {
        Type_Invalid = 0,   Type_SingleShot,
        Type_CancelById,    Type_CancelByRefLock,
        Type_ClearFreeNode, Type_EndWork
    };

private:  
    QxPack::IcVariant  m_cb_p1, m_cb_p2, m_cb_p3, m_cb_p4;   // user callback function context
    qxic_uid_t         m_id;               // the unique id of current item, this should be changed while setup()
    IcTmrStamp         m_end_tm;           // target time stamp
    std::atomic<Type>  m_type;
    void (*m_cb )( qxic_uid_t, const IcVariant&, const IcVariant&, const IcVariant&, const IcVariant&, bool );  // user callback function

    std::atomic_flag  *m_ref_lck;             // ref.locker, used before invoke user callback
    IcSemTemp         *m_call_sem;

protected:
    void  clear( );

public :
    explicit IcTmrItem( ) { clear(); }
    inline qxic_uid_t &   id()    { return m_id; }
    inline Type           type()  { return m_type.load( std::memory_order_seq_cst ); }
    inline void           setType( Type t ) { m_type.store( t, std::memory_order_seq_cst ); }

    inline IcTmrStamp &       endTime()        { return m_end_tm;  }
    inline std::atomic_flag*  refLock() const  { return m_ref_lck; }

    inline void  solveCallSem()
    { if ( m_call_sem != nullptr ){ m_call_sem->notify(); m_call_sem = nullptr; }}

    void   invoke( );

    inline void  setupCancelByRefLock( std::atomic_flag *ref_lck, IcSemTemp *sem )
    { m_id = IcUIDCntr::invalidUID(); m_type = Type_CancelByRefLock; m_ref_lck = ref_lck; m_call_sem = sem; }

    inline void  setupCancelById ( qxic_uid_t id, IcSemTemp *sem )
    { m_id = id; m_type = Type_CancelById; m_call_sem = sem; }

    inline void  setupClearFreeNode( const IcTmrStamp &tm_st ) { m_id = IcUIDCntr::invalidUID(); m_type = Type_ClearFreeNode; m_end_tm = tm_st; }

    inline void  setupEndWork( ) { m_id = IcUIDCntr::invalidUID(); m_type = Type_EndWork; }

    void   setupSingleShot (
       qxic_uid_t, const IcTmrStamp &,
       void (* cb )(qxic_uid_t,const IcVariant&, const IcVariant&, const IcVariant&, const IcVariant&, bool),
       const IcVariant&, const IcVariant&, const IcVariant&, const IcVariant&,
       std::atomic_flag *ref_lck
    );
};

// ============================================================================
// [ protected ]  clear all content
// ============================================================================
void                    IcTmrItem :: clear()
{
    m_type    = Type_Invalid;
    m_cb      = nullptr;  m_cb_p1 = IcVariant();  m_cb_p2 = IcVariant();
    m_ref_lck = nullptr;  m_id    = IcUIDCntr::invalidUID();
    m_call_sem = nullptr;
}

// ============================================================================
// [ public    ]  setup the information
// ============================================================================
void                    IcTmrItem :: setupSingleShot(
    qxic_uid_t id, const IcTmrStamp &end_tm,
    void (* cb )(qxic_uid_t,const IcVariant&, const IcVariant&, const IcVariant&, const IcVariant&, bool),
    const IcVariant &cb_p1, const IcVariant &cb_p2, const IcVariant &cb_p3, const IcVariant &cb_p4,
    std::atomic_flag *ref_lck
) {
    m_type     = Type_SingleShot;   m_end_tm   = end_tm;
    m_cb       = cb;
    m_cb_p1  = cb_p1;   m_cb_p2 = cb_p2;  m_cb_p3 = cb_p3; m_cb_p4 = cb_p4;
    m_ref_lck  = ref_lck;  m_id = id;
    m_call_sem = nullptr;
}

// ============================================================================
// [ public    ]  invoke the callback that user setup
// ============================================================================
void                    IcTmrItem :: invoke()
{
    if ( m_type.load( std::memory_order_seq_cst ) == Type_SingleShot ) {
        bool is_set_ok = ! m_ref_lck->test_and_set();
        if ( m_cb != nullptr ) { m_cb( m_id, m_cb_p1, m_cb_p2, m_cb_p3, m_cb_p4, is_set_ok ); }
        if ( is_set_ok ) { m_ref_lck->clear();}
    } else {
        if ( m_cb != nullptr ) { m_cb( m_id, m_cb_p1, m_cb_p2, m_cb_p3, m_cb_p4, false ); }
    }

    clear();
}


// /////////////////////////////////////////////////////////////////////////////
//
//                      timer item comparator
//
// /////////////////////////////////////////////////////////////////////////////
typedef IcNodeQueueTemp< IcTmrItem >                 IcTmrItemNodeQueue;
typedef IcNodeQueueTemp< IcTmrItem >::Node           IcTmrItemNode;
typedef IcTmrItemNode*                               IcTmrItemNodePtr;

class QXPACK_IC_HIDDEN  IcTmrItemCmp {
public:
    inline bool  operator()( const IcTmrItemNodePtr &a, const IcTmrItemNodePtr &b ) const
    { return a->dataRef().endTime() < b->dataRef().endTime(); }
};

typedef IcHeapVector< IcTmrItemNodePtr, IcTmrItemCmp >  IcTmrItemHeap;

// ////////////////////////////////////////////////////////////////////////////
//
// private implement of timer queue
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN  IcTmrQueueWorker {
private:
    IcSemTemp           m_work_sem;        // the semaphore that used for wake up worker thread
    IcTmrItemHeap       m_work_nodes;      // the timer priority heap
    IcTmrItemNodeQueue  m_free_nodes_ts;   // current freed timer item
    IcTmrItemNodeQueue  m_post_nodes_ts;   // this queue used for store timer item that not solved

    IcUIDCntr         m_uid_cntr;
    std::thread       m_wkr_thr;

protected:    
    // ------------------------------------------------------------------------
    // timer item node manager
    // ------------------------------------------------------------------------
    inline IcTmrItemNode*   newTmrItemNode_ts() {
        IcTmrItemNode *n = m_free_nodes_ts.dequeue();
        n = ( n != nullptr ? n : qxpack_ic_new( IcTmrItemNode ));
        n->dataRef().endTime() = gMakeTimeStampFromNow(0);
        return n;
    }
    inline void             recycleTmrItemNode_ts( IcTmrItemNode *node )
    {  node->dataRef().setType( IcTmrItem::Type_Invalid ); m_free_nodes_ts.takeAndEnqueue( node ); }

    // ------------------------------------------------------------------------
    // functions
    // ------------------------------------------------------------------------
    inline void   deleteAllFreeNode_thr() { m_free_nodes_ts.clear(); }
    inline void   deleteAllPostNode_thr() { m_post_nodes_ts.clear(); }
           void   deleteAllWorkNode_thr();
    inline void   addWorkNode_thr ( IcTmrItemNode *n ) { m_work_nodes.insert( n ); }
           void   cancelWorkNodeById_thr ( IcTmrItemNode* );
           void   cancelWorkNodeByRefLock_thr ( IcTmrItemNode* );
           bool   procPostItem_thr();
           void   doWork_thr( IcTmrStamp &curr );
           void   run_thr();
           void   shrinkWorkNodeHeap_thr(); // nw: 2020/09/10
public :
    explicit IcTmrQueueWorker();
    ~IcTmrQueueWorker();
    qxic_uid_t   singleShot_ts(
        int ms, void (* cb )( qxic_uid_t tm_id, const IcVariant&, const IcVariant &, const IcVariant&, const IcVariant&, bool is_valid ),
        const IcVariant &, const IcVariant&, const IcVariant&, const IcVariant&,
        std::atomic_flag *ref_ctxt
    );
    void         cancelTmrItemByRef_ts( std::atomic_flag *ref_lck );
    void         cancelTmrItemById_ts ( qxic_uid_t t_id );
};

// ============================================================================
// ctor
// ============================================================================
IcTmrQueueWorker :: IcTmrQueueWorker( ) :
    m_work_nodes( nullptr, nullptr, 16, false ),
    m_free_nodes_ts( []( IcTmrItemNode *n, void* ){ qxpack_ic_delete( n, IcTmrItemNode ); }, nullptr ),
    m_post_nodes_ts( []( IcTmrItemNode *n, void* ){
        n->dataRef().setType( IcTmrItem::Type_Invalid );
        n->dataRef().invoke();
        qxpack_ic_delete( n, IcTmrItemNode );
    }, nullptr )
{
    m_wkr_thr = std::thread( [this](){ this->run_thr(); });
}

// ============================================================================
// dtor
// ============================================================================
IcTmrQueueWorker :: ~IcTmrQueueWorker ( )
{
    IcTmrItemNode *node = newTmrItemNode_ts();
    node->dataRef().setupEndWork();
    m_post_nodes_ts.takeAndEnqueue( node );
    m_work_sem.notify();
    m_wkr_thr.join();
}

// ============================================================================
// [ protected ] clear all work item in the heap
// ============================================================================
void                    IcTmrQueueWorker :: deleteAllWorkNode_thr( )
{
    m_work_nodes.clear (
        []( IcTmrItemNodePtr &node, void* ) {
            node->dataRef().setType( IcTmrItem::Type_Invalid );
            node->dataRef().invoke();
            qxpack_ic_delete( node, IcTmrItemNode );
        }, nullptr
    );
}

// ============================================================================
// [ protected ] mark work node that id is same with n->dataRef().id()
// ============================================================================
void                    IcTmrQueueWorker :: cancelWorkNodeById_thr( IcTmrItemNode *node )
{
    // ------------------------------------------------------------------------
    // here just mark the target node invalid, the callback will be done by deleteAllWorkNode_thr()
    // ------------------------------------------------------------------------
    m_work_nodes.searh (
        []( IcTmrItemNodePtr &curr_n, void *tgt_n ){
            IcTmrItemNode *tgt_node = reinterpret_cast< IcTmrItemNode* >( tgt_n );
            if ( curr_n->dataRef().id() == tgt_node->dataRef().id()) {
                curr_n->dataRef().setType( IcTmrItem::Type_Invalid );
                return true;   // found, break searching...
            } else {
                return false;
            }
        }, node
    );
    node->dataRef().solveCallSem();
    recycleTmrItemNode_ts( node );
}

// ============================================================================
// [ protected ] makr all work item that ref lock equal to n->dataRef().refLock()
// ============================================================================
void                    IcTmrQueueWorker :: cancelWorkNodeByRefLock_thr( IcTmrItemNode *node )
{
    // ------------------------------------------------------------------------
    // here just mark the target node invalid, the callback will be done by deleteAllWorkNode_thr()
    // ------------------------------------------------------------------------
    m_work_nodes.enumAll(
        []( IcTmrItemNodePtr &curr_n, void *tgt_n ) {
            IcTmrItemNode *tgt_node = reinterpret_cast< IcTmrItemNode *>( tgt_n );
            if ( curr_n->dataRef().refLock() == tgt_node->dataRef().refLock()) {
                curr_n->dataRef().setType( IcTmrItem::Type_Invalid );
            }
        }, node
    );
    node->dataRef().solveCallSem();
    recycleTmrItemNode_ts( node );
}

// ============================================================================
// [ protected ] process all posted item
// ============================================================================
bool                    IcTmrQueueWorker :: procPostItem_thr()
{
    IcTmrItemNode *node = nullptr;  int max_item = 64; bool is_cont_work = true;
    while ( max_item -- > 0  &&  ( node = m_post_nodes_ts.dequeue() ) != nullptr ) {
        switch( node->dataRef().type()) {
        case IcTmrItem::Type_SingleShot : addWorkNode_thr( node ); break;
        case IcTmrItem::Type_CancelById : cancelWorkNodeById_thr( node ); break;
        case IcTmrItem::Type_CancelByRefLock : cancelWorkNodeByRefLock_thr( node ); break;
        case IcTmrItem::Type_EndWork : is_cont_work = false; recycleTmrItemNode_ts( node ); break;
        default :
            node->dataRef().solveCallSem();
            recycleTmrItemNode_ts( node );
            break;
        }
    }

    return is_cont_work;
}

// ============================================================================
// [ protected ] do the work
// ============================================================================
void                    IcTmrQueueWorker :: doWork_thr( IcTmrStamp &curr_st )
{
    IcTmrItemNode *node = nullptr; volatile bool is_cont = true;

    while ( is_cont  &&  m_work_nodes.removeAt( 0, node ) ) {

        switch( node->dataRef().type()) {
        case IcTmrItem::Type_Invalid    : { // this item is invalid, just recycle it
            node->dataRef().invoke();
            recycleTmrItemNode_ts( node );
        } break;

        case IcTmrItem::Type_SingleShot : { // do calling
            if ( node->dataRef().endTime() <= curr_st ) {
                node->dataRef().invoke();
                recycleTmrItemNode_ts( node );
            } else {
                addWorkNode_thr( node );    // put it back
                is_cont = false;
            }
        } break;

        case IcTmrItem::Type_ClearFreeNode : {
            if ( node->dataRef().endTime() <= curr_st ) {
                IcTmrItemNode *free_node = m_free_nodes_ts.dequeue();
                if ( free_node != nullptr ) { qxpack_ic_delete( free_node, IcTmrItemNode ); }
                node->dataRef().setupClearFreeNode( gMakeTimeStampFromNow( TMRQUEUE_ClearFreeNode_GapMS ));
                m_work_nodes.insert( node );
                shrinkWorkNodeHeap_thr();
            } else {
                addWorkNode_thr( node );
                is_cont = false;
            }
        } break;

        default: // unknown type
            node->dataRef().setType( IcTmrItem::Type_Invalid );
            node->dataRef().invoke();
            recycleTmrItemNode_ts( node );
        }
    }
}

// ============================================================================
// [ protected ] thread run
// ============================================================================
void                    IcTmrQueueWorker :: run_thr()
{
    { // make a clear type node
        IcTmrItemNode *node = newTmrItemNode_ts();
        node->dataRef().setupClearFreeNode( gMakeTimeStampFromNow( TMRQUEUE_ClearFreeNode_GapMS ));
        m_work_nodes.insert( node );
    }
    volatile bool is_cont_work = true; IcTmrStamp  curr_st;// IcLCG yield_lcg; int yield_cntr = yield_lcg.value() % 253 + 50;

    while ( is_cont_work ) {
        is_cont_work = procPostItem_thr();

        IcTmrStamp  top_stamp;
        bool ret = m_work_nodes.visit (
            0, []( IcTmrItemNodePtr &node, void*ctxt ) {
                * T_ObjCast( IcTmrStamp*, ctxt ) = node->dataRef().endTime();
            }, &top_stamp
        );

        curr_st = gMakeTimeStampFromNow(0);

        if ( ret ) {  // got valid time stamp
            if ( top_stamp > curr_st ) { // target stamp is not passed
                    ret = m_work_sem.waitUntil( top_stamp );
                    if ( ret ) {
                        // sem waked by condition
                        is_cont_work = procPostItem_thr();
                        doWork_thr( curr_st );
                    } else {
                        // sem waked by timeout
                        doWork_thr( curr_st );
                        is_cont_work = procPostItem_thr();
                    }
            } else {
                // target time was passed
                doWork_thr( curr_st );
                is_cont_work = procPostItem_thr();
            }
        } else { // got none time stamp
            m_work_sem.waitUntil( gMakeTimeStampFromNow(64));
            is_cont_work = procPostItem_thr();
        }
    }

    deleteAllWorkNode_thr();
    deleteAllPostNode_thr();
    deleteAllFreeNode_thr();
}

// ============================================================================
// [ protected ] shrink queue
// ============================================================================
void                     IcTmrQueueWorker :: shrinkWorkNodeHeap_thr()
{
    m_work_nodes.shrinkIf (
        []( const std::vector<IcTmrItemNodePtr> &vec, void* )->int {
            const size_t nn = 64;
            size_t curr_sz  = vec.size();
            size_t cap_sz   = vec.capacity();
            size_t round_sz = ( curr_sz + nn - 1 ) / nn * nn;
            return int(( cap_sz > nn  &&  round_sz * 2 + nn < cap_sz ) ?
                    cap_sz / 2 : uint(-1));
        }, this
    );
}

// ============================================================================
// [ public    ]  add a new single shot object
// ============================================================================
qxic_uid_t               IcTmrQueueWorker :: singleShot_ts (
    int ms, void (* cb )( qxic_uid_t tm_id, const IcVariant&, const IcVariant&, const IcVariant&, const IcVariant&, bool is_valid ),
    const IcVariant &cb_p1, const IcVariant &cb_p2, const IcVariant &cb_p3, const IcVariant &cb_p4, std::atomic_flag *ref_lck
) {   
    IcTmrItemNode *node = newTmrItemNode_ts();
    node->dataRef().setupSingleShot( m_uid_cntr.fetch(), gMakeTimeStampFromNow( ms ), cb, cb_p1, cb_p2, cb_p3, cb_p4, ref_lck );
    qxic_uid_t  t_id = node->dataRef().id();
    m_post_nodes_ts.takeAndEnqueue( node );
    m_work_sem.notify();
    return t_id;
}

// ============================================================================
// [ public ] make invalid timer item by context
// ============================================================================
void                    IcTmrQueueWorker :: cancelTmrItemByRef_ts ( std::atomic_flag *ref_lck )
{
    IcTmrItemNode *node = newTmrItemNode_ts();
    IcSemTemp  call_sem;
    node->dataRef().setupCancelByRefLock( ref_lck, &call_sem );
    m_post_nodes_ts.takeAndEnqueue( node );
    m_work_sem.notify();
    call_sem.wait();
}

// ============================================================================
// [ public ] make invalid timer item by timer id
// ============================================================================
void                    IcTmrQueueWorker :: cancelTmrItemById_ts( qxic_uid_t t_id )
{
    IcTmrItemNode *node = newTmrItemNode_ts();
    IcSemTemp  call_sem;
    node->dataRef().setupCancelById( t_id, &call_sem );
    m_post_nodes_ts.takeAndEnqueue( node );
    m_work_sem.notify();
    call_sem.wait();
}




// ////////////////////////////////////////////////////////////////////////////
//
//  timer queue private node, used in a map
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN  IcTmrQueueWorkerNode {
private:
    std::atomic<int> m_ref_cntr; IcTmrQueueWorker  m_tmq;
public :
    IcTmrQueueWorkerNode ( ) : m_ref_cntr(1) { }
    ~IcTmrQueueWorkerNode( ) { }
    inline int  incrRefCntr( ) { return m_ref_cntr.fetch_add(1) + 1; }
    inline int  decrRefCntr( ) { return m_ref_cntr.fetch_sub(1) - 1; }
    IcTmrQueueWorker &  tmq( ) { return m_tmq; }
};




// ////////////////////////////////////////////////////////////////////////////
//
//                      global instance manager
//
// ////////////////////////////////////////////////////////////////////////////
typedef std::map< std::string, IcTmrQueueWorkerNode* >  IcTmrQueueWorkerMap;
static std::mutex            g_tmq_map_locker;
static IcTmrQueueWorkerMap   g_tmq_map;

// =================================================================
// [ static ] get the named timer queue by name
// =================================================================
static IcTmrQueueWorkerNode *   gGetInstance( const std::string &name )
{
    IcTmrQueueWorkerNode *node = nullptr;
    g_tmq_map_locker.lock();

    IcTmrQueueWorkerMap::const_iterator itr = g_tmq_map.find( name );
    if ( itr != g_tmq_map.cend()) {
        node = itr->second; node->incrRefCntr();
    } else {
        node = qxpack_ic_new( IcTmrQueueWorkerNode );
        g_tmq_map.insert( std::pair<std::string,IcTmrQueueWorkerNode*>( name, node ));
    }

    g_tmq_map_locker.unlock();
    return  node;
}

// ================================================================
// [ static ]  free the named instance
// ================================================================
static bool             gFreeInstance( const std::string &name )
{
    bool is_delete = false;
    g_tmq_map_locker.lock();

    IcTmrQueueWorkerMap::const_iterator itr = g_tmq_map.find( name );
    if ( itr != g_tmq_map.cend() ) {
        if ( ( itr->second )->decrRefCntr() == 0 ) {
            qxpack_ic_delete( itr->second, IcTmrQueueWorkerNode );
            g_tmq_map.erase( itr );
            is_delete = true;
        }
    }

    g_tmq_map_locker.unlock();
    return is_delete;
}



// ////////////////////////////////////////////////////////////////////////////
//
//                      private of the timer
//
// ////////////////////////////////////////////////////////////////////////////
#define T_PrivPtr( o )  T_ObjCast( IcTmrQueuePriv*, o )
class QXPACK_IC_HIDDEN  IcTmrQueuePriv {
private:
    IcTmrQueueWorkerNode *m_queue_node;
    std::atomic_flag      m_cb_lock;
    std::string           m_queue_name;
protected:
    inline void  lockCallback() { gLockAtomFlag( m_cb_lock ); }
    inline void  releaseCb()    { m_cb_lock.clear(); }
public :
    explicit IcTmrQueuePriv ( const char *queue_name );
    ~IcTmrQueuePriv();
    inline IcTmrQueueWorker& tmq()    { return m_queue_node->tmq(); }
    inline std::atomic_flag& cbLock() { return m_cb_lock; }
};

static const char *g_def_tmr_name = "QxPack::IcTmrQueue::_default_";

// ============================================================================
// ctor
// ============================================================================
IcTmrQueuePriv :: IcTmrQueuePriv ( const char *queue_name )
{
    m_cb_lock.clear();
    m_queue_node = gGetInstance( m_queue_name = std::string( ( queue_name == nullptr ? g_def_tmr_name : queue_name ) ));
}

// ============================================================================
// dtor
// ============================================================================
IcTmrQueuePriv :: ~IcTmrQueuePriv ( )
{
    lockCallback();
    m_queue_node->tmq().cancelTmrItemByRef_ts( & m_cb_lock );
    gFreeInstance( m_queue_name );
}




// ////////////////////////////////////////////////////////////////////////////
//
//                      timer queue  wrap API
//
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcTmrQueue :: IcTmrQueue ( const char *queue_name )
{
    m_obj = qxpack_ic_new( IcTmrQueuePriv, queue_name );
}

// ============================================================================
// dtor
// ============================================================================
IcTmrQueue :: ~IcTmrQueue ( )
{
    if ( m_obj != nullptr ) {
        qxpack_ic_delete( m_obj, IcTmrQueuePriv );
    }
}

// ============================================================================
// do a single shot call
// ============================================================================
qxic_uid_t              IcTmrQueue :: singleShot (
    int ms,
    void (*cb)( qxic_uid_t, const IcVariant &, const IcVariant&, const IcVariant&, const IcVariant&, bool ),
    const IcVariant &cb_p1, const IcVariant &cb_p2, const IcVariant &cb_p3, const IcVariant &cb_p4
) {
    if ( ms < 0 ) { ms = 0; }
    if ( cb == nullptr ) { return 0; }
    return T_PrivPtr( m_obj )->tmq().singleShot_ts( ms, cb, cb_p1, cb_p2, cb_p3, cb_p4, & T_PrivPtr( m_obj )->cbLock() );
}

// ============================================================================
// invalid the single shot call
// ============================================================================
void                    IcTmrQueue :: invalidSingleShot( qxic_uid_t t_id )
{
    if ( t_id != IcUIDCntr::invalidUID() ) {
        T_PrivPtr( m_obj )->tmq().cancelTmrItemById_ts( t_id );
    }
}


// ////////////////////////////////////////////////////////////////////////////
//
//                 name list for global
//
// ////////////////////////////////////////////////////////////////////////////
const char *   IcTmrQueue :: disposeObjCbQueName() { return "QxPack::IcTmrQueue::_dispose_obj_"; }

}

#endif
