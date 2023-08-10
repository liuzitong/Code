// ////////////////////////////////////////////////////////////////////////////
/*!
  @file    qxpack_ic_imagedata
  @author  night wing
  @date    2018/09, 2019/05
  @brief   describe the image data
  @version 0.1.0.0
*/
// ////////////////////////////////////////////////////////////////////////////
#ifndef QXPACK_IC_IMAGEDATA_HXX
#define QXPACK_IC_IMAGEDATA_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include <QImage>
#include <QVariant>
#include <QObject>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief  the image data object, used to store user image and user variant data
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API IcImageData QXPACK_IC_FINAL {

public:
    //! create empty object
    explicit IcImageData( );

    //! create object by another
    IcImageData( const IcImageData & );

    //! copy assign
    IcImageData &  operator = ( const IcImageData & );

    //! dtor
    //virtual ~IcImageData( ); // since 0.4.8, changed to non-virtual
    ~IcImageData( );

    //! check if the object is null or not
    bool         isNull()    const;

    //! return the shallow copy of image
    QImage       image()     const;

    //! return the shadllow copy of user data 
    QVariant     varData()   const;

    //! setup current image
    void         setImage ( const QImage   & );

    //! setup the user data
    void         setVarData ( const QVariant & );

    //! directly access the image, this will do deep copy if shared
    QImage &     imageRef();
    
    //! directly access the user data, this will do deep copy if shared
    QVariant &   varDataRef();

    // ========================================================================
    // static functions
    // ========================================================================
    //! duplicate handle of inner object
    //! @warning user should detach it by manual call detachObjPtr()
    static void *  attachObjPtr ( const IcImageData &im );

    //! free the handle of inner object
    static void    detachObjPtr ( void* );

    //! build an object by spec. handle
    static IcImageData  buildByObjPtr( void* );

private:    
     void *m_obj;
};

}

#endif
