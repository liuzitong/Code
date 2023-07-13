#ifndef PERM_PRIVPERMPAIR_CXX
#define PERM_PRIVPERMPAIR_CXX

#include "perm_privpermpair.hxx"
#include "qxpack/indcom/common/qxpack_ic_pimplprivtemp.hpp"
#include <QMetaType>

namespace FcPerm {

#define T_PrivPtr( o )  perm_objcast( PrivPermPairPriv*, o )
class PrivPermPairPriv : public QxPack::IcPImplPrivTemp<PrivPermPairPriv> {
private:
    QString  m_permName;    // 权限名称
    bool     m_isAllow;     // true 权限被允许， false 权限被禁止

public:
    PrivPermPairPriv( );
    PrivPermPairPriv( const PrivPermPairPriv & );
    virtual ~PrivPermPairPriv( );

    inline QString & permName( ) { return m_permName; }
    inline bool    & isAllow ( ) { return m_isAllow;  }
};

PrivPermPairPriv::PrivPermPairPriv()
{

}

PrivPermPairPriv::PrivPermPairPriv(const PrivPermPairPriv &other)
{
    m_permName = other.m_permName;
    m_isAllow  = other.m_isAllow;
}

PrivPermPairPriv::~PrivPermPairPriv()
{

}

static bool g_is_reg_priv_perm_pair = false;
static void gRegPrivPermPair( )
{
    if ( ! g_is_reg_priv_perm_pair ) {
        g_is_reg_priv_perm_pair = true;
        qRegisterMetaType<FcPerm::PrivPermPair>("FcPerm::PrivPermPair");
        qRegisterMetaType<FcPerm::PrivPermPair>("FcPerm::PrivPermPair&");
    }
}

PrivPermPair::PrivPermPair()
{
    m_obj = Q_NULLPTR; gRegPrivPermPair();
}

PrivPermPair::PrivPermPair(const PrivPermPair &other)
{
    // [HINT] use attach method for share other data
    m_obj = nullptr; gRegPrivPermPair(); // must be init it as null
    PrivPermPairPriv::attach( & m_obj, const_cast<void**>(&other.m_obj) );
}

PrivPermPair &PrivPermPair::operator =(const PrivPermPair &other)
{
    PrivPermPairPriv::attach( & m_obj, const_cast<void**>(&other.m_obj ) );
    return *this;
}

PrivPermPair::~PrivPermPair()
{
    if ( m_obj != nullptr ) { // [HINT] directly drop it.
        PrivPermPairPriv::attach( & m_obj, nullptr );
    }
}

QString PrivPermPair::permName() const
{ return ( m_obj != nullptr ? T_PrivPtr( m_obj )->permName() : QString( ) ); }

bool PrivPermPair::isAllow() const
{ return ( m_obj != nullptr ? T_PrivPtr( m_obj )->isAllow() : false ); }

void PrivPermPair::setPermName(const QString &name)
{ PrivPermPairPriv::instanceCow( & m_obj )->permName( ) = name; }

void PrivPermPair::setIsAllow(const bool &allow)
{ PrivPermPairPriv::instanceCow( & m_obj )->isAllow( ) = allow; }

}

#endif // PERM_PRIVPERMPAIR_CXX
