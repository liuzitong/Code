#ifndef version
#ifdef GL_ES
#define version 100
#else
#define version 120
#endif
#endif

attribute highp vec4  attr_pos;
attribute highp float attr_type;

uniform lowp  float line_size;
uniform highp mat4  qt_Matrix;

varying lowp float vT;

void main( void )
{
    vec4 adjustedPos = attr_pos;
    float type = clamp( attr_type, 0.0, 1.0 );
    gl_Position = qt_Matrix * adjustedPos;
    vT = type; // t( 0 or 1 ) value pass to fragment shader
}
