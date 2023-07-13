#ifndef PERM_USERGRPLISTVM_CXX
#define PERM_USERGRPLISTVM_CXX

#include "perm_usergrplistvm.hxx"
#include "perm/service/perm_editableusergrplistinfo.hxx"
#include "perm/service/perm_usergrpfiltersvc.hxx"
#include "perm/service/perm_editableuserinfo.hxx"
#include "perm/common/perm_memcntr.hxx"
#include <QJsonObject>

namespace FcPerm {

#define T_PrivPtr( o )  perm_objcast( UserGrpListVmPriv*, o )
class PERM_HIDDEN UserGrpListVmPriv
{
private:
    UserGrpListVm   * m_parent;
    UserGrpFilterSvc  m_userGrpFilterSvc;
    EditableUserGrpListInfo m_editableUserGrpListInfo;
    EditableUserInfo* m_editableUserInfo;

public:
    explicit UserGrpListVmPriv ( UserGrpListVm *pa );
    ~UserGrpListVmPriv( );

    inline QObject * editableUserGrpListInfoObj( ) { return &m_editableUserGrpListInfo; }
    inline void  setGrpInfo( const QJsonObject & jo ) { m_editableUserInfo->setGroup(jo); }
};

UserGrpListVmPriv::UserGrpListVmPriv(UserGrpListVm *pa)
{
    m_parent = pa;
    m_editableUserInfo = EditableUserInfo::getInstance();

    m_userGrpFilterSvc.filter( m_editableUserGrpListInfo, *m_editableUserInfo);
}

UserGrpListVmPriv::~UserGrpListVmPriv()
{
    EditableUserInfo::release();
}

///
/// \brief UserGrpListVm::UserGrpListVm
///
UserGrpListVm::UserGrpListVm(const QVariantList &)
{
    m_obj = perm_new( UserGrpListVmPriv, this );
}

UserGrpListVm::~UserGrpListVm()
{
    perm_delete( m_obj, UserGrpListVmPriv );
}

QObject *UserGrpListVm::editableUserGrpListInfoObj() const
{ return T_PrivPtr( m_obj )->editableUserGrpListInfoObj(); }

QJsonObject UserGrpListVm::grpInfo() const
{ return QJsonObject(); }

void UserGrpListVm::setGrpInfo(const QJsonObject &jo)
{ T_PrivPtr( m_obj )->setGrpInfo( jo ); }

}

#endif // PERM_USERGRPLISTVM_CXX
