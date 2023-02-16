#ifndef QXPACK_IC_QUICKIMAGEDATA_HXX
#define QXPACK_IC_QUICKIMAGEDATA_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include "qxpack/indcom/ui_qml_base/qxpack_ic_qsgimtexture.hxx"
#include "qxpack/indcom/sys/qxpack_ic_imagedata.hxx"
#include <QSharedPointer>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief support a user custom image object, running in UI thread only
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcQuickImageData : public QObject {
    Q_OBJECT
    Q_PROPERTY( QSize   imageDataImSize  READ  imageDataImSize  NOTIFY imageDataChanged )
public:
    //! operate mode
    enum UseMode { UseMode_Normal = 0, UseMode_DeleteIfEndUse };

    //! ctor ( empty )
    explicit IcQuickImageData( );

    /*!
     * @brief ctor, create a data object
     */
    IcQuickImageData ( UseMode );

    //! dtor
    virtual ~IcQuickImageData( ) Q_DECL_OVERRIDE;

    //! attach to spec. object
    bool  attach ( const IcQuickImageData &other );

    //! check if it is null
    bool  isNull( ) const;

    //! @since 0.5.10
    QSize  imageDataImSize() const;

    //! @brief set the image data
    //! @note  if UseMode is Normal, and existed a old IcImageData, the old IcImageData \n
    //!  will be free ( or recycle into object cache
    Q_SLOT   void   setImageData ( const IcImageData &im );

    //! begin use the current image data
    //! @note if UseMode is UseMode_DeleteIfEndUse, it will be
    Q_SLOT   void   beginUseImageData ( IcImageData &im );

    //! @brief end use the current image
    Q_SLOT   void   endUseImageData ( );

    //! the image emit this signal
    Q_SIGNAL void  imageDataChanged( );

private:
    void *m_obj;
    Q_DISABLE_COPY( IcQuickImageData )
};

}

#endif 
