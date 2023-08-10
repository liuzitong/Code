#ifndef QXPACK_IC_QUICKPARLNITEM_CXX
#define QXPACK_IC_QUICKPARLNITEM_CXX

#include "qxpack_ic_quickparlnitem.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/sys/qxpack_ic_geom2dalgor.hxx"
#include "qxpack/indcom/ui_qml_charts/qsgnode/qxpack_ic_qsgvalueaxisnode_p.hxx"
#include "qxpack/indcom/ui_qml_charts/qxpack_ic_quicklbllayoutsrc_p.hxx"
#include "qxpack/indcom/algor/qxpack_ic_algor_base.hpp"

#include <QColor>
#include <QAtomicInt>
#include <QQmlEngine>
#include <QDebug>
#include <QSizeF>

#define  LINESIZE_RANGE     QVector2D( 1.0f, 40.0f )
#define  LINESIZE_DEFAULT   (1.0f)
#define  LINESMOOTH_DEFAULT (1.0f)

namespace QxPack {


// ////////////////////////////////////////////////////////////////////////////
//
//                      private quick item object
//
// ////////////////////////////////////////////////////////////////////////////
static void QxPack_IcQuickParLnItem_Reg( )
{
    static bool is_reg = false;
    if ( ! is_reg ) {
        is_reg = true;
        qmlRegisterType<QxPack::IcQuickParLnItem>("qxpack.indcom.ui_qml_charts", 1, 0, "IcQuickParLnItem");
        qDebug() << "qxpack_ic_quickparlnitem registered.";
    }
}

#define T_PrivPtr( o )  T_ObjCast( IcQuickParLnItemPriv*, o )
class QXPACK_IC_HIDDEN  IcQuickParLnItemPriv {
    friend class IcQuickParLnItem;
private:
    IcQuickParLnItem  *m_parent;
    QAtomicInt         m_param_dat_req_cntr, m_mater_req_cntr;
    IcQSGValueAxisNode::Material m_ui_mater;
    IcQSGValueAxisNode::LnParam  m_ui_ln_param;
    QRectF  m_ui_view_port;

protected:
    QRectF  fixedBoundRect( ) const;
public :
    explicit IcQuickParLnItemPriv ( IcQuickParLnItem *pa );
    ~IcQuickParLnItemPriv();

    inline auto  lineSize()   -> float&  { return m_ui_mater.size;   }
    inline auto  lineSmooth() -> float&  { return m_ui_mater.smooth; }
    inline auto  lineColor()  -> QColor& { return m_ui_mater.color;  }
    inline auto  viewport()   -> QRectF& { return m_ui_view_port;    }
    inline auto  uiMater()    -> IcQSGValueAxisNode::Material& { return m_ui_mater;    }
    inline auto  uiLnParam()  -> IcQSGValueAxisNode::LnParam & { return m_ui_ln_param; }

    inline auto  onNewParamReady() -> void { IcAlgor::doIf( [this](){ m_param_dat_req_cntr.storeRelease(1); m_parent->update(); }, m_param_dat_req_cntr.loadAcquire() < 1 ); }
    inline auto  onNewMaterReady() -> void { IcAlgor::doIf( [this](){ m_mater_req_cntr.storeRelease(1); m_parent->update(); }, m_mater_req_cntr.loadAcquire() < 1); }

    auto  qsgUpnNullNode    ( QSGNode* ) -> QSGNode*;
    auto  qsgUpnExistedNode ( QSGNode* ) -> QSGNode*;
};

// ============================================================================
// ctor
// ============================================================================
IcQuickParLnItemPriv :: IcQuickParLnItemPriv ( IcQuickParLnItem *pa )
{
    m_parent = pa;
    m_ui_mater.size   = LINESIZE_DEFAULT;
    m_ui_mater.smooth = LINESMOOTH_DEFAULT;
    m_ui_mater.color = QColor("steelblue");
    m_ui_view_port   = QRectF( 0,0,-1,-1);
    m_ui_ln_param.setIsShowBln( false );
    m_ui_ln_param.setMinorHintStep( 0 );
    m_ui_ln_param.setMinorTickCnt ( 0 );
    m_ui_ln_param.setTickDir( IcQSGValueAxisNode::LnParam::TickDir_Both );

    // force update once
    m_param_dat_req_cntr.storeRelease(1);
    m_mater_req_cntr.storeRelease(1);
    QMetaObject::invokeMethod( m_parent, "update", Qt::QueuedConnection );
}

// ============================================================================
// dtor
// ============================================================================
IcQuickParLnItemPriv :: ~IcQuickParLnItemPriv ( )
{
}

// ============================================================================
// [ protected ] return the fixed bounding rect
// ============================================================================
auto     IcQuickParLnItemPriv :: fixedBoundRect ( ) const -> QRectF
{
    auto bd = m_parent->boundingRect();
    if ( ! m_ui_ln_param.isVertical()) {
        bd.setX( m_ui_ln_param.headRsvdPx() );
        bd.setWidth( bd.width() - m_ui_ln_param.tailRsvdPx() );
    }  else {
        bd.setY( m_ui_ln_param.tailRsvdPx() );
        bd.setHeight( bd.height() - m_ui_ln_param.headRsvdPx() );
    }
    return bd;
}

// ============================================================================
// update the geometry node
// ============================================================================
auto     IcQuickParLnItemPriv :: qsgUpnNullNode ( QSGNode *old ) -> QSGNode*
{
    IcQSGValueAxisNode *node = Q_NULLPTR;
    if ( old != Q_NULLPTR ) { return old; }
    auto bd = fixedBoundRect();
    if ( ! bd.isValid()) { return old; }

    // create geometry node
    node = new IcQSGValueAxisNode ( m_ui_mater );
    if ( node == Q_NULLPTR ) { return node; }
    node->setFlag( QSGNode::OwnedByParent, true ); // parent manage node's life cycle

    // generate the render data
    node->qsg_updateMaterial( m_ui_mater );

    node->qsg_updateGeometry( bd, m_ui_ln_param, m_ui_view_port, m_ui_mater, nullptr );
    m_mater_req_cntr.storeRelease(0);
    m_param_dat_req_cntr.storeRelease(0);

    return node;
}

auto     IcQuickParLnItemPriv :: qsgUpnExistedNode (QSGNode *old ) -> QSGNode*
{
    IcQSGValueAxisNode *node = Q_NULLPTR;

    if ( old == Q_NULLPTR ) { return old; }
    node = static_cast< IcQSGValueAxisNode *>( old );
    auto bd = fixedBoundRect();
    if ( ! bd.isValid() ) { return old; }

    if ( m_mater_req_cntr.loadAcquire() > 0 ) {
        m_mater_req_cntr.storeRelease(0);
        node->qsg_updateMaterial( m_ui_mater );
    }

    if ( m_param_dat_req_cntr.loadAcquire() > 0 ) {
        m_param_dat_req_cntr.storeRelease(0);
        node->qsg_updateGeometry( bd, m_ui_ln_param, m_ui_view_port, m_ui_mater, nullptr );
    }

    return node;
}



// ////////////////////////////////////////////////////////////////////////////
//                      quick item
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcQuickParLnItem :: IcQuickParLnItem ( QQuickItem *pa ) : QQuickItem( pa )
{
    m_obj = qxpack_ic_new( IcQuickParLnItemPriv, this );
    setFlags ( QQuickItem::ItemHasContents );
    setClip(true);
}

// ============================================================================
// dtor
// ============================================================================
IcQuickParLnItem :: ~IcQuickParLnItem ( )
{
    qxpack_ic_delete( m_obj, IcQuickParLnItemPriv );
}

// ////////////////////////////////////////////////////////////////////////////
// access the line propertys
// ////////////////////////////////////////////////////////////////////////////
auto     IcQuickParLnItem :: lineSizeRange() const -> QVector2D
{ return LINESIZE_RANGE; }

auto     IcQuickParLnItem :: lineSize() const -> float
{ return T_PrivPtr( m_obj )->lineSize(); }

auto     IcQuickParLnItem :: setLineSize( float sz ) -> void
{
    auto *priv = T_PrivPtr( m_obj );
    if ( sz >= 0.0f  &&  ! qFuzzyCompare( priv->lineSize(), sz )) {
        priv->lineSize() = sz;
        priv->onNewParamReady();   // force update geometry
        priv->onNewMaterReady();
        QMetaObject::invokeMethod( this, "lineSizeChanged", Qt::QueuedConnection );
    }
}

auto     IcQuickParLnItem :: lineColor() const -> QColor
{ return T_PrivPtr( m_obj )->lineColor(); }

auto     IcQuickParLnItem :: setLineColor( const QColor &cl ) -> void
{
    auto *priv = T_PrivPtr( m_obj );
    if ( priv->lineColor() != cl ) {
        priv->lineColor() = cl;
        priv->onNewMaterReady();   // just render data only
        QMetaObject::invokeMethod( this, "lineColorChanged", Qt::QueuedConnection );
    }
}

auto     IcQuickParLnItem :: viewport() const -> QRectF
{  return T_PrivPtr( m_obj )->viewport();  }

auto     IcQuickParLnItem :: setViewport( const QRectF &rt ) -> void
{
    auto *priv = T_PrivPtr( m_obj );
    if ( priv->viewport() != rt ) {
        priv->viewport() = rt;
        priv->onNewParamReady();   // force update geometry
        QMetaObject::invokeMethod( this, "viewportChanged", Qt::QueuedConnection );
    }
}

auto     IcQuickParLnItem :: tickCount() const -> int
{  return T_PrivPtr( m_obj )->uiLnParam().majorTickCnt(); }

auto     IcQuickParLnItem :: setTickCount( int cnt ) -> void
{
    auto *priv = T_PrivPtr( m_obj );
    if ( priv->uiLnParam().majorTickCnt() != cnt ) {
        priv->uiLnParam().setMajorTickCnt( cnt );
        priv->onNewParamReady();
        QMetaObject::invokeMethod( this, "tickCountChanged", Qt::QueuedConnection );
    }
}

auto     IcQuickParLnItem :: vertical() const -> bool
{  return T_PrivPtr( m_obj )->uiLnParam().isVertical(); }

auto     IcQuickParLnItem :: setVertical( bool sw ) -> void
{
    auto *priv = T_PrivPtr( m_obj );
    if ( priv->uiLnParam().isVertical() != sw ) {
        priv->uiLnParam().setVertical( sw );
        priv->onNewParamReady();
        QMetaObject::invokeMethod( this, "verticalChanged", Qt::QueuedConnection );
    }
}

auto     IcQuickParLnItem :: min() const -> qreal
{  return T_PrivPtr( m_obj )->uiLnParam().min();  }

auto     IcQuickParLnItem :: max() const -> qreal
{  return T_PrivPtr( m_obj )->uiLnParam().max(); }

auto     IcQuickParLnItem :: setMin( qreal v ) -> void
{
    auto *priv = T_PrivPtr( m_obj );
    if ( ! qFuzzyCompare( priv->uiLnParam().min(), v ) ) {
        priv->uiLnParam().setMin( v );
        priv->uiLnParam().setBlnMin( v );
        priv->onNewParamReady();
        QMetaObject::invokeMethod( this, "rangeChanged", Qt::QueuedConnection );
    }
}

auto     IcQuickParLnItem :: setMax( qreal v ) -> void
{
    auto *priv = T_PrivPtr( m_obj );
    if ( ! qFuzzyCompare( priv->uiLnParam().max(), v )) {
        priv->uiLnParam().setMax( v );
        priv->uiLnParam().setBlnMax( v );
        priv->onNewParamReady();
        QMetaObject::invokeMethod( this, "rangeChanged", Qt::QueuedConnection );
    }
}

// since 0.5.11
auto     IcQuickParLnItem :: headRsvdPx() const -> qreal
{ return T_PrivPtr(m_obj)->uiLnParam().headRsvdPx(); }

auto     IcQuickParLnItem :: tailRsvdPx() const -> qreal
{ return T_PrivPtr(m_obj)->uiLnParam().tailRsvdPx(); }

auto     IcQuickParLnItem :: setHeadRsvdPx( qreal n ) -> void
{
    auto *priv = T_PrivPtr( m_obj );
    if ( n < 0 ) { n = 0; }
    if ( ! qFuzzyCompare( n, priv->uiLnParam().headRsvdPx() ) ) {
        priv->uiLnParam().setHeadRsvdPx( n );
        priv->onNewParamReady(); // redraw it
        QMetaObject::invokeMethod( this, "headRsvdPxChanged", Qt::QueuedConnection );
    }
}

auto     IcQuickParLnItem :: setTailRsvdPx( qreal n ) -> void
{
    auto *priv = T_PrivPtr( m_obj );
    if ( n < 0 ) { n = 0; }
    if ( ! qFuzzyCompare( n, priv->uiLnParam().tailRsvdPx())) {
        priv->uiLnParam().setTailRsvdPx( n );
        priv->onNewParamReady(); // redraw it
        QMetaObject::invokeMethod( this, "tailRsvdPxChanged", Qt::QueuedConnection );
    }
}

auto     IcQuickParLnItem :: showHeadLine() const -> bool
{   return T_PrivPtr( m_obj )->uiLnParam().showFirstMajor(); }

auto     IcQuickParLnItem :: showTailLine() const -> bool
{   return T_PrivPtr( m_obj )->uiLnParam().showLastMajor(); }

auto     IcQuickParLnItem :: setShowHeadLine( bool sw ) -> void
{
    auto *priv = T_PrivPtr( m_obj );
    if ( priv->uiLnParam().showFirstMajor() != sw ) {
        priv->uiLnParam().setShowFirstMajor( sw );
        priv->onNewParamReady();
        QMetaObject::invokeMethod( this, "showHeadLineChanged", Qt::QueuedConnection );
    }
}

auto     IcQuickParLnItem :: setShowTailLine( bool sw ) -> void
{
    auto *priv = T_PrivPtr( m_obj );
    if ( priv->uiLnParam().showLastMajor() != sw ) {
        priv->uiLnParam().setShowLastMajor( sw );
        priv->onNewParamReady();
        QMetaObject::invokeMethod( this, "showTailLineChanged", Qt::QueuedConnection );
    }
}

// ============================================================================
// update the paint node ( QSG thread )
// ============================================================================
auto     IcQuickParLnItem :: updatePaintNode( QSGNode *old, UpdatePaintNodeData*) -> QSGNode*
{
    auto *priv   = T_PrivPtr( m_obj );
    QSGNode *node = Q_NULLPTR;

    if ( ! QSizeF( this->width(), this->height() ).isValid( ) ) { return old; }

    if ( old == Q_NULLPTR ) {
        node = priv->qsgUpnNullNode( old );
    } else {
        node = priv->qsgUpnExistedNode( old );
    }

    return node;
}

// ============================================================================
// while the geometry changed, this will be called
// ============================================================================
#if   QT_VERSION_MAJOR < 6
auto     IcQuickParLnItem :: geometryChanged( const QRectF &new_geom, const QRectF &old_geom ) -> void
#else
auto     IcQuickParLnItem :: geometryChange ( const QRectF &new_geom, const QRectF &old_geom ) -> void
#endif
{
    auto *priv = T_PrivPtr( m_obj );
    if ( ! priv->uiLnParam().isVertical()) {
        priv->uiLnParam().setMajorSize( new_geom.height() / 2 );
    } else {
        priv->uiLnParam().setMajorSize( new_geom.width() / 2 );
    }
    priv->onNewParamReady(); // re-generate the vertex data
#if    QT_VERSION_MAJOR < 6
    QQuickItem::geometryChanged( new_geom, old_geom );
#else
    QQuickItem::geometryChange( new_geom, old_geom );
#endif
}


// ============================================================================
// [ static ] register in QML engine
// ============================================================================
auto     IcQuickParLnItem :: _reg() -> void
{  QxPack_IcQuickParLnItem_Reg(); }

}


#endif
