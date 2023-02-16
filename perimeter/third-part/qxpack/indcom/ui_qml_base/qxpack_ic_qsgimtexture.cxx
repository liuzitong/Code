#ifndef QXPACK_IC_QSGIMTEXTURE_CXX
#define QXPACK_IC_QSGIMTEXTURE_CXX

// ////////////////////////////////////////////////////////////////////////////
// defines
// ////////////////////////////////////////////////////////////////////////////
#include <qxpack/indcom/common/qxpack_ic_memcntr.hxx>
#include "qxpack_ic_qsgimtexture.hxx"

#define  QXPACK_IcPImplPrivTemp_new    qxpack_ic_new
#define  QXPACK_IcPImplPrivTemp_delete qxpack_ic_delete
#include "qxpack/indcom/common/qxpack_ic_pimplprivtemp.hpp"

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QSurfaceFormat>

namespace QxPack {

#ifndef GL_RED
#define GL_RED GL_RED_EXT
#endif
#ifndef GL_TEXTURE_SWIZZLE_RGBA
#define GL_TEXTURE_SWIZZLE_RGBA  0x8E46
#endif
#ifndef GL_TEXTURE_SWIZZLE_R
#define GL_TEXTURE_SWIZZLE_R     0x8E42
#endif
#ifndef GL_TEXTURE_SWIZZLE_G
#define GL_TEXTURE_SWIZZLE_G     0x8E43
#endif
#ifndef GL_TEXTURE_SWIZZLE_B
#define GL_TEXTURE_SWIZZLE_B     0x8E44
#endif
#ifndef GL_TEXTURE_SWIZZLE_A
#define GL_TEXTURE_SWIZZLE_A     0x8E45
#endif

// ////////////////////////////////////////////////////////////////////////////
//
//         Functions
//
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// opengl texture format
// here we only support GL_RED, GL_RGB565, GL_RGBA
// every index is the QImage index.
// ============================================================================
static const GLenum gltex_fmt[ ] = {
    // invalid, mono, monolsb, index8,  rgb32,  argb32,  argb32_prem, rgb16
    GL_NONE, GL_NONE, GL_NONE, GL_RED,   GL_NONE, GL_NONE, GL_NONE, GL_RGB565,
    
    // arb8565_prem,  rgb666, argb666_prem,  rgb555,  argb8555_prem, rgb888, rgb444,  argb444_prem 
    GL_NONE, GL_NONE, GL_NONE, GL_NONE,   GL_NONE, GL_RGB, GL_NONE, GL_NONE,

    // rgbx8888,  rgba8888, rgba8888_prem, bgr30, a2bgr30_prem, rgb30, a2rgb30_prem
    GL_RGBA, GL_RGBA, GL_NONE, GL_NONE,   GL_NONE, GL_NONE, GL_NONE, GL_NONE,

    // alpha8, grayscale8
    GL_RED,  GL_RED
};

// ============================================================================
// the QImage to opengl format mapper
// ============================================================================
static const GLenum  qim2gl_fmt[ ] = {

    // invalid, mono, monolsb, index8,  rgb32,  argb32,  argb32_prem, rgb16
    GL_NONE, GL_NONE, GL_NONE, GL_RED,   GL_NONE, GL_NONE, GL_NONE, GL_RGB,

    // arb8565_prem,  rgb666, argb666_prem,  rgb555,  argb8555_prem, rgb888, rgb444,  argb444_prem 
    GL_NONE, GL_NONE, GL_NONE, GL_NONE,   GL_NONE, GL_RGB, GL_NONE, GL_NONE,

    // rgbx8888,  rgba8888, rgba8888_prem, bgr30, a2bgr30_prem, rgb30, a2rgb30_prem
    GL_RGBA, GL_RGBA, GL_NONE, GL_NONE,   GL_NONE, GL_NONE, GL_NONE, GL_NONE,

    // alpha8, grayscale8
    GL_RED,  GL_RED
};

// ============================================================================
// the QImage to opengl format byte format
// ============================================================================
static const uint     qim2gl_bt[ ] = {
    // invalid, mono, monolsb, index8,  rgb32,  argb32,  argb32_prem, rgb16
    0, 0, 0, GL_UNSIGNED_BYTE,   0, 0, 0, GL_UNSIGNED_SHORT_5_6_5,
	
    // arb8565_prem,  rgb666, argb666_prem,  rgb555,  argb8555_prem, rgb888, rgb444,  argb444_prem 
    0, 0, 0, 0,      0, GL_UNSIGNED_BYTE, 0, 0,
	
    // rgbx8888,  rgba8888, rgba8888_prem, bgr30, a2bgr30_prem, rgb30, a2rgb30_prem
    GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE, 0, 0,  0, 0, 0, 0,

    // alpha8, grayscale8
    GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE
};

// ============================================================================
// the alpha channel mapper
// ============================================================================
static const bool    has_alpha_ch[] = {
	
    // invalid, mono, monolsb, index8,  rgb32,  argb32,  argb32_prem, rgb16
    false, false, false, false,    false, true, true, false,
	
    // arb8565_prem,  rgb666, argb666_prem,  rgb555,  argb8555_prem, rgb888, rgb444,  argb444_prem 
    true,  false, true,  false,    true,  false, false, true,

    // rgbx8888,  rgba8888, rgba8888_prem, bgr30, a2bgr30_prem, rgb30, a2rgb30_prem
    true, true, true, false,       false, false, false, false,

    // alpha8, grayscale8
    true, false
};

// ============================================================================
// the bytes of channel
// ============================================================================
static const int   fmt_bytes[ ] = {
    // invalid, mono, monolsb, index8,  rgb32,  argb32,  argb32_prem, rgb16
    1, 1, 1, 1,   1, 1, 1, 2,

    // arb8565_prem,  rgb666, argb666_prem,  rgb555,  argb8555_prem, rgb888, rgb444,  argb444_prem
    2, 3, 3, 2,   3, 3, 2, 2,

    // rgbx8888,  rgba8888, rgba8888_prem, bgr30, a2bgr30_prem, rgb30, a2rgb30_prem
    4, 4, 4, 4,   4, 4, 4, 4,

    // alpha8, grayscale8
    1, 1, 1, 1,   1, 1, 1, 1
};


// ////////////////////////////////////////////////////////////////////////////
// private object
// ////////////////////////////////////////////////////////////////////////////
#define T_PrivPtr( o ) T_ObjCast(IcQSGImTexturePriv*, o )
class QXPACK_IC_HIDDEN IcQSGImTexturePriv : public IcPImplPrivTemp< IcQSGImTexturePriv > {
private:
    uint  m_tex_id;
    int   m_tex_width, m_tex_height;
    uint  m_tex_fmt;
    bool  m_has_alpha, m_is_red_and_es;
    int   m_gl_major_ver;
    QOpenGLFunctions *m_func;
protected:
    static uint  texFmtFromPxFmt   ( int px_fmt ){ return ( size_t( px_fmt ) < sizeof( gltex_fmt )   / sizeof( gltex_fmt[0]    ) ? gltex_fmt[ px_fmt ]    : GL_NONE ); }
    static bool  hasAlphaFromPxFmt ( int px_fmt ){ return ( size_t( px_fmt ) < sizeof( has_alpha_ch )/ sizeof( has_alpha_ch[0] ) ? has_alpha_ch[ px_fmt ] : false  ); }
    static int   fmtBytesFromPxFmt ( int px_fmt ){ return ( size_t( px_fmt ) < sizeof( fmt_bytes )   / sizeof( fmt_bytes[0]    ) ? fmt_bytes[ px_fmt ]    : 1 ); }
    static uint  qim2glFmtFromPxFmt( int px_fmt ){ return ( size_t( px_fmt ) < sizeof( qim2gl_fmt )  / sizeof( qim2gl_fmt[0]   ) ? qim2gl_fmt[ px_fmt ]   : GL_RGBA ); }
    static uint  qim2glBtFromPxFmt ( int px_fmt ){ return ( size_t( px_fmt ) < sizeof( qim2gl_bt  )  / sizeof( qim2gl_bt[0]    ) ? qim2gl_bt[ px_fmt ]    : GL_UNSIGNED_BYTE ); }
public :
    IcQSGImTexturePriv(  );
    IcQSGImTexturePriv( const IcQSGImTexturePriv & )
    { qFatal("IcQSGImTexturePriv did not support CTOR(COPY)"); }  // never ocurred.
    virtual ~IcQSGImTexturePriv( );
    bool   initalize( int w, int h, QImage::Format fmt, bool gen_mip = false );
    
    inline int   textureId( ) const { return int( m_tex_id ); }
    inline int   texWidth( )  const { return m_tex_width;  }
    inline int   texHeight( ) const { return m_tex_height; }
    inline uint  texFormat( ) const { return m_tex_fmt;    }
    inline bool  hasAlpha( )  const { return m_has_alpha;  }
    inline QSize texSize( )   const { return QSize( m_tex_width, m_tex_height ); }
    inline bool  isGLRedAndOpenGLES( ) const { return m_is_red_and_es; }
    inline int   glMajorVersion( ) const     { return m_gl_major_ver;  }
    void   bind( ) { m_func->glBindTexture( GL_TEXTURE_2D, m_tex_id ); }
    bool   updateTexture( const void *bits, int width, int height, QImage::Format fmt );
};

// ================================================================
// ctor
// ================================================================
        IcQSGImTexturePriv :: IcQSGImTexturePriv (  )
{
    m_tex_id = 0;  m_tex_width = m_tex_height = 1;
    m_tex_fmt   = GL_NONE;
    m_has_alpha = false;
    m_is_red_and_es = false;
    m_gl_major_ver  = QSurfaceFormat::defaultFormat().majorVersion();

    QOpenGLContext *ctxt = QOpenGLContext::currentContext( );
    m_func = ( ctxt != Q_NULLPTR ? ctxt->functions( ) : Q_NULLPTR );
}

// ============================================================================
// dtor
// ============================================================================
        IcQSGImTexturePriv :: ~IcQSGImTexturePriv (  )
{
    if ( m_tex_id != 0 ) {
        if ( m_func != Q_NULLPTR ) { m_func->glDeleteTextures ( 1, & m_tex_id ); }
    }
}

// ============================================================================
// initalize texture
// ============================================================================
bool    IcQSGImTexturePriv :: initalize( int width, int height, QImage::Format px_fmt, bool )
{
    if ( m_func == Q_NULLPTR ) { return false; }
    if ( ( m_tex_fmt = IcQSGImTexturePriv::texFmtFromPxFmt( px_fmt )) == GL_NONE ) { return false; }

    m_has_alpha = IcQSGImTexturePriv::hasAlphaFromPxFmt( px_fmt );
    m_tex_width = ( width  > 0 ? width  : 64 );
    m_tex_height= ( height > 0 ? height : 64 );

    m_func->glGenTextures  ( 1, & m_tex_id );
    m_func->glBindTexture  ( GL_TEXTURE_2D, m_tex_id );
    m_func->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST   );
    m_func->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR    );
    m_func->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    m_func->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    if ( m_tex_fmt == GL_RED ) {
        if ( ! QOpenGLContext::currentContext()->isOpenGLES()) {
            GLint swizzleMask[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
            m_func->glTexParameteriv( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
        } else {
            m_is_red_and_es = true;
            m_func->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED  );
            m_func->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED  );
            m_func->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED  );
            m_func->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ONE  );
        }
    }
    m_func->glTexImage2D ( GL_TEXTURE_2D, 0,GLint( m_tex_fmt ), m_tex_width, m_tex_height, 0, IcQSGImTexturePriv::qim2glFmtFromPxFmt( px_fmt ),
                           GL_UNSIGNED_BYTE, nullptr );
    m_func->glBindTexture( GL_TEXTURE_2D, 0 ); 

    return true;
}

// ============================================================================
//  update data to texture
// ============================================================================
bool    IcQSGImTexturePriv :: updateTexture( const void *bits, int width, int height, QImage::Format fmt )
{
    // check paramters
    if ( bits == Q_NULLPTR    ||  IcQSGImTexturePriv::qim2glFmtFromPxFmt( fmt ) == GL_NONE  ||
         width != m_tex_width ||  height != m_tex_height )
    {
        return false;
    }

    // update
    m_func->glBindTexture   ( GL_TEXTURE_2D, m_tex_id );
    m_func->glTexSubImage2D ( GL_TEXTURE_2D, 0, 0, 0, m_tex_width, m_tex_height,
                              IcQSGImTexturePriv::qim2glFmtFromPxFmt( fmt ),
                              IcQSGImTexturePriv::qim2glBtFromPxFmt( fmt ),
                              bits );
    return true;
}

// ////////////////////////////////////////////////////////////////////////////
//  wrap API
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcQSGImTexture :: IcQSGImTexture( )
{
    m_obj = nullptr;
}

// ============================================================================
// ctor
// ============================================================================
IcQSGImTexture :: IcQSGImTexture ( int  width, int height, QImage::Format px_fmt )
{
    m_obj = nullptr;
    IcQSGImTexturePriv::buildIfNull( & m_obj );
    if ( ! T_PrivPtr( m_obj )->initalize( width, height, px_fmt ) ) {
        IcQSGImTexturePriv::attach( & m_obj, nullptr );
        m_obj = nullptr;
    }
}

// ============================================================================
// dtor
// ============================================================================
IcQSGImTexture :: ~IcQSGImTexture (  )
{
    if ( m_obj != Q_NULLPTR ) {
        IcQSGImTexturePriv::attach( & m_obj, nullptr );
        m_obj = nullptr;
    }
}

// ============================================================================
// ctor copy
// ============================================================================
IcQSGImTexture :: IcQSGImTexture ( const IcQSGImTexture &other ) : QSGTexture()
{
    m_obj = nullptr;
    IcQSGImTexturePriv::attach( & m_obj, const_cast<void**>( & other.m_obj ));
}

// ============================================================================
// operator = override
// ============================================================================
IcQSGImTexture &  IcQSGImTexture :: operator = ( const IcQSGImTexture &other )
{
    IcQSGImTexturePriv::attach( & m_obj, const_cast< void **>( & other.m_obj));
    return *this;
}

// ============================================================================
// check if this object is null or not
// ============================================================================
bool    IcQSGImTexture :: isNull( ) const { return ( m_obj == nullptr ); }

// ============================================================================
// override functiosn, see QSGTexture
// ============================================================================
void   IcQSGImTexture :: bind ( ) { if ( m_obj != nullptr ) { T_PrivPtr( m_obj )->bind(); } }
bool   IcQSGImTexture :: hasAlphaChannel ( ) const { return ( m_obj != nullptr ? T_PrivPtr( m_obj )->hasAlpha()  : false ); }
bool   IcQSGImTexture :: hasMipmaps ( )      const { return false; }
int    IcQSGImTexture :: textureId ( )       const { return ( m_obj != nullptr ? T_PrivPtr( m_obj )->textureId() : 0 ); }
QSize  IcQSGImTexture :: textureSize ( )     const { return ( m_obj != nullptr ? T_PrivPtr( m_obj )->texSize()   : QSize()); }

// ============================================================================
// update texture by data
// ============================================================================
bool   IcQSGImTexture :: updateTexture ( const void *bits, int width, int height, QImage::Format fmt )
{
    return ( m_obj != nullptr ?
        T_PrivPtr( m_obj )->updateTexture( bits, width, height, fmt ) :
        false
    );
}

// ============================================================================
// update texture by image
// ============================================================================
bool   IcQSGImTexture :: updateTexture ( const QImage & im )
{
    if ( im.isNull() || m_obj == nullptr ) { return false; }

    if ( im.size() == T_PrivPtr( m_obj )->texSize() ) {
        return T_PrivPtr( m_obj )->updateTexture( im.constBits(), im.width(), im.height(), im.format());
    } else if ( im.width()  > T_PrivPtr( m_obj )->texWidth() || im.height() > T_PrivPtr( m_obj )->texHeight() ) {
        QImage s_im = im.scaled( T_PrivPtr( m_obj )->texSize(), Qt::KeepAspectRatio, Qt::FastTransformation );
        return T_PrivPtr( m_obj )->updateTexture( s_im.constBits( ), s_im.width( ), s_im.height( ), s_im.format());
    } else {
        return false;
    }
}

// ============================================================================
// [ static ] return the maximum texture size
// ============================================================================
static GLint  g_max_tex_size = 0;
int    IcQSGImTexture :: maxTextureSize()
{
    GLint sz = g_max_tex_size;
    if ( sz == 0 ) {
         QOpenGLContext *ctxt = QOpenGLContext::currentContext( );
         QOpenGLFunctions *func = ( ctxt != Q_NULLPTR ? ctxt->functions( ) : Q_NULLPTR );
         if ( func != Q_NULLPTR ) {   
             func->glGetIntegerv( GL_MAX_TEXTURE_SIZE, & sz );
             g_max_tex_size = sz;
         }
    }
    return sz;
}

}

#endif
