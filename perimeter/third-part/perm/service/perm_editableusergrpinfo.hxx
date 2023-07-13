#ifndef PERM_EDITABLEUSERGRPINFO_HXX
#define PERM_EDITABLEUSERGRPINFO_HXX

#include "perm/common/perm_def.h"
#include "qxpack/indcom/afw/qxpack_ic_svcbase.hxx"

namespace FcPerm {

class UserGroupInfoData;
// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief 编辑用户组信息
 */
// ////////////////////////////////////////////////////////////////////////////
class PERM_API EditableUserGrpInfo : public QxPack::IcSvcBase
{
    Q_OBJECT
    Q_PROPERTY(QString groupName READ name NOTIFY userGrpChanged)
    Q_PROPERTY(QString groupMemo READ memo NOTIFY userGrpChanged)
public:
    static EditableUserGrpInfo * getInstance( );
    static void release( );

    QString name( ) const;
    QString memo( ) const;

    UserGroupInfoData userGrpInfo( ) const;
    void setUserGrpInfo( const UserGroupInfoData & );
    void setUserGrpInfo( const QJsonObject & );
    void setPerm( const QJsonObject & );

    Q_INVOKABLE void clear( );
    Q_SIGNAL void userGrpChanged();

private:
    Q_INVOKABLE explicit EditableUserGrpInfo( );
    virtual ~EditableUserGrpInfo( ) Q_DECL_OVERRIDE;

private:
    void *m_obj;
    Q_DISABLE_COPY( EditableUserGrpInfo )
};

}

#endif // PERM_EDITABLEUSERGRPINFO_HXX
