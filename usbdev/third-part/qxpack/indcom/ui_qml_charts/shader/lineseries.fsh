#ifndef version
#ifdef GL_ES
#define version 100
#else
#define version 120
#endif
#endif

uniform lowp float qt_Opacity;

uniform lowp vec4  line_color;
uniform lowp float line_smooth;
uniform lowp float line_size;
//uniform lowp float rdrc_x, rdrc_y, rdrc_width, rdrc_height;
//uniform lowp float rdwc_width, rdwc_height;
//mediump vec4  gl_FragCoord; // NOTE: the frag coord. is relative to the top render window!

// NOTE!!!
// 'vT' this value is being linear interpolated by GPU
// the vertex processor passed this value for each vertex
// and here it's value changed.
varying lowp float vT; // range is [ 0.0, 1.0 ]

#define PI 3.14159265359

void main( void )
{
    // ------------------------------------------------------------------------
    // smoothstep is a function like:
    // genType t;
    //   t = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
    //   return (3.0 - 2.0 * t) * t * t;
    //
    // vT is a automatically Interpolated.
    // ------------------------------------------------------------------------
//    bool is_valid = ( rdrc_width <= 0.0 || rdrc_height <= 0.0 );
//    is_valid = is_valid || ( gl_FragCoord.x >= rdrc_x && ( gl_FragCoord.x < rdrc_x + rdrc_width ));
//    is_valid = is_valid && ( gl_FragCoord.y >= rdwc_height - rdrc_y - rdrc_height );
    bool is_valid = true;
    lowp float tt = 0.0;
    if ( is_valid ) {
        tt = smoothstep( 0.0, clamp( 0.0002*line_size*line_size -0.0281*line_size+0.9757, 0.0, 1.0 ), sin( vT * PI ));
        gl_FragColor = line_color * qt_Opacity * tt;
    } else {
        discard;
    }
}
