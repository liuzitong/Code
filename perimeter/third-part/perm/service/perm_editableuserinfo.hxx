#ifndef PERM_EDITABLEUSERINFO_HXX
#define PERM_EDITABLEUSERINFO_HXX

#include "perm/common/perm_def.h"
#include "qxpack/indcom/afw/qxpack_ic_svcbase.hxx"

namespace FcPerm {

class UserInfoData;
// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief 编辑用户信息
 */
// ////////////////////////////////////////////////////////////////////////////
class PERM_API EditableUserInfo : public QxPack::IcSvcBase
{
    Q_OBJECT
    Q_PROPERTY(QString userName READ name NOTIFY userChanged)
    Q_PROPERTY(QString userPwd  READ pwd  NOTIFY userChanged)
    Q_PROPERTY(QString userMemo READ memo NOTIFY userChanged)
public:
    static EditableUserInfo * getInstance( );
    static void release( );

    QString name  ( ) const;
    QString pwd   ( ) const;
    QString memo( ) const;

    UserInfoData userInfo( ) const;
    void setUserInfo( const UserInfoData & );
    void setUserInfo( const QJsonObject & );
    void setGroup( const QJsonObject & );
    void setPerm( const QJsonObject & );

    Q_INVOKABLE void clear( );

    Q_SIGNAL void userChanged();

private:
    Q_INVOKABLE explicit EditableUserInfo( );
    virtual ~EditableUserInfo( ) Q_DECL_OVERRIDE;

private:
    void *m_obj;
    Q_DISABLE_COPY( EditableUserInfo )
};

}

#endif // PERM_EDITABLEUSERINFO_HXX
