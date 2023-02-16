#ifndef QXPACK_IC_VARIANT_HXX
#define QXPACK_IC_VARIANT_HXX

#include "qxpack_ic_def.h"
#include <stdint.h>
#include <cstring>
#include <cstdlib>
#include <new>
#include "qxpack_ic_global.hxx"

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
// variant type API
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcVariant QXPACK_IC_FINAL {
public:
    // ========================================================================
    // the type of the variant object
    // ========================================================================
    enum Type {
        Type_Invalid   = 0,  Type_Bool   = 1, Type_Int = 2, Type_UInt = 3, Type_LongLong = 4,
        Type_ULongLong = 5,  Type_Double = 6,
        Type_VoidStar  = 31,
        Type_User      = 1024
    };

    // ========================================================================
    // ctors
    // ========================================================================
    explicit IcVariant( );
    IcVariant( const IcVariant & );
    IcVariant( signed   int );
    IcVariant( unsigned int );
    IcVariant( int64_t  );
    IcVariant( uint64_t );
    IcVariant( bool   );
    IcVariant( double );
    IcVariant( void*  );

    // ========================================================================
    // copy assign
    // ========================================================================
    IcVariant &   operator = ( signed int i   ) { return ( *this = IcVariant( i )); }
    IcVariant &   operator = ( unsigned int i ) { return ( *this = IcVariant( i )); }
    IcVariant &   operator = ( int64_t  i ) { return ( *this = IcVariant(i)); }
    IcVariant &   operator = ( uint64_t i ) { return ( *this = IcVariant(i)); }
    IcVariant &   operator = ( double   d ) { return ( *this = IcVariant(d)); }
    IcVariant &   operator = ( void* p ) { return ( *this = IcVariant( p )); }
    IcVariant &   operator = ( bool  b ) { return ( *this = IcVariant( b )); }

    // ========================================================================
    // APIs
    // ========================================================================
    inline  void  makeNull()      { *this = IcVariant(); }
    inline  bool  isNull () const { return ( m_priv.is_null != 0 );  }
    inline  bool  isValid() const { return ( m_priv.type != Type_Invalid ); }

    bool          toBool ( bool *is_ok = nullptr ) const;
    signed int    toInt  ( bool *is_ok = nullptr ) const;
    unsigned int  toUInt ( bool *is_ok = nullptr ) const;
    uint64_t      toULongLong( bool *is_ok = nullptr ) const;
    int64_t       toLongLong ( bool *is_ok = nullptr ) const;
    double        toDouble   ( bool *is_ok = nullptr ) const;
    void*         toVoidStar ( bool *is_ok = nullptr ) const;

    inline bool  isInt() const       { return ( m_priv.type == Type_Int );   }
    inline bool  isLongLong() const  { return ( m_priv.type == Type_LongLong ); }
    inline bool  isUInt() const      { return ( m_priv.type == Type_UInt ); }
    inline bool  isULongLong() const { return ( m_priv.type == Type_ULongLong ); }
    inline bool  isDouble() const    { return ( m_priv.type == Type_Double );}
    inline bool  isVoidStar() const  { return ( m_priv.type == Type_VoidStar && m_priv.is_shared_ptr == 0 ); }

private:    
    // base of shared descriptor
    class QXPACK_IC_API PrivShared {
        QXPACK_IC_DISABLE_COPY( PrivShared )
    protected:
        static void      initRefCntr  ( intptr_t *, intptr_t v );
        static void      deInitRefCntr( intptr_t * );
        static intptr_t  addRefCntr( intptr_t *, intptr_t v );
        static intptr_t  subRefCntr( intptr_t *, intptr_t v );
    public:
        typedef void ( *DelCb )( void* );
    private:
        intptr_t     m_ref_cntr;
        DelCb        m_del_cb;
    public :
        explicit PrivShared() : m_del_cb(nullptr) { initRefCntr(&m_ref_cntr,1); }
        virtual ~PrivShared() { deInitRefCntr(&m_ref_cntr);  }
        inline intptr_t  addRef()   { return addRefCntr( &m_ref_cntr, 1 ); }
        inline intptr_t  decRef()   { return subRefCntr( &m_ref_cntr, 1 ); }
        inline DelCb&    delCbRef() { return m_del_cb; }
    };

    template <typename T>
    class QXPACK_IC_API PrivSharedImpl : public PrivShared {
    private:
        uint8_t   m_obj_spc[ sizeof(T)];
    public :
        PrivSharedImpl( const T& tgt ) { new (m_obj_spc) T( tgt ); }
        virtual ~PrivSharedImpl() override
        {  auto *obj = reinterpret_cast<T*>(m_obj_spc); obj->~T(); }
        inline T&  objRef() { return *reinterpret_cast<T*>(m_obj_spc);  }
    };

    // the private data
    struct Private {
        union Data {
            signed   int  v_int;        unsigned int  v_uint;
            int64_t       v_longlong;   uint64_t      v_ulonglong;
            bool   v_bool;   double  v_dbl;  void*  v_ptr;
            PrivShared*   v_sp;
        } u_data;
        unsigned int  type : 30;
        unsigned int  is_shared_ptr : 1;
        unsigned int  is_null : 1;
    };
    Private  m_priv;

protected:
    inline bool   isSharedObj() const { return ( m_priv.type == Type_VoidStar && m_priv.is_shared_ptr != 0 ); }
    inline void   setScalarType( Type t ) { m_priv.type = static_cast<unsigned int>(t); m_priv.is_null = 0; m_priv.is_shared_ptr = 0; }
    inline void   setInvalidType()    { m_priv.type = Type_Invalid; m_priv.is_null = 1; m_priv.is_shared_ptr = 0; }

           void         detachSharedPtr();

public :
    ~IcVariant( );
    IcVariant &   operator = ( const IcVariant &other );

#ifdef QXPACK_IC_CXX11
    IcVariant ( IcVariant &&other ) {  m_priv = other.m_priv; other.setInvalidType(); }

    IcVariant &   operator = ( IcVariant &&other )
    {
        if ( this != &other ) { m_priv = other.m_priv; other.setInvalidType(); }
        return *this;
    }
#endif

    // ========================================================================
    // set the value
    // ========================================================================
    template <typename T>
    void  setValue( const T& val )
    {
        *this = IcVariant();
        PrivSharedImpl<T>* sp = new PrivSharedImpl<T>( val );
        m_priv.u_data.v_sp = sp;
        m_priv.u_data.v_sp->delCbRef() =
        []( void *obj ){
             auto *impl = reinterpret_cast<PrivSharedImpl<T>*>( obj );
             delete impl;
        };
        m_priv.type = Type_VoidStar; m_priv.is_null = 0; m_priv.is_shared_ptr = 1;
    }

    inline void  setValue( void* v ) { m_priv.u_data.v_ptr = v;  setScalarType( Type_VoidStar );}

    // ========================================================================
    // read the value
    // ========================================================================
    template <typename T>
    T   value() const
    {
        if ( isSharedObj() ) {
            auto *impl = reinterpret_cast<PrivSharedImpl<T>*>( m_priv.u_data.v_sp );
            return impl->objRef();
        } else {
            return T();
        }
    }
};


template <>
inline void  IcVariant::setValue( const bool& val )
{ detachSharedPtr(); m_priv.u_data.v_bool = val; setScalarType( Type_Bool ); }

template <>
inline void  IcVariant::setValue( const signed int& val )
{ detachSharedPtr(); m_priv.u_data.v_int  = val; setScalarType( Type_Int  ); }

template <>
inline void  IcVariant::setValue( const unsigned int& val)
{ detachSharedPtr(); m_priv.u_data.v_uint = val; setScalarType( Type_UInt ); }

template <>
inline void  IcVariant::setValue( const int64_t &v  )
{ detachSharedPtr(); m_priv.u_data.v_longlong = v;  setScalarType( Type_LongLong ); }

template <>
inline void  IcVariant::setValue( const uint64_t &v )
{ detachSharedPtr();  m_priv.u_data.v_ulonglong= v;  setScalarType( Type_ULongLong ); }

template <>
inline void  IcVariant::setValue( const double &v )
{ detachSharedPtr();  m_priv.u_data.v_dbl = v;  setScalarType( Type_Double ); }

template <>
inline void  IcVariant::setValue( const float &v )
{ setValue( double(v)); }

template <>
inline bool         IcVariant::value() const { return m_priv.u_data.v_bool; }
template <>
inline signed   int IcVariant::value() const { return m_priv.u_data.v_int; }
template <>
inline unsigned int IcVariant::value() const { return m_priv.u_data.v_uint; }
template <>
inline int64_t      IcVariant::value() const { return m_priv.u_data.v_longlong; }
template <>
inline uint64_t     IcVariant::value() const { return m_priv.u_data.v_ulonglong; }
template <>
inline double       IcVariant::value() const { return m_priv.u_data.v_dbl; }
template <>
inline void*        IcVariant::value() const { return m_priv.u_data.v_ptr; }


}

#endif
