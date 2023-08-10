#ifndef QXPACK_IC_QSGVALUEAXISNODE_GLSL_4_3_P_CXX
#define QXPACK_IC_QSGVALUEAXISNODE_GLSL_4_3_P_CXX

#include "qxpack_ic_qsgvalueaxisnode_glsl_4_3_p.hxx"
#include <QByteArray>
#include <QString>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QDebug>
#include <QQuickWindow>

#define SHADER_PROG_NAME  QStringLiteral("valProg.IcQSGValueAxisNodeGLSL4_3.QxPack")

namespace QxPack {


// ////////////////////////////////////////////////////////////////////////////
// the GLSL
// ////////////////////////////////////////////////////////////////////////////
struct UniValues {
    float  matrix[4][4];
    float  viewPort[4];
    float  dispRect[4];
};

static const char *vtxTxt43 = // for ES 3.1 or Core 4.3
    "layout( location = 0 ) in highp float vtxValue;\n" // the vertex data will be enumerated by GPU one by one
    "layout( std140, column_major, binding = 0 ) uniform UniValues {\n"
    "    highp mat4   matrix;\n"    // 4N * 4 = 16N   just 4*4*4 = 64 bytes
    "    highp vec4   viewPort;\n"  // 4N
    "    highp vec4   dispRect;\n"  // 2N
    "};\n"
    "void main() {\n"
    "   highp vec2 pt = vec2( x_info.x + gl_VertexID * x_info.y - viewPort.x, viewPort.y - vtxValue ) "
    "                 / vec2( viewPort.z, viewPort.w )"
    "                 * dispSize;\n"
    "   gl_Position = matrix * vec4( pt, 0.0, 1.0 );\n"
    "}\n";

//
static const char *geom43 = // for ES 3.1 or Core 4.3
    "layout(points) in;\n"
    "layout(line_strip, max_vertices = 2 ) out;\n"
    "layout( std140, column_major, binding = 0 ) uniform UniValues {\n" // UniValues must be same as vertex UniValues
    "    highp mat4   matrix;\n"
    "    highp vec4   viewPort;\n"
    "    highp vec4   dispRect;\n"
    "};\n"

    "void main() {\n"
    "    "
    "}\n";

static const char *frag43 = // for ES 3.1 or Core 4.3
    "uniform lowp vec4  UniColor;\n"
    "out vec4 fragColor;\n"
    "void main() {\n"
    "   fragColor = UniColor;\n"
    "}\n";

static QByteArray versionedShaderCode(const char *src, bool is_ogl_es )
{
    QByteArray versionedSrc;

    if ( is_ogl_es ) {
        versionedSrc.append(QByteArrayLiteral("#version 310 es\n"));
    } else {
        versionedSrc.append(QByteArrayLiteral("#version 430 core\n"));
    }
    versionedSrc.append(src);
    return versionedSrc;
}

// ////////////////////////////////////////////////////////////////////////////
//
// ////////////////////////////////////////////////////////////////////////////


}
#endif
