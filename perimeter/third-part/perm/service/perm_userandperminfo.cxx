#ifndef PERM_USERANDPERMINFO_CXX
#define PERM_USERANDPERMINFO_CXX

#include "perm_userandperminfo.hxx"
#include "perm/model/perm_privpermpair.hxx"
#include "perm/model/perm_groupsimpledesc.hxx"
#include "perm/common/perm_memcntr.hxx"
#include <QMutex>
#include <QThread>
#include <QMap>
#include <QCoreApplication>

namespace FcPerm {

static QMap<quint64, QString> gRolePermMap = {
    { 0x40, "Perm.root" },            // 根用户
    { 0x20, "Perm.sys"},              // 系统用户
    { 0x10, "Perm.admin"},            // 管理员用户
    { 0x08, "Perm.manager"},          // 普通管理用户
    { 0x04, "Perm.chkPerm"},          // 第三方权限验证用户
};

#define T_PrivPtr( o )  perm_objcast( UserAndPermInfoPriv*, o )
class PERM_HIDDEN UserAndPermInfoPriv
{
private:
    UserAndPermInfo   * m_parent;
    PrivPermList        m_privPermList;
    GroupPermList       m_groupPermList;
    AuthPermList        m_authPermList;
    AuthGroupList       m_authGroupList;
    ActualPermList      m_actualPermList;
    GroupSimpleList     m_groupSimpleList;
    quint64             m_uid;
    QString             m_accToken;
    QString             m_queryToken;

public:
    explicit UserAndPermInfoPriv ( UserAndPermInfo *pa );
    ~UserAndPermInfoPriv( );

    inline quint64 & UID       ( ) { return m_uid; }
    inline QString & accToken  ( ) { return m_accToken; }
    inline QString & queryToken( ) { return m_queryToken; }
    inline ActualPermList & actualPermList( ) { return m_actualPermList; }
    inline AuthPermList   & authPermList( )   { return m_authPermList; }
    inline AuthGroupList  & authGroupList( )  { return m_authGroupList; }
};

UserAndPermInfoPriv::UserAndPermInfoPriv(UserAndPermInfo *pa)
{
    m_parent = pa;
    m_uid = 0;
}

UserAndPermInfoPriv::~UserAndPermInfoPriv()
{

}

UserAndPermInfo::UserAndPermInfo()
{
    m_obj = perm_new( UserAndPermInfoPriv, this );
}

UserAndPermInfo::~UserAndPermInfo()
{
    perm_delete( m_obj, UserAndPermInfoPriv );
}

quint64 UserAndPermInfo::UID() const
{ return T_PrivPtr( m_obj )->UID( ); }

QString UserAndPermInfo::accToken() const
{ return T_PrivPtr( m_obj )->accToken( ); }

QString UserAndPermInfo::queryToken() const
{ return T_PrivPtr( m_obj )->queryToken( ); }

quint16 UserAndPermInfo::role() const
{
    quint16 perm = 0x00;
    for ( const QString &one : this->actualPermList() )
    {
        if ( gRolePermMap.values().contains(one) )
        { perm |= gRolePermMap.key(one); }
    }

    return perm;
}

ActualPermList UserAndPermInfo::actualPermList() const
{ return T_PrivPtr( m_obj )->actualPermList( ); }

AuthPermList UserAndPermInfo::authPermList() const
{ return T_PrivPtr( m_obj )->authPermList( ); }

AuthGroupList UserAndPermInfo::authGroupList() const
{ return T_PrivPtr( m_obj )->authGroupList( ); }

void UserAndPermInfo::setUID(const quint64 &uid)
{ T_PrivPtr( m_obj )->UID() = uid; }

void UserAndPermInfo::setAccToken(const QString &token)
{ T_PrivPtr( m_obj )->accToken() = token; }

void UserAndPermInfo::setQueryToken(const QString &token)
{ T_PrivPtr( m_obj )->queryToken() = token; }

void UserAndPermInfo::setActualPermList(const ActualPermList &val)
{ T_PrivPtr( m_obj )->actualPermList() = val; }

void UserAndPermInfo::setAuthPermList(const AuthPermList &val)
{ T_PrivPtr( m_obj )->authPermList() = val; }

void UserAndPermInfo::setAuthGroupList(const AuthGroupList &val)
{ T_PrivPtr( m_obj )->authGroupList() = val; }

void UserAndPermInfo::clear()
{
    T_PrivPtr( m_obj )->UID() = 0;
    T_PrivPtr( m_obj )->accToken( ) = QString( );
    T_PrivPtr( m_obj )->actualPermList() = ActualPermList( );
    T_PrivPtr( m_obj )->authPermList() = AuthPermList( );
    T_PrivPtr( m_obj )->authGroupList() = AuthGroupList( );
}


static UserAndPermInfo *g_uap_info = Q_NULLPTR;
static QMutex           g_uap_info_locker;

// ====================================================================
// release the personInfo at final
// ====================================================================
static void  releasePerson( )
{
    g_uap_info_locker.lock();
    if ( g_uap_info != Q_NULLPTR ) {
        perm_delete( g_uap_info, UserAndPermInfo );
        g_uap_info = Q_NULLPTR;
    }
    g_uap_info_locker.unlock();
}

// ====================================================================
// return the global instance of the PersonInfo
// ====================================================================
UserAndPermInfo &    UserAndPermInfo :: globalInstance (  )
{
    g_uap_info_locker.lock();
    if ( g_uap_info == Q_NULLPTR ) {
        g_uap_info = perm_new( UserAndPermInfo );
        if ( QThread::currentThread() != QCoreApplication::instance()->thread()) {
            // not gui thread..
            g_uap_info->moveToThread( QCoreApplication::instance()->thread());
        }
        qAddPostRoutine( & releasePerson );
    }
    g_uap_info_locker.unlock();
    return *g_uap_info;
}

}

#endif // WAFC{AERM_USERANDPERMINFO_CXX
