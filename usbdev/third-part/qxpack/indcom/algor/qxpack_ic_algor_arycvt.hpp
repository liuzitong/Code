#ifndef QXPACK_IC_ALGOR_ARYCVT_HPP
#define QXPACK_IC_ALGOR_ARYCVT_HPP

#include "qxpack_ic_algor_base.hpp"

#include <cstring>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <cmath>
#include <cstdio>

#ifndef QXPACK_IC_CXX11
#if __cplusplus >= 201103L || ( defined(_MSC_VER) && _MSC_VER >= 1800 )
#  define QXPACK_IC_CXX11
#else
#  error "this template require C++11 !"
#endif
#endif

// ////////////////////////////////////////////////////////////////////////////
// : API List
// ----------------------------------------------------------------------------
// cvtTypeAry -- convert the types of an array to another array
// ----------------------------------------------------------------------------
// ////////////////////////////////////////////////////////////////////////////

namespace QxPack {
namespace IcAlgor {

// ============================================================================
// inner helper
// ============================================================================
namespace AryCvtHelper {

struct CvtTypeAryTag_Default {};
struct CvtTypeAryTag_DblToInt{};
struct CvtTypeAryTag_FltToInt{};

template <typename T1, typename T2>
struct CvtTypeAryDispatch { using tag = CvtTypeAryTag_Default; };

template <> struct CvtTypeAryDispatch<double,int8_t>   { using tag = CvtTypeAryTag_DblToInt; };
template <> struct CvtTypeAryDispatch<double,int16_t>  { using tag = CvtTypeAryTag_DblToInt; };
template <> struct CvtTypeAryDispatch<double,int32_t>  { using tag = CvtTypeAryTag_DblToInt; };
template <> struct CvtTypeAryDispatch<double,int64_t>  { using tag = CvtTypeAryTag_DblToInt; };
template <> struct CvtTypeAryDispatch<double,uint8_t>  { using tag = CvtTypeAryTag_DblToInt; };
template <> struct CvtTypeAryDispatch<double,uint16_t> { using tag = CvtTypeAryTag_DblToInt; };
template <> struct CvtTypeAryDispatch<double,uint32_t> { using tag = CvtTypeAryTag_DblToInt; };
template <> struct CvtTypeAryDispatch<double,uint64_t> { using tag = CvtTypeAryTag_DblToInt; };
template <> struct CvtTypeAryDispatch<float,int8_t>   { using tag = CvtTypeAryTag_FltToInt; };
template <> struct CvtTypeAryDispatch<float,int16_t>  { using tag = CvtTypeAryTag_FltToInt; };
template <> struct CvtTypeAryDispatch<float,int32_t>  { using tag = CvtTypeAryTag_FltToInt; };
template <> struct CvtTypeAryDispatch<float,int64_t>  { using tag = CvtTypeAryTag_FltToInt; };
template <> struct CvtTypeAryDispatch<float,uint8_t>  { using tag = CvtTypeAryTag_FltToInt; };
template <> struct CvtTypeAryDispatch<float,uint16_t> { using tag = CvtTypeAryTag_FltToInt; };
template <> struct CvtTypeAryDispatch<float,uint32_t> { using tag = CvtTypeAryTag_FltToInt; };
template <> struct CvtTypeAryDispatch<float,uint64_t> { using tag = CvtTypeAryTag_FltToInt; };

// ============================================================================
// default converter
// ============================================================================
template <typename T1, typename T2>
auto     _cvtTypeAry ( const T1 *s1, T2 *s2, size_t n, CvtTypeAryTag_Default, int flag = 0 ) -> void
{
    static_assert(
        std::is_arithmetic<T1>::value && std::is_arithmetic<T2>::value,
        "T1 and T2 should be arithmetic type!"
    );
    auto m8 = n / 8 * 8; size_t i = 0;
    auto sp = s1; auto dp = s2; (void)(flag);
    for ( ; i < m8; i += 8, sp += 8, dp += 8 ) {
        dp[0] = T2( sp[0] ); dp[1] = T2( sp[1] ); dp[2] = T2( sp[2] ); dp[3] = T2( sp[3] );
        dp[4] = T2( sp[4] ); dp[5] = T2( sp[5] ); dp[6] = T2( sp[6] ); dp[7] = T2( sp[7] );
    }
    for ( ; i < n; i ++ ) { s2[i] = T2( s1[i] ); }
}

// ============================================================================
// double to int
// ============================================================================
template <typename T1, typename T2 >
auto     _cvtTypeAry ( const T1 *s1, T2 *s2, size_t n, CvtTypeAryTag_DblToInt, int flag = 0 ) -> void
{
    (void)(flag);
    auto m4 = n / 4 * 4; size_t i = 0;
    union { double dbl; int64_t itg; } u[4]; int64_t s[4];

    auto sp = s1; auto dp = s2;
    for ( ; i < m4; i += 4, sp += 4, dp += 4 ) {
         u[0].dbl  = sp[0]; u[1].dbl = sp[1]; u[2].dbl = sp[2]; u[3].dbl = sp[3];
         s[0] = u[0].itg & ~0x7fffffffffffffffLL;  s[1] = u[1].itg & ~0x7fffffffffffffffLL;
         s[2] = u[2].itg & ~0x7fffffffffffffffLL;  s[3] = u[3].itg & ~0x7fffffffffffffffLL;
         u[0].itg &= 0x7fffffffffffffffLL;   u[1].itg &= 0x7fffffffffffffffLL;
         u[2].itg &= 0x7fffffffffffffffLL;   u[3].itg &= 0x7fffffffffffffffLL;
         u[0].dbl += 0.5;  u[1].dbl += 0.5;  u[2].dbl += 0.5;  u[3].dbl += 0.5;
         u[0].itg |= s[0]; u[1].itg |= s[1]; u[2].itg |= s[2]; u[3].itg |= s[3];
         dp[0] = T2( u[0].dbl ); dp[1] = T2( u[1].dbl ); dp[2] = T2( u[2].dbl ); dp[3] = T2( u[3].dbl );
    }
    for ( ; i < n; i ++ ) {
        u[0].dbl  = s1[i];  s[0] = u[0].itg & ~0x7fffffffffffffffLL;
        u[0].itg &= 0x7fffffffffffffffLL;
        u[0].dbl += 0.5;   u[0].itg |= s[0];
        s2[i] = T2( u[0].dbl );
    }
}

// ============================================================================
// float to int
// ============================================================================
template <typename T1, typename T2 >
auto     _cvtTypeAry ( const T1 *s1, T2 *s2, size_t n, CvtTypeAryTag_FltToInt, int flag = 0 ) -> void
{
    (void)(flag);
    auto m4 = n / 4 * 4; size_t i = 0;
    union { float dbl; int32_t itg; } u[4]; int32_t s[4];

    auto sp = s1; auto dp = s2;
    for ( ; i < m4; i += 4, sp += 4, dp += 4 ) {
         u[0].dbl  = sp[0]; u[1].dbl = sp[1]; u[2].dbl = sp[2]; u[3].dbl = sp[3];
         s[0] = u[0].itg & ~0x7fffffffLL;  s[1] = u[1].itg & ~0x7fffffffLL;
         s[2] = u[2].itg & ~0x7fffffffLL;  s[3] = u[3].itg & ~0x7fffffffLL;
         u[0].itg &= 0x7fffffffLL; u[1].itg &= 0x7fffffffLL;
         u[2].itg &= 0x7fffffffLL; u[3].itg &= 0x7fffffffLL;
         u[0].dbl += 0.5;  u[1].dbl += 0.5;  u[2].dbl += 0.5;  u[3].dbl += 0.5;
         u[0].itg |= s[0]; u[1].itg |= s[1]; u[2].itg |= s[2]; u[3].itg |= s[3];
         dp[0] = T2( u[0].dbl ); dp[1] = T2( u[1].dbl ); dp[2] = T2( u[2].dbl ); dp[3] = T2( u[3].dbl );
    }
    for ( ; i < n; i ++ ) {
        u[0].dbl = s1[i];  s[0] = u[0].itg & ~0x7fffffffLL;
        u[0].itg &= 0x7fffffffLL; u[0].dbl += 0.5; u[0].itg |= s[0];
        s2[i] = T2( u[0].dbl );
    }
}

}

template <typename T1, typename T2>
auto     cvtTypeAry ( const T1 *s1, T2 *s2, size_t n, int flag = 0 ) -> typename std::enable_if<std::is_same<T1,T2>::value>::type
{  (void)(flag); if ( s1 != s2 ) { std::memcpy( s2, s1, sizeof(T1) * n ); }}

template <typename T1, typename T2>
auto     cvtTypeAry ( const T1 *s1, T2 *s2, size_t n, int flag = 0 ) -> typename std::enable_if<!std::is_same<T1,T2>::value>::type
{  AryCvtHelper::_cvtTypeAry( s1, s2, n, typename AryCvtHelper::CvtTypeAryDispatch<T1,T2>::tag{}, flag ); }



}
}
#endif
