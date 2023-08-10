#ifndef QXPACK_IC_ALGOR_PXMONO_HPP
#define QXPACK_IC_ALGOR_PXMONO_HPP


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
// monoFlipColor        -- flip mono color
// monoFlipVert         -- flip mono vertically
// fillWaveAryByMonoMsb -- fill the wave array value by mono (MSB) pixel
//
// ////////////////////////////////////////////////////////////////////////////
namespace QxPack {
namespace IcAlgor {

//! \brief flip the bits in the mono pixel matrix
//! \param  src_px  [in]  the source pixel
//! \param  dst_px  [out] output pixel matrix
//! \param  px_w    [in]  the pixel width
//! \param  px_h    [in]  the pixel height
//! \param  pitch   [in]  bytes per row
template <typename T = uint8_t >
auto     monoFlipColor (
    const T *src_px, T *dst_px, size_t px_w, size_t px_h, size_t pitch
) -> void
{
    static_assert( std::is_integral<T>::value && sizeof(T) == 1, "T should be uint8_t or int8_t." );
    if ( src_px == nullptr || dst_px == nullptr || pitch < 1 ) { return; }
    (void)(px_w);

    auto dp = dst_px;
    if ( src_px != dst_px ) { std::memcpy( dst_px, src_px, pitch ); }

    if ( sizeof( uintptr_t ) == 4 ) {
        for ( size_t h = 0; h < px_h; h ++, dp += pitch ) {
            auto dp_addr  = uintptr_t( dp );
            auto prev_rem = (( dp_addr + 3UL) & ~3UL ) - dp_addr;
            auto fix_num  = ( pitch - prev_rem ) / 4;
            auto suff_rem = pitch - ( prev_rem + fix_num * 4 );
            notAry( reinterpret_cast<uint8_t *>( dp ), prev_rem );
            notAry( reinterpret_cast<uint32_t*>( &dp[ prev_rem ]), fix_num );
            notAry( reinterpret_cast<uint8_t *>( &dp[ pitch - suff_rem ]), suff_rem );
        }
    } else if ( sizeof( uintptr_t ) == 8 ) {
        for ( size_t h = 0; h < px_h; h ++, dp += pitch ) {
            auto dp_addr  = uintptr_t( dp );
            auto prev_rem = (( dp_addr + 7UL ) & ~7UL ) - dp_addr;
            auto fix_num  = ( pitch - prev_rem ) / 8;
            auto suff_rem = pitch - ( prev_rem + fix_num * 8 );
            notAry( reinterpret_cast<uint8_t *>( dp ), prev_rem );
            notAry( reinterpret_cast<uint32_t*>( &dp[ prev_rem ]), fix_num );
            notAry( reinterpret_cast<uint8_t *>( &dp[ pitch - suff_rem ]), suff_rem );
        }
    } else {
        for ( size_t h = 0; h < px_h; h ++, dp += pitch ) {
            notAry( dp, pitch );
        }
    }
}

//! \brief flip mono vertically
//! \param dst_px   [in]  the pixel (mono) data pointer
//! \param px_w     [in]  the pixel (mono) width
//! \param px_h     [in]  the pixel (mono) height
//! \param pitch    [in]  bytes per row
template <typename T=uint8_t>
auto     monoFlipVert(
    T *dst_px, size_t px_w, size_t px_h, size_t pitch
) -> void
{
    static_assert( std::is_integral<T>::value && sizeof(T) == 1, "T should be uint8_t or int8_t." );
    if ( dst_px == nullptr || pitch < 2 ) { return; }
    (void)(px_w);

    auto half_px_h = px_h / 2;  auto s1 = dst_px; auto s2 = dst_px + pitch * ( px_h - 1 );
    for ( size_t h = 0; h < half_px_h; h ++, s1 += pitch, s2 -= pitch ) {
        swapAry( s1, s2, pitch );
    }
}



//! \brief  fill an wave data by mono data ( black is the data )
//! \param  px       [in]  the pixel ( mono ) data pointer
//! \param  px_w     [in]  the pixel ( mono ) data width in pixel
//! \param  px_h     [in]  the pixel ( mono ) data height in pixel
//! \param  px_pitch [in]  how many bytes a row
//! \param  ary      [out] the value array
//! \param  ary_len  [in]  array size
//! \note   the pixel Y coord. is the math coord.
template <typename T>
auto     fillWaveAryByMonoMsb ( // MSB order
    const uint8_t *px, size_t px_w, size_t px_h, size_t px_pitch,
    T *ary, size_t ary_len, bool is_px_wt, bool is_math_coord = true
) -> void
{
    static_assert( std::is_integral<T>::value, "array T must be integral type!" );
    if ( ary == nullptr || px == nullptr ) { return; }
    if ( px_w < 1 || px_pitch < 1 || px_h < 1 || ary_len < 1 ) { return; }
    if ( ( px_w + 7 ) / 8 > px_pitch ) { px_w = px_pitch / 8; }

    auto min_w = ( ary_len <= px_w ? ary_len : px_w );
    auto wks   = reinterpret_cast<uint8_t*>(std::malloc( ary_len ));
    std::memset( ary, 0, sizeof(T) * ary_len );
    std::memset( wks, 0, ary_len );

    // ----------------------------------------------------------------
    //  wks_curr | bv  |  use idx |  ( BK as data bv == 0 is data )
    // ----------------------------------------------------------------
    //      0      0       0
    //      0      1       1
    //      1      0       0
    //      1      1       0
    // ----------------------------------------------------------------
    T v[2]; v[0] = T(0);
    int s[4] = { 0, 1, 0, 0 }; int si; uint8_t bv, b8_xor;
    b8_xor = ( ! is_px_wt ? uint8_t(-1) : 0 );

    for ( size_t h = 0; h < px_h; h ++, px += px_pitch ) {
        auto p8 = min_w / 8; auto a = ary; auto w = wks;
        v[1] = ( is_math_coord ? T(px_h - h - 1) : T( h ) );

        for ( size_t w8 = 0; w8 < p8; w8 ++, a += 8, w += 8 ) {
            auto b8 = px[ w8 ] ^ b8_xor;

            bv = (( b8 >> 7 ) & 1 ); v[0] = a[0]; si = s[ bv | ( w[0] << 1 ) ]; w[0] |= bv; a[0] = v[ si ];
            bv = (( b8 >> 6 ) & 1 ); v[0] = a[1]; si = s[ bv | ( w[1] << 1 ) ]; w[1] |= bv; a[1] = v[ si ];
            bv = (( b8 >> 5 ) & 1 ); v[0] = a[2]; si = s[ bv | ( w[2] << 1 ) ]; w[2] |= bv; a[2] = v[ si ];
            bv = (( b8 >> 4 ) & 1 ); v[0] = a[3]; si = s[ bv | ( w[3] << 1 ) ]; w[3] |= bv; a[3] = v[ si ];
            bv = (( b8 >> 3 ) & 1 ); v[0] = a[4]; si = s[ bv | ( w[4] << 1 ) ]; w[4] |= bv; a[4] = v[ si ];
            bv = (( b8 >> 2 ) & 1 ); v[0] = a[5]; si = s[ bv | ( w[5] << 1 ) ]; w[5] |= bv; a[5] = v[ si ];
            bv = (( b8 >> 1 ) & 1 ); v[0] = a[6]; si = s[ bv | ( w[6] << 1 ) ]; w[6] |= bv; a[6] = v[ si ];
            bv = (( b8 >> 0 ) & 1 ); v[0] = a[7]; si = s[ bv | ( w[7] << 1 ) ]; w[7] |= bv; a[7] = v[ si ];
        }
        auto pr = min_w % 8;
        if ( pr > 0 ) {
            auto b8 = px[ ( min_w + 7 ) / 8 - 1 ] ^ b8_xor;
            switch( pr ) { // no "break", fallback
            case 7 : bv = (( b8 >> 1 ) & 1 ); v[0] = a[6]; si = s[ bv | ( w[6] << 1 ) ]; w[6] |= bv; a[6] = v[ si ];
            case 6 : bv = (( b8 >> 2 ) & 1 ); v[0] = a[5]; si = s[ bv | ( w[5] << 1 ) ]; w[5] |= bv; a[5] = v[ si ];
            case 5 : bv = (( b8 >> 3 ) & 1 ); v[0] = a[4]; si = s[ bv | ( w[4] << 1 ) ]; w[4] |= bv; a[4] = v[ si ];
            case 4 : bv = (( b8 >> 4 ) & 1 ); v[0] = a[3]; si = s[ bv | ( w[3] << 1 ) ]; w[3] |= bv; a[3] = v[ si ];
            case 3 : bv = (( b8 >> 5 ) & 1 ); v[0] = a[2]; si = s[ bv | ( w[2] << 1 ) ]; w[2] |= bv; a[2] = v[ si ];
            case 2 : bv = (( b8 >> 6 ) & 1 ); v[0] = a[1]; si = s[ bv | ( w[1] << 1 ) ]; w[1] |= bv; a[1] = v[ si ];
            case 1 : bv = (( b8 >> 7 ) & 1 ); v[0] = a[0]; si = s[ bv | ( w[0] << 1 ) ]; w[0] |= bv; a[0] = v[ si ];
            default: break;
            }
        }
    }

    std::free( wks );
}



}
}
#endif
