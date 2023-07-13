#ifndef PERM_SPECUSERPERMLISTINFO_CXX
#define PERM_SPECUSERPERMLISTINFO_CXX

#include "perm_specuserpermlistinfo.hxx"
#include "perm/model/perm_userinfodata.hxx"
#include "perm/common/perm_memcntr.hxx"
#include <QJsonObject>
#include <QJsonArray>

namespace FcPerm {

#define T_PrivPtr( o )  perm_objcast( SpecUserPermListInfoPriv*, o )
class PERM_HIDDEN SpecUserPermListInfoPriv
{
private:
    SpecUserPermListInfo  * m_parent;
    UserInfoData            m_userInfo;

public:
    explicit SpecUserPermListInfoPriv ( SpecUserPermListInfo * );
    virtual ~SpecUserPermListInfoPriv( );

    inline UserInfoData & userInfo( ) { return m_userInfo; }
};

SpecUserPermListInfoPriv::SpecUserPermListInfoPriv(SpecUserPermListInfo *pa)
{
    m_parent = pa;
}

SpecUserPermListInfoPriv::~SpecUserPermListInfoPriv()
{

}

SpecUserPermListInfo::SpecUserPermListInfo()
{
    m_obj = perm_new( SpecUserPermListInfoPriv, this );
}

SpecUserPermListInfo::~SpecUserPermListInfo()
{
    perm_delete( m_obj, SpecUserPermListInfoPriv );
}

QStringList SpecUserPermListInfo::actualPermList() const
{ return T_PrivPtr( m_obj )->userInfo().actualPermList(); }

UserInfoData SpecUserPermListInfo::userInfo() const
{ return T_PrivPtr( m_obj )->userInfo(); }

void SpecUserPermListInfo::setUserInfo(const UserInfoData &user)
{ T_PrivPtr( m_obj )->userInfo() = user; }

}

#endif // PERM_SPECUSERPERMLISTINFO_CXX
