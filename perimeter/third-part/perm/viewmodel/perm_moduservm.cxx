#ifndef PERM_MODUSERVM_CXX
#define PERM_MODUSERVM_CXX

#include "perm_moduservm.hxx"
#include "perm/service/perm_permbroker.hxx"
#include "perm/service/perm_editableuserinfo.hxx"
#include "perm/model/perm_userlistinfo.hxx"
#include "perm/service/perm_aesencryption.hxx"
#include "perm/model/perm_userinfodata.hxx"
#include "perm/common/perm_memcntr.hxx"
#include "perm/common/perm_guns.hxx"
#include "../../base/common/perimeter_guns.hxx"
#include "qxpack/indcom/afw/qxpack_ic_appctrlbase.hxx"
#include "qxpack/indcom/afw/qxpack_ic_msgbus.hxx"
#include <QTimer>

namespace FcPerm {

#define T_PrivPtr( o )  perm_objcast( ModUserVmPriv*, o )
class PERM_HIDDEN ModUserVmPriv
{
private:
    ModUserVm   * m_parent;
    PermBroker  * m_permBroker;
    EditableUserInfo * m_editUserInfo;
    UserListInfo* m_userListInfo;

protected:
    void   modifyLocalLoginUser( const UserInfoData & );

public:
    explicit ModUserVmPriv ( ModUserVm *pa );
    ~ModUserVmPriv( );

    inline QObject * editUserInfoObj( ) { return m_editUserInfo; }
    void  setUserInfo( const QJsonObject & jo );
    void  save( );
};

void ModUserVmPriv::modifyLocalLoginUser(const UserInfoData &user)
{
    QString uid = AESEncryption::encode(QString("%1").arg(user.UID(), 16, 16, QChar('0')).toUtf8()).toBase64();
    QString name = AESEncryption::encode(user.name().toUtf8()).toBase64();
    QString pwd = AESEncryption::encode(user.pwd().toUtf8()).toBase64();

    QJsonObject param = {{"UID", uid}, {"name", name}, {"pwd", pwd}, {"method", "modifyLocalLogin"}};

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

ModUserVmPriv::ModUserVmPriv(ModUserVm *pa)
{
    m_parent = pa;
    m_permBroker   = PermBroker::getInstance();
    m_userListInfo = UserListInfo::getInstance();
    m_editUserInfo = EditableUserInfo::getInstance();

    m_permBroker->getUser( *m_userListInfo, *m_editUserInfo );
}

ModUserVmPriv::~ModUserVmPriv()
{
    PermBroker::freeInstance();
    UserListInfo::release();
    EditableUserInfo::release();
}

void ModUserVmPriv::setUserInfo(const QJsonObject &jo)
{
    m_editUserInfo->setUserInfo(jo);
}

void ModUserVmPriv::save()
{
    UserInfoData user_info = m_editUserInfo->userInfo();
    modifyLocalLoginUser( user_info );

    m_permBroker->setUser( *m_editUserInfo, *m_userListInfo );
}

ModUserVm::ModUserVm(const QVariantList &)
{
    m_obj = perm_new( ModUserVmPriv, this );
}

ModUserVm::~ModUserVm()
{
    perm_delete( m_obj, ModUserVmPriv );
}

QObject *ModUserVm::editUserInfoObj() const
{ return T_PrivPtr( m_obj )->editUserInfoObj(); }

QJsonObject ModUserVm::userInfo() const
{ return QJsonObject(); }

void ModUserVm::setUserInfo(const QJsonObject &jo)
{ T_PrivPtr( m_obj )->setUserInfo(jo); }

void ModUserVm::editUserGroup()
{ QTimer::singleShot(200, [this](){ emit this->showUserGrpListView(); }); }

void ModUserVm::editUserPerm()
{ QTimer::singleShot(200, [this](){ emit this->showUserPermListView(); }); }

void ModUserVm::save()
{ T_PrivPtr( m_obj )->save(); }

}

#endif // PERM_MODUSERVM_CXX
