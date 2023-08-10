
#include "glwin2.hxx"

#include <QtCore/QCoreApplication>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QPainter>
#include <QDebug>
#include <QOpenGLShaderProgram>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <cstring>
#include <QFile>
#include <QByteArray>
#include "../oglcalc/oglcalc.hxx"
#include "../oglcalc/qtoglfuncbroker.hxx"



QByteArray versionedShaderCode(const char *src)
{
    QByteArray versionedSrc;

    if (QOpenGLContext::currentContext()->isOpenGLES())
        versionedSrc.append(QByteArrayLiteral("#version 310 es\n"));
    else
        versionedSrc.append(QByteArrayLiteral("#version 430 core\n"));

    versionedSrc.append(src);
    return versionedSrc;
}

static const char *cs_add =
        "layout( std430, binding=1 ) buffer PosRead  { float posi_r[]; };\n"
        "layout( std430, binding=2 ) buffer PosWrite { float posi_w[]; };\n"
        "layout(local_size_x = 8, local_size_y = 1, local_size_z = 1) in;\n"
        "uniform float add_val;\n"
        "void main()\n"
        "{\n"
        "    uint gid = gl_GlobalInvocationID.x;\n"
        "    posi_w[ gid ] = posi_r[ gid ] + add_val; \n"
        "}\n";

static const char *cs_mul =
        "layout( std430, binding=1 ) buffer PosRead  { float posi_r[]; };\n"
        "layout( std430, binding=2 ) buffer PosWrite { float posi_w[]; };\n"
        "layout(local_size_x = 8, local_size_y = 1, local_size_z = 1) in;\n"
        "uniform float mul_val;\n"
        "void main()\n"
        "{\n"
        "    uint gid = gl_GlobalInvocationID.x;\n"
        "    posi_w[ gid ] = posi_r[ gid ] * mul_val; \n"
        "}\n";



OpenGLWindow::OpenGLWindow(QWindow *parent)
      : QWindow(parent), m_context(Q_NULLPTR)
{
    setSurfaceType(QWindow::OpenGLSurface);
    QMetaObject::invokeMethod( this, "test", Qt::QueuedConnection );
}

OpenGLWindow::~OpenGLWindow()
{
    m_context->makeCurrent( this );
}

void OpenGLWindow::render(QPainter *painter)
{
    Q_UNUSED(painter);
}

void OpenGLWindow::initialize()
{
}

void OpenGLWindow::render()
{
}

void OpenGLWindow :: test()
{
    if (!m_context) {
        m_context = new QOpenGLContext(this);
        m_context->setFormat(requestedFormat());
        m_context->create();
    }
    m_context->makeCurrent(this);

    OglCalc::init( new OglCalc::QtOglFuncBroker, []( void*, OglCalc::GLFuncBroker *brkr )-> void { delete brkr; }, nullptr );

    auto cs_info = OglCalc::ComputeGLInfo::query();
    qInfo(
        "max work group cnt: %d, %d, %d",
        cs_info.maxWorkGroupCount( OglCalc::ComputeGLInfo::Dim_X ),
        cs_info.maxWorkGroupCount( OglCalc::ComputeGLInfo::Dim_Y ),
        cs_info.maxWorkGroupCount( OglCalc::ComputeGLInfo::Dim_Z )
    );
    qInfo(
       "max work group size: %d, %d, %d",
        cs_info.maxWorkGroupSize ( OglCalc::ComputeGLInfo::Dim_X ),
        cs_info.maxWorkGroupSize ( OglCalc::ComputeGLInfo::Dim_Y ),
        cs_info.maxWorkGroupSize ( OglCalc::ComputeGLInfo::Dim_Z )
    );
    qInfo( "max ssbo bindings: %d", cs_info.maxSsboBindings() );
    qInfo( "max ssb num per shader: %d", cs_info.maxSsbNumPerShader() );
    qInfo( "max ssb size in shader: %d", cs_info.maxSsbSizeInShader() );

    {
        OglCalc::Text txt_cs_add = OglCalc::Text::copyFrom( cs_add, 0 );
        OglCalc::Text txt_cs_mul = OglCalc::Text::copyFrom( cs_mul, 0 );

        OglCalc::Program add_prog( txt_cs_add, true );       
        qInfo( "add prog status: %d", add_prog.status());
        OglCalc::Program mul_prog( txt_cs_mul, true );
        qInfo( "mul prog status: %d", mul_prog.status());

        #define BUFF_SIZE 64

        OglCalc::Buffer  buff_a( OglCalc::Buffer::TypeHint_App_ToGL, true, BUFF_SIZE * sizeof(float) );
        OglCalc::Buffer  buff_b( OglCalc::Buffer::TypeHint_GL_ToGL,  true, BUFF_SIZE * sizeof(float) );

        add_prog.use();
        add_prog.setBuffer( buff_a, 1 );
        add_prog.setBuffer( buff_b, 2 );
        float add_val = 11;
        add_prog.setUniform( "add_val", 1, &add_val );

        mul_prog.use();
        mul_prog.setBuffer( buff_b, 1 );
        mul_prog.setBuffer( buff_a, 2 );
        float mul_val = 1000;
        mul_prog.setUniform( "mul_val", 1, &mul_val );


        // do add action
        buff_a.mapAndDo(
            OglCalc::Buffer::MapHint_WriteOnly,
            [](void*, void*dp, size_t sz ) -> void {
               auto dat = reinterpret_cast<float*>(dp);
               for ( size_t i = 0; i < sz / sizeof(float); i ++ ) { dat[i] = i; }
            }, nullptr
        );
        add_prog.run( 64/8 );

        // do mul. action
        buff_a.cvtToType( OglCalc::Buffer::TypeHint_GL_ToApp, true );
        mul_prog.run( 64/8 );
        buff_a.mapAndDo (
            OglCalc::Buffer::MapHint_ReadOnly,
            []( void*, void*dp, size_t sz ) -> void {
               auto dat = reinterpret_cast<float*>(dp);
               for ( size_t i = 0; i < sz / sizeof(float); i ++ ) {
                   qInfo() << dat[i];
               }
            }, nullptr
        );
    }

    OglCalc::deinit();
}






