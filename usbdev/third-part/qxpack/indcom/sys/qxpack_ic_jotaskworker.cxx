// ////////////////////////////////////////////////////////////////////////////
// @author nightwing
// @history
//        2019/05/25 re-designed
// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
// @author nightwing
// @history
//        2019/04/27  checked.
// ////////////////////////////////////////////////////////////////////////////
#ifndef QXPACK_IC_JOTASKWORKER_CXX
#define QXPACK_IC_JOTASKWORKER_CXX

#include "qxpack_ic_jotaskworker.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/common/qxpack_ic_queuetemp.hpp"
#include "qxpack/indcom/sys/qxpack_ic_sharedthreadpool_priv.hxx"
#include "qxpack/indcom/common/qxpack_ic_global.hxx"
#include "qxpack/indcom/common/qxpack_ic_pimplprivtemp.hpp"

#include <QThread>
#include <QThreadPool>
#include <QElapsedTimer>
#include <QDebug>
#include <QtGlobal>
#include <QSharedPointer>
#include <QSemaphore>
#include <QMutex>
#include <QList>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
//                        worker item
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN  IcJoTaskWorker_Item {
private:
    QJsonObject  m_jo;  IcVarData m_ext_param;
    IcJoTaskWorker::TaskHandler m_task_handler; void *m_task_ctxt;
    QSharedPointer<QSemaphore>  m_wait_sem;
    bool m_is_need_drop, m_req_notify_start,  m_req_notify_delete;
public :
    explicit IcJoTaskWorker_Item ( );
    ~IcJoTaskWorker_Item( );
    void  markDrop( ) { m_is_need_drop = true; }
    bool  init(
       IcJoTaskWorker::TaskHandler task_handler, void *task_ctxt,
       const QJsonObject &jo, const IcVarData &ext_param,
       const QSharedPointer<QSemaphore> &sem, bool req_notify_start, bool req_notify_delete
    );
    void  handle( );
};

// ============================================================================
// ctor
// ============================================================================
IcJoTaskWorker_Item :: IcJoTaskWorker_Item ( )
{
    m_is_need_drop = false;
    m_req_notify_start = m_req_notify_delete = false;
    m_task_ctxt = nullptr;
}

// ============================================================================
// initailize the item
// ============================================================================
bool  IcJoTaskWorker_Item :: init (
    IcJoTaskWorker::TaskHandler task_handler, void *task_ctxt,
    const QJsonObject &jo, const IcVarData &ext_param,
    const QSharedPointer<QSemaphore> &sem, bool req_notify_start, bool req_notify_delete
) {
    m_task_handler = task_handler; m_task_ctxt = task_ctxt;
    m_jo = jo;                     m_ext_param = ext_param;
    m_wait_sem = sem;
    m_req_notify_start  = req_notify_start;
    m_req_notify_delete = req_notify_delete;

    return true;
}

// ============================================================================
// dtor
// ============================================================================
IcJoTaskWorker_Item :: ~IcJoTaskWorker_Item ( )
{
    if ( m_req_notify_delete && m_wait_sem.data() != Q_NULLPTR ) {
        m_wait_sem->release();
    }
}

// ============================================================================
// handle the task
// ============================================================================
void  IcJoTaskWorker_Item :: handle ( )
{
    if ( m_req_notify_start) { m_wait_sem->release(); }
    if ( m_task_handler != Q_NULLPTR ) {
        m_task_handler( m_task_ctxt, m_jo, m_ext_param,
           ( ! m_is_need_drop ? IcJoTaskWorker::HandlerType_Routine : IcJoTaskWorker::HandlerType_Drop )
        );
    }
}


// ////////////////////////////////////////////////////////////////////////////
//            task item queue
// ////////////////////////////////////////////////////////////////////////////
typedef IcNodeQueueTemp<IcJoTaskWorker_Item>  IcJoTaskWorker_ItemQueue;

// ////////////////////////////////////////////////////////////////////////////
//
//                         runnable item
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN  IcJoTaskWorker_RunObj : public QRunnable {
private:
    QSharedPointer<IcJoTaskWorker_ItemQueue>  m_task_queue_ts;
    QSharedPointer<QAtomicInt>  m_task_cntr;
public :
    explicit IcJoTaskWorker_RunObj (
        QSharedPointer<IcJoTaskWorker_ItemQueue> &q, QSharedPointer<QAtomicInt> &cntr
    ) {
        qxpack_ic_incr_new_cntr( IcJoTaskWorker_RunObj );
        m_task_queue_ts = q; m_task_cntr = cntr;
    }

    virtual ~IcJoTaskWorker_RunObj( ) Q_DECL_OVERRIDE
    {
        qxpack_ic_decr_new_cntr( IcJoTaskWorker_RunObj );
    }

    virtual  void  run( ) Q_DECL_OVERRIDE
    {
        if ( m_task_queue_ts.data() != Q_NULLPTR ) {
            IcJoTaskWorker_ItemQueue::Node *node = m_task_queue_ts->dequeue();
            if ( node != Q_NULLPTR ) {
                node->dataRef().handle();
                qxpack_ic_delete( node, IcJoTaskWorker_ItemQueue::Node );
                m_task_cntr->fetchAndSubOrdered(1);
            }
        }
    }
};




// ////////////////////////////////////////////////////////////////////////////
//
//                      private object
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN  IcJoTaskWorkerPriv : public QObject {
    Q_OBJECT
private:
    QSharedPointer<IcJoTaskWorker_ItemQueue>  m_task_queue_ts; // nw: IcNodeQueueTemp is thread-safe
    QSharedPointer<QAtomicInt>   m_task_cntr;
    QThreadPool             *m_thread_pool;
    IcSharedThreadPoolPriv  *m_shared_thread_pool;
    bool    m_is_shutdown_stage;
protected:
    bool    addTaskNode( IcJoTaskWorker_ItemQueue::Node *n );
public :
    explicit IcJoTaskWorkerPriv ( int max_t_num, QObject *pa = Q_NULLPTR );
    virtual ~IcJoTaskWorkerPriv ( ) Q_DECL_OVERRIDE;

    void    shutdown( );
    bool    waitForAllDone( int ms = -1 );
    bool    post (
        IcJoTaskWorker::TaskHandler task_handler, void *task_ctxt,
        const QJsonObject &jo,  const IcVarData &ext_param
    );
    bool    send (
        IcJoTaskWorker::TaskHandler task_handler, void *task_ctxt,
        const QJsonObject &jo,  const IcVarData &ext_param, bool req_wait_end
    );
};

// ============================================================================
// ctor
// ============================================================================
IcJoTaskWorkerPriv :: IcJoTaskWorkerPriv ( int max_t_num, QObject *pa )
  : QObject( pa ),
    m_task_queue_ts (
        new IcJoTaskWorker_ItemQueue (
            []( IcJoTaskWorker_ItemQueue::Node *n, void* ) {
                qxpack_ic_delete( n, IcJoTaskWorker_ItemQueue::Node );
            }, nullptr
        ),
        []( IcJoTaskWorker_ItemQueue *q ){ delete q; }
    ),
    m_task_cntr (
        new QAtomicInt(0),[](QAtomicInt *c ) { delete c; }
    )
{
    // ------------------------------------------------------------------------
    // initialize the member
    // ------------------------------------------------------------------------
    m_is_shutdown_stage = false;
    m_thread_pool = Q_NULLPTR;
    m_shared_thread_pool = Q_NULLPTR;

    // ------------------------------------------------------------------------
    // initialize the thread pool
    // ------------------------------------------------------------------------
    if ( max_t_num >= 0 ) {
        m_thread_pool = qxpack_ic_new_qobj( QThreadPool );
        m_thread_pool->setMaxThreadCount( ( max_t_num < 1 ? 1 : max_t_num ));
        m_thread_pool->setExpiryTimeout ( 30000 );
    } else {
        m_shared_thread_pool = qxpack_ic_new( IcSharedThreadPoolPriv );
    }
}

// ============================================================================
// dtor
// ============================================================================
IcJoTaskWorkerPriv :: ~IcJoTaskWorkerPriv ( )
{
    if ( m_thread_pool != Q_NULLPTR ) { m_thread_pool->deleteLater(); }
    if ( m_shared_thread_pool != Q_NULLPTR ) {
        qxpack_ic_delete( m_shared_thread_pool, IcSharedThreadPoolPriv );
    }
}

// ============================================================================
// shutdown, once setup it, any post and send function will be failed
// ============================================================================
void   IcJoTaskWorkerPriv :: shutdown ( )
{
   if ( m_is_shutdown_stage ) { return; }
    m_is_shutdown_stage = true;

    // ------------------------------------------------------------------------
    // grab all remain nodes into tmp list
    // ------------------------------------------------------------------------
    QList<IcJoTaskWorker_ItemQueue::Node*> tmp;
    m_task_queue_ts->blockDequeueUntil (
        []( IcJoTaskWorker_ItemQueue::Node* n, void *ctxt ) {
            QList<IcJoTaskWorker_ItemQueue::Node*> *tmp_list =
                reinterpret_cast< QList<IcJoTaskWorker_ItemQueue::Node*>* >( ctxt );
            tmp_list->append( n );
        }, &tmp,
        []( IcJoTaskWorker_ItemQueue::Node*, void*) { return true; }, this
    );

    // ------------------------------------------------------------------------
    // drop all node
    // ------------------------------------------------------------------------
    QList<IcJoTaskWorker_ItemQueue::Node*>::const_iterator c_itr = tmp.constBegin();
    while ( c_itr != tmp.constEnd()) {
        IcJoTaskWorker_ItemQueue::Node *node = (*c_itr ++ );
        node->dataRef().markDrop();
        node->data().handle();
        qxpack_ic_delete( node, IcJoTaskWorker_ItemQueue::Node );
        m_task_cntr->fetchAndSubOrdered(1);
    }
}

// ============================================================================
// wait for all item done
// ============================================================================
bool  IcJoTaskWorkerPriv :: waitForAllDone( int ms )
{
    bool is_done = false; IcLCG lcg;
    int loop_tm = lcg.value() % 100 + 50;

    if ( ms != -1 ) { // wait until spec. times
        QElapsedTimer cntr_tmr; cntr_tmr.start();
        while ( !( is_done = ( m_task_cntr->loadAcquire() == 0 ))) {
            if ( cntr_tmr.hasExpired( ms )) { break; }
            if ( loop_tm -- < 1 ) {
                loop_tm = lcg.value() % 100 + 50;
                QThread::yieldCurrentThread();
            }
        }

    } else {        // infinite waiting
        while ( !( is_done = ( m_task_cntr->loadAcquire() == 0 ))) {
            if ( loop_tm -- < 1 ) {
                loop_tm = lcg.value() % 100 + 50;
                QThread::yieldCurrentThread();
            }
        }
    }

    return is_done;
}

// ============================================================================
// add task node,
// ============================================================================
bool   IcJoTaskWorkerPriv :: addTaskNode( IcJoTaskWorker_ItemQueue::Node *node )
{
    QThreadPool *pool = nullptr;

    if ( m_thread_pool != Q_NULLPTR ) {
        pool = m_thread_pool;
    } else if ( m_shared_thread_pool != Q_NULLPTR ) {
        pool = m_shared_thread_pool->threadPool();
    } else {
        ;
    }

    if ( pool != Q_NULLPTR ) {
        m_task_queue_ts->takeAndEnqueue( node );
        IcJoTaskWorker_RunObj *run_obj = new IcJoTaskWorker_RunObj( m_task_queue_ts, m_task_cntr );
        run_obj->setAutoDelete( true );
        m_task_cntr->fetchAndAddOrdered(1);

        pool->start( run_obj );
        return true;

    } else {
        node->dataRef().markDrop();
        node->dataRef().handle();
        qxpack_ic_delete( node, IcJoTaskWorker_ItemQueue::Node );
        return false;
    }
}

// ============================================================================
// post a item into task queue
// ============================================================================
bool   IcJoTaskWorkerPriv :: post(
    IcJoTaskWorker::TaskHandler task_handler, void *task_ctxt,
    const QJsonObject &jo,  const IcVarData &ext_param
) {
    if ( m_is_shutdown_stage ) {
        if ( task_handler != Q_NULLPTR ) {
            task_handler( task_ctxt, jo, ext_param, IcJoTaskWorker::HandlerType_Drop );
        }
        return true;
    }

    // ------------------------------------------------------------------------
    // create new item and post it
    // ------------------------------------------------------------------------
    IcJoTaskWorker_ItemQueue::Node *node = qxpack_ic_new ( IcJoTaskWorker_ItemQueue::Node );
    node->dataRef().init (
        task_handler, task_ctxt, jo, ext_param,
        QSharedPointer<QSemaphore>(), false, false
    );

    return this->addTaskNode( node );
}

// ============================================================================
//  send a task into queue
// ============================================================================
bool   IcJoTaskWorkerPriv :: send (
    IcJoTaskWorker::TaskHandler task_handler, void *task_ctxt,
    const QJsonObject &jo,  const IcVarData &ext_param, bool req_wait_end
) {
    // ------------------------------------------------------------------------
    // drop item if already shutdown
    // ------------------------------------------------------------------------
    if ( m_is_shutdown_stage ) {
        if ( task_handler != Q_NULLPTR ) {
            task_handler( task_ctxt, jo, ext_param, IcJoTaskWorker::HandlerType_Drop );
        }
        return true;
    }

    // ------------------------------------------------------------------------
    // create new item and post it
    // ------------------------------------------------------------------------
    IcJoTaskWorker_ItemQueue::Node *node = qxpack_ic_new ( IcJoTaskWorker_ItemQueue::Node );
    QSharedPointer<QSemaphore> sem = QSharedPointer<QSemaphore> (
        new QSemaphore, [](QSemaphore *s ){ delete s; }
    );
    node->dataRef().init (
        task_handler, task_ctxt, jo, ext_param, sem, true, req_wait_end
    );
    if ( this->addTaskNode( node )) {
        sem->acquire(1);
        if ( req_wait_end ) { sem->acquire(1); }
    }

    return true;
}





// ////////////////////////////////////////////////////////////////////////////
//
//                    IcJoTaskWorker wrapper API
//
// ////////////////////////////////////////////////////////////////////////////
#define T_PrivPtr( o )  T_ObjCast( IcJoTaskWorkerPriv*, o )

// ============================================================================
// ctor
// ============================================================================
IcJoTaskWorker :: IcJoTaskWorker ( QObject *pa ) : QObject( pa )
{
    m_obj = qxpack_ic_new_qobj( IcJoTaskWorkerPriv, -1 );
}

// ============================================================================
// ctor & create new thread pool
// ============================================================================
IcJoTaskWorker :: IcJoTaskWorker ( int max_t_num, QObject *pa ) : QObject( pa )
{
    m_obj = qxpack_ic_new_qobj( IcJoTaskWorkerPriv, max_t_num );
}

// ============================================================================
// dtor
// ============================================================================
IcJoTaskWorker :: ~IcJoTaskWorker ( )
{
    T_PrivPtr( m_obj )->shutdown();
    T_PrivPtr( m_obj )->waitForAllDone( -1 );
    qxpack_ic_delete_qobj( T_PrivPtr( m_obj ));
}

// ============================================================================
// once called it, any post or send will failed
// ============================================================================
void IcJoTaskWorker :: shutdown ( )
{ T_PrivPtr( m_obj )->shutdown(); }

// ============================================================================
// waiting until no item in the queue
// ============================================================================
bool IcJoTaskWorker :: waitForAllDone ( int ms )
{ return T_PrivPtr( m_obj )->waitForAllDone( ms ); }

// ============================================================================
// post a task callback to do
// ============================================================================
bool IcJoTaskWorker :: post(
    TaskHandler task_handler, void *task_ctxt, const QJsonObject &jo, const IcVarData &ext_param
) {
    return T_PrivPtr( m_obj )->post( task_handler, task_ctxt, jo, ext_param );
}

// ============================================================================
// send a task callback to do
// ============================================================================
bool IcJoTaskWorker :: send(
    TaskHandler task_handler, void *task_ctxt, const QJsonObject &jo, const IcVarData &ext_param,
    bool wait_for_finish
) {
    return T_PrivPtr( m_obj )->send( task_handler, task_ctxt, jo, ext_param, wait_for_finish );
}


}

#include "qxpack_ic_jotaskworker.moc"
#endif
