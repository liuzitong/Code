// ////////////////////////////////////////////////////////////////////////////
// File: opengl calculator implement
// Author: W.B.
//
// ////////////////////////////////////////////////////////////////////////////
#ifndef OGLCALC_HXX
#define OGLCALC_HXX

#include <cstdlib>
#include <cstdint>
#include <cstddef>

namespace OglCalc {

// ////////////////////////////////////////////////////////////////////////////
// definitions
// ////////////////////////////////////////////////////////////////////////////
#define OGLCALC_API
#define OGLCALC_HIDDEN

typedef void  (* MemCntrTrace )( int req_cntr, const char *file, const char *func, int ln, const char *type );

#if __cplusplus >= 201103L || ( defined(_MSC_VER) && _MSC_VER >= 1800 )
#  define OGLCALC_CXX11
#  define OGLCALC_FINAL    final
#  define OGLCALC_OVERRIDE override
#else
#  undef  OGLCALC_CXX11
#  define OGLCALC_FINAL
#  define OGLCALC_OVERRIDE
#  error "this source need c++11 !"
#endif

#define OGLCALC_DISABLE_COPY( t ) private : t ( const t & ); t & operator = ( const t & );


// ////////////////////////////////////////////////////////////////////////////
//
// [ Functions ]  initialize the opengl functions
//
// ////////////////////////////////////////////////////////////////////////////
class OGLCALC_API GLFuncBroker;

enum Status {
    Status_OK = 0,
    Status_Unexcept = -1,
    Status_No_GLFuncBroker          = -100,
    Status_No_ShaderCompiler        = -101,
    Status_SSB_Bindings_Too_Small   = -102,

    Status_Failed_To_Compile_Shader = -200,
    Status_Failed_To_Link_Program   = -201
};

typedef void (* DeleteBrkrFunc )( void*, GLFuncBroker* );

OGLCALC_API  MemCntrTrace  setMemCntrTrace( MemCntrTrace );
OGLCALC_API  Status   init   ( GLFuncBroker*, DeleteBrkrFunc, void *deletebrkr_info );
OGLCALC_API  Status   deinit ( );

// ////////////////////////////////////////////////////////////////////////////
// [ broker ] the simple GL function broker ( version 0 )
// ////////////////////////////////////////////////////////////////////////////
class OGLCALC_API  GLFuncBroker {
private:
    void  *m_obj;
public :
    typedef unsigned int   GLenum;
    typedef unsigned char  GLboolean;
    typedef unsigned int   GLbitfield;
    typedef signed char    GLbyte;
    typedef char           GLchar;
    typedef short          GLshort;
    typedef int            GLint;
    typedef int            GLsizei;
    typedef unsigned char  GLubyte;
    typedef unsigned short GLushort;
    typedef unsigned int   GLuint;
    typedef float          GLfloat;
    typedef float          GLclampf;
    typedef double         GLdouble;
    typedef double         GLclampd;
    typedef void           GLvoid;
    typedef ptrdiff_t      GLsizeiptr;
    typedef ptrdiff_t      GLintptr;

    enum Intrface {
        Intrface_GLFuncBroker = 0,
    };

    GLFuncBroker();
    virtual ~GLFuncBroker();
    virtual void*   queryIntrface  ( Intrface );  // just return interface, do not add the reference.

    virtual const char*  shaderVersionStr() const = 0;

    virtual GLenum  glGetError     ( void ) = 0;
    virtual void    glGetIntegerv  ( GLenum pname, GLint *params) = 0;
    virtual void    glGetBooleanv  ( GLenum pname, GLboolean *params) = 0;
    virtual void    glGetIntegeri_v( GLenum target, GLuint index, GLint *data) = 0;

    virtual GLuint  glCreateShader ( GLenum type   ) = 0;
    virtual void    glDeleteShader ( GLuint shader ) = 0;
    virtual void    glCompileShader( GLuint shader ) = 0;
    virtual void    glShaderSource ( GLuint shader, GLsizei count, const char **string, const GLint *length ) = 0;
    virtual void    glGetShaderiv  ( GLuint shader, GLenum pname, GLint *params ) = 0;
    virtual void    glGetShaderInfoLog ( GLuint shader, GLsizei bufsize, GLsizei *length, char *infolog ) = 0;
    virtual void    glAttachShader ( GLuint program, GLuint shader ) = 0;
    virtual void    glDetachShader ( GLuint program, GLuint shader ) = 0;

    virtual GLuint  glCreateProgram ( ) = 0;
    virtual void    glDeleteProgram ( GLuint program ) = 0;
    virtual void    glProgramParameteri ( GLuint program, GLenum pname, GLint value ) = 0;
    virtual void    glLinkProgram   ( GLuint program ) = 0;
    virtual void    glGetProgramiv  ( GLuint program, GLenum pname, GLint *params ) = 0;
    virtual void    glGetProgramInfoLog ( GLuint program, GLsizei bufsize, GLsizei *length, char *infolog ) = 0;
    virtual void    glUseProgram    ( GLuint program ) = 0;

    virtual void    glGenBuffers     ( GLsizei n, GLuint *buffers ) = 0;
    virtual void    glDeleteBuffers  ( GLsizei n, const GLuint *buffers ) = 0;
    virtual void    glBindBuffer     ( GLenum target, GLuint buffer ) = 0;
    virtual void    glBufferData     ( GLenum target, GLsizeiptr size, const void *data, GLenum usage ) = 0;
    virtual void*   glMapBufferRange ( GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access) = 0;
    virtual GLboolean  glUnmapBuffer ( GLenum target ) = 0;
    virtual void    glBindBufferBase ( GLenum target, GLuint index, GLuint buffer ) = 0;
    virtual void    glGetBufferParameteriv ( GLenum target, GLenum value, GLint * data ) = 0;

    virtual void    glDispatchCompute ( GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z) = 0;
    virtual void    glMemoryBarrier   ( GLbitfield barriers ) = 0;

    // need changed to UBO ....
    virtual GLint   glGetUniformLocation ( GLuint program, const GLchar *name ) = 0;
    virtual void    glUniform1fv  ( GLint loc, GLsizei cnt, const GLfloat *vp ) = 0; // for float
    virtual void    glUniform2fv  ( GLint loc, GLsizei cnt, const GLfloat *vp ) = 0; // for vec2
    virtual void    glUniform3fv  ( GLint loc, GLsizei cnt, const GLfloat *vp ) = 0; // for vec3
    virtual void    glUniform4fv  ( GLint loc, GLsizei cnt, const GLfloat *vp ) = 0; // for vec4
    virtual void    glUniform1iv  ( GLint loc, GLsizei cnt, const GLint *vp   ) = 0; // for int
    virtual void    glUniform2iv  ( GLint loc, GLsizei cnt, const GLint *vp   ) = 0; // for ivec2
    virtual void    glUniform3iv  ( GLint loc, GLsizei cnt, const GLint *vp   ) = 0; // for ivec3
    virtual void    glUniform4iv  ( GLint loc, GLsizei cnt, const GLint *vp   ) = 0; // for ivec4
    virtual void    glUniform1uiv ( GLint loc, GLsizei cnt, const GLuint *vp  ) = 0; // for uint
    virtual void    glUniform2uiv ( GLint loc, GLsizei cnt, const GLuint *vp  ) = 0; // for uivec2
    virtual void    glUniform3uiv ( GLint loc, GLsizei cnt, const GLuint *vp  ) = 0; // for uivec3
    virtual void    glUniform4uiv ( GLint loc, GLsizei cnt, const GLuint *vp  ) = 0; // for uivec4
    virtual void    glUniformMatrix2fv  ( GLint loc, GLsizei cnt, GLboolean trans, const GLfloat *vp ) = 0; // trans = GL_TRUE means row major matrix
    virtual void    glUniformMatrix3fv  ( GLint loc, GLsizei cnt, GLboolean trans, const GLfloat *vp ) = 0;
    virtual void    glUniformMatrix4fv  ( GLint loc, GLsizei cnt, GLboolean trans, const GLfloat *vp ) = 0;
    virtual void    glUniformMatrix2x3fv( GLint loc, GLsizei cnt, GLboolean trans, const GLfloat *vp ) = 0;
    virtual void    glUniformMatrix3x2fv( GLint loc, GLsizei cnt, GLboolean trans, const GLfloat *vp ) = 0;
    virtual void    glUniformMatrix2x4fv( GLint loc, GLsizei cnt, GLboolean trans, const GLfloat *vp ) = 0;
    virtual void    glUniformMatrix4x2fv( GLint loc, GLsizei cnt, GLboolean trans, const GLfloat *vp ) = 0;
    virtual void    glUniformMatrix3x4fv( GLint loc, GLsizei cnt, GLboolean trans, const GLfloat *vp ) = 0;
    virtual void    glUniformMatrix4x3fv( GLint loc, GLsizei cnt, GLboolean trans, const GLfloat *vp ) = 0;
};

// ////////////////////////////////////////////////////////////////////////////
// [ class ]       text that used for output information
// ////////////////////////////////////////////////////////////////////////////
class OGLCALC_API  Text OGLCALC_FINAL {
public:
    explicit Text ( );
    ~Text ( );
    Text ( size_t size );
    Text ( const Text & );
    Text &  operator = ( const Text & );
#ifdef OGLCALC_CXX11
    Text ( Text &&ot ) { m_obj = ot.m_obj; ot.m_obj = nullptr; }
    Text &  operator = ( Text &&ot ) { if ( m_obj != ot.m_obj ) { makeNull(); m_obj = ot.m_obj; ot.m_obj = nullptr; } return *this; }
#endif
    bool   isNull() const;
    void   makeNull();
    bool   isEmpty() const;
    const char*  constData() const;
    char*   data();
    size_t  size() const;
    void    shrink();
    Text    toLower() const;

    bool operator == (const Text &) const;
    bool operator == (const char* ) const;
    inline bool operator != (const Text &ot ) { return !( *this == ot ); }

    static Text  copyFrom( const char*, size_t );
    static Text  format  ( const char *fmt, ...  );
private:
    void      *m_obj;
};


// ////////////////////////////////////////////////////////////////////////////
// [ buffer object ] SSBO
// ////////////////////////////////////////////////////////////////////////////
class OGLCALC_API  Buffer OGLCALC_FINAL {
public:
    enum MapHint {
        MapHint_ReadWrite = 0, MapHint_ReadOnly, MapHint_WriteOnly
    };
    enum TypeHint {
        TypeHint_App_ToGL = 0, TypeHint_GL_ToApp, TypeHint_GL_ToGL
    };

    explicit Buffer( );
    Buffer ( TypeHint, bool is_modi_rept, size_t req_sz );
    ~Buffer();
    Buffer( const Buffer & );
    Buffer & operator = ( const Buffer & );
#ifdef OGLCALC_CXX11
    Buffer ( Buffer &&ot ) { m_obj = ot.m_obj; ot.m_obj = nullptr; }
    Buffer &  operator = ( Buffer &&ot ) { if ( m_obj != ot.m_obj ) { makeNull(); m_obj = ot.m_obj; ot.m_obj = nullptr; } return *this; }
#endif
    void      makeNull();
    bool      isNull() const;
    void      cvtToType( TypeHint, bool is_modi_rept );
    intptr_t  glId() const;
    size_t    size() const;
    void      mapAndDo( MapHint, void (* cb )( void *cb_ctxt, void* data, size_t data_sz ), void *cb_ctxt );
private:
    void *m_obj;
};

// ////////////////////////////////////////////////////////////////////////////
//
// a helper gpu platform information about compute shader.
// NOTE: this class should create in OpenGL Thread
//
// ////////////////////////////////////////////////////////////////////////////
class OGLCALC_API  ComputeGLInfo OGLCALC_FINAL {
public:
    enum Dim { Dim_X = 0, Dim_Y = 1 , Dim_Z = 2 };
    explicit ComputeGLInfo();
    ~ComputeGLInfo();
    ComputeGLInfo( const ComputeGLInfo & );
    ComputeGLInfo & operator = ( const ComputeGLInfo & );
#ifdef OGLCALC_CXX11
    ComputeGLInfo ( ComputeGLInfo &&ot ) { m_obj = ot.m_obj; ot.m_obj = nullptr; }
    ComputeGLInfo & operator = ( ComputeGLInfo &&ot ) { if ( m_obj != ot.m_obj ) { makeNull(); m_obj = ot.m_obj; ot.m_obj = nullptr; } return *this; }
#endif
    void   makeNull();
    bool   isNull() const;
    bool   isShaderCompilerSupport() const;
    int    maxWorkGroupCount( Dim ) const;
    int    maxWorkGroupSize ( Dim ) const;
    int    maxSsboBindings() const;     // shared buffer object bindings
    int    maxSsbNumPerShader() const;  // shared storage block number in a shader
    int    maxSsbSizeInShader() const;  // shared storage block size declared in shader ( just for Fixed size SSB in shader declare, not for glBufferData )

    static ComputeGLInfo  query();
private:
    void *m_obj;
};


// ////////////////////////////////////////////////////////////////////////////
//
// the gpu program class, this class need running in opengl thread.
//
// ////////////////////////////////////////////////////////////////////////////
class OGLCALC_API  Program OGLCALC_FINAL {
public:
    struct Uniform_vec2 { float x, y; };
    struct Uniform_vec3 { float x, y, z; };
    struct Uniform_vec4 { float x, y, z, w; };
    struct Uniform_ivec2{ int x, y; };
    struct Uniform_ivec3{ int x, y, z; };
    struct Uniform_ivec4{ int x, y, z, w; };
    struct Uniform_uvec2{ unsigned int x, y; };
    struct Uniform_uvec3{ unsigned int x, y, z; };
    struct Uniform_uvec4{ unsigned int x, y, z, w; };
    struct Uniform_mat2x2 { float m[ 2*2 ]; };
    struct Uniform_mat3x3 { float m[ 3*3 ]; };
    struct Uniform_mat4x4 { float m[ 4*4 ]; };
    struct Uniform_mat2x3 { float m[ 2*3 ]; };
    struct Uniform_mat3x2 { float m[ 3*2 ]; };
    struct Uniform_mat2x4 { float m[ 2*4 ]; };
    struct Uniform_mat4x2 { float m[ 4*2 ]; };
    struct Uniform_mat3x4 { float m[ 3*4 ]; };
    struct Uniform_mat4x3 { float m[ 4*3 ]; };

    explicit Program( const Text &compute_shader, bool req_append_shader_version );
    ~Program();
    Status  status() const;
    Text    infoLog() const;

    bool    setBuffer( const Buffer &, intptr_t bind_point );
    Buffer  buffer( intptr_t bind_point ) const;
    void    rmvBuffer( intptr_t bind_point );
    void    clearAllBuffers();

    bool    use();
    bool    run( size_t x_num, size_t y_num = 1, size_t z_num = 1 );

    bool    setUniform( const char*, size_t cnt, const float *vp );
    bool    setUniform( const char*, size_t cnt, const int   *vp );
    bool    setUniform( const char*, size_t cnt, const unsigned int *vp );
    bool    setUniform( const char*, size_t cnt, const Uniform_vec2* );
    bool    setUniform( const char*, size_t cnt, const Uniform_vec3* );
    bool    setUniform( const char*, size_t cnt, const Uniform_vec4* );
    bool    setUniform( const char*, size_t cnt, const Uniform_ivec2* );
    bool    setUniform( const char*, size_t cnt, const Uniform_ivec3* );
    bool    setUniform( const char*, size_t cnt, const Uniform_ivec4* );
    bool    setUniform( const char*, size_t cnt, const Uniform_uvec2* );
    bool    setUniform( const char*, size_t cnt, const Uniform_uvec3* );
    bool    setUniform( const char*, size_t cnt, const Uniform_uvec4* );
    bool    setUniform( const char*, size_t cnt, bool req_trans, const Uniform_mat2x2* );
    bool    setUniform( const char*, size_t cnt, bool req_trans, const Uniform_mat3x3* );
    bool    setUniform( const char*, size_t cnt, bool req_trans, const Uniform_mat4x4* );
    bool    setUniform( const char*, size_t cnt, bool req_trans, const Uniform_mat2x3* );
    bool    setUniform( const char*, size_t cnt, bool req_trans, const Uniform_mat3x2* );
    bool    setUniform( const char*, size_t cnt, bool req_trans, const Uniform_mat2x4* );
    bool    setUniform( const char*, size_t cnt, bool req_trans, const Uniform_mat4x2* );
    bool    setUniform( const char*, size_t cnt, bool req_trans, const Uniform_mat3x4* );
    bool    setUniform( const char*, size_t cnt, bool req_trans, const Uniform_mat4x3* );
private:
    void *m_obj;
    OGLCALC_DISABLE_COPY( Program )
};


}

#endif
