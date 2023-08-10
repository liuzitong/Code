#ifndef QXPACK_IC_RANDCOLOR_API_C
#define QXPACK_IC_RANDCOLOR_API_C

#include "qxpack_ic_randcolor_api.h"

typedef struct { int r, g, b; } rgb_t;

// ////////////////////////////////////////////////////////////////////////////
//                      local function
// ////////////////////////////////////////////////////////////////////////////
static inline double gAbs( double n ) { return ( n >= 0 ? n : -n ); }
static inline bool   gFuzzIsNull( double n ) { return gAbs( n ) <= 0.000000000001; }

// ============================================================================
// HSV to RGBA
// ============================================================================
static void  gHsvToRgba( double h, double s, double v, rgb_t *rsl )
{
    double h6 = h * 6;          // if h is in [0,360],  hi = |h/60| % 6
    int h_i = ( int )( h6 );    //                      f  = h/60 - hi
    double f  = h6 - h_i;
    double p = v * ( 1 - s );
    double q = v * ( 1 - f * s );
    double t = v * ( 1 - ( 1 - f ) * s );

    double r,g,b;
    switch( h_i ) {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    case 5: r = v; g = p; b = q; break;
    default: break;
    }

    rsl->r = (int)( r * 256 );
    rsl->g = (int)( g * 256 );
    rsl->b = (int)( b * 256 );
}

// ============================================================================
// clamp value
// ============================================================================
inline static double gClamp( double v, double min, double max )
{
    if ( v <= min ) { return min; }
    if ( v >= max ) { return max; }
    return v;
}

// ============================================================================
// gen random
// ============================================================================
inline static uint16_t  gRand( uint32_t *s )
{
    *s = ( uint32_t )( *s * 214013 + 2531011 );
    return ( uint16_t )( *s >> 16 );
}

// ////////////////////////////////////////////////////////////////////////////
//                      implement API
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// generate color by Hue
// ============================================================================
QXPACK_IC_HIDDEN void  QxPack_IcRandColorByHue(
    int seed, double s, double v, int (* Cb )( void*, int,int,int ), void *cb_ctxt
) {
    // ------------------------------------------------------------------------
    // cond. test
    // ------------------------------------------------------------------------
    if ( Cb == NULL ) { return; }

    // ------------------------------------------------------------------------
    //                  initialize stage
    // ------------------------------------------------------------------------
    s = gClamp( s, 0, 1.0 );
    v = gClamp( v, 0, 1.0 );
    uint32_t rs = ( seed == 0 ? ( uint32_t )( Cb ) : ( uint32_t )( seed ));

    // ------------------------------------------------------------------------
    //                  process stage
    // ------------------------------------------------------------------------
    int ret = 1; rgb_t rgb;
    for(; ret != 0; ) {
        double h = ( gRand( &rs ) / 65535.0 + 0.618034 );
        if ( h > 1.0 ) { h -= 1.0; }
        gHsvToRgba( h, s, v, &rgb );
        ret = Cb( cb_ctxt, rgb.r, rgb.g, rgb.b );
    }
}



#endif
