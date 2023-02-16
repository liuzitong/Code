// ////////////////////////////////////////////////////////////////////////////
/*!
 * @file qxpack_ic_datatranscli
 * @author nightwing
 * @date 2019/05
 * @verbatim
 *     <author>    <date>    <history memo.>
 *     nightwing  2019/05     build
 *     nightwing  2019/05/24  check point
 *     nightwing  2019/10/24  add fullServerName
 * @endverbatim
 */
// ////////////////////////////////////////////////////////////////////////////
#ifndef QXPACK_IC_DATATRANS_CLI_CXX
#define QXPACK_IC_DATATRANS_CLI_CXX

#include "qxpack_ic_datatrans_cli.hxx"
#include "qxpack/indcom/net/qxpack_ic_datatrans_rshdebase.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/sys/qxpack_ic_rmtobjcreator_priv.hxx"
#include "qxpack/indcom/sys/qxpack_ic_rmtobjdeletor_priv.hxx"

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//  private object
// ////////////////////////////////////////////////////////////////////////////
#define T_PrivPtr( o ) T_ObjCast( IcDataTransCliPriv*, o )
class QXPACK_IC_HIDDEN  IcDataTransCliPriv : public QObject {
    Q_OBJECT
private:
    QThread  *m_thread;  IcDataTransRsHdeBase *m_impl;
    QList<QByteArray>  m_recv_list;
    IcDataTransFactory m_fact; void *m_fact_ctxt;
    QString  m_rs_type; bool m_is_self_thread;
protected:
    Q_SLOT void  onNewRecvData ( const QByteArray& );
           void  initImpl  ( );
           void  deinitImpl( );
public :
    explicit IcDataTransCliPriv( const QString &rs_type, IcDataTransFactory, void*, QThread *thr = Q_NULLPTR );
    virtual ~IcDataTransCliPriv( ) Q_DECL_OVERRIDE;

    inline IcDataTransRsHdeBase* implement() { return m_impl; }
    inline bool  hasPendingRecvData () { return ! m_recv_list.isEmpty(); }
    inline void  close( ) { this->deinitImpl(); }

    void  open ( const QString &host, quint16 );
    bool  takePendingRecvData ( QByteArray & );
    bool  post ( const QByteArray & );

    Q_SIGNAL void newRecvData( );

    // below signal is just a wrap jump...
    Q_SIGNAL void postDataSent( );
    Q_SIGNAL void hostConnected( );
    Q_SIGNAL void finished( );
    Q_SIGNAL void errorMessage( int, const QString & );
};

// ============================================================================
// ctor
// ============================================================================
IcDataTransCliPriv :: IcDataTransCliPriv (
    const QString &rs_type, IcDataTransFactory fact, void *ctxt, QThread *thr
) : QObject( Q_NULLPTR ), m_impl( Q_NULLPTR )
{
    m_is_self_thread = ( thr == Q_NULLPTR );
    m_thread = thr;

    m_rs_type = rs_type;
    m_fact = fact; m_fact_ctxt = ctxt;
}

// ============================================================================
// dtor
// ============================================================================
IcDataTransCliPriv :: ~IcDataTransCliPriv ( )
{
    // nw@( 20190522 1513 ) old: QObject::disconnect ( this, Q_NULLPTR, Q_NULLPTR, Q_NULLPTR );
    // use blockSignals instead
    this->blockSignals( true );
    this->deinitImpl();
}

// ============================================================================
// take the pending response from inner queue
// ============================================================================
bool  IcDataTransCliPriv :: takePendingRecvData( QByteArray &ba )
{
    if ( ! m_recv_list.isEmpty() ) {
        ba = m_recv_list.takeFirst();
        return true;
    } else {
        return false;
    }
}

// ============================================================================
// slot: handle the new data vector signal
// ============================================================================
void  IcDataTransCliPriv :: onNewRecvData ( const QByteArray &ba )
{
    m_recv_list.push_back( ba );
    emit this->newRecvData();
}

// ============================================================================
// post the request to target host
// ============================================================================
bool  IcDataTransCliPriv :: post ( const QByteArray &ba )
{
    bool call_ret = false;

    if ( m_impl != Q_NULLPTR ) {
        // @20191105-1404 nw: fixed due to thread not same
        call_ret = QMetaObject::invokeMethod (
            m_impl, "post", Qt::AutoConnection,
            Q_ARG( const QByteArray&, ba ), Q_ARG( bool, true )
        );
    }

    return call_ret;
}

// ============================================================================
// initalize the implement object
// ============================================================================
void   IcDataTransCliPriv :: initImpl( )
{
    if ( m_is_self_thread  &&  m_thread == Q_NULLPTR ) { m_thread = new QThread; }
    if ( ! m_thread->isRunning()) { m_thread->start(); }

    // ------------------------------------------------------------------------
    // create the implement in target thread
    // ------------------------------------------------------------------------
    m_impl = qobject_cast<IcDataTransRsHdeBase*>(
      IcRmtObjCreator::createObjInThread (
          m_thread, []( void *ctxt )->QObject* {
              IcDataTransCliPriv *t_this = reinterpret_cast<IcDataTransCliPriv*>(ctxt);
              return  IcDataTransRsHdeBase::createObj (
                  t_this->m_rs_type, t_this->m_fact, t_this->m_fact_ctxt
              );
          }, this, false
    ));

    // ------------------------------------------------------------------------
    // connect all necessary signals
    // ------------------------------------------------------------------------
    if ( m_impl != Q_NULLPTR ) {
        QObject::connect(
            m_impl, SIGNAL(newData( const QByteArray &)),
            this,   SLOT(onNewRecvData( const QByteArray &))
        );
        QObject::connect(
            m_impl, SIGNAL(dataSent()), this, SIGNAL(postDataSent())
        );
        QObject::connect(
            m_impl, SIGNAL(errorMsg(int,const QString &)),
            this,   SIGNAL(errorMessage(int, const QString &))
        );
        QObject::connect( m_impl, SIGNAL(finished()), this, SIGNAL(finished()) );
        QObject::connect( m_impl, SIGNAL(hostConnected()), this, SIGNAL(hostConnected()));
    }
}

// ============================================================================
// delete the implement object
// ============================================================================
void   IcDataTransCliPriv :: deinitImpl()
{
    if ( m_impl != Q_NULLPTR ) {
        QObject::disconnect( m_impl, Q_NULLPTR, this, Q_NULLPTR );
        IcRmtObjDeletor::deleteObjInThread (
            m_thread, []( void *ctxt, QObject *) {
                IcDataTransCliPriv *t_this = reinterpret_cast<IcDataTransCliPriv*>( ctxt );
                t_this->m_impl->close();
                IcDataTransRsHdeBase::deleteObj( t_this->m_impl );
            }, this, Q_NULLPTR, false
        );
        m_impl = Q_NULLPTR;
    }

    if ( m_is_self_thread  && m_thread != Q_NULLPTR ) {
        m_thread->quit(); m_thread->wait(); m_thread->deleteLater(); m_thread = Q_NULLPTR;
    }
}

// ============================================================================
// open the target host
// ============================================================================
void   IcDataTransCliPriv :: open ( const QString &host, quint16 port )
{
    this->deinitImpl();
    this->initImpl();
    if ( m_impl != Q_NULLPTR ) {
        // nw: 20191105 fixed due to qthread maybe same as caller
        bool call_ret = QMetaObject::invokeMethod (
            m_impl, "open", Qt::AutoConnection,  // is caller same to target impl, directly call.
            Q_ARG( const QString &, host ),
            Q_ARG( quint16, port ),
            Q_ARG( int, QIODevice::ReadWrite )
        );

        if ( ! call_ret ) { this->deinitImpl(); } // failed to do open call.
    }
}


// ////////////////////////////////////////////////////////////////////////////
// wrap API
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor routine.
// ============================================================================
static void  gCtorRoutine( void *t_priv, IcDataTransCli *t_this )
{
    QObject::connect( T_PrivPtr( t_priv ), SIGNAL(newRecvData()), t_this, SIGNAL(newRecvData()));
    QObject::connect( T_PrivPtr( t_priv ), SIGNAL(postDataSent()), t_this, SIGNAL(postDataSent()));
    QObject::connect( T_PrivPtr( t_priv ), SIGNAL(hostConnected()), t_this, SIGNAL(hostConnected()));
    QObject::connect( T_PrivPtr( t_priv ), SIGNAL(finished()), t_this, SIGNAL(finished()));
    QObject::connect(
        T_PrivPtr( t_priv ), SIGNAL(errorMessage(int, const QString &)),
        t_this, SIGNAL(errorMessage(int,const QString &))
    );
}

// ============================================================================
// ctor
// ============================================================================
IcDataTransCli :: IcDataTransCli (
    const QString &rs_type, IcDataTransFactory  fact, void *ctxt, QObject *pa
) : QObject( pa )
{
    m_obj = qxpack_ic_new( IcDataTransCliPriv, rs_type, fact, ctxt );
    gCtorRoutine( m_obj, this );
}

// ============================================================================
// ctor ( override )
// ============================================================================
IcDataTransCli :: IcDataTransCli (
    const QString &rs_type, QThread *thr, IcDataTransFactory  fact, void *ctxt, QObject *pa
) : QObject( pa )
{
    m_obj = qxpack_ic_new( IcDataTransCliPriv, rs_type, fact, ctxt, thr );
    gCtorRoutine( m_obj, this );
}

// ============================================================================
// dtor
// ============================================================================
IcDataTransCli :: ~IcDataTransCli ( )
{
    QObject::disconnect( T_PrivPtr( m_obj ), Q_NULLPTR, this, Q_NULLPTR );
    qxpack_ic_delete( m_obj, IcDataTransCliPriv );
}

// ============================================================================
// check if implement is working
// ============================================================================
bool IcDataTransCli :: isWorking() const
{
    if ( T_PrivPtr( m_obj )->implement() == Q_NULLPTR ) {
        return false;
    } else { // @20191218 nw: directly visite it due to the calling is not change the object ( isWorking() const )
        return T_PrivPtr( m_obj )->implement()->isWorking();
//        bool ret = false;
        // @20191105-1402 nw: fixed due to thread maybe not same        
//        bool call_ret = QMetaObject::invokeMethod (
//            T_PrivPtr( m_obj )->implement(), "isWorking",
//            ( QThread::currentThread() != T_PrivPtr(m_obj)->implement()->thread() ?
//            Qt::BlockingQueuedConnection : Qt::AutoConnection ),
//            Q_RETURN_ARG( bool, ret )
//        );
//        return ( call_ret ? ret : false );
    }
}

// ============================================================================
// connect to target host, the hostConnected() will be emit if connected
// ============================================================================
void  IcDataTransCli :: connectToHost( const QString &host, quint16 port )
{  T_PrivPtr( m_obj )->open( host, port ); }

// ============================================================================
// check if has pending response
// ============================================================================
bool  IcDataTransCli :: hasPendingRecvData ( )
{  return T_PrivPtr( m_obj )->hasPendingRecvData(); }

// ============================================================================
// take the next pending response
// ============================================================================
bool  IcDataTransCli :: takeNextPendingRecvData( QByteArray &ba )
{  return T_PrivPtr( m_obj )->takePendingRecvData( ba ); }

// ============================================================================
// post the request to server host
// ============================================================================
bool  IcDataTransCli :: post( const QByteArray &ba )
{  return T_PrivPtr( m_obj )->post( ba ); }

// ============================================================================
// return the remote host name
// ============================================================================
QString  IcDataTransCli :: fullHostName() const
{  return T_PrivPtr( m_obj )->implement()->fullHostName(); }

// ============================================================================
// [ static ] retreive the rs type
// ============================================================================
QStringList  IcDataTransCli :: avaliableRsTypeList( )
{  return IcDataTransRsHdeBase::avaliableRsTypeList(); }

}

#include "qxpack_ic_datatrans_cli.moc"
#endif
