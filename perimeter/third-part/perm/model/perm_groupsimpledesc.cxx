#ifndef PERM_GROUPSIMPLEDESC_CXX
#define PERM_GROUPSIMPLEDESC_CXX

#include "perm_groupsimpledesc.hxx"
#include "qxpack/indcom/common/qxpack_ic_pimplprivtemp.hpp"

namespace FcPerm {

#define T_PrivPtr( o )  perm_objcast( GroupSimpleDescPriv*, o )
class GroupSimpleDescPriv : public QxPack::IcPImplPrivTemp<GroupSimpleDescPriv> {
private:
    quint64  m_gid;     // 用户组id
    QString  m_name;    // 用户组名称

public:
    GroupSimpleDescPriv( );
    GroupSimpleDescPriv( const GroupSimpleDescPriv & );
    virtual ~GroupSimpleDescPriv( );

    inline quint64 & gid ( ) { return m_gid; }
    inline QString & name( ) { return m_name; }
};

GroupSimpleDescPriv::GroupSimpleDescPriv()
{

}

GroupSimpleDescPriv::GroupSimpleDescPriv(const GroupSimpleDescPriv &other)
{
    m_gid = other.m_gid;
    m_name = other.m_name;
}

GroupSimpleDescPriv::~GroupSimpleDescPriv()
{

}

static bool g_is_reg_group_simple_desc = false;
static void gRegGroupSimpleDesc( )
{
    if ( ! g_is_reg_group_simple_desc ) {
        g_is_reg_group_simple_desc = true;
        qRegisterMetaType<FcPerm::GroupSimpleDesc>("FcPerm::GroupSimpleDesc");
        qRegisterMetaType<FcPerm::GroupSimpleDesc>("FcPerm::GroupSimpleDesc&");
    }
}

GroupSimpleDesc::GroupSimpleDesc()
{
    m_obj = Q_NULLPTR; gRegGroupSimpleDesc();
}

GroupSimpleDesc::GroupSimpleDesc(const quint64 &gid, const QString &name)
{
    m_obj = Q_NULLPTR; gRegGroupSimpleDesc();
    GroupSimpleDescPriv::buildIfNull( & m_obj );
    GroupSimpleDescPriv *priv = T_PrivPtr( m_obj );
    priv->gid() = gid;
    priv->name() = name;
}

GroupSimpleDesc::GroupSimpleDesc(const GroupSimpleDesc &other)
{
    // [HINT] use attach method for share other data
    m_obj = nullptr; gRegGroupSimpleDesc(); // must be init it as null
    GroupSimpleDescPriv::attach( & m_obj, const_cast<void**>(&other.m_obj) );
}

GroupSimpleDesc &GroupSimpleDesc::operator =(const GroupSimpleDesc &other)
{
    GroupSimpleDescPriv::attach( & m_obj, const_cast<void**>(&other.m_obj ) );
    return *this;
}

GroupSimpleDesc::~GroupSimpleDesc()
{
    if ( m_obj != nullptr ) { // [HINT] directly drop it.
        GroupSimpleDescPriv::attach( & m_obj, nullptr );
    }
}

quint64 GroupSimpleDesc::gid() const
{ return ( m_obj != nullptr ? T_PrivPtr( m_obj )->gid() : 0 ); }

QString GroupSimpleDesc::name() const
{ return ( m_obj != nullptr ? T_PrivPtr( m_obj )->name() : QString( ) ); }

}

#endif // PERM_GROUPSIMPLEDESC_CXX
