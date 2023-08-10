#ifndef QXPACK_IC_ALGOR_ARYSTATS_HPP
#define QXPACK_IC_ALGOR_ARYSTATS_HPP

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
// aryMean         -- eval the mean of an array, T must be arightmatic
// aryZeroCentered --
// ----------------------------------------------------------------------------
// ////////////////////////////////////////////////////////////////////////////

namespace QxPack {
namespace IcAlgor {

// ============================================================================
// inner helper
// ============================================================================
namespace AryStatsHelper {

template <typename T, typename R>
auto    _aryMean( const T *ary, size_t num, R &ret ) -> typename std::enable_if<std::is_floating_point<T>::value>::type
{
    double sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
    size_t m4 = num / 4 * 4; size_t i = 0; auto ptr = ary;

    for ( ; i < m4; i += 4, ptr += 4 ) { sum0 += ptr[0]; sum1 += ptr[1]; sum2 += ptr[2]; sum3 += ptr[3]; }
    for ( ; i < num; i ++, ptr ++ ) { sum0 += ptr[0]; }

    ret = R( num > 0 ? ( sum0 + sum1 + sum2 + sum3 ) / double(num) : DBL_MAX );
}

template <typename T, typename R>
auto    _aryMean( const T *ary, size_t num, R &ret ) -> typename std::enable_if<!std::is_floating_point<T>::value>::type
{
    int64_t sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
    size_t m4 = num / 4 * 4; size_t i = 0; auto ptr = ary;

    for ( ; i < m4; i += 4, ptr += 4 ) { sum0 += ptr[0]; sum1 += ptr[1]; sum2 += ptr[2]; sum3 += ptr[3]; }
    for ( ; i < num; i ++, ptr ++ ) { sum0 += ptr[0]; }

    ret = R( num > 0 ? ( sum0 + sum1 + sum2 + sum3 ) / int64_t(num) : INT_MAX );
}

template <typename T>
auto    _arySub ( T *ary, size_t num, T val ) -> void
{
    size_t m4 = num / 4; size_t i = 0; auto ptr = ary;
    for ( ; i < m4; i += 4, ptr += 4 ) { ptr[0] -= val; ptr[1] -= val; ptr[2] -= val; ptr[3] -= val; }
    for ( ; i < num; i ++, ptr ++ ) { ptr[0] -= val; }
}

}

// ============================================================================
// eval the mean of the array
// ============================================================================
template <typename T, typename R>
auto    aryMean ( const  T *ary, size_t num ) -> R
{
    static_assert( std::is_arithmetic<T>::value, "T should be arithmetic!" );
    auto ret = R(0); AryStatsHelper::_aryMean( ary, num, ret );
    return ret;
}

//! @return the mean value
template <typename T>
auto    aryZeroCentered( T *ary, size_t num ) -> T
{
    static_assert( std::is_arithmetic<T>::value, "T should be arithmetic!" );
    auto mean = T(0); AryStatsHelper::_aryMean( ary, num, mean );
    AryStatsHelper::_arySub( ary, num, mean );
    return mean;
}



}
}
#endif
