#ifndef QXPACK_IC_QSGOGLSHADERHLP_P_CXX
#define QXPACK_IC_QSGOGLSHADERHLP_P_CXX

#include "qxpack_ic_qsgoglshaderhlp_p.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/common/qxpack_ic_pimplprivtemp.hpp"
#include <QMap>
#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>
#include <QReadWriteLock>
#include <QAtomicInt>
#include <QAtomicPointer>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
// inner data
// ////////////////////////////////////////////////////////////////////////////
class  QXPACK_IC_HIDDEN  IcQSGOglShaderHlpPriv {
private:
    QMap<QString,QPair<int,GLuint>>  m_prog_map;
public :
    explicit IcQSGOglShaderHlpPriv( );
    ~IcQSGOglShaderHlpPriv( );
    inline auto  progMapRef() -> QMap<QString,QPair<int,GLuint>> & { return m_prog_map; }
};

// ctor
IcQSGOglShaderHlpPriv :: IcQSGOglShaderHlpPriv ( )
{
}

// dtor
IcQSGOglShaderHlpPriv :: ~IcQSGOglShaderHlpPriv ( )
{
    auto ctxt = QOpenGLContext::currentContext();
    auto func = ctxt->functions();

    auto curr_prog_id = GLint(0);
    func->glGetIntegerv( GL_CURRENT_PROGRAM, &curr_prog_id );
    auto citr = m_prog_map.constBegin();
    while ( citr != m_prog_map.constEnd()) {
        auto p_id = citr.value().second; citr ++;
        if ( p_id == GLuint( curr_prog_id )) { func->glUseProgram(0); }
        func->glDeleteProgram( p_id );
    }
}

// ////////////////////////////////////////////////////////////////////////////
// global functions
// ////////////////////////////////////////////////////////////////////////////
static QReadWriteLock        g_rw_locker;
static QAtomicInt            g_ref_cntr(0);
static QAtomicPointer<IcQSGOglShaderHlpPriv>  g_ptr(Q_NULLPTR);

static auto   gGetHlpPriv( ) -> IcQSGOglShaderHlpPriv*
{
    g_rw_locker.lockForWrite();
    auto  ptr = g_ptr.loadAcquire();
    if ( ptr == Q_NULLPTR ) {
        ptr = qxpack_ic_new( IcQSGOglShaderHlpPriv );
        g_ptr.storeRelease( ptr );
    }
    g_ref_cntr.fetchAndAddOrdered(1);
    g_rw_locker.unlock();
    return ptr;
}

static auto   gFreeHlpPriv( ) -> bool
{
     bool is_free = false;

     g_rw_locker.lockForWrite();
     auto ptr = g_ptr.loadAcquire();
     if ( ptr != Q_NULLPTR ) {
         if ( g_ref_cntr.fetchAndSubOrdered(1) - 1 <= 0 ) {
             qxpack_ic_delete( ptr, IcQSGOglShaderHlpPriv );
             g_ptr.storeRelease(Q_NULLPTR);
             is_free = true;
         }
     } else {
         g_ref_cntr.storeRelease(0);
     }
     g_rw_locker.unlock();

     return is_free;
}

// ////////////////////////////////////////////////////////////////////////////
// private functions
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcQSGOglShaderHlp:: IcQSGOglShaderHlp( )
{
    m_obj = gGetHlpPriv();
}

// ============================================================================
// dtor
// ============================================================================
IcQSGOglShaderHlp:: ~IcQSGOglShaderHlp( )
{
    gFreeHlpPriv();
}

// ============================================================================
// methods
// ============================================================================
// query the program
// return the program id
auto     IcQSGOglShaderHlp:: tryAddRefProgram ( const QString &prog_name ) -> GLuint
{
    auto prog_id = GLuint(0);

    g_rw_locker.lockForRead();
    auto ptr = g_ptr.loadAcquire();
    if ( ptr != Q_NULLPTR ) {
        auto &map = ptr->progMapRef();
        auto itr = map.find( prog_name );
        if ( itr != map.end()) {
            prog_id = itr.value().second;
            itr.value().first ++;
        }
    }
    g_rw_locker.unlock();

    return prog_id;
}

// take and add program, the program ownership is owned by this helper object
auto     IcQSGOglShaderHlp:: takeAddProgram ( const QString &prog_name, GLuint prog_id ) -> bool
{
    bool is_add = false;

    g_rw_locker.lockForWrite();
    auto ptr = g_ptr.loadAcquire();
    if ( ptr != Q_NULLPTR ) {
        auto &map = ptr->progMapRef();
        auto citr = map.constFind( prog_name );
        if ( citr == map.constEnd() ) {
            map.insert( prog_name, QPair<int,GLuint>( 1, prog_id ));
            qxpack_ic_incr_new_cntr(1);
        }
        is_add = true;
    }
    g_rw_locker.unlock();

    return is_add;
}

// remove
auto     IcQSGOglShaderHlp:: decRefProgram ( const QString &prog_name ) -> void
{
     GLuint prog_id = 0;

     g_rw_locker.lockForWrite();
     auto ptr = g_ptr.loadAcquire();
     if ( ptr != Q_NULLPTR ) {
         auto &map = ptr->progMapRef();
         auto itr = map.find( prog_name );
         if ( itr != map.end()) {
             if ( ( -- itr.value().first ) == 0 ) {
                 prog_id = itr.value().second;
                 map.erase( itr );
                 qxpack_ic_decr_new_cntr(1);
             }
         }
     }
     g_rw_locker.unlock();

     if ( prog_id != 0 ) {
         auto ef = QOpenGLContext::currentContext()->functions();
         auto curr_prog_id = GLint(0);
         ef->glGetIntegerv( GL_CURRENT_PROGRAM, &curr_prog_id );
         if ( prog_id == GLuint( curr_prog_id )) { ef->glUseProgram(0); }
         ef->glDeleteProgram( prog_id );
     }
}

//! [ static ] compile shader function, return a program if succ.

auto     IcQSGOglShaderHlp:: compileShader (
    const QByteArray &shd, QByteArray &info_log, GLenum sdr_type
) -> GLuint
{
    auto gf = QOpenGLContext::currentContext()->functions();

    static GLboolean is_sdr_supp = false;
    if ( ! is_sdr_supp ) { gf->glGetBooleanv( GL_SHADER_COMPILER, & is_sdr_supp ); }
    if ( ! is_sdr_supp ) {
        qFatal("OpenGL does not support Shader Compiler!");
    }

    auto sd = gf->glCreateShader( sdr_type );
    GLint sd_ok = GL_FALSE;

    auto cs_txt = shd.constData();
    const char *cs_pptr[] = { cs_txt };
    const GLint cs_len[]  = { GLint( std::strlen( cs_txt )) };

    gf->glShaderSource ( sd, 1, cs_pptr, cs_len );
    gf->glCompileShader( sd );
    gf->glGetShaderiv  ( sd, GL_COMPILE_STATUS, &sd_ok );

    if ( sd_ok != GL_TRUE ) { // compile failed.
        GLint info_sz = 0;
        gf->glGetShaderiv( sd, GL_INFO_LOG_LENGTH, &info_sz );
        QByteArray  log( 8 + info_sz, 0 );
        GLsizei act_len = 0;
        gf->glGetShaderInfoLog( sd, info_sz, &act_len, log.data() );
        gf->glDeleteShader( sd ); sd = 0;
        info_log = log;
    }

    return ( sd_ok == GL_TRUE ? sd : 0 );
}

//! [ static ] create the program
auto     IcQSGOglShaderHlp:: createProgram (
    GLuint vtx_sd, GLuint geom_sd, GLuint frag_sd, QByteArray  &info_log,
    void (* LinkBefore )( void *ctxt, GLuint prog_id ), void *ctxt
) -> GLuint
{
    if ( vtx_sd == 0 || frag_sd == 0 ) { return 0; }
    auto gf = QOpenGLContext::currentContext()->functions();

    auto prog = gf->glCreateProgram();
    GLint prog_ok = GL_FALSE;

    gf->glAttachShader ( prog, vtx_sd  );
    gf->glAttachShader ( prog, frag_sd );
    if ( geom_sd != 0 ) { gf->glAttachShader( prog, geom_sd ); }

    if ( LinkBefore != Q_NULLPTR ) { LinkBefore( ctxt, prog ); }

    gf->glLinkProgram  ( prog );
    gf->glGetProgramiv ( prog, GL_LINK_STATUS, & prog_ok );

    if ( prog_ok != GL_TRUE ) {
       GLint info_sz = 0;
       gf->glGetProgramiv( prog, GL_INFO_LOG_LENGTH, &info_sz);
       QByteArray  log( 8 + info_sz, 0 );
       GLsizei act_len = 0;
       gf->glGetProgramInfoLog( prog, info_sz, &act_len, log.data() );
       info_log = log;
     }

     // detached shader if program is not need to use shader.
     gf->glDetachShader( prog, vtx_sd  );
     gf->glDetachShader( prog, frag_sd );
     if ( geom_sd != 0 ) { gf->glDetachShader( prog, geom_sd ); }

     if ( prog_ok != GL_TRUE ) { gf->glDeleteProgram( prog ); }
     return ( prog_ok == GL_TRUE ? prog : 0 );
}

}

#endif
