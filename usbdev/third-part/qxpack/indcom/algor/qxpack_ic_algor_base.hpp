#ifndef QXPACK_IC_ALGOR_BASE_HPP
#define QXPACK_IC_ALGOR_BASE_HPP

#include <cmath>
#include <cfloat>
#include <cstdint>
#include <cstring>

#include <type_traits>
#include <utility>
#include <functional>

#ifndef QXPACK_IC_CXX11
#if __cplusplus >= 201103L || ( defined(_MSC_VER) && _MSC_VER >= 1800 )
#  define QXPACK_IC_CXX11
#else
#  error "this template require C++11 !"
#endif
#endif

// ////////////////////////////////////////////////////////////////////////////
// : API list
// ----------------------------------------------------------------------------
// zero          -- a method that used to clear memory to 0
// swap/swapIf   -- swap two element position
// min/max       -- return minimal/maximum element reference
// clamp         -- limit the element into a range [ min, max ]
// isInRange     -- check if the element in range [ min, max ]
// abs           -- return the absolute value of the element
// round/round64 -- return the integral type of the float 
// fuzzyIsNull   -- check if the float value is 0(Null)
// isNull        -- check if the integral value is 0
// fuzzyCompare  -- compare two float value is equal or not
// doIf          -- call std::function if condition meet
// signval       -- return 0 or 1 for type sign
// ////////////////////////////////////////////////////////////////////////////

namespace QxPack {
namespace IcAlgor {

// ============================================================================
// memory setup
// ============================================================================
template <typename T>
inline auto zero( T *m, size_t n = 1 ) -> T*
{
    static_assert( std::is_arithmetic<T>::value || std::is_pod<T>::value, "T must be POD or arithmetic!" );
    std::memset( m, 0, sizeof( T ) * n );
    return m;
}

template <typename T>
inline auto zero( T &m ) -> void
{   std::memset( m, 0, sizeof(T) ); }


// ============================================================================
// swap two elements
// ============================================================================
template <typename T>
inline auto  swap( T &a, T &b ) -> typename std::enable_if<std::is_class<T>::value>::type
{ std::swap( a, b ); }

template <typename T>
inline auto  swap( T &a, T &b ) -> typename std::enable_if<!std::is_class<T>::value>::type
{ T c = a; a = b; b = c; }

template <typename T>
inline auto  swapIf( T &a, T &b, bool sw ) ->  typename std::enable_if<std::is_class<T>::value>::type
{ if ( sw ) { std::swap( a, b );  }}

template <typename T>
inline auto  swapIf( T &a, T &b, bool sw ) ->  typename std::enable_if<!std::is_class<T>::value>::type
{ if ( sw ) { T c = a; a = b; b = c; }}

// ============================================================================
// minimal value / maximum value
// ============================================================================
template <typename T>
inline auto  min( const T &a, const T &b ) -> const T& { return ( a < b ? a : b ); }

template <typename T>
inline auto  max( const T &a, const T &b ) -> const T& { return ( a < b ? b : a ); }

// ============================================================================
// clamp the value in range
// ============================================================================
template <typename T>
inline auto  clamp( const T&v, const T&rg_min, const T&rg_max ) -> const T&
{  return min( max( v, min( rg_min, rg_max )), max( rg_min, rg_max ) ); }

template <typename T>
inline auto  isInRange( const T&v, const T&rg_min, const T&rg_max ) -> bool
{  return ( v >= min(rg_min, rg_max) && v <= max(rg_min,rg_max)); }

// ============================================================================
// return absolute value of T
// ============================================================================
template <typename T>
inline auto  abs( const T &t ) -> T { return t >= 0 ? t : -t; }

// ============================================================================
// round the value upper.
// ============================================================================
template <typename T>
inline auto  round( const T &d ) -> int { return int( std::round( d )); }

template <typename T>
inline auto  round64( const T &d ) -> int64_t { return int64_t( std::round(d)); }

// ============================================================================
// fuzzy check if value is zero.
// ============================================================================
namespace fuzzyIsNullHelper {
    template <typename T>
    inline auto  _fuzzyIsNullFloat( const T &v ) -> bool
    { return abs( double(v) ) <= 0.000000000001; }

    template <>
    inline auto  _fuzzyIsNullFloat( const float &v ) -> bool
    { return abs( v ) <= 0.00001f; }

    template <>
    inline auto  _fuzzyIsNullFloat( const double &v ) -> bool
    { return abs( v ) <= 0.000000000001; }

    template <typename T>
    inline auto  _fuzzyIsNullInt( const T &v ) -> bool
    { return v == 0; }

    template <typename T >
    inline auto  _fuzzyIsNull( const T &v, bool &ret ) -> typename std::enable_if<std::is_floating_point<T>::value>::type
    { ret = _fuzzyIsNullFloat( v ); }

    template <typename T >
    inline auto  _fuzzyIsNull( const T &v, bool &ret ) -> typename std::enable_if<!std::is_floating_point<T>::value>::type
    {
        static_assert( std::is_integral<T>::value, "T should be integral!" );
        ret = _fuzzyIsNullInt( v );
    }
}

template <typename T>
inline auto  fuzzyIsNull( const T &v ) -> bool
{  bool ret; fuzzyIsNullHelper::_fuzzyIsNull( v, ret ); return ret;  }

template <typename T>
inline auto  fuzzyIsNull( const T &v, unsigned int decm_num ) -> bool
{ return abs( double(v) ) <= std::pow(10,-int(decm_num)); }

template <typename T>
inline auto isNull( const T &d ) -> bool
{ bool ret; fuzzyIsNullHelper::_fuzzyIsNull( d, ret ); return ret; }


// ============================================================================
// fuzzy compare two value, check if them are equal
// ============================================================================
template <typename T>
inline auto  fuzzyCompare( const T &a, const T &b ) -> bool { return ( a == b ); }

template <>
inline auto  fuzzyCompare( const double &p1, const double & p2 ) -> bool
{  return (abs(p1 - p2) * 1000000000000. <= min(abs(p1), abs(p2)));  }

template <>
inline auto  fuzzyCompare( const float  &p1, const float  & p2 ) -> bool
{  return (abs(p1 - p2) * 100000.f <= min(abs(p1), abs(p2))); } 


// ============================================================================
// do function if condition meet
// ============================================================================
template <typename T = bool >
inline auto  doIf( const std::function<void(void*)> &func, T sw, void *ctxt ) -> void
{ if ( sw ) { func( ctxt ); } }

template <typename T = bool >
inline auto  doIf( const std::function<void(void)> &func, T sw ) -> void
{ if ( sw ) { func(); } }

// ============================================================================
// check sign value of type
// ============================================================================
template <typename T>
inline auto  signVal( const T &v ) -> int { return int( v ) > 0 ? 1 : 0; }

template <> inline auto signVal( const uint8_t  & ) -> int { return 0; }
template <> inline auto signVal( const uint16_t & ) -> int { return 0; }
template <> inline auto signVal( const uint32_t & ) -> int { return 0; }
template <> inline auto signVal( const uint64_t & ) -> int { return 0; }
template <> inline auto signVal( const int8_t  &v ) -> int { return  int( uint8_t (v) >> 7  ); }
template <> inline auto signVal( const int16_t &v ) -> int { return  int( uint16_t(v) >> 15 ); }
template <> inline auto signVal( const int32_t &v ) -> int { return  int( uint32_t(v) >> 31 ); }
template <> inline auto signVal( const int64_t &v ) -> int { return  int( uint64_t(v) >> 63 ); }

template <> inline auto signVal( const float &v ) -> int
{ const uint32_t *v2 = reinterpret_cast<const uint32_t*>(&v); return int(uint32_t(*v2) >> 31 ); }

template <> inline auto signVal( const double &v ) -> int
{ const uint64_t *v2 = reinterpret_cast<const uint64_t*>(&v); return int(uint64_t(*v2) >> 63 ); }




}
}

#endif
