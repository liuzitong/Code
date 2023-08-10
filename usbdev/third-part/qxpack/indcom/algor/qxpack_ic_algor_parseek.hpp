#ifndef QXPACK_IC_ALGOR_PARSEEK_HPP
#define QXPACK_IC_ALGOR_PARSEEK_HPP

#include <type_traits>
#include <utility>
#include <cstdint>
#include <functional>
#include "qxpack_ic_algor_base.hpp"

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
// parSeekBs -- seek single parameter by callback function
//
// ////////////////////////////////////////////////////////////////////////////
namespace QxPack {
namespace IcAlgor {

// ============================================================================
// seek the parameter by binary search
// ============================================================================
template <typename T >
auto   paramSeekBs( T par_beg, T par_end, const std::function<int(const T&)> &seek_cb, int itr_cnt = -1 ) -> std::pair<bool,T>
{
    static_assert( std::is_arithmetic<T>::value, "T must be arithmetic!" );
    T  curr = ( par_beg + par_end ) / T(2); T last_curr = curr; int rsl = 0;

    do {
        rsl = seek_cb( curr );
        if ( rsl == 0 ) { break; } // found.

        last_curr = curr;
        if ( rsl >  0 ) { // current param. result is bigger than target
            par_end = curr;
        } else {          // current param. result is less than target
            par_beg = curr;
        }

        curr = ( par_beg + par_end ) / T(2);

        if ( itr_cnt > 0 ) { if ( -- itr_cnt < 0 ) { break; }} // not found, iterator count is too large

    } while ( true );

    return ( rsl == 0 ? std::pair<bool,T>( true, curr ) : std::pair<bool,T>( false, T()));
}

}
}
#endif
