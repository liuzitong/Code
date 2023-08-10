#ifndef QXPACK_IC_ALGOR_RDFTSG_HPP
#define QXPACK_IC_ALGOR_RDFTSG_HPP


#include <cstdlib>
#include <cstdio>
#include <vector>
#include <cstring>
#include <tuple>
#include <type_traits>
#include <utility>

#include "qxpack_ic_algor_base.hpp"
#include "qxpack_ic_algor_fftsgsubrt.hpp"
#include "qxpack_ic_algor_pot.hpp"
#include "qxpack_ic_algor_ary.hpp"

#ifndef QXPACK_IC_CXX11
#if __cplusplus >= 201103L || ( defined(_MSC_VER) && _MSC_VER >= 1800 )
#  define QXPACK_IC_CXX11
#else
#  error "this template require C++11 !"
#endif
#endif

namespace QxPack {
namespace IcAlgor {

//! \brief low API for allocate an work-space data for 'rdftsg'
template <typename T = double >
auto     rdftsgAllocIPW ( int e_num ) -> typename std::tuple<int,int*,T*>
{
    static_assert(
        std::is_floating_point<T>::value && ( sizeof(T) == 4 || sizeof(T) == 8 ),
        "T must be float! e.g float or double."
    );
    auto ip = reinterpret_cast<int*>( std::malloc( size_t( e_num + 2 ) * sizeof(int) ));
    auto wk = reinterpret_cast<T*>  ( std::malloc( size_t( e_num * 5 / 4 ) * sizeof(T) ));
    ip[0] = 0; // setup init flag.
    return std::tuple<int,int*,T*>( e_num, ip, wk );
}

//! \brief low API for free the work-space data that 'rdftsgAllocIPW' returned
template <typename T = double >
auto     rdftsgFreeIPW ( const std::tuple<int,int*,T*> &par ) -> void
{
    static_assert(
        std::is_floating_point<T>::value && ( sizeof(T) == 4 || sizeof(T) == 8 ),
        "T must be float! e.g float or double."
    );
    auto ip = std::get<1>( par );
    auto  w = std::get<2>( par );
    if ( ip != nullptr ) { std::free( ip ); }
    if (  w != nullptr ) { std::free(  w ); }
}

//! \brief  Real Discrete Fourier Transform / Inverse Real Discrete Fourier Transform
//! \param  ipw   [in]       the workspace that allocated by 'rdftsgAllocIpw'
//! \param  a     [in_out]   the input data / output data pointer.
//! \param  isgn  [in]       >= 0 means convert 'a' to fft data, < 0 means convert 'a' to orignal data.
//! \param  auto_sc_in_bw [in]  while isgn < 0, this flag will cause orignal data will be corrected.
//! \note   Normally, in IFFT ( convert fft data to orignal data ), the result data should be correct by user. \n
//!   but this routine can automatically scale data by setting 'auto_sc_in_bw' to true.
template <typename T = double >
auto     rdftsg ( const std::tuple<int,int*,T*> &ipw,  T *a, int isgn, bool auto_sc_in_bw = true ) -> void
{
    static_assert(
        std::is_floating_point<T>::value && ( sizeof(T) == 4 || sizeof(T) == 8 ),
        "T must be float! e.g float or double."
    );
    auto  n = std::get<0>( ipw );
    auto ip = std::get<1>( ipw );
    auto  w = std::get<2>( ipw );
    T xi;   int nw, nc;

    nw = ip[0];
    if (n > (nw << 2)) {
        nw = n >> 2;
        FFTSG::makewt( nw, ip, w );
    }
    nc = ip[1];
    if (n > (nc << 2)) {
        nc = n >> 2;
        FFTSG::makect(nc, ip, w + nw);
    }
    if ( isgn >= 0 ) {
        if (n > 4) {
            FFTSG::cftfsub(n, a, ip, nw, w);
            FFTSG::rftfsub(n, a, nc, w + nw);
        } else if (n == 4) {
            FFTSG::cftfsub(n, a, ip, nw, w);
        }
        xi    = a[0] - a[1];
        a[0] += a[1];
        a[1]  = xi;
    } else {
        a[1]  = 0.5 * (a[0] - a[1]);
        a[0] -= a[1];
        if (n > 4) {
            FFTSG::rftbsub(n, a, nc, w + nw);
            FFTSG::cftbsub(n, a, ip, nw, w);
        } else if (n == 4) {
            FFTSG::cftbsub(n, a, ip, nw, w);
        } else { }

        // patched, for scale data to back..
        if ( auto_sc_in_bw ) { FFTSG::scaleData( a, n ); }
    }
}

// ////////////////////////////////////////////////////////////////////////////
//
// Solver for rdft ( High Level API )
//
// ////////////////////////////////////////////////////////////////////////////
template <typename T = double>
class  RdftSgSolver final {

public :
    enum FFtType {
        FFtType_HalfRawOrder = 0, FFtType_HalfCorrectedOrder = 1
    };

    enum PaddingMode {
        PaddingMode_Zero = 0
    };

private:
    std::tuple<int,int*,T*>  m_ipw;
    PaddingMode   m_pad_mode;

protected:
    auto  makeIpwNull() -> void { m_ipw = std::tuple<int,int*,T*>( 0, nullptr, nullptr ); }
    auto  freeIpw()     -> void { rdftsgFreeIPW( m_ipw ); makeIpwNull(); }
    auto  ensureIpw ( int num ) -> void
    {
        auto n = std::get<0>(m_ipw);
        if ( n < 1 || n != num ) {
            freeIpw();
            m_ipw = rdftsgAllocIPW( num );
        }
    }

    template <typename VT = std::vector<T>>
    auto  makePOTSrc( const VT &src ) -> VT
    {
        auto  n  = src.size();
        auto  n2 = int( upperToNextPOT( src.size() ));
        if (  n == n2 ) { return src; }

         if ( n2 < 0 ) {
            std::fprintf( stderr, "RdftSgSolver<T>::toFFT(): source size can not round up to POT. ( n = %d )\n", n );
            std::abort();
        }

        VT rsl;
        rsl.resize( n2 );
        std::memcpy( rsl.data(), src.data(), n * sizeof(T));

        switch( m_pad_mode ) {
        case  PaddingMode_Zero : zero( & rsl.data()[ n ], n2 - n ); break;
        default: zero( & rsl.data()[ n ], n2 - n ); break;
        }

        return rsl;
    }

public :
    explicit RdftSgSolver( PaddingMode pm = PaddingMode_Zero );
    ~RdftSgSolver( ) { freeIpw(); }


    template <typename TV = std::vector<T>>
    auto  toFFT  ( const TV &src, FFtType ft = FFtType_HalfRawOrder ) -> TV
    {
        static_assert( ! std::is_same<std::vector<T>,std::decay<TV>>::value, "TV must be std::vector<T>!" );

        if ( src.empty()) { return TV(); }

        // makesure the input data are POT size
        TV src_tmp;
        if ( ! isPOT( src.size() ) ) { src_tmp = makePOTSrc( src ); }
        const TV *sp = ( src_tmp.empty() ? &src : &src_tmp );

        // allocate IPW if needed
        auto sp_sz = sp->size();
        ensureIpw( sp->size() );

        // calc FFT
        TV rsl; rsl.reserve( sp_sz + 2 );
        switch( ft ) {
        case FFtType_HalfCorrectedOrder : {
            rsl.resize ( sp_sz + 2 );
            std::memcpy( rsl.data(), sp->data(), sp_sz * sizeof(T));
            rdftsg( m_ipw, rsl.data(), 1 );
            auto rsl_dp = rsl.data();
            auto pivot  = rsl_dp[1]; rsl_dp[1] = 0;
            rsl_dp[ sp_sz ] = pivot; rsl_dp[ sp_sz + 1 ] = 0;
        } break;

        default : { // half raw order, the rsl[1] is pivot element
            rsl.resize( sp_sz );
            std::memcpy( rsl.data(), sp->data(), sp_sz * sizeof(T));
            rdftsg( m_ipw, rsl.data(), 1 );
        } break;
        }

        return rsl;
    }


    template <typename TV = std::vector<T>>
    auto  toData ( const TV &fft, FFtType ft = FFtType_HalfRawOrder ) -> TV
    {
        static_assert( ! std::is_same<std::vector<T>,std::decay<TV>>::value, "TV must be std::vector<T>!" );

        auto fft_sz = fft.size();
        if ( fft_sz < 2 ) { return TV(); }

        // calc actual sie
        auto act_sz = fft_sz;
        switch( ft ) {
        case FFtType_HalfCorrectedOrder : act_sz = fft_sz - 2; break;
        default : break;
        }
        if ( ! isPOT( act_sz ) ) { return TV(); }

        // allocate IPW if needed
        ensureIpw( act_sz );

        // calc data
        TV rsl;
        rsl.reserve( act_sz );
        rsl.resize ( act_sz  );
        std::memcpy( rsl.data(), fft.data(), act_sz * sizeof(T));

        switch( ft ) {
        case FFtType_HalfCorrectedOrder : {
            rsl.data()[1] = fft.data()[ fft_sz - 2 ];
        } break;
        default: break;
        }

        rdftsg( m_ipw, rsl.data(), -1, true );

        return rsl;
    }


};

template <typename T>
RdftSgSolver<T> :: RdftSgSolver ( PaddingMode pm )
{
    static_assert (
        std::is_floating_point<T>::value && ( sizeof(T) == 4 || sizeof(T) == 8 ),
        "T must be float! e.g float or double."
    );
    m_pad_mode = pm;
    makeIpwNull();
}





}
}

#endif

