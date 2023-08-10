// ////////////////////////////////////////////////////////////////////////////
// File: opengl calculator implement ( Qt5 function broker )
// Author: W.B.
// Date: 2021/05
// ////////////////////////////////////////////////////////////////////////////
#ifndef QTOGLFUNCBROKER_HXX
#define QTOGLFUNCBROKER_HXX

#include "oglcalc.hxx"
#include <QSurfaceFormat>

namespace OglCalc {

// ////////////////////////////////////////////////////////////////////////////
//
// the QT opengl function broker
//
// ////////////////////////////////////////////////////////////////////////////
class   QtOglFuncBroker : public GLFuncBroker {

public:
    explicit QtOglFuncBroker( );
    QtOglFuncBroker( const char *version_str );
    virtual ~QtOglFuncBroker(  ) OGLCALC_OVERRIDE;

    virtual const char*  shaderVersionStr() const OGLCALC_OVERRIDE;

    virtual GLenum  glGetError     ( void ) OGLCALC_OVERRIDE;
    virtual void    glGetIntegerv  ( GLenum pname, GLint *params) OGLCALC_OVERRIDE;
    virtual void    glGetBooleanv  ( GLenum pname, GLboolean *params) OGLCALC_OVERRIDE;
    virtual void    glGetIntegeri_v( GLenum target, GLuint index, GLint *data) OGLCALC_OVERRIDE;

    virtual GLuint  glCreateShader ( GLenum type   ) OGLCALC_OVERRIDE;
    virtual void    glDeleteShader ( GLuint shader ) OGLCALC_OVERRIDE;
    virtual void    glCompileShader( GLuint shader ) OGLCALC_OVERRIDE;
    virtual void    glShaderSource ( GLuint shader, GLsizei count, const char **string, const GLint *length ) OGLCALC_OVERRIDE;
    virtual void    glGetShaderiv  ( GLuint shader, GLenum pname, GLint *params ) OGLCALC_OVERRIDE;
    virtual void    glGetShaderInfoLog ( GLuint shader, GLsizei bufsize, GLsizei *length, char *infolog ) OGLCALC_OVERRIDE;
    virtual void    glAttachShader ( GLuint program, GLuint shader ) OGLCALC_OVERRIDE;
    virtual void    glDetachShader ( GLuint program, GLuint shader ) OGLCALC_OVERRIDE;

    virtual GLuint  glCreateProgram ( ) OGLCALC_OVERRIDE;
    virtual void    glDeleteProgram ( GLuint program ) OGLCALC_OVERRIDE;
    virtual void    glProgramParameteri ( GLuint program, GLenum pname, GLint value ) OGLCALC_OVERRIDE;
    virtual void    glLinkProgram   ( GLuint program ) OGLCALC_OVERRIDE;
    virtual void    glGetProgramiv  ( GLuint program, GLenum pname, GLint *params ) OGLCALC_OVERRIDE;
    virtual void    glGetProgramInfoLog ( GLuint program, GLsizei bufsize, GLsizei *length, char *infolog ) OGLCALC_OVERRIDE;
    virtual void    glUseProgram    ( GLuint program ) OGLCALC_OVERRIDE;

    virtual void    glGenBuffers     ( GLsizei n, GLuint *buffers ) OGLCALC_OVERRIDE;
    virtual void    glDeleteBuffers  ( GLsizei n, const GLuint *buffers ) OGLCALC_OVERRIDE;
    virtual void    glBindBuffer     ( GLenum target, GLuint buffer ) OGLCALC_OVERRIDE;
    virtual void    glBufferData     ( GLenum target, GLsizeiptr size, const void *data, GLenum usage ) OGLCALC_OVERRIDE;
    virtual void*   glMapBufferRange ( GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access) OGLCALC_OVERRIDE;
    virtual GLboolean  glUnmapBuffer ( GLenum target ) OGLCALC_OVERRIDE;
    virtual void    glBindBufferBase ( GLenum target, GLuint index, GLuint buffer ) OGLCALC_OVERRIDE;
    virtual void    glGetBufferParameteriv ( GLenum target, GLenum value, GLint * data ) OGLCALC_OVERRIDE;

    virtual void    glDispatchCompute ( GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z ) OGLCALC_OVERRIDE;
    virtual void    glMemoryBarrier   ( GLbitfield barriers ) OGLCALC_OVERRIDE;

    virtual GLint   glGetUniformLocation( GLuint program, const GLchar *name ) OGLCALC_OVERRIDE;
    virtual void    glUniform1fv ( GLint loc, GLsizei cnt, const GLfloat *vp ) OGLCALC_OVERRIDE; // for float
    virtual void    glUniform2fv ( GLint loc, GLsizei cnt, const GLfloat *vp ) OGLCALC_OVERRIDE; // for vec2
    virtual void    glUniform3fv ( GLint loc, GLsizei cnt, const GLfloat *vp ) OGLCALC_OVERRIDE; // for vec3
    virtual void    glUniform4fv ( GLint loc, GLsizei cnt, const GLfloat *vp ) OGLCALC_OVERRIDE; // for vec4
    virtual void    glUniform1iv ( GLint loc, GLsizei cnt, const GLint *vp   ) OGLCALC_OVERRIDE; // for int
    virtual void    glUniform2iv ( GLint loc, GLsizei cnt, const GLint *vp   ) OGLCALC_OVERRIDE; // for ivec2
    virtual void    glUniform3iv ( GLint loc, GLsizei cnt, const GLint *vp   ) OGLCALC_OVERRIDE; // for ivec3
    virtual void    glUniform4iv ( GLint loc, GLsizei cnt, const GLint *vp   ) OGLCALC_OVERRIDE; // for ivec4
    virtual void    glUniform1uiv( GLint loc, GLsizei cnt, const GLuint *vp  ) OGLCALC_OVERRIDE; // for uint
    virtual void    glUniform2uiv( GLint loc, GLsizei cnt, const GLuint *vp  ) OGLCALC_OVERRIDE; // for uivec2
    virtual void    glUniform3uiv( GLint loc, GLsizei cnt, const GLuint *vp  ) OGLCALC_OVERRIDE; // for uivec3
    virtual void    glUniform4uiv( GLint loc, GLsizei cnt, const GLuint *vp  ) OGLCALC_OVERRIDE; // for uivec4
    virtual void    glUniformMatrix2fv( GLint loc, GLsizei cnt, GLboolean trans, const GLfloat *vp ) OGLCALC_OVERRIDE; // trans = GL_TRUE means row major matrix
    virtual void    glUniformMatrix3fv( GLint loc, GLsizei cnt, GLboolean trans, const GLfloat *vp ) OGLCALC_OVERRIDE;
    virtual void    glUniformMatrix4fv( GLint loc, GLsizei cnt, GLboolean trans, const GLfloat *vp ) OGLCALC_OVERRIDE;
    virtual void    glUniformMatrix2x3fv( GLint loc, GLsizei cnt, GLboolean trans, const GLfloat *vp ) OGLCALC_OVERRIDE;
    virtual void    glUniformMatrix3x2fv( GLint loc, GLsizei cnt, GLboolean trans, const GLfloat *vp ) OGLCALC_OVERRIDE;
    virtual void    glUniformMatrix2x4fv( GLint loc, GLsizei cnt, GLboolean trans, const GLfloat *vp ) OGLCALC_OVERRIDE;
    virtual void    glUniformMatrix4x2fv( GLint loc, GLsizei cnt, GLboolean trans, const GLfloat *vp ) OGLCALC_OVERRIDE;
    virtual void    glUniformMatrix3x4fv( GLint loc, GLsizei cnt, GLboolean trans, const GLfloat *vp ) OGLCALC_OVERRIDE;
    virtual void    glUniformMatrix4x3fv( GLint loc, GLsizei cnt, GLboolean trans, const GLfloat *vp ) OGLCALC_OVERRIDE;


    static bool isSupport( int major, int minor, bool gles = false, QSurfaceFormat::OpenGLContextProfile profile = QSurfaceFormat::NoProfile );
private:
    void *m_obj;
};

}
#endif
