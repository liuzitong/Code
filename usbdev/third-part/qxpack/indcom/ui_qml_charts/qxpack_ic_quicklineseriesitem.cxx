#ifndef QXPACK_IC_QUICKLINESERIESITEM_CXX
#define QXPACK_IC_QUICKLINESERIESITEM_CXX

#include "qxpack_ic_quicklineseriesitem.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/ui_qml_base/qxpack_ic_quickxyseriesdata.hxx"
#include "qxpack/indcom/ui_qml_base/qxpack_ic_quickaryseriesdata.hxx"
#include "qxpack/indcom/sys/qxpack_ic_geom2dalgor.hxx"
#include "qxpack/indcom/ui_qml_charts/qsgnode/qxpack_ic_qsglineseriesnode_p.hxx"

#include <QColor>
#include <QAtomicInt>
#include <QQmlEngine>
#include <QQuickWindow>
#include <QMetaType>
#include <QLineF>
#include <QtMath>
#include <QtGlobal>
#include <QSGGeometry>
#include <QSGClipNode>
#include <QSGFlatColorMaterial>
#include <QDebug>

#define  LINESIZE_RANGE      QVector2D( 1.0f, 10.0f )
#define  LINESIZE_DEFAULT    (1.0f)

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
//                 The Pick Information Object
//
// ////////////////////////////////////////////////////////////////////////////
IcQuickLineSeriesItemPickDataInfo :: IcQuickLineSeriesItemPickDataInfo( )
{ m_ext = nullptr; }

IcQuickLineSeriesItemPickDataInfo :: IcQuickLineSeriesItemPickDataInfo( const IcQuickLineSeriesItemPickDataInfo &ot )
{
    m_pt_lo = ot.m_pt_lo; m_pt_up = ot.m_pt_up;
    m_pt_val = ot.m_pt_val;
}

IcQuickLineSeriesItemPickDataInfo :: ~IcQuickLineSeriesItemPickDataInfo( )
{  }

auto     IcQuickLineSeriesItemPickDataInfo :: operator = ( const IcQuickLineSeriesItemPickDataInfo &ot ) -> IcQuickLineSeriesItemPickDataInfo &
{
    m_pt_lo  = ot.m_pt_lo; m_pt_up = ot.m_pt_up; m_pt_val = ot.m_pt_val;
    return *this;
}

auto     IcQuickLineSeriesItemPickDataInfo :: operator == ( const IcQuickLineSeriesItemPickDataInfo &ot ) const -> bool
{ return m_pt_lo == ot.m_pt_lo && m_pt_up == ot.m_pt_up && m_pt_val == ot.m_pt_val; }

auto     IcQuickLineSeriesItemPickDataInfo :: setPointInfo( const QPointF &lo, const QPointF &hi, const QPointF &v ) -> void
{ m_pt_lo = lo; m_pt_up = hi; m_pt_val = v; }


// ////////////////////////////////////////////////////////////////////////////
//
//                 Line Series Private Item
//
// ////////////////////////////////////////////////////////////////////////////
static void QxPack_IcQuickLineSeriesItem_Reg( )
{
    static bool is_reg = false;
    if ( ! is_reg ) {
        is_reg = true;
        qmlRegisterType<QxPack::IcQuickLineSeriesItem>("qxpack.indcom.ui_qml_charts", 1, 0, "IcQuickLineSeriesItem");
        qRegisterMetaType<QxPack::IcQuickLineSeriesItemPickDataInfo>();
#if QT_VERSION_MAJOR < 6
        QMetaType::registerEqualsComparator<QxPack::IcQuickLineSeriesItemPickDataInfo>();
#endif
        qDebug() << "qxpack_ic_quicklineseriesitem registered."; // nw: 2019/05/07 added
    }
}

#define T_PrivPtr( o )  T_ObjCast( IcQuickLineSeriesItemPriv*, o )
class QXPACK_IC_HIDDEN  IcQuickLineSeriesItemPriv {
    friend class IcQuickLineSeriesItem;
private:
    IcQuickLineSeriesItem  *m_parent;
    IcQuickXYSeriesData     m_ser_dat;
    QAtomicInt              m_ser_dat_req_cntr, m_dat_attr_req_cntr;
    IcQuickArySeriesData    m_ary_dat;

    IcQSGLineSeriesNode::Material m_ui_mater;
    QRectF m_ui_view_port;
protected:

public :
    explicit IcQuickLineSeriesItemPriv ( IcQuickLineSeriesItem *pa );
    ~IcQuickLineSeriesItemPriv();

    inline auto  lineSize()   -> float&  { return m_ui_mater.size;   }
    inline auto  lineColor()  -> QColor& { return m_ui_mater.color;  }
    inline auto  viewport()   -> QRectF& { return m_ui_view_port; }
    inline auto  uiMater()    -> IcQSGLineSeriesNode::Material& { return m_ui_mater; }
    inline auto  xySeriesData() -> IcQuickXYSeriesData& { return m_ser_dat; }
    inline auto  arySeriesData()-> IcQuickArySeriesData&{ return m_ary_dat; } // added 20210520

    auto  setXYSeriesData( QObject* ) -> void;
    auto  setArySeriesData( QObject* )-> void; // added 20210520

    auto  onNewDataReady () -> void;
    auto  onNewDataAttrReady() -> void;
    auto  qsgUpdSeriesDataNull ( QSGNode* ) -> QSGNode*; // chg 20210520 ( from qsgUpdXySeriesDataNull )
    auto  qsgUpnNullNode    ( QSGNode*, const QList<QPointF>& ) -> QSGNode*;
    auto  qsgUpnNullNode    ( QSGNode*, const IcQuickArySeriesData::SeriesData & ) -> QSGNode*; // added 20210520
    auto  qsgUpnExistedNode ( QSGNode*, const QList<QPointF>& ) -> QSGNode*;
    auto  qsgUpnExistedNode ( QSGNode*, const IcQuickArySeriesData::SeriesData & ) -> QSGNode*; // added 20210520
};

// ============================================================================
// ctor
// ============================================================================
IcQuickLineSeriesItemPriv :: IcQuickLineSeriesItemPriv ( IcQuickLineSeriesItem *pa )
{
    m_parent = pa;
    m_ui_mater.size = LINESIZE_DEFAULT;  m_ui_mater.color = QColor("steelblue");
    m_ui_view_port = QRectF( 0,0,-1,-1);

    // ------------------------------------------------------------------------
    // DO NOT directly call upate() due to read source
    // ------------------------------------------------------------------------
    QObject::connect(
        & m_ser_dat, & IcQuickXYSeriesData::dataChanged, & m_ser_dat,
        [this]() { this->onNewDataReady(); }
    );
    QObject::connect(
        & m_ary_dat, & IcQuickArySeriesData::dataChanged, & m_ary_dat,
        [this]() { this->onNewDataReady(); }
    );

    // force update once
    m_ser_dat_req_cntr.storeRelease(1);
    m_dat_attr_req_cntr.storeRelease(1);
    QMetaObject::invokeMethod( m_parent, "update", Qt::QueuedConnection );
}

// ============================================================================
// dtor
// ============================================================================
IcQuickLineSeriesItemPriv :: ~IcQuickLineSeriesItemPriv ( )
{
    m_ser_dat.blockSignals(true);
    m_ary_dat.blockSignals(true);
}

// ============================================================================
// set the wave line data
// ============================================================================
auto     IcQuickLineSeriesItemPriv :: setXYSeriesData( QObject *obj ) -> void
{
    IcQuickXYSeriesData *dat_src = qobject_cast< IcQuickXYSeriesData*>( obj );
    if ( dat_src != Q_NULLPTR ) {
        m_ser_dat.attach( *dat_src ); // attach to another
    } else {
        IcQuickXYSeriesData tmp; m_ser_dat.attach( tmp );      // drop current
    }
    emit m_parent->xySeriesDataChanged();

    // --- added, 20210520, here need force release old array series data
    { IcQuickArySeriesData tmp; m_ary_dat.attach( tmp );}
    emit m_parent->arySeriesDataChanged();
    // ---

    onNewDataReady(); // force read data    
}

// ============================================================================
// set the wve line data by array ( added 20210520 )
// ============================================================================
auto     IcQuickLineSeriesItemPriv :: setArySeriesData( QObject *obj ) -> void
{
    IcQuickArySeriesData *dat_src = qobject_cast< IcQuickArySeriesData*>( obj );
    if ( dat_src != Q_NULLPTR ) {
        m_ary_dat.attach( *dat_src );
    } else {
        IcQuickArySeriesData tmp; m_ary_dat.attach( tmp );
    }
    emit m_parent->arySeriesDataChanged();

    { IcQuickXYSeriesData tmp; m_ser_dat.attach( tmp ); }
    emit m_parent->xySeriesDataChanged();

    onNewDataReady();
}

// ============================================================================
// handle while new data arraived
// ============================================================================
auto     IcQuickLineSeriesItemPriv :: onNewDataReady() -> void
{
    if ( m_ser_dat_req_cntr.loadAcquire() < 1 ) {
       // qDebug("new data ready");
        m_ser_dat_req_cntr.storeRelease(1);
        m_parent->update(); // shedule a paint
       // QMetaObject::invokeMethod( m_parent, "update", Qt::QueuedConnection );
    } else {
//        qDebug("ser data cntr > 0!");
    }
}

// ============================================================================
// handle while new material changed
// ============================================================================
auto     IcQuickLineSeriesItemPriv :: onNewDataAttrReady() -> void
{
    if ( m_dat_attr_req_cntr.loadAcquire() < 1 ) {
        m_dat_attr_req_cntr.storeRelease(1);
        m_parent->update();
       // QMetaObject::invokeMethod( m_parent, "update", Qt::QueuedConnection );
    }
}

// ============================================================================
// update the geometry node
// ============================================================================
auto     IcQuickLineSeriesItemPriv :: qsgUpdSeriesDataNull( QSGNode *old ) -> QSGNode*
{
    if ( old == Q_NULLPTR ) { return old; }
    auto node = static_cast< IcQSGLineSeriesNode* >( old );

    if ( m_dat_attr_req_cntr.loadAcquire() > 0 ) {
        m_dat_attr_req_cntr.storeRelease(0);
        node->setMaterial( m_ui_mater );
        node->setViewport( m_ui_view_port );
    }

    if ( m_ser_dat_req_cntr.loadAcquire() > 0 ) {
        m_ser_dat_req_cntr.storeRelease(0);
        node->setData( QList<QPointF>() );
        node->setData( QVector<float>(), 0, 1 );
    }

    return node;
}

auto     IcQuickLineSeriesItemPriv :: qsgUpnNullNode ( QSGNode *old, const QList<QPointF> &lst ) -> QSGNode*
{
    if ( old != Q_NULLPTR ) { return old; }

    m_dat_attr_req_cntr.storeRelease(0);
    m_ser_dat_req_cntr.storeRelease(0);

    auto node = IcQSGLineSeriesNode::createByContext( m_parent );
    node->setFlag( QSGNode::OwnedByParent, true );
    node->setMaterial( m_ui_mater );
    node->setViewport( m_ui_view_port );
    node->setData( lst );

    return node;
}

// added 20210520
auto     IcQuickLineSeriesItemPriv :: qsgUpnNullNode ( QSGNode *old, const IcQuickArySeriesData::SeriesData &sd ) -> QSGNode*
{
    if ( old != Q_NULLPTR ) { return old; }

    m_dat_attr_req_cntr.storeRelease(0);
    m_ser_dat_req_cntr.storeRelease(0);

    auto node = IcQSGLineSeriesNode::createByContext( m_parent );
    node->setFlag( QSGNode::OwnedByParent, true );
    node->setMaterial( m_ui_mater );
    node->setViewport( m_ui_view_port );
    node->setData( sd.vec, sd.x_ofv, sd.step );

    return node;
}

auto     IcQuickLineSeriesItemPriv :: qsgUpnExistedNode (QSGNode *old, const QList<QPointF> &lst ) -> QSGNode*
{
    if ( old == Q_NULLPTR ) { return old; }
    auto node = static_cast< IcQSGLineSeriesNode *>( old );

    if ( m_dat_attr_req_cntr.loadAcquire() > 0 ) {
        m_dat_attr_req_cntr.storeRelease(0);
        node->setMaterial( m_ui_mater );
        node->setViewport( m_ui_view_port );
    }

    if ( m_ser_dat_req_cntr.loadAcquire() > 0 ) {
        m_ser_dat_req_cntr.storeRelease(0);
        node->setData( lst );
       // qDebug("setup list data.");
    }

    return node;
}

auto     IcQuickLineSeriesItemPriv :: qsgUpnExistedNode (QSGNode *old, const IcQuickArySeriesData::SeriesData &sd ) -> QSGNode*
{
    if ( old == Q_NULLPTR ) { return old; }
    auto node = static_cast< IcQSGLineSeriesNode *>( old );

    if ( m_dat_attr_req_cntr.loadAcquire() > 0 ) {
        m_dat_attr_req_cntr.storeRelease(0);
        node->setMaterial( m_ui_mater );
        node->setViewport( m_ui_view_port );
    }

    if ( m_ser_dat_req_cntr.loadAcquire() > 0 ) {
        m_ser_dat_req_cntr.storeRelease(0);
        node->setData( sd.vec, sd.x_ofv, sd.step );
      // qDebug("setup vector data.");
    }

    return node;
}


// ////////////////////////////////////////////////////////////////////////////
//                      quick item
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcQuickLineSeriesItem :: IcQuickLineSeriesItem ( QQuickItem *pa ) : QQuickItem( pa )
{
    m_obj = qxpack_ic_new( IcQuickLineSeriesItemPriv, this );
    this->setFlags ( QQuickItem::ItemHasContents );
}

// ============================================================================
// dtor
// ============================================================================
IcQuickLineSeriesItem :: ~IcQuickLineSeriesItem ( )
{
    qxpack_ic_delete( m_obj, IcQuickLineSeriesItemPriv );
}

// ============================================================================
// access the wave line data
// ============================================================================
auto     IcQuickLineSeriesItem :: xySeriesData() const -> QObject*
{ return & T_PrivPtr( m_obj )->xySeriesData();}

auto     IcQuickLineSeriesItem :: setXYSeriesData( QObject *dat ) -> void
{  T_PrivPtr( m_obj )->setXYSeriesData( dat ); }

// added 20210520
auto     IcQuickLineSeriesItem :: arySeriesData() const -> QObject*
{ return  & T_PrivPtr( m_obj )->arySeriesData(); }

// added 20210520
auto     IcQuickLineSeriesItem :: setArySeriesData( QObject *dat ) -> void
{   T_PrivPtr( m_obj )->setArySeriesData( dat ); }

// ============================================================================
// access the line propertys
// ============================================================================
auto     IcQuickLineSeriesItem :: lineSize() const  -> float
{ return T_PrivPtr( m_obj )->lineSize(); }

auto     IcQuickLineSeriesItem :: lineColor() const -> QColor
{ return T_PrivPtr( m_obj )->lineColor(); }

auto     IcQuickLineSeriesItem :: lineSizeRange() const -> QVector2D
{ return LINESIZE_RANGE; }

auto     IcQuickLineSeriesItem :: setLineSize( float sz ) -> void
{
    auto priv = T_PrivPtr(m_obj);
    if ( sz >= 0.0f  &&  ! qFuzzyCompare( priv->lineSize(), sz )) {
        priv->lineSize() = sz;
        priv->onNewDataAttrReady();
        QMetaObject::invokeMethod( this, "lineSizeChanged", Qt::QueuedConnection );
    }
}

auto     IcQuickLineSeriesItem :: setLineColor( const QColor &cl ) -> void
{
    auto priv = T_PrivPtr(m_obj);
    if ( priv->lineColor() != cl ) {
        priv->lineColor() = cl;
        priv->onNewDataAttrReady();
        QMetaObject::invokeMethod( this, "lineColorChanged", Qt::QueuedConnection );
    }
}

auto     IcQuickLineSeriesItem :: viewport() const -> QRectF
{  return T_PrivPtr( m_obj )->viewport();  }

auto     IcQuickLineSeriesItem :: setViewport( const QRectF &rt ) -> void
{
    auto priv = T_PrivPtr(m_obj);
    if ( priv->viewport() != rt ) {
        priv->viewport() = rt;
        priv->onNewDataAttrReady(); // force update geometry
        QMetaObject::invokeMethod( this, "viewportChanged", Qt::QueuedConnection );
    }
}

// ============================================================================
// update the paint node
// ============================================================================
auto     IcQuickLineSeriesItem :: updatePaintNode( QSGNode *old, UpdatePaintNodeData*) -> QSGNode*
{
    IcQuickLineSeriesItemPriv *t_this = T_PrivPtr( m_obj );
    QSGNode *node = Q_NULLPTR;

    if ( ! QSizeF( this->width(), this->height() ).isValid( ) ) { return old; }

    // ------------------------------------------------------------------------
    // A. wave line data changed
    //   A-1) the wave line data is empty.
    //      A-1-1) the old is NULL, directly return.
    //      A-1-2) the old is not NULL, clear content, return
    //   A-2) the wave line data is not empty
    //      A-2-1) the old is NULL, build and upload
    //      A-2-2) the old is not NULL, upload
    // B. normally update
    //      directly return old.
    // ------------------------------------------------------------------------
    auto lst = t_this->m_ser_dat.data();
    auto sd  = t_this->m_ary_dat.data();

    if ( lst.isEmpty() && sd.vec.isEmpty() ) {
       if ( old == Q_NULLPTR ) { // A-1-2
            t_this->m_ser_dat_req_cntr.storeRelease(0);
            t_this->m_dat_attr_req_cntr.storeRelease(0);
            return old;
        } else {                  // A-1-2
            return t_this->qsgUpdSeriesDataNull( old );
        }
    } else {
       if ( old == Q_NULLPTR ) { // A-2-1
            if ( ! lst.isEmpty() ) {
                node = t_this->qsgUpnNullNode( old, lst );
            } else if ( ! sd.vec.isEmpty() ){
                node = t_this->qsgUpnNullNode( old, sd );
            } else {
                return t_this->qsgUpdSeriesDataNull( old );
            }
        } else {                  // A-2-2
            if ( ! lst.isEmpty()) {
                node = t_this->qsgUpnExistedNode( old, lst );
            } else if ( ! sd.vec.isEmpty() ) {
                node = t_this->qsgUpnExistedNode( old, sd );
            } else {
                return t_this->qsgUpdSeriesDataNull( old );
            }
        }
        return node;
    }
}

// ============================================================================
// while the geometry changed, this will be called
// ============================================================================
#if   ( QT_VERSION_MAJOR < 6 )
auto     IcQuickLineSeriesItem :: geometryChanged( const QRectF &new_geom, const QRectF &old_geom ) -> void
#else
auto     IcQuickLineSeriesItem :: geometryChange ( const QRectF &new_geom, const QRectF &old_geom ) -> void
#endif
{
    auto priv = T_PrivPtr(m_obj);
    if ( new_geom != old_geom ) {
        priv->onNewDataAttrReady();

#if   QT_VERSION_MAJOR < 6
        QQuickItem::geometryChanged( new_geom, old_geom );
#else
        QQuickItem::geometryChange ( new_geom, old_geom );
#endif
        QMetaObject::invokeMethod( this, "geomChanged", Qt::QueuedConnection );
    }
}

// ============================================================================
// pick the data at spec. hit_coord point
// ============================================================================
auto     IcQuickLineSeriesItem :: makePickInfoObjAt( const QPointF &hit_coord, PickDataMode pm ) const -> QxPack::IcQuickLineSeriesItemPickDataInfo
{
    QxPack::IcQuickLineSeriesItemPickDataInfo pi;

    QRectF bd = boundingRect();
    if ( ! bd.isValid() ) { return pi; }
    auto *priv    = T_PrivPtr( m_obj );
    auto &vw_rect = priv->viewport();
    if ( ! vw_rect.isValid() ) { return pi; }

    // ------------------------------------------------------------------------
    // estimate clicked point view rect position
    // ------------------------------------------------------------------------
    qreal hori_pct = hit_coord.x() / bd.width();
    qreal x_ofv    = hori_pct * vw_rect.width()  + vw_rect.x();
//    qreal vert_pct = hit_coord.y() / bd.height();
//    qreal y_ofv    = vert_pct * vw_rect.height() + vw_rect.y();
    bool is_found = false;
    QPair<QPointF, QPointF> pair;
    if ( ! priv->xySeriesData().isNull()) {
        pair = priv->xySeriesData().findBound( x_ofv, is_found );
    } else {
        pair = priv->arySeriesData().findBound( x_ofv, is_found );
    }
    if ( ! is_found ) { return pi; }

    // ------------------------------------------------------------------------
    // calculate information
    // ------------------------------------------------------------------------
    auto &fst = pair.first; auto &sec = pair.second;
    switch( pm ) {
    case PickDataMode_NearstDist : {
        QLineF ln_a( mapValuePointToPxPos( fst ), hit_coord );
        QLineF ln_b( mapValuePointToPxPos( sec ), hit_coord );
        QPointF rsl_pt = ( ln_a.length() <= ln_b.length()) ? fst : sec;
        pi.setPointInfo( fst, sec, rsl_pt );
    } break;

    case PickDataMode_LinearX : {
        QLineF ln_x_ofv( QPointF( x_ofv, qMin(fst.y(),sec.y())), QPointF( x_ofv, qMax(fst.y(), sec.y())));
        QLineF ln_dat( fst, sec );
        QPointF int_pt;
#if  QT_VERSION_MAJOR < 6 && QT_VERSION_MINOR < 14
        auto int_type = ln_dat.intersect( ln_x_ofv, &int_pt );
#else
        auto int_type = ln_dat.intersects( ln_x_ofv, &int_pt );
#endif
        if ( int_type == QLineF::BoundedIntersection ) {
            pi.setPointInfo( fst, sec, int_pt );
        } else {
            pi.setPointInfo( fst, sec, fst );
        }

    } break;

    default : // nearest x
        QPointF rsl_pt = ( x_ofv - fst.rx() <= sec.rx() - x_ofv ? fst : sec );
        pi.setPointInfo( fst, sec, rsl_pt );
    }

    return pi;
}

// ============================================================================
// map the value point to pixel position
// ============================================================================
auto     IcQuickLineSeriesItem :: mapValuePointToPxPos( const QPointF &pt ) const -> QPointF
{
    auto *priv = T_PrivPtr( m_obj );
    auto &vw_rect = priv->viewport();
    auto bd = boundingRect();

    qreal x_sc = ( pt.x() - vw_rect.x()) / vw_rect.width();
    qreal y_sc = ( vw_rect.y() - pt.y()) / vw_rect.height();

    return QPointF( bd.x() + x_sc * bd.width(), bd.y() + y_sc * bd.height());
}

// ============================================================================
// [ static ] register in QML engine
// ============================================================================
auto     IcQuickLineSeriesItem :: _reg() -> void
{  QxPack_IcQuickLineSeriesItem_Reg(); }

}
#endif
