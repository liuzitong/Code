#ifndef PERM_USERGRPMGRVM_CXX
#define PERM_USERGRPMGRVM_CXX

#include "perm_usergrpmgrvm.hxx"
#include "perm/service/perm_permbroker.hxx"
#include "perm/service/perm_specusergrppermlistinfo.hxx"
#include "perm/model/perm_usergrplistinfo.hxx"
#include "perm/model/perm_usergroupinfodata.hxx"
#include "perm/service/perm_userandperminfo.hxx"
#include "perm/model/perm_typedef.hxx"
#include "perm/common/perm_memcntr.hxx"
#include "perm/common/perm_guns.hxx"
//#include "base/common/ce_guns.hxx"
#include "../../base/common/perimeter_guns.hxx"
#include "qxpack/indcom/afw/qxpack_ic_appctrlbase.hxx"
#include "qxpack/indcom/afw/qxpack_ic_confirmbus.hxx"
#include <QTimer>

namespace FcPerm {

#define T_PrivPtr( o )  perm_objcast( UserGrpMgrVmPriv*, o )
class PERM_HIDDEN UserGrpMgrVmPriv
{
private:
    UserGrpMgrVm   * m_parent;
    PermBroker     * m_permBroker;
    UserGrpListInfo* m_userGrpListInfo;
    SpecUserGrpPermListInfo m_specUserGrpPermListInfo;
    bool           m_canBrowseGroup;
    bool           m_canCreateGroup;
    bool           m_canModifyGroup;
    bool           m_canRemoveGroup;

protected:
    void   requestMessage( const QJsonObject & );

public:
    explicit UserGrpMgrVmPriv ( UserGrpMgrVm *pa );
    ~UserGrpMgrVmPriv( );

    inline QObject* userGrpListInfoObj ( ) { return m_userGrpListInfo; }
    inline QObject* specUserGrpPermListInfoObj ( ) { return & m_specUserGrpPermListInfo; }
    inline bool     canCreateGroup      ( ) { return m_canCreateGroup; }
    inline bool     canModifyGroup      ( ) { return m_canModifyGroup; }
    inline bool     canRemoveGroup      ( ) { return m_canRemoveGroup; }

    void   removeGrp( ) { m_permBroker->deleteUserGrp( *m_userGrpListInfo ); }
    void   activeGrpPermList( );
    void   checkCreate( );
    void   checkModify( );
    void   checkRemove( );
};

void UserGrpMgrVmPriv::requestMessage(const QJsonObject &param)
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

UserGrpMgrVmPriv::UserGrpMgrVmPriv(UserGrpMgrVm *pa)
{
    m_parent = pa;
    m_userGrpListInfo = UserGrpListInfo::getInstance();
    m_permBroker = PermBroker::getInstance();
    m_canBrowseGroup = m_permBroker->userAndPermInfo()->actualPermList().contains("Perm.userGroup.browse");
    m_canCreateGroup = m_permBroker->userAndPermInfo()->actualPermList().contains("Perm.userGroup.create");
    m_canModifyGroup = m_permBroker->userAndPermInfo()->actualPermList().contains("Perm.userGroup.modify");
    m_canRemoveGroup = m_permBroker->userAndPermInfo()->actualPermList().contains("Perm.userGroup.delete");

    if ( m_canBrowseGroup )
    { m_permBroker->getUserGrpList( *m_userGrpListInfo ); }
    else
    { requestMessage({{"code", -1}, {"msg", QObject::tr("The user does not have user group browsing permissions. Please contact the administrator.")}}); }
}

UserGrpMgrVmPriv::~UserGrpMgrVmPriv()
{
    UserGrpListInfo::release();
    PermBroker::freeInstance();
}

void UserGrpMgrVmPriv::activeGrpPermList()
{
    if ( m_permBroker->getSpecUserGrpPermList( *m_userGrpListInfo, m_specUserGrpPermListInfo ) )
    {
        QTimer::singleShot( 200, [=](){ emit m_parent->showGrpPermList(); });
    }
}

void UserGrpMgrVmPriv::checkCreate()
{
    if ( !m_canCreateGroup )
    {
        // 登录用户没有用户修改权限
        requestMessage({{"code", -1}, {"msg", QObject::tr("The user does not have user group creation permissions. Please contact the administrator.")}});
        return;
    }

    emit m_parent->showNewUserGrpView();
}

void UserGrpMgrVmPriv::checkModify()
{
    if ( !m_canModifyGroup )
    {
        // 登录用户没有用户修改权限
        requestMessage({{"code", -1}, {"msg", QObject::tr("The user does not have user group modification permissions. Please contact the administrator.")}});
        return;
    }

    UserGroupInfoData group;
    if ( m_permBroker->getUserGrp(*m_userGrpListInfo, group) )
    {
        if ( m_permBroker->userAndPermInfo()->role() <= group.role() )
        {
            // 登录用户和被修改用户角色相同
            requestMessage({{"code", -1}, {"msg", QObject::tr("Insufficient permissions to be modified.")}});
            return;
        }
    }

    emit m_parent->showModUserGrpView();
}

void UserGrpMgrVmPriv::checkRemove()
{
    if ( !m_canRemoveGroup )
    {
        // 登录用户没有用户修改权限
        requestMessage({{"code", -1}, {"msg", QObject::tr("The user does not have user group remove permissions. Please contact the administrator.")}});
        return;
    }

    UserGroupInfoData group;
    if ( m_permBroker->getUserGrp(*m_userGrpListInfo, group) )
    {
        if ( m_permBroker->userAndPermInfo()->role() <= group.role() )
        {
            // 登录用户和被修改用户角色相同
            requestMessage({{"code", -1}, {"msg", QObject::tr("Insufficient permissions to be removed.")}});
            return;
        }
    }

    QxPack::IcAppCtrlBase *app_ctrl = QxPack::IcAppCtrlBase::instance( GUNS_AppCtrl );
    QxPack::IcConfirmBus *cfm_bus = app_ctrl->cfmBus(); // [HINT] just get the confirm bus

    QxPack::IcConfirmBusPkg cfm_pkg;
    cfm_pkg.setGroupName( GUNS_DefaultMethodSvcCfmGroup );
    cfm_pkg.setMessage( QObject::tr("Whether to remove the current user group?") );

    cfm_bus->reqConfirm( cfm_pkg );

    if ( cfm_pkg.isGotResult()) {
        QJsonObject jo_rsl = cfm_pkg.result();
        if ( jo_rsl.value("result").toString().toLower() == QStringLiteral("yes")) {
            this->removeGrp();
        }
    }
}

UserGrpMgrVm::UserGrpMgrVm(const QVariantList &)
{
    m_obj = perm_new( UserGrpMgrVmPriv, this );
}

UserGrpMgrVm::~UserGrpMgrVm()
{
    perm_delete( m_obj, UserGrpMgrVmPriv );
}

QObject *UserGrpMgrVm::userGrpListInfoObj() const
{ return T_PrivPtr( m_obj )->userGrpListInfoObj(); }

bool UserGrpMgrVm::canCreateGroup() const
{ return T_PrivPtr( m_obj )->canCreateGroup(); }

bool UserGrpMgrVm::canModifyGroup() const
{ return T_PrivPtr( m_obj )->canModifyGroup(); }

bool UserGrpMgrVm::canRemoveGroup() const
{ return T_PrivPtr( m_obj )->canRemoveGroup(); }

QObject *UserGrpMgrVm::specUserGrpPermListInfoObj() const
{ return T_PrivPtr( m_obj )->specUserGrpPermListInfoObj(); }

void UserGrpMgrVm::activeGrpPermList()
{ T_PrivPtr( m_obj )->activeGrpPermList(); }

void UserGrpMgrVm::checkCreate()
{ T_PrivPtr( m_obj )->checkCreate(); }

void UserGrpMgrVm::checkModify()
{ T_PrivPtr( m_obj )->checkModify(); }

void UserGrpMgrVm::checkRemove()
{ T_PrivPtr( m_obj )->checkRemove(); }

}

#endif // PERM_USERGRPMGRVM_CXX
