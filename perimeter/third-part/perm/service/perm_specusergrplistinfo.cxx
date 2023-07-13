#ifndef PERM_SPECUSERGRPLISTINFO_CXX
#define PERM_SPECUSERGRPLISTINFO_CXX

#include "perm_specusergrplistinfo.hxx"
#include "perm/model/perm_userinfodata.hxx"
#include "perm/common/perm_memcntr.hxx"
#include <QJsonObject>
#include <QJsonArray>

namespace FcPerm {

#define T_PrivPtr( o )  perm_objcast( SpecUserGrpListInfoPriv*, o )
class PERM_HIDDEN SpecUserGrpListInfoPriv
{
private:
    SpecUserGrpListInfo  * m_parent;
    UserInfoData           m_userInfo;

public:
    explicit SpecUserGrpListInfoPriv ( SpecUserGrpListInfo * );
    virtual ~SpecUserGrpListInfoPriv( );

    inline UserInfoData & userInfo( ) { return m_userInfo; }
};

SpecUserGrpListInfoPriv::SpecUserGrpListInfoPriv(SpecUserGrpListInfo *pa)
{
    m_parent = pa;
}

SpecUserGrpListInfoPriv::~SpecUserGrpListInfoPriv()
{

}

SpecUserGrpListInfo::SpecUserGrpListInfo()
{
    m_obj = perm_new( SpecUserGrpListInfoPriv, this );
}

SpecUserGrpListInfo::~SpecUserGrpListInfo()
{
    perm_delete( m_obj, SpecUserGrpListInfoPriv );
}

QStringList SpecUserGrpListInfo::groupList() const
{ return T_PrivPtr( m_obj )->userInfo().groupList().values(); }

UserInfoData SpecUserGrpListInfo::userInfo() const
{ return T_PrivPtr( m_obj )->userInfo(); }

void SpecUserGrpListInfo::setUserInfo(const UserInfoData &user)
{ T_PrivPtr( m_obj )->userInfo() = user; }

}

#endif // PERM_SPECUSERGRPLISTINFO_CXX
