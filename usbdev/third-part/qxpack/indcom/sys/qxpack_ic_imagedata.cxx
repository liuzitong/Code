#ifndef QXPACK_IC_IMAGEDATA_CXX
#define QXPACK_IC_IMAGEDATA_CXX

#include "qxpack_ic_imagedata.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/common/qxpack_ic_pimplprivtemp.hpp"
//#include "qxpack/indcom/common/qxpack_ic_objectcache.hxx"
#include "qxpack/indcom/common/qxpack_ic_queuetemp.hpp"

#include <QVector>
#include <QList>
#include <QString>



namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
//                     IcImageData
//
// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
//  private image data object
// ////////////////////////////////////////////////////////////////////////////
#define T_PrivPtr( o )  T_ObjCast( IcImageDataPriv*, o )
class QXPACK_IC_HIDDEN IcImageDataPriv : public IcPImplPrivTemp< IcImageDataPriv > {
private:
    QImage  m_image;  QVariant  m_var;  QString  m_cache_name;
public :
    IcImageDataPriv ( );
    IcImageDataPriv ( const IcImageDataPriv & );
    virtual ~IcImageDataPriv( ) { }
    inline QImage    image() const { return m_image; }
    inline QVariant  varData() const { return m_var; }
    inline QImage &  imageRef( )   { return m_image; }
    inline QVariant& varDataRef( ) { return m_var;   } 
    inline void      setImage   ( const QImage &im    ) { m_image = im;  }
    inline void      setVarData ( const QVariant &var ) { m_var   = var; }
};

// ============================================================================
// ctor
// ============================================================================
IcImageDataPriv :: IcImageDataPriv ( ) { }

// ============================================================================
// ctor ( Copy )
// ============================================================================
IcImageDataPriv :: IcImageDataPriv ( const IcImageDataPriv &other )
{
    m_image = other.m_image; m_var = other.m_var;
}

// ////////////////////////////////////////////////////////////////////////////
// wrapper API
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// create an empty object
// ============================================================================
IcImageData :: IcImageData( ) { m_obj = nullptr; }

// ============================================================================
// create object by another
// ============================================================================
IcImageData :: IcImageData( const IcImageData &other )
{
    m_obj = nullptr;
    IcImageDataPriv::attach( & m_obj, const_cast<void **>( & other.m_obj ));
}

// ============================================================================
// copy assign
// ============================================================================
IcImageData &  IcImageData :: operator = ( const IcImageData &other )
{
    IcImageDataPriv::attach( & m_obj, const_cast<void **>( & other.m_obj ));
    return *this;
}

// ============================================================================
// dtor
// ============================================================================
IcImageData :: ~IcImageData( )
{
    if ( m_obj != nullptr ) {
        IcImageDataPriv::attach( & m_obj, nullptr );
    }
}

// ============================================================================
// service APIs
// ============================================================================
bool     IcImageData :: isNull()  const { return ( m_obj == nullptr ); }
QImage   IcImageData :: image()   const { return ( m_obj != nullptr ? T_PrivPtr( m_obj )->image() : QImage()); }
QVariant IcImageData :: varData() const { return ( m_obj != nullptr ? T_PrivPtr( m_obj )->varData() : QVariant()); }
void     IcImageData :: setImage   ( const QImage &im )   { IcImageDataPriv::instanceCow( & m_obj )->setImage( im ); }
void     IcImageData :: setVarData ( const QVariant &var) { IcImageDataPriv::instanceCow( & m_obj )->setVarData( var ); }

QImage &  IcImageData :: imageRef ( ) { return IcImageDataPriv::instanceCow( & m_obj )->imageRef();   }
QVariant& IcImageData :: varDataRef( ){ return IcImageDataPriv::instanceCow( & m_obj )->varDataRef(); }

// ============================================================================
// [ static ]  attach existed object inner pointer
// ============================================================================
void*     IcImageData :: attachObjPtr( const IcImageData &im )
{
    void *obj = nullptr;
    if ( im.m_obj != nullptr ) {
        IcImageDataPriv::attach( & obj, const_cast<void**>( & im.m_obj ));
    }
    return obj;
}

// ============================================================================
// [ static ]  detach the object pointer
// ============================================================================
void      IcImageData :: detachObjPtr( void *obj )
{
    if ( obj != nullptr ) {
        IcImageDataPriv::attach( & obj, nullptr ); // drop
    }
}

// ============================================================================
// [ static ] build an object by spec. object pointer
// ============================================================================
IcImageData  IcImageData :: buildByObjPtr( void *obj )
{
    IcImageData  im;
    if ( obj != nullptr ) {
        IcImageDataPriv::attach( & im.m_obj, const_cast<void**>( &obj ));
    }
    return im;
}


// ////////////////////////////////////////////////////////////////////////////
//
// ////////////////////////////////////////////////////////////////////////////


}

#endif
