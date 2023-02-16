#ifndef QXPACK_IC_DATATRANS_RSSVR_IMPL_BUNDLE_HXX
#define QXPACK_IC_DATATRANS_RSSVR_IMPL_BUNDLE_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include "qxpack/indcom/net/qxpack_ic_datatrans_buff.hxx"
#include "qxpack/indcom/net/qxpack_ic_datatrans_rssvrbase.hxx"
#include <QVariant>

// ////////////////////////////////////////////////////////////////////////////
//
//                     local socket server
//
// ////////////////////////////////////////////////////////////////////////////
#include <QLocalServer>

namespace QxPack {

class QXPACK_IC_HIDDEN  IcDataTransRsSvrLoc_Socket : public QLocalServer {
    Q_OBJECT
protected:
    virtual void incomingConnection( quintptr handle ) Q_DECL_OVERRIDE
    { emit this->newIncoming( QVariant::fromValue( qintptr( handle )) ); }
public:
    IcDataTransRsSvrLoc_Socket( ) { }
    virtual ~IcDataTransRsSvrLoc_Socket( ) Q_DECL_OVERRIDE { }
    Q_SIGNAL void newIncoming( QVariant );
};

class QXPACK_IC_HIDDEN  IcDataTransRsSvrLoc : public IcDataTransRsSvrBase {
    Q_OBJECT
    Q_DISABLE_COPY( IcDataTransRsSvrLoc )
protected:
    IcDataTransRsSvrLoc_Socket m_svr_sock;
public :
    explicit IcDataTransRsSvrLoc ( QObject *pa = Q_NULLPTR );
    virtual ~IcDataTransRsSvrLoc ( ) Q_DECL_OVERRIDE;

    virtual bool     startServer( const QString &nm, quint16 port ) Q_DECL_OVERRIDE;
    virtual void     stopServer ( ) Q_DECL_OVERRIDE;
    virtual bool     isListening( ) const Q_DECL_OVERRIDE { return m_svr_sock.isListening(); }
    virtual QString  rsType( ) const Q_DECL_OVERRIDE { return rsTypeName(); }
    virtual QString  fullServerName( ) const Q_DECL_OVERRIDE { return m_svr_sock.fullServerName(); }
    static  QString  rsTypeName( ) { return QStringLiteral("rsLoc"); }
};

}

// ////////////////////////////////////////////////////////////////////////////
//
//               tcp socket server
//
// ///////////////////////////////////////////////////////////////////////////
#include <QTcpServer>

namespace QxPack {

class QXPACK_IC_HIDDEN  IcDataTransRsSvrTcp_Socket : public QTcpServer {
    Q_OBJECT
protected:
    virtual void incomingConnection( qintptr handle ) Q_DECL_OVERRIDE
    { emit this->newIncoming( QVariant::fromValue( handle ) ); }
public:
    IcDataTransRsSvrTcp_Socket( ) { }
    virtual ~IcDataTransRsSvrTcp_Socket( ) Q_DECL_OVERRIDE { }
    Q_SIGNAL void newIncoming( QVariant );
};

class QXPACK_IC_HIDDEN  IcDataTransRsSvrTcp : public IcDataTransRsSvrBase {
    Q_OBJECT
    Q_DISABLE_COPY( IcDataTransRsSvrTcp )
protected:
    IcDataTransRsSvrTcp_Socket m_svr_sock;
public :
    explicit IcDataTransRsSvrTcp ( QObject *pa = Q_NULLPTR );
    virtual ~IcDataTransRsSvrTcp ( ) Q_DECL_OVERRIDE;

    virtual bool     startServer( const QString &nm, quint16 port ) Q_DECL_OVERRIDE;
    virtual void     stopServer ( ) Q_DECL_OVERRIDE;
    virtual bool     isListening( ) const Q_DECL_OVERRIDE { return m_svr_sock.isListening(); }
    virtual QString  rsType( ) const Q_DECL_OVERRIDE { return rsTypeName(); }
    virtual QString  fullServerName( ) const Q_DECL_OVERRIDE { return m_svr_sock.serverAddress().toString(); }

    static  QString  rsTypeName( ) { return QStringLiteral("rsTcp"); }
};


}
#endif
