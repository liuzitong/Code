#ifndef QXPACK_IC_ALGOR_ARYDET_HPP
#define QXPACK_IC_ALGOR_ARYDET_HPP

#include <cstring>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>

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
// thrGtPosDetAry -- detect the threshold value on array, output position
// ----------------------------------------------------------------------------
// ////////////////////////////////////////////////////////////////////////////

namespace QxPack {
namespace IcAlgor {

//! detect the threshold value position on array
//! \param  ary     [in] the array pointer
//! \param  ary_sz  [in] the array element number
//! \param  pos_ary [out] the position array used to store the position
//! \param  pos_ary_sz [in] the maximum of position array
//! \param  thr     [in] the threshold value
//! \return how many positions was detected
//! \warning  comparator is >, not >=
template <typename T>
auto  thrGtPosDetAry ( const T *ary, size_t ary_sz, size_t *pos_ary, size_t pos_ary_sz, const T &thr ) -> size_t
{
    if ( ary == nullptr || pos_ary == nullptr ) { return 0; }

    size_t thr_num = 0;
    for ( size_t i = 0; i < ary_sz && thr_num < pos_ary_sz; i ++ ) {
        if ( ary[ i ] > thr ) { pos_ary[ thr_num ++ ] = i; }
    }
    return thr_num;
}




}
}
#endif
