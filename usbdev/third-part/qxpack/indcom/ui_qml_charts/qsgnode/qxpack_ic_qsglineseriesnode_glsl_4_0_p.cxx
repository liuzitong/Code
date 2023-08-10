#ifndef QXPACK_IC_QSGLINESERIESNODE_GLSL_4_0_P_CXX
#define QXPACK_IC_QSGLINESERIESNODE_GLSL_4_0_P_CXX

#include "qxpack_ic_qsglineseriesnode_glsl_4_0_p.hxx"
#include <QByteArray>
#include <QString>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QDebug>
#include <QQuickWindow>

#define SHADER_VEC_PROG_NAME  QStringLiteral("VecProg.IcQSGLineSeriesNodeGLSL4_0.QxPack")
#define SHADER_LST_PROG_NAME  QStringLiteral("LstProg.IcQSGLineSeriesNodeGLSL4_0.QxPack")

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
// the GLSL
// ////////////////////////////////////////////////////////////////////////////
struct UniValues {
    float  matrix[4][4];
    float  viewPort[4];
    float  dispSize[2];
    float  x_info[2];
};

static const char *vtxTxt40_vec = // for ES 3.0 or Core 4.0
    "layout( location = 0 ) in highp float vtxValue;\n" // the vertex data will be enumerated by GPU one by one
    "layout( std140, column_major ) uniform UniValues {\n"
    "    highp mat4   matrix;\n"    // 4N * 4 = 16N   just 4*4*4 = 64 bytes
    "    highp vec4   viewPort;\n"  // 4N
    "    highp vec2   dispSize;\n"  // 2N
    "    highp vec2   x_info;\n"
    "};\n"
    "void main() {\n"
    "   highp vec2 pt = vec2( x_info.x + gl_VertexID * x_info.y - viewPort.x, viewPort.y - vtxValue ) "
    "                 / vec2( viewPort.z, viewPort.w )"
    "                 * dispSize;\n"
    "   gl_Position = matrix * vec4( pt, 0.0, 1.0 );\n"
    "}\n";

static const char *vtxTxt40_lst =
    "layout( location = 0 ) in highp vec2 vtxValue;\n" // the vertex data will be enumerated by GPU one by one
    "layout( std140, column_major ) uniform UniValues {\n"
    "    highp mat4   matrix;\n"    // 4N * 4 = 16N   just 4*4*4 = 64 bytes
    "    highp vec4   viewPort;\n"  // 4N
    "    highp vec2   dispSize;\n"  // 2N
    "    highp vec2   x_info;\n"
    "};\n"
    "void main() {\n"
    "   highp vec2 pt = vec2( vtxValue.x - viewPort.x, viewPort.y - vtxValue.y ) "
    "                 / vec2( viewPort.z, viewPort.w )"
    "                 * dispSize;\n"
    "   gl_Position = matrix * vec4( pt, 0.0, 1.0 );\n"
    "}\n";

static const char *frag43 = // for ES 3.0 or Core 4.0
    "uniform lowp vec4  UniColor;\n"
    "out vec4 fragColor;\n"
    "void main() {\n"
    "   fragColor = UniColor;\n"
    "}\n";

static QByteArray versionedShaderCode(const char *src, bool is_ogl_es )
{
    QByteArray versionedSrc;

    if ( is_ogl_es ) {
        versionedSrc.append(QByteArrayLiteral("#version 300 es\n"));
    } else {
        versionedSrc.append(QByteArrayLiteral("#version 400 core\n"));
    }
    versionedSrc.append(src);
    return versionedSrc;
}

// ////////////////////////////////////////////////////////////////////////////
// QSG Line Series Node
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor ( create it in QSG thread )
// ============================================================================
IcQSGLineSeriesNodeGLSL4_0 :: IcQSGLineSeriesNodeGLSL4_0(  QQuickItem *item )
         : IcQSGLineSeriesNode( item )
{
    m_vec_prog_id = 0; m_lst_prog_id = 0;
    m_vbo_id = 0;  m_vbo_buff_size = 0;
    m_ubo_id = 0;  m_vec_uni_color_loc = 0; m_lst_uni_color_loc = 0;
    m_vec_unival_idx = 0; m_lst_unival_idx = 0;
    m_is_init = false;

    // ------------------------------------------------------------------------
    // check the opengl version
    // : makesure the OpenGL 4.3 or OpenGL ES 3.1, and existed shader compiler
    // ------------------------------------------------------------------------
    auto ctxt = QOpenGLContext::currentContext();
    auto fmt  = ctxt->format();
    auto ver  = fmt.version();
    m_is_ogl_es = ctxt->isOpenGLES();

    bool is_bad_ver = false;
    if ( ! m_is_ogl_es ) {
        if ( ver.first < 4 ){
            is_bad_ver = true;
        }
    } else {
        if ( ver.first < 3 ) {
            is_bad_ver = true;
        }
    }
    if ( is_bad_ver ) {
        qFatal("OpenGL version req. 4.0 or ES 3.0!");
    }
}

// ============================================================================
// dtor
// ============================================================================
IcQSGLineSeriesNodeGLSL4_0 :: ~IcQSGLineSeriesNodeGLSL4_0( )
{
    releaseResources();
}

// ============================================================================
// release resources ( must be in QSG thread )
// ============================================================================
auto     IcQSGLineSeriesNodeGLSL4_0 :: releaseResources() -> void
{
    if ( m_vec_prog_id != 0 ) {
        m_sdr_hlp.decRefProgram( SHADER_VEC_PROG_NAME );
        m_vec_prog_id = 0;
    }
    if ( m_lst_prog_id != 0 ) {
        m_sdr_hlp.decRefProgram( SHADER_LST_PROG_NAME );
        m_lst_prog_id = 0;
    }
    ensureUBO( false );
    ensureVBO( false );

    m_is_init = false;
}

// ============================================================================
// ensure create or delete UBO
// ============================================================================
auto     IcQSGLineSeriesNodeGLSL4_0 :: ensureUBO( bool sw ) -> void
{
    auto glf = QOpenGLContext::currentContext()->extraFunctions();
    auto curr_ubo_id = GLint(0);
    glf->glGetIntegerv( GL_UNIFORM_BUFFER_BINDING, & curr_ubo_id );

    if ( ! sw  ) {
        // free UBO anyway
        if ( m_ubo_id != 0 ) {
            if ( m_ubo_id == GLuint(curr_ubo_id)) { glf->glBindBuffer( GL_UNIFORM_BUFFER, 0 ); curr_ubo_id = 0; }
            glf->glDeleteBuffers( 1, & m_ubo_id ); m_ubo_id = 0;
        }
    } else {
        // create UBO if vector program existed and list program existed
        if ( m_vec_prog_id != 0 && m_lst_prog_id != 0 ) {
            GLint vec_uni_sz = 0;
            glf->glGetActiveUniformBlockiv( m_vec_prog_id, m_vec_unival_idx, GL_UNIFORM_BLOCK_DATA_SIZE, &vec_uni_sz );
            GLint lst_uni_sz = 0;
            glf->glGetActiveUniformBlockiv( m_lst_prog_id, m_lst_unival_idx, GL_UNIFORM_BLOCK_DATA_SIZE, &lst_uni_sz );

            GLint act_uni_sz = 0;
            glf->glGenBuffers( 1, & m_ubo_id );
            glf->glBindBuffer( GL_UNIFORM_BUFFER, m_ubo_id );
            glf->glBufferData( GL_UNIFORM_BUFFER, qopengl_GLsizeiptr( vec_uni_sz >= lst_uni_sz ? vec_uni_sz : lst_uni_sz ), nullptr, GL_DYNAMIC_DRAW );
            glf->glGetBufferParameteriv( GL_UNIFORM_BUFFER, GL_BUFFER_SIZE, &act_uni_sz );
            glf->glBindBuffer( GL_UNIFORM_BUFFER, GLuint( curr_ubo_id )); // curr_ubo_id == 0 ( maybe removed or other )

            if ( act_uni_sz < ( vec_uni_sz >= lst_uni_sz ? vec_uni_sz : lst_uni_sz )) {
                qFatal("allocate GL_UNIFORM_BUFFER failed, req %d size", ( vec_uni_sz >= lst_uni_sz ? vec_uni_sz : lst_uni_sz ) );
            }
        }
    }
}

// ============================================================================
// ensure create or delete VBO
// ============================================================================
auto     IcQSGLineSeriesNodeGLSL4_0 :: ensureVBO( bool sw ) -> void
{
    auto glf = QOpenGLContext::currentContext()->extraFunctions();
    auto curr_vbo_id = GLint(0);
    glf->glGetIntegerv( GL_ARRAY_BUFFER_BINDING, &curr_vbo_id );

    // ------------------------------------------------------------------------
    // sw := false, free the VBO
    // ------------------------------------------------------------------------
    if ( ! sw ) {
        if ( m_vbo_id != 0 ) {
            if ( m_vbo_id == GLuint( curr_vbo_id )) { glf->glBindBuffer( GL_ARRAY_BUFFER, 0 ); curr_vbo_id = 0; }
            glf->glDeleteBuffers( 1, & m_vbo_id ); m_vbo_id = 0; m_vbo_buff_size = 0;
        }
        return;
    }

    // ------------------------------------------------------------------------
    // check current VBO size, if same as the request size, re-use it.
    // ------------------------------------------------------------------------
    if ( m_vbo_id != 0 ) {
        if ( m_req_size != m_vbo_buff_size ) {
            if ( m_vbo_id == GLuint( curr_vbo_id )) { glf->glBindBuffer( GL_ARRAY_BUFFER, 0 ); curr_vbo_id = 0; }
            glf->glDeleteBuffers( 1, & m_vbo_id ); m_vbo_id = 0; m_vbo_buff_size = 0;
        } else {
            return; // can reuse the VBO
        }
    }

    // ------------------------------------------------------------------------
    // try create VBO, here the m_vbo_id must be 0!
    // ------------------------------------------------------------------------
    if ( m_vbo_id != 0 ) { qFatal("check failed, m_vbo_id != 0 !"); }
    if ( m_req_size <= 0 ) { return; } // need not to reallocated, the required size is 0.

    m_vbo_buff_size = m_req_size;
    GLint act_uni_sz = 0;
    glf->glGenBuffers( 1, & m_vbo_id );
    glf->glBindBuffer( GL_ARRAY_BUFFER, m_vbo_id );
    glf->glBufferData( GL_ARRAY_BUFFER, qopengl_GLsizeiptr( m_vbo_buff_size ), nullptr, GL_STATIC_DRAW );
    glf->glGetBufferParameteriv( GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &act_uni_sz );
    glf->glBindBuffer( GL_ARRAY_BUFFER, GLuint( curr_vbo_id ));

    if ( act_uni_sz < GLint( m_vbo_buff_size )) {
        qFatal("allocate GL_ARRAY_BUFFER failed, req %d size", m_vbo_buff_size );
    }
}

// ============================================================================
// init resources
// ============================================================================
auto     IcQSGLineSeriesNodeGLSL4_0 :: initResoruces() -> void
{
    if ( m_is_init ) { return; }

    // ------------------------------------------------------------------------
    // build program, or attach them from program cache.
    // If the cache returned 0, means no program existed.
    // ------------------------------------------------------------------------
    auto *glf = QOpenGLContext::currentContext()->extraFunctions();
    m_vec_prog_id = m_sdr_hlp.tryAddRefProgram( SHADER_VEC_PROG_NAME );
    m_lst_prog_id = m_sdr_hlp.tryAddRefProgram( SHADER_LST_PROG_NAME );

    auto vtx_vec_sd = GLuint(0); auto vtx_vec_dat = QByteArray();
    auto vtx_lst_sd = GLuint(0); auto vtx_lst_dat = QByteArray();
    auto frag_sd    = GLuint(0); auto frag_dat    = QByteArray();

    // no program existed, need to load shader to create it
    if ( m_vec_prog_id == 0 ) {
        if ( vtx_vec_dat.isEmpty()) { vtx_vec_dat = versionedShaderCode( vtxTxt40_vec, m_is_ogl_es ); }
        if ( frag_dat.isEmpty())    { frag_dat    = versionedShaderCode( frag43, m_is_ogl_es ); }
    }
    if ( m_lst_prog_id == 0 ) {
        if ( vtx_lst_dat.isEmpty()) { vtx_lst_dat = versionedShaderCode( vtxTxt40_lst, m_is_ogl_es ); }
        if ( frag_dat.isEmpty())    { frag_dat    = versionedShaderCode( frag43, m_is_ogl_es ); }
    }

    // ------------------------------------------------------------------------
    // compile shader if needed
    // ------------------------------------------------------------------------
    {
        QByteArray  log_info;

        if ( ! vtx_vec_dat.isEmpty() ) {
            vtx_vec_sd = IcQSGOglShaderHlp::compileShader( vtx_vec_dat, log_info, GL_VERTEX_SHADER );
            if ( vtx_vec_sd == 0 ) { qFatal("comp. vector vertex shader failed!"); }
            vtx_vec_dat = QByteArray();
        }
        if ( ! vtx_lst_dat.isEmpty() ) {
            vtx_lst_sd = IcQSGOglShaderHlp::compileShader( vtx_lst_dat, log_info, GL_VERTEX_SHADER );
            if ( vtx_lst_sd == 0 ) {
                if ( vtx_vec_sd != 0 ) { glf->glDeleteShader( vtx_vec_sd ); vtx_vec_sd = 0; }
                qFatal("comp. list vertex shader failed!");
            }
            vtx_lst_dat = QByteArray();
        }
        if ( ! frag_dat.isEmpty() ) {
            frag_sd = IcQSGOglShaderHlp::compileShader( frag_dat, log_info, GL_FRAGMENT_SHADER );
            if ( frag_sd == 0) {
                if ( vtx_vec_sd != 0 ) { glf->glDeleteShader( vtx_vec_sd ); vtx_vec_sd = 0; }
                if ( vtx_lst_sd != 0 ) { glf->glDeleteShader( vtx_lst_sd ); vtx_lst_sd = 0; }
                qFatal("comp. fragment shader failed!");
            }
            frag_dat = QByteArray();
        }
    }

    // ------------------------------------------------------------------------
    // build program if needed
    // ------------------------------------------------------------------------
    if ( m_vec_prog_id == 0 && vtx_vec_sd != 0 && frag_sd != 0 ) {
         QByteArray log_info;
         m_vec_prog_id = IcQSGOglShaderHlp::createProgram( vtx_vec_sd, 0, frag_sd, log_info, nullptr, nullptr );
         if ( m_vec_prog_id == 0 ) {
             glf->glDeleteShader( vtx_vec_sd ); glf->glDeleteShader( vtx_lst_sd ); glf->glDeleteShader( frag_sd );
             qFatal("link prog. failed!");
         }
         if ( ! m_sdr_hlp.takeAddProgram( SHADER_VEC_PROG_NAME, m_vec_prog_id )) {
             qFatal("failed to add program");
         }
     }

     if ( m_lst_prog_id == 0 && vtx_lst_sd != 0 && frag_sd != 0 ){
         QByteArray log_info;
         m_lst_prog_id = IcQSGOglShaderHlp::createProgram( vtx_lst_sd, 0, frag_sd, log_info, nullptr, nullptr );
         if ( m_lst_prog_id == 0 ) {
             glf->glDeleteShader( vtx_vec_sd ); glf->glDeleteShader( vtx_lst_sd ); glf->glDeleteShader( frag_sd );
             qFatal("link prog. failed!");
         }
         if ( ! m_sdr_hlp.takeAddProgram( SHADER_LST_PROG_NAME, m_lst_prog_id )) {
             qFatal("failed to add program");
         }
    }

    // ------------------------------------------------------------------------
    // free resources
    // ------------------------------------------------------------------------
    if ( vtx_vec_sd != 0 ) { glf->glDeleteShader( vtx_vec_sd ); }
    if ( vtx_lst_sd != 0 ) { glf->glDeleteShader( vtx_lst_sd ); }
    if ( frag_sd    != 0 ) { glf->glDeleteShader( frag_sd ); }

    // ------------------------------------------------------------------------
    // query location for uniform variables.
    // ------------------------------------------------------------------------
    m_vec_uni_color_loc = glf->glGetUniformLocation( m_vec_prog_id, "UniColor" );
    m_lst_uni_color_loc = glf->glGetUniformLocation( m_lst_prog_id, "UniColor" );
    m_vec_unival_idx    = glf->glGetUniformBlockIndex( m_vec_prog_id, "UniValues");
    m_vec_unival_idx    = glf->glGetUniformBlockIndex( m_lst_prog_id, "UniValues");
    if ( m_ubo_id == 0 ) { ensureUBO( true ); }

    // ------------------------------------------------------------------------
    // final check
    // ------------------------------------------------------------------------
    if ( m_vec_prog_id == 0 || m_lst_prog_id == 0 ) {
        qFatal("program build failed!");
    }

    m_is_init = true;
}

// ============================================================================
// set the uniform value ( color )
// ============================================================================
auto     IcQSGLineSeriesNodeGLSL4_0 :: setupUniform() -> void
{
    auto glf = QOpenGLContext::currentContext()->extraFunctions();
    auto opac  = m_item->opacity();
    auto color = m_mater.color;
    auto red_f   = color.redF() * opac;
    auto green_f = color.greenF() * opac;
    auto blue_f  = color.blueF() * opac;
    auto alpha_f = color.alphaF() * opac;

    switch( m_dat_type ) {
    case DataType_Vector : {
        glf->glUniform4f( m_vec_uni_color_loc, float(red_f), float(green_f), float(blue_f), float(alpha_f) );
    } break;
    case DataType_List : {
        glf->glUniform4f( m_lst_uni_color_loc, float(red_f), float(green_f), float(blue_f), float(alpha_f) );
    } break;
    default: break;
    }
}

// ============================================================================
// upload UBO buffer
// ============================================================================
auto     IcQSGLineSeriesNodeGLSL4_0 :: bindAndUploadUBO( const RenderState *state ) -> void
{
    auto glf = QOpenGLContext::currentContext()->extraFunctions();

    UniValues  uv;
    {
        auto proj_mat = *state->projectionMatrix() * *matrix();
        std::memcpy( & uv.matrix[0][0], proj_mat.constData(), sizeof( uv.matrix ));
    }
    uv.viewPort[0] = float( m_vw_port.x());      uv.viewPort[1] = float( m_vw_port.y());
    uv.viewPort[2] = float( m_vw_port.width());  uv.viewPort[3] = float( m_vw_port.height());
    uv.dispSize[0] = float( m_rt_curr_disp_w );  uv.dispSize[1] = float( m_rt_curr_disp_h );
    uv.x_info[0]   = m_x_ofv;  uv.x_info[1] = m_x_step;

    glf->glBindBuffer( GL_UNIFORM_BUFFER, m_ubo_id );
    auto is_bind_base = true;
    switch( m_dat_type ) {
    case DataType_Vector : glf->glBindBufferBase( GL_UNIFORM_BUFFER, m_vec_unival_idx, m_ubo_id ); break;
    case DataType_List   : glf->glBindBufferBase( GL_UNIFORM_BUFFER, m_lst_unival_idx, m_ubo_id ); break;
    default: is_bind_base = false;
    }
    if ( is_bind_base ) {
        auto ptr = glf->glMapBufferRange( GL_UNIFORM_BUFFER, 0, sizeof( uv ), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT );
        std::memcpy( ptr, &uv, sizeof( UniValues ) );
        glf->glUnmapBuffer( GL_UNIFORM_BUFFER );
    }
}

// ============================================================================
// upload VBO buffer
// ============================================================================
auto     IcQSGLineSeriesNodeGLSL4_0 :: bindAndUploadVBO_Vec() -> void
{
    m_rt_curr_data_cnt = m_vbo_buff_size / sizeof( float );

    // bind array buffer for use
    auto glf = QOpenGLContext::currentContext()->extraFunctions();
    glf->glBindBuffer( GL_ARRAY_BUFFER, m_vbo_id );

    // upload data if needed
    if ( m_is_data_chg && ! m_vec_data.isEmpty()) {
        m_is_data_chg = false;  // glMapBufferRange avaliable on OpenGL 4.0/ES 3.0
        auto ptr = glf->glMapBufferRange( GL_ARRAY_BUFFER, 0, GLsizeiptr( m_rt_curr_data_cnt * sizeof(float) ),  GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT );
        std::memcpy( ptr, m_vec_data.constData(), size_t( m_vec_data.size()) * sizeof(float));
        glf->glUnmapBuffer( GL_ARRAY_BUFFER );
        m_vec_data = QVector<float>();
    }

    // setup the buffer data format.
    glf->glVertexAttribPointer( 0, 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0));
    glf->glEnableVertexAttribArray( 0 ); // enable location 0
}

auto     IcQSGLineSeriesNodeGLSL4_0 :: bindAndUploadVBO_Lst() -> void
{
    m_rt_curr_data_cnt = m_vbo_buff_size / (sizeof(float)*2);

    // bind array buffer for use
    auto glf = QOpenGLContext::currentContext()->extraFunctions();
    glf->glBindBuffer( GL_ARRAY_BUFFER, m_vbo_id );

    // upload data if needed
    if ( m_is_data_chg && ! m_lst_data.isEmpty()) {
        m_is_data_chg = false;
        struct FloatPoint { float x, y; };
        auto ptr = reinterpret_cast< FloatPoint*>( glf->glMapBufferRange( GL_ARRAY_BUFFER, 0, GLsizeiptr( m_rt_curr_data_cnt * sizeof(float) ),  GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT ));

        auto citr = m_lst_data.constBegin(); auto ofv = int(0);
        while ( citr != m_lst_data.constEnd()) {
            auto &pt = *( citr ++ );
            ptr[ofv].x = float( pt.x()); ptr[ofv].y = float( pt.y());
            ++ ofv;
        }
        glf->glUnmapBuffer( GL_ARRAY_BUFFER );

        m_lst_data = QList<QPointF>();
    }

    // setup the buffer data format
    glf->glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0));
    glf->glEnableVertexAttribArray( 0 );
}

// ============================================================================
//  render
// ============================================================================
auto     IcQSGLineSeriesNodeGLSL4_0 :: render( const RenderState *state ) -> void
{
   // qDebug("render called.");
    if ( ! m_is_init ) { initResoruces(); }
    ensureVBO( true ); // this checked vector and list data if them are empty.

    // ------------------------------------------------------------------------
    // directly return if need not to draw
    // ------------------------------------------------------------------------
    if ( m_vbo_id == 0 ) { return; }         // no data to draw
    if ( ! m_vw_port.isValid() ) { return; } // view port is not invalid
   // auto px_ratio = m_item->window()->devicePixelRatio();
    m_rt_curr_disp_w = m_item->width() /** px_ratio*/;
    m_rt_curr_disp_h = m_item->height() /** px_ratio*/;
    if ( m_rt_curr_disp_w <= 0 || m_rt_curr_disp_h <= 0 ) { return; }

    // ------------------------------------------------------------------------
    // use the program
    // ------------------------------------------------------------------------
    auto glf = QOpenGLContext::currentContext()->extraFunctions();
    m_rt_prev_ubo_id = 0; m_rt_prev_vbo_id = 0;
    glf->glGetIntegerv( GL_UNIFORM_BUFFER_BINDING, reinterpret_cast<GLint*>( & m_rt_prev_ubo_id ));
    glf->glGetIntegerv( GL_ARRAY_BUFFER,    reinterpret_cast<GLint*>( & m_rt_prev_vbo_id ));
    auto prev_prog = GLuint(0);
    glf->glGetIntegerv( GL_CURRENT_PROGRAM, reinterpret_cast<GLint*>( & prev_prog ));

    switch( m_dat_type ) {
    case DataType_Vector : glf->glUseProgram( m_vec_prog_id ); break;
    case DataType_List   : glf->glUseProgram( m_lst_prog_id ); break;
    default: glf->glUseProgram(0); break;
    }

    // ------------------------------------------------------------------------
    // upload  buffers
    // ------------------------------------------------------------------------
    m_rt_curr_data_cnt = 0;
    bindAndUploadUBO( state );
    setupUniform();
    switch( m_dat_type ) {
    case DataType_Vector : bindAndUploadVBO_Vec(); break;
    case DataType_List   : bindAndUploadVBO_Lst(); break;
    default: break;
    }

    // ------------------------------------------------------------------------
    // draw lines
    // ------------------------------------------------------------------------
    if ( m_rt_curr_data_cnt > 0 ) {

        // setup the blend function
        glf->glEnable   (GL_BLEND);
        glf->glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA );

        // setup depth test
        glf->glDisable  ( GL_DEPTH_TEST  );  // close depth test
        glf->glDepthMask( GL_FALSE );        // close depth test write

        // setup scissor area
        if (state->scissorEnabled()) { // means item  or parent used clip attribute
            glf->glEnable(GL_SCISSOR_TEST);
            const QRect r = state->scissorRect(); // already bottom-up
            glf->glScissor(r.x(), r.y(), r.width(), r.height());
        } else {                       // do not draw out the area
//            // the opengl viewport is x [ -1, 1 ], y [ -1, 1 ].
//            // the window center is 0.
//            QPointF lt_pt; auto w_sz = m_item->window()->size();
//            {
//                auto mat = *state->projectionMatrix() * *matrix();
//                auto zero_pt = mat * QVector4D( 0, 0, 0, 1 );
//                // eval the left point.
//                lt_pt.rx() = qreal( w_sz.width() * 0.5 * ( 1.0 + double(zero_pt.x())));
//                lt_pt.ry() = qreal( w_sz.height()* 0.5 * ( 1.0 - double(zero_pt.y())));
//            }

//            glf->glScissor(
//                GLint( std::floor(lt_pt.x()) ),
//                GLint( w_sz.height() - std::floor(lt_pt.y()) - m_rt_curr_disp_h ),
//                GLint( m_rt_curr_disp_w ),
//                GLint( m_rt_curr_disp_h )
//            );
//            glf->glEnable( GL_SCISSOR_TEST );
        }

        // setup the stencil
        if (state->stencilEnabled()) {
            glf->glEnable( GL_STENCIL_TEST );
            glf->glStencilFunc( GL_EQUAL, state->stencilValue(), 0xFF);
            glf->glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP);
        } else {

        }

        glf->glLineWidth( m_mater.size );
        glf->glDrawArrays( GL_LINE_STRIP, 0, GLsizei( m_rt_curr_data_cnt ));
    }

    // ------------------------------------------------------------------------
    // restore
    // ------------------------------------------------------------------------
    glf->glBindBuffer( GL_UNIFORM_BUFFER, m_rt_prev_ubo_id );
    glf->glBindBuffer( GL_ARRAY_BUFFER,   m_rt_prev_vbo_id );
    glf->glUseProgram( prev_prog );
}

// ============================================================================
// other functions
// ============================================================================
auto     IcQSGLineSeriesNodeGLSL4_0 :: changedStates() const -> StateFlags
{ return BlendState | StencilState | ScissorState | DepthState; }

auto     IcQSGLineSeriesNodeGLSL4_0 :: flags() const -> RenderingFlags
{ return BoundedRectRendering /*| DepthAwareRendering*/; }

auto     IcQSGLineSeriesNodeGLSL4_0 :: rect()  const -> QRectF
{ return  QRectF(0, 0, m_item->width(), m_item->height()); }

}
#endif
