// ============================================================================
// author: nightwing
// date  : 2019/05/24
// to do : check point
// ============================================================================

#ifndef QXPACK_IC_DATATRANS_RSHDE_IMPL_BUNDLE_CXX
#define QXPACK_IC_DATATRANS_RSHDE_IMPL_BUNDLE_CXX

#include "qxpack_ic_datatrans_rshde_impl_bundle.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"

// ////////////////////////////////////////////////////////////////////////////
//
//              local socket
//
// ////////////////////////////////////////////////////////////////////////////
#define LOC_READ_BUFFER_SIZE      (512*1024)

namespace QxPack {

// ============================================================================
// ctor
// ============================================================================
IcDataTransRsHdeLoc :: IcDataTransRsHdeLoc (
    IcDataTransFactory fact, void *ctxt, QObject *pa
) : IcDataTransRsHdeBase( fact, ctxt, pa )
{
    m_is_work = false;

    // ------------------------------------------------------------------------
    // create a local socket as QIODevice
    // ------------------------------------------------------------------------
    m_loc_sck = new QLocalSocket;
    QObject::connect( m_loc_sck, SIGNAL(connected()),    this, SLOT(onLocConnected()));
    QObject::connect( m_loc_sck, SIGNAL(disconnected()), this, SLOT(onLocDisconnected()));
    QObject::connect(
        m_loc_sck, SIGNAL(error(QLocalSocket::LocalSocketError)),
        this, SLOT(onLocSocketError(QLocalSocket::LocalSocketError))
    );
    m_loc_sck->setReadBufferSize( LOC_READ_BUFFER_SIZE );

    // ------------------------------------------------------------------------
    // ensure create read and write buffer
    // ------------------------------------------------------------------------
    QSharedPointer<QIODevice> io_dev = QSharedPointer<QIODevice>(
        m_loc_sck, [](QIODevice *dev){ dev->deleteLater(); }
    );
    this->createBuffer ( io_dev );
}

// ============================================================================
// dtor
// ============================================================================
IcDataTransRsHdeLoc :: ~IcDataTransRsHdeLoc ( )
{
    // ------------------------------------------------------------------------
    // the m_io_dev_sp keeped the m_loc_sck life-cycle
    // ------------------------------------------------------------------------
    QObject::disconnect( m_loc_sck, Q_NULLPTR, this, Q_NULLPTR );
    m_loc_sck->close();

}

// ============================================================================
// slot: target host connected
// ============================================================================
void  IcDataTransRsHdeLoc :: onLocConnected()
{
    m_is_work = true;
    emit this->hostConnected();
}

// ============================================================================
// slot: target disconnected
// ============================================================================
void  IcDataTransRsHdeLoc :: onLocDisconnected()
{
    if ( m_is_work ) {
        m_is_work = false;
        emit this->finished();
    }
}

// ============================================================================
// slot: socket error
// ============================================================================
void  IcDataTransRsHdeLoc :: onLocSocketError ( QLocalSocket::LocalSocketError err )
{
    Q_UNUSED( err );
    QString err_msg =  m_loc_sck->errorString();
    m_is_work = false; m_loc_sck->abort();
    emit this->errorMsg( m_loc_sck->error(), err_msg );
    emit this->finished( );
}

// ============================================================================
// connect to target host
// ============================================================================
void  IcDataTransRsHdeLoc :: open ( const QString &host_name, quint16 port, int om )
{
    Q_UNUSED( port )
    if ( host_name.isEmpty()) { return; }
    if ( m_loc_sck->isOpen()) { m_loc_sck->abort(); }
    m_loc_sck->connectToServer( host_name, static_cast<QIODevice::OpenMode>( om ));
}

// ============================================================================
// post the data to host
// ============================================================================
bool  IcDataTransRsHdeLoc :: post( const QByteArray &ba, bool f )
{
    if ( ba.isEmpty() || ! m_is_work ) { return false; }
    return pkgWriteBuffer()->post( ba, f );
}

// ============================================================================
// init by socket descriptor
// ============================================================================
bool  IcDataTransRsHdeLoc :: initByDscr ( QVariant v )
{
    if ( m_is_work ) { return m_is_work; }    
    m_is_work = m_loc_sck->setSocketDescriptor( v.value<qintptr>() );
    if ( ! m_is_work ) {
        qCritical (
            "Setup socket descriptor failed, code = '%d'", m_loc_sck->error()
        );
    }
    return m_is_work;
}

// ============================================================================
// return the full server name
// ============================================================================
QString   IcDataTransRsHdeLoc :: fullHostName() const
{
    if ( m_is_work ) { return m_loc_sck->fullServerName(); } else { return QString(); }
}

}


// ////////////////////////////////////////////////////////////////////////////
//
//                tcp socket
//
// ////////////////////////////////////////////////////////////////////////////
#define TCP_READ_BUFFER_SIZE      (512*1024)
#define TCP_OPT_RECV_BUFFER_SIZE  (512*1024)
#define TCP_OPT_SEND_BUFFER_SIZE  (512*1024)

namespace QxPack {

// ============================================================================
// ctor
// ============================================================================
IcDataTransRsHdeTcp :: IcDataTransRsHdeTcp ( IcDataTransFactory fact, void *ctxt, QObject *pa )
: IcDataTransRsHdeBase( fact, ctxt, pa )
{
    m_is_work = false;

    // ------------------------------------------------------------------------
    // create TCP socket as QIODevice
    // ------------------------------------------------------------------------
    m_tcp_sck = new QTcpSocket;
    QObject::connect( m_tcp_sck, SIGNAL(connected()),    this, SLOT(onTcpConnected()));
    QObject::connect( m_tcp_sck, SIGNAL(disconnected()), this, SLOT(onTcpDisconnected()));
    QObject::connect(
        m_tcp_sck, SIGNAL(error(QAbstractSocket::SocketError)),
        this, SLOT(onTcpSocketError(QAbstractSocket::SocketError))
    );
    m_tcp_sck->setReadBufferSize( TCP_READ_BUFFER_SIZE );

    // ------------------------------------------------------------------------
    // ensure create read and write buffer
    // ------------------------------------------------------------------------
    QSharedPointer<QIODevice> io_dev = QSharedPointer<QIODevice>(
        m_tcp_sck, [](QIODevice *dev){ dev->deleteLater(); }
    );
    this->createBuffer( io_dev );
}

// ============================================================================
// dtor
// ============================================================================
IcDataTransRsHdeTcp :: ~IcDataTransRsHdeTcp ( )
{
    // ------------------------------------------------------------------------
    // the m_io_dev_sp keeped the m_tcp_sck life
    // ------------------------------------------------------------------------
    QObject::disconnect( m_tcp_sck, Q_NULLPTR, this, Q_NULLPTR );
    m_tcp_sck->close();
}

// ============================================================================
// slot: target host connected
// ============================================================================
void  IcDataTransRsHdeTcp :: onTcpConnected()
{
    m_is_work = true;
    emit this->hostConnected();
}

// ============================================================================
// slot: target disconnected
// ============================================================================
void  IcDataTransRsHdeTcp :: onTcpDisconnected()
{
    if ( m_is_work ) {
        m_is_work = false;
        emit this->finished();
    }
}

// ============================================================================
// slot: socket error
// ============================================================================
void  IcDataTransRsHdeTcp :: onTcpSocketError ( QAbstractSocket::SocketError err )
{
    Q_UNUSED( err );
    QString err_msg = m_tcp_sck->errorString();
    m_is_work = false; m_tcp_sck->abort();
    emit this->errorMsg( m_tcp_sck->error(), err_msg );
    emit this->finished( );
}

// ============================================================================
// connect to target host
// ============================================================================
void  IcDataTransRsHdeTcp :: open ( const QString &host_name, quint16 port, int om )
{
    if ( host_name.isEmpty()) { return; }
    if ( m_tcp_sck->isOpen()) { m_tcp_sck->abort(); }
    m_host_name = host_name;
    m_tcp_sck->connectToHost ( host_name, port, static_cast<QIODevice::OpenMode>( om ));
}

// ============================================================================
// post the data to host
// ============================================================================
bool  IcDataTransRsHdeTcp :: post( const QByteArray &ba, bool f )
{
    if ( ba.isEmpty() || ! m_is_work ) { return false; }
    return pkgWriteBuffer()->post( ba, f );
}

// ============================================================================
// init by socket descriptor
// ============================================================================
bool  IcDataTransRsHdeTcp :: initByDscr ( QVariant v )
{
    if ( m_is_work ) { return m_is_work; }
    m_is_work = m_tcp_sck->setSocketDescriptor( v.value<qintptr>() );
    if ( ! m_is_work ) {
        qCritical (
            "Setup socket descriptor failed, code = '%d'", m_tcp_sck->error()
        );
    }
    return m_is_work;
}

}

#endif
