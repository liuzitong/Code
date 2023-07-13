#ifndef PERM_EDITABLEUSERGRPINFO_CXX
#define PERM_EDITABLEUSERGRPINFO_CXX

#include "perm_editableusergrpinfo.hxx"
#include "perm/model/perm_usergroupinfodata.hxx"
#include "perm/common/perm_memcntr.hxx"
#include "qxpack/indcom/sys/qxpack_ic_rmtobjdeletor_priv.hxx"
#include <QMutex>
#include <QJsonArray>
#include <QJsonObject>

namespace FcPerm {

#define T_PrivPtr( o )  perm_objcast( EditableUserGrpInfoPriv*, o )
class PERM_HIDDEN EditableUserGrpInfoPriv
{
private:
    EditableUserGrpInfo  * m_parent;
    UserGroupInfoData      m_userGrpInfo;

public:
    explicit EditableUserGrpInfoPriv ( EditableUserGrpInfo *pa );
    ~EditableUserGrpInfoPriv( );

    inline UserGroupInfoData & userGrpInfo( ) { return m_userGrpInfo; }
};

EditableUserGrpInfoPriv::EditableUserGrpInfoPriv(EditableUserGrpInfo *pa)
{
    m_parent = pa;
}

EditableUserGrpInfoPriv::~EditableUserGrpInfoPriv()
{
}

static QAtomicPointer<EditableUserGrpInfo>  g_ptr( Q_NULLPTR );
static QMutex       g_locker;
static QAtomicInt   g_ref_cntr(0);

EditableUserGrpInfo *EditableUserGrpInfo::getInstance()
{
    EditableUserGrpInfo *ptr = Q_NULLPTR;

    g_locker.lock();
    ptr = g_ptr.loadAcquire();
    if ( ptr == Q_NULLPTR ) {
        g_ptr.store( ( ptr = new EditableUserGrpInfo( ) ) );
        g_ref_cntr.store(1);
    } else {
        g_ref_cntr.fetchAndAddOrdered(1);
    }
    g_locker.unlock();
    return ptr;
}

void EditableUserGrpInfo::release()
{
    EditableUserGrpInfo *ptr = Q_NULLPTR;

    g_locker.lock();
    ptr = g_ptr.loadAcquire();
    if ( ptr != Q_NULLPTR ) {
        if ( g_ref_cntr.fetchAndSubOrdered(1) - 1 == 0 ) {
            QxPack::IcRmtObjDeletor::deleteObjInThread(
                ptr->thread(),
                [](void*, QObject* obj ) {
                    EditableUserGrpInfo *svc = qobject_cast<EditableUserGrpInfo*>( obj );
                    delete svc;
                }, Q_NULLPTR, ptr, false
            );
            g_ptr.store( Q_NULLPTR );
        }
    }
    g_locker.unlock();
}

QString EditableUserGrpInfo::name() const
{ return T_PrivPtr( m_obj )->userGrpInfo().name(); }

QString EditableUserGrpInfo::memo() const
{ return T_PrivPtr( m_obj )->userGrpInfo().memo(); }

void EditableUserGrpInfo::setUserGrpInfo(const QJsonObject &jo)
{
    if ( jo.contains("GID") )
    { T_PrivPtr( m_obj )->userGrpInfo().setGID(jo.value("GID").toString().toULongLong(Q_NULLPTR, 16)); }
    if ( jo.contains("name") )
    { T_PrivPtr( m_obj )->userGrpInfo().setName(jo.value("name").toString()); }
    if ( jo.contains("memo") )
    { T_PrivPtr( m_obj )->userGrpInfo().setMemo(jo.value("memo").toString()); }
}

void EditableUserGrpInfo::setPerm(const QJsonObject &jo)
{
    if ( jo.contains("permList") )
    {
        QStringList perms;
        QJsonArray ja = jo.value("permList").toArray();
        QJsonArray::const_iterator citr = ja.constBegin();
        while ( citr != ja.constEnd())
        {
            QJsonObject child = (*( citr ++ )).toObject();
            QString permName = child.value("permName").toString();
            perms.push_back(permName);
        }
        T_PrivPtr( m_obj )->userGrpInfo().setPermList(perms);
    }
}

UserGroupInfoData EditableUserGrpInfo::userGrpInfo() const
{ return T_PrivPtr( m_obj )->userGrpInfo(); }

void EditableUserGrpInfo::setUserGrpInfo(const UserGroupInfoData &group)
{ T_PrivPtr( m_obj )->userGrpInfo() = group; }

void EditableUserGrpInfo::clear()
{ T_PrivPtr( m_obj )->userGrpInfo() = UserGroupInfoData(); emit userGrpChanged(); }

EditableUserGrpInfo::EditableUserGrpInfo()
{
    m_obj = perm_new( EditableUserGrpInfoPriv, this );
}

EditableUserGrpInfo::~EditableUserGrpInfo()
{
    perm_delete( m_obj, EditableUserGrpInfoPriv );
}

}

#endif // PERM_EDITABLEUSERGRPINFO_CXX
