#ifndef OGLCALC_CXX
#define OGLCALC_CXX

#include "oglcalc.hxx"
#include <cstdlib>
#include <cstdint>
#include <atomic> //C++11
#include <mutex>
#include <vector>
#include <cstdarg>
#include <cstdio>
#include <cstring>

namespace OglCalc {

// ////////////////////////////////////////////////////////////////////////////
// [ mem cntr ] memory counter
// ////////////////////////////////////////////////////////////////////////////
static std::atomic<int>  g_atom(0);
static std::mutex        g_mem_trace_locker;
static std::atomic<MemCntrTrace>  g_mem_trace( nullptr );

static int   updateNewCntr ( int cntr, const char *file = nullptr, const char *func = nullptr, int ln = 0, const char *type = nullptr )
{
    auto old = g_atom.fetch_add(cntr);
    if ( g_mem_trace.load( std::memory_order_acquire ) != nullptr ) {
        g_mem_trace_locker.lock();
        auto ptr = g_mem_trace.load( std::memory_order_acquire );
        if ( ptr != nullptr ) { ptr( cntr, file, func, ln, type ); }
        g_mem_trace_locker.unlock();
    }
    return old + cntr;
}

OGLCALC_API  auto  setMemCntrTrace( MemCntrTrace tc ) -> MemCntrTrace
{
    g_mem_trace_locker.lock();
    auto old = g_mem_trace.load( std::memory_order_acquire );
    g_mem_trace.store( tc );
    g_mem_trace_locker.unlock();
    return old;
}

#if defined(_DEBUG)
#  define  oglcalc_new( t, ... )         ( OglCalc::updateNewCntr( 1,__FILE__,__FUNCTION__,__LINE__, #t ),  new t ( __VA_ARGS__ ))
#  define  oglcalc_delete( o, t )      do{ OglCalc::updateNewCntr(-1,__FILE__,__FUNCTION__,__LINE__, #t ); delete  static_cast< t *>( o ); }while(0)
#  define  oglcalc_alloc( sz )           ( OglCalc::updateNewCntr( 1,__FILE__,__FUNCTION__,__LINE__ ),     std::malloc( sz ) )
#  define  oglcalc_realloc( p, sz )      ( OglCalc::updateNewCntr(  ( p != nullptr ? 0 : 1 ), __FILE__,__FUNCTION__,__LINE__ ),  std::realloc( p, sz ) )
#  define  oglcalc_free( p )           do{ OglCalc::updateNewCntr(-1,__FILE__,__FUNCTION__,__LINE__);  std::free( p ); }while(0)
#else
#  define  oglcalc_new( t, ... )         ( OglCalc::updateNewCntr(1), new t ( __VA_ARGS__ ))
#  define  oglcalc_delete( o, t )      do{ OglCalc::updateNewCntr(-1); delete static_cast< t *>( o ); }while(0)
#  define  oglcalc_alloc( sz )           ( OglCalc::updateNewCntr(1), std::malloc( sz ) )
#  define  oglcalc_realloc( p, sz )      ( OglCalc::updateNewCntr(( p != nullptr ? 0 : 1 )), std::realloc( p, sz ))
#  define  oglcalc_free( p )           do{ OglCalc::updateNewCntr(-1); std::free( p ); }while(0)
#endif

#define PImplPrivTemp_PtrCast( t, o )  ( reinterpret_cast< t >( reinterpret_cast< intptr_t >( o )))
#define PImplPrivTemp_IntPtrCast( o )  ( reinterpret_cast< intptr_t >( o ))
#define PImplPrivTemp_new              oglcalc_new
#define PImplPrivTemp_delete           oglcalc_delete
#define oglcalc_objcast( t, o )        ( static_cast< t >( o ) )



// some OpenGL macros
#define GL_UNIFORM_BUFFER                     0x8A11
#define GL_UNIFORM_BUFFER_BINDING             0x8A28
#define GL_SHADER_COMPILER                    0x8DFA
#define GL_COMPUTE_SHADER                     0x91B9
#define GL_CURRENT_PROGRAM                    0x8B8D
#define GL_MAX_COMPUTE_UNIFORM_BLOCKS         0x91BB
#define GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS    0x91BC
#define GL_MAX_COMPUTE_IMAGE_UNIFORMS         0x91BD
#define GL_MAX_COMPUTE_SHARED_MEMORY_SIZE     0x8262
#define GL_MAX_COMPUTE_UNIFORM_COMPONENTS     0x8263
#define GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS       0x8264
#define GL_MAX_COMPUTE_ATOMIC_COUNTERS              0x8265
#define GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS  0x8266
#define GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS  0x90EB
#define GL_MAX_COMPUTE_WORK_GROUP_COUNT        0x91BE
#define GL_MAX_COMPUTE_WORK_GROUP_SIZE         0x91BF
#define GL_COMPUTE_WORK_GROUP_SIZE             0x8267
#define GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS   0x90DB
#define GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS  0x90DC
#define GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS  0x90DD
#define GL_MAX_SHADER_STORAGE_BLOCK_SIZE       0x90DE
#define GL_SHADER_STORAGE_BUFFER          0x90D2
#define GL_SHADER_STORAGE_BUFFER_BINDING  0x90D3
#define GL_SHADER_STORAGE_BARRIER_BIT     0x00002000

#define GL_STATIC_DRAW                    0x88E4
#define GL_STATIC_READ                    0x88E5
#define GL_STATIC_COPY                    0x88E6
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_DYNAMIC_READ                   0x88E9
#define GL_DYNAMIC_COPY                   0x88EA

#define GL_BUFFER_ACCESS                  0x88BB
#define GL_BUFFER_MAPPED                  0x88BC
#define GL_BUFFER_MAP_POINTER             0x88BD

#define GL_MAP_READ_BIT                   0x0001
#define GL_MAP_WRITE_BIT                  0x0002
#define GL_MAP_INVALIDATE_RANGE_BIT       0x0004
#define GL_MAP_INVALIDATE_BUFFER_BIT      0x0008

#define GL_TRUE                            1
#define GL_FALSE                           0
#define GL_COMPILE_STATUS                  0x8B81
#define GL_INFO_LOG_LENGTH                 0x8B84
#define GL_PROGRAM_BINARY_RETRIEVABLE_HINT 0x8257
#define GL_LINK_STATUS                     0x8B82

#define GL_SPEC_MIN_CS_WKGRP_CNT      65535
#define GL_SPEC_MIN_CS_WKGRP_SIZE_XY  1024
#define GL_SPEC_MIN_CS_WKGRP_SIZE_Z   64
#define GL_SPEC_MIN_LIMIT_CS_INVOC_PER_GRP  1024  // x * y * z < limit



// ////////////////////////////////////////////////////////////////////////////
// [ local functions ]
// ////////////////////////////////////////////////////////////////////////////
struct GLFuncBrokerInfo {
    GLFuncBroker   *brkr;
    DeleteBrkrFunc  del_func;
    void           *del_info;
    GLFuncBrokerInfo( ) : brkr( nullptr ), del_func( nullptr ), del_info( nullptr ) { }
    GLFuncBrokerInfo( GLFuncBroker *bk, DeleteBrkrFunc func, void *info ) : brkr(bk), del_func(func), del_info(info) { }
};

static GLFuncBrokerInfo g_glfunc_info;
static std::mutex       g_glfunc_brkr_locker;

static auto  gSetupBrkr( const GLFuncBrokerInfo &info  ) -> GLFuncBrokerInfo
{
    g_glfunc_brkr_locker.lock();
    auto old = g_glfunc_info;
    g_glfunc_info = info;
    g_glfunc_brkr_locker.unlock();

    return old;
}

inline static auto  glFunc() -> GLFuncBroker* { return g_glfunc_info.brkr; }

OGLCALC_API auto  init( GLFuncBroker *brkr, DeleteBrkrFunc func, void *func_info ) -> Status
{
    gSetupBrkr( GLFuncBrokerInfo( brkr, func, func_info ) );
    return Status_OK;
}

OGLCALC_API auto  deinit( ) -> Status
{
    auto old = gSetupBrkr( GLFuncBrokerInfo() );
    if ( old.del_func != nullptr ) {
        old.del_func( old.del_info, old.brkr );
    }
    return Status_OK;
}

static auto   gCompShader( GLFuncBroker *gf, const Text &cs, Text &info_log ) -> GLFuncBroker::GLuint
{
    auto sd = gf->glCreateShader( GL_COMPUTE_SHADER );
    GLFuncBroker::GLint sd_ok = GL_FALSE;

    auto cs_txt = cs.constData();
    const char *cs_pptr[] = { cs_txt };
    const GLFuncBroker::GLint cs_len[] = { GLFuncBroker::GLint( std::strlen( cs_txt )) };

    gf->glShaderSource ( sd, 1, cs_pptr, cs_len );
    gf->glCompileShader( sd );
    gf->glGetShaderiv  ( sd, GL_COMPILE_STATUS, &sd_ok );

    if ( sd_ok != GL_TRUE ) { // compile failed.
        GLFuncBroker::GLint info_sz = 0;
        gf->glGetShaderiv( sd, GL_INFO_LOG_LENGTH, &info_sz );
        Text  log( size_t( 8 + info_sz ));
        auto  txt_ptr = log.data();
        GLFuncBroker::GLsizei act_len = 0;
        gf->glGetShaderInfoLog( sd, info_sz, &act_len, txt_ptr );
        gf->glDeleteShader( sd ); sd = 0;
        info_log = log;
    }

    return ( sd_ok == GL_TRUE ? sd : 0 );
}

static auto   gLinkProg( GLFuncBroker *gf, GLFuncBroker::GLuint sd, Text &info_log ) -> GLFuncBroker::GLuint
{
    auto prog = gf->glCreateProgram(); GLFuncBroker::GLint prog_ok = GL_FALSE;

    gf->glAttachShader ( prog, sd );
    gf->glProgramParameteri ( prog, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE );
    gf->glLinkProgram  ( prog );
    gf->glGetProgramiv ( prog, GL_LINK_STATUS, & prog_ok );

    if ( prog_ok != GL_TRUE ) {
       GLFuncBroker::GLint info_sz = 0;
       gf->glGetProgramiv( prog, GL_INFO_LOG_LENGTH, &info_sz);
       Text  log( size_t( 8 + info_sz ));
       auto  txt_ptr = log.data();
       GLFuncBroker::GLsizei act_len = 0;
       gf->glGetProgramInfoLog( prog, info_sz, &act_len, txt_ptr );
       info_log = log;
     }
     gf->glDetachShader( prog, sd ); // need not to use this shader on this program

     if ( prog_ok != GL_TRUE ) { gf->glDeleteProgram( prog ); }

     return ( prog_ok == GL_TRUE ? prog : 0 );
}


// ////////////////////////////////////////////////////////////////////////////
// [ template ] PImpl. class
// ////////////////////////////////////////////////////////////////////////////
template <typename T>
class OGLCALC_HIDDEN PImplPrivTemp  {
public:
    PImplPrivTemp () { m_ref_cntr.store(1); }
    virtual ~PImplPrivTemp ( ) { }
    static T*    buildIfNull ( void **w_obj );
    static T*    createInstance ( )           { return PImplPrivTemp_new( T ); }
    static T*    createInstance ( void *ref ) { return PImplPrivTemp_new( T,  * PImplPrivTemp_PtrCast( T*, ref ) ); }
    static T*    addRef    ( void *cd )       {
        if ( cd != nullptr ) { PImplPrivTemp_PtrCast( T*, cd )->addRefCntr(); }
        return  PImplPrivTemp_PtrCast( T*, cd );
    }
    static bool  releaseRef( void *   );
    static bool  attach    ( void **src_obj, void **dst_obj );
    static T*    instanceCow ( void ** );
protected:
    inline int  addRefCntr( )    { return m_ref_cntr.fetch_add(1) + 1; }
    inline int  releaseRefCntr( ){ return m_ref_cntr.fetch_sub(1) - 1; }
    inline int  currRefCntr( )   { return m_ref_cntr.load(); }
private:
    std::atomic<int>  m_ref_cntr;
};

template <typename T >
auto     PImplPrivTemp<T> :: buildIfNull( void **w_obj ) -> T*
{
    if (  w_obj == nullptr ) { return nullptr; }
    if ( *w_obj == nullptr ) { *w_obj = PImplPrivTemp<T>::createInstance( ); }
    return PImplPrivTemp_PtrCast( T*, *w_obj );
}

template <typename T>
auto     PImplPrivTemp<T> :: releaseRef ( void *cd ) -> bool
{
    bool is_released = false;
    if ( cd != nullptr ) {
        if ( PImplPrivTemp_PtrCast( T*, cd )->releaseRefCntr() == 0 ) {
            PImplPrivTemp_delete( cd, T );
            is_released = true;
        }
    }
    return is_released;
}

template <typename T >
auto     PImplPrivTemp<T> :: attach ( void **src_obj, void **dst_obj ) -> bool
{
    if ( src_obj == dst_obj ) { return true; }
    if ( src_obj == nullptr ) { return false; }
    if ( dst_obj != nullptr  &&  *src_obj == *dst_obj ) { return true; } // need not to attach.

    if ( *src_obj != nullptr ) {
        PImplPrivTemp<T>::releaseRef( *src_obj );
        *src_obj = nullptr;
    }

    intptr_t tmp_ptr = 0x100;
    if ( dst_obj > reinterpret_cast<void**>( tmp_ptr )) {
        if ( *dst_obj != nullptr ) {
            *src_obj = PImplPrivTemp<T>::addRef( *dst_obj );
        }
    }

    return true;
}

template <typename T>
auto     PImplPrivTemp<T> :: instanceCow ( void **w_obj ) -> T*
{
    if ( w_obj == nullptr  ) { return nullptr; }
    if ( *w_obj == nullptr ) { *w_obj = PImplPrivTemp<T>::createInstance( ); }
    if ( PImplPrivTemp_PtrCast( T*, *w_obj)->currRefCntr( ) == 1 ) {
        return PImplPrivTemp_PtrCast( T*, *w_obj );
    }

    T *new_obj = PImplPrivTemp<T>::createInstance( *w_obj );
    PImplPrivTemp<T>::releaseRef( *w_obj );
    *w_obj = new_obj;

    return new_obj;
}

// ////////////////////////////////////////////////////////////////////////////
// [ GLFuncBroker  ]
// ////////////////////////////////////////////////////////////////////////////
#define T_GLF( o )  oglcalc_objcast( GLFuncBrokerPriv*, o )
class OGLCALC_HIDDEN  GLFuncBrokerPriv {
private:
public :
    explicit GLFuncBrokerPriv( ) {  }
    ~GLFuncBrokerPriv( ) { }
};
GLFuncBroker :: GLFuncBroker ( ) { m_obj = oglcalc_new( GLFuncBrokerPriv ); }
GLFuncBroker :: ~GLFuncBroker( ) { oglcalc_delete( m_obj, GLFuncBrokerPriv ); }
auto     GLFuncBroker :: queryIntrface( Intrface ) -> void* { return nullptr; }


// ////////////////////////////////////////////////////////////////////////////
// [ class ]  text component
// ////////////////////////////////////////////////////////////////////////////
#define T_TxtPriv( o )  oglcalc_objcast( TextPriv*, o )
class OGLCALC_HIDDEN  TextPriv : public PImplPrivTemp<TextPriv> {
private  :  char *m_text; size_t m_size;
protected:
    inline void freeText( ) { if ( m_text != nullptr ) { oglcalc_free( m_text ); m_text = nullptr; m_size = 0; } }
           void allocText( size_t sz );
public :
    explicit TextPriv( ) : m_text( nullptr ), m_size(0) { }
    TextPriv ( const TextPriv & );
    virtual ~TextPriv( ) OGLCALC_OVERRIDE;
    inline auto  init( size_t size ) -> void { allocText( size ); }
    inline auto  textRef() -> char*&  { return m_text; }
    inline auto  sizeRef() -> size_t& { return m_size; }
};

TextPriv :: ~TextPriv( ) { freeText(); }

TextPriv ::  TextPriv ( const TextPriv &ot )
{
    m_size = ot.m_size;
    m_text = nullptr;
    if ( ot.m_text != nullptr ) {
        allocText( ot.m_size );
        std::memcpy( const_cast<char*>( m_text ), ot.m_text, size_t( m_size ));
    }
}

auto     TextPriv :: allocText( size_t sz ) -> void
{
    freeText();
    // for sz == 0, means a empty string. we allocate 2 bytes for fill zero.
    m_text = reinterpret_cast<char*>( oglcalc_alloc( size_t( sz + 2 )));
    m_size = sz;
    std::memset( const_cast<char*>( m_text ), 0, size_t( sz + 2 ));
}

Text ::  Text ( ) { m_obj = nullptr; }
Text :: ~Text ( ) { makeNull(); }
Text ::  Text ( size_t size )
{
    m_obj = nullptr;
    TextPriv::buildIfNull( &m_obj );
    T_TxtPriv( m_obj )->init( size );
}

Text :: Text ( const Text &ot )
{ m_obj = nullptr; TextPriv::attach( &m_obj, const_cast<void**>( &ot.m_obj )); }

auto     Text :: operator = ( const Text &ot ) -> Text &
{ TextPriv::attach( &m_obj, const_cast<void**>( &ot.m_obj )); return *this; }

auto     Text :: isNull() const -> bool  { return m_obj == nullptr; }
auto     Text :: makeNull() -> void      { if ( m_obj != nullptr ) { TextPriv::attach( &m_obj, nullptr ); m_obj = nullptr; }}
auto     Text :: isEmpty() const -> bool { return ( m_obj != nullptr ? T_TxtPriv(m_obj)->sizeRef() < 1 : true ); }
auto     Text :: constData() const -> const char* { return ( m_obj != nullptr ? T_TxtPriv(m_obj)->textRef() : nullptr ); }
auto     Text :: data() -> char*         { return ( m_obj != nullptr ? TextPriv::instanceCow(&m_obj)->textRef() : nullptr ); }
auto     Text :: size() const -> size_t  { return ( m_obj != nullptr ? T_TxtPriv(m_obj)->sizeRef() : 0 ); }

auto     Text :: shrink() -> void
{
    if ( m_obj != nullptr ) {
        auto priv = T_TxtPriv(m_obj);
        auto c_sz = std::strlen( priv->textRef() );
        if ( c_sz < priv->sizeRef() ) { // need shrink...
            void *new_obj = nullptr;
            TextPriv::buildIfNull( &new_obj );
            auto new_priv = T_TxtPriv(new_obj);
            new_priv->init( c_sz );
            std::memcpy( new_priv->textRef(), priv->textRef(), c_sz );
            makeNull();      // make current null.
            m_obj = new_obj; // save current created object.
        }
    }
}

auto     Text :: toLower() const -> Text
{
    if ( m_obj == nullptr ) { return Text(); }

    Text rsl( *this );
    TextPriv::instanceCow( &rsl.m_obj );

    auto *tgt = T_TxtPriv(rsl.m_obj)->textRef();
    size_t sz = T_TxtPriv(rsl.m_obj)->sizeRef();
    for ( size_t i = 0; i < sz; i ++ ) {
        if ( tgt[sz] >= 'A' && tgt[sz] <= 'Z' ) { tgt[sz] = tgt[sz] - 'A' + 'a'; }
    }
    return rsl;
}

auto     Text :: operator == ( const Text &ot ) const -> bool
{
    if ( m_obj == ot.m_obj ) { return true; }

    if ( m_obj == nullptr || ot.m_obj == nullptr ) { return false; }

    auto *src_txt = T_TxtPriv(m_obj)->textRef();
    auto *tgt_txt = T_TxtPriv(ot.m_obj)->textRef();

    if ( T_TxtPriv(m_obj)->textRef() == T_TxtPriv(ot.m_obj)->textRef()) { return true; }
    if ( T_TxtPriv(m_obj)->sizeRef() == T_TxtPriv(ot.m_obj)->sizeRef()) {
        return ( std::memcmp( src_txt, tgt_txt, T_TxtPriv(m_obj)->sizeRef()) == 0 );
    } else {
        return false;
    }
}

auto     Text :: operator == ( const char *ptr ) const -> bool
{
    if ( ptr == nullptr && m_obj == nullptr ) { return true; }
    if ( ptr == nullptr || m_obj == nullptr ) { return false; }

    auto *src_txt = T_TxtPriv(m_obj)->textRef();
    size_t tgt_len = std::strlen( ptr );

    if ( T_TxtPriv(m_obj)->textRef() == ptr ) { return true; }
    if ( T_TxtPriv(m_obj)->sizeRef() == tgt_len ) {
        return ( std::memcmp( src_txt, ptr, tgt_len ) == 0 );
    } else {
        return false;
    }
}

auto     Text :: copyFrom( const char *txt, size_t sz ) -> Text
{
    if ( txt == nullptr ) { return Text(); }
    if ( sz < 1 ) { sz = std::strlen( txt ); }
    Text text( sz );
    std::memcpy( T_TxtPriv(text.m_obj)->textRef(), txt, sz );
    return text;
}

auto     Text :: format( const char *fmt, ... ) -> Text
{
    va_list args1;
    va_start( args1, fmt );
    int vect_sz = 0; std::vector<char> buf;
    do {
        buf = std::vector<char>( size_t( vect_sz += 1024 ));
    } while ( std::vsprintf( buf.data(), fmt, args1 ) < 0 );
    va_end( args1 );
    return Text::copyFrom( buf.data(), 0 );
}

// ////////////////////////////////////////////////////////////////////////////
// [ class ] buffer object
// ////////////////////////////////////////////////////////////////////////////
#define T_Buf( o )  oglcalc_objcast( BufferPriv*, o )
class OGLCALC_HIDDEN BufferPriv : public PImplPrivTemp<BufferPriv> {
private:
    GLFuncBroker::GLuint m_gl_id; size_t m_sz;  GLFuncBroker *m_gl_func;
public :
    explicit BufferPriv( );
    BufferPriv( const BufferPriv & ) { std::abort();  } // never ocurred
    virtual ~BufferPriv() OGLCALC_OVERRIDE;
    //inline auto  init( size_t size, unsigned int bind_port ) -> void;
    inline auto  glId()      -> GLFuncBroker::GLuint& { return m_gl_id; }
    inline auto  size()      -> size_t&   { return m_sz; }
    inline auto  glFuncRef() -> GLFuncBroker*& { return m_gl_func; }
};

BufferPriv :: BufferPriv ( )
{
    m_sz = 0; m_gl_func = glFunc();
    if ( m_gl_func == nullptr ) { std::abort(); }
    m_gl_func->glGenBuffers( 1, &m_gl_id );
}

BufferPriv :: ~BufferPriv( )
{
    GLFuncBroker::GLint curr_bid = 0;
    m_gl_func->glGetIntegerv( GL_SHADER_STORAGE_BUFFER_BINDING, &curr_bid );
    GLFuncBroker::GLint is_map = GL_FALSE;
    m_gl_func->glGetBufferParameteriv( GL_SHADER_STORAGE_BUFFER, GL_BUFFER_MAPPED, &is_map );
    if ( curr_bid == GLFuncBroker::GLint( m_gl_id )) { // current buffer is binding
        if ( is_map == GL_TRUE ) { m_gl_func->glUnmapBuffer( GL_SHADER_STORAGE_BUFFER ); }
        m_gl_func->glBindBuffer( GL_SHADER_STORAGE_BUFFER, 0 );
    }
    m_gl_func->glDeleteBuffers( 1, & m_gl_id );
}

Buffer ::  Buffer ( ) { m_obj = nullptr; }
Buffer :: ~Buffer ( ) { makeNull(); }
Buffer ::  Buffer ( TypeHint h, bool is_modi_rept, size_t req_sz )
{
    if ( req_sz < 8 ) { req_sz = 8; }
    m_obj = nullptr;
    BufferPriv::buildIfNull(&m_obj);
    auto priv = T_Buf( m_obj );
    priv->size()     = req_sz;
    cvtToType( h, is_modi_rept );
}

Buffer ::  Buffer ( const Buffer &ot ) { m_obj = nullptr; BufferPriv::attach(&m_obj, const_cast<void**>(&ot.m_obj)); }
auto     Buffer :: operator = ( const Buffer &ot ) -> Buffer& { BufferPriv::attach(&m_obj, const_cast<void**>(&ot.m_obj)); return *this; }
auto     Buffer :: isNull()    const -> bool { return ( m_obj == nullptr ); }
auto     Buffer :: makeNull()        -> void { if ( m_obj != nullptr ) { BufferPriv::attach(&m_obj,nullptr); m_obj = nullptr; } }
auto     Buffer :: glId()      const -> intptr_t { return ( m_obj != nullptr ? intptr_t( T_Buf(m_obj)->glId()) : 0 ); }
auto     Buffer :: size()      const -> size_t   { return ( m_obj != nullptr ? T_Buf(m_obj)->size() : 0 ); }

auto     Buffer :: cvtToType( TypeHint h, bool is_modi_rept ) -> void
{
    if ( m_obj == nullptr ) { return; }

    auto priv = T_Buf( m_obj );
    auto gf = priv->glFuncRef();

    // check if current buffer is being binding
    GLFuncBroker::GLint curr_bid = 0;
    gf->glGetIntegerv( GL_SHADER_STORAGE_BUFFER_BINDING, &curr_bid );
    if ( curr_bid != GLFuncBroker::GLint( priv->glId()) ) { gf->glBindBuffer ( GL_SHADER_STORAGE_BUFFER, priv->glId() ); }

    switch( h ) {
    case TypeHint_App_ToGL : gf->glBufferData( GL_SHADER_STORAGE_BUFFER, GLFuncBroker::GLint( priv->size()), nullptr, ( is_modi_rept ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW )); break;
    case TypeHint_GL_ToApp : gf->glBufferData( GL_SHADER_STORAGE_BUFFER, GLFuncBroker::GLint( priv->size()), nullptr, ( is_modi_rept ? GL_DYNAMIC_READ : GL_STATIC_READ )); break;
    // GL to GL
    default: gf->glBufferData( GL_SHADER_STORAGE_BUFFER, GLFuncBroker::GLint( priv->size()), nullptr, ( is_modi_rept ? GL_DYNAMIC_COPY : GL_STATIC_COPY ) ); break;
    }

    // restore old binding
    if ( curr_bid != GLFuncBroker::GLint( priv->glId()) ) { gf->glBindBuffer( GL_SHADER_STORAGE_BUFFER, GLFuncBroker::GLuint( curr_bid )); }
}

auto     Buffer :: mapAndDo (MapHint hint, void (*cb )(void *, void *, size_t ), void *cb_ctxt ) -> void
{
    if ( m_obj == nullptr || cb == nullptr ) { return; }
    auto priv = T_Buf(m_obj);
    auto gf   = priv->glFuncRef();

    // map read write buffer
    GLFuncBroker::GLint curr_bid = 0;
    gf->glGetIntegerv( GL_SHADER_STORAGE_BUFFER_BINDING, &curr_bid );
    if ( curr_bid != GLFuncBroker::GLint( priv->glId()) ) { gf->glBindBuffer ( GL_SHADER_STORAGE_BUFFER, priv->glId() ); }

    // map by hint
    void *dp = nullptr;
    switch( hint ) {
    case MapHint_ReadOnly  : dp = gf->glMapBufferRange( GL_SHADER_STORAGE_BUFFER, 0, GLFuncBroker::GLsizeiptr( priv->size() ), GL_MAP_READ_BIT ); break;
    case MapHint_WriteOnly : dp = gf->glMapBufferRange( GL_SHADER_STORAGE_BUFFER, 0, GLFuncBroker::GLsizeiptr( priv->size() ), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT ); break;
    default :                dp = gf->glMapBufferRange( GL_SHADER_STORAGE_BUFFER, 0, GLFuncBroker::GLsizeiptr( priv->size() ), GL_MAP_READ_BIT | GL_MAP_WRITE_BIT ); break;
    }
    cb( cb_ctxt, dp, priv->size() );
    gf->glUnmapBuffer( GL_SHADER_STORAGE_BUFFER );

    if ( curr_bid != GLFuncBroker::GLint( priv->glId())) { gf->glBindBuffer( GL_SHADER_STORAGE_BUFFER, GLFuncBroker::GLuint( curr_bid )); }
}


// ////////////////////////////////////////////////////////////////////////////
// [ class ] compute information
// ////////////////////////////////////////////////////////////////////////////
#define T_CSGL( o )  oglcalc_objcast( ComputeGLInfoPriv*, o )
class OGLCALC_HIDDEN ComputeGLInfoPriv : public PImplPrivTemp<ComputeGLInfoPriv>{
private:
    GLFuncBroker::GLint  m_wkgrp_cnt[3], m_wkgrp_sz[3];
    GLFuncBroker::GLint  m_max_invoca_per_wkgrp;
    GLFuncBroker::GLint  m_max_ssbo_bind_port, m_max_cs_ssb_num_per_shader, m_max_cs_ssb_sz_per_shader;
    GLFuncBroker::GLboolean m_is_shader_comp_support;
public :
    explicit ComputeGLInfoPriv( );
    ComputeGLInfoPriv( const ComputeGLInfoPriv & );
    virtual ~ComputeGLInfoPriv( ) OGLCALC_OVERRIDE;
    auto  load( GLFuncBroker * ) -> void;
    inline auto  wkGrpCntPtr()  -> GLFuncBroker::GLint* { return &m_wkgrp_cnt[0]; }
    inline auto  wkGrpSizePtr() -> GLFuncBroker::GLint* { return &m_wkgrp_sz[0];  }
    inline auto  maxSsboBind()  -> GLFuncBroker::GLint  { return m_max_ssbo_bind_port; }
    inline auto  maxCsSsbNumPerShader()  -> GLFuncBroker::GLint { return m_max_cs_ssb_num_per_shader; }
    inline auto  maxCsSsbSizePerShader() -> GLFuncBroker::GLint { return m_max_cs_ssb_sz_per_shader; }
    inline auto  isShaderCompSupport()   -> bool { return bool( m_is_shader_comp_support > 0 ); }
};

ComputeGLInfoPriv :: ~ComputeGLInfoPriv( ) { }

ComputeGLInfoPriv :: ComputeGLInfoPriv ( )
{
    m_wkgrp_cnt[0] = m_wkgrp_cnt[1] = m_wkgrp_cnt[2] = GL_SPEC_MIN_CS_WKGRP_CNT;
    m_wkgrp_sz [0] = m_wkgrp_sz [1] = GL_SPEC_MIN_CS_WKGRP_SIZE_XY;
    m_wkgrp_sz [2] = GL_SPEC_MIN_CS_WKGRP_SIZE_Z;
    m_max_invoca_per_wkgrp = GL_SPEC_MIN_LIMIT_CS_INVOC_PER_GRP;
    m_max_ssbo_bind_port   = 8;       // ....maybe not corrected
    m_max_cs_ssb_num_per_shader = 8;  // ....maybe not corrected
    m_max_cs_ssb_sz_per_shader  = 1024;
    m_is_shader_comp_support = true;
}

ComputeGLInfoPriv :: ComputeGLInfoPriv ( const ComputeGLInfoPriv &ot )
{
    m_wkgrp_cnt[0] = ot.m_wkgrp_cnt[0]; m_wkgrp_cnt[1] = ot.m_wkgrp_cnt[1]; m_wkgrp_cnt[2] = ot.m_wkgrp_cnt[2];
    m_wkgrp_sz [0] = ot.m_wkgrp_sz [0]; m_wkgrp_sz [1] = ot.m_wkgrp_sz [1]; m_wkgrp_sz [2] = ot.m_wkgrp_sz [2];
    m_max_invoca_per_wkgrp      = ot.m_max_invoca_per_wkgrp;
    m_max_ssbo_bind_port        = ot.m_max_ssbo_bind_port;
    m_max_cs_ssb_num_per_shader = ot.m_max_cs_ssb_num_per_shader;
    m_max_cs_ssb_sz_per_shader  = ot.m_max_cs_ssb_sz_per_shader;
}

auto     ComputeGLInfoPriv :: load( GLFuncBroker *gl_func ) -> void
{
    gl_func->glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, & m_wkgrp_cnt[0] );
    gl_func->glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, & m_wkgrp_cnt[1] );
    gl_func->glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, & m_wkgrp_cnt[2] );
    gl_func->glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_SIZE,  0, & m_wkgrp_sz[0] );
    gl_func->glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_SIZE,  1, & m_wkgrp_sz[1] );
    gl_func->glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_SIZE,  2, & m_wkgrp_sz[2] );
    gl_func->glGetIntegerv  ( GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, & m_max_invoca_per_wkgrp );
    gl_func->glGetIntegerv  ( GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, & m_max_ssbo_bind_port   );
    gl_func->glGetIntegerv  ( GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS,  & m_max_cs_ssb_num_per_shader );
    gl_func->glGetIntegerv  ( GL_MAX_SHADER_STORAGE_BLOCK_SIZE,      & m_max_cs_ssb_sz_per_shader );
    gl_func->glGetBooleanv  ( GL_SHADER_COMPILER, & m_is_shader_comp_support );
}

ComputeGLInfo ::  ComputeGLInfo ( ) { m_obj = nullptr; }
ComputeGLInfo :: ~ComputeGLInfo ( ) { makeNull(); }
ComputeGLInfo ::  ComputeGLInfo ( const ComputeGLInfo &ot ) { m_obj = nullptr; ComputeGLInfoPriv::attach(&m_obj, const_cast<void**>( &ot.m_obj)); }
auto     ComputeGLInfo :: operator = ( const ComputeGLInfo &ot ) -> ComputeGLInfo & { ComputeGLInfoPriv::attach(&m_obj, const_cast<void**>( &ot.m_obj)); return *this; }
auto     ComputeGLInfo :: isNull() const -> bool { return m_obj == nullptr; }
auto     ComputeGLInfo :: isShaderCompilerSupport() const -> bool { return T_CSGL(m_obj)->isShaderCompSupport(); }
auto     ComputeGLInfo :: makeNull() -> void     { if ( m_obj != nullptr ) { ComputeGLInfoPriv::attach(&m_obj, nullptr ); m_obj = nullptr; } }
auto     ComputeGLInfo :: maxWorkGroupCount( Dim d ) const -> int {  return int( T_CSGL(m_obj)->wkGrpCntPtr()[d] ); }
auto     ComputeGLInfo :: maxWorkGroupSize ( Dim d ) const -> int {  return int( T_CSGL(m_obj)->wkGrpSizePtr()[d] ); }
auto     ComputeGLInfo :: maxSsboBindings()    const -> int { return T_CSGL(m_obj)->maxSsboBind(); }
auto     ComputeGLInfo :: maxSsbNumPerShader() const -> int { return T_CSGL(m_obj)->maxCsSsbNumPerShader(); }
auto     ComputeGLInfo :: maxSsbSizeInShader() const -> int { return T_CSGL(m_obj)->maxCsSsbSizePerShader();};

// static, to create a GL information for compute
auto     ComputeGLInfo :: query() -> ComputeGLInfo
{
    ComputeGLInfo  gli;
    auto gl_func = glFunc();

    if ( gl_func != nullptr ) {
        ComputeGLInfoPriv::buildIfNull( & gli.m_obj );
        auto gl_info = T_CSGL(gli.m_obj);
        gl_info->load( gl_func );
    }

    return gli;
}




// ////////////////////////////////////////////////////////////////////////////
// [ GPU program ]
// ////////////////////////////////////////////////////////////////////////////
#define MAX_PROGRAM_BUFF_BIND  32
#define MAX_PROGRAM_UNIFORM_BUFF_BIND 16
#define T_Prog( o )  oglcalc_objcast( ProgramPriv*, o )
class OGLCALC_API ProgramPriv {
public :
    struct BuffDscr {
        Buffer    buffer; intptr_t  bind_point;
        BuffDscr() : buffer(), bind_point(0) { }
        BuffDscr( const Buffer &bf, intptr_t bp ) : buffer( bf ), bind_point( bp ) { }
    };
private:
    GLFuncBroker *m_gl_func; Status m_st_code; Text m_info_log;
    GLFuncBroker::GLuint  m_gl_prog;
    BuffDscr m_buff_dscr[ MAX_PROGRAM_BUFF_BIND ];

protected:
    auto  ctor() -> void;
    auto  dtor() -> void;
public :
    explicit ProgramPriv( );
    ~ProgramPriv( ) { dtor(); }
    auto  init( const Text & ) -> void;
    auto  deinit() -> void;
    inline auto  glFuncRef()   -> GLFuncBroker*& { return m_gl_func; }
    inline auto  statusRef()   -> Status&        { return m_st_code;  }
    inline auto  glProgRef()   -> GLFuncBroker::GLuint& { return m_gl_prog; }
    inline auto  infoLogRef()  -> Text&       { return m_info_log; }
    inline auto  buffDscrRef() -> BuffDscr*   { return m_buff_dscr; }
    auto  bindBuffPoint()      -> void;
    auto  unbindBufferPoint()  -> void;
};

ProgramPriv :: ProgramPriv ( )
{
    m_gl_func   = nullptr; m_st_code = Status_Unexcept; m_gl_prog = 0;
    ctor();
}

auto     ProgramPriv :: ctor ( ) -> void
{
    m_gl_func = glFunc();

    if ( m_gl_func != nullptr ) {
        // check if support shader compiler
        GLFuncBroker::GLboolean is_sc_support = false;
        m_gl_func->glGetBooleanv( GL_SHADER_COMPILER, & is_sc_support );
        if ( is_sc_support ) {
            m_st_code = Status_OK;
        } else {
            m_st_code = Status_No_ShaderCompiler;
        }

        // query the binding port number
        GLFuncBroker::GLint bind_num = 0;
        m_gl_func->glGetIntegerv ( GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, & bind_num );
        if ( bind_num < 1 ) {
            m_st_code = Status_SSB_Bindings_Too_Small;
        }
    } else {
        m_st_code   = Status_No_GLFuncBroker;
    }
}

auto     ProgramPriv :: dtor() -> void
{ }

auto     ProgramPriv :: init( const Text &cs ) -> void
{
    if ( m_gl_func == nullptr ) { return; }

    auto sd = gCompShader( m_gl_func, cs, m_info_log );
    if ( sd == 0 ) { m_st_code = Status_Failed_To_Compile_Shader; return; }
    m_gl_prog = gLinkProg( m_gl_func, sd, m_info_log );
    m_gl_func->glDeleteShader( sd );

    if ( m_gl_prog == 0 ) { m_st_code = Status_Failed_To_Link_Program; }
}

auto     ProgramPriv :: deinit() -> void
{
    if ( m_gl_func == nullptr || m_gl_prog == 0 ) { return; }
    m_gl_func->glUseProgram(0);
    m_gl_func->glDeleteProgram( m_gl_prog );
}

auto     ProgramPriv :: bindBuffPoint() -> void
{
    for ( size_t i = 0; i < MAX_PROGRAM_BUFF_BIND; i ++ ) {
        auto buf_id     = m_buff_dscr[i].buffer.glId();
        auto bind_point = m_buff_dscr[i].bind_point;
        if ( bind_point > 0  &&  buf_id > 0 ) {
            m_gl_func->glBindBufferBase( GL_SHADER_STORAGE_BUFFER, GLFuncBroker::GLuint( bind_point ), GLFuncBroker::GLuint( buf_id ));
        }
    }
}

auto     ProgramPriv :: unbindBufferPoint() -> void
{
    for ( size_t i = 0; i < MAX_PROGRAM_BUFF_BIND; i ++ ) {
        auto bind_point = m_buff_dscr[i].bind_point;
        if ( bind_point > 0 ) {
            m_gl_func->glBindBufferBase( GL_SHADER_STORAGE_BUFFER, GLFuncBroker::GLuint( bind_point ), 0 );
        }
    }
}


// ////////////////////////////////////////////////////////////////////////////
// program wrap API
// ////////////////////////////////////////////////////////////////////////////
Program :: Program ( const Text &cs,  bool req_macro_hdr )
{
    m_obj = oglcalc_new( ProgramPriv );

    auto priv = T_Prog(m_obj);
    if ( priv->statusRef() == Status_OK ) {
        if ( req_macro_hdr ) {
            auto cs2 = Text::format( "%s%s", priv->glFuncRef()->shaderVersionStr(), cs.constData() );
            T_Prog( m_obj )->init( cs2 );
        } else {
            T_Prog( m_obj )->init( cs  );
        }
    }
}

Program :: ~Program( )
{
    T_Prog( m_obj )->deinit();
    oglcalc_delete( m_obj, ProgramPriv );
}

auto     Program :: status()  const -> Status { return T_Prog(m_obj)->statusRef(); }
auto     Program :: infoLog() const -> Text   { return T_Prog(m_obj)->infoLogRef(); }

auto     Program :: setBuffer( const Buffer &bf, intptr_t bp ) -> bool
{
    auto priv = T_Prog(m_obj);
    if ( bp > 0 && bp < MAX_PROGRAM_BUFF_BIND ) {
        auto bd = & priv->buffDscrRef()[ bp ];
        bd->buffer     = bf;
        bd->bind_point = bp;
        return true;
    } else {
        return false;
    }
}

auto     Program :: buffer( intptr_t bp ) const -> Buffer
{
    auto priv = T_Prog(m_obj);
    if ( bp > 0 && bp < MAX_PROGRAM_BUFF_BIND ) {
        return priv->buffDscrRef()[ bp ].buffer;
    } else {
        return Buffer();
    }
}

auto     Program :: rmvBuffer( intptr_t bp ) -> void
{
    auto priv = T_Prog(m_obj);
    if ( bp > 0 && bp < MAX_PROGRAM_BUFF_BIND ) {
        auto bd = & priv->buffDscrRef()[ bp ];
        bd->buffer = Buffer();
        bd->bind_point = 0;
    } else {

    }
}

auto     Program :: clearAllBuffers() -> void
{
    auto priv = T_Prog(m_obj);
    for ( size_t i = 1; i < MAX_PROGRAM_BUFF_BIND; i ++ ) {
        auto bd = & priv->buffDscrRef()[i];
        bd->buffer     = Buffer();
        bd->bind_point = 0;
    }
}

auto     Program :: use() -> bool
{
    auto priv = T_Prog(m_obj);
    if ( priv->statusRef() != Status_OK ) { return false; }
    auto gf = priv->glFuncRef();
    gf->glUseProgram( priv->glProgRef() );

    GLFuncBroker::GLint c_prog = 0;
    gf->glGetIntegerv( GL_CURRENT_PROGRAM, &c_prog );
    return ( GLFuncBroker::GLuint( c_prog ) != ( priv->glProgRef()) );
}

auto     Program :: run( size_t x, size_t y, size_t z ) -> bool
{
    auto priv = T_Prog(m_obj);
    if ( priv->statusRef() != Status_OK ) { return false; }
    auto gf = priv->glFuncRef();

    GLFuncBroker::GLint c_prog = 0;
    gf->glGetIntegerv( GL_CURRENT_PROGRAM, &c_prog );
    if ( c_prog != GLFuncBroker::GLint( priv->glProgRef()) ) { gf->glUseProgram(priv->glProgRef()); }

    priv->bindBuffPoint();
    gf->glDispatchCompute( GLFuncBroker::GLuint( x ), GLFuncBroker::GLuint( y ), GLFuncBroker::GLuint( z ));
    gf->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
    priv->unbindBufferPoint();

    gf->glUseProgram( GLFuncBroker::GLuint( c_prog ) );
    return true;
}


auto     Program :: setUniform( const char *name , size_t cnt, const float *vp ) -> bool
{
    auto priv = T_Prog(m_obj);
    if ( vp == nullptr || priv->statusRef() != Status_OK ) { return false; }
    if ( cnt < 1 ) { return true; }
    auto gf = priv->glFuncRef();
    auto loc = gf->glGetUniformLocation( priv->glProgRef(), name );
    if ( loc == -1 ) { return false; }
    gf->glUniform1fv( loc, GLFuncBroker::GLsizei( cnt ), vp );
    return true;
}

auto     Program :: setUniform( const char *name, size_t cnt, const int   *vp ) -> bool
{
    auto priv = T_Prog(m_obj);
    if ( vp == nullptr || priv->statusRef() != Status_OK ) { return false; }
    if ( cnt < 1 ) { return true; }
    auto gf = priv->glFuncRef();
    auto loc = gf->glGetUniformLocation( priv->glProgRef(), name );
    if ( loc == -1 ) { return false; }
    gf->glUniform1iv( loc, GLFuncBroker::GLsizei( cnt ), vp );
    return true;
}

auto     Program :: setUniform( const char *name, size_t cnt, const unsigned int *vp ) -> bool
{
    auto priv = T_Prog(m_obj);
    if ( vp == nullptr || priv->statusRef() != Status_OK ) { return false; }
    if ( cnt < 1 ) { return true; }
    auto gf = priv->glFuncRef();
    auto loc = gf->glGetUniformLocation( priv->glProgRef(), name );
    if ( loc == -1 ) { return false; }
    gf->glUniform1uiv( loc, GLFuncBroker::GLsizei( cnt ), vp );
    return true;
}

auto     Program :: setUniform( const char *name, size_t cnt, const Uniform_vec2 *vp ) -> bool
{
    auto priv = T_Prog(m_obj);
    if ( vp == nullptr || priv->statusRef() != Status_OK ) { return false; }
    if ( cnt < 1 ) { return true; }
    auto gf = priv->glFuncRef();
    auto loc = gf->glGetUniformLocation( priv->glProgRef(), name );
    if ( loc == -1 ) { return false; }
    gf->glUniform2fv( loc, GLFuncBroker::GLsizei( cnt ), reinterpret_cast<const GLFuncBroker::GLfloat*>( vp ));
    return true;
}

auto     Program :: setUniform( const char *name, size_t cnt, const Uniform_vec3 *vp ) -> bool
{
    auto priv = T_Prog(m_obj);
    if ( vp == nullptr || priv->statusRef() != Status_OK ) { return false; }
    if ( cnt < 1 ) { return true; }
    auto gf = priv->glFuncRef();
    auto loc = gf->glGetUniformLocation( priv->glProgRef(), name );
    if ( loc == -1 ) { return false; }
    gf->glUniform3fv( loc, GLFuncBroker::GLsizei( cnt ),  reinterpret_cast<const GLFuncBroker::GLfloat*>( vp ));
    return true;
}

auto     Program :: setUniform( const char *name, size_t cnt, const Uniform_vec4 *vp ) -> bool
{
    auto priv = T_Prog(m_obj);
    if ( vp == nullptr || priv->statusRef() != Status_OK ) { return false; }
    if ( cnt < 1 ) { return true; }
    auto gf = priv->glFuncRef();
    auto loc = gf->glGetUniformLocation( priv->glProgRef(), name );
    if ( loc == -1 ) { return false; }
    gf->glUniform4fv( loc, GLFuncBroker::GLsizei( cnt ),  reinterpret_cast<const GLFuncBroker::GLfloat*>( vp ));
    return true;
}

auto     Program :: setUniform( const char *name, size_t cnt, const Uniform_ivec2 *vp ) -> bool
{
    auto priv = T_Prog(m_obj);
    if ( vp == nullptr || priv->statusRef() != Status_OK ) { return false; }
    if ( cnt < 1 ) { return true; }
    auto gf = priv->glFuncRef();
    auto loc = gf->glGetUniformLocation( priv->glProgRef(), name );
    if ( loc == -1 ) { return false; }
    gf->glUniform2iv( loc, GLFuncBroker::GLsizei( cnt ), reinterpret_cast<const GLFuncBroker::GLint*>( vp ));
    return true;
}

auto     Program :: setUniform( const char *name, size_t cnt, const Uniform_ivec3 *vp ) -> bool
{
    auto priv = T_Prog(m_obj);
    if ( vp == nullptr || priv->statusRef() != Status_OK ) { return false; }
    if ( cnt < 1 ) { return true; }
    auto gf = priv->glFuncRef();
    auto loc = gf->glGetUniformLocation( priv->glProgRef(), name );
    if ( loc == -1 ) { return false; }
    gf->glUniform3iv( loc, GLFuncBroker::GLsizei( cnt ),  reinterpret_cast<const GLFuncBroker::GLint*>( vp ));
    return true;
}

auto     Program :: setUniform( const char *name, size_t cnt, const Uniform_ivec4 *vp ) -> bool
{
    auto priv = T_Prog(m_obj);
    if ( vp == nullptr || priv->statusRef() != Status_OK ) { return false; }
    if ( cnt < 1 ) { return true; }
    auto gf = priv->glFuncRef();
    auto loc = gf->glGetUniformLocation( priv->glProgRef(), name );
    if ( loc == -1 ) { return false; }
    gf->glUniform4iv( loc, GLFuncBroker::GLsizei( cnt ),  reinterpret_cast<const GLFuncBroker::GLint*>( vp ));
    return true;
}

auto     Program :: setUniform( const char *name, size_t cnt, const Uniform_uvec2 *vp ) -> bool
{
    auto priv = T_Prog(m_obj);
    if ( vp == nullptr || priv->statusRef() != Status_OK ) { return false; }
    if ( cnt < 1 ) { return true; }
    auto gf = priv->glFuncRef();
    auto loc = gf->glGetUniformLocation( priv->glProgRef(), name );
    if ( loc == -1 ) { return false; }
    gf->glUniform2uiv( loc, GLFuncBroker::GLsizei( cnt ), reinterpret_cast<const GLFuncBroker::GLuint*>( vp ));
    return true;
}

auto     Program :: setUniform( const char *name, size_t cnt, const Uniform_uvec3 *vp ) -> bool
{
    auto priv = T_Prog(m_obj);
    if ( vp == nullptr || priv->statusRef() != Status_OK ) { return false; }
    if ( cnt < 1 ) { return true; }
    auto gf = priv->glFuncRef();
    auto loc = gf->glGetUniformLocation( priv->glProgRef(), name );
    if ( loc == -1 ) { return false; }
    gf->glUniform3uiv( loc, GLFuncBroker::GLsizei( cnt ), reinterpret_cast<const GLFuncBroker::GLuint*>( vp ));
    return true;
}

auto     Program :: setUniform( const char *name, size_t cnt, const Uniform_uvec4 *vp ) -> bool
{
    auto priv = T_Prog(m_obj);
    if ( vp == nullptr || priv->statusRef() != Status_OK ) { return false; }
    if ( cnt < 1 ) { return true; }
    auto gf = priv->glFuncRef();
    auto loc = gf->glGetUniformLocation( priv->glProgRef(), name );
    if ( loc == -1 ) { return false; }
    gf->glUniform4uiv( loc, GLFuncBroker::GLsizei( cnt ), reinterpret_cast<const GLFuncBroker::GLuint*>( vp ));
    return true;
}

auto     Program :: setUniform( const char *name, size_t cnt, bool req_trans, const Uniform_mat2x2 *vp ) -> bool
{
    auto priv = T_Prog(m_obj);
    if ( vp == nullptr || priv->statusRef() != Status_OK ) { return false; }
    if ( cnt < 1 ) { return true; }
    auto gf = priv->glFuncRef();
    auto loc = gf->glGetUniformLocation( priv->glProgRef(), name );
    if ( loc == -1 ) { return false; }
    gf->glUniformMatrix2fv( loc, GLFuncBroker::GLsizei( cnt ), req_trans, reinterpret_cast<const GLFuncBroker::GLfloat*>( vp ));
    return true;
}

auto     Program :: setUniform( const char *name, size_t cnt, bool req_trans, const Uniform_mat3x3 *vp ) -> bool
{
    auto priv = T_Prog(m_obj);
    if ( vp == nullptr || priv->statusRef() != Status_OK ) { return false; }
    if ( cnt < 1 ) { return true; }
    auto gf = priv->glFuncRef();
    auto loc = gf->glGetUniformLocation( priv->glProgRef(), name );
    if ( loc == -1 ) { return false; }
    gf->glUniformMatrix3fv( loc, GLFuncBroker::GLsizei( cnt ), req_trans, reinterpret_cast<const GLFuncBroker::GLfloat*>( vp ));
    return true;
}

auto     Program :: setUniform( const char *name, size_t cnt, bool req_trans, const Uniform_mat4x4 *vp ) -> bool
{
    auto priv = T_Prog(m_obj);
    if ( vp == nullptr || priv->statusRef() != Status_OK ) { return false; }
    if ( cnt < 1 ) { return true; }
    auto gf = priv->glFuncRef();
    auto loc = gf->glGetUniformLocation( priv->glProgRef(), name );
    if ( loc == -1 ) { return false; }
    gf->glUniformMatrix4fv( loc, GLFuncBroker::GLsizei( cnt ), req_trans, reinterpret_cast<const GLFuncBroker::GLfloat*>( vp ));
    return true;
}

auto     Program :: setUniform( const char *name, size_t cnt, bool req_trans, const Uniform_mat2x3 *vp ) -> bool
{
    auto priv = T_Prog(m_obj);
    if ( vp == nullptr || priv->statusRef() != Status_OK ) { return false; }
    if ( cnt < 1 ) { return true; }
    auto gf = priv->glFuncRef();
    auto loc = gf->glGetUniformLocation( priv->glProgRef(), name );
    if ( loc == -1 ) { return false; }
    gf->glUniformMatrix2x3fv( loc, GLFuncBroker::GLsizei( cnt ), req_trans, reinterpret_cast<const GLFuncBroker::GLfloat*>( vp ));
    return true;
}

auto     Program :: setUniform( const char *name, size_t cnt, bool req_trans, const Uniform_mat3x2 *vp ) -> bool
{
    auto priv = T_Prog(m_obj);
    if ( vp == nullptr || priv->statusRef() != Status_OK ) { return false; }
    if ( cnt < 1 ) { return true; }
    auto gf = priv->glFuncRef();
    auto loc = gf->glGetUniformLocation( priv->glProgRef(), name );
    if ( loc == -1 ) { return false; }
    gf->glUniformMatrix3x2fv( loc, GLFuncBroker::GLsizei( cnt ), req_trans, reinterpret_cast<const GLFuncBroker::GLfloat*>( vp ));
    return true;
}

auto     Program :: setUniform( const char *name, size_t cnt, bool req_trans, const Uniform_mat2x4 *vp ) -> bool
{
    auto priv = T_Prog(m_obj);
    if ( vp == nullptr || priv->statusRef() != Status_OK ) { return false; }
    if ( cnt < 1 ) { return true; }
    auto gf = priv->glFuncRef();
    auto loc = gf->glGetUniformLocation( priv->glProgRef(), name );
    if ( loc == -1 ) { return false; }
    gf->glUniformMatrix2x4fv( loc, GLFuncBroker::GLsizei( cnt ), req_trans, reinterpret_cast<const GLFuncBroker::GLfloat*>( vp ));
    return true;
}

auto     Program :: setUniform( const char *name, size_t cnt, bool req_trans, const Uniform_mat4x2 *vp ) -> bool
{
    auto priv = T_Prog(m_obj);
    if ( vp == nullptr || priv->statusRef() != Status_OK ) { return false; }
    if ( cnt < 1 ) { return true; }
    auto gf = priv->glFuncRef();
    auto loc = gf->glGetUniformLocation( priv->glProgRef(), name );
    if ( loc == -1 ) { return false; }
    gf->glUniformMatrix4x2fv( loc, GLFuncBroker::GLsizei( cnt ), req_trans, reinterpret_cast<const GLFuncBroker::GLfloat*>( vp ));
    return true;
}

auto     Program :: setUniform( const char *name, size_t cnt, bool req_trans, const Uniform_mat3x4 *vp ) -> bool
{
    auto priv = T_Prog(m_obj);
    if ( vp == nullptr || priv->statusRef() != Status_OK ) { return false; }
    if ( cnt < 1 ) { return true; }
    auto gf = priv->glFuncRef();
    auto loc = gf->glGetUniformLocation( priv->glProgRef(), name );
    if ( loc == -1 ) { return false; }
    gf->glUniformMatrix3x4fv( loc, GLFuncBroker::GLsizei( cnt ), req_trans, reinterpret_cast<const GLFuncBroker::GLfloat*>( vp ));
    return true;
}

auto     Program :: setUniform( const char *name, size_t cnt, bool req_trans, const Uniform_mat4x3 *vp ) -> bool
{
    auto priv = T_Prog(m_obj);
    if ( vp == nullptr || priv->statusRef() != Status_OK ) { return false; }
    if ( cnt < 1 ) { return true; }
    auto gf = priv->glFuncRef();
    auto loc = gf->glGetUniformLocation( priv->glProgRef(), name );
    if ( loc == -1 ) { return false; }
    gf->glUniformMatrix4x3fv( loc, GLFuncBroker::GLsizei( cnt ), req_trans, reinterpret_cast<const GLFuncBroker::GLfloat*>( vp ));
    return true;
}



}
#endif
