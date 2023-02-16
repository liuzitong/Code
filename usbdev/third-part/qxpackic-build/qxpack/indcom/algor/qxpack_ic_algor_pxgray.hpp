#ifndef QXPACK_IC_ALGOR_PXGRAY_HPP
#define QXPACK_IC_ALGOR_PXGRAY_HPP


#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <type_traits>
#include <utility>
#include <functional>

#include "qxpack_ic_arch_def.h"
#include "qxpack_ic_algor_ary.hpp"

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
// grayRemapColor -- remap the gray color of a 2D image
//
// ////////////////////////////////////////////////////////////////////////////

namespace QxPack {
namespace IcAlgor {

//! remap the gray image color
//! \param src_px       [in]  the source gray scale grid
//! \param dst_px       [out] the dest. gray scale grid
//! \param px_w         [in]  pixel width, for src_px and dst_px grid area
//! \param px_h         [in]  pixel height, for src_px and dst_px grid area
//! \param gray_tbl     [in]  the color map table
//! \param gray_tbl_sz  [in]  the color map table size.
//! \note if 'gray_tbl_sz' smaller than T + 1, that will automatically generate the mission
//!  color map table
template <typename T >
auto     grayRemapColor (
    const T *src_px,    T  *dst_px,
    size_t   px_w,      size_t  px_h,
    size_t   src_pitch, size_t  dst_pitch,
    const T *gray_tbl,  size_t  gray_tbl_sz
) -> void
{
    // ------------------------------------------------------------------------
    // condition check
    // ------------------------------------------------------------------------
    static_assert(
        std::is_integral<T>::value && sizeof(T) < sizeof(size_t),
        "T should be integral, and sizeof(T) < sizeof(size_t) !"
    );
    if ( src_px == nullptr || dst_px == nullptr || gray_tbl == nullptr ) { return; }
    if ( src_pitch < 1 || dst_pitch < 1 || gray_tbl_sz < 1 ) { return; }

    // ------------------------------------------------------------------------
    // do mapping
    // ------------------------------------------------------------------------
    auto m8 = px_w / 8;
    for ( size_t h = 0; h < px_h; h ++, src_px += src_pitch, dst_px += dst_pitch ) {
        size_t w = 0; auto sp = src_px; auto dp = dst_px;
        for ( ; w < m8; w += 8, sp += 8, dp += 8 ) {
            dp[0] = gray_tbl[ sp[0]]; dp[1] = gray_tbl[ sp[1]];
            dp[2] = gray_tbl[ sp[2]]; dp[3] = gray_tbl[ sp[3]];
            dp[4] = gray_tbl[ sp[4]]; dp[5] = gray_tbl[ sp[5]];
            dp[6] = gray_tbl[ sp[6]]; dp[7] = gray_tbl[ sp[7]];
        }
        for ( ; w < px_w; w ++, sp ++, dp ++ ) {
            dp[0] = gray_tbl[ sp[0] ];
        }
    }
}






}
}
#endif
