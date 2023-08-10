#ifndef QXPACK_IC_QUICKLINESERIESITEM_CXX
#define QXPACK_IC_QUICKLINESERIESITEM_CXX

#include "qxpack_ic_quicklineseriesitem.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/ui_qml_base/qxpack_ic_quickxyseriesdata.hxx"
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

#define  LINESIZE_RANGE      QVector2D( 1.0f, 40.0f )
#define  LINESMOOTH_RANGE    QVector2D( 0.0f, 1.0f )
#define  LINESMOOTH_DEFAULT  (0.98f)
#define  LINESIZE_DEFAULT    (1.0f)

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//                      inner QObject
// ////////////////////////////////////////////////////////////////////////////
IcQuickLineSeriesItemPickDataInfo :: IcQuickLineSeriesItemPickDataInfo( )
{
   m_ext = nullptr;
}

IcQuickLineSeriesItemPickDataInfo :: IcQuickLineSeriesItemPickDataInfo( const IcQuickLineSeriesItemPickDataInfo &ot )
{
    m_pt_lo = ot.m_pt_lo; m_pt_up = ot.m_pt_up;
    m_pt_val = ot.m_pt_val;
}

IcQuickLineSeriesItemPickDataInfo &  IcQuickLineSeriesItemPickDataInfo :: operator = ( const IcQuickLineSeriesItemPickDataInfo &ot )
{
    m_pt_lo  = ot.m_pt_lo; m_pt_up = ot.m_pt_up;
    m_pt_val = ot.m_pt_val;
    return *this;
}

IcQuickLineSeriesItemPickDataInfo :: ~IcQuickLineSeriesItemPickDataInfo( )
{
}

bool   IcQuickLineSeriesItemPickDataInfo :: operator == ( const IcQuickLineSeriesItemPickDataInfo &ot ) const
{
    return m_pt_lo == ot.m_pt_lo && m_pt_up == ot.m_pt_up && m_pt_val == ot.m_pt_val;
}

void   IcQuickLineSeriesItemPickDataInfo :: setPointInfo( const QPointF &lo, const QPointF &hi, const QPointF &v )
{
    m_pt_lo = lo; m_pt_up = hi; m_pt_val = v;
}


// ////////////////////////////////////////////////////////////////////////////
//
//                      private quick item object
//
// ////////////////////////////////////////////////////////////////////////////
static void QxPack_IcQuickLineSeriesItem_Reg( )
{
    static bool is_reg = false;
    if ( ! is_reg ) {
        is_reg = true;
        qmlRegisterType<QxPack::IcQuickLineSeriesItem>("qxpack.indcom.ui_qml_charts", 1, 0, "IcQuickLineSeriesItem");
        qRegisterMetaType<QxPack::IcQuickLineSeriesItemPickDataInfo>();
        QMetaType::registerEqualsComparator<QxPack::IcQuickLineSeriesItemPickDataInfo>();
        qDebug() << "qxpack_ic_quicklineseriesitem registered."; // nw: 2019/05/07 added
    }
}

#define T_PrivPtr( o )  T_ObjCast( IcQuickLineSeriesItemPriv*, o )
class QXPACK_IC_HIDDEN  IcQuickLineSeriesItemPriv {
    friend class IcQuickLineSeriesItem;
private:
    IcQuickXYSeriesData    m_ser_dat;
    QAtomicInt             m_ser_dat_req_cntr, m_mater_req_cntr;
    IcQuickLineSeriesItem *m_parent;

    IcQSGLineSeriesNode::Material m_ui_mater;
    QRectF  m_ui_view_port;

public :
    explicit IcQuickLineSeriesItemPriv ( IcQuickLineSeriesItem *pa );
    ~IcQuickLineSeriesItemPriv();

    inline float &   lineSize()   { return m_ui_mater.size;   }
    inline float &   lineSmooth() { return m_ui_mater.smooth; }
    inline QColor&   lineColor()  { return m_ui_mater.color;  }

    inline QRectF &  viewport()   { return m_ui_view_port; }
    inline IcQSGLineSeriesNode::Material &  uiMater() { return m_ui_mater; }

    void   setXYSeriesData( QObject* );
    inline IcQuickXYSeriesData&  xySeriesData() { return m_ser_dat; }

    void  onNewDataReady();
    void  onNewMaterReady();


    QSGNode*   qsgUpdXYSeriesDataNull ( QSGNode* );
    QSGNode*   qsgUpnNullNode    ( QSGNode*, const QList<QPointF>& );
    QSGNode*   qsgUpnExistedNode ( QSGNode*, const QList<QPointF>& );
};

// ============================================================================
// ctor
// ============================================================================
IcQuickLineSeriesItemPriv :: IcQuickLineSeriesItemPriv ( IcQuickLineSeriesItem *pa )
{
    m_parent = pa;
    m_ui_mater.size = LINESIZE_DEFAULT; m_ui_mater.smooth = LINESMOOTH_DEFAULT; m_ui_mater.color = QColor("steelblue");
    m_ui_view_port = QRectF( 0,0,-1,-1);

    // ------------------------------------------------------------------------
    // DO NOT directly call upate() due to read image source
    // ------------------------------------------------------------------------
    QObject::connect(
        & m_ser_dat, & IcQuickXYSeriesData::dataChanged, & m_ser_dat,
        [this]() { this->onNewDataReady(); }
    );

    // force update once
    m_ser_dat_req_cntr.storeRelease(1);
    m_mater_req_cntr.storeRelease(1);
    QMetaObject::invokeMethod( m_parent, "update", Qt::QueuedConnection );
}

// ============================================================================
// dtor
// ============================================================================
IcQuickLineSeriesItemPriv :: ~IcQuickLineSeriesItemPriv ( )
{
    m_ser_dat.blockSignals(true);
}

// ============================================================================
// set the wave line data
// ============================================================================
void                    IcQuickLineSeriesItemPriv :: setXYSeriesData( QObject *obj )
{
    IcQuickXYSeriesData *dat_src = qobject_cast< IcQuickXYSeriesData*>( obj );
    if ( dat_src != Q_NULLPTR ) {
        m_ser_dat.attach( *dat_src ); // attach to another
    } else {
        IcQuickXYSeriesData tmp;
        m_ser_dat.attach( tmp );      // drop current
    }

    m_ser_dat_req_cntr.storeRelease(0);
    emit m_parent->xySeriesDataChanged();

    onNewDataReady(); // force read data
}

// ============================================================================
// handle while new data arraived
// ============================================================================
void                    IcQuickLineSeriesItemPriv :: onNewDataReady()
{
    if ( m_ser_dat_req_cntr.loadAcquire() < 1 ) {
        m_ser_dat_req_cntr.storeRelease(1);
        m_parent->update(); // shedule a paint
    }
}

// ============================================================================
// handle while new material changed
// ============================================================================
void                    IcQuickLineSeriesItemPriv :: onNewMaterReady()
{
    if ( m_mater_req_cntr.loadAcquire() < 1 ) {
        m_mater_req_cntr.storeRelease(1);
        m_parent->update();
    }
}

// ============================================================================
// update the geometry node
// ============================================================================
QSGNode*                IcQuickLineSeriesItemPriv :: qsgUpdXYSeriesDataNull( QSGNode *old )
{
    IcQSGLineSeriesNode *node = Q_NULLPTR;
    if ( old == Q_NULLPTR ) { return old; }

    node = static_cast< IcQSGLineSeriesNode *>( old );

    if ( m_mater_req_cntr.loadAcquire() > 0 ) {
        m_mater_req_cntr.storeRelease(0);
        node->qsg_updateMaterial( m_ui_mater );
    }

    if ( m_ser_dat_req_cntr.loadAcquire() > 0 ) {
        m_ser_dat_req_cntr.storeRelease(0);
        node->qsg_updateGeometry( m_parent->boundingRect(), QList<QPointF>(), m_ui_view_port, m_ui_mater );
    }

    return node;
}

QSGNode*                IcQuickLineSeriesItemPriv :: qsgUpnNullNode ( QSGNode *old, const QList<QPointF> &lst )
{
    IcQSGLineSeriesNode *node = Q_NULLPTR;
    if ( old != Q_NULLPTR ) { return old; }

    // ------------------------------------------------------------------------
    // create geometry node
    // ------------------------------------------------------------------------
    node = new IcQSGLineSeriesNode ( m_ui_mater );
    if ( node == Q_NULLPTR ) { return node; }
    node->setFlag( QSGNode::OwnedByParent, true ); // parent manage node's life cycle

    // ------------------------------------------------------------------------
    // generate the render data
    // ------------------------------------------------------------------------
    node->qsg_updateMaterial( m_ui_mater );
    node->qsg_updateGeometry( m_parent->boundingRect(), lst, m_ui_view_port, m_ui_mater );
    m_mater_req_cntr.storeRelease(0);
    m_ser_dat_req_cntr.storeRelease(0);

    return node;
}

QSGNode*                IcQuickLineSeriesItemPriv :: qsgUpnExistedNode (QSGNode *old, const QList<QPointF> &lst )
{
    IcQSGLineSeriesNode *node = Q_NULLPTR;

    if ( old == Q_NULLPTR ) { return old; }
    node = static_cast< IcQSGLineSeriesNode *>( old );

    if ( m_mater_req_cntr.loadAcquire() > 0 ) {
        m_mater_req_cntr.storeRelease(0);
        node->qsg_updateMaterial( m_ui_mater );
    }

    if ( m_ser_dat_req_cntr.loadAcquire() > 0 ) {
        m_ser_dat_req_cntr.storeRelease(0);
        node->qsg_updateGeometry( m_parent->boundingRect(), lst, m_ui_view_port, m_ui_mater );
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

    // this is needed. make the object draw
    this->setFlags ( QQuickItem::ItemHasContents );
    this->setClip(true);
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
QObject*                IcQuickLineSeriesItem :: xySeriesData() const
{ return qobject_cast<QObject*>( & T_PrivPtr( m_obj )->xySeriesData() );}

void                    IcQuickLineSeriesItem :: setXYSeriesData( QObject *dat )
{  T_PrivPtr( m_obj )->setXYSeriesData( dat ); }

// ============================================================================
// access the line propertys
// ============================================================================
float                   IcQuickLineSeriesItem :: lineSize() const
{ return T_PrivPtr( m_obj )->lineSize(); }

QColor                  IcQuickLineSeriesItem :: lineColor() const
{ return T_PrivPtr( m_obj )->lineColor(); }

QVector2D               IcQuickLineSeriesItem :: lineSizeRange() const
{ return LINESIZE_RANGE; }

void                    IcQuickLineSeriesItem :: setLineSize( float sz )
{
    if ( sz >= 0.0f  &&  ! qFuzzyCompare( T_PrivPtr( m_obj )->lineSize(), sz )) {
        T_PrivPtr( m_obj )->lineSize() = sz;
        T_PrivPtr( m_obj )->onNewDataReady();
        T_PrivPtr( m_obj )->onNewMaterReady();
        QMetaObject::invokeMethod( this, "lineSizeChanged", Qt::QueuedConnection );
    }
}

void                    IcQuickLineSeriesItem :: setLineColor( const QColor &cl )
{
    if ( T_PrivPtr( m_obj )->lineColor() != cl ) {
        T_PrivPtr( m_obj )->lineColor() = cl;
        T_PrivPtr( m_obj )->onNewMaterReady();
        QMetaObject::invokeMethod( this, "lineColorChanged", Qt::QueuedConnection );
    }
}

QRectF                  IcQuickLineSeriesItem :: viewport() const
{  return T_PrivPtr( m_obj )->viewport();  }

void                    IcQuickLineSeriesItem :: setViewport( const QRectF &rt )
{
    if ( T_PrivPtr( m_obj )->viewport() != rt ) {
        T_PrivPtr( m_obj )->viewport() = rt;
        T_PrivPtr( m_obj )->onNewDataReady(); // force update geometry
        QMetaObject::invokeMethod( this, "viewportChanged", Qt::QueuedConnection );
    }
}

// ============================================================================
// update the paint node
// ============================================================================
QSGNode*                IcQuickLineSeriesItem :: updatePaintNode( QSGNode *old, UpdatePaintNodeData*)
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
    QList<QPointF> lst = t_this->m_ser_dat.data();
    if ( lst.isEmpty()) {
        if ( old == Q_NULLPTR ) { // A-1-2
            t_this->m_mater_req_cntr.storeRelease(0);
            t_this->m_ser_dat_req_cntr.storeRelease(0);
            return old;
        } else {                  // A-1-2
            return t_this->qsgUpdXYSeriesDataNull( old );
        }
    } else {
        if ( old == Q_NULLPTR ) { // A-2-1
            node = t_this->qsgUpnNullNode( old, lst );
        } else {                  // A-2-2
            node = t_this->qsgUpnExistedNode( old, lst );
        }
        return node;
    }
}

// ============================================================================
// while the geometry changed, this will be called
// ============================================================================
void                    IcQuickLineSeriesItem :: geometryChanged( const QRectF &new_geom, const QRectF &old_geom )
{
    if ( new_geom != old_geom ) {
        T_PrivPtr( m_obj )->onNewMaterReady();
        T_PrivPtr( m_obj )->onNewDataReady(); // regenerate the vertex data
        QQuickItem::geometryChanged( new_geom, old_geom );
        QMetaObject::invokeMethod( this, "geomChanged", Qt::QueuedConnection );
    }
}

// ============================================================================
// pick the data at spec. hit_coord point
// ============================================================================
QxPack::IcQuickLineSeriesItemPickDataInfo  IcQuickLineSeriesItem :: makePickInfoObjAt( const QPointF &hit_coord, PickDataMode pm ) const
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
    QPair<QPointF, QPointF> pair = priv->xySeriesData().findBound( x_ofv, is_found );
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
        auto int_type = ln_dat.intersect( ln_x_ofv, &int_pt );
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
QPointF                 IcQuickLineSeriesItem :: mapValuePointToPxPos( const QPointF &pt ) const
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
void                    IcQuickLineSeriesItem :: _reg()
{  QxPack_IcQuickLineSeriesItem_Reg(); }

}
#endif
