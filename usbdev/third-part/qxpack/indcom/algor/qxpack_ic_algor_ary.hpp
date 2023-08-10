#ifndef QXPACK_IC_ALGOR_ARY_HPP
#define QXPACK_IC_ALGOR_ARY_HPP

#include "qxpack_ic_algor_base.hpp"

#include <cstring>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <cmath>
#include <cstdio>
#include <cstring>

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
// revAryOrder       -- reverse the array order. e.g  0 1 2 ==> 2 1 0
// mirrorExtendAry   -- extend the array, head and tail. e.g  0 1 2 ==>  2 1 0 0 1 2 2 1 0
// mirrorExtendAryHalf    -- extend the array, just half part. e.g   0 1 2 3 ==>  1 0 0 1 2 3 3 2
// mirrorExtendAryHalfBAP -- extend the array, just half part. ( but border as pivot )
// selInAryRef       -- enum all elements in array, and compare each pair.
// maxInAryRef       -- find the maximum element reference in array
// minInAryRef       -- find the minimum element reference in array
// dotMulAry         -- A .* B
// linearInterpolAry  -- linear interpolation method to scale array to another array
// linearInterpolAry2 -- same as linearInterpolAry, but use two type
// xorAry -- do xor binary operation by a value on array
// andAry -- do and binary operation by a value on array
// orAry  -- do or  binary operation by a value on array
// notAry -- do not binary operation on array 
// sumAry -- do sum the array operation
// svrAry -- same value range in array
// rolAry -- roll shift array
// addAry -- add value on the array   ( since 2021/02/24, 2021/07/02 )
// mulAry -- mul value on the array   ( since 2021/02/24 )
// clampAry -- clamp array value in range ( since 2021/02/24 )
// ----------------------------------------------------------------------------
// ////////////////////////////////////////////////////////////////////////////

namespace QxPack {
namespace IcAlgor {

// ============================================================================
// fill array
// ============================================================================
//! fill the array by T
//! \param  d  [in]  the target array pointer
//! \param  v  [in]  value
//! \param  n  [in]  the target array element number
template <typename T>
auto  fillAry ( T *d, const T &v, size_t n ) -> void
{
#if 0
    for ( size_t i = 0; i < n; i ++ ){ d[i] = v; }
#else
    if ( sizeof(T) > 1 ) {
        size_t m8 = n / 8 * 8; size_t i = 0;
        for ( ; i < m8; i += 8, d += 8 ) {
            d[0] = v; d[1] = v; d[2] = v; d[3] = v;
            d[4] = v; d[5] = v; d[6] = v; d[7] = v;
        }
        for ( ; i < n; i ++, d ++ ) { d[0] = v; }
    } else {
        std::memset( d, v, n * sizeof(T)); // sizeof(T) == 1
    }
#endif
}


// ============================================================================
// reverse the array order
// ============================================================================
//! reverse the array to another array
//! \param  src  [in]  source array pointer
//! \param  dst  [in]  target array pointer
//! \param  n    [in]  the element number ( src and dst )
template <typename T>
auto  revAryOrder( const T *src, T *dst, size_t n ) -> void
{
#if 0
    for ( size_t i = 0, j = n - 1; i < n; i ++, j -- ) { dst[i] = src[j]; }
#else
    size_t m8 = n / 8 * 8; size_t i = 0;
    auto s = src + n - 8; auto d = dst;
    for ( ; i < m8; i += 8, s -= 8, d += 8 ) {
        d[0] = s[7]; d[1] = s[6]; d[2] = s[5]; d[3] = s[4];
        d[4] = s[3]; d[5] = s[2]; d[6] = s[1]; d[7] = s[0];
    }
    s += ( 8 - 1 );
    for ( ; i < n; i ++, s --, d ++ ) {
        d[0] = s[0];
    }
#endif
}

//! reverse the array order ( IN-PLACE )
//! \param  dst  [in_out]  the array pointer
//! \param  n    [in]  the array element number
template <typename T>
auto  revAryOrder( T *dst, size_t n ) -> typename std::enable_if<!std::is_arithmetic<T>::value>::type
{
   auto half_n = n / 2;
   for ( size_t i = 0, j = n - 1; i < half_n; i ++, j -- ) { swap( dst[i], dst[j]); }
}

template <typename T>
auto  revAryOrder( T *dst, size_t n ) -> typename std::enable_if<std::is_arithmetic<T>::value>::type
{
#if 0
    auto half_n = n / 2;
    for ( size_t i = 0, j = n - 1; i < half_n; i ++, j -- ) { swap( dst[i], dst[j]); }
#else
    auto half_n = n / 2; T tp[4];
    auto m8 = half_n / 4 * 4; size_t i = 0;
    auto s1 = dst; auto s2 = dst + n - 4;
    for ( ; i < m8; i += 4, s1 += 4, s2 -= 4 ) {
        tp[0] = s1[0]; tp[1] = s1[1]; tp[2] = s1[2]; tp[3] = s1[3];
        s1[0] = s2[3]; s1[1] = s2[2]; s1[2] = s2[1]; s1[3] = s2[0];
        s2[2] = tp[1]; s2[3] = tp[0]; s2[0] = tp[3]; s2[1] = tp[2];
    }
    s2 += ( 4 - 1 );
    for ( ; i < half_n; i ++, s1 ++, s2 -- ) {
        tp[0] = s1[0]; s1[0] = s2[0]; s2[0] = tp[0];
    }
#endif
}

// ============================================================================
// do mirror extend data
// ============================================================================
//! do array mirror extend
//! \param  src    [in]  the source array
//! \param  src_sz [in]  source array size
//! \param  dst    [out] the target array
//! \param  dst_sz [in]  target array size
//! \return  the required dst size.
//! \note   the minimal dst_sz should be src_sz * 3
//! \example src { 1 2 3 4 } ==> dst { 4 3 2 1 1 2 3 4 4 3 2 1 }
//! \example src { 1 2 3 }   ==> dst { 3 2 1 1 2 3 3 2 1 }
template <typename T>
auto  mirrorExtendAry( const T *src, size_t src_sz, T *dst, size_t dst_sz ) -> size_t
{
    static_assert(
        std::is_pod<T>::value || std::is_arithmetic<T>::value, "T must be POD or arithmetic type!"
    );
    if ( dst == nullptr || src_sz * 3 > dst_sz ) { return src_sz * 3; }

    revAryOrder( src, dst, size_t( src_sz ));
    std::memcpy( dst + src_sz, src, src_sz * sizeof(T) );
    std::memcpy( dst + src_sz + src_sz, dst, src_sz * sizeof(T) );
    return src_sz * 3;
}

//! do mirror extend the array ( half part version )
//! \param  src    [in]  the source array
//! \param  src_sz [in]  source array size
//! \param  dst    [out] the target array.
//! \param  dst_sz [in]  target array size
//! \return  the required dst size.
//! \note   'dst' can be null if user want to known how many number should use in target array.
//! \example  src { 1 2 3 4 } ==> dst{ 2 1 1 2 3 4 4 3 }
//! \example  src { 1 2 3  }  ==> dst{ 1 1 2 3 3 }
template <typename T> // NOTE: the src_sz should be even number!
auto  mirrorExtendAryHalf( const T *src, size_t src_sz, T *dst, size_t dst_sz ) -> size_t
{
    static_assert(
        std::is_pod<T>::value || std::is_arithmetic<T>::value, "T must be POD or arithmetic type!"
    );
    auto tgt_sz = ( src_sz > 1 ? src_sz * 2 - ( src_sz % 2 ) : 3 );
    if ( dst == nullptr || tgt_sz > dst_sz ) { return tgt_sz; }

    auto half_src_sz = ( src_sz > 1 ? src_sz / 2 : 1 );
    revAryOrder( src, dst, half_src_sz );
    std::memcpy( &dst[ half_src_sz ], src, src_sz * sizeof(T) );
    revAryOrder( &src[ src_sz - half_src_sz ], &dst[ src_sz + half_src_sz ], half_src_sz  );

    return tgt_sz;
}

//! do mirror extend the array ( half and boundary element as pivot  )
//! \param  src    [in]  the source array
//! \param  src_sz [in]  source array size
//! \param  dst    [out] the target array.
//! \param  dst_sz [in]  target array size
//! \return  the required dst size.
//! \example  src{ 1 2 3 4 } ==> dst{ 3 2 1 2 3 4 3 2 }
//! \example  src{ 1 2 3 }   ==> dst{ 2 1 2 3 2 }
template <typename T> // NOTE: the src_sz should be even number!
auto  mirrorExtendAryHalfBAP( const T *src, size_t src_sz, T *dst, size_t dst_sz ) -> size_t
{
    static_assert(
        std::is_pod<T>::value || std::is_arithmetic<T>::value, "T must be POD or arithmetic type!"
    );
    auto tgt_sz = ( src_sz > 1 ? src_sz * 2 - ( src_sz % 2 ) : 3 );
    if ( dst == nullptr || tgt_sz > dst_sz ) { return tgt_sz; }

    if ( src_sz > 1 ) {
        auto half_src_sz = src_sz / 2;
        revAryOrder( &src[1], dst, half_src_sz );
        std::memcpy( &dst[ half_src_sz ], src, src_sz * sizeof(T) );
        revAryOrder( &src[ src_sz - half_src_sz - 1 ], &dst[ src_sz + half_src_sz ], half_src_sz  );
    } else {
        fillAry( dst, src[0], dst_sz );
    }

    return tgt_sz;
}

// ============================================================================
// enum all elements, return the last selected element
// ============================================================================
template <typename T>
class  selInAryRefLess {
public: inline auto  operator() ( const T& sel, const T& tgt ) const -> bool { return sel < tgt; }
};
template <typename T>
class  selInAryRefGreater {
public: inline auto  operator() ( const T& sel, const T& tgt ) const -> bool { return sel > tgt; }
};

//! select each element in array
//! \return the selected element information, a pair [ position, value ref. ]
//! \param  ary [in] the array pointer
//! \param  n   [in] the array element number
//! \param  sel [in] the functor.
//! \details  this function enum all elements, and compare selected element with current element.
//!  if functor return true, it will remember current as selected element.
template <typename T, class Sel  >
auto     selInAryRef( const T *ary, size_t n, const Sel &sel ) -> typename std::pair<size_t, const T&>
{
    const T *e = &ary[0]; size_t p = 0;
    for ( size_t i = 1; i < n; i ++ ) { if ( sel( *e, ary[i] ) ) { e = &ary[ p = i]; } }
    return  std::pair<size_t,const T&>( p, *e );
}

//! find the maximum element in this array
//! \param  ary [in] the array pointer
//! \param   n  [in] the array element number
//! \return the maximum element information: pair [ position, elem ref. ]
template <typename T>
inline auto  maxInAryRef( const T *ary, size_t n ) -> typename std::pair<size_t, const T&>
{ return selInAryRef( ary, n, selInAryRefLess<T>() );  }

//! find the minimal element in this array
//! \param  ary [in] the array pointer
//! \param   n  [in] the array element number
//! \return the minimal element information: pair [ position, elem ref. ]
template <typename T>
inline auto  minInAryRef( const T *ary, size_t n ) -> typename std::pair<size_t, const T&>
{ return selInAryRef( ary, n, selInAryRefGreater<T>()); }

// ============================================================================
// interpolation array to another array
// ============================================================================
template <typename T>
class linearInterpolAryEval {
public: inline auto  operator() ( const T& a, const T& b, const double &f ) const -> T
        { return f * ( b - a ) + a; /*b * f + a * ( 1.0 - f );*/ }
};

//! do linear interpolation on source array for target size
//! \param  src      [in]  source, array pointer
//! \param  src_num  [in]  source, array element number
//! \param  dst      [out] target, array pointer
//! \param  dst_num  [in]  target, array element number
//! \param  ev       [in]  functor, return the result of ( a, b, percentage )
template <typename T, class Eval = linearInterpolAryEval<T> > // use Eval to do more custom operation ( like clamp and evaluate )
auto     linearInterpolAry ( const T *src, size_t src_num, T *dst, size_t dst_num, const Eval &ev = linearInterpolAryEval<T>() ) -> void
{
    // src_num and dst_num must be greater than 0
    if ( src_num < 1 || dst_num < 1 ) { std::abort(); }

    // directly copy if source and dest. number are same
    if ( src_num == dst_num ) {
        std::memcpy( dst, src, sizeof(T) * src_num );
        return;
    }

    // spec. case..
    if ( src_num < 2 ) {
        for ( size_t i = 0; i < dst_num; i ++ ) { dst[i] = src[0]; }
        return;
    }
    if ( src_num == 2 && fuzzyCompare( src[0], src[1])) {
        for ( size_t i = 0; i < dst_num; i ++ ) { dst[i] = src[0]; }
        return;
    }

    // do routine ( src_num > 1 )
    // in future, can test uintptr shift to do optimization.
    {
        double step = ( src_num - 1.0 ) / ( dst_num - 1.0 );
        double pos  = 0.0;
        dst[0] = src[0]; pos += step;
        if ( dst_num < 2 ) { return; }
        for ( size_t i = 1; i < dst_num - 1; i ++, pos += step ) {
            double int_part, fract_part;
            fract_part = std::modf( pos, &int_part );
            size_t pos_int = size_t( int_part );
            dst[i] = ev( src[ pos_int ], src[ pos_int + 1 ], fract_part );
        }
        dst[ dst_num - 1 ] = src[ src_num - 1 ];
    }
}


template <typename T1, typename T2, class Eval = linearInterpolAryEval<T2> > // use Eval to do more custom operation ( like clamp and evaluate )
auto     linearInterpolAry2 ( const T1 *src, size_t src_num, T2 *dst,  size_t dst_num, const Eval &ev = linearInterpolAryEval<T2>() ) -> void
{
    static_assert( ! std::is_same< typename std::decay<T1>::type, typename std::decay<T2>::type >::value, "T1 and T2 is same, should use linearInterpolAry()!" );

    // spec. case..
    if ( src_num < 2 ) {
        for ( size_t i = 0; i < dst_num; i ++ ) { dst[i] = T2( src[0] ); }
        return;
    }
    if ( src_num == 2 && fuzzyCompare( src[0], src[1])) {
        for ( size_t i = 0; i < dst_num; i ++ ) { dst[i] = src[0]; }
        return;
    }

    // do routine ( src_num > 1 )
    // in future, can test uintptr shift to do optimization.
    {
        double step = ( src_num - 1.0 ) / ( dst_num - 1.0 );
        double pos  = 0.0;
        dst[0] = src[0]; pos += step;
        if ( dst_num < 2 ) { return; }
        for ( size_t i = 1; i < dst_num - 1; i ++, pos += step ) {
            double int_part, fract_part;
            fract_part = std::modf( pos, &int_part );
            size_t pos_int = size_t( int_part );
            dst[i] = ev( T2(src[ pos_int ]), T2(src[ pos_int + 1 ]), fract_part );
        }
        dst[ dst_num - 1 ] = T2( src[ src_num - 1 ] );
    }
}


// ============================================================================
// like matlab .* method
// ============================================================================
template <typename Tx>
class  dotMulAryClamp {
public: inline auto  operator() ( const Tx &v ) const -> Tx { return v; }
};

template <typename Tx, typename Ty, class Clamp >
inline auto  dotMulAry( Tx *rsl, const Tx *ary1, const Ty *ary2, size_t ary_sz, const Clamp &cl ) -> void
{  for ( size_t n = 0; n < ary_sz; n ++ ) { rsl[n] = cl( ary1[n] * ary2[n] ); }}


// ============================================================================
// bool operation
// ============================================================================
template <typename T>
auto     xorAry(  T *ary, const T &v, size_t n ) -> void
{
    static_assert( std::is_integral<T>::value, "T should be integral!");
    if ( n < 1 ) { return; }
    auto dp = ary; auto m8 = n / 8 * 8;
    size_t i = 0;
    for ( ; i < m8; i += 8, dp += 8 ) {
        dp[0] ^= v; dp[1] ^= v; dp[2] ^= v; dp[3] ^= v;
        dp[4] ^= v; dp[5] ^= v; dp[6] ^= v; dp[7] ^= v;
    }
    for ( ; i < n; i ++ ) { ary[i] ^= v; }
}

template <typename T>
auto     andAry( T *ary, const T &v, size_t n ) -> void
{
    static_assert( std::is_integral<T>::value, "T should be integral!");
    if ( n < 1 ) { return; }
    auto dp = ary; auto m8 = n / 8 * 8;
    size_t i = 0;
    for ( ; i < m8; i += 8, dp += 8 ) {
        dp[0] &= v; dp[1] &= v; dp[2] &= v; dp[3] &= v;
        dp[4] &= v; dp[5] &= v; dp[6] &= v; dp[7] &= v;
    }
    for ( ; i < n; i ++ ) { ary[i] &= v; }
}

template <typename T>
inline auto  notAry( T *ary, size_t n ) -> void
{ xorAry( ary, T(-1), n ); }

template <typename T>
auto     orAry( T *ary, const T &v, size_t n ) -> void
{
    static_assert( std::is_integral<T>::value, "T should be integral!");
    if ( n < 1 ) { return; }
    auto dp = ary; auto m8 = n / 8 * 8;
    size_t i = 0;
    for ( ; i < m8; i += 8, dp += 8 ) {
        dp[0] |= v; dp[1] |= v; dp[2] |= v; dp[3] |= v;
        dp[4] |= v; dp[5] |= v; dp[6] |= v; dp[7] |= v;
    }
    for ( ; i < n; i ++ ) { ary[i] |= v; }
}

// ============================================================================
// sum the array
// ============================================================================
template <typename T, typename R = T >
auto     sumAry ( const T *ary, size_t n ) -> R
{
    static_assert( std::is_arithmetic<T>::value, "T should be arithmetic!");
    auto m8 = n / 8 * 8; size_t i = 0; R ra,rb,rc = 0; auto a = ary;
    for ( ; i < m8; i += 8, a += 8 ) {
        ra = a[0] + a[1] + a[2] + a[3];
        rb = a[4] + a[5] + a[6] + a[7];
        rc += ( ra + rb );
    }
    for ( ; i < n; i ++, a ++ ) { rc += a[0]; }

    return rc;
}


// ============================================================================
// swap two arrays
// ============================================================================
template <typename T>
auto     swapAry ( T *s1, T *s2, size_t n ) -> typename std::enable_if<std::is_arithmetic<T>::value>::type
{
    auto m4 = n / 4 * 4; size_t i = 0;
    T tm[4];
    for ( ; i < m4; i += 4, s1 += 4, s2 += 4 ) {
        tm[0] = s1[0]; tm[1] = s1[1]; tm[2] = s1[2]; tm[3] = s1[3];
        s1[0] = s2[0]; s1[1] = s2[1]; s1[2] = s2[2]; s1[3] = s2[3];
        s2[0] = tm[0]; s2[1] = tm[1]; s2[2] = tm[2]; s2[3] = tm[3];
    }
    for ( ; i < n; i ++, s1 ++, s2 ++ ) {
        tm[0] = s1[0]; s1[0] = s2[0]; s2[0] = tm[0];
    }
}

template <typename T>
auto     swapAry ( T *s1, T *s2, size_t n ) -> typename std::enable_if<!std::is_arithmetic<T>::value>::type
{
    for ( size_t i = 0; i < n; i ++, s1 ++, s2 ++ ) {
        swap( s1[0], s2[0] );
    }
}

// ============================================================================
// find the same element range in array
// ============================================================================
//! \param  ary  [in] the array pointer
//! \param   n   [in] the array element number
//! \param   v_pos [in] the value position in the array
//! \return pair [n,n] for invalid.
template <typename T>
auto     svrAry( const T *ary, size_t n, size_t v_pos ) -> std::pair<size_t,size_t>
{
    if ( ary == nullptr ||  v_pos > n ) { return std::pair<size_t,size_t>( n, n ); }
    if ( n < 2 ) { return std::pair<size_t,size_t>( 0, 0 ); }
    auto &vr = ary[ v_pos ];

    // find low bound
    auto lo = &ary[ v_pos ];
    while ( lo > ary && *lo == vr ) { -- lo; }
    if ( *lo != vr ) { ++ lo; }

    // find high bound
    auto hi  = &ary[ v_pos ];  auto lmt = ary + n - 1;
    while ( hi < lmt && *hi == vr ) { ++ hi; }
    if ( *hi != vr ) { -- hi; }

    return std::pair<size_t,size_t>( lo - ary, hi - ary );
}

// ============================================================================
// roll shift array
// =============================================================================
template <typename T>
auto     rolAry ( const T *ary, T *dst, size_t n, size_t sh ) -> void
{
    if ( ary == nullptr || dst == nullptr || n < 2 ) { return; }
    sh = sh % n;
    if ( sh > 0  ) {
        std::memcpy( dst, ary + sh, sizeof(T) * ( n - sh ) );
        std::memcpy( dst + n - sh, ary, sizeof(T) * sh );
    } else {
        std::memcpy( dst, ary, n * sizeof(T));
    }
}

template <typename T>
auto     rolAry ( T *ary, size_t n, size_t sh ) -> void
{
    if ( n < 1 || ( sh % n ) == 0 ) { return; }
    if ( n * sizeof(T) < 64 ) {
        T tmp[64/sizeof(T)];
        rolAry( ary, tmp, n, sh );
        std::memcpy( ary, tmp, n * sizeof(T) );
    } else {
        auto tmp = reinterpret_cast<T*>( std::malloc( sizeof(T) * n ));
        rolAry( ary, tmp, n, sh );
        std::memcpy( ary, tmp, n * sizeof(T));
        std::free( tmp );
    }
}

// ============================================================================
// add value on the array
// ============================================================================
template <typename T>
auto     addAry( T *ary, size_t n, const T &v ) -> void
{
    static_assert( std::is_arithmetic<T>::value, "T should be arithmetic!" );

    size_t m8 = n / 8 * 8; size_t i = 0;
    auto ptr = ary;
    for ( ; i < m8; i += 8, ptr += 8 ) {
        ptr[0] += v; ptr[1] += v; ptr[2] += v; ptr[3] += v;
        ptr[4] += v; ptr[5] += v; ptr[6] += v; ptr[7] += v;
    }
    for ( ; i < n; i ++ ) { ary[i] += v; }
}

template <typename T>
auto     addAry ( T *ary1, const T* ary2, size_t n ) -> void
{
    static_assert( std::is_arithmetic<T>::value, "T should be arithmetic!" );

    size_t m8 = n / 8 * 8; size_t i = 0;
    auto ptr1 = ary1; auto ptr2 = ary2;
    for (; i < m8; i += 8, ptr1 += 8, ptr2 += 8 ) {
        ptr1[0] += ptr2[0]; ptr1[1] += ptr2[1]; ptr1[2] += ptr2[2]; ptr1[3] += ptr2[3];
        ptr1[4] += ptr2[4]; ptr1[5] += ptr2[5]; ptr1[6] += ptr2[6]; ptr1[7] += ptr2[7];
    }
    for ( ; i < n; i ++ ) { ary1[i] += ary2[i]; }
}


// ============================================================================
// multiply value on array
// ============================================================================
template <typename T>
auto     mulAry ( T *ary, size_t n, const T &v ) -> void
{
    static_assert( std::is_arithmetic<T>::value, "T should be arithmetic!" );

    size_t m8 = n / 8 * 8; size_t i = 0;
    auto ptr = ary;
    for ( ; i < m8; i += 8, ptr += 8 ) {
        ptr[0] *= v; ptr[1] *= v; ptr[2] *= v; ptr[3] *= v;
        ptr[4] *= v; ptr[5] *= v; ptr[6] *= v; ptr[7] *= v;
    }
    for ( ; i < n; i ++ ) { ary[i] *= v; }
}

// ============================================================================
// clamp array value in range
// ============================================================================
template <typename T>
auto     clampAry ( T *ary, size_t n, const T &rg_min, const T &rg_max ) -> void
{
    static_assert( std::is_arithmetic<T>::value, "T should be arithmetic!" );

    const T& min_v = min( rg_min, rg_max );
    const T& max_v = max( rg_min, rg_max );

    // ------------------------------------------------------------------------
    //  v - min  |  max - v
    // ------------------------------------------------------------------------
    //    0           0         00 -- return v
    //    0           1         01 -- return max ( v > max )
    //    1           0         10 -- return min ( v < min )
    //    1           1         11 -- invalid ( return v )
    // ------------------------------------------------------------------------
    size_t m4 = n / 4 * 4; size_t i = 0;  auto ptr = ary;

    {
    const T* tp0[ 4 ], *tp1[ 4], *tp2[ 4 ], *tp3[ 4 ];
    tp0[0] = &min_v; tp0[1] = &max_v; tp0[2] = &min_v; tp0[3] = &min_v;
    tp1[0] = &min_v; tp1[1] = &max_v; tp1[2] = &min_v; tp1[3] = &min_v;
    tp2[0] = &min_v; tp2[1] = &max_v; tp2[2] = &min_v; tp2[3] = &min_v;
    tp3[0] = &min_v; tp3[1] = &max_v; tp3[2] = &min_v; tp3[3] = &min_v;

    int  vsm[4], msv[4];

    for ( ; i < m4; i += 4, ptr += 4 ) {
        tp0[0] = &ptr[0]; tp1[0] = &ptr[1];  tp2[0] = &ptr[2]; tp3[0] = &ptr[3];
        tp0[3] = &ptr[0]; tp1[3] = &ptr[1];  tp2[3] = &ptr[2]; tp3[3] = &ptr[3];

        vsm[0] = signVal( ptr[0] - min_v );  vsm[1] = signVal( ptr[1] - min_v );
        vsm[2] = signVal( ptr[2] - min_v );  vsm[3] = signVal( ptr[3] - min_v );
        msv[0] = signVal( max_v - ptr[0] );  msv[1] = signVal( max_v - ptr[1] );
        msv[2] = signVal( max_v - ptr[2] );  msv[3] = signVal( max_v - ptr[3] );

        ptr[0] = *( tp0[ ( vsm[0] << 1 ) | msv[0] ] );
        ptr[1] = *( tp1[ ( vsm[1] << 1 ) | msv[1] ] );
        ptr[2] = *( tp2[ ( vsm[2] << 1 ) | msv[2] ] );
        ptr[3] = *( tp3[ ( vsm[3] << 1 ) | msv[3] ] );
    }
    }

    for ( ; i < n; i ++ ) {
        ary[i] = ( ary[i] >= min_v ? ( ary[i] <= max_v ? ary[i] : max_v ) : min_v );
    }
}


}
}
#endif
