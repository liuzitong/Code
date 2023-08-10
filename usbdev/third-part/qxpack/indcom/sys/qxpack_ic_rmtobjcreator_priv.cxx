#ifndef QXPACK_IC_RMTOBJCREATOR_PRIV_CXX
#define QXPACK_IC_RMTOBJCREATOR_PRIV_CXX

#include "qxpack_ic_rmtobjcreator_priv.hxx"
#include <QThread>
#include <QMetaObject>
#include <QEventLoop>
#include <QObject>
#include <QAtomicPointer>

namespace QxPack {

// //////////////////////////////////////////////////////////
// creator implement
// //////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN IcRmtObjCreator_Impl : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY( IcRmtObjCreator_Impl )
private:
    IcRmtObjCreator::CreateFunc  m_func; void *m_ctxt;
    QEventLoop *m_evt;

public :
    // -------------------------------------------------------
    // ctor
    // -------------------------------------------------------
    IcRmtObjCreator_Impl ( IcRmtObjCreator::CreateFunc f, void *c, QEventLoop *evt )
                 : QObject( Q_NULLPTR )
    { m_func = f; m_ctxt = c; m_evt = evt; }

    // -------------------------------------------------------
    // dtor
    // -------------------------------------------------------
    virtual ~IcRmtObjCreator_Impl ( ) Q_DECL_OVERRIDE
    {
        if ( m_evt != Q_NULLPTR ) {
            QMetaObject::invokeMethod( m_evt, "quit", Qt::QueuedConnection );
        }
    }

    Q_INVOKABLE void  doCreate( qintptr obj_pptr );
};

// ===========================================================
// create object
// ===========================================================
void   IcRmtObjCreator_Impl :: doCreate( qintptr obj_pptr )
{
    QAtomicPointer<QObject> *pptr = reinterpret_cast<QAtomicPointer<QObject>*>( obj_pptr );
    if ( pptr->loadAcquire() == Q_NULLPTR ) {
        if ( m_func != Q_NULLPTR ) {
            pptr->storeRelease( m_func( m_ctxt ) );
        }
    }
}

// //////////////////////////////////////////////////////////
// action delay executor
// //////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN  IcRmtObjCreator_ActDelay : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY( IcRmtObjCreator_ActDelay )
public :
    IcRmtObjCreator_ActDelay( ) : QObject( Q_NULLPTR ) { }

    // -------------------------------------------------------
    //
    // -------------------------------------------------------
    Q_INVOKABLE void callCreateAndDel( QObject *tgt, qintptr rsl_pptr )
    {
        // 1) trigger target the doCreate method in remote thread
        QMetaObject::invokeMethod(
            tgt, "doCreate", Qt::QueuedConnection, Q_ARG( qintptr, rsl_pptr )
        );

        // 2) shed. delete later
        tgt->deleteLater();
    }
};

// //////////////////////////////////////////////////////////
//  wrap API
// //////////////////////////////////////////////////////////
    IcRmtObjCreator :: IcRmtObjCreator ( ) { }
    IcRmtObjCreator :: ~IcRmtObjCreator( ) { }

// =========================================================
// create object in thread
// =========================================================
QObject*  IcRmtObjCreator :: createObjInThread( QThread *t, CreateFunc f, void *ctxt, bool req_evt_loop )
{
    QAtomicPointer<QObject> r_obj( Q_NULLPTR );
    qintptr  r_obj_pptr = reinterpret_cast<qintptr>( &r_obj );

    if ( f == Q_NULLPTR ) { return Q_NULLPTR; }
    if ( t == Q_NULLPTR ) {
        r_obj = (* f )( ctxt );

    } else if ( QThread::currentThread() != t ) {
        if ( ! QThread::currentThread()->isRunning() ) {
            qFatal("[ IcRmtObjCreator::createObjInThread ] target thread(0x%llx) is not running!", quintptr( QThread::currentThread()) );
        }

        if ( req_evt_loop ) {
            QEventLoop evt;
            IcRmtObjCreator_Impl *rmt_cr = new IcRmtObjCreator_Impl( f, ctxt, &evt );
            rmt_cr->moveToThread( t );

            // shed. a call later
            IcRmtObjCreator_ActDelay act_delay;
            QMetaObject::invokeMethod ( // callCreateAndDel() will delete the rmt_cr
                &act_delay, "callCreateAndDel", Qt::QueuedConnection,
                Q_ARG( QObject*, rmt_cr ), Q_ARG( qintptr, r_obj_pptr )
            );

            // enter eventloop
            evt.exec( QEventLoop::ExcludeUserInputEvents );

        } else {
            IcRmtObjCreator_Impl *rmt_cr = new IcRmtObjCreator_Impl( f, ctxt, Q_NULLPTR );
            rmt_cr->moveToThread( t );

            // blocking call until result return
            QMetaObject::invokeMethod (
                rmt_cr, "doCreate", Qt::BlockingQueuedConnection,
                Q_ARG( qintptr, r_obj_pptr )
            );
            rmt_cr->deleteLater();
       }

    } else {
        r_obj = (* f )( ctxt );
    }

    return r_obj.loadAcquire();
}

}

#include "qxpack_ic_rmtobjcreator_priv.moc"
#endif
