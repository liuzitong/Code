#ifndef version
#ifdef GL_ES
#define version 100
#else
#define version 120
#endif
#endif

uniform lowp vec4  line_color;
uniform lowp float qt_Opacity;
uniform lowp float line_smooth;
uniform lowp float line_size;

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
    //lowp float tt = smoothstep( 0.0, 0.4, sin( vT * PI )); // the smooth param is the more line_sz width the smaller ...
    //lowp float tt = smoothstep( line_smooth, 1.0, sin( vT * PI ) );
    lowp float tt = smoothstep( 0.0, clamp( 0.0002*line_size*line_size -0.0281*line_size+0.9757, 0.0, 1.0 ), sin( vT * PI )); // the smooth param is the more line_sz width the smaller ...

    gl_FragColor = line_color * qt_Opacity; //* tt;
}
