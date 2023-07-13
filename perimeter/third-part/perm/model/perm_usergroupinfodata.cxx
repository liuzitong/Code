#ifndef PERM_USERGROUPINFODATA_CXX
#define PERM_USERGROUPINFODATA_CXX

#include "perm_usergroupinfodata.hxx"
#include "qxpack/indcom/common/qxpack_ic_pimplprivtemp.hpp"
#include <QMap>
#include <QJsonObject>
#include <QMetaType>

namespace FcPerm {

static QMap<quint64, QString> gRolePermMap = {
    { 0x40, "Perm.root" },            // 根用户
    { 0x20, "Perm.sys"},              // 系统用户
    { 0x10, "Perm.admin"},            // 管理员用户
    { 0x08, "Perm.manager"},          // 普通管理用户
    { 0x04, "Perm.chkPerm"},          // 第三方权限验证用户
};

#define T_PrivPtr( o )  perm_objcast( UserGroupInfoDataPriv*, o )
class UserGroupInfoDataPriv : public QxPack::IcPImplPrivTemp<UserGroupInfoDataPriv> {
private:
    quint64  m_gid;     // 用户组ID
    QString  m_name;    // 用户组名称
    QString  m_memo;    // 用户组备注
    QStringList m_permList; // 用户组权限

public:
    UserGroupInfoDataPriv( );
    UserGroupInfoDataPriv( const UserGroupInfoDataPriv & );
    virtual ~UserGroupInfoDataPriv( );

    inline quint64 & GID ( ) { return m_gid; }
    inline QString & name( ) { return m_name; }
    inline QString & memo( ) { return m_memo; }
    inline QStringList & permList( ) { return m_permList; }
};

UserGroupInfoDataPriv::UserGroupInfoDataPriv()
{

}

UserGroupInfoDataPriv::UserGroupInfoDataPriv(const UserGroupInfoDataPriv &other)
{
    m_gid  = other.m_gid;
    m_name = other.m_name;
    m_memo = other.m_memo;
}

UserGroupInfoDataPriv::~UserGroupInfoDataPriv()
{

}

static bool g_is_reg_user_grp_info_data = false;
static void gRegUserGroupInfoData( )
{
    if ( ! g_is_reg_user_grp_info_data ) {
        g_is_reg_user_grp_info_data = true;
        qRegisterMetaType<FcPerm::UserGroupInfoData>("FcPerm::UserGroupInfoData");
        qRegisterMetaType<FcPerm::UserGroupInfoData>("FcPerm::UserGroupInfoData&");
    }
}

UserGroupInfoData::UserGroupInfoData()
{
    m_obj = Q_NULLPTR; gRegUserGroupInfoData();
}

UserGroupInfoData::UserGroupInfoData(const QJsonArray &ja)
{
    m_obj = nullptr;  gRegUserGroupInfoData();
    UserGroupInfoDataPriv::buildIfNull( & m_obj );
    UserGroupInfoDataPriv *priv = T_PrivPtr( m_obj );
    priv->GID()  = ja.at(0).toString().toULongLong(Q_NULLPTR, 16);
    priv->name() = ja.at(1).toString();
    priv->memo() = ja.at(2).toString();
}

UserGroupInfoData::UserGroupInfoData(const QJsonObject &jo)
{
    m_obj = nullptr;  gRegUserGroupInfoData();
    UserGroupInfoDataPriv::buildIfNull( & m_obj );
    UserGroupInfoDataPriv *priv = T_PrivPtr( m_obj );
    priv->GID() = jo.contains("GID") ? jo.value("GID").toString().toULongLong(Q_NULLPTR, 16) : 0;
    priv->name() = jo.contains("name") ? jo.value("name").toString() : QStringLiteral("");
    priv->memo() = jo.contains("memo") ? jo.value("memo").toString() : QStringLiteral("");
    if ( jo.contains("permList") )
    {
        QJsonArray permList = jo.value("permList").toArray();
        QJsonArray::const_iterator citr = permList.constBegin();
        while ( citr != permList.constEnd() )
        { priv->permList().push_back((*citr++).toString()); }
    }
}

UserGroupInfoData::UserGroupInfoData(const UserGroupInfoData &other)
{
    // [HINT] use attach method for share other data
    m_obj = nullptr; gRegUserGroupInfoData(); // must be init it as null
    UserGroupInfoDataPriv::attach( & m_obj, const_cast<void**>(&other.m_obj) );
}

UserGroupInfoData &UserGroupInfoData::operator =(const UserGroupInfoData &other)
{
    UserGroupInfoDataPriv::attach( & m_obj, const_cast<void**>(&other.m_obj ) );
    return *this;
}

UserGroupInfoData::~UserGroupInfoData()
{
    if ( m_obj != nullptr ) { // [HINT] directly drop it.
        UserGroupInfoDataPriv::attach( & m_obj, nullptr );
    }
}

bool UserGroupInfoData::isNull() const
{ return m_obj == nullptr; }

quint64 UserGroupInfoData::GID() const
{ return ( m_obj != nullptr ? T_PrivPtr( m_obj )->GID() : 0 ); }

QString UserGroupInfoData::name() const
{ return ( m_obj != nullptr ? T_PrivPtr( m_obj )->name() : QString( ) ); }

QString UserGroupInfoData::memo() const
{ return ( m_obj != nullptr ? T_PrivPtr( m_obj )->memo() : QString( ) ); }

// =======================================================================
// 获取角色权限
// =======================================================================
quint16 UserGroupInfoData::role() const
{
    quint16 perm = 0x00;
    for ( const QString &one : this->permList() )
    {
        if ( gRolePermMap.values().contains(one) )
        { perm |= gRolePermMap.key(one); }
    }

    return perm;
}

QStringList UserGroupInfoData::permList() const
{ return ( m_obj != nullptr ? T_PrivPtr( m_obj )->permList() : QStringList( ) ); }

void UserGroupInfoData::setGID(const quint64 & id)
{ UserGroupInfoDataPriv::instanceCow( &m_obj )->GID() = id; }

void UserGroupInfoData::setName(const QString &name)
{ UserGroupInfoDataPriv::instanceCow( &m_obj )->name() = name; }

void UserGroupInfoData::setMemo(const QString &memo)
{ UserGroupInfoDataPriv::instanceCow( &m_obj )->memo() = memo; }

void UserGroupInfoData::setPermList(const QStringList & permList)
{ UserGroupInfoDataPriv::instanceCow( &m_obj )->permList() = permList; }

}

#endif // PERM_USERGROUPINFODATA_CXX
