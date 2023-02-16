#ifndef QXPACK_IC_GLOBAL_HXX
#define QXPACK_IC_GLOBAL_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"

#include <cstdint>
#include <atomic>  // C++11
#include <thread>
#include <cmath>

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4311 )
#pragma warning( disable : 4302 )
#endif

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
// utility macro and inline functions
//
// ////////////////////////////////////////////////////////////////////////////
template <typename T>
    inline const T& min( const T &a, const T &b ) { return ( a < b ? a : b ); }
template <typename T>
    inline const T& max( const T &a, const T &b ) { return ( a < b ? b : a ); }
template <typename T>
    inline const T& clamp( const T&v, const T&rg_min, const T&rg_max )
{  return min( max( v, min( rg_min, rg_max )), max( rg_min, rg_max ) ); }
template <typename T>
    inline bool     isInRange( const T&v, const T&rg_min, const T&rg_max )
{  return ( v >= min(rg_min, rg_max) && v <= max(rg_min,rg_max)); }

template <typename T>
    inline T abs( const T &t) { return t >= 0 ? t : -t; }

template <typename T>
    inline int    round( const T &d ) { return static_cast<int>(d); }
template <>
    inline int    round( const double &d )
    { return d >= 0.0 ? int( d + 0.5 ) : int(d - double(int(d-1)) + 0.5) + int(d-1); }
template <>
    inline int    round( const float & d )
    { return d >= 0.0f ? int(d + 0.5f) : int(d - float(int(d-1)) + 0.5f) + int(d-1); }

// ============================================================================
//! round double value into int64_t
// ============================================================================
template <typename T>
    inline int64_t  round64( const T &d ) { return static_cast<int64_t>(d); }
template <>
    inline int64_t  round64( const double &d )
    { return d >= 0.0 ? int64_t(d + 0.5) : int64_t( d - double( int64_t( d-1 )) + 0.5) + int64_t( d-1 ); }
template <>
    inline int64_t  round64( const float  &f )
    { return f >= 0.0f ? int64_t(f + 0.5f) : int64_t(f - float(int64_t(f-1)) + 0.5f) + int64_t(f-1); }

template <typename T>
    inline const T& bound( const T &min_v, const T &val, const T &max_v )
{  return max( min_v, min( max_v, val ));  }

template <typename T>
    inline bool    fuzzyIsNull( const T &) { return false; }
template <>
    inline bool    fuzzyIsNull( const double &d ) { return abs( d ) <= 0.000000000001; }
template <>
    inline bool    fuzzyIsNull( const float  &f ) { return abs( f ) <= 0.00001f;  }
template <>
    inline bool    fuzzyIsNull( const int &i ) { return i == 0; }

template <typename T>
    inline bool    fuzzyIsNull( const T&, unsigned int ) { return false; }
template <>
    inline bool    fuzzyIsNull( const double &d, unsigned int decm_num )
{ return abs( d ) <= std::pow(10,-int(decm_num)); }
template <>
    inline bool    fuzzyIsNull( const float &f, unsigned int decm_num )
{ return fuzzyIsNull( double( f ), decm_num ); }
template <>
    inline bool    fuzzyIsNull( const int &i, unsigned int )
{ return i == 0; }


template <typename T>
    inline bool    isNull( const T &d) { return ( d == 0 ); }
template <>
    inline bool    isNull( const double &d )
{
    union U { double d; int64_t i; }; U val; val.d = d;
    return ( val.i & 0x7fffffffffffffffLL ) == 0;
}
template <>
    inline bool    isNull( const float &f )
{
    union U { float f; int32_t i; }; U val; val.f = f;
    return ( val.i & 0x7fffffffL ) == 0;
}
template <>
    inline bool    isNull( const int &i )
{ return i == 0; }

template <typename T>
    inline bool    fuzzyCompare( const T &a, const T &b ) { return ( a == b ); }
template <>
    inline bool    fuzzyCompare( const double &p1, const double & p2 )
{  return (abs(p1 - p2) * 1000000000000. <= min(abs(p1), abs(p2)));  }
template <>
   inline bool     fuzzyCompare( const float  &p1, const float  & p2 )
{  return (abs(p1 - p2) * 100000.f <= min(abs(p1), abs(p2))); } 
template <>
   inline bool     fuzzyCompare( const int &p1, const int &p2 )
{  return p1 == p2; }

template <typename T>
    inline bool    isErrDevLess( const T&p1, const T&p2, const T&eps )
{  return (abs(p1-p2)) < abs(eps);  }
template <>
    inline bool    isErrDevLess( const double &p1, const double &p2, const double &eps)
{  return int64_t(( abs(p1-p2) * 1000000000000. )) < int64_t( eps * 1000000000000. ); }
template <>
    inline bool    isErrDevLess( const float &p1, const float &p2, const float &eps)
{  return int64_t(( abs(p1-p2) * 100000.f )) < int64_t( eps * 100000.f ); }


template <typename T>
    inline bool    isErrDevGreater( const T&p1, const T&p2, const T&eps )
{  return (abs(p1-p2)) > abs(eps);  }
template <>
    inline bool    isErrDevGreater( const double &p1, const double &p2, const double &eps)
{  return int64_t(( abs(p1-p2) * 1000000000000. )) > int64_t( eps * 1000000000000. ); }
template <>
    inline bool    isErrDevGreater( const float &p1, const float &p2, const float &eps)
{  return int64_t(( abs(p1-p2) * 100000.f )) > int64_t( eps * 100000.f ); }


template <typename T>
    inline bool    isErrDevLessEqu( const T&p1, const T&p2, const T&eps )
{  return ! isErrDevGreater( p1,p2,eps); }
template <typename T>
    inline bool    isErrDevGreaterEqu(  const T&p1, const T&p2, const T&eps )
{  return ! isErrDevLess( p1, p2, eps ); }

//! round the number
//! @details this routine round the float number
//! @param v         [in] the value
//! @param point_num [in] the float point number your want
QXPACK_IC_API  double  decimalRound( double v, int point_num );

//! guess the separator in the path string
//! @details this routine will try to find '\' or '/' character in the path string
//! @note if no separator, return 0
QXPACK_IC_API  char    guessPathSeparator( const char *path );

//! trip path string as short path string by separator.
//! @details this function trip the path string from end to start.
//! @param src_path [in]  the source path string
//! @param dst      [out] target buffer, used for store the tripped string.
//! @param dst_size [in]  the dest. buffer size
//! @param sepa_num [in]  the separator number
//! @note  function will guess the separator
QXPACK_IC_API void tailPath (
    const char *src_path, char *dst, size_t dst_size, size_t sepa_num
);

//! simple LCG, generate the random value.
class QXPACK_IC_HIDDEN IcLCG {
private: uint32_t m_seed;
public :
    IcLCG( ) { m_seed = uint32_t( reinterpret_cast<uintptr_t>(this)); }
    IcLCG( uint32_t s ) { m_seed = s; }

    inline uint16_t value( )
    {
        m_seed = uint32_t( m_seed * 214013 + 2531011 );
        return uint16_t( m_seed >> 16 );
    }

    inline unsigned int  uintValue()
    {
        union { unsigned int uint_val; uint16_t u16_val[4]; } u_val;
        u_val.u16_val[0] = value(); u_val.u16_val[1] = value();
        u_val.u16_val[2] = value(); u_val.u16_val[3] = value();
        return ( u_val.uint_val >> 1 );
    }
};

//! simple UID counter, it is 64bit
typedef  uint64_t  qxic_uid_t;
class QXPACK_IC_HIDDEN  IcUIDCntr  {

private:
    std::atomic_flag  m_busy_lck; qxic_uid_t m_cntr;
public:
    explicit IcUIDCntr( ) : m_cntr( 0 ) { m_busy_lck.clear(); }

    static bool        isInvalidUID( qxic_uid_t v ) { return ( v == 0 ); }
    static qxic_uid_t  invalidUID( ) { return 0; }

    qxic_uid_t  fetch( )
    {
        int busy_cnt = 24;
        // --------------------------------------------------------------------
        // WARNING: by C++11 spec., test_and_set() return old value
        // HERE JUST loop while old value is true!
        // --------------------------------------------------------------------
        while ( m_busy_lck.test_and_set( std::memory_order_seq_cst )) { // acquire lock
            if ( busy_cnt -- > 0 ) { continue; }
            std::this_thread::yield(); busy_cnt = 24;
        }

        qxic_uid_t v = ++ m_cntr;
        if ( IcUIDCntr::isInvalidUID( v ) ) { v = ++ m_cntr; }

        m_busy_lck.clear( std::memory_order_seq_cst ); // release lock
        return v;
    }
};



}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
