// ////////////////////////////////////////////////////////////////////////////
// @author nightwing
// @verbatim
//      author     date           memo.
//      nightwing  2020/07/28     init
// @endvarbatim
// ////////////////////////////////////////////////////////////////////////////
#ifndef QXPACK_IC_QUICKVALUEAXISITEM_HXX
#define QXPACK_IC_QUICKVALUEAXISITEM_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"

#include <QQuickItem>
#include <QSGNode>
#include <QPointF>
#include <QVector2D>
#include <QRectF>
#include <QFont>
#include <QColor>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
//                 Value Axis Item
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcQuickValueAxisItem : public QQuickItem {
    Q_OBJECT

public :
    enum TickDirection {
        TickDirection_CW = 0, TickDirection_CCW = 1, TickDirection_Both
    };
    Q_ENUM( TickDirection )

private:
    Q_PROPERTY( QVector2D  lineSizeRange  READ  lineSizeRange  CONSTANT )
    Q_PROPERTY( qreal      lineSize   READ  lineSize    WRITE  setLineSize   NOTIFY  lineSizeChanged )
    Q_PROPERTY( QColor     lineColor  READ  lineColor   WRITE  setLineColor  NOTIFY  lineColorChanged )
    Q_PROPERTY( QRectF     viewport   READ  viewport    WRITE  setViewport   NOTIFY  viewportChanged )

    Q_PROPERTY( TickDirection  tickDirection  READ  tickDirection  WRITE  setTickDirection  NOTIFY  tickDirectionChanged )

    Q_PROPERTY( bool    vertical   READ  vertical    WRITE  setVertical   NOTIFY  verticalChanged  )
    Q_PROPERTY( qreal   min        READ  min         WRITE  setMin        NOTIFY  rangeChanged    )
    Q_PROPERTY( qreal   max        READ  max         WRITE  setMax        NOTIFY  rangeChanged    )
    Q_PROPERTY( qreal   blnMin     READ  blnMin      WRITE  setBlnMin     NOTIFY  blnRangeChanged )
    Q_PROPERTY( qreal   blnMax     READ  blnMax      WRITE  setBlnMax     NOTIFY  blnRangeChanged )

    Q_PROPERTY( int    majorTickCount      READ  majorTickCount    WRITE  setMajorTickCount       NOTIFY  majorTickCountChanged )
    Q_PROPERTY( int    minorTickCount      READ  minorTickCount    WRITE  setMinorTickCount       NOTIFY  minorTickCountChanged )
    Q_PROPERTY( int    minorHintStep       READ  minorHintStep     WRITE  setMinorHintStep        NOTIFY  minorHintStepChanged  )
    Q_PROPERTY( qreal  majorSize           READ  majorSize         WRITE  setMajorSize            NOTIFY  majorSizeChanged )
    Q_PROPERTY( qreal  minorSize           READ  minorSize         WRITE  setMinorSize            NOTIFY  minorSizeChanged )
    Q_PROPERTY( qreal  minorHintSize       READ  minorHintSize     WRITE  setMinorHintSize        NOTIFY  minorHintSizeChanged )

    Q_PROPERTY( bool   dispStartTickLabel  READ  dispStartTickLabel WRITE  setDispStartTickLabel  NOTIFY dispStartTickLabelChanged )
    Q_PROPERTY( bool   dispMinorTickLabel  READ  dispMinorTickLabel WRITE  setDispMinorTickLabel  NOTIFY dispMinorTickLabelChanged )
    Q_PROPERTY( bool   dispMinorHintLabel  READ  dispMinorHintLabel WRITE  setDispMinorHintLabel  NOTIFY dispMinorHintLabelChanged )

    Q_PROPERTY( QFont  labelFont           READ  labelFont          WRITE  setLabelFont           NOTIFY labelFontChanged   )
    Q_PROPERTY( QColor labelColor          READ  labelColor         WRITE  setLabelColor          NOTIFY labelColorChanged  )
    Q_PROPERTY( int    labelPrecision      READ  labelPrecision     WRITE  setLabelPrecision      NOTIFY labelPrecisionChanged   )
    Q_PROPERTY( int    labelDistOfv        READ  labelDistOfv       WRITE  setLabelDistOfv        NOTIFY labelDistOfvChanged )

    Q_PROPERTY( qreal  headRsvdPx          READ  headRsvdPx         WRITE setHeadRsvdPx           NOTIFY  headRsvdPxChanged )
    Q_PROPERTY( qreal  tailRsvdPx          READ  tailRsvdPx         WRITE setTailRsvdPx           NOTIFY  tailRsvdPxChanged )

    // since 0.5.12
    Q_PROPERTY( bool   labelVisible        READ  labelVisible       WRITE setLabelVisible         NOTIFY  labelVisibleChanged )
public:
     //! ctor
     explicit IcQuickValueAxisItem ( QQuickItem *pa = Q_NULLPTR );

     //! dtor
     virtual ~IcQuickValueAxisItem () Q_DECL_OVERRIDE;

     QVector2D   lineSizeRange() const;

     //! control this value axis is a horiztonal or vertical axis
     bool       vertical() const;
     void       setVertical( bool );

     //! access the viewport ( geometry )
     //! @note simplely you can setup ( min, min, max - min, max - min )
     QRectF     viewport() const;
     void       setViewport( const QRectF & );

     //! access the line size ( material )
     float      lineSize ()  const;
     void       setLineSize( float );

     //! acess the line color ( material )
     QColor     lineColor()  const;
     void       setLineColor( const QColor & );

     //! access the major tick count
     //! @example the ruler, assume 10cm. It has 10 major ticks
     int        majorTickCount() const;
     void       setMajorTickCount( int );

     //! access the minor tick count
     //! @note this means how many base ticks between two major ticks.
     //! @example  the ruler, assume 10cm. It has 10 major ticks. \n
     //!  and the gap( 0 cm to 1 cm ), has 9 minor ticks.
     int        minorTickCount() const;
     void       setMinorTickCount( int );

     //! access the minor hint step
     //! @example  the ruler, the gap ( 0cm to 1cm ), has 9 minor ticks, but
     //!  the 5th minor tick is more bigger than others. The 5th minor tick is just minor hint tick.
     int        minorHintStep() const;
     void       setMinorHintStep ( int );

     //! access the tick direction
     TickDirection  tickDirection() const;
     void           setTickDirection( TickDirection );

     //! access the tick size information
     qreal      majorSize() const;
     qreal      minorSize() const;
     qreal      minorHintSize() const;
     void       setMajorSize( qreal );
     void       setMinorSize( qreal );
     void       setMinorHintSize( qreal );

     //! access the value range
     qreal      min() const;
     qreal      max() const;
     void       setMin( qreal );
     void       setMax( qreal );

     //! access the base line min and max range
     //! @note the base line means the value axis
     qreal      blnMin() const;
     qreal      blnMax() const;
     void       setBlnMin( qreal );
     void       setBlnMax( qreal );

     //! access the label font and color
     QFont      labelFont()  const;
     QColor     labelColor() const;
     void       setLabelFont ( const QFont & );
     void       setLabelColor( const QColor & );

     //! access the label point precision and distance offset
     int        labelPrecision() const;
     int        labelDistOfv() const;
     void       setLabelPrecision ( int );
     void       setLabelDistOfv( int );

     //! access the label display property
     bool       dispStartTickLabel() const;
     bool       dispMinorTickLabel() const;
     bool       dispMinorHintLabel() const;
     void       setDispStartTickLabel( bool );
     void       setDispMinorTickLabel( bool );
     void       setDispMinorHintLabel( bool );

     //! access the head & tail rsvd pixel
     qreal      headRsvdPx() const;
     qreal      tailRsvdPx() const;
     void       setHeadRsvdPx( qreal );
     void       setTailRsvdPx( qreal );

     //! since 0.5.12
     bool       labelVisible() const;
     void       setLabelVisible( bool );

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
     Q_SIGNAL void  majorTickCountChanged();
     Q_SIGNAL void  minorTickCountChanged();

     Q_SIGNAL void  minorHintStepChanged();
     Q_SIGNAL void  majorSizeChanged();
     Q_SIGNAL void  minorSizeChanged();
     Q_SIGNAL void  minorHintSizeChanged();
     Q_SIGNAL void  tickDirectionChanged();
     Q_SIGNAL void  verticalChanged();
     Q_SIGNAL void  rangeChanged();
     Q_SIGNAL void  blnRangeChanged();
     Q_SIGNAL void  dispStartTickLabelChanged();
     Q_SIGNAL void  dispMinorTickLabelChanged();
     Q_SIGNAL void  dispMinorHintLabelChanged();
     Q_SIGNAL void  labelFontChanged();
     Q_SIGNAL void  labelColorChanged();
     Q_SIGNAL void  labelPrecisionChanged();
     Q_SIGNAL void  labelDistOfvChanged();

     Q_SIGNAL void  headRsvdPxChanged();
     Q_SIGNAL void  tailRsvdPxChanged();

     // since 0.5.12
     Q_SIGNAL void  labelVisibleChanged();

     //! the hold API for static library
     static  void    _reg();

private:
    void *m_obj;
    Q_DISABLE_COPY( IcQuickValueAxisItem )
};

}

#endif
