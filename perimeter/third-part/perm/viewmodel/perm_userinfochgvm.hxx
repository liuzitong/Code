#ifndef PERM_USERINFOCHGVM_HXX
#define PERM_USERINFOCHGVM_HXX

#include "perm/common/perm_def.h"
#include "qxpack/indcom/afw/qxpack_ic_viewmodelbase.hxx"

namespace FcPerm {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief 用户信息变更页面容器
 */
// ////////////////////////////////////////////////////////////////////////////
class PERM_API UserInfoChgVm : public QxPack::IcViewModelBase
{
    Q_OBJECT
    Q_PROPERTY(QObject*    editUserInfo READ editUserInfoObj CONSTANT)
    Q_PROPERTY(QJsonObject userAndPwd   READ userAndPwd WRITE setUserAndPwd)
public:
    Q_INVOKABLE explicit UserInfoChgVm( const QVariantList & );
    Q_INVOKABLE virtual ~UserInfoChgVm( ) Q_DECL_OVERRIDE;

    QObject * editUserInfoObj( ) const;

    QJsonObject userAndPwd( ) const;
    void setUserAndPwd( const QJsonObject & );

    Q_INVOKABLE void diChgUserPwd( );

private:
    void *m_obj;
    Q_DISABLE_COPY( UserInfoChgVm )
};

}

#endif // PERM_USERINFOCHGVM_HXX
