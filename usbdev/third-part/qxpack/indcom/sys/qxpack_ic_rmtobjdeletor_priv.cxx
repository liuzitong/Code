#ifndef QXPACK_IC_RMTOBJDELETOR_PRIV_CXX
#define QXPACK_IC_RMTOBJDELETOR_PRIV_CXX

#include "qxpack_ic_rmtobjdeletor_priv.hxx"

#include <QObject>
#include <QThread>
#include <QMetaObject>
#include <QEventLoop>
#include <QSemaphore>

namespace QxPack {

// ////////////////////////////////////////////////////
// deletor implement
// ////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN IcRmtObjDeletor_Impl : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY( IcRmtObjDeletor_Impl )
private:
    IcRmtObjDeletor::DeleteFunc  m_d_func; void *m_ctxt;
    QObject    *m_obj;
    QEventLoop *m_evt;
    QSemaphore *m_sem;
public :
    // -------------------------------------------------------
    // ctor
    // -------------------------------------------------------
    IcRmtObjDeletor_Impl ( IcRmtObjDeletor::DeleteFunc d_func, void *ctxt, QObject *obj, QEventLoop *evt )
        : QObject( Q_NULLPTR )
    {
        m_d_func = d_func; m_ctxt = ctxt; m_obj = obj; m_evt = evt; m_sem = Q_NULLPTR;
    }

    // ------------------------------------------------------
    // ctor
    // ------------------------------------------------------
    IcRmtObjDeletor_Impl ( IcRmtObjDeletor::DeleteFunc d_func, void *ctxt, QObject *obj, QSemaphore *sem )
        : QObject( Q_NULLPTR )
    {
        m_d_func = d_func; m_ctxt = ctxt; m_obj = obj; m_evt = Q_NULLPTR; m_sem = sem;
    }

    // -------------------------------------------------------
    // dtor
    // -------------------------------------------------------
    virtual ~IcRmtObjDeletor_Impl ( ) Q_DECL_OVERRIDE
    {
        if ( m_evt != Q_NULLPTR ) {
            QMetaObject::invokeMethod ( m_evt, "quit", Qt::QueuedConnection );
        }
        if ( m_sem != Q_NULLPTR ) {
            m_sem->release(1);
        }
    }

    // delete action and del self.
    Q_INVOKABLE  void  doDeleteAndDelSelf( );
};

// =============================================================
// do the delete operation
// =============================================================
void   IcRmtObjDeletor_Impl :: doDeleteAndDelSelf()
{
    // nw: 2019/05/18 fixed.
    if ( m_d_func != Q_NULLPTR ) {
        m_d_func( m_ctxt, m_obj ); // maybe shed. a deleteLater(),
    }
    this->deleteLater(); // makesure delete self. this will do after m_obj deleted.
}

// //////////////////////////////////////////////////////////
// jumper
// //////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN  IcRmtObjDeletor_ActDelay : public QObject {
    Q_OBJECT
public :
    IcRmtObjDeletor_ActDelay( ) : QObject( Q_NULLPTR ) { }

    Q_INVOKABLE void callDeleteAndDelSelf( QObject *tgt )
    {
        QMetaObject::invokeMethod ( tgt, "doDeleteAndDelSelf", Qt::QueuedConnection );
    }
};


// //////////////////////////////////////////////////////////
// wrapper API
// //////////////////////////////////////////////////////////
    IcRmtObjDeletor :: IcRmtObjDeletor ( ) { }
    IcRmtObjDeletor :: ~IcRmtObjDeletor( ) { }

// ==========================================================
// do delete action
// ==========================================================
bool  IcRmtObjDeletor :: deleteObjInThread (
    QThread *t, DeleteFunc f, void *ctxt, QObject *obj, bool req_evt_loop
)
{
    if ( f == Q_NULLPTR ) { return true; }
    if ( t == Q_NULLPTR ) {
       (* f )( ctxt, obj );

    } else if ( QThread::currentThread() != t ) {
        // check if thread is running or not
        if ( ! QThread::currentThread()->isRunning() ) {
            qFatal("[ IcRmtObjDeletor::deleteObjInThread ] the target thread(0x%llx) is not running!", quintptr( QThread::currentThread()) );
        }

        // delete the target object
        if ( req_evt_loop ) {
            QEventLoop evt;
            IcRmtObjDeletor_Impl *rmt_cr = new IcRmtObjDeletor_Impl ( f, ctxt, obj, &evt );
            rmt_cr->moveToThread( t );

            IcRmtObjDeletor_ActDelay  act_delay;
            QMetaObject::invokeMethod( // NOTE: the rmt_cr will be delete in doDelete()
                &act_delay, "callDeleteAndDelSelf", Qt::QueuedConnection,
                Q_ARG( QObject*, rmt_cr )
            );
            evt.exec( QEventLoop::ExcludeUserInputEvents );

        } else {
            QSemaphore sem(0);
            IcRmtObjDeletor_Impl *rmt_cr = new IcRmtObjDeletor_Impl( f, ctxt, obj, &sem );
            rmt_cr->moveToThread( t );

            QMetaObject::invokeMethod (
                    rmt_cr, "doDeleteAndDelSelf", Qt::BlockingQueuedConnection
            );
            sem.acquire(1); // here to wait for rmt_cr delete ocurred.
        }
    } else {
        (* f )( ctxt, obj );
    }

    return true;
}

}

#include "qxpack_ic_rmtobjdeletor_priv.moc"
#endif
