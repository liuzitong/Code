#ifndef PERM_RPCCLIENT_CXX
#define PERM_RPCCLIENT_CXX

#include "perm_rpcclient.hxx"
#include "perm/common/perm_memcntr.hxx"
#include "jrpcplat/client/jrpcplat_loccli.hxx"
#include <QTimer>

namespace FcPerm {

#define T_PrivPtr( o )  perm_objcast( RpcClientPriv*, o )
class PERM_HIDDEN RpcClientPriv
{
private:
    RpcClient         * m_parent;
    JRpcPlat::LocCli  * m_cli;
    QString             m_svr_name;
    bool                m_connected;

public:
    explicit RpcClientPriv ( const QString &name, RpcClient *pa );
    ~RpcClientPriv( );

    inline JRpcPlat::LocCli*  rpcClient( ) { return m_cli; }

    void  connectToServer( const QString &svr );
    void  disconnectFromServer( );
};

RpcClientPriv::RpcClientPriv(const QString &name, RpcClient *pa)
    : m_parent( pa ), m_connected(false)
{
    JRpcPlat::LocCliCfg cfg;
    cfg.setAliasName( name );
    m_cli = perm_new( JRpcPlat::LocCli, cfg );

    QObject::connect( m_cli, & JRpcPlat::LocCli::ready, [this](){
        // 客户端连接成功
        m_connected = true;
        emit m_parent->ready();
    });
    QObject::connect( m_cli, & JRpcPlat::LocCli::serverDisconnected, [this](){
        if ( m_connected ) { return; }
        QTimer::singleShot( 200, [=]() { m_cli->connectToServer(m_svr_name); } );
    });
    QObject::connect( m_cli, & JRpcPlat::LocCli::newNotice, [this](){
        emit m_parent->newNotice( m_cli->takeNextPendingNotice() );
    });
    QObject::connect( m_cli, & JRpcPlat::LocCli::newRequest, [this](){
        emit m_parent->newRequest( m_cli->takeNextPendingRequest() );
    });
}

RpcClientPriv::~RpcClientPriv()
{
    perm_delete( m_cli, JRpcPlat::LocCli );
}

void RpcClientPriv::connectToServer(const QString &svr)
{
    m_svr_name = svr;
    m_cli->connectToServer( m_svr_name );
}

void RpcClientPriv::disconnectFromServer()
{
    m_cli->disconnectFromServer();
}

RpcClient::RpcClient(const QString &name)
{
    m_obj = perm_new( RpcClientPriv, name, this );
}

RpcClient::~RpcClient()
{
    perm_delete( m_obj, RpcClientPriv );
}

JRpcPlat::LocCli *RpcClient::rpcClient() const
{ return T_PrivPtr( m_obj )->rpcClient(); }

void RpcClient::connectToServer(const QString &svr)
{ T_PrivPtr( m_obj )->connectToServer( svr ); }

void RpcClient::disconnectFromServer()
{ T_PrivPtr( m_obj )->disconnectFromServer( ); }

}

#endif // PERM_RPCCLIENT_CXX
