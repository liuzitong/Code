#ifndef QXPACK_IC_GLOBAL_CXX
#define QXPACK_IC_GLOBAL_CXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include "qxpack_ic_global.hxx"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <memory>

namespace QxPack {

// ============================================================================
// guess the separator character
// ============================================================================
QXPACK_IC_API char  guessPathSeparator( const char *path )
{
    if ( path == nullptr ) { return 0; }
    while ( *path != 0 ) {
        if ( *path == '\\') { return '\\'; }
        if ( *path == '/' ) { return '/';  }
        ++ path;
    }
    return 0;
}

// ============================================================================
// trip the path string as short string by separator
// ============================================================================
QXPACK_IC_API void tailPath (
    const char *src, char *dst, size_t max_sz,  size_t sepa_num
) {
    // ------------------------------------------------------------------------
    // cehck param. and setup the last byte to zero.
    // ------------------------------------------------------------------------
    if ( max_sz < 1 || src == nullptr || dst == nullptr ) { return; }
    dst[ -- max_sz ] = 0;
    if ( sepa_num == 0 ) { sepa_num = size_t( -1 ); }

    // ------------------------------------------------------------------------
    // skip separator until find the last separator
    // ------------------------------------------------------------------------
    size_t src_len  = std::strlen( src );
    size_t read_cnt = 0;
    size_t sepa_cnt = 0;
    char   sepa = guessPathSeparator( src );
    const char *sp = src + src_len;
    while ( read_cnt < max_sz  &&  sepa_cnt < sepa_num  &&  sp >= src ) {
        -- sp; ++ read_cnt;
        if ( *sp == sepa ) { ++ sepa_cnt; }
    }

    // ------------------------------------------------------------------------
    // copy the data into dest. buffer
    // ------------------------------------------------------------------------
    if ( sp == src ) {
        std::memcpy( dst, sp, read_cnt );
    } else {
        if ( max_sz - read_cnt > 1 ) { // space is enough to add '..' string
            dst[0] = dst[1] = '.';
            dst += 2; max_sz -= 2;
            std::memcpy( dst, sp, read_cnt );
        } else {
            std::memcpy( dst, sp, read_cnt );
            if ( max_sz > 1 ) {
                dst[0] = dst[1] = '.';
            }
        }
    }
    dst[ read_cnt ] = 0;
}

// ============================================================================
// decimal number round
// ============================================================================
static const double  deci_scale[] = {
    1, 1e-1, 1e-2, 1e-3, 1e-4,  1e-5, 1e-6, 1e-7, 1e-8
};
QXPACK_IC_API  double  decimalRound( double v, int n )
{
    double  sc; bool is_neg = false;
    double  rsl;

    if ( v < 0 ) { v = -v; is_neg = true;}
    if ( n < 0 ) { n = -n; }

    if ( n < 8 ) { sc = deci_scale[ n ]; }
    else {
      sc = 1.0; while ( n -- > 0  &&  sc > 0 ) { sc *= 0.1; }
    }

    if ( ! fuzzyIsNull( sc )  &&  sc * 0.1 > 0 ) {
        rsl = v + sc * 0.5;
        rsl = rsl - std::fmod( rsl, sc );
    } else {
        rsl = v;
    }

    return ( ! is_neg ? rsl : -rsl );
}











}

#endif
