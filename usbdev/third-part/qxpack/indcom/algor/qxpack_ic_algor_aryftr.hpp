#ifndef QXPACK_IC_ALGOR_ARYFTR_HPP
#define QXPACK_IC_ALGOR_ARYFTR_HPP

#include "qxpack_ic_algor_ary.hpp"

#include <cstring>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <cmath>
#include <cstdio>
#include <cstdlib>

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
// AryFtrKrnlWorkBuff    -- a class that used to keep workspace for some filter
// boxFtrAry             -- do box filter on array
// uptrndFixFtrAry       -- all value should be uptrends
// ----------------------------------------------------------------------------
// ////////////////////////////////////////////////////////////////////////////

namespace QxPack {
namespace IcAlgor {

// ////////////////////////////////////////////////////////////////////////////
// the buffer memory layout
// [ work space ( size = kernel size ) | mirror tail space ( half - kernel size )
// e.g, an array [ 1, 2, 3, 4, 5, 6, 7 ], krnl size is 5, after called init():
//    [ 3,2, 1,2,3 | 6,5 ] ==>
//    the [ 3,2,1,2,3 ] is work space, and [6,5] is the mirror tail space
// ////////////////////////////////////////////////////////////////////////////
template <typename T >
class  AryFtrKrnlWorkBuff final {
private:
    T *m_wk_dat; uint16_t *m_wk_idx; size_t  m_wk_sz;
protected:
    auto  _freeBuff() -> void
    {  std::free( m_wk_idx ); m_wk_dat = nullptr; m_wk_idx = nullptr; }

    auto  _initBuff( uint16_t req_sz, size_t ary_sz ) -> void
    {
        if ( req_sz % 2 == 0 ) { req_sz ++; } // enforce odd.
        if ( ary_sz < 1 ) { ary_sz = 1; }

        auto half_krn = size_t( req_sz / 2 );
        if ( half_krn >= ary_sz ) { // clamp the kernel size
            // e.g: ary_sz == 2, half_krnl = 2 ( krnl = 5 ),
            //      adjust to half_krnl = 1, krn_sz = 1 * 2 + 1 = 3
            half_krn = ary_sz - 1;
        }
        m_wk_sz  = half_krn * 2 + 1;

        auto r_idx_sz = ( sizeof(uint16_t) * m_wk_sz + sizeof(intptr_t) - 1 ) / sizeof(intptr_t) * sizeof( intptr_t );
        auto mem_sz   = r_idx_sz + sizeof(T) * ( m_wk_sz + ( half_krn > 0 ? half_krn : 1 ) + 1 ); // append 1 for sentinel

        auto buff = reinterpret_cast<uint8_t*> ( std::malloc( mem_sz ));
        m_wk_idx  = reinterpret_cast<uint16_t*>( buff );
        m_wk_dat  = reinterpret_cast<T*>( buff + r_idx_sz );
        m_wk_dat[ m_wk_sz + ( half_krn > 0 ? half_krn : 1 ) + 1 - 1 ] = T(0x5a);
    }

public :
    AryFtrKrnlWorkBuff( uint16_t req_sz, size_t ary_sz )
    {
        static_assert( std::is_arithmetic<T>::value, "T should be arithmetic!" );
        m_wk_dat = nullptr; m_wk_idx = nullptr; m_wk_sz = 0;
        _initBuff( req_sz, ary_sz );
    }

    ~AryFtrKrnlWorkBuff( ) { _freeBuff(); }

    inline auto  workBuffSize()       const -> size_t   { return m_wk_sz;  }
    inline auto  mirrorTailBuffSize() const -> size_t   { auto r = m_wk_sz / 2; return ( r > 0 ? r : 1 ); }
    inline auto  mirrorTailBuff()     const -> const T* { return m_wk_dat + m_wk_sz; }
    inline auto  workIndex()          const -> const uint16_t* { return m_wk_idx; }
    inline auto  workBuff() -> T* { return m_wk_dat; }
    inline auto  isSentOK() const -> bool { return ( m_wk_dat[ m_wk_sz + ( m_wk_sz / 2 > 0 ? m_wk_sz / 2 : 1 ) + 1 - 1 ] == T(0x5a)); }

    //! \return true means already initied.
    auto  init( const T *ary, size_t ary_sz ) -> bool
    {
        auto hk = m_wk_sz / 2;
        if ( ary == nullptr || ary_sz < hk || m_wk_sz < 1 ) { return false; }
        if ( hk > 0 ) {
            // init work index buffer
            for ( size_t i = 0; i < m_wk_sz; i ++ ) { m_wk_idx[i] = uint16_t( i + 1 ); }
            m_wk_idx[ m_wk_sz - 1 ] = 0;

            // init work buffer
            revAryOrder( &ary[1], &m_wk_dat[0], hk );
            std::memcpy( &m_wk_dat[ hk ], &ary[0], ( hk + 1 ) * sizeof(T));

            // init mirror tail buffer
            revAryOrder( &ary[ ary_sz - 1 - hk ], &m_wk_dat[ m_wk_sz ],  hk );
        } else {
            m_wk_idx[0] = 0;
            m_wk_dat[0] = ary[0]; m_wk_dat[1] = ary[0];
        }
        return true;
    }
};


//! \param  ary    [in_out] the array pointer
//! \param  ary_sz [in] the array element length
//! \param  kw     [in] the kernel work buffer
//! \return true for done.
template <typename T >
auto     boxFtrAry ( T *ary, size_t ary_sz, AryFtrKrnlWorkBuff<T> &kw ) -> bool
{
    static_assert( std::is_arithmetic<T>::value, "T should be arithmetic type!" );
    if ( ary_sz < 2 || kw.workBuffSize() < 3 ) { return false; } // if kw.size() is 1, need not to do solve
    if ( ary_sz < kw.workBuffSize() / 2 + 1  ) { return false; } // bad kernel work buffer for this array
    if ( ! kw.init( ary, ary_sz )) { return false; }

    auto dp = kw.workBuff();
    auto ip = kw.workIndex();
    auto hk = kw.workBuffSize() / 2;
    auto inv_ksz = 1.0 / kw.workBuffSize();
    auto lmt_pos = ary_sz - hk;

    // normal part
    auto sum = sumAry<T,double>( dp, kw.workBuffSize() );
    ary[0]   = T( sum * inv_ksz );
    auto idx = 0;
    auto inp = const_cast<const T*>( &ary[ hk ] );  // ensure inp point to edge of kernel, so inp[1] is the new element
    for ( size_t n = 1; n < lmt_pos; n ++ ) {
        sum    += double( inp[n] - dp[idx]); // drop current old element, and append new element
        dp[idx] = inp[n];
        idx     = ip[idx];
        ary[n]  = T( sum * inv_ksz );
    }

    // last part
    inp = kw.mirrorTailBuff();
    for ( size_t n = lmt_pos; n < ary_sz; n ++, inp ++ ) {
        sum    += double( inp[0] - dp[idx] );
        dp[idx] = inp[0];
        idx     = ip[idx];
        ary[n]  = T( sum * inv_ksz );
    }

    return true;
}


//! \details this filter ensure all values are uptrends
//! \param  ary     [in_out]  the array pointer
//! \param  ary_sz  [in]  the array size
template <typename T>
auto     uptrndFixFtrAry ( T *ary, size_t ary_sz ) -> bool
{
    if ( ary_sz < 1 || ary == nullptr ) { return true; }
    auto *e = &ary[0];
    for ( size_t i = 1; i < ary_sz; i ++ ) {
        if      ( ary[i] < *e ) { ary[i] = *e; }
        else if ( ary[i] > *e ) { e = &ary[i]; }
        else { }
    }
    return true;
}



}
}
#endif
