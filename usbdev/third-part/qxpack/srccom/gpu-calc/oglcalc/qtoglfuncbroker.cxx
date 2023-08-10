#ifndef QTOGLFUNCBROKER_CXX
#define QTOGLFUNCBROKER_CXX

#include "qtoglfuncbroker.hxx"
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QOpenGLContext>

namespace OglCalc {

// ////////////////////////////////////////////////////////////////////////////
// private object
// ////////////////////////////////////////////////////////////////////////////
#define T_Priv( o )  ( static_cast< QtOglFuncBrokerPriv* >( o ) )
class  QtOglFuncBrokerPriv {
private:
    QOpenGLContext *m_ctxt;
    QOpenGLExtraFunctions *m_extra_func;
    Text  m_cs_hdr;
public :
    explicit QtOglFuncBrokerPriv( );
    ~QtOglFuncBrokerPriv( );
    inline auto  contextRef() -> QOpenGLContext*        { return m_ctxt; }
    inline auto  extraFunc()  -> QOpenGLExtraFunctions* { return m_extra_func; }
    inline auto  csHdrText()  -> Text&  { return m_cs_hdr; }
};

QtOglFuncBrokerPriv :: QtOglFuncBrokerPriv ( )
{
    m_ctxt       = QOpenGLContext::currentContext();
    m_extra_func = m_ctxt->extraFunctions();

    if ( m_ctxt->isOpenGLES()) {
        m_cs_hdr = Text::copyFrom( "#version 310 es\n", 0 );
    } else {
        m_cs_hdr = Text::copyFrom( "#version 430 core\n", 0 );
    }
}


QtOglFuncBrokerPriv :: ~QtOglFuncBrokerPriv ( )
{}

// ////////////////////////////////////////////////////////////////////////////
// wrap API
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
QtOglFuncBroker :: QtOglFuncBroker ( )
{
    m_obj = new QtOglFuncBrokerPriv;
}

QtOglFuncBroker :: QtOglFuncBroker ( const char *cs_hdr_ptr )
{
    m_obj = new QtOglFuncBrokerPriv;
    if ( cs_hdr_ptr != nullptr ) {
        auto priv = T_Priv(m_obj);
        priv->csHdrText() = Text::copyFrom( cs_hdr_ptr, 0 );
    }
}

// ============================================================================
// dtor
// ============================================================================
QtOglFuncBroker :: ~QtOglFuncBroker ( )
{
    delete reinterpret_cast<QtOglFuncBrokerPriv*>( m_obj );
}

// ============================================================================
// api wrapper
// ============================================================================
auto     QtOglFuncBroker :: shaderVersionStr() const -> const char* { return T_Priv(m_obj)->csHdrText().constData(); }
auto     QtOglFuncBroker :: glGetError( void ) -> GLenum { return T_Priv(m_obj)->extraFunc()->glGetError(); }
auto     QtOglFuncBroker :: glGetIntegerv  ( GLenum pname, GLint *params )              -> void { T_Priv(m_obj)->extraFunc()->glGetIntegerv( pname, params ); }
auto     QtOglFuncBroker :: glGetBooleanv  ( GLenum pname, GLboolean *params )          -> void { T_Priv(m_obj)->extraFunc()->glGetBooleanv( pname, params ); }
auto     QtOglFuncBroker :: glGetIntegeri_v( GLenum target, GLuint index, GLint *data ) -> void { T_Priv(m_obj)->extraFunc()->glGetIntegeri_v( target, index, data ); }
auto     QtOglFuncBroker :: glCreateShader ( GLenum type   )  -> GLuint { return T_Priv(m_obj)->extraFunc()->glCreateShader( type ); }
auto     QtOglFuncBroker :: glDeleteShader ( GLuint shader )  -> void   { T_Priv(m_obj)->extraFunc()->glDeleteShader( shader ); }
auto     QtOglFuncBroker :: glCompileShader( GLuint shader )  -> void   { T_Priv(m_obj)->extraFunc()->glCompileShader( shader ); }
auto     QtOglFuncBroker :: glShaderSource ( GLuint shader, GLsizei count, const char **string, const GLint *length ) -> void { T_Priv(m_obj)->extraFunc()->glShaderSource( shader, count, string, length ); }
auto     QtOglFuncBroker :: glGetShaderiv  ( GLuint shader, GLenum pname, GLint *params ) -> void { T_Priv(m_obj)->extraFunc()->glGetShaderiv( shader, pname, params ); }
auto     QtOglFuncBroker :: glGetShaderInfoLog ( GLuint shader, GLsizei bufsize, GLsizei *length, char *infolog ) -> void { T_Priv(m_obj)->extraFunc()->glGetShaderInfoLog( shader, bufsize, length, infolog ); }
auto     QtOglFuncBroker :: glAttachShader ( GLuint program, GLuint shader ) -> void { T_Priv(m_obj)->extraFunc()->glAttachShader( program, shader ); }
auto     QtOglFuncBroker :: glDetachShader ( GLuint program, GLuint shader ) -> void { T_Priv(m_obj)->extraFunc()->glDetachShader( program, shader ); }

auto     QtOglFuncBroker :: glCreateProgram ( ) -> GLuint { return T_Priv(m_obj)->extraFunc()->glCreateProgram(); }
auto     QtOglFuncBroker :: glDeleteProgram ( GLuint program ) -> void { T_Priv(m_obj)->extraFunc()->glDeleteProgram( program ); }
auto     QtOglFuncBroker :: glProgramParameteri ( GLuint program, GLenum pname, GLint value ) -> void { T_Priv(m_obj)->extraFunc()->glProgramParameteri( program, pname, value ); }
auto     QtOglFuncBroker :: glLinkProgram   ( GLuint program ) -> void { T_Priv(m_obj)->extraFunc()->glLinkProgram( program ); }
auto     QtOglFuncBroker :: glGetProgramiv  ( GLuint program, GLenum pname, GLint *params )   -> void { T_Priv(m_obj)->extraFunc()->glGetProgramiv( program, pname, params ); }
auto     QtOglFuncBroker :: glGetProgramInfoLog ( GLuint program, GLsizei bufsize, GLsizei *length, char *infolog ) -> void { T_Priv(m_obj)->extraFunc()->glGetProgramInfoLog( program, bufsize, length, infolog ); }
auto     QtOglFuncBroker :: glUseProgram    ( GLuint program ) -> void { T_Priv(m_obj)->extraFunc()->glUseProgram( program ); }

auto     QtOglFuncBroker :: glGenBuffers   ( GLsizei n,       GLuint *buffers ) -> void { T_Priv(m_obj)->extraFunc()->glGenBuffers( n, buffers ); }
auto     QtOglFuncBroker :: glDeleteBuffers( GLsizei n, const GLuint *buffers ) -> void { T_Priv(m_obj)->extraFunc()->glDeleteBuffers( n, buffers ); }
auto     QtOglFuncBroker :: glBindBuffer   ( GLenum target, GLuint buffer   ) -> void { T_Priv(m_obj)->extraFunc()->glBindBuffer( target, buffer ); }
auto     QtOglFuncBroker :: glBufferData   ( GLenum target, GLsizeiptr size, const void *data, GLenum usage ) -> void { T_Priv(m_obj)->extraFunc()->glBufferData( target, size, data, usage ); }
auto     QtOglFuncBroker :: glMapBufferRange ( GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access ) -> void* { return T_Priv(m_obj)->extraFunc()->glMapBufferRange( target, offset, length, access ); }
auto     QtOglFuncBroker :: glUnmapBuffer    ( GLenum target ) -> GLboolean { return T_Priv(m_obj)->extraFunc()->glUnmapBuffer( target ); }
auto     QtOglFuncBroker :: glBindBufferBase      ( GLenum target, GLuint index, GLuint buffer ) -> void { T_Priv(m_obj)->extraFunc()->glBindBufferBase( target, index, buffer ); }
auto     QtOglFuncBroker :: glGetBufferParameteriv( GLenum target, GLenum value, GLint *data   ) -> void { T_Priv(m_obj)->extraFunc()->glGetBufferParameteriv( target, value, data ); }

auto     QtOglFuncBroker :: glDispatchCompute ( GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z ) -> void { T_Priv(m_obj)->extraFunc()->glDispatchCompute( num_groups_x, num_groups_y, num_groups_z ); }
auto     QtOglFuncBroker :: glMemoryBarrier   ( GLbitfield barriers ) -> void { T_Priv(m_obj)->extraFunc()->glMemoryBarrier( barriers ); }

auto     QtOglFuncBroker :: glGetUniformLocation( GLuint program, const GLchar *name ) -> GLint { return T_Priv(m_obj)->extraFunc()->glGetUniformLocation( program, name ); }
auto     QtOglFuncBroker :: glUniform1fv ( GLint loc, GLsizei cnt, const GLfloat *vp ) -> void  { T_Priv(m_obj)->extraFunc()->glUniform1fv( loc, cnt, vp ); }
auto     QtOglFuncBroker :: glUniform2fv ( GLint loc, GLsizei cnt, const GLfloat *vp ) -> void  { T_Priv(m_obj)->extraFunc()->glUniform2fv( loc, cnt, vp ); }
auto     QtOglFuncBroker :: glUniform3fv ( GLint loc, GLsizei cnt, const GLfloat *vp ) -> void  { T_Priv(m_obj)->extraFunc()->glUniform3fv( loc, cnt, vp ); }
auto     QtOglFuncBroker :: glUniform4fv ( GLint loc, GLsizei cnt, const GLfloat *vp ) -> void  { T_Priv(m_obj)->extraFunc()->glUniform4fv( loc, cnt, vp ); }
auto     QtOglFuncBroker :: glUniform1iv ( GLint loc, GLsizei cnt, const GLint *vp   ) -> void  { T_Priv(m_obj)->extraFunc()->glUniform1iv( loc, cnt, vp ); }
auto     QtOglFuncBroker :: glUniform2iv ( GLint loc, GLsizei cnt, const GLint *vp   ) -> void  { T_Priv(m_obj)->extraFunc()->glUniform2iv( loc, cnt, vp ); }
auto     QtOglFuncBroker :: glUniform3iv ( GLint loc, GLsizei cnt, const GLint *vp   ) -> void  { T_Priv(m_obj)->extraFunc()->glUniform3iv( loc, cnt, vp ); }
auto     QtOglFuncBroker :: glUniform4iv ( GLint loc, GLsizei cnt, const GLint *vp   ) -> void  { T_Priv(m_obj)->extraFunc()->glUniform4iv( loc, cnt, vp ); }
auto     QtOglFuncBroker :: glUniform1uiv( GLint loc, GLsizei cnt, const GLuint *vp  ) -> void  { T_Priv(m_obj)->extraFunc()->glUniform1uiv( loc, cnt, vp ); }
auto     QtOglFuncBroker :: glUniform2uiv( GLint loc, GLsizei cnt, const GLuint *vp  ) -> void  { T_Priv(m_obj)->extraFunc()->glUniform2uiv( loc, cnt, vp ); }
auto     QtOglFuncBroker :: glUniform3uiv( GLint loc, GLsizei cnt, const GLuint *vp  ) -> void  { T_Priv(m_obj)->extraFunc()->glUniform3uiv( loc, cnt, vp ); }
auto     QtOglFuncBroker :: glUniform4uiv( GLint loc, GLsizei cnt, const GLuint *vp  ) -> void  { T_Priv(m_obj)->extraFunc()->glUniform4uiv( loc, cnt, vp ); }

auto     QtOglFuncBroker :: glUniformMatrix2fv  ( GLint loc, GLsizei cnt, GLboolean trans, const GLfloat *vp ) -> void { T_Priv(m_obj)->extraFunc()->glUniformMatrix2fv  ( loc, cnt, trans, vp ); }
auto     QtOglFuncBroker :: glUniformMatrix3fv  ( GLint loc, GLsizei cnt, GLboolean trans, const GLfloat *vp ) -> void { T_Priv(m_obj)->extraFunc()->glUniformMatrix3fv  ( loc, cnt, trans, vp ); }
auto     QtOglFuncBroker :: glUniformMatrix4fv  ( GLint loc, GLsizei cnt, GLboolean trans, const GLfloat *vp ) -> void { T_Priv(m_obj)->extraFunc()->glUniformMatrix4fv  ( loc, cnt, trans, vp ); }
auto     QtOglFuncBroker :: glUniformMatrix2x3fv( GLint loc, GLsizei cnt, GLboolean trans, const GLfloat *vp ) -> void { T_Priv(m_obj)->extraFunc()->glUniformMatrix2x3fv( loc, cnt, trans, vp ); }
auto     QtOglFuncBroker :: glUniformMatrix3x2fv( GLint loc, GLsizei cnt, GLboolean trans, const GLfloat *vp ) -> void { T_Priv(m_obj)->extraFunc()->glUniformMatrix3x2fv( loc, cnt, trans, vp ); }
auto     QtOglFuncBroker :: glUniformMatrix2x4fv( GLint loc, GLsizei cnt, GLboolean trans, const GLfloat *vp ) -> void { T_Priv(m_obj)->extraFunc()->glUniformMatrix2x4fv( loc, cnt, trans, vp ); }
auto     QtOglFuncBroker :: glUniformMatrix4x2fv( GLint loc, GLsizei cnt, GLboolean trans, const GLfloat *vp ) -> void { T_Priv(m_obj)->extraFunc()->glUniformMatrix4x2fv( loc, cnt, trans, vp ); }
auto     QtOglFuncBroker :: glUniformMatrix3x4fv( GLint loc, GLsizei cnt, GLboolean trans, const GLfloat *vp ) -> void { T_Priv(m_obj)->extraFunc()->glUniformMatrix3x4fv( loc, cnt, trans, vp ); }
auto     QtOglFuncBroker :: glUniformMatrix4x3fv( GLint loc, GLsizei cnt, GLboolean trans, const GLfloat *vp ) -> void { T_Priv(m_obj)->extraFunc()->glUniformMatrix4x3fv( loc, cnt, trans, vp ); }


// ////////////////////////////////////////////////////////////////////////////
// check if opengl version is supported
// ////////////////////////////////////////////////////////////////////////////
auto     QtOglFuncBroker :: isSupport( int major, int minor, bool gles, QSurfaceFormat::OpenGLContextProfile profile ) -> bool
{
    QOpenGLContext ctx;
    QSurfaceFormat fmt;

    fmt.setVersion(major, minor);
    if ( gles ) {
        fmt.setRenderableType(QSurfaceFormat::OpenGLES);
    } else {
        fmt.setRenderableType(QSurfaceFormat::OpenGL);
        fmt.setProfile(profile);
    }

    ctx.setFormat(fmt);
    ctx.create();
    if ( !ctx.isValid()) { return false; }

    int ctxMajor = ctx.format().majorVersion();
    int ctxMinor = ctx.format().minorVersion();
    bool isGles = (ctx.format().renderableType() == QSurfaceFormat::OpenGLES);

    if ( isGles != gles ) { return false; }
    if ( ctxMajor < major) { return false; }
    if ( ctxMajor == major && ctxMinor < minor) { return false; }
    if ( !gles && ctx.format().profile() != profile ) { return false; }

    return true;
}


}
#endif
