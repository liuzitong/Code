// ////////////////////////////////////////////////////////////////////////////
// @author nightwing
// @verbatim
//      author     date           memo.
//      nightwing  2020/07/08     init
//      nightwing  2020/09/24     add new function: pickYdataAt
// @endvarbatim
// ////////////////////////////////////////////////////////////////////////////
#ifndef QXPACK_IC_QUICKLINESERIESITEM_HXX
#define QXPACK_IC_QUICKLINESERIESITEM_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"

#include <QQuickItem>
#include <QSGNode>
#include <QPointF>
#include <QVector2D>
#include <QRectF>
#include <QPointF>

namespace QxPack {

class IcQuickLineSeriesItemPickDataInfo;

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief  this item is used in Quick scene.
 * @details
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcQuickLineSeriesItem : public QQuickItem {
    Q_OBJECT

public :
    enum PickDataMode {
        PickDataMode_NearestX = 0,
        PickDataMode_NearstDist,
        PickDataMode_LinearX
    };
    Q_ENUM( PickDataMode )

private:
    Q_PROPERTY( QObject*  xySeriesData   READ  xySeriesData  WRITE  setXYSeriesData    NOTIFY xySeriesDataChanged )
    Q_PROPERTY( float     lineSize       READ  lineSize      WRITE  setLineSize        NOTIFY lineSizeChanged   )
    Q_PROPERTY( QColor    lineColor      READ  lineColor     WRITE  setLineColor       NOTIFY lineColorChanged  )
    Q_PROPERTY( QVector2D lineSizeRange  READ  lineSizeRange  CONSTANT )
    Q_PROPERTY( QRectF    viewport       READ  viewport      WRITE  setViewport        NOTIFY  viewportChanged )

    // added 20210520
    Q_PROPERTY( QObject * arySeriesData  READ  arySeriesData  WRITE  setArySeriesData  NOTIFY arySeriesDataChanged )
public:
     //! ctor
     explicit IcQuickLineSeriesItem ( QQuickItem *pa = Q_NULLPTR );

     //! dtor
     virtual ~IcQuickLineSeriesItem () Q_DECL_OVERRIDE;

     //! access the data source
     QObject*   xySeriesData() const;
     void       setXYSeriesData( QObject* );

     //! access the array data source
     //! @note xySeriesData and arySereisData are mutually exclusive
     //! @since 0.5.13
     QObject*   arySeriesData() const;
     void       setArySeriesData( QObject* );

     //! access the line property
     float      lineSize()   const;
     QColor     lineColor()  const;
     QVector2D  lineSizeRange()   const;

     //! @note the viewport is a math coord direction. And the y is the top-left point!
     QRectF    viewport() const;
     void      setViewport( const QRectF& );

     void      setLineSize  ( float );
     void      setLineColor( const QColor & );

     Q_INVOKABLE  QxPack::IcQuickLineSeriesItemPickDataInfo  makePickInfoObjAt( const QPointF &hit_coord, PickDataMode = PickDataMode::PickDataMode_NearestX ) const;
     Q_INVOKABLE  QPointF  mapValuePointToPxPos( const QPointF & ) const;

     // =======================================================================
     // override functions
     // =======================================================================
protected:
     virtual QSGNode*   updatePaintNode( QSGNode*,UpdatePaintNodeData*) Q_DECL_OVERRIDE;

#if  ( QT_VERSION_MAJOR < 6 )
     virtual void       geometryChanged( const QRectF &newGeometry, const QRectF &oldGeometry ) Q_DECL_OVERRIDE;
#else
     virtual void       geometryChange ( const QRectF &newGeometry, const QRectF &oldGeometry ) Q_DECL_OVERRIDE;     
#endif

public :
     Q_SIGNAL void  xySeriesDataChanged();
     Q_SIGNAL void  lineSizeChanged  ();
     Q_SIGNAL void  lineColorChanged ();

     Q_SIGNAL void  viewportChanged();
     Q_SIGNAL void  geomChanged();

     // added 20210520
     Q_SIGNAL void  arySeriesDataChanged();

     //! the hold API for static library
     static  void    _reg();

protected:

private:
    void *m_obj;
    Q_DISABLE_COPY( IcQuickLineSeriesItem )
};


// ////////////////////////////////////////////////////////////////////////////
// the pick data information
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcQuickLineSeriesItemPickDataInfo  {
    Q_GADGET
    Q_PROPERTY( QPointF  lowerPoint  READ  lowerPoint  CONSTANT )
    Q_PROPERTY( QPointF  upperPoint  READ  upperPoint  CONSTANT )
    Q_PROPERTY( QPointF  valuePoint  READ  valuePoint  CONSTANT )
public:
    explicit IcQuickLineSeriesItemPickDataInfo( );
    IcQuickLineSeriesItemPickDataInfo( const IcQuickLineSeriesItemPickDataInfo & );
    IcQuickLineSeriesItemPickDataInfo &  operator = ( const IcQuickLineSeriesItemPickDataInfo & );
    ~IcQuickLineSeriesItemPickDataInfo();
    bool operator == ( const IcQuickLineSeriesItemPickDataInfo & ) const;
    inline QPointF lowerPoint() const { return m_pt_lo; }
    inline QPointF upperPoint() const { return m_pt_up; }
    inline QPointF valuePoint() const { return m_pt_val; }

protected:  
    void  setPointInfo( const QPointF &lo, const QPointF &hi, const QPointF &v );
    friend class IcQuickLineSeriesItem;
private:
    QPointF m_pt_lo, m_pt_up, m_pt_val;
    void *m_ext;
};

}
Q_DECLARE_METATYPE( QxPack::IcQuickLineSeriesItemPickDataInfo )
#endif
