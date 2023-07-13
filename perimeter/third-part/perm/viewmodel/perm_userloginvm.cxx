#ifndef PERM_USERLOGINVM_CXX
#define PERM_USERLOGINVM_CXX

#include "perm_userloginvm.hxx"
#include "perm/service/perm_permbroker.hxx"
#include "perm/service/perm_localpwdmgrsvc.hxx"
#include "perm/service/perm_userandperminfo.hxx"
#include "perm/common/perm_memcntr.hxx"
#include "perm/common/perm_guns.hxx"
#include "../../base/common/perimeter_guns.hxx"
#include "qxpack/indcom/afw/qxpack_ic_appctrlbase.hxx"
//#include "main/model/main_rpcvkbd.hxx"

namespace FcPerm {

#define T_PrivPtr( o )  perm_objcast( UserLoginVmPriv*, o )
class PERM_HIDDEN UserLoginVmPriv
{
private:
    UserLoginVm   * m_parent;
    bool            m_isLogin;
    QJsonObject     m_userLoginData;
    LocalPwdMgrSvc* m_localPwdMgrSvc;
    PermBroker    * m_permBroker;

protected:
    void   requestMessage( const QJsonObject & );

public:
    explicit UserLoginVmPriv ( UserLoginVm *pa );
    ~UserLoginVmPriv( );

    bool  enableOfVkb( ) const;
    void  setEnableOfVkb( bool );
    inline QObject * localUserObj( ) { return m_localPwdMgrSvc; }
    inline QJsonObject & userLoginData( ) { return m_userLoginData; }

    void online(bool is_on);

    inline void login( );
    inline void logout( );
    void quitLogin();
};

void UserLoginVmPriv::requestMessage(const QJsonObject &param)
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

UserLoginVmPriv::UserLoginVmPriv(UserLoginVm *pa)
{
    m_parent = pa; m_isLogin  = false;
    m_localPwdMgrSvc = LocalPwdMgrSvc::getInstance();
    m_permBroker = PermBroker::getInstance();
}

UserLoginVmPriv::~UserLoginVmPriv()
{
    LocalPwdMgrSvc::release();
    PermBroker::freeInstance();
}

bool UserLoginVmPriv::enableOfVkb() const
{
    //return FcMain::RpcVkbd::globalInstance().enabled();
    return true;
}

void UserLoginVmPriv::setEnableOfVkb(bool sw)
{
    //FcMain::RpcVkbd::globalInstance().setEnabled(sw);
}

void UserLoginVmPriv::online(bool is_on)
{
    m_permBroker->connectToServer(GUNS_JRPC_SVC_NAME);
}

void UserLoginVmPriv::login()
{
    if ( m_userLoginData.isEmpty() ) { return; }

    QString name = m_userLoginData.value("name").toString().trimmed();
    QString pwd  = m_userLoginData.value("pwd").toString().trimmed();
    int type = m_userLoginData.value("type").toInt();

    if ( type == UserLoginVm::MAIN_LOGIN )
    {
        if ( name == m_localPwdMgrSvc->name() &&
             pwd.isEmpty() )
        {
            pwd = m_localPwdMgrSvc->pwd();
        }
    }

    if ( m_permBroker->login({{"name", name}, {"pwd", pwd}}) )
    {
        m_isLogin = true;
        if ( type == UserLoginVm::MAIN_LOGIN )
        {
            if ( m_permBroker->userAndPermInfo()->actualPermList().contains("Perm.root")  ||
                 m_permBroker->userAndPermInfo()->actualPermList().contains("Perm.sys")   ||
                 m_permBroker->userAndPermInfo()->actualPermList().contains("Perm.admin") ||
                 m_permBroker->userAndPermInfo()->actualPermList().contains("Perm.manage") )
            {
//                m_app_set_svc->setQuitCode(0);
            }else{
//                m_app_set_svc->setQuitCode(1);
            }
            m_localPwdMgrSvc->loginTo(name, pwd);
            emit m_parent->showNavMainView();
        }

        else if ( type == UserLoginVm::MGR_LOGIN )
        {
            if ( m_permBroker->userAndPermInfo()->actualPermList().contains("Perm.root")  ||
                 m_permBroker->userAndPermInfo()->actualPermList().contains("Perm.sys")   ||
                 m_permBroker->userAndPermInfo()->actualPermList().contains("Perm.admin") ||
                 m_permBroker->userAndPermInfo()->actualPermList().contains("Perm.manage") )
            {
                emit m_parent->showUserBrowseView();
            }
            else
            {
                requestMessage({{"code", 32041}, {"msg", QObject::tr("No permission to perform this operation.")}});
            }
        }
    }
}

void UserLoginVmPriv::logout()
{
    if ( m_isLogin )
    { if ( m_permBroker->logout() ) { m_isLogin = false; } }
}

void UserLoginVmPriv::quitLogin()
{
    m_permBroker->quitRpcPerm();
}

UserLoginVm::UserLoginVm(const QVariantList &)
{
    m_obj = perm_new( UserLoginVmPriv, this );
}

UserLoginVm::~UserLoginVm()
{
    perm_delete( m_obj, UserLoginVmPriv );
}

QObject *UserLoginVm::localUserObj() const
{ return T_PrivPtr( m_obj )->localUserObj(); }

void UserLoginVm::online(bool is_on)
{
    T_PrivPtr( m_obj )->online( is_on );
}

QJsonObject UserLoginVm::userLoginData() const
{ return T_PrivPtr( m_obj )->userLoginData(); }

void UserLoginVm::setUserLoginData(const QJsonObject &jo)
{ T_PrivPtr( m_obj )->userLoginData() = jo; }

bool UserLoginVm::enableOfVkb() const
{ return T_PrivPtr( m_obj )->enableOfVkb(); }

void UserLoginVm::setEnableOfVkb(bool sw)
{ T_PrivPtr( m_obj )->setEnableOfVkb(sw); }

void UserLoginVm::login()
{ T_PrivPtr( m_obj )->login(); }

void UserLoginVm::logout()
{ T_PrivPtr( m_obj )->logout(); }

void UserLoginVm::quitLogin()
{
    T_PrivPtr( m_obj )->quitLogin();
}

}

#endif // PERM_USERLOGINVM_CXX
