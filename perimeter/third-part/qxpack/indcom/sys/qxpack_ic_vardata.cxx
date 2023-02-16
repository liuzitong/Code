#ifndef QXPACK_IC_VARDATA_CXX
#define QXPACK_IC_VARDATA_CXX

#include "qxpack_ic_vardata.hxx"
#include <QVariant>

#include <qxpack/indcom/common/qxpack_ic_memcntr.hxx>
#define QXPACK_IcPImplPrivTemp_new     qxpack_ic_new
#define QXPACK_IcPImplPrivTemp_delete  qxpack_ic_delete
#include <qxpack/indcom/common/qxpack_ic_pimplprivtemp.hpp>

#include <qxpack/indcom/common/qxpack_ic_dyncinit_priv.hxx>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
// register in meta system
// ////////////////////////////////////////////////////////////////////////////
static bool g_is_reg = false;
static void gRegInQt( )
{
    if ( ! g_is_reg ) {
        qRegisterMetaType<QxPack::IcVarData>("QxPack::IcVarData");
        qRegisterMetaType<QxPack::IcVarData>("QxPack::IcVarData&");
        g_is_reg = true;
    }
}

// ////////////////////////////////////////////////////////////////////////////
//               private object
// ////////////////////////////////////////////////////////////////////////////
#define T_PrivPtr( o )  (( IcVarDataPriv * ) o )
class QXPACK_IC_HIDDEN  IcVarDataPriv : public IcPImplPrivTemp< IcVarDataPriv > {
private:
    qintptr  m_tag, m_id; QVariant  m_var; QString m_name;
public :
    IcVarDataPriv ( );
    IcVarDataPriv ( const IcVarDataPriv & );
    virtual ~IcVarDataPriv( ) { }
    inline int tag( ) { return m_tag; }
    inline int id ( ) { return m_id;  }
    inline QString  name() { return m_name; }
    inline QVariant variant( ) { return m_var; }
    inline void  setTag( int t ) { m_tag = t; }
    inline void  setId ( int i ) { m_id  = i; }
    inline void  setName( const QString & nm ) { m_name = nm; }
    inline void  setVariant( const QVariant &v ) { m_var = v; }
};

// ============================================================================
// ctor
// ============================================================================
IcVarDataPriv :: IcVarDataPriv ( )
{
    m_tag = m_id = ( qintptr )( this );
}

// ============================================================================
// ctor( copy )
// ============================================================================
IcVarDataPriv :: IcVarDataPriv ( const IcVarDataPriv &other )
{
    m_tag = other.m_tag; m_id = other.m_id;
    m_var = other.m_var; m_name = other.m_name;
}

// ////////////////////////////////////////////////////////////////////////////
// wrap API
// ////////////////////////////////////////////////////////////////////////////
IcVarData :: IcVarData ( )
{ gRegInQt(); m_obj = nullptr; }

IcVarData :: IcVarData ( const IcVarData &other )
{
  gRegInQt(); m_obj = nullptr;
   IcVarDataPriv::attach( & m_obj, const_cast< void **>( & other.m_obj ));
}

IcVarData :: IcVarData ( const QVariant &var )
{
  gRegInQt(); m_obj = nullptr;
  IcVarDataPriv::buildIfNull( & m_obj )->setVariant( var );
}

IcVarData &  IcVarData :: operator = ( const IcVarData & other )
{
    IcVarDataPriv::attach( & m_obj, const_cast< void **>( & other.m_obj ));
    return *this;
}

IcVarData :: ~IcVarData ( )
{
    if ( m_obj != Q_NULLPTR ) {
        IcVarDataPriv::attach( & m_obj, nullptr );
    }
}

bool     IcVarData :: isNull( ) const  { return ( m_obj == nullptr ); }
qintptr  IcVarData :: id ( ) const     { return ( m_obj != nullptr ? T_PrivPtr( m_obj )->id() : 0 ); }
qintptr  IcVarData :: tag( ) const     { return ( m_obj != nullptr ? T_PrivPtr( m_obj )->tag() : 0 ); }
QString  IcVarData :: name( ) const    { return ( m_obj != nullptr ? T_PrivPtr( m_obj )->name() : QString()); }
QVariant IcVarData :: variant( ) const { return ( m_obj != nullptr ? T_PrivPtr( m_obj )->variant() : QVariant() ); }

void     IcVarData :: setId  ( qintptr id ) { IcVarDataPriv::instanceCow( & m_obj )->setId( id ); }
void     IcVarData :: setTag ( qintptr t  ) { IcVarDataPriv::instanceCow( & m_obj )->setTag( t ); }
void     IcVarData :: setName( const QString &nm )    { IcVarDataPriv::instanceCow( & m_obj )->setName( nm ); }
void     IcVarData :: setVariant( const QVariant &v ) { IcVarDataPriv::instanceCow( & m_obj )->setVariant( v ); }


}



#endif
