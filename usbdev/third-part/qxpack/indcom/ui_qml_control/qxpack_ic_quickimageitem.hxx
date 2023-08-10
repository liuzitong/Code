// ////////////////////////////////////////////////////////////////////////////
// @author nightwing
// @verbatim
//      author     date           memo.
//      nightwing  2019/05/07     changed data source !
//      nightwing  2019/06/05     add fill mode
//      nightwing  2021/03/22     add geometryChanged
// @endvarbatim
// ////////////////////////////////////////////////////////////////////////////
#ifndef QXPACK_IC_QUICKIMAGEITEM_HXX
#define QXPACK_IC_QUICKIMAGEITEM_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"

#include <QQuickItem>
#include <QSGNode>
#include <QImage>
#include <QPointF>
#include <QSizeF>
#include <QSGTextureProvider>
#include <QSGSimpleTextureNode>
#include <QRectF>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief  this item is used in Quick scene.
 * @details It need IcSharedImSource object for image data
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcQuickImageItem : public QQuickItem {
    Q_OBJECT

    Q_PROPERTY( QObject*  imageData        READ  imageData           WRITE  setImageData    NOTIFY imageDataChanged )
    Q_PROPERTY( bool      flipHorizontal   READ  flipHorizontal      WRITE  setFlipHorizontal   )
    Q_PROPERTY( bool      flipVertical     READ  flipVertical        WRITE  setFlipVertical     )
    Q_PROPERTY( QSizeF    textureSize      READ  textureSize         NOTIFY textureSizeChanged  )

    // NOTE: here use string as enumeric value. Due to QML doest not support enum.
    Q_PROPERTY( QString   fillModeStr      READ  fillModeStr         NOTIFY fillModeChanged )
    Q_PROPERTY( int       fillMode         READ  fillMode            WRITE  setFillMode      NOTIFY fillModeChanged )
public:
     //! ctor
     IcQuickImageItem ( QQuickItem *pa = Q_NULLPTR );

     //! dtor
     virtual ~IcQuickImageItem () Q_DECL_OVERRIDE;

     //! return the data
     QObject*  imageData() const;
     void      setImageData( QObject* );

     //! the image flip function
     bool      flipHorizontal( ) const;
     bool      flipVertical( )   const;
     void      setFlipHorizontal( bool sw );
     void      setFlipVertical  ( bool sw );

     //! the fill mode
     // "KeepAspectRatio"   ( Qt::KeepAspectRatio )
     // "IgnoreAspectRatio" ( Qt::IgnoreAspectRatio )
     QString   fillModeStr() const;
     int       fillMode( ) const;
     void      setFillMode ( int );

     //! return current texture size
     QSizeF    textureSize( ) const;

     //! return the point from image point
     Q_INVOKABLE QPointF  mapImagePointToItemPoint ( const QPointF &im_pt );

     //! return the item point
     Q_INVOKABLE QPointF  mapItemPointToImagePoint ( const QPointF &item_pt );

     // =======================================================================
     // override functions
     // =======================================================================
protected:
     virtual QSGNode*   updatePaintNode(QSGNode*,UpdatePaintNodeData*) Q_DECL_OVERRIDE;

public:
     virtual QSGTextureProvider *  textureProvider() const Q_DECL_OVERRIDE;
     virtual bool       isTextureProvider() const Q_DECL_OVERRIDE;

     //! emit while call setImData
     Q_SIGNAL  void  imageDataChanged( );

     //! emit while texture size changed
     Q_SIGNAL  void  textureSizeChanged( );

     //! emit the fill mode change
     Q_SIGNAL  void  fillModeChanged( );

     //! the hold API for static library
     static  void    _reg();

protected:
    QSGNode*  qsgUpnImSourceDataNull( QSGNode* );
    QSGNode*  qsgUpnNullNode    ( QSGNode *old, const QImage &im );
    QSGNode*  qsgUpnExistedNode ( QSGNode *old, const QImage &im );
    void      qsgEnsureFlipMode ( QSGNode * );
    void      qsgRemapTexture   ( QSGNode *, const QSizeF & );
private:
    void *m_obj;
    Q_DISABLE_COPY( IcQuickImageItem )

protected:

// since 0.5.12 added
#if  QT_VERSION_MAJOR < 6
     virtual void       geometryChanged ( const QRectF &newGeometry, const QRectF &oldGeometry ) Q_DECL_OVERRIDE;
#else
     virtual void       geometryChange  ( const QRectF &newGeometry, const QRectF &oldGeometry ) Q_DECL_OVERRIDE;
#endif

};

}

#endif
