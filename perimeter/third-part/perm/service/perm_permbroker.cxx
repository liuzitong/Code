#ifndef PERM_PERMBROKER_CXX
#define PERM_PERMBROKER_CXX

#include "perm_permbroker.hxx"
#include "perm/common/perm_memcntr.hxx"
#include "perm/service/perm_svrprocess.hxx"
#include "perm/service/perm_rpcclient.hxx"
#include "perm/service/perm_userandperminfo.hxx"
#include "perm/model/perm_userinfodata.hxx"
#include "perm/model/perm_usergroupinfodata.hxx"
#include "perm/model/perm_typedef.hxx"
#include "perm/model/perm_groupsimpledesc.hxx"
#include "perm/model/perm_userlistinfo.hxx"
#include "perm/model/perm_usergrplistinfo.hxx"
#include "perm_specusergrplistinfo.hxx"
#include "perm_specuserpermlistinfo.hxx"
#include "perm_specusergrppermlistinfo.hxx"
#include "perm_editableuserinfo.hxx"
#include "perm_editableusergrpinfo.hxx"
#include "perm_editableuserdichgpwdinfo.hxx"
#include "jrpcplat/client/jrpcplat_loccli.hxx"
#include <QMutex>
#include <QFile>
#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonObject>
#include <QDir>

#if defined( QT_DEBUG )
#  if defined( Q_OS_WIN32 ) || defined( Q_OS_WIN )
#    define TEXT_PermExe  QStringLiteral("rpcpermd.exe")
#  else
#    define TEXT_PermExe  QStringLiteral("rpcpermd")
#  endif
#else
#  if defined( Q_OS_WIN32 ) || defined( Q_OS_WIN )
#    define TEXT_PermExe  QStringLiteral("rpcperm.exe")
#  else
#    define TEXT_PermExe  QStringLiteral("rpcperm")
#  endif
#endif

#define  TEXT_CliName    QStringLiteral("ClinetPerm")

namespace FcPerm {

#define T_PrivPtr( o )  perm_objcast( PermBrokerPriv*, o )
class PERM_HIDDEN PermBrokerPriv
{
private:
    PermBroker           *m_parent;
    RpcClient            *m_rpc_cli;
    SvrProcess           *m_svr_pro;
    UserAndPermInfo      *m_user_and_perm_info;
    QString               m_svr_name;

protected:
    void setUIDOfUserAndPermInfo( const QString & );
    void setAccTokenOfUserAndPermInfo( const QString & );
    void setQueryTokenOfUserAndPermInfo( const QString & );
    void setActualPermListOfUserAndPermInfo( const QJsonArray & );
    void setAuthPermListOfUserAndPermInfo( const QJsonArray & );
    void setAuthGroupListOfUserAndPermInfo( const QJsonArray & );

public:
    explicit PermBrokerPriv ( PermBroker *pa );
    ~PermBrokerPriv( );
    void  connectToServer( const QString & );
    void  disconnectToServer( );

    inline UserAndPermInfo * userAndPermInfo( ) { return m_user_and_perm_info; }
    bool  login( const QJsonObject & );
    bool  logout( );
    void  getUserList( UserListInfo & );
    bool  deleteUser ( UserListInfo & );
    bool  getSpecUserPermList( const UserListInfo &, SpecUserPermListInfo & );
    bool  getSpecUserGrpList ( const UserListInfo &, SpecUserGrpListInfo  & );
    void  getUserGrpList( UserGrpListInfo & );
    bool  deleteUserGrp( UserGrpListInfo & );
    bool  getSpecUserGrpPermList( const UserGrpListInfo &, SpecUserGrpPermListInfo & );
    bool  getSpecUserGrpPermList( const QList<quint64> &, QStringList & );
    bool  createUser( EditableUserInfo &, UserListInfo &);
    bool  createUserGrp( EditableUserGrpInfo &, UserGrpListInfo & );
    bool  setUser( EditableUserInfo &, UserListInfo & );
    bool  setUserGrp( EditableUserGrpInfo &, UserGrpListInfo & );
    bool  getUser( const UserListInfo &, EditableUserInfo & );
    bool  getUser( const UserListInfo &, UserInfoData & );
    bool  getUserGrp( const UserGrpListInfo &, EditableUserGrpInfo & );
    bool  getUserGrp( const UserGrpListInfo &, UserGroupInfoData & );
    bool  diChgUserPwd( EditableUserDiChgPwdInfo & );
    void  quitRpcPerm();

private:
    void  starSvrProcess();
};

void PermBrokerPriv::setUIDOfUserAndPermInfo(const QString &data)
{ m_user_and_perm_info->setUID(data.toULongLong(Q_NULLPTR, 16)); }

void PermBrokerPriv::setAccTokenOfUserAndPermInfo(const QString &data)
{ m_user_and_perm_info->setAccToken(data); }

void PermBrokerPriv::setQueryTokenOfUserAndPermInfo(const QString &data)
{ m_user_and_perm_info->setQueryToken(data); }

void PermBrokerPriv::setActualPermListOfUserAndPermInfo(const QJsonArray &data)
{
    ActualPermList actual_perm_list;
    QJsonArray::const_iterator citr = data.constBegin();
    while ( citr != data.constEnd())
    { actual_perm_list.push_back((*( citr ++ )).toString()); }
    m_user_and_perm_info->setActualPermList(actual_perm_list);
}

void PermBrokerPriv::setAuthPermListOfUserAndPermInfo(const QJsonArray &data)
{
    AuthPermList auth_perm_list;
    QJsonArray::const_iterator citr = data.constBegin();
    while ( citr != data.constEnd())
    { auth_perm_list.push_back((*( citr ++ )).toString()); }
    m_user_and_perm_info->setAuthPermList(auth_perm_list);
}

void PermBrokerPriv::setAuthGroupListOfUserAndPermInfo(const QJsonArray &data)
{
    AuthGroupList auth_group_list;
    QJsonArray::const_iterator citr = data.constBegin();
    while ( citr != data.constEnd())
    {
        QJsonValue group = (*( citr ++ ));
        if ( !group.isArray() ) { continue; }

        QJsonArray ja = group.toArray();
        if ( ja.size() == 2 )
        {
            GroupSimpleDesc group_simple_desc;
            quint64 gid = ja.at(0).toString().toULongLong(Q_NULLPTR, 16);
            QString name= ja.at(1).toString();
            auth_group_list.push_back(GroupSimpleDesc(gid, name));
        }
    }
    m_user_and_perm_info->setAuthGroupList(auth_group_list);
}

PermBrokerPriv::PermBrokerPriv(PermBroker *pa)
{
    m_parent = pa;
    m_rpc_cli = Q_NULLPTR;
    m_svr_pro = Q_NULLPTR;
    m_user_and_perm_info = &UserAndPermInfo::globalInstance();
    
    m_svr_pro = perm_new( SvrProcess );
    QObject::connect( m_svr_pro, &SvrProcess::finished, m_parent, &PermBroker::quit );
}

PermBrokerPriv::~PermBrokerPriv()
{
    if ( m_rpc_cli != Q_NULLPTR ) { perm_delete( m_rpc_cli, RpcClient ); }
    if ( m_svr_pro != Q_NULLPTR ) { perm_delete( m_svr_pro, SvrProcess ); }
}

void PermBrokerPriv::connectToServer(const QString &svr)
{
    if ( m_rpc_cli == Q_NULLPTR ) {
        m_svr_name = svr;
        m_rpc_cli = perm_new( RpcClient, TEXT_CliName );
        QObject::connect( m_rpc_cli, &RpcClient::ready, [this](){
            m_rpc_cli->rpcClient()->regMethod( QStringList() << "rpc::JRpcNtc::*" );
            this->starSvrProcess();
        } );
        QObject::connect( m_rpc_cli, &RpcClient::newNotice, [this](const JRpcPlat::NoticePkg &ntc){
            QJsonObject params = ntc.params().toObject();
            QJsonObject client = params.value("client").toObject();
            QString name = client.value("aliasName").toString();

            if ( ntc.method() == "rpc::JRpcNtc::methodReg" ) {
                if ( name == "ServerPerm" ) {
                    emit m_parent->ready();
                }
                return;
            }
        } );
        m_rpc_cli->connectToServer( svr );
    }
}

void PermBrokerPriv::disconnectToServer()
{
    if(m_rpc_cli == Q_NULLPTR){ return; }
    m_rpc_cli->disconnectFromServer();
}

bool PermBrokerPriv::login(const QJsonObject &jo)
{
    if(m_rpc_cli == Q_NULLPTR){ return false; }
    QJsonObject params = {
        {"name", jo.value("name").toString()},
        {"pwd", jo.value("pwd").toString()},
        {"ntcIfc", m_rpc_cli->rpcClient()->uidStr()}
    };

    JRpcPlat::RequestPkg req;
    req.setId( 1000 );
    req.setMethod("RpcPerm.login");
    req.setParams(params);
    JRpcPlat::ResponsePkg resp = m_rpc_cli->rpcClient()->invokeRequest( req, false, 6000 );
    if ( !resp.isResult() )
    {
        emit m_parent->error(resp.errCode(), resp.errMsg());
        return false;
    }

    QJsonObject data = resp.data().toObject();

    if ( data.contains("actualPermList") )
    {
        // 判断是否是仅用于第三方权限验证查询用户
        QJsonArray perm_array = data.value("actualPermList").toArray();
        if ( !perm_array.contains("Perm.root") &&
             !perm_array.contains("Perm.sys") &&
             !perm_array.contains("Perm.admin") &&
             !perm_array.contains("Perm.manager") &&
             perm_array.contains("Perm.chkPerm") )
        {
            emit m_parent->error(-1, QObject::tr("Only used for third-party permission verification query users cannot login."));
            return false;
        }
    }

    if ( data.contains("UID")            ) { this->setUIDOfUserAndPermInfo(data.value("UID").toString()); }
    if ( data.contains("accToken")       ) { this->setAccTokenOfUserAndPermInfo(data.value("accToken").toString()); }
    if ( data.contains("queryToken")     ) { this->setQueryTokenOfUserAndPermInfo(data.value("queryToken").toString()); }
    if ( data.contains("actualPermList") ) { this->setActualPermListOfUserAndPermInfo(data.value("actualPermList").toArray()); }
    if ( data.contains("authPermList")   ) { this->setAuthPermListOfUserAndPermInfo(data.value("authPermList").toArray()); }
    if ( data.contains("authGroupList")  ) { this->setAuthGroupListOfUserAndPermInfo(data.value("authGroupList").toArray()); }

    return true;
}

bool PermBrokerPriv::logout()
{
    if(m_rpc_cli == Q_NULLPTR){ return false; }
    QJsonObject params = {
        {"UID", QStringLiteral("%1").arg(m_user_and_perm_info->UID(), 16, 16, QLatin1Char('0'))},
        {"accToken", m_user_and_perm_info->accToken()}
    };

    JRpcPlat::RequestPkg req;
    req.setId( 1000 );
    req.setMethod("RpcPerm.logout");
    req.setParams(params);
    JRpcPlat::ResponsePkg resp = m_rpc_cli->rpcClient()->invokeRequest( req, false, 6000 );
    if ( !resp.isResult() )
    {
        emit m_parent->error(resp.errCode(), resp.errMsg());
        return false;
    }

    m_user_and_perm_info->clear();

    return true;
}

void PermBrokerPriv::getUserList(UserListInfo &listInfo)
{
    if(m_rpc_cli == Q_NULLPTR){ return; }
    QJsonObject params = {
        {"UID", QStringLiteral("%1").arg(m_user_and_perm_info->UID(), 16, 16, QLatin1Char('0'))},
        {"accToken", m_user_and_perm_info->accToken()}
    };

    JRpcPlat::RequestPkg req;
    req.setId( 1000 );
    req.setMethod("RpcPerm.getUserList");
    req.setParams(params);
    JRpcPlat::ResponsePkg resp = m_rpc_cli->rpcClient()->invokeRequest( req, false, 6000 );
    if ( !resp.isResult() )
    {
        emit m_parent->error(resp.errCode(), resp.errMsg());
        return;
    }

    listInfo.empItem();
    QJsonObject data = resp.data().toObject();
    if ( data.contains("userList") )
    {
        QJsonArray userList = data.value("userList").toArray();
        QJsonArray::const_iterator citr = userList.constBegin();
        while ( citr != userList.constEnd() )
        {
            QJsonArray user = (*citr++).toArray();
            if ( user.size() == 4 )
            { listInfo.addItem( UserInfoData(user) ); }
        }
    }
}

bool PermBrokerPriv::deleteUser(UserListInfo &listInfo)
{
    if(m_rpc_cli == Q_NULLPTR){ return false; }
    UserInfoData currentUser = listInfo.currentUserInfo();
    if ( currentUser.isNull() ) { return false; }

    QJsonObject params = {
        {"UID", QStringLiteral("%1").arg(m_user_and_perm_info->UID(), 16, 16, QLatin1Char('0'))},
        {"accToken", m_user_and_perm_info->accToken()},
        {"targetUID", QStringLiteral("%1").arg(currentUser.UID(), 16, 16, QLatin1Char('0'))}
    };

    JRpcPlat::RequestPkg req;
    req.setId( 1000 );
    req.setMethod("RpcPerm.deleteUser");
    req.setParams(params);
    JRpcPlat::ResponsePkg resp = m_rpc_cli->rpcClient()->invokeRequest( req, false, 6000 );
    if ( !resp.isResult() )
    {
        emit m_parent->error(resp.errCode(), resp.errMsg());
        return false;
    }

    listInfo.rmvItem();

    return true;
}

bool PermBrokerPriv::getSpecUserPermList(const UserListInfo &listInfo, SpecUserPermListInfo &permList)
{
    if(m_rpc_cli == Q_NULLPTR){ return false; }
    UserInfoData currentUser = listInfo.currentUserInfo();
    if ( currentUser.isNull() ) { return false; }

    QJsonObject params = {
        {"UID", QStringLiteral("%1").arg(m_user_and_perm_info->UID(), 16, 16, QLatin1Char('0'))},
        {"accToken", m_user_and_perm_info->accToken()},
        {"targetUID", QStringLiteral("%1").arg(currentUser.UID(), 16, 16, QLatin1Char('0'))}
    };

    JRpcPlat::RequestPkg req;
    req.setId( 1000 );
    req.setMethod("RpcPerm.getUser");
    req.setParams(params);
    JRpcPlat::ResponsePkg resp = m_rpc_cli->rpcClient()->invokeRequest( req, false, 6000 );
    if ( !resp.isResult() )
    {
        emit m_parent->error(resp.errCode(), resp.errMsg());
        return false;
    }

    permList.setUserInfo( UserInfoData(resp.data().toObject()) );

    return true;
}

bool PermBrokerPriv::getSpecUserGrpList(const UserListInfo &listInfo, SpecUserGrpListInfo &grpList)
{
    if(m_rpc_cli == Q_NULLPTR){ return false; }
    UserInfoData currentUser = listInfo.currentUserInfo();
    if ( currentUser.isNull() ) { return false; }

    QJsonObject params = {
        {"UID", QStringLiteral("%1").arg(m_user_and_perm_info->UID(), 16, 16, QLatin1Char('0'))},
        {"accToken", m_user_and_perm_info->accToken()},
        {"targetUID", QStringLiteral("%1").arg(currentUser.UID(), 16, 16, QLatin1Char('0'))}
    };

    JRpcPlat::RequestPkg req;
    req.setId( 1000 );
    req.setMethod("RpcPerm.getUser");
    req.setParams(params);
    JRpcPlat::ResponsePkg resp = m_rpc_cli->rpcClient()->invokeRequest( req, false, 6000 );
    if ( !resp.isResult() )
    {
        emit m_parent->error(resp.errCode(), resp.errMsg());
        return false;
    }

    grpList.setUserInfo( UserInfoData(resp.data().toObject()) );

    return true;
}

void PermBrokerPriv::getUserGrpList(UserGrpListInfo &listInfo)
{
    if(m_rpc_cli == Q_NULLPTR){ return ; }
    QJsonObject parmas = {
        {"UID", QStringLiteral("%1").arg(m_user_and_perm_info->UID(), 16, 16, QLatin1Char('0'))},
        {"accToken", m_user_and_perm_info->accToken()},
    };

    JRpcPlat::RequestPkg req;
    req.setId( 1000 );
    req.setMethod("RpcPerm.getUserGroupList");
    req.setParams(parmas);
    JRpcPlat::ResponsePkg resp = m_rpc_cli->rpcClient()->invokeRequest( req, false, 6000 );
    if ( !resp.isResult() )
    {
        emit m_parent->error(resp.errCode(), resp.errMsg());
        return;
    }


    QJsonObject data = resp.data().toObject();
    if ( data.contains("userGroupList") )
    {
        UserGroupInfoList grpList;
        QJsonArray userGroupList = data.value("userGroupList").toArray();
        QJsonArray::const_iterator citr = userGroupList.constBegin();
        while ( citr != userGroupList.constEnd() )
        {
            QJsonArray user = (*citr++).toArray();
            if ( user.size() == 3 )
            { grpList.push_back( UserGroupInfoData(user) ); }
        }
        listInfo.refItem( grpList );
    }
}

bool PermBrokerPriv::deleteUserGrp(UserGrpListInfo &listInfo)
{
    if(m_rpc_cli == Q_NULLPTR){ return false; }
    UserGroupInfoData currentGroup = listInfo.currentUserGrpInfo();
    if ( currentGroup.isNull() ) { return false; }

    QJsonObject parmas = {
        {"UID", QStringLiteral("%1").arg(m_user_and_perm_info->UID(), 16, 16, QLatin1Char('0'))},
        {"accToken", m_user_and_perm_info->accToken()},
        {"targetGID", QStringLiteral("%1").arg(currentGroup.GID(), 16, 16, QLatin1Char('0'))}
    };

    JRpcPlat::RequestPkg req;
    req.setId( 1000 );
    req.setMethod("RpcPerm.deleteUserGroup");
    req.setParams(parmas);
    JRpcPlat::ResponsePkg resp = m_rpc_cli->rpcClient()->invokeRequest( req, false, 6000 );
    if ( !resp.isResult() )
    {
        emit m_parent->error(resp.errCode(), resp.errMsg());
        return false;
    }
    {
        // 成功删除用户组后，将当前用户组从用户组列表中移除
        AuthGroupList grpList = m_user_and_perm_info->authGroupList();
        QJsonArray grp_array;
        for ( int i=0; i<grpList.size(); i++ )
        {
            if ( grpList.at(i).gid() == currentGroup.GID() ) { continue; }
            QString gid  = QString("%1").arg(grpList.at(i).gid(), 16, 16, QLatin1Char('0'));
            QString name = grpList.at(i).name();
            grp_array.push_back( QJsonArray() << gid << name );
        }
        this->setAuthGroupListOfUserAndPermInfo(grp_array);

        // 从用户组列表中删除
        listInfo.rmvItem();
    }

    return true;
}

bool PermBrokerPriv::getSpecUserGrpPermList(const UserGrpListInfo &listInfo, SpecUserGrpPermListInfo &permList)
{
    if(m_rpc_cli == Q_NULLPTR){ return false; }
    UserGroupInfoData currentGroup = listInfo.currentUserGrpInfo();
    if ( currentGroup.isNull() ) { return false; }

    QJsonObject parmas = {
        {"UID", QStringLiteral("%1").arg(m_user_and_perm_info->UID(), 16, 16, QLatin1Char('0'))},
        {"accToken", m_user_and_perm_info->accToken()},
        {"targetGID", QStringLiteral("%1").arg(currentGroup.GID(), 16, 16, QLatin1Char('0'))}
    };

    JRpcPlat::RequestPkg req;
    req.setId( 1000 );
    req.setMethod("RpcPerm.getUserGroup");
    req.setParams(parmas);
    JRpcPlat::ResponsePkg resp = m_rpc_cli->rpcClient()->invokeRequest( req, false, 6000 );
    if ( !resp.isResult() )
    {
        emit m_parent->error(resp.errCode(), resp.errMsg());
        return false;
    }

    permList.setUserGrpInfo( UserGroupInfoData(resp.data().toObject()) );

    return true;
}

bool PermBrokerPriv::getSpecUserGrpPermList(const QList<quint64> &gidList, QStringList &permList)
{
    if(m_rpc_cli == Q_NULLPTR){ return false; }
    if ( gidList.size() <= 0 ) { return false; }

    QJsonArray targetGIDs;
    QList<quint64>::const_iterator citr = gidList.constBegin();
    while ( citr != gidList.constEnd() )
    { targetGIDs.push_back(QString("%1").arg((*(citr++)), 16, 16, QLatin1Char('0'))); }

    QJsonObject params = {
        {"UID", QStringLiteral("%1").arg(m_user_and_perm_info->UID(), 16, 16, QLatin1Char('0'))},
        {"accToken", m_user_and_perm_info->accToken()},
        {"targetGIDs", targetGIDs}
    };

    JRpcPlat::RequestPkg req;
    req.setId( 1000 );
    req.setMethod("RpcPerm.getUserGroup");
    req.setParams(params);
    JRpcPlat::ResponsePkg resp = m_rpc_cli->rpcClient()->invokeRequest( req, false, 6000 );
    if ( !resp.isResult() )
    {
        emit m_parent->error(resp.errCode(), resp.errMsg());
        return false;
    }

    QJsonObject respValue = resp.data().toObject();
    if ( respValue.contains("combinedPermList") )
    {
        QJsonArray combinedPermList = respValue.value("combinedPermList").toArray();
        QJsonArray::const_iterator citr = combinedPermList.constBegin();
        while ( citr != combinedPermList.constEnd() )
        { permList.push_back((*(citr++)).toString()); }
    }

    return true;
}

bool PermBrokerPriv::createUser(EditableUserInfo &user, UserListInfo & listInfo)
{
    if(m_rpc_cli == Q_NULLPTR){ return false; }
    QString name = user.userInfo().name();
    QString pwd  = user.userInfo().pwd();
    QString memo = user.userInfo().memo();
    QJsonArray privPermList;
    {
        QMap<QString, bool>::const_iterator citr = user.userInfo().privPermList().constBegin();
        while ( citr != user.userInfo().privPermList().constEnd() )
        { privPermList.push_back( QJsonArray() << citr.key() << citr.value() ); citr++; }
    }

    QJsonArray groupList;
    {
        QMap<quint64, QString>::const_iterator citr = user.userInfo().groupList().constBegin();
        while ( citr != user.userInfo().groupList().constEnd() )
        { groupList.push_back( QJsonArray() << QString("%1").arg(citr.key(), 16, 16, QLatin1Char('0'))<< citr.value() ); citr++; }
    }

    QJsonObject params = {
        {"UID", QStringLiteral("%1").arg(m_user_and_perm_info->UID(), 16, 16, QLatin1Char('0'))},
        {"accToken", m_user_and_perm_info->accToken()},
        {"isAppendMode", true},
        {"userInfo", QJsonObject({
             {"UID",  "" },
             {"name", name},
             {"pwd",  pwd },
             {"memo", memo},
             {"privPermList", privPermList},
             {"groupList", groupList}
         })}
    };

    JRpcPlat::RequestPkg req;
    req.setId( 1000 );
    req.setMethod("RpcPerm.setUser");
    req.setParams(params);
    JRpcPlat::ResponsePkg resp = m_rpc_cli->rpcClient()->invokeRequest( req, false, 6000 );
    if ( !resp.isResult() )
    {
        emit m_parent->error(resp.errCode(), resp.errMsg());
        return false;
    }

    emit m_parent->error(0, QObject::tr("User created successfully."));
    user.clear();
    listInfo.addItem(UserInfoData(resp.data().toObject()));

    return true;
}

bool PermBrokerPriv::createUserGrp(EditableUserGrpInfo &group, UserGrpListInfo &listInfo)
{
    if(m_rpc_cli == Q_NULLPTR){ return false; }
    QString name= group.userGrpInfo().name();
    QString memo = group.userGrpInfo().memo();
    QJsonArray permList;
    {
        for ( int i = 0; i < group.userGrpInfo().permList().size(); i++ )
        { permList.push_back( group.userGrpInfo().permList().at(i) ); }
    }

    QJsonObject params = {
        {"UID", QStringLiteral("%1").arg(m_user_and_perm_info->UID(), 16, 16, QLatin1Char('0'))},
        {"accToken", m_user_and_perm_info->accToken()},
        {"isAppendMode", true},
        {"groupInfo", QJsonObject({
             {"GID",  "" },
             {"name", name},
             {"memo", memo},
             {"permList", permList}
         })}
    };

    JRpcPlat::RequestPkg req;
    req.setId( 1000 );
    req.setMethod("RpcPerm.setUserGroup");
    req.setParams(params);
    JRpcPlat::ResponsePkg resp = m_rpc_cli->rpcClient()->invokeRequest( req, false, 6000 );
    if ( !resp.isResult() )
    {
        emit m_parent->error(resp.errCode(), resp.errMsg());
        return false;
    }

    emit m_parent->error(0, QObject::tr("User group created successfully."));

    {
        // 清空编辑页面数据
        group.clear();

        // 插入用户组列表
        listInfo.addItem( UserGroupInfoData(resp.data().toObject()) );

        // 成功插入用户组后，当前用户组可分配给当前用户
        AuthGroupList grpList = m_user_and_perm_info->authGroupList();
        QJsonArray grp_array;
        for ( int i=0; i<grpList.size(); i++ )
        {
            QString gid  = QString("%1").arg(grpList.at(i).gid(), 16, 16, QLatin1Char('0'));
            QString name = grpList.at(i).name();
            grp_array.push_back( QJsonArray() << gid << name );
        }
        grp_array.push_back( QJsonArray() << resp.data().toObject().value("GID") << resp.data().toObject().value("name") );
        this->setAuthGroupListOfUserAndPermInfo(grp_array);
    }

    return true;
}

bool PermBrokerPriv::setUser(EditableUserInfo &user, UserListInfo &listInfo)
{
    if(m_rpc_cli == Q_NULLPTR){ return false; }
    QString uid  = QString("%1").arg(user.userInfo().UID(), 16, 16, QLatin1Char('0'));
    QString name = user.userInfo().name();
    QString pwd  = user.userInfo().pwd();
    QString memo = user.userInfo().memo();
    QJsonArray privPermList;
    {
        QMap<QString, bool>::const_iterator citr = user.userInfo().privPermList().constBegin();
        while ( citr != user.userInfo().privPermList().constEnd() )
        { privPermList.push_back( QJsonArray() << citr.key() << citr.value() ); citr++; }
    }

    QJsonArray groupList;
    {
        QMap<quint64, QString>::const_iterator citr = user.userInfo().groupList().constBegin();
        while ( citr != user.userInfo().groupList().constEnd() )
        { groupList.push_back( QJsonArray() << QString("%1").arg(citr.key(), 16, 16, QLatin1Char('0'))<< citr.value() ); citr++; }
    }

    QJsonObject params = {
        {"UID", QStringLiteral("%1").arg(m_user_and_perm_info->UID(), 16, 16, QLatin1Char('0'))},
        {"accToken", m_user_and_perm_info->accToken()},
        {"isAppendMode", false},
        {"userInfo", QJsonObject({
             {"UID",  uid },
             {"name", name},
             {"pwd",  pwd },
             {"memo", memo},
             {"privPermList", privPermList},
             {"groupList", groupList}
         })}
    };

    JRpcPlat::RequestPkg req;
    req.setId( 1000 );
    req.setMethod("RpcPerm.setUser");
    req.setParams(params);
    JRpcPlat::ResponsePkg resp = m_rpc_cli->rpcClient()->invokeRequest( req, false, 6000 );
    if ( !resp.isResult() )
    {
        emit m_parent->error(resp.errCode(), resp.errMsg());
        return false;
    }


    emit m_parent->error(0, QObject::tr("User modified successfully."));
    UserInfoData info(resp.data().toObject());
    user.setUserInfo(info);
    listInfo.modItem(info);

    return true;
}

bool PermBrokerPriv::setUserGrp(EditableUserGrpInfo &group, UserGrpListInfo &listInfo)
{
    if(m_rpc_cli == Q_NULLPTR){ return false; }
    QString gid  = QString("%1").arg(group.userGrpInfo().GID(), 16, 16, QLatin1Char('0'));
    QString name = group.userGrpInfo().name();
    QString memo = group.userGrpInfo().memo();
    QJsonArray permList;
    {
        for ( int i = 0; i < group.userGrpInfo().permList().size(); i++ )
        { permList.push_back( group.userGrpInfo().permList().at(i) ); }
    }

    QJsonObject params = {
        {"UID", QStringLiteral("%1").arg(m_user_and_perm_info->UID(), 16, 16, QLatin1Char('0'))},
        {"accToken", m_user_and_perm_info->accToken()},
        {"isAppendMode", false},
        {"groupInfo", QJsonObject({
             {"GID",  gid },
             {"name", name},
             {"memo", memo},
             {"permList", permList}
         })}
    };

    JRpcPlat::RequestPkg req;
    req.setId( 1000 );
    req.setMethod("RpcPerm.setUserGroup");
    req.setParams(params);
    JRpcPlat::ResponsePkg resp = m_rpc_cli->rpcClient()->invokeRequest( req, false, 6000 );
    if ( !resp.isResult() )
    {
        emit m_parent->error(resp.errCode(), resp.errMsg());
        return false;
    }

    emit m_parent->error(0, QObject::tr("User group modified successfully."));
    UserGroupInfoData info(resp.data().toObject());
    group.setUserGrpInfo(info);
    listInfo.modItem(info);

    return true;
}

bool PermBrokerPriv::getUser(const UserListInfo &listInfo, EditableUserInfo &editableUserInfo)
{
    if(m_rpc_cli == Q_NULLPTR){ return false; }
    UserInfoData currentUser = listInfo.currentUserInfo();
    if ( currentUser.isNull() ) { return false; }

    QJsonObject params = {
        {"UID", QStringLiteral("%1").arg(m_user_and_perm_info->UID(), 16, 16, QLatin1Char('0'))},
        {"accToken", m_user_and_perm_info->accToken()},
        {"targetUID", QStringLiteral("%1").arg(currentUser.UID(), 16, 16, QLatin1Char('0'))}
    };

    JRpcPlat::RequestPkg req;
    req.setId( 1000 );
    req.setMethod("RpcPerm.getUser");
    req.setParams(params);
    JRpcPlat::ResponsePkg resp = m_rpc_cli->rpcClient()->invokeRequest( req, false, 6000 );
    if ( !resp.isResult() )
    {
        emit m_parent->error(resp.errCode(), resp.errMsg());
        return false;
    }

    editableUserInfo.setUserInfo(UserInfoData(resp.data().toObject()));

    return true;
}

bool PermBrokerPriv::getUser(const UserListInfo &listInfo, UserInfoData &user)
{
    if(m_rpc_cli == Q_NULLPTR){ return false; }
    UserInfoData currentUser = listInfo.currentUserInfo();
    if ( currentUser.isNull() ) { return false; }

    QJsonObject params = {
        {"UID", QStringLiteral("%1").arg(m_user_and_perm_info->UID(), 16, 16, QLatin1Char('0'))},
        {"accToken", m_user_and_perm_info->accToken()},
        {"targetUID", QStringLiteral("%1").arg(currentUser.UID(), 16, 16, QLatin1Char('0'))}
    };

    JRpcPlat::RequestPkg req;
    req.setId( 1000 );
    req.setMethod("RpcPerm.getUser");
    req.setParams(params);
    JRpcPlat::ResponsePkg resp = m_rpc_cli->rpcClient()->invokeRequest( req, false, 6000 );
    if ( !resp.isResult() )
    {
//        qInfo() << "RpcPerm.getUser request failed." << resp.errCode() << resp.errMsg();
        emit m_parent->error(resp.errCode(), resp.errMsg());
        return false;
    }

    user = UserInfoData(resp.data().toObject());

    return true;
}

bool PermBrokerPriv::getUserGrp(const UserGrpListInfo &listInfo, EditableUserGrpInfo &group)
{
    if(m_rpc_cli == Q_NULLPTR){ return false; }
    UserGroupInfoData currentGroup = listInfo.currentUserGrpInfo();
    if ( currentGroup.isNull() ) { return false; }

    QJsonObject parmas = {
        {"UID", QStringLiteral("%1").arg(m_user_and_perm_info->UID(), 16, 16, QLatin1Char('0'))},
        {"accToken", m_user_and_perm_info->accToken()},
        {"targetGID", QStringLiteral("%1").arg(currentGroup.GID(), 16, 16, QLatin1Char('0'))}
    };

    JRpcPlat::RequestPkg req;
    req.setId( 1000 );
    req.setMethod("RpcPerm.getUserGroup");
    req.setParams(parmas);
    JRpcPlat::ResponsePkg resp = m_rpc_cli->rpcClient()->invokeRequest( req, false, 6000 );
    if ( !resp.isResult() )
    {
        emit m_parent->error(resp.errCode(), resp.errMsg());
        return false;
    }

    group.setUserGrpInfo( UserGroupInfoData(resp.data().toObject()) );

    return true;
}

bool PermBrokerPriv::getUserGrp(const UserGrpListInfo &listInfo, UserGroupInfoData &group)
{
    if(m_rpc_cli == Q_NULLPTR){ return false; }
    UserGroupInfoData currentGroup = listInfo.currentUserGrpInfo();
    if ( currentGroup.isNull() ) { return false; }

    QJsonObject parmas = {
        {"UID", QStringLiteral("%1").arg(m_user_and_perm_info->UID(), 16, 16, QLatin1Char('0'))},
        {"accToken", m_user_and_perm_info->accToken()},
        {"targetGID", QStringLiteral("%1").arg(currentGroup.GID(), 16, 16, QLatin1Char('0'))}
    };

    JRpcPlat::RequestPkg req;
    req.setId( 1000 );
    req.setMethod("RpcPerm.getUserGroup");
    req.setParams(parmas);
    JRpcPlat::ResponsePkg resp = m_rpc_cli->rpcClient()->invokeRequest( req, false, 6000 );
    if ( !resp.isResult() )
    {
//        qInfo() << "RpcPerm.getUser request failed." << resp.errCode() << resp.errMsg();
        emit m_parent->error(resp.errCode(), resp.errMsg());
        return false;
    }

//    qInfo() << "RpcPerm.getUser request sucessfull." << resp.data();

    group = UserGroupInfoData(resp.data().toObject());

    return true;
}

bool PermBrokerPriv::diChgUserPwd(EditableUserDiChgPwdInfo &chgPwdInfo)
{
    if(m_rpc_cli == Q_NULLPTR){ return false; }
    QJsonObject parmas = {
        {"name", chgPwdInfo.name()},
        {"oldPwd", chgPwdInfo.oldPwd()},
        {"newPwd", chgPwdInfo.newPwd()}
    };

    JRpcPlat::RequestPkg req;
    req.setId( 1000 );
    req.setMethod("RpcPerm.diChgUserPwd");
    req.setParams(parmas);
    JRpcPlat::ResponsePkg resp = m_rpc_cli->rpcClient()->invokeRequest( req, false, 6000 );
    if ( !resp.isResult() )
    {
        emit m_parent->error(resp.errCode(), resp.errMsg());
        return false;
    }
    emit m_parent->error(0, QObject::tr("user password changed successfully."));
    chgPwdInfo.clear();

    return true;
}

void PermBrokerPriv::quitRpcPerm()
{
    if(m_rpc_cli == Q_NULLPTR){ return; }
    QJsonObject jo;
    JRpcPlat::NoticePkg npkg;
    npkg.setMethod(QStringLiteral("Perm::Method::Disconnect")); npkg.setParams(jo);
    m_rpc_cli->rpcClient()->postNotice(npkg);
}

void PermBrokerPriv::starSvrProcess()
{
    if ( m_svr_pro != Q_NULLPTR ) {
        QString program = QCoreApplication::applicationDirPath() + "/" + TEXT_PermExe;
        if ( !QFile::exists(program) ) {
            return;
        }
        QDir dir(QCoreApplication::applicationDirPath());
        dir.cdUp();
        QStringList arguments;
        arguments << QStringLiteral("--locSvrName") << m_svr_name
                  << QStringLiteral("--dbPath")     << QString("%1/params/syseye-nw/system.dat").arg(dir.path());
        m_svr_pro->start(program, arguments);

    }
}

PermBroker::PermBroker()
{
    m_obj = perm_new( PermBrokerPriv, this );
}

PermBroker::~PermBroker()
{
    this->disconnectToServer();
    perm_delete( m_obj, PermBrokerPriv );
}

static QAtomicPointer<PermBroker>  g_ptr( Q_NULLPTR );
static QMutex       g_locker;
static QAtomicInt   g_ref_cntr(0);

PermBroker * PermBroker::getInstance()
{
    PermBroker *ptr = Q_NULLPTR;

    g_locker.lock();
    ptr = g_ptr.loadAcquire();
    if ( ptr == Q_NULLPTR ) {
        g_ptr.store( ( ptr = perm_new( PermBroker ) ) );
        g_ref_cntr.store(1);
    } else {
        g_ref_cntr.fetchAndAddOrdered(1);
    }
    g_locker.unlock();
    return ptr;
}

void PermBroker::freeInstance()
{
    PermBroker *ptr = Q_NULLPTR;

    g_locker.lock();
    ptr = g_ptr.loadAcquire();
    if ( ptr != Q_NULLPTR ) {
        if ( g_ref_cntr.fetchAndSubOrdered(1) - 1 == 0 ) {
            perm_delete( ptr, PermBroker );
            g_ptr.store( Q_NULLPTR );
        }
    }
    g_locker.unlock();
}

void PermBroker::connectToServer(const QString &svr)
{ T_PrivPtr( m_obj )->connectToServer( svr ); }

void PermBroker::disconnectToServer()
{ T_PrivPtr( m_obj )->disconnectToServer( ); }

UserAndPermInfo *PermBroker::userAndPermInfo() const
{ return T_PrivPtr( m_obj )->userAndPermInfo( ); }

bool PermBroker::login(const QJsonObject &params)
{ return T_PrivPtr( m_obj )->login(params); }

bool PermBroker::logout()
{ return T_PrivPtr( m_obj )->logout( ); }

void PermBroker::getUserList(UserListInfo &listInfo)
{ T_PrivPtr( m_obj )->getUserList(listInfo); }

bool PermBroker::deleteUser(UserListInfo &listInfo)
{ return T_PrivPtr( m_obj )->deleteUser(listInfo); }

bool PermBroker::getSpecUserPermList(const UserListInfo &listInfo, SpecUserPermListInfo &permList)
{ return T_PrivPtr( m_obj )->getSpecUserPermList(listInfo, permList); }

bool PermBroker::getSpecUserGrpList(const UserListInfo &listInfo, SpecUserGrpListInfo &grpList)
{ return T_PrivPtr( m_obj )->getSpecUserGrpList(listInfo, grpList); }

void PermBroker::getUserGrpList(UserGrpListInfo &listInfo)
{ T_PrivPtr( m_obj )->getUserGrpList(listInfo); }

bool PermBroker::deleteUserGrp(UserGrpListInfo &listInfo)
{ return T_PrivPtr( m_obj )->deleteUserGrp(listInfo); }

bool PermBroker::getSpecUserGrpPermList(const UserGrpListInfo &listInfo, SpecUserGrpPermListInfo &permList)
{ return T_PrivPtr( m_obj )->getSpecUserGrpPermList(listInfo, permList); }

bool PermBroker::getSpecUserGrpPermList(const QList<quint64> &gidList, QStringList &permList)
{ return T_PrivPtr( m_obj )->getSpecUserGrpPermList(gidList, permList); }

bool PermBroker::createUser(EditableUserInfo &user, UserListInfo &listInfo)
{ return T_PrivPtr( m_obj )->createUser(user, listInfo); }

bool PermBroker::createUserGrp(EditableUserGrpInfo &group, UserGrpListInfo &grpList)
{ return T_PrivPtr( m_obj )->createUserGrp(group, grpList); }

bool PermBroker::setUser(EditableUserInfo &user, UserListInfo &listInfo)
{ return T_PrivPtr( m_obj )->setUser(user, listInfo); }

bool PermBroker::setUserGrp(EditableUserGrpInfo &group, UserGrpListInfo &grpList)
{ return T_PrivPtr( m_obj )->setUserGrp(group, grpList); }

bool PermBroker::getUser(const UserListInfo &listInfo, EditableUserInfo &eUserInfo)
{ return T_PrivPtr( m_obj )->getUser(listInfo, eUserInfo); }

bool PermBroker::getUser(const UserListInfo &listInfo, UserInfoData &user)
{ return T_PrivPtr( m_obj )->getUser(listInfo, user); }

bool PermBroker::getUserGrp(const UserGrpListInfo &grpList, EditableUserGrpInfo &eGrpInfo)
{ return T_PrivPtr( m_obj )->getUserGrp(grpList, eGrpInfo); }

bool PermBroker::getUserGrp(const UserGrpListInfo &grpList, UserGroupInfoData &group)
{ return T_PrivPtr( m_obj )->getUserGrp(grpList, group); }

bool PermBroker::diChgUserPwd(EditableUserDiChgPwdInfo &chgPwdInfo)
{ return T_PrivPtr( m_obj )->diChgUserPwd(chgPwdInfo); }

void PermBroker::quitRpcPerm()
{ return T_PrivPtr( m_obj )->quitRpcPerm(); }

}

#endif // PERM_PERMBROKER_CXX
