#ifndef PERM_SPECUSERGRPPERMLISTINFO_CXX
#define PERM_SPECUSERGRPPERMLISTINFO_CXX

#include "perm_specusergrppermlistinfo.hxx"
#include "perm/model/perm_usergroupinfodata.hxx"
#include "perm/common/perm_memcntr.hxx"
#include <QJsonObject>
#include <QJsonArray>

namespace FcPerm {

#define T_PrivPtr( o )  perm_objcast( SpecUserGrpPermListInfoPriv*, o )
class PERM_HIDDEN SpecUserGrpPermListInfoPriv
{
private:
    SpecUserGrpPermListInfo  * m_parent;
    UserGroupInfoData          m_userGrpInfo;

public:
    explicit SpecUserGrpPermListInfoPriv ( SpecUserGrpPermListInfo * );
    virtual ~SpecUserGrpPermListInfoPriv( );

    inline UserGroupInfoData & userGrpInfo( ) { return m_userGrpInfo; }
};

SpecUserGrpPermListInfoPriv::SpecUserGrpPermListInfoPriv(SpecUserGrpPermListInfo *pa)
{
    m_parent = pa;
}

SpecUserGrpPermListInfoPriv::~SpecUserGrpPermListInfoPriv()
{

}

SpecUserGrpPermListInfo::SpecUserGrpPermListInfo()
{
    m_obj = perm_new( SpecUserGrpPermListInfoPriv, this );
}

SpecUserGrpPermListInfo::~SpecUserGrpPermListInfo()
{
    perm_delete( m_obj, SpecUserGrpPermListInfoPriv );
}

QStringList SpecUserGrpPermListInfo::permList() const
{ return T_PrivPtr( m_obj )->userGrpInfo().permList(); }

UserGroupInfoData SpecUserGrpPermListInfo::userGrpInfo() const
{ return T_PrivPtr( m_obj )->userGrpInfo(); }

void SpecUserGrpPermListInfo::setUserGrpInfo(const UserGroupInfoData &group)
{ T_PrivPtr( m_obj )->userGrpInfo() = group; }

}

#endif // PERM_SPECUSERGRPPERMLISTINFO_CXX
