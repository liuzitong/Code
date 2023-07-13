#ifndef PERM_RPCCLIENT_HXX
#define PERM_RPCCLIENT_HXX

#include "perm/common/perm_def.h"
#include "qxpack/indcom/afw/qxpack_ic_svcbase.hxx"

namespace JRpcPlat {
class LocCli;
class NoticePkg;
class RequestPkg;
}

namespace FcPerm {

///////////////////////////////////////////////////
///
/// RpcClient
///
//////////////////////////////////////////////////
class PERM_API RpcClient : public QxPack::IcSvcBase
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit RpcClient( const QString & name );

    virtual ~RpcClient( ) Q_DECL_OVERRIDE;

    JRpcPlat::LocCli*  rpcClient( ) const;

    /**
     * @brief connectToServer
     * @param svr
     * 连接jrpc服务
     */
    void  connectToServer( const QString &svr );

    /**
     * @brief disconnectFromServer
     * 从JRpc服务器断开
     */
    void  disconnectFromServer( );

    Q_SIGNAL void  ready( );
    Q_SIGNAL void  newRequest ( const JRpcPlat::RequestPkg &pkg);
    Q_SIGNAL void  newNotice ( const JRpcPlat::NoticePkg &pkg );

private:
    void *m_obj;
    Q_DISABLE_COPY( RpcClient )
};

}

#endif // PERM_RPCCLIENT_HXX
