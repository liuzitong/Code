#ifndef PERM_SPECUSERPERMLISTINFO_HXX
#define PERM_SPECUSERPERMLISTINFO_HXX

#include "perm/common/perm_def.h"
#include "qxpack/indcom/afw/qxpack_ic_svcbase.hxx"

namespace FcPerm {

class UserInfoData;
// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief 指定用户用户权限详情
 */
// ////////////////////////////////////////////////////////////////////////////
class PERM_API SpecUserPermListInfo : public QxPack::IcSvcBase
{
    Q_OBJECT
    Q_PROPERTY(QStringList actualPermList READ actualPermList  CONSTANT)
public:
    Q_INVOKABLE explicit SpecUserPermListInfo( );
    virtual ~SpecUserPermListInfo( ) Q_DECL_OVERRIDE;

    QStringList actualPermList( ) const;

    UserInfoData userInfo( ) const;
    void setUserInfo( const UserInfoData & );

private:
    void *m_obj;
    Q_DISABLE_COPY( SpecUserPermListInfo )
};

}

#endif // PERM_SPECUSERPERMLISTINFO_HXX
