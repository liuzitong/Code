#ifndef PERM_USERGRPPERMFILTERSVC_CXX
#define PERM_USERGRPPERMFILTERSVC_CXX

#include "perm_usergrppermfiltersvc.hxx"
#include "perm_editableusergrppermlistinfo.hxx"
#include "perm_editableusergrpinfo.hxx"
#include "perm/service/perm_userandperminfo.hxx"
#include "perm/model/perm_typedef.hxx"
#include "perm/model/perm_usergroupinfodata.hxx"
#include "perm/common/perm_memcntr.hxx"

namespace FcPerm {

#define T_PrivPtr( o )  perm_objcast( UserGrpPermFilterSvcPriv*, o )
class PERM_HIDDEN UserGrpPermFilterSvcPriv
{
private:
    UserGrpPermFilterSvc  * m_parent;
    UserAndPermInfo       * m_user_and_perm_info;

public:
    UserGrpPermFilterSvcPriv ( UserGrpPermFilterSvc * );
    ~UserGrpPermFilterSvcPriv( );

    inline void filter( EditableUserGrpPermListInfo &, const EditableUserGrpInfo & );
};

UserGrpPermFilterSvcPriv::UserGrpPermFilterSvcPriv(UserGrpPermFilterSvc *pa)
{
    m_parent = pa;
    m_user_and_perm_info = &UserAndPermInfo::globalInstance();
}

UserGrpPermFilterSvcPriv::~UserGrpPermFilterSvcPriv()
{

}

void UserGrpPermFilterSvcPriv::filter(EditableUserGrpPermListInfo &permList, const EditableUserGrpInfo &group)
{
    QStringList permInfoList = group.userGrpInfo().permList();
    AuthPermList authPermList = m_user_and_perm_info->authPermList();
    for ( int i = 0; i < authPermList.size(); i++ )
    {
        QString permName = authPermList.at(i);
        permList.addPerm( permInfoList.contains(permName), permName );
    }
}

UserGrpPermFilterSvc::UserGrpPermFilterSvc()
{
    m_obj = perm_new( UserGrpPermFilterSvcPriv, this );
}

UserGrpPermFilterSvc::~UserGrpPermFilterSvc()
{
    perm_delete( m_obj, UserGrpPermFilterSvcPriv );
}

void UserGrpPermFilterSvc::filter(EditableUserGrpPermListInfo &userGrpPermListInfo, const EditableUserGrpInfo &userGrpInfo)
{ T_PrivPtr( m_obj )->filter( userGrpPermListInfo, userGrpInfo ); }

}

#endif // PERM_USERGRPPERMFILTERSVC_CXX
