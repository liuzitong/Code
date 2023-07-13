#ifndef PERM_USERMGRVM_CXX
#define PERM_USERMGRVM_CXX

#include "perm_usermgrvm.hxx"
#include "perm/service/perm_permbroker.hxx"
#include "perm/service/perm_specusergrplistinfo.hxx"
#include "perm/service/perm_specuserpermlistinfo.hxx"
#include "perm/model/perm_userlistinfo.hxx"
#include "perm/service/perm_localpwdmgrsvc.hxx"
#include "perm/service/perm_aesencryption.hxx"
#include "perm/model/perm_userinfodata.hxx"
#include "perm/service/perm_userandperminfo.hxx"
#include "perm/model/perm_typedef.hxx"
#include "perm/common/perm_memcntr.hxx"
#include "perm/common/perm_guns.hxx"
#include "../../base/common/perimeter_guns.hxx"
#include "qxpack/indcom/afw/qxpack_ic_appctrlbase.hxx"
#include "qxpack/indcom/afw/qxpack_ic_msgbus.hxx"
#include "qxpack/indcom/afw/qxpack_ic_confirmbus.hxx"

#include <QTimer>

namespace FcPerm {

#define T_PrivPtr( o )  perm_objcast( UserMgrVmPriv*, o )
class PERM_HIDDEN UserMgrVmPriv
{
private:
    UserMgrVm    * m_parent;
    UserListInfo * m_userListInfo;
    PermBroker   * m_permBroker;
    LocalPwdMgrSvc * m_localPwdMgrSvc;
    SpecUserGrpListInfo  m_specUserGrpListInfo;
    SpecUserPermListInfo m_specUserPermListInfo;
    bool           m_canAutoLoginUser;
    bool           m_canBrowseUser;
    bool           m_canCreateUser;
    bool           m_canModifyUser;
    bool           m_canRemoveUser;

protected:
    void   setLocalLoginUser( const UserInfoData & );
    void   deleteLocalLoginUser( const UserInfoData & );
    void   requestMessage( const QJsonObject & );

public:
    explicit UserMgrVmPriv ( UserMgrVm * );
    ~UserMgrVmPriv( );

    inline QObject* userListInfoObj ( ) { return m_userListInfo; }
    inline bool     canAutoLoginUser( ) { return m_canAutoLoginUser; }
    inline bool     canCreateUser   ( ) { return m_canCreateUser; }
    inline bool     canModifyUser   ( ) { return m_canModifyUser; }
    inline bool     canRemoveUser   ( ) { return m_canRemoveUser; }
    inline QObject* specUserGrpListInfoObj ( ) { return & m_specUserGrpListInfo; }
    inline QObject* specUserPermListInfoObj( ) { return & m_specUserPermListInfo; }
    inline QObject* localUserObj    ( ) { return m_localPwdMgrSvc; }

    void   setAutoLogin( );
    void   cancelAutoLogin( );
    void   removeUser( );
    void   activeGroupList( );
    void   activePermList( );
    void   checkCreate( );
    void   checkModify( );
    void   checkRemove( );
};

void UserMgrVmPriv::setLocalLoginUser(const UserInfoData &user)
{
    QString uid = AESEncryption::encode(QString("%1").arg(user.UID(), 16, 16, QChar('0')).toUtf8()).toBase64();
    QString name = AESEncryption::encode(user.name().toUtf8()).toBase64();
    QString pwd = AESEncryption::encode(user.pwd().toUtf8()).toBase64();

    QJsonObject param = {{"UID", uid}, {"name", name}, {"pwd", pwd}, {"method", "setupLocalLogin"}};

    // [HINT] the application controller can get by name ( name is defined in
    // the user application. normally user should added a base/common/projname_guns.h
    // the GUNS_AppCtrl is there
    QxPack::IcAppCtrlBase *app_ctrl = QxPack::IcAppCtrlBase::instance( GUNS_AppCtrl );
    QxPack::IcMsgBus *msg_bus = app_ctrl->msgBus();

    // [HINT] the message package is a copy on write class.
    QxPack::IcMsgBusPkg  pkg;
    pkg.setGroupName( GUNS_LocalPwdMgrSvcMsgGroup );  // Hint: must setup the msg group name
    pkg.setJoParam( param );

    // [HINT] use this method to post the message.
    // after posted, the 'pkg' is attached by MsgBus inner, here can destruct it.
    msg_bus->post( pkg );
}

void UserMgrVmPriv::deleteLocalLoginUser(const UserInfoData &user)
{
    QString uid = AESEncryption::encode(QString("%1").arg(user.UID(), 16, 16, QChar('0')).toUtf8()).toBase64();
    QString name = AESEncryption::encode(user.name().toUtf8()).toBase64();
    QString pwd = AESEncryption::encode(user.pwd().toUtf8()).toBase64();

    QJsonObject param = {{"UID", uid}, {"name", name}, {"pwd", pwd}, {"method", "removeLocalLogin"}};

    // [HINT] the application controller can get by name ( name is defined in
    // the user application. normally user should added a base/common/projname_guns.h
    // the GUNS_AppCtrl is there
    QxPack::IcAppCtrlBase *app_ctrl = QxPack::IcAppCtrlBase::instance( GUNS_AppCtrl );
    QxPack::IcMsgBus *msg_bus = app_ctrl->msgBus();

    // [HINT] the message package is a copy on write class.
    QxPack::IcMsgBusPkg  pkg;
    pkg.setGroupName( GUNS_LocalPwdMgrSvcMsgGroup );  // Hint: must setup the msg group name
    pkg.setJoParam( param );

    // [HINT] use this method to post the message.
    // after posted, the 'pkg' is attached by MsgBus inner, here can destruct it.
    msg_bus->post( pkg );
}

void UserMgrVmPriv::requestMessage(const QJsonObject &param)
{
    // [HINT] the application controller can get by name ( name is defined in
    // the user application. normally user should added a base/common/projname_guns.h
    // the GUNS_AppCtrl is there
    QxPack::IcAppCtrlBase *app_ctrl = QxPack::IcAppCtrlBase::instance( GUNS_AppCtrl );
    QxPack::IcMsgBus *msg_bus = app_ctrl->msgBus();

    // [HINT] the message package is a copy on write class.
    QxPack::IcMsgBusPkg  pkg;
    pkg.setGroupName( GUNS_DefaultMethodSvcMsgGroup );  // Hint: must setup the msg group name
    pkg.setJoParam( param );

    // [HINT] use this method to post the message.
    // after posted, the 'pkg' is attached by MsgBus inner, here can destruct it.
    msg_bus->post( pkg );
}

UserMgrVmPriv::UserMgrVmPriv(UserMgrVm *pa)
{
    m_parent = pa;
    m_userListInfo = UserListInfo::getInstance();
    m_permBroker   = PermBroker::getInstance();
    m_localPwdMgrSvc = LocalPwdMgrSvc::getInstance();
    m_canAutoLoginUser = m_permBroker->userAndPermInfo()->actualPermList().contains("Perm.admin") ||
    m_permBroker->userAndPermInfo()->actualPermList().contains("Perm.root");
    m_canBrowseUser = m_permBroker->userAndPermInfo()->actualPermList().contains("Perm.user.browse");
    m_canCreateUser = m_permBroker->userAndPermInfo()->actualPermList().contains("Perm.user.create");
    m_canModifyUser = m_permBroker->userAndPermInfo()->actualPermList().contains("Perm.user.modify");
    m_canRemoveUser = m_permBroker->userAndPermInfo()->actualPermList().contains("Perm.user.delete");

    if ( m_canBrowseUser )
    { m_permBroker->getUserList( *m_userListInfo ); }
    else
    { requestMessage({{"code", -1}, {"msg", QObject::tr("The user does not have user browsing permissions. Please contact the administrator.")}}); }
}

UserMgrVmPriv::~UserMgrVmPriv()
{
    UserListInfo::release();
    PermBroker::freeInstance();
    LocalPwdMgrSvc::release();
}

void UserMgrVmPriv::setAutoLogin()
{
    UserInfoData user_info = m_userListInfo->currentUserInfo();
    setLocalLoginUser( user_info );
}

void UserMgrVmPriv::cancelAutoLogin()
{
    UserInfoData user_info = m_userListInfo->currentUserInfo();
    deleteLocalLoginUser( user_info );
}

void UserMgrVmPriv::removeUser()
{
    UserInfoData user_info = m_userListInfo->currentUserInfo();
    deleteLocalLoginUser( user_info );
    m_permBroker->deleteUser( *m_userListInfo );
}

void UserMgrVmPriv::activeGroupList()
{
    if ( m_permBroker->getSpecUserGrpList( *m_userListInfo, m_specUserGrpListInfo ) )
    {
        QTimer::singleShot( 200, [=](){ emit m_parent->showGroupList(); });
    }
}

void UserMgrVmPriv::activePermList()
{
    if ( m_permBroker->getSpecUserPermList( *m_userListInfo, m_specUserPermListInfo ) )
    {
        QTimer::singleShot( 200, [=](){ emit m_parent->showPermList(); });
    }
}

void UserMgrVmPriv::checkCreate()
{
    if ( !m_canCreateUser )
    {
        // 登录用户没有用户修改权限
        requestMessage({{"code", -1}, {"msg", QObject::tr("The user does not have user creation permissions. Please contact the administrator.")}});
        return;
    }

    emit m_parent->showNewUserView();
}

void UserMgrVmPriv::checkModify()
{
    if ( !m_canModifyUser )
    {
        // 登录用户没有用户修改权限
        requestMessage({{"code", -1}, {"msg", QObject::tr("The user does not have user modification permissions. Please contact the administrator.")}});
        return;
    }

    UserInfoData user;
    if ( m_permBroker->getUser( *m_userListInfo, user ) )
    {
        if ( m_permBroker->userAndPermInfo()->role() <= user.role() )
        {
            // 登录用户和被修改用户角色相同
            requestMessage({{"code", -1}, {"msg", QObject::tr("Insufficient permissions to be modified.")}});
            return;
        }
    }

    emit m_parent->showModUserView();
}

void UserMgrVmPriv::checkRemove()
{
    if ( !m_canRemoveUser )
    {
        // 登录用户没有用户修改权限
        requestMessage({{"code", -1}, {"msg", QObject::tr("The user does not have user remove permissions. Please contact the administrator.")}});
        return;
    }

    UserInfoData user;
    if ( m_permBroker->getUser( *m_userListInfo, user ) )
    {
        if ( m_permBroker->userAndPermInfo()->role() <= user.role() )
        {
            // 登录用户和被修改用户角色相同
            requestMessage({{"code", -1}, {"msg", QObject::tr("Insufficient permissions to be removed.")}});
            return;
        }
    }

    // 发送确认删除当前用户请求
    QxPack::IcAppCtrlBase *app_ctrl = QxPack::IcAppCtrlBase::instance( GUNS_AppCtrl );
    QxPack::IcConfirmBus *cfm_bus = app_ctrl->cfmBus(); // [HINT] just get the confirm bus

    QxPack::IcConfirmBusPkg cfm_pkg;
    cfm_pkg.setGroupName( GUNS_DefaultMethodSvcCfmGroup );
    cfm_pkg.setMessage( QObject::tr("Whether to remove the current user?") );

    cfm_bus->reqConfirm( cfm_pkg );

    if ( cfm_pkg.isGotResult()) {
        QJsonObject jo_rsl = cfm_pkg.result();
        if ( jo_rsl.value("result").toString().toLower() == QStringLiteral("yes")) {
            // 确认删除
            this->removeUser();
        }
    }
}

UserMgrVm::UserMgrVm(const QVariantList &)
{
    m_obj = perm_new( UserMgrVmPriv, this );
}

UserMgrVm::~UserMgrVm()
{
    perm_delete( m_obj, UserMgrVmPriv );
}

QObject *UserMgrVm::userListInfoObj() const
{ return T_PrivPtr( m_obj )->userListInfoObj(); }

bool UserMgrVm::canAutoLoginUser() const
{ return T_PrivPtr( m_obj )->canAutoLoginUser(); }

bool UserMgrVm::canCreateUser() const
{ return T_PrivPtr( m_obj )->canCreateUser(); }

bool UserMgrVm::canModifyUser() const
{ return T_PrivPtr( m_obj )->canModifyUser(); }

bool UserMgrVm::canRemoveUser() const
{ return T_PrivPtr( m_obj )->canRemoveUser(); }

QObject *UserMgrVm::specUserGrpListInfoObj() const
{ return T_PrivPtr( m_obj )->specUserGrpListInfoObj(); }

QObject *UserMgrVm::specUserPermListInfoObj() const
{ return T_PrivPtr( m_obj )->specUserPermListInfoObj(); }

QObject *UserMgrVm::localUserObj() const
{ return T_PrivPtr( m_obj )->localUserObj(); }

void UserMgrVm::setAutoLogin()
{ T_PrivPtr( m_obj )->setAutoLogin(); }

void UserMgrVm::cancelAutoLogin()
{ T_PrivPtr( m_obj )->cancelAutoLogin(); }

void UserMgrVm::activeGroupList()
{ T_PrivPtr( m_obj )->activeGroupList(); }

void UserMgrVm::activePermList()
{ T_PrivPtr( m_obj )->activePermList(); }

void UserMgrVm::checkCreate()
{ T_PrivPtr( m_obj )->checkCreate(); }

void UserMgrVm::checkModify()
{ T_PrivPtr( m_obj )->checkModify(); }

void UserMgrVm::checkRemove()
{ T_PrivPtr( m_obj )->checkRemove(); }

}

#endif // PERM_USERMGRVM_CXX
