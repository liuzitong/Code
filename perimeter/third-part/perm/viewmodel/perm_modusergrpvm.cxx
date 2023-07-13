#ifndef PERM_MODUSERGRPVM_CXX
#define PERM_MODUSERGRPVM_CXX

#include "perm_modusergrpvm.hxx"
#include "perm/service/perm_permbroker.hxx"
#include "perm/service/perm_editableusergrpinfo.hxx"
#include "perm/model/perm_usergrplistinfo.hxx"
#include "perm/common/perm_memcntr.hxx"
#include <QTimer>

namespace FcPerm {

#define T_PrivPtr( o )  perm_objcast( ModUserGrpVmPriv*, o )
class PERM_HIDDEN ModUserGrpVmPriv
{
private:
    ModUserGrpVm   * m_parent;
    PermBroker     * m_permBroker;
    EditableUserGrpInfo * m_editUserGrpInfo;
    UserGrpListInfo* m_userGrpListInfo;

public:
    explicit ModUserGrpVmPriv ( ModUserGrpVm *pa );
    ~ModUserGrpVmPriv( );

    inline QObject * editUserGrpInfoObj( ) { return m_editUserGrpInfo; }
    inline void      setGrpInfo( const QJsonObject & jo ) { m_editUserGrpInfo->setUserGrpInfo(jo);  }
    inline void      save( ) { m_permBroker->setUserGrp( *m_editUserGrpInfo, *m_userGrpListInfo ); }
};

ModUserGrpVmPriv::ModUserGrpVmPriv(ModUserGrpVm *pa)
{
    m_parent = pa;
    m_permBroker      = PermBroker::getInstance();
    m_userGrpListInfo = UserGrpListInfo::getInstance();
    m_editUserGrpInfo = EditableUserGrpInfo::getInstance();

    m_permBroker->getUserGrp( *m_userGrpListInfo, *m_editUserGrpInfo );
}

ModUserGrpVmPriv::~ModUserGrpVmPriv()
{
    PermBroker::freeInstance();
    UserGrpListInfo::release();
    EditableUserGrpInfo::release();
}

ModUserGrpVm::ModUserGrpVm(const QVariantList &)
{
    m_obj = perm_new( ModUserGrpVmPriv, this );
}

ModUserGrpVm::~ModUserGrpVm()
{
    perm_delete( m_obj, ModUserGrpVmPriv );
}

QObject *ModUserGrpVm::editUserGrpInfoObj() const
{ return T_PrivPtr( m_obj )->editUserGrpInfoObj(); }

QJsonObject ModUserGrpVm::grpInfo() const
{ return QJsonObject(); }

void ModUserGrpVm::setGrpInfo(const QJsonObject &jo)
{ T_PrivPtr( m_obj )->setGrpInfo(jo); }

void ModUserGrpVm::editUserGrpPerm()
{ QTimer::singleShot(200, [this](){ emit this->showUserGrpPermListView(); }); }

void ModUserGrpVm::save()
{ T_PrivPtr( m_obj )->save(); }

}

#endif // PERM_MODUSERGRPVM_CXX
