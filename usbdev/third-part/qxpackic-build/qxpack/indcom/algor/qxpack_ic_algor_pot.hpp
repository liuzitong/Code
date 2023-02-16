#ifndef QXPACK_IC_ALGOR_POT_HPP
#define QXPACK_IC_ALGOR_POT_HPP

#include <type_traits>
#include <utility>
#include <cstdint>

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
// isPOT          -- check if the number is power of two
// upperToNextPOT -- round up the value, and makesure the value is power of two
// lowerToPrevPOT -- round own the value, and makesure the value is power of two
//
// ////////////////////////////////////////////////////////////////////////////
namespace QxPack {
namespace IcAlgor {

// ============================================================================
// check if the T is power of two
// ============================================================================
template <typename T>
inline auto  isPOT( const T &v ) -> bool
{
   static_assert( std::is_integral<T>::value, "required intergral value!" );
   static_assert( std::is_unsigned<T>::value, "required unsigned  value!" );
   return  v && !( v & (v - 1));
}

// ============================================================================
// upper the unsigned integral value to power of 2
// ============================================================================
template <typename T>
inline auto  upperToNextPOT( const T &v ) -> T
{
   static_assert( std::is_integral<T>::value, "required intergral value!" );
   static_assert( std::is_unsigned<T>::value, "required unsigned  value!" );
    T  p = 1; while ( p < v ) { p += p; } return p;
}

template <>
inline auto  upperToNextPOT( const uint32_t &v ) -> uint32_t
{
    auto n = v - 1;
    n |= n >> 1; n |= n >> 2; n |= n >> 4; n |= n >> 8;
    n |= n >> 16;
    return ++ n;
}

template <>
inline auto  upperToNextPOT( const uint64_t &v ) -> uint64_t
{
    auto n = v - 1;
    n |= n >> 1;   n |= n >> 2;   n |= n >> 4; n |= n >> 8;
    n |= n >> 16;  n |= n >> 32;
    return ++ n;
}

// ============================================================================
// lower the unsigned integral value to power of 2
// ============================================================================
template <typename T>
inline auto  lowerToPrevPOT( const T &v ) -> T
{  T t = upperToNextPOT( v ); return isPOT( t ) ? t : t >> 1; }


}
}

#endif
