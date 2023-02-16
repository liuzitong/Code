#ifndef QXPACK_IC_QSGIMTEXTURE_CXX
#define QXPACK_IC_QSGIMTEXTURE_CXX

// ////////////////////////////////////////////////////////////////////////////
// defines
// ////////////////////////////////////////////////////////////////////////////
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack_ic_qsgimtexture.hxx"

#define  QXPACK_IcPImplPrivTemp_new    qxpack_ic_new
#define  QXPACK_IcPImplPrivTemp_delete qxpack_ic_delete
#include "qxpack/indcom/common/qxpack_ic_pimplprivtemp.hpp"

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QSurfaceFormat>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
// private object
// ////////////////////////////////////////////////////////////////////////////


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

}

// ============================================================================
// dtor
// ============================================================================
IcQSGImTexture :: ~IcQSGImTexture (  )
{
    if ( m_obj != Q_NULLPTR ) {
        m_obj = nullptr;
    }
}

// ============================================================================
// ctor copy
// ============================================================================
IcQSGImTexture :: IcQSGImTexture ( const IcQSGImTexture &other ) : QSGTexture()
{
    m_obj = nullptr;
}

// ============================================================================
// operator = override
// ============================================================================
IcQSGImTexture &  IcQSGImTexture :: operator = ( const IcQSGImTexture &other )
{
    return *this;
}

// ============================================================================
// check if this object is null or not
// ============================================================================
bool    IcQSGImTexture :: isNull( ) const { return ( m_obj == nullptr ); }

// ============================================================================
// override functiosn, see QSGTexture
// ============================================================================
bool   IcQSGImTexture :: hasAlphaChannel ( ) const { return false; }
bool   IcQSGImTexture :: hasMipmaps ( )      const { return false; }
QSize  IcQSGImTexture :: textureSize ( )     const { return QSize(); }
qint64 IcQSGImTexture :: comparisonKey() const     { return 0;  }

// ============================================================================
// update texture by data
// ============================================================================
bool   IcQSGImTexture :: updateTexture ( const void *bits, int width, int height, QImage::Format fmt )
{
    return false;
}

// ============================================================================
// update texture by image
// ============================================================================
bool   IcQSGImTexture :: updateTexture ( const QImage & im )
{
     return false;
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
