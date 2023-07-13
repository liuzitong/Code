#ifndef PERM_SPECUSERGRPPERMLISTINFO_HXX
#define PERM_SPECUSERGRPPERMLISTINFO_HXX

#include "perm/common/perm_def.h"
#include "qxpack/indcom/afw/qxpack_ic_svcbase.hxx"

namespace FcPerm {

class UserGroupInfoData;
// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief 指定用户组权限详情
 */
// ////////////////////////////////////////////////////////////////////////////
class PERM_API SpecUserGrpPermListInfo : public QxPack::IcSvcBase
{
    Q_OBJECT
    Q_PROPERTY( QStringList  permList READ  permList  CONSTANT )
public:
    Q_INVOKABLE explicit SpecUserGrpPermListInfo( );
    virtual ~SpecUserGrpPermListInfo( ) Q_DECL_OVERRIDE;

    QStringList permList( ) const;

    UserGroupInfoData userGrpInfo( ) const;
    void setUserGrpInfo( const UserGroupInfoData & );

private:
    void *m_obj;
    Q_DISABLE_COPY( SpecUserGrpPermListInfo )
};

}

#endif // PERM_SPECUSERGRPPERMLISTINFO_HXX
