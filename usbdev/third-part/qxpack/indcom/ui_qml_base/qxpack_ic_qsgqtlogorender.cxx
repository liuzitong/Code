#ifndef QXPACK_IC_QSGQTLOGORENDER_CXX
#define QXPACK_IC_QSGQTLOGORENDER_CXX

#include "qxpack_ic_qsgqtlogorender.hxx"
#include <math.h>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
// implement API
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcQSGQtLogoRender::IcQSGQtLogoRender( )
{

}

// ============================================================================
// dtor
// ============================================================================
IcQSGQtLogoRender::~IcQSGQtLogoRender()
{
}

// ============================================================================
// initialize
// ============================================================================
bool IcQSGQtLogoRender::initialize()
{
    initializeOpenGLFunctions();

    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);

    QOpenGLShader *vshader1 = new QOpenGLShader(QOpenGLShader::Vertex, &program1);
    const char *vsrc1 =
        "attribute highp vec4 vertex;\n"
        "attribute mediump vec3 normal;\n"
        "uniform mediump mat4 matrix;\n"
        "varying mediump vec4 color;\n"
        "void main(void)\n"
        "{\n"
        "    vec3 toLight = normalize(vec3(0.0, 0.3, 1.0));\n"
        "    float angle = max(dot(normal, toLight), 0.0);\n"
        "    vec3 col = vec3(0.40, 1.0, 0.0);\n"
        "    color = vec4(col * 0.2 + col * 0.8 * angle, 1.0);\n"
        "    color = clamp(color, 0.0, 1.0);\n"
        "    gl_Position = matrix * vertex;\n"
        "}\n";
    vshader1->compileSourceCode(vsrc1);

    QOpenGLShader *fshader1 = new QOpenGLShader(QOpenGLShader::Fragment, &program1);
    const char *fsrc1 =
        "varying mediump vec4 color;\n"
        "void main(void)\n"
        "{\n"
        "    gl_FragColor = color;\n"
        "}\n";
    fshader1->compileSourceCode(fsrc1);

    program1.addShader(vshader1);
    program1.addShader(fshader1);
    program1.link();
   // program1.removeAllShaders();


    vertexAttr1 = program1.attributeLocation("vertex");
    normalAttr1 = program1.attributeLocation("normal");
    matrixUniform1 = program1.uniformLocation("matrix");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    m_fAngle = 0;
    m_fScale = 1;
    createGeometry();

    return true;
}

// ============================================================================
// draw the qt logo by vertex data on the target
// ============================================================================
void IcQSGQtLogoRender::paintQtLogo()
{
    program1.enableAttributeArray(normalAttr1);
    program1.enableAttributeArray(vertexAttr1);
    program1.setAttributeArray(vertexAttr1, vertices.constData());
    program1.setAttributeArray(normalAttr1, normals.constData());
    glDrawArrays( GL_TRIANGLES, 0, vertices.size() );
    program1.disableAttributeArray(normalAttr1);
    program1.disableAttributeArray(vertexAttr1);
}

// ============================================================================
// do the render
// ============================================================================
void IcQSGQtLogoRender::render()
{
    glDepthMask(true);

    //glClearColor(0.5f, 0.5f, 0.7f, 1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glFrontFace(GL_CW);
    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    QMatrix4x4 modelview;
    modelview.rotate( float(m_fAngle), 0.0f, 1.0f, 0.0f);
    modelview.rotate( float(m_fAngle), 1.0f, 0.0f, 0.0f);
    modelview.rotate( float(m_fAngle), 0.0f, 0.0f, 1.0f);
    modelview.scale( float(m_fScale));
    modelview.translate(0.0f, -0.2f, 0.0f);

    program1.bind();
    program1.setUniformValue(matrixUniform1, modelview);
    paintQtLogo();
    program1.release();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    m_fAngle += 1.0;
}

// ============================================================================
// make the geometry of the Q character, this function generate the verex data
// and the normals data
// ============================================================================
void   IcQSGQtLogoRender :: createGeometry()
{
    vertices.clear();
    normals.clear();

    qreal x1 = +0.06;
    qreal y1 = -0.14;
    qreal x2 = +0.14;
    qreal y2 = -0.06;
    qreal x3 = +0.08;
    qreal y3 = +0.00;
    qreal x4 = +0.30;
    qreal y4 = +0.22;

    quad(x1, y1, x2, y2, y2, x2, y1, x1);
    quad(x3, y3, x4, y4, y4, x4, y3, x3);

    extrude(x1, y1, x2, y2);
    extrude(x2, y2, y2, x2);
    extrude(y2, x2, y1, x1);
    extrude(y1, x1, x1, y1);
    extrude(x3, y3, x4, y4);
    extrude(x4, y4, y4, x4);
    extrude(y4, x4, y3, x3);

    const qreal Pi = 3.14159;
    const int NumSectors = 100;

    for (int i = 0; i < NumSectors; ++i) {
        qreal angle1 = (i * 2 * Pi) / NumSectors;
        qreal x5 = 0.30 * sin(angle1);
        qreal y5 = 0.30 * cos(angle1);
        qreal x6 = 0.20 * sin(angle1);
        qreal y6 = 0.20 * cos(angle1);

        qreal angle2 = ((i + 1) * 2 * Pi) / NumSectors;
        qreal x7 = 0.20 * sin(angle2);
        qreal y7 = 0.20 * cos(angle2);
        qreal x8 = 0.30 * sin(angle2);
        qreal y8 = 0.30 * cos(angle2);

        quad(x5, y5, x6, y6, x7, y7, x8, y8);

        extrude(x6, y6, x7, y7);
        extrude(x8, y8, x5, y5);
    }

    for (int i = 0;i < vertices.size();i++)
        vertices[i] *= 2.0f;
}

// ============================================================================
// helper function
// ============================================================================
void IcQSGQtLogoRender::quad( qreal x1, qreal y1, qreal x2, qreal y2, qreal x3, qreal y3, qreal x4, qreal y4)
{
    vertices << QVector3D( float(x1), float(y1), -0.05f);
    vertices << QVector3D( float(x2), float(y2), -0.05f);
    vertices << QVector3D( float(x4), float(y4), -0.05f);

    vertices << QVector3D( float(x3), float(y3), -0.05f);
    vertices << QVector3D( float(x4), float(y4), -0.05f);
    vertices << QVector3D( float(x2), float(y2), -0.05f);

    QVector3D n = QVector3D::normal
        (QVector3D( float(x2 - x1), float(y2 - y1), 0.0f), QVector3D(float(x4 - x1), float(y4 - y1), 0.0f));

    normals << n;
    normals << n;
    normals << n;

    normals << n;
    normals << n;
    normals << n;

    vertices << QVector3D( float(x4), float(y4), 0.05f);
    vertices << QVector3D( float(x2), float(y2), 0.05f);
    vertices << QVector3D( float(x1), float(y1), 0.05f);

    vertices << QVector3D( float(x2), float(y2), 0.05f);
    vertices << QVector3D( float(x4), float(y4), 0.05f);
    vertices << QVector3D( float(x3), float(y3), 0.05f);

    n = QVector3D::normal
        (QVector3D( float(x2 - x4), float(y2 - y4), 0.0f), QVector3D(float(x1 - x4), float(y1 - y4), 0.0f));

    normals << n;
    normals << n;
    normals << n;

    normals << n;
    normals << n;
    normals << n;
}

// ============================================================================
// help function
// ============================================================================
void IcQSGQtLogoRender::extrude(qreal x1, qreal y1, qreal x2, qreal y2)
{
    vertices << QVector3D( float(x1), float(y1), +0.05f);
    vertices << QVector3D( float(x2), float(y2), +0.05f);
    vertices << QVector3D( float(x1), float(y1), -0.05f);

    vertices << QVector3D( float(x2), float(y2), -0.05f);
    vertices << QVector3D( float(x1), float(y1), -0.05f);
    vertices << QVector3D( float(x2), float(y2), +0.05f);

    QVector3D n = QVector3D::normal
        (QVector3D( float(x2 - x1), float(y2 - y1), 0.0f), QVector3D(0.0f, 0.0f, -0.1f));

    normals << n;
    normals << n;
    normals << n;

    normals << n;
    normals << n;
    normals << n;
}

}

#endif

