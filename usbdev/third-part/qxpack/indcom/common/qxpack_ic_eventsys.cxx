#ifndef QXPACK_IC_EVENTSYS_CXX
#define QXPACK_IC_EVENTSYS_CXX

#include "qxpack_ic_eventsys.hxx"
#include "qxpack/indcom/common/qxpack_ic_def.h"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/common/qxpack_ic_logging.hxx"
#include "qxpack/indcom/common/qxpack_ic_semtemp.hpp"
#include "qxpack/indcom/common/qxpack_ic_queuetemp.hpp"
#include "qxpack/indcom/common/qxpack_ic_dyncinit_priv.hxx"
#include "qxpack/indcom/common/qxpack_ic_object.hxx"

#include <thread> // C++11
#include <map>    // C++11
#include <mutex>  // C++11
#include <atomic> // C++11
#include <queue>
#include <vector>

#define ICEVENTSYS_Default_CacheQueSize (64)
#define ICEVENTSYS_Default_DispTmrGap   (500)

namespace QxPack {

static volatile bool g_is_dync_dtor = false;

// ////////////////////////////////////////////////////////////////////////////
//
//                      Event Object
//
// ////////////////////////////////////////////////////////////////////////////
IcEvent :: IcEvent( unsigned short type, DelCallback cb, void *cb_ctxt )
{
    m_type = type; m_del_cb = cb; m_del_ctxt = cb_ctxt;
}

IcEvent :: ~IcEvent() {}

void   IcEvent :: dispose( IcEvent *evt )
{
    if ( evt != nullptr ) {
        if ( evt->m_posted != 0 ) {
            if ( evt->m_del_cb != nullptr ) {
               evt->m_del_cb( evt, evt->m_del_ctxt ); // delete it.
            } else {
               delete evt; // maybe crashed if new is located another runtime library!
            }
        }
    }
}




// ////////////////////////////////////////////////////////////////////////////
//
//                      Event Node Queue ( private )
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN  EvtNodeQue {
private:
    // the Node in the node queue
    class Node {
        typedef IcEvent*   EvtPtr;
        typedef IcObject*  ObjPtr;
    private: EvtPtr m_evt_ptr; ObjPtr m_obj_ptr; int m_prior;
    public :
        explicit Node( ) { m_evt_ptr = nullptr; m_obj_ptr = nullptr;; m_prior = 0; }
        Node( ObjPtr o, EvtPtr e, int prior = 0 ) { m_evt_ptr = e; m_obj_ptr = o; m_prior = prior; }
        inline IcObject*  object() const { return m_obj_ptr; }
        inline IcEvent*   evtPtr() const { return m_evt_ptr; }
        inline int        prior()  const { return m_prior;   }
    };

    // the compare of the node
    struct QXPACK_IC_HIDDEN  NodeCmp {
        bool operator() ( const Node& lhs, const Node &rhs ) const
        { return lhs.prior() < rhs.prior(); }
    };

    typedef std::priority_queue<Node,std::vector<Node>,NodeCmp> Que;
    Que m_que; std::mutex m_locker; size_t m_max_size, m_curr_size;

protected:
    void  lock()  { if ( ! g_is_dync_dtor ) { m_locker.lock();   }}
    void  unlock(){ if ( ! g_is_dync_dtor ) { m_locker.unlock(); }}

    void  tryShrink( )
    {
        // --------------------------------------------------------------------
        // this routine used for shrink memory of current queue.
        // we build a new queue to store these node.
        // --------------------------------------------------------------------
        if ( m_max_size >= 512 &&  m_curr_size < m_max_size / 2 ) {
            Que tmp; while ( m_que.empty()) {
                tmp.push( m_que.top() ); m_que.pop();
            }
            m_que.swap( tmp );
            m_curr_size = m_que.size();
            m_max_size  = m_curr_size;
        }
    }

public :
    explicit EvtNodeQue( ) : m_que( NodeCmp() ) { m_max_size = 0; m_curr_size = 0; }
    ~EvtNodeQue()
    {
        Que tmp;
        lock(); tmp.swap( m_que ); m_curr_size = m_max_size = 0; unlock();

        while ( ! tmp.empty()) {
            const Node &n = tmp.top();
            if ( n.evtPtr() != nullptr ) { IcEvent::dispose( n.evtPtr() ); }
            tmp.pop();
        }
    }

    inline bool    isEmpty() const { return ( m_curr_size < 1 ); }
    inline size_t  size() const    { return m_curr_size; }

    void  takeAndPush_ts( IcObject *o, IcEvent *e, int p = 0 )
    {
        lock();
        m_que.push( Node( o, e, p ) );
        ++ m_curr_size; if ( m_max_size < m_curr_size ) { m_max_size = m_curr_size; }
        unlock();
    }

    bool  tryTop_ts( IcObject **o, IcEvent **e, int *p, bool is_pop = true )
    {
        bool is_valid = true;
        lock();
        if ( ! m_que.empty() ) {
           const Node &n = m_que.top();
           if ( o != nullptr ) { *o = n.object(); }
           if ( e != nullptr ) { *e = n.evtPtr(); }
           if ( p != nullptr ) { *p = n.prior();  }
           if ( is_pop ) { m_que.pop(); -- m_curr_size; }
           tryShrink();
        } else { is_valid = false; }
        unlock();

        return is_valid;
    }

    void  rmvAllByObj_ts( IcObject* );
};



// ////////////////////////////////////////////////////////////////////////////
//
//                      Event Queue
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN  IcEventQueue {
private:
    std::thread::id  m_thr_id;
    EvtNodeQue       m_node_que;
public :
    explicit IcEventQueue( );
    ~IcEventQueue() { }
    inline std::thread::id &  tidRef() { return m_thr_id; }
    inline bool  hasPendingEvt() { return ! m_node_que.isEmpty(); }
    inline bool  tryDeque  ( IcObject **o, IcEvent **e, int *p ) { return m_node_que.tryTop_ts( o, e, p ); }
    inline void  takeAndEnq( IcObject *o, IcEvent *e, int p )    { m_node_que.takeAndPush_ts( o, e, p ); }

};

// ============================================================================
// ctor
// ============================================================================
IcEventQueue :: IcEventQueue( )
{
    m_thr_id = std::this_thread::get_id();
}




// ////////////////////////////////////////////////////////////////////////////
//
//                      Event Queue Global manager
//
// ////////////////////////////////////////////////////////////////////////////
typedef std::pair<int,IcEventQueue*>                EvtQueueMapItem;
typedef std::map<std::thread::id,EvtQueueMapItem >  EvtQueueMap;

class QXPACK_IC_HIDDEN  IcEventQueueGlobalMgr {
private:
     std::mutex   m_locker;
     EvtQueueMap  m_que_map;
public :
     explicit IcEventQueueGlobalMgr( );
     ~IcEventQueueGlobalMgr( );
     bool           isExistQue( const std::thread::id & );
     IcEventQueue*  getEvtQue ( const std::thread::id & );
     void           freeEvtQue( const std::thread::id & );
     static IcEventQueueGlobalMgr*  globalInst( );
     static void                    delGlobalInst( );
};

// ============================================================================
// ctor
// ============================================================================
IcEventQueueGlobalMgr :: IcEventQueueGlobalMgr( )
{  }

// ============================================================================
// dtor
// ============================================================================
IcEventQueueGlobalMgr :: ~IcEventQueueGlobalMgr( )
{
    EvtQueueMap::const_iterator c_itr = m_que_map.cbegin();
    while ( c_itr != m_que_map.cend() ) {
        if ( c_itr->second.second != nullptr ) {
            qxpack_ic_delete( c_itr->second.second, IcEventQueue );
        }
        ++ c_itr;
    }
}

// ============================================================================
// check if spec. thread's event queue is exist or not
// ============================================================================
bool     IcEventQueueGlobalMgr :: isExistQue( const std::thread::id &tid )
{
    bool is_found = false;
    m_locker.lock();
    EvtQueueMap::const_iterator c_itr = m_que_map.find( tid );
    is_found = ( c_itr != m_que_map.cend());
    m_locker.unlock();

    return is_found;
}

// ============================================================================
// return the event queue spec. by thread id
// ============================================================================
IcEventQueue*   IcEventQueueGlobalMgr :: getEvtQue( const std::thread::id &tid )
{
    IcEventQueue *evt = nullptr;
    m_locker.lock();
    EvtQueueMap::iterator c_itr = m_que_map.find( tid );
    if ( c_itr == m_que_map.cend()) {
        evt = qxpack_ic_new( IcEventQueue );
        evt->tidRef() = tid; // overwrite thread id
        m_que_map.insert( std::pair<std::thread::id,EvtQueueMapItem >(
            evt->tidRef(), EvtQueueMapItem( 1, evt )
        ));
    } else {
        volatile EvtQueueMapItem &item = c_itr->second;
        evt = item.second;
        ++ item.first;
    }
    m_locker.unlock();

    return evt;
}

// ============================================================================
// remove event queue spec. by thread id
// ============================================================================
void            IcEventQueueGlobalMgr :: freeEvtQue( const std::thread::id &tid )
{
    IcEventQueue *evt = nullptr;
    m_locker.lock();
    EvtQueueMap::iterator itr = m_que_map.find( tid );
    if ( itr != m_que_map.end()) {
        volatile EvtQueueMapItem &item = itr->second;
        if ( -- item.first <= 0 ) {
            evt = item.second;
            m_que_map.erase( itr );
        }
    }
    m_locker.unlock();

    if ( evt != nullptr ) {  qxpack_ic_delete( evt, IcEventQueue ); }
}

// ============================================================================
// [ static ] global instance of manager
// ============================================================================
static std::atomic<IcEventQueueGlobalMgr*>  g_evt_que_mgr(nullptr);
static std::mutex  g_evt_que_mgr_locker;
static IcDyncInit  g_evt_que_mgr_dync( nullptr, & IcEventQueueGlobalMgr::delGlobalInst );
IcEventQueueGlobalMgr*    IcEventQueueGlobalMgr :: globalInst()
{
    IcEventQueueGlobalMgr* mgr = nullptr;
    mgr = g_evt_que_mgr.load( std::memory_order_seq_cst );
    if ( mgr == nullptr ) {
        g_evt_que_mgr_locker.lock();
        mgr = g_evt_que_mgr.load( std::memory_order_seq_cst );
        if ( mgr == nullptr ) {
            mgr = qxpack_ic_new( IcEventQueueGlobalMgr );
            g_evt_que_mgr.store( mgr );
        }
        g_evt_que_mgr_locker.unlock();
    }

    return mgr;
}

// ============================================================================
// [ static ] delete global instance
// ============================================================================
void    IcEventQueueGlobalMgr :: delGlobalInst()
{
    // this ocurred at application end, need not to use locker!
    g_is_dync_dtor = true;
    IcEventQueueGlobalMgr *mgr = g_evt_que_mgr.load( std::memory_order_seq_cst );
    if ( mgr != nullptr ) { qxpack_ic_delete( mgr, IcEventQueueGlobalMgr ); }
    g_evt_que_mgr.store(nullptr);
}




// ////////////////////////////////////////////////////////////////////////////
//
//                      default Event Dispatcher
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN  IcEventDispatcher {
private:
    IcSemTemp     m_sem;
    IcEventQueue *m_evt_que;
public :
    explicit IcEventDispatcher( );
    ~IcEventDispatcher( );
    inline void  wakeUp( ) { m_sem.notify(); }
    void  procEvt( );
};

// ============================================================================
// ctor
// ============================================================================
IcEventDispatcher :: IcEventDispatcher ( )
{
    std::thread::id tid = std::this_thread::get_id();
    m_evt_que = IcEventQueueGlobalMgr::globalInst()->getEvtQue( tid );
}

// ============================================================================
// dtor
// ============================================================================
IcEventDispatcher :: ~IcEventDispatcher ( )
{
    std::thread::id tid = m_evt_que->tidRef();
    IcEventQueueGlobalMgr::globalInst()->freeEvtQue( tid );
}

// ============================================================================
// process the one event
// ============================================================================
void     IcEventDispatcher :: procEvt()
{
    if (   )
}







// ////////////////////////////////////////////////////////////////////////////
//
//                      Event Loop
//
// ////////////////////////////////////////////////////////////////////////////
#define T_PrivPtr( o )  T_ObjCast( IcEventLoopPriv*, o )
class QXPACK_IC_HIDDEN  IcEventLoopPriv {
private:
    IcEventQueue*
public :
    explicit IcEventLoopPriv( );
    ~IcEventLoopPriv( );

};

// ============================================================================
// ctor
// ============================================================================
IcEventLoopPriv :: IcEventLoopPriv ( )
   :
{
}

// ============================================================================
// dtor
// ============================================================================
IcEventLoopPriv :: ~IcEventLoopPriv ( )
{
}


// ////////////////////////////////////////////////////////////////////////////
//                      wrap API
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
//
// ============================================================================




// ////////////////////////////////////////////////////////////////////////////
//
//                      event object remover
//
// ////////////////////////////////////////////////////////////////////////////
namespace IcEvtSysSpc{
QXPACK_IC_HIDDEN void  rmvAllEventByObj( IcObject* )
{

}

}

}
#endif
