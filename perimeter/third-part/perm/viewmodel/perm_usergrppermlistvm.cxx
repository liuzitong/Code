#ifndef PERM_USERGRPPERMLISTVM_CXX
#define PERM_USERGRPPERMLISTVM_CXX

#include "perm_usergrppermlistvm.hxx"
#include "perm/service/perm_usergrppermfiltersvc.hxx"
#include "perm/service/perm_editableusergrpinfo.hxx"
#include "perm/service/perm_editableusergrppermlistinfo.hxx"
#include "perm/common/perm_memcntr.hxx"
#include <QJsonObject>

namespace FcPerm {

#define T_PrivPtr( o )  perm_objcast( UserGrpPermListVmPriv*, o )
class PERM_HIDDEN UserGrpPermListVmPriv
{
private:
    UserGrpPermListVm   * m_parent;
    UserGrpPermFilterSvc  m_userGrpPermFilterSvc;
    EditableUserGrpPermListInfo m_editableUserGrpPermListInfo;
    EditableUserGrpInfo * m_editableUserGrpInfo;

public:
    explicit UserGrpPermListVmPriv ( UserGrpPermListVm *pa );
    ~UserGrpPermListVmPriv( );

    inline QObject * editableUserGrpPermListInfoObj( ) { return &m_editableUserGrpPermListInfo; }
    inline void setPermInfoList( const QJsonObject & jo ) { m_editableUserGrpInfo->setPerm(jo); }
};

UserGrpPermListVmPriv::UserGrpPermListVmPriv(UserGrpPermListVm *pa)
{
    m_parent = pa;
    m_editableUserGrpInfo = EditableUserGrpInfo::getInstance();

    m_userGrpPermFilterSvc.filter( m_editableUserGrpPermListInfo, *m_editableUserGrpInfo );
}

UserGrpPermListVmPriv::~UserGrpPermListVmPriv()
{
    EditableUserGrpInfo::release();
}

UserGrpPermListVm::UserGrpPermListVm(const QVariantList &)
{
    m_obj = perm_new( UserGrpPermListVmPriv, this );
}

UserGrpPermListVm::~UserGrpPermListVm()
{
    perm_delete( m_obj, UserGrpPermListVmPriv );
}

QObject *UserGrpPermListVm::editableUserGrpPermListInfoObj() const
{ return T_PrivPtr( m_obj )->editableUserGrpPermListInfoObj(); }

QJsonObject UserGrpPermListVm::permInfoList() const
{ return QJsonObject(); }

void UserGrpPermListVm::setPermInfoList(const QJsonObject &jo)
{ T_PrivPtr( m_obj )->setPermInfoList(jo); }

}

#endif // PERM_USERGRPPERMLISTVM_CXX
