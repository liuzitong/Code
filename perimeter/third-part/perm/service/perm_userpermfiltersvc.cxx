#ifndef PERM_USERPERMFILTERSVC_CXX
#define PERM_USERPERMFILTERSVC_CXX

#include "perm_userpermfiltersvc.hxx"
#include "perm_editableuserpermlistinfo.hxx"
#include "perm_editableuserinfo.hxx"
#include "perm_permbroker.hxx"
#include "perm/service/perm_userandperminfo.hxx"
#include "perm/model/perm_userinfodata.hxx"
#include "perm/model/perm_typedef.hxx"
#include "perm/common/perm_memcntr.hxx"

namespace FcPerm {

#define T_PrivPtr( o )  perm_objcast( UserPermFilterSvcPriv*, o )
class PERM_HIDDEN UserPermFilterSvcPriv
{
private:
    UserPermFilterSvc  * m_parent;
    PermBroker        * m_permBroker;

protected:
    bool getInheritPermList( const EditableUserInfo &, QStringList & );

public:
    UserPermFilterSvcPriv ( UserPermFilterSvc * );
    ~UserPermFilterSvcPriv( );

    inline void filter( EditableUserPermListInfo &, const EditableUserInfo & );
};

bool UserPermFilterSvcPriv::getInheritPermList(const EditableUserInfo &editableUserInfo, QStringList &permList)
{
    QList<quint64> gids = editableUserInfo.userInfo().groupList().keys();
    return m_permBroker->getSpecUserGrpPermList(gids, permList);
}

UserPermFilterSvcPriv::UserPermFilterSvcPriv(UserPermFilterSvc *pa)
{
    m_parent = pa;
    m_permBroker = PermBroker::getInstance();
}

UserPermFilterSvcPriv::~UserPermFilterSvcPriv()
{
    PermBroker::freeInstance();
}

void UserPermFilterSvcPriv::filter(EditableUserPermListInfo &userPermListInfo, const EditableUserInfo &editableUserInfo)
{
    QStringList  inheritPermList; getInheritPermList( editableUserInfo, inheritPermList );
    AuthPermList authPermList = m_permBroker->userAndPermInfo()->authPermList();;
    QMap<QString, bool> privPermList = editableUserInfo.userInfo().privPermList();
    for ( int i = 0; i < authPermList.size(); i++ )
    {
        QString permName = authPermList.at(i);
        if ( inheritPermList.contains(permName) && privPermList.keys().contains(permName) )
        { userPermListInfo.addPerm(true, true, privPermList.value(permName), permName); }
        else if ( inheritPermList.contains(permName) )
        { userPermListInfo.addPerm(true, false, false, permName); }
        else if ( privPermList.keys().contains(permName) )
        { userPermListInfo.addPerm(false, true, privPermList.value(permName), permName); }
        else
        { userPermListInfo.addPerm(false, false, false, permName); }
    }
}

UserPermFilterSvc::UserPermFilterSvc()
{
    m_obj = perm_new( UserPermFilterSvcPriv, this );
}

UserPermFilterSvc::~UserPermFilterSvc()
{
    perm_delete( m_obj, UserPermFilterSvcPriv );
}

void UserPermFilterSvc::filter(EditableUserPermListInfo &userPermListInfo, const EditableUserInfo &userInfo)
{ T_PrivPtr( m_obj )->filter( userPermListInfo, userInfo ); }

}

#endif // PERM_USERPERMFILTERSVC_CXX
