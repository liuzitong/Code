#ifndef QXPACK_IC_QUICKFBOLINESERIESITEM_CXX
#define QXPACK_IC_QUICKFBOLINESERIESITEM_CXX

#include "qxpack_ic_quickfbolineseriesitem.hxx"
#include "qxpack/indcom/ui_qml_base/qxpack_ic_qsgrenderworker.hxx"

#include <QQmlEngine>
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QDebug>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
// register functions, it will register object type in QML engine
// while QCoreApplication finish the ctor
// ////////////////////////////////////////////////////////////////////////////
static void QxPack_IcQuickFboLineSeriesItem_Reg( )
{
    static bool is_reg = false;
    if ( ! is_reg ) {
        is_reg = true;
        qmlRegisterType<QxPack::IcQuickFboLineSeriesItem>("qxpack.indcom.ui_qml_charts", 1, 0, "IcQuickFboLineSeriesItem");
    }
}

// ////////////////////////////////////////////////////////////////////////////
// IcQSGLineSeriesRender
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN  IcQSGLineSeriesRender : public IcQSGRenderWorker,  protected QOpenGLFunctions {
private:
    QOpenGLShaderProgram  m_program;
public:
    explicit IcQSGLineSeriesRender();
    virtual ~IcQSGLineSeriesRender() Q_DECL_OVERRIDE;
    virtual bool initialize() Q_DECL_OVERRIDE;
    virtual void render() Q_DECL_OVERRIDE;
    virtual bool isReqUpdateAfterRender() Q_DECL_OVERRIDE { return false; }
};

IcQSGLineSeriesRender :: IcQSGLineSeriesRender ( )
{

}

IcQSGLineSeriesRender :: ~IcQSGLineSeriesRender( )
{

}

auto     IcQSGLineSeriesRender :: initialize() -> bool
{
    initializeOpenGLFunctions();
    glLoadIdentity();

    m_program.addCacheableShaderFromSourceCode(
        QOpenGLShader::Vertex,
//        "attribute highp vec4 vertices;"
//        "varying highp vec2 coords;"
//        "void main() {"
//        "    gl_Position = vertices;"
//        "    coords = vertices.xy;"
//        "}"
          "uniform   mat4   mvpMatrix;"
          "attribute vec4   vVertex;"
          "void main(void)"
          "{"
          "    gl_Position = mvpMatrix * vVertex;"
          "}"
    );
    m_program.addCacheableShaderFromSourceCode(
        QOpenGLShader::Fragment,
//        "uniform lowp float t;"
//        "varying highp vec2 coords;"
//        "void main() {"
//        "    lowp float i = 1. - (pow(abs(coords.x), 4.) + pow(abs(coords.y), 4.));"
//        "    i = smoothstep(t - 0.8, t + 0.8, i);"
//        "    i = floor(i * 20.) / 20.;"
//        "    gl_FragColor = vec4(coords * .5 + .5, i, i);"
//        "}"
          "void main(void)"
          "{"
          "   gl_FragColor = gl_Color;"
          "}"
    );

    m_program.bindAttributeLocation("vVertex", 0);
    m_program.link();

    //glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    return true;
}

auto     IcQSGLineSeriesRender :: render() -> void
{
    m_program.bind();

    float values[] = {
        7.5f, 7.5f , 0.0f,
        7.5f, -7.5f, 0.0f,
        -7.5f, -7.5f, 0.0f,
        -7.5f, 7.5f, 0.0f
    };

    m_program.setAttributeArray(0, GL_FLOAT, values, 2);
    QMatrix4x4 mat4x4;
    mat4x4.ortho( 0, 800, 0, 600, 0.1f, 20.f  );
    m_program.setUniformValue( "mvpMatrix", mat4x4 );

    GLuint pBuffer;
    glGenBuffers(1, &pBuffer);

    QVector3D box[5] = {
        { 100, 200, 0.f },
        { 200, 200, 0.f },
        { 200, 100, 0.f },
        { 100, 100, 0.f },
        { 100, 200, 0.f }
    };

    glBindBuffer(GL_ARRAY_BUFFER, pBuffer);
    glEnableVertexAttribArray( GL_ATTRIBUTE_VERTEX);
    glBufferData(GL_ARRAY_BUFFER, sizeof(M3DVector3f)*5, box, GL_STATIC_DRAW);
    glVertexAttribPointer(AGL_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glColor3f(0.f, 1.f, 1.f);
    glLineWidth(2.f);

    glDrawArrays(GL_LINES, 0, 5);

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);

    glDisableVertexAttribArray(AGL_ATTRIBUTE_VERTEX);
    glDeleteBuffers(1, &pBuffer);
}



// ////////////////////////////////////////////////////////////////////////////
// quick qt logo item
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcQuickFboLineSeriesItem :: IcQuickFboLineSeriesItem ( QQuickItem *pa )
    : IcQuickFboRenderBase (
        [](void*,const QString &op, const QVariant &par )->QVariant {
            if ( op == QStringLiteral("createQSGRenderWorker")) {
                IcQSGRenderWorker *wkr = new IcQSGLineSeriesRender();
                return QVariant::fromValue( static_cast<void*>(wkr));
            } else if ( op == QStringLiteral("deleteQSGRenderWorker")) {
                IcQSGRenderWorker *wkr = static_cast<QxPack::IcQSGRenderWorker*>( par.value<void*>());
                if ( wkr != Q_NULLPTR ) { delete wkr; }
                return QVariant();
            } else {
                return QVariant();
            }
        }, this, pa
    )
{
    m_obj = Q_NULLPTR;
}

// ============================================================================
// dtor
// ============================================================================
IcQuickFboLineSeriesItem :: ~IcQuickFboLineSeriesItem( )
{

}


void  IcQuickFboLineSeriesItem :: _reg() {QxPack_IcQuickFboLineSeriesItem_Reg(); }

}


#endif
