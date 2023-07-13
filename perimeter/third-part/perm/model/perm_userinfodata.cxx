#include "perm_userinfodata.hxx"
#include "qxpack/indcom/common/qxpack_ic_pimplprivtemp.hpp"
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

#define T_PrivPtr( o )  perm_objcast( UserInfoDataPriv*, o )
class UserInfoDataPriv : public QxPack::IcPImplPrivTemp<UserInfoDataPriv> {
private:
    quint64  m_uid;     // 用户ID
    QString  m_name;    // 用户名称
    QString  m_pwd;     // 用户密码
    QString  m_memo;    // 用户备注
    QMap<QString, bool> m_privPermList; // 用户私有权限
    QStringList m_groupPermList;        // 用户组权限集合
    QStringList m_actualPermList;       // 最终权限集合，由私有权限和用户组权限计算获得
    QMap<quint64, QString> m_groupList; // 用户组集合

public:
    UserInfoDataPriv( );
    UserInfoDataPriv( const UserInfoDataPriv & );
    virtual ~UserInfoDataPriv( );

    inline quint64 & UID ( ) { return m_uid; }
    inline QString & name( ) { return m_name; }
    inline QString & pwd ( ) { return m_pwd; }
    inline QString & memo( ) { return m_memo; }
    inline QMap<QString, bool> & privPermList( ) { return m_privPermList; }
    inline QStringList & groupPermList ( ) { return m_groupPermList; }
    inline QStringList & actualPermList( ) { return m_actualPermList; }
    inline QMap<quint64, QString> & groupList( ) { return m_groupList; }
};

UserInfoDataPriv::UserInfoDataPriv()
{

}

UserInfoDataPriv::UserInfoDataPriv(const UserInfoDataPriv &other)
{
    m_uid  = other.m_uid;
    m_name = other.m_name;
    m_pwd  = other.m_pwd;
    m_memo = other.m_memo;
    m_privPermList = other.m_privPermList;
    m_groupList = other.m_groupList;
    m_actualPermList = other.m_actualPermList;
    m_groupList = other.m_groupList;
}

UserInfoDataPriv::~UserInfoDataPriv()
{

}

static bool g_is_reg_user_info_data = false;
static void gRegUserInfoData( )
{
    if ( ! g_is_reg_user_info_data ) {
        g_is_reg_user_info_data = true;
        qRegisterMetaType<FcPerm::UserInfoData>("FcPerm::UserInfoData");
        qRegisterMetaType<FcPerm::UserInfoData>("FcPerm::UserInfoData&");
    }
}

UserInfoData::UserInfoData()
{
    m_obj = Q_NULLPTR; gRegUserInfoData();
}

UserInfoData::UserInfoData(const QJsonArray &ja)
{
    m_obj = nullptr;  gRegUserInfoData();
    UserInfoDataPriv::buildIfNull( & m_obj );
    UserInfoDataPriv *priv = T_PrivPtr( m_obj );
    priv->UID()  = ja.at(0).toString().toULongLong(Q_NULLPTR, 16);
    priv->name() = ja.at(1).toString();
    priv->pwd()  = ja.at(2).toString();
    priv->memo() = ja.at(3).toString();
}

UserInfoData::UserInfoData(const QJsonObject &jo)
{
    m_obj = nullptr;  gRegUserInfoData();
    UserInfoDataPriv::buildIfNull( & m_obj );
    UserInfoDataPriv *priv = T_PrivPtr( m_obj );
    priv->UID()  = jo.contains("UID") ? jo.value("UID").toString().toULongLong(Q_NULLPTR, 16) : 0;
    priv->name() = jo.contains("name") ? jo.value("name").toString() : QStringLiteral("");
    priv->pwd()  = jo.contains("pwd" ) ? jo.value("pwd").toString() : QStringLiteral("");
    priv->memo() = jo.contains("memo") ? jo.value("memo").toString() : QStringLiteral("");
    if ( jo.contains("privPermList") )
    {
        QJsonArray privPermList = jo.value("privPermList").toArray();
        QJsonArray::const_iterator citr = privPermList.constBegin();
        while ( citr != privPermList.constEnd() )
        {
            QJsonArray perm = (*citr++).toArray();
            if ( perm.size() == 2 )
            { priv->privPermList().insert(perm.at(0).toString(), perm.at(1).toBool()); }
        }
    }
    if ( jo.contains("groupList") )
    {
        QJsonArray groupList = jo.value("groupList").toArray();
        QJsonArray::const_iterator citr = groupList.constBegin();
        while ( citr != groupList.constEnd() )
        {
            QJsonArray grip = (*citr++).toArray();
            if ( grip.size() == 2 )
            { priv->groupList().insert(grip.at(0).toString().toULongLong(Q_NULLPTR, 16), grip.at(1).toString()); }
        }
    }
    if ( jo.contains("actualPermList"))
    {
        QJsonArray permList = jo.value("actualPermList").toArray();
        QJsonArray::const_iterator citr = permList.constBegin();
        while ( citr != permList.constEnd())
        { priv->actualPermList().push_back((*( citr ++ )).toString()); }
    }
}

UserInfoData::UserInfoData(const UserInfoData &other)
{
    // [HINT] use attach method for share other data
    m_obj = nullptr; gRegUserInfoData(); // must be init it as null
    UserInfoDataPriv::attach( & m_obj, const_cast<void**>(&other.m_obj) );
}

UserInfoData &UserInfoData::operator =(const UserInfoData &other)
{
    UserInfoDataPriv::attach( & m_obj, const_cast<void**>(&other.m_obj ) );
    return *this;
}

UserInfoData::~UserInfoData()
{
    if ( m_obj != nullptr ) { // [HINT] directly drop it.
        UserInfoDataPriv::attach( & m_obj, nullptr );
    }
}

bool UserInfoData::isNull() const
{ return m_obj == nullptr; }

quint64 UserInfoData::UID() const
{ return ( m_obj != nullptr ? T_PrivPtr( m_obj )->UID() : 0 ); }

QString UserInfoData::name() const
{ return ( m_obj != nullptr ? T_PrivPtr( m_obj )->name() : QString( ) ); }

QString UserInfoData::pwd() const
{ return ( m_obj != nullptr ? T_PrivPtr( m_obj )->pwd() : QString( ) ); }

QString UserInfoData::memo() const
{ return ( m_obj != nullptr ? T_PrivPtr( m_obj )->memo() : QString( ) ); }
// =======================================================================
// 获取角色权限
// =======================================================================
quint16 UserInfoData::role() const
{
    quint16 perm = 0x00;
    for ( const QString &one : this->actualPermList() )
    {
        if ( gRolePermMap.values().contains(one) )
        { perm |= gRolePermMap.key(one); }
    }

    return perm;
}

QMap<QString, bool> UserInfoData::privPermList() const
{ return ( m_obj != nullptr ? T_PrivPtr( m_obj )->privPermList() : QMap<QString, bool>( ) ); }

QStringList UserInfoData::groupPermList() const
{ return ( m_obj != nullptr ? T_PrivPtr( m_obj )->groupPermList() : QStringList( ) ); }

QStringList UserInfoData::actualPermList() const
{ return ( m_obj != nullptr ? T_PrivPtr( m_obj )->actualPermList() : QStringList( ) ); }

QMap<quint64, QString> UserInfoData::groupList() const
{ return ( m_obj != nullptr ? T_PrivPtr( m_obj )->groupList() : QMap<quint64, QString>( ) ); }

void UserInfoData::setUID(const quint64 & id)
{ UserInfoDataPriv::instanceCow( &m_obj )->UID() = id; }

void UserInfoData::setName(const QString &name)
{ UserInfoDataPriv::instanceCow( &m_obj )->name() = name; }

void UserInfoData::setPwd(const QString &pwd)
{ UserInfoDataPriv::instanceCow( &m_obj )->pwd() = pwd; }

void UserInfoData::setMemo(const QString &memo)
{ UserInfoDataPriv::instanceCow( &m_obj )->memo() = memo; }

void UserInfoData::setPrivPermList(const QMap<QString, bool> &privPermList)
{ UserInfoDataPriv::instanceCow( &m_obj )->privPermList() = privPermList; }

void UserInfoData::setGroupPermList(const QStringList &groupPermList)
{ UserInfoDataPriv::instanceCow( &m_obj )->groupPermList() = groupPermList; }

void UserInfoData::setActualPermList(const QStringList &actualPermList)
{ UserInfoDataPriv::instanceCow( &m_obj )->actualPermList() = actualPermList; }

void UserInfoData::setGroupList(const QMap<quint64, QString> &groupList)
{ UserInfoDataPriv::instanceCow( &m_obj )->groupList() = groupList; }

}
