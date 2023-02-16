#ifndef QXPACK_IC_ALGOR_SHUFFLE_HPP
#define QXPACK_IC_ALGOR_SHUFFLE_HPP

#include <cstdlib>
#include <chrono>  // C++11
#include <cstdint>
#include <utility> // C++11
#include <random>  // C++11

#include "qxpack_ic_algor_base.hpp"

namespace QxPack {
namespace IcAlgor {

// ////////////////////////////////////////////////////////////////////////////
//
// Shuffle algorithm
// : shuffle is a method used to re-arrange an array
//
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// knuth suffle ( fair method )
// ============================================================================
template <typename T>
auto     knuthShuffle ( T *ary, int ary_sz ) -> void
{
   if ( ary_sz - 1 < 1 || ary == nullptr ) { return; }

   std::chrono::steady_clock::time_point now_s = std::chrono::steady_clock::now();
   int64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>( now_s.time_since_epoch() ).count();
   std::default_random_engine e; e.seed( unsigned(ms) );
   std::uniform_int_distribution<int> u( 0, ary_sz - 1 );

   int swap_pos = 0;
   for ( int i = ary_sz - 1, j = 0; i >= 0; i --, j ++ ) {
       swap_pos = u(e) % ( i + 1 );
       swap( ary[j], ary[ swap_pos + j ] );
    }
}




}
}
#endif
