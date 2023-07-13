#ifndef PERM_EDITABLEUSERINFO_CXX
#define PERM_EDITABLEUSERINFO_CXX

#include "perm_editableuserinfo.hxx"
#include "perm/model/perm_userinfodata.hxx"
#include "perm/common/perm_memcntr.hxx"
#include "qxpack/indcom/sys/qxpack_ic_rmtobjdeletor_priv.hxx"
#include <QMutex>
#include <QJsonArray>
#include <QJsonObject>

namespace FcPerm {

#define T_PrivPtr( o )  perm_objcast( EditableUserInfoPriv*, o )
class PERM_HIDDEN EditableUserInfoPriv
{
private:
    EditableUserInfo     * m_parent;
    UserInfoData           m_userInfo;

public:
    explicit EditableUserInfoPriv ( EditableUserInfo *pa );
    ~EditableUserInfoPriv( );

    inline UserInfoData & userInfo( ) { return m_userInfo; }
};

EditableUserInfoPriv::EditableUserInfoPriv(EditableUserInfo *pa)
{
    m_parent = pa;
}

EditableUserInfoPriv::~EditableUserInfoPriv()
{
}

static QAtomicPointer<EditableUserInfo>  g_ptr( Q_NULLPTR );
static QMutex       g_locker;
static QAtomicInt   g_ref_cntr(0);

EditableUserInfo *EditableUserInfo::getInstance()
{
    EditableUserInfo *ptr = Q_NULLPTR;

    g_locker.lock();
    ptr = g_ptr.loadAcquire();
    if ( ptr == Q_NULLPTR ) {
        g_ptr.store( ( ptr = new EditableUserInfo( ) ) );
        g_ref_cntr.store(1);
    } else {
        g_ref_cntr.fetchAndAddOrdered(1);
    }
    g_locker.unlock();
    return ptr;
}

void EditableUserInfo::release()
{
    EditableUserInfo *ptr = Q_NULLPTR;

    g_locker.lock();
    ptr = g_ptr.loadAcquire();
    if ( ptr != Q_NULLPTR ) {
        if ( g_ref_cntr.fetchAndSubOrdered(1) - 1 == 0 ) {
            QxPack::IcRmtObjDeletor::deleteObjInThread(
                ptr->thread(),
                [](void*, QObject* obj ) {
                    EditableUserInfo *svc = qobject_cast<EditableUserInfo*>( obj );
                    delete svc;
                }, Q_NULLPTR, ptr, false
            );
            g_ptr.store( Q_NULLPTR );
        }
    }
    g_locker.unlock();
}

QString EditableUserInfo::name() const
{ return T_PrivPtr( m_obj )->userInfo().name(); }

QString EditableUserInfo::pwd() const
{ return T_PrivPtr( m_obj )->userInfo().pwd(); }

QString EditableUserInfo::memo() const
{ return T_PrivPtr( m_obj )->userInfo().memo(); }

UserInfoData EditableUserInfo::userInfo() const
{ return T_PrivPtr( m_obj )->userInfo(); }

void EditableUserInfo::setUserInfo(const UserInfoData &info)
{ T_PrivPtr( m_obj )->userInfo() = info; }

void EditableUserInfo::setUserInfo(const QJsonObject &jo)
{
    if ( jo.contains("UID") )
    { T_PrivPtr( m_obj )->userInfo().setUID(jo.value("UID").toString().toULongLong(Q_NULLPTR, 16)); }
    if ( jo.contains("name") )
    { T_PrivPtr( m_obj )->userInfo().setName(jo.value("name").toString()); }
    if ( jo.contains("pwd") )
    { T_PrivPtr( m_obj )->userInfo().setPwd(jo.value("pwd").toString()); }
    if ( jo.contains("memo") )
    { T_PrivPtr( m_obj )->userInfo().setMemo(jo.value("memo").toString()); }
}

void EditableUserInfo::setGroup(const QJsonObject &jo)
{
    if ( jo.contains("groupList") )
    {
        QMap<quint64, QString> groups;
        QJsonArray ja = jo.value("groupList").toArray();
        QJsonArray::const_iterator citr = ja.constBegin();
        while ( citr != ja.constEnd())
        {
            QJsonObject child = (*( citr ++ )).toObject();
            quint64 gid = child.value("gid").toString().toULongLong(Q_NULLPTR, 16);
            QString name = child.value("name").toString();
            groups.insert( gid, name );
        }
        T_PrivPtr( m_obj )->userInfo().setGroupList(groups);
    }
}

void EditableUserInfo::setPerm(const QJsonObject &jo)
{
    if ( jo.contains("permList") )
    {
        QMap<QString, bool> privPermList;
        QJsonArray ja = jo.value("permList").toArray();
        QJsonArray::const_iterator citr = ja.constBegin();
        while ( citr != ja.constEnd())
        {
            QJsonObject child = (*( citr ++ )).toObject();
            QString perm = child.value("perm").toString();
            bool    isEnable= child.value("isEnable").toBool();
            privPermList.insert(perm, isEnable);
        }
        T_PrivPtr( m_obj )->userInfo().setPrivPermList(privPermList);
    }
}

void EditableUserInfo::clear()
{ T_PrivPtr( m_obj )->userInfo() = UserInfoData(); emit userChanged(); }

EditableUserInfo::EditableUserInfo()
{
    m_obj = perm_new( EditableUserInfoPriv, this );
}

EditableUserInfo::~EditableUserInfo()
{
    perm_delete( m_obj, EditableUserInfoPriv );
}

}

#endif // PERM_EDITABLEUSERINFO_CXX
