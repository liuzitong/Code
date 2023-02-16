// ////////////////////////////////////////////////////////////////////////////
/*!
 * @file qxpack_ic_datatranssvr
 * @author nightwing
 * @date 2019/05
 * @verbatim
 *     <author>    <date>    <history memo.>
 *     nightwing  2019/05     build
 *     nightwing  2019/05     fixed.
 * @endverbatim
 */
// ////////////////////////////////////////////////////////////////////////////
#ifndef QXPACK_IC_DATATRANS_SVR_CXX
#define QXPACK_IC_DATATRANS_SVR_CXX

#include "qxpack_ic_datatrans_svr.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/sys/qxpack_ic_rmtobjcreator_priv.hxx"
#include "qxpack/indcom/sys/qxpack_ic_rmtobjdeletor_priv.hxx"
#include "qxpack/indcom/net/qxpack_ic_datatrans_buff.hxx"
#include "qxpack/indcom/net/qxpack_ic_datatrans_rshdebase.hxx"
#include "qxpack/indcom/net/qxpack_ic_datatrans_rssvrbase.hxx"

#include <QByteArray>
#include <QMetaObject>
#include <QMutex>
#include <QList>
#include <QPair>
#include <QAtomicInt>

#ifdef __GNUC__ // in GCC 5, close below warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif


namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
//                        requests handler
//
// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
//                        handler private
// ////////////////////////////////////////////////////////////////////////////
#define T_SvrHandler( o )  T_PtrCast( IcDataTransHandlerPriv*, o )
class QXPACK_IC_HIDDEN IcDataTransHandlerPriv : public QObject {
    Q_OBJECT
private:
    QThread  *m_thread; IcDataTransRsHdeBase *m_impl; qintptr m_sck;
    QList<QByteArray>  m_recv_list;
    IcDataTransFactory m_fact; void *m_fact_ctxt;
    QString  m_rs_type; bool m_is_self_thread;
protected:
           void  initImpl( );
           void  deinitImpl( );
           bool  postData( const QByteArray& );
    Q_SLOT void  onNewRecvData ( const QByteArray& );
public :
    IcDataTransHandlerPriv ( const QString &rs, qintptr sck, IcDataTransFactory, void*, QThread *thr = Q_NULLPTR );
    virtual ~IcDataTransHandlerPriv() Q_DECL_OVERRIDE;

    inline IcDataTransRsHdeBase* implement() { return m_impl; }
    inline bool  hasPendingRecvData() { return ! m_recv_list.isEmpty(); }

    bool  takePendingRecvData ( QByteArray & );
    bool  post ( const QByteArray & );

    Q_SIGNAL void newRecvData ( );

    // below signal is a wrapper jump
    Q_SIGNAL void postDataSent( );
    Q_SIGNAL void hostConnected( );
    Q_SIGNAL void finished( );
    Q_SIGNAL void errorMessage( int, const QString & );
};

// ============================================================================
// ctor
// ============================================================================
IcDataTransHandlerPriv :: IcDataTransHandlerPriv (
    const QString &rs, qintptr sck, IcDataTransFactory fact, void *ctxt, QThread *thr
) : QObject( Q_NULLPTR ), m_impl( Q_NULLPTR ), m_sck( sck )
{
    m_is_self_thread = ( thr == Q_NULLPTR );
    m_thread = thr;

    m_fact = fact; m_fact_ctxt = ctxt; m_rs_type = rs;
    this->initImpl( );
}

// ============================================================================
// dtor
// ============================================================================
IcDataTransHandlerPriv :: ~IcDataTransHandlerPriv ( )
{
    this->deinitImpl();
}

// ============================================================================
// init. implement object
// ============================================================================
void  IcDataTransHandlerPriv :: initImpl ( )
{
    // ------------------------------------------------------------------------
    // create pimpl object in target thread
    // ------------------------------------------------------------------------
    if ( m_is_self_thread  &&  m_thread == Q_NULLPTR ) { m_thread = new QThread; }
    if ( ! m_thread->isRunning() ) { m_thread->start(); }

    m_impl = qobject_cast<IcDataTransRsHdeBase*>(
      IcRmtObjCreator::createObjInThread (
          m_thread, []( void *ctxt )->QObject* {
              IcDataTransHandlerPriv *t_this = reinterpret_cast<IcDataTransHandlerPriv*>( ctxt );
              return IcDataTransRsHdeBase::createObj (
                  t_this->m_rs_type, t_this->m_fact, t_this->m_fact_ctxt
              );
          }, this, false
    ));

    // ------------------------------------------------------------------------
    // ensure get the request package
    // ------------------------------------------------------------------------
    if ( m_impl != Q_NULLPTR ) {
        QObject::connect(
            m_impl, SIGNAL(newData(const QByteArray&)), this, SLOT(onNewRecvData(const QByteArray&))
        );
        QObject::connect( m_impl, SIGNAL(errorMsg(int,const QString&)), this, SIGNAL(errorMessage(int,const QString &)));
        QObject::connect( m_impl, SIGNAL(hostConnected()), this, SIGNAL(hostConnected()));
        QObject::connect( m_impl, SIGNAL(dataSent()), this, SIGNAL(postDataSent()));
        QObject::connect( m_impl, SIGNAL(finished()), this, SIGNAL(finished()));

        bool rmt_ret = false;
        QMetaObject::invokeMethod (
            m_impl, "initByDscr",
            (( QThread::currentThread() != m_impl->thread() ) ? Qt::BlockingQueuedConnection : Qt::AutoConnection ),
            Q_RETURN_ARG(bool,rmt_ret), Q_ARG( QVariant, QVariant::fromValue( m_sck ))
        );
    }
}

// ============================================================================
// deinit the implement
// ============================================================================
void  IcDataTransHandlerPriv :: deinitImpl()
{
    if ( m_impl != Q_NULLPTR ) {
        QObject::disconnect( m_impl, Q_NULLPTR, this, Q_NULLPTR );
        IcRmtObjDeletor::deleteObjInThread(
            m_thread, []( void *ctxt, QObject* ) {
                IcDataTransHandlerPriv *t_this = reinterpret_cast<IcDataTransHandlerPriv*>( ctxt );
                t_this->m_impl->close();
                IcDataTransRsHdeBase::deleteObj( t_this->m_impl );
            }, this, Q_NULLPTR,false
        );
        m_impl = Q_NULLPTR;
    }

    if ( m_is_self_thread  &&  m_thread != Q_NULLPTR ) {
        m_thread->quit(); m_thread->wait(); m_thread->deleteLater(); m_thread = Q_NULLPTR;
    }

}

// ============================================================================
// slot: handle the new data
// ============================================================================
void IcDataTransHandlerPriv :: onNewRecvData ( const QByteArray &ba )
{
    if ( ! ba.isEmpty()) {
        m_recv_list.push_back ( ba );
        emit this->newRecvData();
    }
}

// ============================================================================
// take the data from recv. list
// ============================================================================
bool IcDataTransHandlerPriv :: takePendingRecvData ( QByteArray &ba )
{
    bool is_taked_req = false;

    if ( ! m_recv_list.empty() ) {
        ba = m_recv_list.takeFirst();
        is_taked_req = true;
    }

    return is_taked_req;
}

// ============================================================================
// post the data to implement object
// ============================================================================
bool IcDataTransHandlerPriv :: postData ( const QByteArray &ba )
{
    bool call_ret = false;
    if ( m_impl != Q_NULLPTR ) {
        call_ret = QMetaObject::invokeMethod (
            m_impl, "post", Qt::AutoConnection,
            Q_ARG( const QByteArray&, ba ), Q_ARG(bool,true)
        );
    }
    return call_ret;
}

// ============================================================================
// post the data, called from wrapper
// ============================================================================
bool IcDataTransHandlerPriv :: post ( const QByteArray &ba )
{
    this->postData( ba ); // directly call postData()
    return true;
}

// ////////////////////////////////////////////////////////////////////////////
//                  handler wrap API
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor routine
// ============================================================================
static void  gCtorRoutine( void *t_priv, IcDataTransHandler *t_this )
{
    QObject::connect ( T_SvrHandler( t_priv ), SIGNAL(newRecvData()), t_this, SIGNAL(newRecvData()));
    QObject::connect ( T_SvrHandler( t_priv ), SIGNAL(postDataSent()), t_this, SIGNAL(postDataSent()));
    QObject::connect ( T_SvrHandler( t_priv ), SIGNAL(hostConnected()), t_this, SIGNAL(hostConnected()));
    QObject::connect ( T_SvrHandler( t_priv ), SIGNAL(finished()), t_this, SIGNAL(finished()));
    QObject::connect (
        T_SvrHandler( t_priv ), SIGNAL(errorMessage(int, const QString & )),
        t_this, SIGNAL(errorMessage(int, const QString &))
    );
}

// ============================================================================
// ctor
// ============================================================================
IcDataTransHandler :: IcDataTransHandler (
    const QString &rs_type, qintptr sck, IcDataTransFactory fact, void *ctxt, QObject *pa
) : QObject( pa )
{
    m_obj = qxpack_ic_new( IcDataTransHandlerPriv, rs_type, sck, fact, ctxt );
    gCtorRoutine( m_obj, this );
}

// ============================================================================
// ctor
// ============================================================================
IcDataTransHandler :: IcDataTransHandler (
    const QString &rs_type, qintptr sck, QThread *thr, IcDataTransFactory fact, void *ctxt, QObject *pa
) : QObject( pa )
{
    m_obj = qxpack_ic_new( IcDataTransHandlerPriv, rs_type, sck, fact, ctxt, thr );
    gCtorRoutine( m_obj, this );
}

// ============================================================================
// dtor
// ============================================================================
IcDataTransHandler :: ~IcDataTransHandler ( )
{
    T_SvrHandler( m_obj )->disconnect();
    qxpack_ic_delete( m_obj, IcDataTransHandlerPriv );
}

// ============================================================================
// check if the socket is working or not
// ============================================================================
bool  IcDataTransHandler :: isWorking() const
{
    bool is_work = false;
    IcDataTransRsHdeBase *impl = T_SvrHandler( m_obj )->implement();
    if ( impl != Q_NULLPTR ) {
        QMetaObject::invokeMethod (
            impl, "isWorking",
            ( QThread::currentThread() != impl->thread() ? Qt::BlockingQueuedConnection : Qt::AutoConnection ),
            Q_RETURN_ARG(bool,is_work)
        );
    }
    return is_work;
}

// ============================================================================
// post response vector to socket, true means posted.
// ============================================================================
bool  IcDataTransHandler :: post( const QByteArray &ba )
{ return T_SvrHandler( m_obj )->post( ba ); }

// ============================================================================
// check if exist pending request
// ============================================================================
bool  IcDataTransHandler :: hasPendingRecvData ( ) const
{ return T_SvrHandler( m_obj )->hasPendingRecvData( ); }

// ============================================================================
// get the next pending request
// ============================================================================
bool IcDataTransHandler :: takeNextPendingRecvData( QByteArray &ba )
{ return T_SvrHandler( m_obj )->takePendingRecvData( ba ); }





// ////////////////////////////////////////////////////////////////////////////
//
//                             Server
//
// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
//                         server private object
// ////////////////////////////////////////////////////////////////////////////
#define T_SvrPriv( o )  T_ObjCast( IcDataTransSvrPriv*, o )
class QXPACK_IC_HIDDEN  IcDataTransSvrPriv : public QObject {
    Q_OBJECT
private:
    QThread *m_thread; IcDataTransRsSvrBase *m_impl;
    QString  m_host, m_full_svr_name;  quint16  m_port; QString m_rs_type;
    QMutex   m_hde_locker;  QList<QObject*>  m_hde_list;
    IcDataTransFactory m_fact; void *m_fact_ctxt;
    IcDataTransSvr *m_parent;  bool  m_is_self_thread;
protected:
    Q_SLOT void onReqHandleNewIncoming( QVariant );
           void initImpl( );
           void deinitImpl( );
public :
    explicit IcDataTransSvrPriv (
        const QString &rs, const QString &host, quint16 port, IcDataTransFactory, void*, IcDataTransSvr*,
        QThread *thr = Q_NULLPTR
    );
    virtual ~IcDataTransSvrPriv( ) Q_DECL_OVERRIDE;
    inline quint16  port( ) const  { return m_port; }
    inline QString  host( ) const  { return m_host; }
    inline IcDataTransRsSvrBase* implement() { return m_impl; }
    inline QString & fullSvrNameRef() { return m_full_svr_name; }
    QObject*         takeNextPendingHandler( );
    bool             hasPendingHandler( );

    Q_SIGNAL void newHandler( );
    Q_SIGNAL void accpetError( int err, const QString & );
};

// ============================================================================
// ctor
// ============================================================================
IcDataTransSvrPriv :: IcDataTransSvrPriv (
    const QString &rs_type, const QString &host, quint16 port,
    IcDataTransFactory fact, void *ctxt, IcDataTransSvr *parent,
    QThread *svr_thr
) {
    m_is_self_thread = ( svr_thr == Q_NULLPTR ); // nw: here means need a local thread.
    m_thread  = svr_thr;

    m_parent = parent;
    m_port = port; m_host = host;
    m_fact = fact; m_fact_ctxt = ctxt; m_rs_type = rs_type;

    this->initImpl();
}

// ============================================================================
// dtor
// ============================================================================
IcDataTransSvrPriv :: ~IcDataTransSvrPriv ( )
{
    // ------------------------------------------------------------------------
    // release all handler
    // ------------------------------------------------------------------------
    m_hde_locker.lock();
    QList<QObject*>::const_iterator c_itr = m_hde_list.cbegin();
    while ( c_itr != m_hde_list.cend()) {
        QObject *hde = *( c_itr ++ );
        if ( hde != Q_NULLPTR ) { hde->deleteLater(); }
    }
    m_hde_list.clear();
    m_hde_locker.unlock();

    // ------------------------------------------------------------------------
    // delete the server
    // ------------------------------------------------------------------------
    this->deinitImpl();
}

// ============================================================================
// init implement
// ============================================================================
void  IcDataTransSvrPriv :: initImpl()
{
    // ------------------------------------------------------------------------
    // create pimpl object in target thread
    // ------------------------------------------------------------------------
    if ( m_is_self_thread  &&  m_thread == Q_NULLPTR ) { m_thread = new QThread; }
    if ( ! m_thread->isRunning() ) { m_thread->start();  }

    m_impl = qobject_cast<IcDataTransRsSvrBase*>(
      IcRmtObjCreator::createObjInThread (
          m_thread, []( void *ctxt )->QObject* {
              IcDataTransSvrPriv *t_this = reinterpret_cast<IcDataTransSvrPriv*>( ctxt );
              return IcDataTransRsSvrBase::createObj( t_this->m_rs_type );
          }, this, false
    ));

    if ( m_impl != Q_NULLPTR ) {
        QObject::connect(
            m_impl, SIGNAL(reqHandleNewIncoming(QVariant)), this, SLOT(onReqHandleNewIncoming(QVariant))
        );
    }
}

// ============================================================================
// deinit implement
// ============================================================================
void  IcDataTransSvrPriv :: deinitImpl()
{
    if ( m_impl != Q_NULLPTR ) {
        QObject::disconnect( m_impl, Q_NULLPTR, this, Q_NULLPTR );
        IcRmtObjDeletor::deleteObjInThread(
            m_thread, []( void *ctxt, QObject *) {
                IcDataTransSvrPriv *t_this = reinterpret_cast<IcDataTransSvrPriv*>( ctxt );
                IcDataTransRsSvrBase::deleteObj( t_this->m_impl );
            }, this, Q_NULLPTR,false
        );
        m_impl = Q_NULLPTR;
    }

    if ( m_is_self_thread  &&  m_thread != Q_NULLPTR ) { // delete local thread only
       m_thread->quit(); m_thread->wait(); m_thread->deleteLater(); m_thread = Q_NULLPTR;
    }
}

// ============================================================================
// while a new incoming, we create a handler to accept
// ============================================================================
void  IcDataTransSvrPriv :: onReqHandleNewIncoming( QVariant handle )
{
    // ------------------------------------------------------------------------
    // create the handler
    // ------------------------------------------------------------------------
    if ( ! handle.isValid() ) { return; }

    QObject* hde = m_parent->createHandler (
        m_rs_type, handle.value<qintptr>(), m_fact, m_fact_ctxt
    );
    if ( hde != Q_NULLPTR ) {
        m_hde_locker.lock();
        m_hde_list.push_back( hde );
        m_hde_locker.unlock();
        emit this->newHandler();
    }
}

// ============================================================================
// take next handler
// ============================================================================
QObject*  IcDataTransSvrPriv :: takeNextPendingHandler()
{
    QObject *hde = Q_NULLPTR;
    m_hde_locker.lock();
    if ( m_hde_list.size() > 0 ) { hde = m_hde_list.takeFirst(); }
    m_hde_locker.unlock();
    return hde;
}

// ============================================================================
// check if existed handler in list
// ============================================================================
bool  IcDataTransSvrPriv :: hasPendingHandler( )
{
    bool is_remain_hde = false;

    m_hde_locker.lock();
    is_remain_hde = ( ! m_hde_list.isEmpty() );
    m_hde_locker.unlock();

    return is_remain_hde;
}

// ////////////////////////////////////////////////////////////////////////////
//                          server wrapper
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcDataTransSvr :: IcDataTransSvr (
    const QString &rs_type, const QString &host, quint16 port,
    IcDataTransFactory fact, void *fact_ctxt,
    QObject *pa ) : QObject( pa )
{
    m_obj = qxpack_ic_new( IcDataTransSvrPriv, rs_type, host, port, fact, fact_ctxt, this );
    QObject::connect (
        T_SvrPriv( m_obj ), SIGNAL(newHandler()),
        this, SIGNAL(newHandler()), Qt::QueuedConnection
    );
    QObject::connect (
       T_SvrPriv( m_obj ), SIGNAL(accpetError(int,const QString&)),
       this, SIGNAL(acceptError(int,const QString&)), Qt::QueuedConnection
    );
}

// ============================================================================
// ctor ( override )
// ============================================================================
IcDataTransSvr :: IcDataTransSvr (
    const QString &rs_type, const QString &host, quint16 port,
    QThread *svr_thr,
    IcDataTransFactory fact, void *fact_ctxt,
    QObject *pa ) : QObject( pa )
{
    m_obj = qxpack_ic_new( IcDataTransSvrPriv, rs_type, host, port, fact, fact_ctxt, this, svr_thr );
    QObject::connect (
        T_SvrPriv( m_obj ), SIGNAL(newHandler()),
        this, SIGNAL(newHandler()), Qt::QueuedConnection
    );
    QObject::connect (
       T_SvrPriv( m_obj ), SIGNAL(accpetError(int,const QString&)),
       this, SIGNAL(acceptError(int,const QString&)), Qt::QueuedConnection
    );
}

// ============================================================================
// dtor
// ============================================================================
IcDataTransSvr :: ~IcDataTransSvr ( )
{
    // nw@( 20190522 1515 ) old:  QObject::disconnect( T_SvrPriv( m_obj ), Q_NULLPTR, Q_NULLPTR, Q_NULLPTR );
    this->blockSignals( true );
    qxpack_ic_delete( m_obj, IcDataTransSvrPriv );
}

// ============================================================================
// start the server, here begin to listening on the port
// ============================================================================
bool  IcDataTransSvr :: startServer( )
{
    if ( T_SvrPriv( m_obj )->implement() == Q_NULLPTR ) {
        return false;
    } else {
        bool ret = false;
        bool call_ret = QMetaObject::invokeMethod (
            T_SvrPriv( m_obj )->implement(), "startServer",
            ( QThread::currentThread() != T_SvrPriv(m_obj)->implement()->thread() ?
              Qt::BlockingQueuedConnection : Qt::AutoConnection ),
            Q_RETURN_ARG( bool, ret ),
            Q_ARG( const QString &, T_SvrPriv( m_obj )->host()),
            Q_ARG( quint16, T_SvrPriv( m_obj )->port())
        );
        return ( call_ret ? ret : false );
    }
}

// ============================================================================
// stop the server
// ============================================================================
void  IcDataTransSvr :: stopServer()
{
    if ( T_SvrPriv( m_obj )->implement() != Q_NULLPTR ) {
        QMetaObject::invokeMethod(
            T_SvrPriv( m_obj )->implement(), "stopServer",
            ( T_SvrPriv(m_obj)->implement()->thread() != QThread::currentThread() ?
            Qt::BlockingQueuedConnection : Qt::AutoConnection )
        );
    }
}

// ============================================================================
// return true means existed handler, false for no or not working
// ============================================================================
bool  IcDataTransSvr :: hasPendingHandler() const
{ return T_SvrPriv( m_obj )->hasPendingHandler(); }

// ============================================================================
// check if this object is working
// ============================================================================
bool  IcDataTransSvr :: isListening() const
{
    IcDataTransSvrPriv *priv = T_SvrPriv( m_obj );

    if ( priv->implement() != Q_NULLPTR ) {
        bool ret = false;
        bool call_ret = QMetaObject::invokeMethod (
            priv->implement(), "isListening",
            ( priv->implement()->thread() != QThread::currentThread() ?
              Qt::BlockingQueuedConnection : Qt::AutoConnection ),
              Q_RETURN_ARG( bool, ret )
        );
        return ( call_ret ? ret : false );
    } else {
        return false;
    }
}

// ============================================================================
// take a pending handler from inner list
// ============================================================================
QObject* IcDataTransSvr :: takeNextPendingHandler( )
{ return T_SvrPriv( m_obj )->takeNextPendingHandler(); }

// ============================================================================
// [ virtual ] create new handler
// ============================================================================
QObject* IcDataTransSvr :: createHandler (
    const QString &rs_type, qintptr dscr, IcDataTransFactory fact, void *fact_ctxt
) {
    IcDataTransHandler *hde = qxpack_ic_new_qobj( IcDataTransHandler, rs_type, dscr, fact, fact_ctxt );
    if ( ! hde->isWorking()) { hde->deleteLater(); hde = Q_NULLPTR; }
    return hde;
}

// ============================================================================
// [ static ] return all supported avaliableRsTypeList
// ============================================================================
QStringList  IcDataTransSvr :: avaliableRsTypeList ( )
{ return IcDataTransRsSvrBase::avaliableRsTypeList(); }

// ============================================================================
// return the full server name, nw: 2019/12/13 added
// ============================================================================
QString                 IcDataTransSvr :: fullSvrName() const
{
    IcDataTransSvrPriv *priv = T_SvrPriv( m_obj );
    if ( priv->implement() != Q_NULLPTR ) {
        if ( priv->fullSvrNameRef().isEmpty()) {
            QString nm;
            bool call_ret = QMetaObject::invokeMethod(
                priv->implement(), "fullServerName",
                ( priv->implement()->thread() != QThread::currentThread() ?
                 Qt::BlockingQueuedConnection : Qt::AutoConnection ),
                Q_RETURN_ARG( QString, nm )
            );
            priv->fullSvrNameRef() = nm;
            return ( call_ret ? nm : QString() );
        } else {
            return priv->fullSvrNameRef();
        }
    } else {
        return QString();
    }
}


}


#ifdef __GNUC__ // in GCC 5, close below warnings
#pragma GCC diagnostic pop
#endif

#include "qxpack_ic_datatrans_svr.moc"

#endif
