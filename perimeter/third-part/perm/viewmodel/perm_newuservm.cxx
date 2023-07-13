#ifndef PERM_NEWUSERVM_CXX
#define PERM_NEWUSERVM_CXX

#include "perm_newuservm.hxx"
#include "perm/service/perm_permbroker.hxx"
#include "perm/service/perm_editableuserinfo.hxx"
#include "perm/model/perm_userlistinfo.hxx"
#include "perm/common/perm_memcntr.hxx"
#include <QTimer>

namespace FcPerm {

#define T_PrivPtr( o )  perm_objcast( NewUserVmPriv*, o )
class PERM_HIDDEN NewUserVmPriv
{
private:
    NewUserVm   * m_parent;
    PermBroker  * m_permBroker;
    EditableUserInfo * m_editUserInfo;
    UserListInfo* m_userListInfo;

public:
    explicit NewUserVmPriv ( NewUserVm *pa );
    ~NewUserVmPriv( );

    inline QObject * editUserInfoObj( ) { return m_editUserInfo; }
    inline void setUserInfo( const QJsonObject & jo ) { m_editUserInfo->setUserInfo(jo); }
    inline void save( ) { m_permBroker->createUser( *m_editUserInfo, *m_userListInfo ); }
};

NewUserVmPriv::NewUserVmPriv(NewUserVm *pa)
{
    m_parent = pa;
    m_permBroker   = PermBroker::getInstance();
    m_userListInfo = UserListInfo::getInstance();
    m_editUserInfo = EditableUserInfo::getInstance();
}

NewUserVmPriv::~NewUserVmPriv()
{
    PermBroker::freeInstance();
    UserListInfo::release();
    EditableUserInfo::release();
}

NewUserVm::NewUserVm(const QVariantList &)
{
    m_obj = perm_new( NewUserVmPriv, this );
}

NewUserVm::~NewUserVm()
{
    perm_delete( m_obj, NewUserVmPriv );
}

QObject *NewUserVm::editUserInfoObj() const
{ return T_PrivPtr( m_obj )->editUserInfoObj(); }

QJsonObject NewUserVm::userInfo() const
{ return QJsonObject(); }

void NewUserVm::setUserInfo(const QJsonObject &jo)
{ T_PrivPtr( m_obj )->setUserInfo(jo); }

void NewUserVm::editUserGroup()
{ QTimer::singleShot(200, [this](){ emit this->showUserGrpListView(); }); }

void NewUserVm::editUserPerm()
{ QTimer::singleShot(200, [this](){ emit this->showUserPermListView(); }); }

void NewUserVm::save()
{ T_PrivPtr( m_obj )->save(); }

}

#endif // PERM_NEWUSERVM_CXX
