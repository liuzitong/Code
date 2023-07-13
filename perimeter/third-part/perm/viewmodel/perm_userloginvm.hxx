#ifndef PERM_USERLOGINVM_HXX
#define PERM_USERLOGINVM_HXX

#include "perm/common/perm_def.h"
#include "qxpack/indcom/afw/qxpack_ic_viewmodelbase.hxx"

namespace FcPerm {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief 用户登录页面容器
 */
// ////////////////////////////////////////////////////////////////////////////
class PERM_API UserLoginVm : public QxPack::IcViewModelBase
{
    Q_OBJECT
    Q_PROPERTY(QObject * localUser READ localUserObj CONSTANT)
    Q_PROPERTY(QJsonObject userLoginData READ userLoginData WRITE setUserLoginData NOTIFY userLoginDataChanged)
    Q_PROPERTY(bool enableOfVkb READ enableOfVkb WRITE setEnableOfVkb NOTIFY enableOfVkbChanged)
    Q_ENUMS(LoginType)
public:
    enum LoginType { MAIN_LOGIN, MGR_LOGIN };

    Q_INVOKABLE explicit UserLoginVm( const QVariantList & );
    Q_INVOKABLE virtual ~UserLoginVm( ) Q_DECL_OVERRIDE;

    QObject * localUserObj( ) const;
    Q_INVOKABLE void online( bool is_on );

    QJsonObject userLoginData( ) const;
    void setUserLoginData( const QJsonObject & );
    Q_SIGNAL void userLoginDataChanged( );

    bool  enableOfVkb( ) const;
    void  setEnableOfVkb( bool );
    Q_SIGNAL void enableOfVkbChanged( );

    Q_INVOKABLE void login( );
    Q_INVOKABLE void logout( );
    Q_INVOKABLE void quitLogin();

    Q_SIGNAL void showNavMainView( );
    Q_SIGNAL void showUserBrowseView( );

private:
    void *m_obj;
    Q_DISABLE_COPY( UserLoginVm )
};

}

#endif // PERM_USERLOGINVM_HXX
