#ifndef PERM_USERGRPFILTERSVC_CXX
#define PERM_USERGRPFILTERSVC_CXX

#include "perm_usergrpfiltersvc.hxx"
#include "perm_editableusergrplistinfo.hxx"
#include "perm_editableuserinfo.hxx"
#include "perm/service/perm_userandperminfo.hxx"
#include "perm/model/perm_typedef.hxx"
#include "perm/model/perm_groupsimpledesc.hxx"
#include "perm/model/perm_userinfodata.hxx"
#include "perm/common/perm_memcntr.hxx"

namespace FcPerm {

#define T_PrivPtr( o )  perm_objcast( UserGrpFilterSvcPriv*, o )
class PERM_HIDDEN UserGrpFilterSvcPriv
{
private:
    UserGrpFilterSvc  * m_parent;
    UserAndPermInfo      *m_user_and_perm_info;

public:
    UserGrpFilterSvcPriv ( UserGrpFilterSvc * );
    ~UserGrpFilterSvcPriv( );

    inline void filter( EditableUserGrpListInfo &, const EditableUserInfo & );
};

UserGrpFilterSvcPriv::UserGrpFilterSvcPriv(UserGrpFilterSvc *pa)
{
    m_parent = pa;
    m_user_and_perm_info = &UserAndPermInfo::globalInstance();
}

UserGrpFilterSvcPriv::~UserGrpFilterSvcPriv()
{

}

void UserGrpFilterSvcPriv::filter(EditableUserGrpListInfo &editableGrpInfo, const EditableUserInfo &editableUserInfo)
{
    AuthGroupList authGroupList = m_user_and_perm_info->authGroupList();
    for ( int i = 0; i < authGroupList.size(); i++ )
    {
        GroupSimpleDesc group = authGroupList.at(i);
        if ( editableUserInfo.userInfo().groupList().contains(group.gid()) )
        { editableGrpInfo.addGrp(true, QString("%1").arg(group.gid(), 16, 16, QLatin1Char('0')), group.name()); }
        else
        { editableGrpInfo.addGrp(false, QString("%1").arg(group.gid(), 16, 16, QLatin1Char('0')), group.name()); }
    }
}

UserGrpFilterSvc::UserGrpFilterSvc()
{
    m_obj = perm_new( UserGrpFilterSvcPriv, this );
}

UserGrpFilterSvc::~UserGrpFilterSvc()
{
    perm_delete( m_obj, UserGrpFilterSvcPriv );
}

void UserGrpFilterSvc::filter(EditableUserGrpListInfo &userGrpListInfo, const EditableUserInfo &userInfo)
{ T_PrivPtr( m_obj )->filter( userGrpListInfo, userInfo ); }

}

#endif // PERM_USERGRPFILTERSVC_CXX
