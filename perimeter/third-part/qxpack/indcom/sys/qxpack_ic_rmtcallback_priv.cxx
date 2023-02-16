#ifndef QXPACK_IC_RMTCALLBACK_PRIV_CXX
#define QXPACK_IC_RMTCALLBACK_PRIV_CXX

#include "qxpack_ic_rmtcallback_priv.hxx"
#include "../common/qxpack_ic_pimplprivtemp.hpp"
#include <QMetaObject>
#include <QTimer>
#include <QObject>
#include <QMetaObject>
#include <QThread>

namespace QxPack {


// ////////////////////////////////////////////////////////////////////////////
//
//                      trigger callback
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcRmtCallbackTrg : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY( IcRmtCallbackTrg )
public :
    explicit IcRmtCallbackTrg( ) { }
    virtual ~IcRmtCallbackTrg( ) { }
    Q_SIGNAL void reqTrigger( );
};




// ////////////////////////////////////////////////////////////////////////////
//
//                      worker
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN  IcRmtCallback_Wkr : public QObject {
    Q_OBJECT
private:
    IcRmtCallback::CallbackFunc m_func; void *m_ctxt;
    int  m_trg_tm, m_curr_trg;
protected:
    Q_SLOT  void  tmr_onTimeout( );
public :
    explicit IcRmtCallback_Wkr( IcRmtCallback::CallbackFunc f, void *ctxt)
        : m_func( f ), m_ctxt( ctxt ), m_trg_tm(1), m_curr_trg(0) { }
    virtual ~IcRmtCallback_Wkr( ) Q_DECL_OVERRIDE { }

    Q_INVOKABLE void  initTrg( QObject*, int trg_tm );

    Q_INVOKABLE void  doWork   ( int ms );
    Q_SLOT      void  onReqTrg ( );
};

// ============================================================================
// do the work
// ============================================================================
void                    IcRmtCallback_Wkr :: doWork( int ms )
{
    if ( ms > 0 ) {
         QTimer::singleShot( ms, this, SLOT(tmr_onTimeout()) );
    } else {
        if ( m_func != Q_NULLPTR ) { m_func( m_ctxt ); }
        this->deleteLater();
    }
}

// ============================================================================
// slot: handle the time out
// ============================================================================
void                    IcRmtCallback_Wkr :: tmr_onTimeout()
{
    if ( m_func != Q_NULLPTR ) { m_func( m_ctxt ); }
    this->deleteLater();
}

// ============================================================================
// initalize the trigger
// ============================================================================
void                    IcRmtCallback_Wkr :: initTrg( QObject* obj, int trg_tm )
{
    IcRmtCallbackTrg *trg = qobject_cast<IcRmtCallbackTrg*>(obj);
    if ( trg == Q_NULLPTR ) { return; }
    QObject::connect( trg, SIGNAL(reqTrigger()), this, SLOT(onReqTrg()), Qt::AutoConnection );
    QObject::connect( trg, SIGNAL(destroyed(QObject*)), this, SLOT(deleteLater()));
    m_trg_tm = ( trg_tm > 0 ? trg_tm : 1 );
    m_curr_trg = 0;
}

// ============================================================================
// slot: handle req. trigger
// ============================================================================
void                    IcRmtCallback_Wkr :: onReqTrg()
{
    if ( ++ m_curr_trg < m_trg_tm ) { return; }
    m_curr_trg = 0;
    if ( m_func != Q_NULLPTR ) { m_func( m_ctxt ); }
}





// ////////////////////////////////////////////////////////////////////////////
//
//                      private of trigger
//
// ////////////////////////////////////////////////////////////////////////////
#define T_TrgImp( o )  T_ObjCast( IcRmtCallback_TrgImp*, o  )
class QXPACK_IC_HIDDEN  IcRmtCallback_TrgImp : public IcPImplPrivTemp<IcRmtCallback_TrgImp> {
private:
    IcRmtCallbackTrg *m_trg;
public :
    explicit IcRmtCallback_TrgImp( ) : m_trg(Q_NULLPTR) { }
    IcRmtCallback_TrgImp( const IcRmtCallback_TrgImp &other ) { m_trg = other.m_trg; }
    virtual ~IcRmtCallback_TrgImp( ) Q_DECL_OVERRIDE { if ( m_trg != Q_NULLPTR ) { m_trg->deleteLater(); }}
    inline IcRmtCallbackTrg*&  trg() { return m_trg; }
};

// ============================================================================
// ctor
// ============================================================================
IcRmtCallback::Trigger :: Trigger ( )
{ m_obj = nullptr; }

// ============================================================================
// dtor
// ============================================================================
IcRmtCallback::Trigger :: ~Trigger( )
{
    if ( m_obj != nullptr ) {
        IcRmtCallback_TrgImp::attach( & m_obj, nullptr );
    }
}

// ============================================================================
// ctor ( copy )
// ============================================================================
IcRmtCallback::Trigger :: Trigger ( const IcRmtCallback::Trigger & other )
{
    m_obj = nullptr;
    IcRmtCallback_TrgImp::attach( & m_obj, const_cast<void**>(&other.m_obj));
}

// ============================================================================
// assign ( copy )
// ============================================================================
IcRmtCallback::Trigger &   IcRmtCallback::Trigger :: operator = ( const IcRmtCallback::Trigger &other )
{
    IcRmtCallback_TrgImp::attach( &m_obj, const_cast<void**>(&other.m_obj));
    return *this;
}

#ifdef QXPACK_IC_CXX11
IcRmtCallback::Trigger :: Trigger ( IcRmtCallback::Trigger &&other )
{
    m_obj = other.m_obj; other.m_obj = nullptr;
}

IcRmtCallback::Trigger &   IcRmtCallback::Trigger :: operator = ( IcRmtCallback::Trigger &&other )
{
    if ( m_obj != nullptr ) { IcRmtCallback_TrgImp::attach(&m_obj,nullptr); }
    m_obj = other.m_obj; other.m_obj = nullptr;
    return *this;
}
#endif

// ============================================================================
// request trigger
// ============================================================================
void                    IcRmtCallback::Trigger :: reqTrigger()
{
    if ( m_obj != nullptr ) {
        QMetaObject::invokeMethod (
            T_TrgImp(m_obj)->trg(), "reqTrigger", Qt::QueuedConnection
        );
    }
}


// ////////////////////////////////////////////////////////////////////////////
//                      implement API
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcRmtCallback :: IcRmtCallback( ) { }

// ============================================================================
// dtor
// ============================================================================
IcRmtCallback :: ~IcRmtCallback( ) { }

// ============================================================================
// post a delayed call in remote thread
// ============================================================================
void                    IcRmtCallback :: postDelayedCallbackInThread (
    QThread *t, CallbackFunc f, void *ctxt, int ms
) {
    if ( f == Q_NULLPTR ) { return; }
    if ( ms > 0 ) {
        IcRmtCallback_Wkr *wkr = new IcRmtCallback_Wkr( f, ctxt );
        wkr->moveToThread( t );
        QMetaObject::invokeMethod( wkr, "doWork", Qt::AutoConnection, Q_ARG( int, ms ) );
    } else {
        if ( QThread::currentThread() != t ) {
            IcRmtCallback_Wkr *wkr = new IcRmtCallback_Wkr( f, ctxt );
            wkr->moveToThread( t );
            QMetaObject::invokeMethod( wkr, "doWork", Qt::BlockingQueuedConnection, Q_ARG( int, ms ) );
        } else {
            f( ctxt );
        }
    }
}

// ============================================================================
// initialize a callback trigger in thread
// ============================================================================
IcRmtCallback::Trigger  IcRmtCallback :: initTrgCallbackInThread( QThread *t, CallbackFunc f, void *ctxt, int trg_tm )
{
    if ( f == Q_NULLPTR ) { return IcRmtCallback::Trigger(); }

    IcRmtCallback_Wkr *wkr = new IcRmtCallback_Wkr( f, ctxt );
    wkr->moveToThread( t );

    IcRmtCallback::Trigger trg;
    IcRmtCallback_TrgImp::buildIfNull( &trg.m_obj );

    IcRmtCallbackTrg *trg_qobj = new IcRmtCallbackTrg;
    T_TrgImp( trg.m_obj )->trg() = trg_qobj;

    QMetaObject::invokeMethod(
        wkr, "initTrg", ( QThread::currentThread() == t ? Qt::AutoConnection : Qt::BlockingQueuedConnection ),
        Q_ARG( QObject*, qobject_cast<QObject*>( trg_qobj ) ), Q_ARG( int, trg_tm )
    );

    return  trg;
}



}

#include "qxpack_ic_rmtcallback_priv.moc"
#endif

