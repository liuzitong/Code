#ifndef PERM_SPECUSERGRPLISTINFO_HXX
#define PERM_SPECUSERGRPLISTINFO_HXX

#include "perm/common/perm_def.h"
#include "qxpack/indcom/afw/qxpack_ic_svcbase.hxx"

namespace FcPerm {

class UserInfoData;
// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief 指定用户用户组详情
 */
// ////////////////////////////////////////////////////////////////////////////

class PERM_API SpecUserGrpListInfo : public QxPack::IcSvcBase
{
    Q_OBJECT
    Q_PROPERTY(QStringList groupList READ groupList  CONSTANT)
public:
    Q_INVOKABLE explicit SpecUserGrpListInfo( );
    virtual ~SpecUserGrpListInfo( ) Q_DECL_OVERRIDE;

    QStringList groupList( ) const;

    UserInfoData userInfo( ) const;
    void setUserInfo( const UserInfoData & );

private:
    void *m_obj;
    Q_DISABLE_COPY( SpecUserGrpListInfo )
};

}

#endif // PERM_SPECUSERGRPLISTINFO_HXX
