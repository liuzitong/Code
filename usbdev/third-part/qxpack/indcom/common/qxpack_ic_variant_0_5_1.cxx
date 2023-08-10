#ifndef QXPACK_IC_VARIANT_CXX
#define QXPACK_IC_VARIANT_CXX

#include "qxpack_ic_variant.hxx"
#include <cstdlib>

namespace QxPack {


// ////////////////////////////////////////////////////////////////////////////
// API implement
// ////////////////////////////////////////////////////////////////////////////
IcVariant :: IcVariant ( )
{
    m_priv.u_data.v_longlong = 0;
    m_priv.type          = Type_Invalid;
    m_priv.is_shared_ptr = 0;
    m_priv.is_null       = 1;
}

IcVariant :: ~IcVariant ( )
{
    detachSharedPtr();
}

#define CTOR_PLAIN_VAL( param_type, tp, member ) \
    IcVariant :: IcVariant ( param_type par ) \
{  m_priv.u_data.member = par; m_priv.type = tp; m_priv.is_shared_ptr = 0; m_priv.is_null = 0; }

CTOR_PLAIN_VAL( signed  int,  Type_Int,       v_int  )
CTOR_PLAIN_VAL( unsigned int, Type_UInt,      v_uint )
CTOR_PLAIN_VAL( int64_t,      Type_LongLong,  v_longlong )
CTOR_PLAIN_VAL( uint64_t,     Type_ULongLong, v_ulonglong )
CTOR_PLAIN_VAL( double,       Type_Double,    v_dbl )
CTOR_PLAIN_VAL( void*,        Type_VoidStar,  v_ptr )
CTOR_PLAIN_VAL( bool,         Type_Bool,      v_bool )

// ============================================================================
// convert APIs
// ============================================================================
#define FUNC_TO_PLAIN_VAR( rt, func, tp, member, def_val ) \
rt  IcVariant :: func ( bool *is_ok ) const { \
    if ( is_ok != nullptr ) { *is_ok = ( m_priv.type == tp ); } \
    return ( m_priv.type == tp ? m_priv.u_data.member : def_val ); \
}

FUNC_TO_PLAIN_VAR( bool,         toBool,      Type_Bool,      v_bool, false )
FUNC_TO_PLAIN_VAR( signed int,   toInt,       Type_Int,       v_int,  0 )
FUNC_TO_PLAIN_VAR( unsigned int, toUInt,      Type_UInt,      v_uint, 0 )
FUNC_TO_PLAIN_VAR( int64_t,      toLongLong,  Type_LongLong,  v_longlong,  0 )
FUNC_TO_PLAIN_VAR( uint64_t,     toULongLong, Type_ULongLong, v_ulonglong, 0 )
FUNC_TO_PLAIN_VAR( double,       toDouble,    Type_Double,    v_dbl,  0 )
FUNC_TO_PLAIN_VAR( void*,        toVoidStar,  Type_VoidStar,  v_ptr, nullptr )

IcVariant :: IcVariant ( const IcVariant &other )
{
    m_priv = other.m_priv;
    if ( isSharedObj() ) { m_priv.u_data.v_sp->addRef(); }
}

IcVariant &  IcVariant :: operator = ( const IcVariant &other )
{
    if ( this != &other ) {
        detachSharedPtr();
        m_priv = other.m_priv;
        if ( isSharedObj()) { m_priv.u_data.v_sp->addRef(); }
    }
    return *this;
}

// ============================================================================
// [ static ] create a shared object
// ============================================================================
IcVariant::PrivShared*   IcVariant :: createSharedPtr()
{  return new IcVariant::PrivShared; }

// ============================================================================
//  detach shared pointer, this maybe delete shared structure
// ============================================================================
void  IcVariant :: detachSharedPtr ( )
{
    if ( isSharedObj() ) {
        if ( m_priv.u_data.v_sp->decRef() == 0 ) {
            delete m_priv.u_data.v_sp;
            m_priv.u_data.v_sp = nullptr;
        }
    }
    setInvalidType();
}

}

#endif
