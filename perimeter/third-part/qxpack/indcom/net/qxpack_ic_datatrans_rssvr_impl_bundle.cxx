// ============================================================================
// author: nightwing
// date  : 2019/05/24
// to do : check point
// ============================================================================

#ifndef QXPACK_IC_DATATRANS_RSSVR_IMPL_BUNDLE_CXX
#define QXPACK_IC_DATATRANS_RSSVR_IMPL_BUNDLE_CXX

#include "qxpack_ic_datatrans_rssvr_impl_bundle.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"

// /////////////////////////////////////////////////////////////////////////////
//
//                    local server
//
// ////////////////////////////////////////////////////////////////////////////
namespace QxPack {

// ============================================================================
// ctor
// ============================================================================
IcDataTransRsSvrLoc :: IcDataTransRsSvrLoc ( QObject *pa ) : IcDataTransRsSvrBase( pa )
{
    QObject::connect(
       & m_svr_sock, SIGNAL(newIncoming(QVariant)), this, SIGNAL(reqHandleNewIncoming(QVariant))
    );
}

// ============================================================================
// dtor
// ============================================================================
IcDataTransRsSvrLoc :: ~IcDataTransRsSvrLoc ( )
{
    QObject::disconnect( & m_svr_sock, Q_NULLPTR, this, Q_NULLPTR );
    m_svr_sock.close();
}

// ============================================================================
// [ override ] start the server..
// ============================================================================
bool IcDataTransRsSvrLoc :: startServer(const QString &nm, quint16 )
{
    if ( ! m_svr_sock.isListening()) {
        return m_svr_sock.listen( nm );
    } else {
        return false;
    }
}

// ============================================================================
// [ override ] stop the server
// ============================================================================
void   IcDataTransRsSvrLoc :: stopServer ( )
{
    if ( m_svr_sock.isListening() ) {
        m_svr_sock.close();
    }
}

}

// /////////////////////////////////////////////////////////////////////////////
//
//                    Tcp server
//
// ////////////////////////////////////////////////////////////////////////////
namespace QxPack {

// ============================================================================
// ctor
// ============================================================================
IcDataTransRsSvrTcp :: IcDataTransRsSvrTcp ( QObject *pa ) : IcDataTransRsSvrBase( pa )
{
    QObject::connect(
       & m_svr_sock, SIGNAL(newIncoming(QVariant)), this, SIGNAL(reqHandleNewIncoming(QVariant))
    );
}

// ============================================================================
// dtor
// ============================================================================
IcDataTransRsSvrTcp :: ~IcDataTransRsSvrTcp ( )
{
    QObject::disconnect( & m_svr_sock, Q_NULLPTR, this, Q_NULLPTR );
    m_svr_sock.close();
}

// ============================================================================
// [ override ] start the server..
// ============================================================================
bool IcDataTransRsSvrTcp :: startServer(const QString &nm, quint16 port )
{
    if ( ! m_svr_sock.isListening()) {
        return m_svr_sock.listen( ( nm.isEmpty() ? QHostAddress::Any : QHostAddress( nm )), port );
    } else {
        return false;
    }
}

// ============================================================================
// [ override ] stop the server
// ============================================================================
void   IcDataTransRsSvrTcp :: stopServer ( )
{
    if ( m_svr_sock.isListening() ) {
        m_svr_sock.close();
    }
}



}
#endif
