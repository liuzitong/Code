#ifndef QXPACK_IC_DATATRANS_RSHDE_IMPL_BUNDLE_HXX
#define QXPACK_IC_DATATRANS_RSHDE_IMPL_BUNDLE_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include "qxpack/indcom/net/qxpack_ic_datatrans_buff.hxx"
#include "qxpack/indcom/net/qxpack_ic_datatrans_rshdebase.hxx"

// ////////////////////////////////////////////////////////////////////////////
//
//                 Local Socket
//
// ////////////////////////////////////////////////////////////////////////////
#include <QLocalSocket>

namespace QxPack {

class QXPACK_IC_HIDDEN  IcDataTransRsHdeLoc : public IcDataTransRsHdeBase {
    Q_OBJECT
    Q_DISABLE_COPY( IcDataTransRsHdeLoc )
protected:
    QLocalSocket *m_loc_sck;
    bool  m_is_work;

    Q_SLOT void  onLocDisconnected ( );
    Q_SLOT void  onLocConnected ( );
    Q_SLOT void  onLocSocketError ( QLocalSocket::LocalSocketError );
public :
    explicit IcDataTransRsHdeLoc ( IcDataTransFactory, void*, QObject *pa = Q_NULLPTR );
    virtual ~IcDataTransRsHdeLoc ( ) Q_DECL_OVERRIDE;

    virtual void     open ( const QString &host_name, quint16 port, int om ) Q_DECL_OVERRIDE;
    virtual bool     initByDscr ( QVariant ) Q_DECL_OVERRIDE;
    virtual bool     post ( const QByteArray &, bool = true ) Q_DECL_OVERRIDE;
    virtual void     close( )             Q_DECL_OVERRIDE { m_loc_sck->close(); }
    virtual bool     isWorking( ) const   Q_DECL_OVERRIDE { return m_is_work; }
    virtual QString  rsType( ) const      Q_DECL_OVERRIDE { return rsTypeName(); }
    virtual QString  fullHostName() const Q_DECL_OVERRIDE;
    static  QString  rsTypeName( ) { return QStringLiteral("rsLoc"); }
};

}


// ////////////////////////////////////////////////////////////////////////////
//
//               tcp socket
//
// ////////////////////////////////////////////////////////////////////////////
#include <QTcpSocket>

namespace QxPack {

class QXPACK_IC_HIDDEN  IcDataTransRsHdeTcp : public IcDataTransRsHdeBase {
    Q_OBJECT
    Q_DISABLE_COPY( IcDataTransRsHdeTcp )
protected:
    QTcpSocket  *m_tcp_sck;
    QString      m_host_name;
    bool  m_is_work;

    Q_SLOT void  onTcpDisconnected ( );
    Q_SLOT void  onTcpConnected ( );
    Q_SLOT void  onTcpSocketError ( QAbstractSocket::SocketError );
public :
    explicit IcDataTransRsHdeTcp ( IcDataTransFactory, void*, QObject *pa = Q_NULLPTR );
    virtual ~IcDataTransRsHdeTcp ( ) Q_DECL_OVERRIDE;

    virtual  void    open ( const QString &host_name, quint16 port, int open_mode ) Q_DECL_OVERRIDE;
    virtual  bool    initByDscr ( QVariant ) Q_DECL_OVERRIDE;
    virtual  bool    post ( const QByteArray &, bool = true ) Q_DECL_OVERRIDE;
    virtual  void    close( )       Q_DECL_OVERRIDE { m_tcp_sck->close(); }
    virtual  bool    isWorking( ) const  Q_DECL_OVERRIDE { return m_is_work; }
    virtual  QString rsType() const      Q_DECL_OVERRIDE { return rsTypeName(); }
    virtual  QString fullHostName() const Q_DECL_OVERRIDE { return m_host_name; }
    static QString rsTypeName() { return QStringLiteral("rsTcp"); }
};

}


#endif
