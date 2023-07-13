#ifndef PERM_USERPERMLISTVM_CXX
#define PERM_USERPERMLISTVM_CXX

#include "perm_userpermlistvm.hxx"
#include "perm/service/perm_userpermfiltersvc.hxx"
#include "perm/service/perm_editableuserinfo.hxx"
#include "perm/service/perm_editableuserpermlistinfo.hxx"
#include "perm/common/perm_memcntr.hxx"
#include <QJsonObject>

namespace FcPerm {

#define T_PrivPtr( o )  perm_objcast( UserPermListVmPriv*, o )
class PERM_HIDDEN UserPermListVmPriv
{
private:
    UserPermListVm   * m_parent;
    UserPermFilterSvc m_userPermFilterSvc;
    EditableUserPermListInfo m_editableUserPermListInfo;
    EditableUserInfo * m_editableUserInfo;

public:
    explicit UserPermListVmPriv ( UserPermListVm *pa );
    ~UserPermListVmPriv( );

    inline QObject * editableUserPermListInfoObj( ) { return &m_editableUserPermListInfo; }
    inline void  setPermInfo( const QJsonObject & jo ) { m_editableUserInfo->setPerm(jo); }
};

UserPermListVmPriv::UserPermListVmPriv(UserPermListVm *pa)
{
    m_parent = pa;
    m_editableUserInfo = EditableUserInfo::getInstance();

    m_userPermFilterSvc.filter( m_editableUserPermListInfo, *m_editableUserInfo);
}

UserPermListVmPriv::~UserPermListVmPriv()
{
    EditableUserInfo::release();
}

UserPermListVm::UserPermListVm(const QVariantList &)
{
    m_obj = perm_new( UserPermListVmPriv, this );
}

UserPermListVm::~UserPermListVm()
{
    perm_delete( m_obj, UserPermListVmPriv );
}

QObject *UserPermListVm::editableUserPermListInfoObj() const
{ return T_PrivPtr( m_obj )->editableUserPermListInfoObj(); }

QJsonObject UserPermListVm::permInfo() const
{ return QJsonObject(); }

void UserPermListVm::setPermInfo(const QJsonObject &jo)
{ T_PrivPtr( m_obj )->setPermInfo( jo ); }

}

#endif // PERM_USERPERMLISTVM_CXX
