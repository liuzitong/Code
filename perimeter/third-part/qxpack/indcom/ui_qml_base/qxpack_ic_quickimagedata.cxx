#ifndef QXPACK_IC_QUICKIMAGEDATA_CXX
#define QXPACK_IC_QUICKIMAGEDATA_CXX

#include "qxpack_ic_quickimagedata.hxx"
#include <QDebug>
#include <QReadWriteLock>

namespace QxPack { static void  gDeletePrivObj( void *obj ); }
#define DeletePrivObj( o, t )   do{ QxPack::gDeletePrivObj(( t *) o ); }while(0)
#define QXPACK_IcPImplPrivTemp_new( t, ... )  ( new t( __VA_ARGS__ ))
#define QXPACK_IcPImplPrivTemp_delete DeletePrivObj
#include "qxpack/indcom/common/qxpack_ic_pimplprivtemp.hpp"

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//  private object
// ////////////////////////////////////////////////////////////////////////////
#define T_PrivPtr( o )  T_ObjCast( IcQuickImageDataPriv *, o )
class QXPACK_IC_HIDDEN IcQuickImageDataPriv :
    public QObject, public IcPImplPrivTemp< IcQuickImageDataPriv >
{
    Q_OBJECT
private:
    IcImageData   m_im_data; IcQuickImageData::UseMode  m_use_mode;
public :
    // ------------------------------------------------------------------------
    // ctor
    // ------------------------------------------------------------------------
    IcQuickImageDataPriv ( )
    {
        m_use_mode = IcQuickImageData::UseMode_Normal;
    }

    // ------------------------------------------------------------------------
    // ctor ( copy )
    // ------------------------------------------------------------------------
    IcQuickImageDataPriv ( const IcQuickImageDataPriv & ) : QObject( Q_NULLPTR )
    { qFatal("IcQuickImageDataPriv::ctor(copy) did not support!"); }

    // ------------------------------------------------------------------------
    // dtor
    // ------------------------------------------------------------------------
    virtual ~IcQuickImageDataPriv( ) { }

    // ------------------------------------------------------------------------
    // initailize function
    // ------------------------------------------------------------------------
    inline bool  init( IcQuickImageData::UseMode m )
    { m_use_mode = m; return true; }

    // ------------------------------------------------------------------------
    // update the image data,
    // ------------------------------------------------------------------------
    inline void  updateImageData ( const IcImageData &im )
    {
        m_im_data = im; // attach to spec. image
        emit this->imageDataChanged();
    }

    inline void  emptyImageData( ) { m_im_data = IcImageData(); }

    // ------------------------------------------------------------------------
    // start use the image data
    // ------------------------------------------------------------------------
    inline void  beginUseImageData( IcImageData &im )
    { im = m_im_data; }

    // ------------------------------------------------------------------------
    // end use the image data
    // ------------------------------------------------------------------------
    inline void  endUseImageData( )
    {
        if ( m_use_mode == IcQuickImageData::UseMode_DeleteIfEndUse ) {
            m_im_data = IcImageData();
        }
    }

    inline auto    imageDataSize() const -> QSize { return m_im_data.image().size(); }

    Q_SIGNAL void imageDataChanged( );
};

// ============================================================================
// delete the object
// ============================================================================
static void  gDeletePrivObj( void *obj )
{
    if ( obj != Q_NULLPTR ) {
        T_PrivPtr( obj )->emptyImageData();
        T_PrivPtr( obj )->deleteLater();
    }
}


// ////////////////////////////////////////////////////////////////////////////
// API wrapper
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor ( empty )
// ============================================================================
IcQuickImageData :: IcQuickImageData ( ) { m_obj = nullptr; }

// ============================================================================
// ctor ( with a spec. use mode )
// ============================================================================
IcQuickImageData :: IcQuickImageData ( IcQuickImageData::UseMode m )
{
    m_obj = nullptr;
    IcQuickImageDataPriv::buildIfNull( & m_obj );
    T_PrivPtr( m_obj )->init( m );
    QObject::connect( T_PrivPtr( m_obj ), SIGNAL(imageDataChanged()), this, SIGNAL(imageDataChanged()), Qt::QueuedConnection );
}

// ============================================================================
// dtor
// ============================================================================
IcQuickImageData :: ~IcQuickImageData ( )
{
    if ( m_obj != nullptr ) {
        QObject::disconnect( T_PrivPtr( m_obj ), Q_NULLPTR, this, Q_NULLPTR );
        IcQuickImageDataPriv::attach( & m_obj, nullptr );
    }
}

// ============================================================================
// attach to other object
// ============================================================================
bool  IcQuickImageData :: attach ( const IcQuickImageData &other )
{
    // free old connections
    if ( m_obj != nullptr ) {
        QObject::disconnect( T_PrivPtr( m_obj ), Q_NULLPTR, this, Q_NULLPTR );
        IcQuickImageDataPriv::attach( & m_obj, nullptr );
        m_obj = nullptr;
    }

    // attach to other object
    IcQuickImageDataPriv::attach( & m_obj, const_cast< void **>( & other.m_obj ));
    if ( m_obj != nullptr ) {
        QObject::connect( T_PrivPtr( m_obj ), SIGNAL(imageDataChanged()), this, SIGNAL(imageDataChanged()));
    }

    return true;
}

// ============================================================================
// check if current object is null or not
// ============================================================================
bool  IcQuickImageData :: isNull() const { return ( m_obj == nullptr ); }

// ============================================================================
// update current image data
// ============================================================================
void  IcQuickImageData :: setImageData ( const IcImageData &im )
{
    if ( m_obj != nullptr ) {
        T_PrivPtr( m_obj )->updateImageData( im );
    }
}

// ============================================================================
// begin use the image data
// ============================================================================
void   IcQuickImageData :: beginUseImageData( IcImageData &im )
{
    if ( m_obj != nullptr ) {
        T_PrivPtr( m_obj )->beginUseImageData( im );
    }
}

// ============================================================================
// end use the image data
// ============================================================================
void   IcQuickImageData :: endUseImageData()
{
    if ( m_obj != nullptr ) {
        T_PrivPtr( m_obj )->endUseImageData();
    }
}

// ============================================================================
// access the image data image size
// ============================================================================
auto     IcQuickImageData :: imageDataImSize() const -> QSize
{
    if ( m_obj != nullptr ) {
        return T_PrivPtr(m_obj)->imageDataSize();
    } else {
        return QSize(0,0);
    }
}

}

#include "qxpack_ic_quickimagedata.moc"
#endif
