#ifndef PERM_EDITABLEUSERDICHGPWDINFO_HXX
#define PERM_EDITABLEUSERDICHGPWDINFO_HXX

#include "perm/common/perm_def.h"
#include "qxpack/indcom/afw/qxpack_ic_svcbase.hxx"

namespace FcPerm {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief 编辑用户直接改变密码
 */
// ////////////////////////////////////////////////////////////////////////////
class PERM_API EditableUserDiChgPwdInfo : public QxPack::IcSvcBase
{
    Q_OBJECT
    Q_PROPERTY(QObject * pwdValidator READ pwdValidatorObj CONSTANT)
    Q_PROPERTY(QString name   READ name   NOTIFY nameChanged)
    Q_PROPERTY(QString oldPwd READ oldPwd NOTIFY oldPwdChanged)
    Q_PROPERTY(QString newPwd READ newPwd NOTIFY newPwdChanged)
public:
    explicit EditableUserDiChgPwdInfo( );
    virtual ~EditableUserDiChgPwdInfo( ) Q_DECL_OVERRIDE;

    QObject * pwdValidatorObj() const;

    QString name  ( ) const;
    Q_SIGNAL void nameChanged( );

    QString oldPwd( ) const;
    Q_SIGNAL void oldPwdChanged( );

    QString newPwd( ) const;
    Q_SIGNAL void newPwdChanged( );

    void setUserAndPwd( const QJsonObject & );
    void clear( );

private:
    void *m_obj;
    Q_DISABLE_COPY( EditableUserDiChgPwdInfo )
};

}

#endif // PERM_EDITABLEUSERDICHGPWDINFO_HXX
