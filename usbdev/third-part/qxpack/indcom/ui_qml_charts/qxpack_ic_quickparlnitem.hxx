// ////////////////////////////////////////////////////////////////////////////
// @author nightwing
// @verbatim
//      author     date           memo.
//      nightwing  2020/09/23     init
// @endvarbatim
// ////////////////////////////////////////////////////////////////////////////
#ifndef QXPACK_IC_QUICKPARLNITEM_HXX
#define QXPACK_IC_QUICKPARLNITEM_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"

#include <QQuickItem>
#include <QVector2D>
#include <QRectF>
#include <QColor>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief  this item is used in Quick scene.
 * @details
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcQuickParLnItem : public QQuickItem {
    Q_OBJECT

    Q_PROPERTY( QVector2D  lineSizeRange  READ  lineSizeRange  CONSTANT )
    Q_PROPERTY( qreal   lineSize   READ  lineSize    WRITE  setLineSize   NOTIFY  lineSizeChanged )
    Q_PROPERTY( QColor  lineColor  READ  lineColor   WRITE  setLineColor  NOTIFY  lineColorChanged )
    Q_PROPERTY( QRectF  viewport   READ  viewport    WRITE  setViewport   NOTIFY  viewportChanged )
    Q_PROPERTY( qreal   min        READ  min         WRITE  setMin        NOTIFY  rangeChanged    )
    Q_PROPERTY( qreal   max        READ  max         WRITE  setMax        NOTIFY  rangeChanged    )
    Q_PROPERTY( int    tickCount   READ  tickCount   WRITE  setTickCount  NOTIFY  tickCountChanged )
    Q_PROPERTY( bool   vertical    READ  vertical    WRITE  setVertical   NOTIFY  verticalChanged  )

    //! added 2021/03/13, since 0.5.11
    Q_PROPERTY( qreal  headRsvdPx  READ  headRsvdPx  WRITE setHeadRsvdPx  NOTIFY  headRsvdPxChanged )
    Q_PROPERTY( qreal  tailRsvdPx  READ  tailRsvdPx  WRITE setTailRsvdPx  NOTIFY  tailRsvdPxChanged )
    Q_PROPERTY( bool   showHeadLine  READ  showHeadLine   WRITE  setShowHeadLine  NOTIFY showHeadLineChanged )
    Q_PROPERTY( bool   showTailLine  READ  showTailLine   WRITE  setShowTailLine  NOTIFY showTailLineChanged )

public:
     //! ctor
     explicit IcQuickParLnItem ( QQuickItem *pa = Q_NULLPTR );

     //! dtor
     virtual ~IcQuickParLnItem () Q_DECL_OVERRIDE;

     QVector2D   lineSizeRange() const;

     //! control this value axis is a horiztonal or vertical axis
     bool        vertical() const;
     void        setVertical( bool );

     //! access the viewport ( geometry )
     QRectF     viewport() const;
     void       setViewport( const QRectF & );

     //! access the line size ( material )
     float      lineSize ()  const;
     void       setLineSize( float );

     //! acess the line color ( material )
     QColor     lineColor()  const;
     void       setLineColor( const QColor & );

     //! access the major tick count
     int        tickCount() const;
     void       setTickCount( int );

     //! access the value range
     qreal      min() const;
     qreal      max() const;
     void       setMin( qreal );
     void       setMax( qreal );

     //! access the head & tail rsvd pixel
     //! since 0.5.11
     qreal      headRsvdPx() const;
     qreal      tailRsvdPx() const;
     void       setHeadRsvdPx( qreal );
     void       setTailRsvdPx( qreal );
     bool       showHeadLine() const;
     bool       showTailLine() const;
     void       setShowHeadLine( bool );
     void       setShowTailLine( bool );

     // =======================================================================
     // override functions
     // =======================================================================
protected:
     virtual QSGNode*   updatePaintNode ( QSGNode*,UpdatePaintNodeData*) Q_DECL_OVERRIDE;

#if  QT_VERSION_MAJOR < 6
     virtual void       geometryChanged ( const QRectF &newGeometry, const QRectF &oldGeometry ) Q_DECL_OVERRIDE;
#else
     virtual void       geometryChange  ( const QRectF &newGeometry, const QRectF &oldGeometry ) Q_DECL_OVERRIDE;
#endif

public :
     Q_SIGNAL void  lineSizeChanged();
     Q_SIGNAL void  lineColorChanged();

     Q_SIGNAL void  viewportChanged();
     Q_SIGNAL void  tickCountChanged();

     Q_SIGNAL void  rangeChanged();
     Q_SIGNAL void  verticalChanged();

     // since 0.5.11 changed
     Q_SIGNAL void  headRsvdPxChanged();
     Q_SIGNAL void  tailRsvdPxChanged();
     Q_SIGNAL void  showHeadLineChanged();
     Q_SIGNAL void  showTailLineChanged();

     //! the hold API for static library
     static  void    _reg();

private:
    void *m_obj;
    Q_DISABLE_COPY( IcQuickParLnItem )
};

}

#endif
