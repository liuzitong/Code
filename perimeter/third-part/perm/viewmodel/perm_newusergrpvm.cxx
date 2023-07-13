#ifndef PERM_NEWUSERGRPVM_CXX
#define PERM_NEWUSERGRPVM_CXX

#include "perm_newusergrpvm.hxx"
#include "perm/service/perm_permbroker.hxx"
#include "perm/service/perm_editableusergrpinfo.hxx"
#include "perm/model/perm_usergrplistinfo.hxx"
#include "perm/common/perm_memcntr.hxx"
#include <QTimer>

namespace FcPerm {

#define T_PrivPtr( o )  perm_objcast( NewUserGrpVmPriv*, o )
class PERM_HIDDEN NewUserGrpVmPriv
{
private:
    NewUserGrpVm   * m_parent;
    PermBroker     * m_permBroker;
    EditableUserGrpInfo * m_editUserGrpInfo;
    UserGrpListInfo* m_userGrpListInfo;

public:
    explicit NewUserGrpVmPriv ( NewUserGrpVm *pa );
    ~NewUserGrpVmPriv( );

    inline QObject * editUserGrpInfoObj( ) { return m_editUserGrpInfo; }
    inline void      setGrpInfo( const QJsonObject & jo ) { m_editUserGrpInfo->setUserGrpInfo(jo);  }
    inline void      save( ) { m_permBroker->createUserGrp( *m_editUserGrpInfo, *m_userGrpListInfo ); }
};

NewUserGrpVmPriv::NewUserGrpVmPriv(NewUserGrpVm *pa)
{
    m_parent = pa;
    m_permBroker      = PermBroker::getInstance();
    m_userGrpListInfo = UserGrpListInfo::getInstance();
    m_editUserGrpInfo = EditableUserGrpInfo::getInstance();
}

NewUserGrpVmPriv::~NewUserGrpVmPriv()
{
    PermBroker::freeInstance();
    UserGrpListInfo::release();
    EditableUserGrpInfo::release();
}

NewUserGrpVm::NewUserGrpVm(const QVariantList &)
{
    m_obj = perm_new( NewUserGrpVmPriv, this );
}

NewUserGrpVm::~NewUserGrpVm()
{
    perm_delete( m_obj, NewUserGrpVmPriv );
}

QObject *NewUserGrpVm::editUserGrpInfoObj() const
{ return T_PrivPtr( m_obj )->editUserGrpInfoObj(); }

QJsonObject NewUserGrpVm::grpInfo() const
{ return QJsonObject(); }

void NewUserGrpVm::setGrpInfo(const QJsonObject &jo)
{ T_PrivPtr( m_obj )->setGrpInfo(jo); }

void NewUserGrpVm::editUserGrpPerm()
{ QTimer::singleShot(200, [this](){ emit this->showUserGrpPermListView(); }); }

void NewUserGrpVm::save()
{ T_PrivPtr( m_obj )->save(); }

}

#endif // PERM_NEWUSERGRPVM_CXX
