#ifndef QXPACK_IC_RMTOBJSIGBLOCKER_CXX
#define QXPACK_IC_RMTOBJSIGBLOCKER_CXX

#include "qxpack_ic_rmtobjsigblocker_priv.hxx"
#include <QEventLoop>
#include <QSemaphore>
#include <QThread>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//  blocker implement
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN IcRmtObjSigBlocker_Impl : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY( IcRmtObjSigBlocker_Impl )
private:
    QObject    *m_obj;  QEventLoop *m_evt; QSemaphore *m_sem;
    bool   m_sw;
public :
    // ------------------------------------------------------------------------
    // ctor
    // ------------------------------------------------------------------------
    IcRmtObjSigBlocker_Impl ( QObject *obj, bool sw, QEventLoop *evt ) : QObject( Q_NULLPTR )
    {
        m_obj = obj; m_sw = sw; m_evt = evt; m_sem = Q_NULLPTR;
        QObject::connect( m_obj, SIGNAL(destroyed(QObject*)), this, SLOT(obj_onDestoryed(QObject*)));
    }

    // ------------------------------------------------------------------------
    // ctor
    // ------------------------------------------------------------------------
    IcRmtObjSigBlocker_Impl ( QObject *obj, bool sw, QSemaphore *sem ) : QObject( Q_NULLPTR )
    {
        m_obj = obj; m_sw = sw; m_evt = Q_NULLPTR; m_sem = sem;
        QObject::connect( m_obj, SIGNAL(destroyed(QObject*)), this, SLOT(obj_onDestoryed(QObject*)));
    }

    // ------------------------------------------------------------------------
    // dtor
    // ------------------------------------------------------------------------
    virtual ~IcRmtObjSigBlocker_Impl ( ) Q_DECL_OVERRIDE
    {
        if ( m_evt != Q_NULLPTR ) {
            QMetaObject::invokeMethod ( m_evt, "quit", Qt::QueuedConnection );
        }
        if ( m_sem != Q_NULLPTR ) {
            m_sem->release(1);
        }
    }

    // ------------------------------------------------------------------------
    // destoryed object handler
    // ------------------------------------------------------------------------
    Q_SLOT  void obj_onDestoryed( QObject* )
    {
        m_obj = Q_NULLPTR;
    }

    // ------------------------------------------------------------------------
    // block action and del self.
    // ------------------------------------------------------------------------
    Q_INVOKABLE  void  doBlockAndDelSelf( );
};

// =============================================================
// do the delete operation
// =============================================================
void   IcRmtObjSigBlocker_Impl :: doBlockAndDelSelf()
{
    if ( m_obj != Q_NULLPTR ) {
        m_obj->blockSignals( m_sw );
    }
    this->deleteLater(); // makesure delete self. this will do after m_obj deleted.
}

// ////////////////////////////////////////////////////////////////////////////
// jumper
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN  IcRmtObjSigBlocker_ActDelay : public QObject {
    Q_OBJECT
public :
    IcRmtObjSigBlocker_ActDelay( ) : QObject( Q_NULLPTR ) { }

    Q_INVOKABLE void callBlockAndDelSelf( QObject *tgt )
    {
        QMetaObject::invokeMethod ( tgt, "doBlockAndDelSelf", Qt::QueuedConnection );
    }
};


// ////////////////////////////////////////////////////////////////////////////
// implement code
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor / dtor
// ============================================================================
IcRmtObjSigBlocker :: IcRmtObjSigBlocker ( ) { }
IcRmtObjSigBlocker :: ~IcRmtObjSigBlocker( ) { }

// ============================================================================
// [ static ] remote thread
// ============================================================================
void   IcRmtObjSigBlocker :: blockSignals( QObject *obj, bool sw, bool req_evt_loop )
{
    if ( obj == Q_NULLPTR ) { return; }
    QThread *t = obj->thread();

    if ( QThread::currentThread() != t ) {
        // check if thread is running or not
        if ( ! QThread::currentThread()->isRunning() ) {
            qFatal("[ IcRmtObjSigBlocker::deleteObjInThread ] the target thread(0x%llx) is not running!", quintptr( QThread::currentThread()) );
        }

        // send to the target object
        if ( req_evt_loop ) {
            QEventLoop evt;
            IcRmtObjSigBlocker_Impl *impl = new IcRmtObjSigBlocker_Impl( obj, sw, &evt  );
            impl->moveToThread( t );

            IcRmtObjSigBlocker_ActDelay act_delay;
            QMetaObject::invokeMethod(  // NOTE: the impl will be delete in doDelete()
                &act_delay, "callBlockAndDelSelf", Qt::QueuedConnection,
                Q_ARG( QObject*, impl )
            );

            evt.exec( QEventLoop::ExcludeUserInputEvents );

        } else {
            QSemaphore sem(0);
            IcRmtObjSigBlocker_Impl *rmpl = new IcRmtObjSigBlocker_Impl( obj, sw, &sem );
            rmpl->moveToThread( t );

            QMetaObject::invokeMethod (
                rmpl, "doBlockAndDelSelf", Qt::BlockingQueuedConnection
            );
            sem.acquire(1); // here to wait for rmt_cr delete ocurred.
        }
    } else {
        if ( obj != Q_NULLPTR ) {
           obj->blockSignals( sw );
        }
    }
}


}

#include "qxpack_ic_rmtobjsigblocker_priv.moc"
#endif
