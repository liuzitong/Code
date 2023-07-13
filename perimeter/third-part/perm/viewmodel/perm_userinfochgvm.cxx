#ifndef PERM_USERINFOCHGVM_CXX
#define PERM_USERINFOCHGVM_CXX

#include "perm_userinfochgvm.hxx"
#include "perm/service/perm_permbroker.hxx"
#include "perm/service/perm_editableuserdichgpwdinfo.hxx"
#include "perm/service/perm_aesencryption.hxx"
#include "perm/common/perm_memcntr.hxx"
#include "perm/common/perm_guns.hxx"
#include "../../base/common/perimeter_guns.hxx"
#include "qxpack/indcom/afw/qxpack_ic_appctrlbase.hxx"
#include "qxpack/indcom/afw/qxpack_ic_msgbus.hxx"

namespace FcPerm {

#define T_PrivPtr( o )  perm_objcast( UserInfoChgVmPriv*, o )
class PERM_HIDDEN UserInfoChgVmPriv
{
private:
    UserInfoChgVm   * m_parent;
    PermBroker      * m_permBroker;
    EditableUserDiChgPwdInfo m_editUserInfo;

protected:
    void   modifyLocalLoginUser( const QString & name, const QString & pwd );

public:
    explicit UserInfoChgVmPriv ( UserInfoChgVm *pa );
    ~UserInfoChgVmPriv( );

    inline QObject * editUserInfoObj( ) { return &m_editUserInfo; }
    inline void setUserAndPwd( const QJsonObject & jo ) { m_editUserInfo.setUserAndPwd(jo); }
    inline void diChgUserPwd( );
};

void UserInfoChgVmPriv::modifyLocalLoginUser(const QString &name, const QString &pwd)
{
    QString name_str = AESEncryption::encode(name.toUtf8()).toBase64();
    QString pwd_str = AESEncryption::encode(pwd.toUtf8()).toBase64();

    QJsonObject param = {{"name", name_str}, {"pwd", pwd_str}, {"method", "modifyLocalLogin"}};

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

UserInfoChgVmPriv::UserInfoChgVmPriv(UserInfoChgVm *pa)
{
    m_parent = pa;
    m_permBroker = PermBroker::getInstance();
}

UserInfoChgVmPriv::~UserInfoChgVmPriv()
{
    PermBroker::freeInstance();
}

void UserInfoChgVmPriv::diChgUserPwd()
{
    QString name = m_editUserInfo.name();
    QString pwd  = m_editUserInfo.newPwd();
    if ( m_permBroker->diChgUserPwd( m_editUserInfo ) )
    {
        modifyLocalLoginUser( name, pwd );
    }
}

UserInfoChgVm::UserInfoChgVm(const QVariantList &)
{
    m_obj = perm_new( UserInfoChgVmPriv, this );
}

UserInfoChgVm::~UserInfoChgVm()
{
    perm_delete( m_obj, UserInfoChgVmPriv );
}

QObject *UserInfoChgVm::editUserInfoObj() const
{ return T_PrivPtr( m_obj )->editUserInfoObj(); }

QJsonObject UserInfoChgVm::userAndPwd() const
{ return QJsonObject(); }

void UserInfoChgVm::setUserAndPwd(const QJsonObject &jo)
{ T_PrivPtr( m_obj )->setUserAndPwd(jo); }

void UserInfoChgVm::diChgUserPwd()
{ T_PrivPtr( m_obj )->diChgUserPwd(); }

}

#endif // PERM_USERINFOCHGVM_CXX
