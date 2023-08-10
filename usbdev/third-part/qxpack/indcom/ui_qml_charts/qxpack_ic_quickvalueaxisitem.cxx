#ifndef QXPACK_IC_QUICKVALUEAXISITEM_CXX
#define QXPACK_IC_QUICKVALUEAXISITEM_CXX

#include "qxpack_ic_quickvalueaxisitem.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/sys/qxpack_ic_geom2dalgor.hxx"
#include "qxpack/indcom/ui_qml_charts/qsgnode/qxpack_ic_qsgvalueaxisnode_p.hxx"
#include "qxpack/indcom/ui_qml_charts/qxpack_ic_quicklbllayoutsrc_p.hxx"
#include "qxpack/indcom/algor/qxpack_ic_algor_base.hpp"

#include <QColor>
#include <QAtomicInt>
#include <QQmlEngine>
#include <QDebug>
#include <QQuickPaintedItem>
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QFont>
#include <QFontMetricsF>
#include <QSizeF>

//#if QXPACK_IC_QT_VER_INT == 597
//#include <QtQuick/5.9.7/QtQuick/private/qquicktext_p.h>
//#else
//# error "miss QXPACK_IC_QT_VER_INT"
//#endif

#define  LINESIZE_RANGE     QVector2D( 1.0f, 10.0f )
#define  LINESIZE_DEFAULT   (1.0f)
#define  LINESMOOTH_DEFAULT (1.0f)


namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
//                 Label Text Drawer
// this class used to label all ticks by value
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN  IcQuickValueAxisItemText : public QQuickPaintedItem {
    Q_OBJECT
private:
    QAtomicInt             m_upd_req_cntr;
    IcQuickLblLayoutSrc    m_lbl_src;
    QFont  m_font;  QColor m_color;

    IcQSGValueAxisNode::LnParam  *m_ln_par;
    int   m_lbl_precision, m_lbl_dist_ofv;
    bool  m_disp_start_tick_lbl, m_disp_minor_tick_lbl, m_disp_minor_hint_lbl;

protected:
Q_SLOT void  lblSrc_onDataChanged() { reqUpdate(); }
       auto  paint_hori( const QRectF &bd, QPainter *) -> void;
       auto  paint_vert( const QRectF &bd, QPainter *) -> void;
       auto  fixedBoundRect() const -> QRectF;
public :
    explicit IcQuickValueAxisItemText( IcQSGValueAxisNode::LnParam *, QQuickItem* pa = Q_NULLPTR );
    virtual ~IcQuickValueAxisItemText( ) Q_DECL_OVERRIDE;

    virtual auto  paint( QPainter *) -> void Q_DECL_OVERRIDE;

#if   QT_VERSION_MAJOR < 6
    virtual auto  geometryChanged( const QRectF &newGeometry, const QRectF &oldGeometry ) -> void Q_DECL_OVERRIDE;
#else
    virtual auto  geometryChange ( const QRectF &newGeometry, const QRectF &oldGeometry ) -> void Q_DECL_OVERRIDE;
#endif
            auto  reqUpdate() -> void;

    inline auto  lblSrc() -> IcQuickLblLayoutSrc & { return m_lbl_src; }
    inline auto  font()  const -> const QFont & { return m_font;  }
    inline auto  color() const -> const QColor& { return m_color; }
    inline auto  dispStartTickLbl() const -> bool { return m_disp_start_tick_lbl; }
    inline auto  dispMinorTickLbl() const -> bool { return m_disp_minor_tick_lbl; }
    inline auto  dispMinorHintLbl() const -> bool { return m_disp_minor_hint_lbl; }
    inline auto  lblPrecision()     const -> int  { return m_lbl_precision; }
    inline auto  lblDistOfv()       const -> int  { return m_lbl_dist_ofv; }
    inline auto  setFont ( const QFont  &f )    -> void { IcAlgor::doIf( [this,&f](){ m_font = f; reqUpdate(); }, m_font != f ); }    // { if ( m_font != f ) { m_font = f;  reqUpdate(); }}
    inline auto  setColor( const QColor &c )    -> void { IcAlgor::doIf( [this,&c](){ m_color= c; reqUpdate(); }, m_color!= c ); }    // { if ( m_color!= c ) { m_color = c; reqUpdate(); }}
    inline auto  setDispStartTickLbl( bool sw ) -> void { IcAlgor::doIf( [this,sw](){ m_disp_start_tick_lbl = sw; reqUpdate(); }, m_disp_start_tick_lbl != sw ); } // { if ( m_disp_start_tick_lbl != sw ) { m_disp_start_tick_lbl = sw; reqUpdate(); }}
    inline auto  setDispMinorTickLbl( bool sw ) -> void { IcAlgor::doIf( [this,sw](){ m_disp_minor_tick_lbl = sw; reqUpdate(); }, m_disp_minor_tick_lbl != sw ); } // { if ( m_disp_minor_tick_lbl != sw ) { m_disp_minor_tick_lbl= sw; reqUpdate(); }}
    inline auto  setDispMinorHintLbl( bool sw ) -> void { IcAlgor::doIf( [this,sw](){ m_disp_minor_hint_lbl = sw; reqUpdate(); }, m_disp_minor_hint_lbl != sw ); } // { if ( m_disp_minor_hint_lbl != sw ) { m_disp_minor_hint_lbl= sw; reqUpdate(); }}
    inline auto  setLblPrecision( int n )  -> void { IcAlgor::doIf( [this,n](){ m_lbl_precision = n; reqUpdate(); }, m_lbl_precision != n ); }  // { if ( m_lbl_precision != n ) { m_lbl_precision = n; reqUpdate(); }}
    inline auto  setLblDistOfv  ( int n )  -> void { IcAlgor::doIf( [this,n](){ m_lbl_dist_ofv  = n; reqUpdate(); }, m_lbl_dist_ofv  != n ); }  // { if ( m_lbl_dist_ofv  != n ) { m_lbl_dist_ofv  = n; reqUpdate(); }}
};

// ============================================================================
// ctor
// ============================================================================
IcQuickValueAxisItemText :: IcQuickValueAxisItemText ( IcQSGValueAxisNode::LnParam *ln_pa, QQuickItem *pa )
    : QQuickPaintedItem( pa ), m_ln_par( ln_pa ),
    m_disp_start_tick_lbl( true ), m_disp_minor_tick_lbl( false ), m_disp_minor_hint_lbl( false )
{
    //setRenderTarget( QQuickPaintedItem::FramebufferObject );

    m_upd_req_cntr.storeRelease(0);
    m_color = QColor(0,0,0);
    m_lbl_precision = 2;  m_lbl_dist_ofv = 0;

    QObject::connect( &m_lbl_src, SIGNAL(dataChanged()),this, SLOT(lblSrc_onDataChanged()));
}

// ============================================================================
// dtor
// ============================================================================
IcQuickValueAxisItemText :: ~IcQuickValueAxisItemText ( )
{
    m_lbl_src.blockSignals(true);
}

// ============================================================================
// handle require of the update
// ============================================================================
auto     IcQuickValueAxisItemText :: reqUpdate() -> void
{
    if ( m_upd_req_cntr.loadAcquire() < 1 ) {
        m_upd_req_cntr.storeRelease(1);
        this->update();
    }
}

// ============================================================================
// geometry changed
// ============================================================================
#if     QT_VERSION_MAJOR > 5
auto     IcQuickValueAxisItemText :: geometryChange ( const QRectF &newGeometry, const QRectF &oldGeometry ) -> void
#else
auto     IcQuickValueAxisItemText :: geometryChanged( const QRectF &newGeometry, const QRectF &oldGeometry ) -> void
#endif
{
#if  QT_VERSION_MAJOR < 6
    QQuickPaintedItem::geometryChanged( newGeometry, oldGeometry );
#else
    QQuickPaintedItem::geometryChange( newGeometry, oldGeometry );
#endif
    reqUpdate();
}

// ============================================================================
// paint horizontal
// ============================================================================
auto     IcQuickValueAxisItemText :: paint_hori( const QRectF &bd, QPainter *painter ) -> void
{
    QFontMetricsF font_metric( m_font, painter->device() );

    auto org_bd    = boundingRect();
    auto start_idx = ( m_disp_start_tick_lbl ? m_lbl_src.startIdx() : m_lbl_src.startIdx() + 1 );

    switch ( m_ln_par->tickDir() ) {
    case IcQSGValueAxisNode::LnParam::TickDir_CCW : {
        auto pos_h = org_bd.height() * 0.5 - m_ln_par->majorSize() - m_lbl_dist_ofv;
        for ( qint64 i = start_idx; i <= m_lbl_src.endIdx(); i ++ ) {
            auto tk_type = IcQuickLblLayoutSrc::TickType_Major;
            auto pct_pos = m_lbl_src.viewPctPosAt( i, &tk_type );
            if ( ! m_disp_minor_tick_lbl && tk_type == IcQuickLblLayoutSrc::TickType_Minor ) { continue; }
            if ( ! m_disp_minor_hint_lbl && tk_type == IcQuickLblLayoutSrc::TickType_MinorHint ) { continue; }

            auto num_str = QString::number( m_lbl_src.valPosAt( i ), 'f', m_lbl_precision );
            painter->drawText( QPointF( pct_pos * bd.width() + bd.x() - font_metric.boundingRect( num_str ).width() * 0.5, pos_h ), num_str );
         }
    } break;

    case IcQSGValueAxisNode::LnParam::TickDir_CW  : {
        auto pos_h = org_bd.height() * 0.5 + m_ln_par->majorSize() + font_metric.ascent() + m_lbl_dist_ofv;
        for ( qint64 i = start_idx; i <= m_lbl_src.endIdx(); i ++ ) {
            auto tk_type = IcQuickLblLayoutSrc::TickType_Major;
            auto pct_pos = m_lbl_src.viewPctPosAt( i, &tk_type );
            if ( ! m_disp_minor_tick_lbl && tk_type == IcQuickLblLayoutSrc::TickType_Minor ) { continue; }
            if ( ! m_disp_minor_hint_lbl && tk_type == IcQuickLblLayoutSrc::TickType_MinorHint ) { continue; }

            auto num_str = QString::number( m_lbl_src.valPosAt( i ), 'f', m_lbl_precision );
            painter->drawText( QPointF( pct_pos * bd.width() + bd.x() - font_metric.boundingRect( num_str ).width() * 0.5, pos_h ), num_str );
         }
    } break;

    default:
        auto pos_ccw = org_bd.height() * 0.5 - m_ln_par->majorSize() - m_lbl_dist_ofv;
        auto pos_cw  = org_bd.height() * 0.5 + m_ln_par->majorSize() + font_metric.ascent() + m_lbl_dist_ofv;
        for ( qint64 i = start_idx; i <= m_lbl_src.endIdx(); i ++ ) {
            auto tk_type = IcQuickLblLayoutSrc::TickType_Major;
            auto pct_pos = m_lbl_src.viewPctPosAt( i, &tk_type );

            if ( ! m_disp_minor_tick_lbl && tk_type == IcQuickLblLayoutSrc::TickType_Minor ) { continue; }
            if ( ! m_disp_minor_hint_lbl && tk_type == IcQuickLblLayoutSrc::TickType_MinorHint ) { continue; }

            auto num_str = QString::number( m_lbl_src.valPosAt( i ), 'f', m_lbl_precision );
            painter->drawText( QPointF( pct_pos * bd.width() + bd.x() - font_metric.boundingRect( num_str ).width() * 0.5, pos_ccw ), num_str );
            painter->drawText( QPointF( pct_pos * bd.width() + bd.x() - font_metric.boundingRect( num_str ).width() * 0.5, pos_cw  ), num_str );
        }
    }
}

// ============================================================================
// paint vertically
// ============================================================================
auto     IcQuickValueAxisItemText :: paint_vert( const QRectF &bd, QPainter *painter ) -> void
{
    QFontMetricsF font_metric( m_font, painter->device() );

    auto org_bd       = boundingRect();
    auto font_h_patch = ( font_metric.ascent() ) * 0.35;
    auto start_idx    = ( m_disp_start_tick_lbl ? m_lbl_src.startIdx() : m_lbl_src.startIdx() + 1 );

    switch ( m_ln_par->tickDir() ) {
    case IcQSGValueAxisNode::LnParam::TickDir_CCW : {
        auto pos_ccw = org_bd.width() * 0.5 - m_ln_par->majorSize() - m_lbl_dist_ofv;
        for ( qint64 i = start_idx; i <= m_lbl_src.endIdx(); i ++ ) {
            auto tk_type = IcQuickLblLayoutSrc::TickType_Major;
            auto pct_pos = m_lbl_src.viewPctPosAt( i, &tk_type );
            if ( ! m_disp_minor_tick_lbl && tk_type == IcQuickLblLayoutSrc::TickType_Minor ) { continue; }
            if ( ! m_disp_minor_hint_lbl && tk_type == IcQuickLblLayoutSrc::TickType_MinorHint ) { continue; }

            auto num_str = QString::number( m_lbl_src.valPosAt( i ), 'f', m_lbl_precision );
            auto str_bd  = font_metric.boundingRect( num_str );
            painter->drawText( QPointF( pos_ccw - str_bd.width(), org_bd.height() - pct_pos * bd.height() - bd.y() + font_h_patch ), num_str );
         }
    } break;

    case IcQSGValueAxisNode::LnParam::TickDir_CW  : {
        auto pos_cw = org_bd.width() * 0.5 + m_ln_par->majorSize() + m_lbl_dist_ofv;
        for ( qint64 i = start_idx; i <= m_lbl_src.endIdx(); i ++ ) {
            auto tk_type = IcQuickLblLayoutSrc::TickType_Major;
            qreal pct_pos = m_lbl_src.viewPctPosAt( i, &tk_type );
            if ( ! m_disp_minor_tick_lbl && tk_type == IcQuickLblLayoutSrc::TickType_Minor ) { continue; }
            if ( ! m_disp_minor_hint_lbl && tk_type == IcQuickLblLayoutSrc::TickType_MinorHint ) { continue; }

            auto num_str = QString::number( m_lbl_src.valPosAt( i ), 'f', m_lbl_precision );
            painter->drawText( QPointF( pos_cw,  org_bd.height() - pct_pos * bd.height() - bd.y() + font_h_patch ), num_str );
         }
    } break;

    default:
        auto pos_ccw = width() * 0.5 - m_ln_par->majorSize() - m_lbl_dist_ofv;
        auto pos_cw  = width() * 0.5 + m_ln_par->majorSize() + m_lbl_dist_ofv;
        for ( qint64 i = start_idx; i <= m_lbl_src.endIdx(); i ++ ) {
            auto tk_type = IcQuickLblLayoutSrc::TickType_Major;
            auto pct_pos = m_lbl_src.viewPctPosAt( i, &tk_type );

            if ( ! m_disp_minor_tick_lbl && tk_type == IcQuickLblLayoutSrc::TickType_Minor ) { continue; }
            if ( ! m_disp_minor_hint_lbl && tk_type == IcQuickLblLayoutSrc::TickType_MinorHint ) { continue; }

            auto num_str = QString::number( m_lbl_src.valPosAt( i ), 'f', m_lbl_precision );
            auto str_bd  = font_metric.boundingRect( num_str );
            painter->drawText( QPointF( pos_ccw - str_bd.width(), org_bd.height() - pct_pos * bd.height() - bd.y() + font_h_patch ), num_str );
            painter->drawText( QPointF( pos_cw,  org_bd.height() - pct_pos * bd.height() - bd.y() + font_h_patch ), num_str );
        }
    }
}

// ============================================================================
// [ protected ] fixed bounding rect
// ============================================================================
auto     IcQuickValueAxisItemText :: fixedBoundRect() const -> QRectF
{
    auto bd = boundingRect();
    if ( ! m_ln_par->isVertical() ) {
       bd.setX( m_ln_par->headRsvdPx() );
       bd.setWidth( bd.width() - m_ln_par->tailRsvdPx() );
    } else {
       bd.setY(m_ln_par->headRsvdPx() );
       bd.setHeight( bd.height() - m_ln_par->tailRsvdPx() );
    }
    return bd;
}

// ============================================================================
// painter on the device
// ============================================================================
auto     IcQuickValueAxisItemText :: paint( QPainter *painter ) -> void
{
    if ( m_upd_req_cntr.loadAcquire() < 1 ) { return; }
    m_upd_req_cntr.storeRelease(0);

    // clear the canvas
    painter->fillRect( boundingRect(), QColor(0,0,0,0)); // clear all
    if ( ! m_lbl_src.isValid() || ! isVisible() ) { return; }

    // ensure fixed bounding rect
    auto bd = fixedBoundRect();
    if ( ! bd.isValid() ) { return; }

    // paint by current settings
    painter->setPen ( m_color );
    painter->setFont( m_font  );

    if ( ! m_ln_par->isVertical()) {
        paint_hori( bd, painter );
    } else {
        paint_vert( bd, painter );
    }
}





// ////////////////////////////////////////////////////////////////////////////
//                 Value Axis Item private object
// ////////////////////////////////////////////////////////////////////////////
static auto  QxPack_IcQuickValueAxisItem_Reg( ) -> void
{
    static bool is_reg = false;
    if ( ! is_reg ) {
        is_reg = true;
        qmlRegisterType<QxPack::IcQuickValueAxisItem>("qxpack.indcom.ui_qml_charts", 1, 0, "IcQuickValueAxisItem");
        qDebug() << "qxpack_ic_quickvalueaxisitem registered.";
    }
}

#define T_PrivPtr( o )  T_ObjCast( IcQuickValueAxisItemPriv*, o )
class QXPACK_IC_HIDDEN  IcQuickValueAxisItemPriv {
    friend class IcQuickValueAxisItem;
private:
    IcQuickValueAxisItem  *m_parent;
    QAtomicInt             m_param_dat_req_cntr, m_mater_req_cntr;

    IcQSGValueAxisNode::Material m_ui_mater;
    IcQSGValueAxisNode::LnParam  m_ui_ln_param;
    QRectF  m_ui_view_port;

    IcQuickValueAxisItemText  m_text_item;

protected:
    auto  fixedBoundRect( ) const -> QRectF;
public :
    explicit IcQuickValueAxisItemPriv ( IcQuickValueAxisItem *pa );
    ~IcQuickValueAxisItemPriv();

    inline auto  textItem()   -> IcQuickValueAxisItemText & { return m_text_item; }
    inline auto  lineSize()   -> float & { return m_ui_mater.size;   }
    inline auto  lineSmooth() -> float & { return m_ui_mater.smooth; }
    inline auto  lineColor()  -> QColor& { return m_ui_mater.color;  }
    inline auto  viewport()   -> QRectF& { return m_ui_view_port; }
    inline auto  uiMater()    -> IcQSGValueAxisNode::Material& { return m_ui_mater; }
    inline auto  uiLnParam()  -> IcQSGValueAxisNode::LnParam&  { return m_ui_ln_param; }
    inline auto  lblLayoutSrc() -> IcQuickLblLayoutSrc & { return m_text_item.lblSrc(); }
    inline auto  onNewParamReady() -> void { IcAlgor::doIf( [this](){ m_param_dat_req_cntr.storeRelease(1); m_parent->update(); }, m_param_dat_req_cntr.loadAcquire() < 1 ); }
    inline auto  onNewMaterReady() -> void { IcAlgor::doIf( [this](){ m_mater_req_cntr.storeRelease(1);     m_parent->update(); }, m_mater_req_cntr.loadAcquire() < 1 ); }
    auto  qsgUpnNullNode    ( QSGNode* ) -> QSGNode*;
    auto  qsgUpnExistedNode ( QSGNode* ) -> QSGNode*;
};

// ============================================================================
// ctor
// ============================================================================
IcQuickValueAxisItemPriv :: IcQuickValueAxisItemPriv ( IcQuickValueAxisItem *pa )
     : m_text_item( & m_ui_ln_param ) // put this item
{
    m_parent = pa;
    m_ui_mater.size  = LINESIZE_DEFAULT; m_ui_mater.smooth = LINESMOOTH_DEFAULT;
    m_ui_mater.color = QColor("steelblue");
    m_ui_view_port   = QRectF( 0,0,-1,-1);
    m_text_item.setVisible(true);
    m_text_item.setParentItem( m_parent );

    // force update once
    m_param_dat_req_cntr.storeRelease(1);
    m_mater_req_cntr.storeRelease(1);
    QMetaObject::invokeMethod( m_parent, "update", Qt::QueuedConnection );
}

// ============================================================================
// dtor
// ============================================================================
IcQuickValueAxisItemPriv :: ~IcQuickValueAxisItemPriv ( )
{
}

// ============================================================================
// [ protected ] return the fixed bounding rect
// ============================================================================
auto     IcQuickValueAxisItemPriv :: fixedBoundRect ( ) const -> QRectF
{
    QRectF bd = m_parent->boundingRect();
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
auto     IcQuickValueAxisItemPriv :: qsgUpnNullNode ( QSGNode *old ) -> QSGNode*
{
    IcQSGValueAxisNode *node = Q_NULLPTR;
    if ( old != Q_NULLPTR ) { return old; }
    QRectF bd = fixedBoundRect();
    if ( ! bd.isValid()) { return old; }

    // create geometry node
    node = new IcQSGValueAxisNode ( m_ui_mater );
    if ( node == Q_NULLPTR ) { return node; }
    node->setFlag( QSGNode::OwnedByParent, true ); // parent manage node's life cycle

    // generate the render data
    node->qsg_updateMaterial( m_ui_mater );
    node->qsg_updateGeometry( bd, m_ui_ln_param, m_ui_view_port, m_ui_mater, & m_text_item.lblSrc() );
    m_mater_req_cntr.storeRelease(0);
    m_param_dat_req_cntr.storeRelease(0);

    return node;
}

auto     IcQuickValueAxisItemPriv :: qsgUpnExistedNode ( QSGNode *old ) -> QSGNode*
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
        node->qsg_updateGeometry( bd, m_ui_ln_param, m_ui_view_port, m_ui_mater, & m_text_item.lblSrc() );

   }

    return node;
}



// ////////////////////////////////////////////////////////////////////////////
//                 Value Axis Wrapper API
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcQuickValueAxisItem :: IcQuickValueAxisItem ( QQuickItem *pa ) : QQuickItem( pa )
{
    m_obj = qxpack_ic_new( IcQuickValueAxisItemPriv, this );
    setFlags ( QQuickItem::ItemHasContents );
    setClip( true );
}

// ============================================================================
// dtor
// ============================================================================
IcQuickValueAxisItem :: ~IcQuickValueAxisItem ( )
{
    qxpack_ic_delete( m_obj, IcQuickValueAxisItemPriv );
}

// ============================================================================
// access the line propertys
// ============================================================================
auto     IcQuickValueAxisItem :: lineSizeRange() const -> QVector2D
{ return LINESIZE_RANGE; }

auto     IcQuickValueAxisItem :: lineSize() const -> float
{ return T_PrivPtr( m_obj )->lineSize(); }

auto     IcQuickValueAxisItem :: setLineSize( float sz ) -> void
{
    auto *priv = T_PrivPtr( m_obj );
    if ( sz >= 0.0f  &&  ! qFuzzyCompare( priv->lineSize(), sz )) {
        priv->lineSize() = sz;
        priv->onNewParamReady();   // force update geometry
        priv->onNewMaterReady();
        QMetaObject::invokeMethod( this, "lineSizeChanged", Qt::QueuedConnection );
    }
}

auto     IcQuickValueAxisItem :: lineColor() const -> QColor
{ return T_PrivPtr( m_obj )->lineColor(); }

auto     IcQuickValueAxisItem :: setLineColor( const QColor &cl ) -> void
{
    auto *priv = T_PrivPtr( m_obj );
    if ( priv->lineColor() != cl ) {
        priv->lineColor() = cl;
        priv->onNewMaterReady();   // just render data only
        QMetaObject::invokeMethod( this, "lineColorChanged", Qt::QueuedConnection );
    }
}

auto     IcQuickValueAxisItem :: viewport() const -> QRectF
{  return T_PrivPtr( m_obj )->viewport();  }

auto     IcQuickValueAxisItem :: setViewport( const QRectF &rt ) -> void
{
    auto *priv = T_PrivPtr( m_obj );
    if ( priv->viewport() != rt ) {
        priv->viewport() = rt;
        priv->onNewParamReady();   // force update geometry
        QMetaObject::invokeMethod( this, "viewportChanged", Qt::QueuedConnection );
    }
}

auto     IcQuickValueAxisItem :: majorTickCount() const -> int
{  return T_PrivPtr( m_obj )->uiLnParam().majorTickCnt(); }

auto     IcQuickValueAxisItem :: setMajorTickCount( int cnt ) -> void
{
    auto *priv = T_PrivPtr( m_obj );
    if ( priv->uiLnParam().majorTickCnt() != cnt ) {
        priv->uiLnParam().setMajorTickCnt( cnt );
        priv->onNewParamReady();
        QMetaObject::invokeMethod( this, "majorTickCountChanged", Qt::QueuedConnection );
    }
}

auto     IcQuickValueAxisItem :: minorTickCount() const -> int
{  return T_PrivPtr( m_obj )->uiLnParam().minorTickCnt(); }

auto     IcQuickValueAxisItem :: setMinorTickCount( int cnt ) -> void
{
    auto *priv = T_PrivPtr( m_obj );
    if ( priv->uiLnParam().minorTickCnt() != cnt ) {
        priv->uiLnParam().setMinorTickCnt( cnt );
        priv->onNewParamReady();
        QMetaObject::invokeMethod( this, "minorTickCountChanged", Qt::QueuedConnection );
    }
}

auto     IcQuickValueAxisItem :: minorHintStep() const -> int
{  return T_PrivPtr( m_obj )->uiLnParam().minorHintStep(); }

auto     IcQuickValueAxisItem :: setMinorHintStep( int h_cnt ) -> void
{
    auto *priv = T_PrivPtr( m_obj );
    if ( priv->uiLnParam().minorHintStep() != h_cnt ) {
        priv->uiLnParam().setMinorHintStep( h_cnt );
        priv->onNewParamReady();
        QMetaObject::invokeMethod( this, "minorHintStepChanged", Qt::QueuedConnection );
    }
}

auto     IcQuickValueAxisItem :: majorSize() const -> qreal
{ return  T_PrivPtr( m_obj )->uiLnParam().majorSize(); }

auto     IcQuickValueAxisItem :: minorSize() const -> qreal
{ return  T_PrivPtr( m_obj )->uiLnParam().minorSize(); }

auto     IcQuickValueAxisItem :: minorHintSize() const -> qreal
{ return  T_PrivPtr( m_obj )->uiLnParam().minorStepSize(); }

auto     IcQuickValueAxisItem :: setMajorSize( qreal sz ) -> void
{
    auto *priv   = T_PrivPtr( m_obj );
    auto &ln_par = priv->uiLnParam();
    if ( ! qFuzzyCompare( ln_par.majorSize(), sz )) {
        ln_par.setMajorSize( sz );
        priv->onNewParamReady();
        QMetaObject::invokeMethod( this, "majorSizeChanged", Qt::QueuedConnection );
    }
}

auto     IcQuickValueAxisItem :: setMinorSize( qreal sz ) -> void
{
    auto *priv   = T_PrivPtr( m_obj );
    auto &ln_par = priv->uiLnParam();
    if ( ! qFuzzyCompare( ln_par.minorSize(), sz )) {
        ln_par.setMinorSize( sz );
        priv->onNewParamReady();
        QMetaObject::invokeMethod( this, "minorSizeChanged", Qt::QueuedConnection );
    }
}

auto     IcQuickValueAxisItem :: setMinorHintSize( qreal sz ) -> void
{
    auto *priv   = T_PrivPtr( m_obj );
    auto &ln_par = priv->uiLnParam();
    if ( ! qFuzzyCompare( ln_par.minorSize(), sz )) {
        ln_par.setMinorStepSize( sz );
        priv->onNewParamReady();
        QMetaObject::invokeMethod( this, "minorHintSizeChanged", Qt::QueuedConnection );
    }
}


auto     IcQuickValueAxisItem :: vertical() const -> bool
{  return T_PrivPtr( m_obj )->uiLnParam().isVertical(); }

auto     IcQuickValueAxisItem :: setVertical( bool sw ) -> void
{
    auto *priv = T_PrivPtr( m_obj );
    if ( priv->uiLnParam().isVertical() != sw ) {
        priv->uiLnParam().setVertical( sw );
        priv->onNewParamReady();
        QMetaObject::invokeMethod( this, "verticalChanged", Qt::QueuedConnection );
    }
}

auto     IcQuickValueAxisItem :: min() const -> qreal
{  return T_PrivPtr( m_obj )->uiLnParam().min();  }

auto     IcQuickValueAxisItem :: max() const -> qreal
{  return T_PrivPtr( m_obj )->uiLnParam().max(); }

auto     IcQuickValueAxisItem :: setMin( qreal v ) -> void
{
    auto *priv = T_PrivPtr( m_obj );
    if ( ! qFuzzyCompare( priv->uiLnParam().min(), v ) ) {
        priv->uiLnParam().setMin( v );
        priv->onNewParamReady();
        QMetaObject::invokeMethod( this, "rangeChanged", Qt::QueuedConnection );
    }
}

auto     IcQuickValueAxisItem :: setMax( qreal v ) -> void
{
    auto *priv = T_PrivPtr( m_obj );
    if ( ! qFuzzyCompare( priv->uiLnParam().max(), v )) {
        priv->uiLnParam().setMax( v );
        priv->onNewParamReady();
        QMetaObject::invokeMethod( this, "rangeChanged", Qt::QueuedConnection );
    }
}

auto     IcQuickValueAxisItem :: blnMin() const -> qreal
{ return T_PrivPtr( m_obj )->uiLnParam().blnMin(); }

auto     IcQuickValueAxisItem :: blnMax() const -> qreal
{ return T_PrivPtr( m_obj )->uiLnParam().blnMax(); }

auto     IcQuickValueAxisItem :: setBlnMin( qreal v ) -> void
{
    auto *priv = T_PrivPtr( m_obj );
    if ( ! qFuzzyCompare( priv->uiLnParam().blnMin(), v )) {
        priv->uiLnParam().setBlnMin( v );
        priv->onNewParamReady();
        QMetaObject::invokeMethod( this, "blnRangeChanged", Qt::QueuedConnection );
    }
}

auto     IcQuickValueAxisItem :: setBlnMax( qreal v ) -> void
{
    auto *priv = T_PrivPtr( m_obj );
    if ( ! qFuzzyCompare( priv->uiLnParam().blnMax(), v )) {
        priv->uiLnParam().setBlnMax( v );
        priv->onNewParamReady();
        QMetaObject::invokeMethod( this, "blnRangeChanged", Qt::QueuedConnection );
    }
}

auto     IcQuickValueAxisItem :: tickDirection() const -> TickDirection
{
    auto  td = T_PrivPtr( m_obj )->uiLnParam().tickDir();
    switch( td ) {
    case IcQSGValueAxisNode::LnParam::TickDir_CW : return TickDirection_CW;
    case IcQSGValueAxisNode::LnParam::TickDir_CCW: return TickDirection_CCW;
    default : return TickDirection_Both;
    }
}

auto     IcQuickValueAxisItem :: setTickDirection( TickDirection td ) -> void
{
    auto *priv = T_PrivPtr( m_obj );
    if ( tickDirection() != td ) {
        IcQSGValueAxisNode::LnParam::TickDir td2;
        switch( td ) {
        case TickDirection_CW : td2 = IcQSGValueAxisNode::LnParam::TickDir_CW; break;
        case TickDirection_CCW: td2 = IcQSGValueAxisNode::LnParam::TickDir_CCW; break;
        default: td2 = IcQSGValueAxisNode::LnParam::TickDir_Both; break;
        }
        priv->uiLnParam().setTickDir( td2 );
        priv->onNewParamReady();
        priv->textItem().reqUpdate();
        QMetaObject::invokeMethod( this, "tickDirectionChanged", Qt::QueuedConnection );
    }
}

auto     IcQuickValueAxisItem :: labelFont() const -> QFont
{ return T_PrivPtr( m_obj )->textItem().font(); }

auto     IcQuickValueAxisItem :: labelColor() const -> QColor
{ return T_PrivPtr( m_obj )->textItem().color(); }

auto     IcQuickValueAxisItem :: dispStartTickLabel() const -> bool
{ return T_PrivPtr( m_obj )->textItem().dispStartTickLbl(); }

auto     IcQuickValueAxisItem :: dispMinorTickLabel() const -> bool
{ return T_PrivPtr( m_obj )->textItem().dispMinorTickLbl(); }

auto     IcQuickValueAxisItem :: dispMinorHintLabel() const -> bool
{ return T_PrivPtr( m_obj )->textItem().dispMinorHintLbl(); }

auto     IcQuickValueAxisItem :: setLabelFont ( const QFont &f ) -> void
{
   auto *priv = T_PrivPtr( m_obj );
   if ( priv->textItem().font() != f ) {
       priv->textItem().setFont( f );
       QMetaObject::invokeMethod( this, "labelFontChanged", Qt::QueuedConnection );
   }
}

auto     IcQuickValueAxisItem :: setLabelColor( const QColor &c ) -> void
{
   auto *priv = T_PrivPtr( m_obj );
   if ( priv->textItem().color() != c ) {
       priv->textItem().setColor( c );
       QMetaObject::invokeMethod( this, "labelColorChanged", Qt::QueuedConnection );
   }
}

auto     IcQuickValueAxisItem :: setDispStartTickLabel( bool sw ) -> void
{
   auto *priv = T_PrivPtr( m_obj );
   if ( priv->textItem().dispStartTickLbl() != sw ) {
       priv->textItem().setDispStartTickLbl( sw );
       QMetaObject::invokeMethod( this, "dispStartTickLabelChanged", Qt::QueuedConnection );
   }
}

auto     IcQuickValueAxisItem :: setDispMinorTickLabel( bool sw ) -> void
{
    auto *priv = T_PrivPtr( m_obj );
    if ( priv->textItem().dispMinorTickLbl() != sw ) {
        priv->textItem().setDispMinorTickLbl( sw );
        QMetaObject::invokeMethod( this, "dispMinorTickLabelChanged", Qt::QueuedConnection );
    }
}

auto     IcQuickValueAxisItem :: setDispMinorHintLabel( bool sw ) -> void
{
    auto *priv = T_PrivPtr( m_obj );
    if ( priv->textItem().dispMinorHintLbl() != sw ) {
        priv->textItem().setDispMinorHintLbl( sw );
        QMetaObject::invokeMethod( this, "dispMinorHintLabelChanged", Qt::QueuedConnection );
    }
}

auto     IcQuickValueAxisItem :: labelPrecision() const -> int
{ return T_PrivPtr( m_obj )->textItem().lblPrecision(); }

auto     IcQuickValueAxisItem :: setLabelPrecision( int n ) -> void
{
    auto *priv = T_PrivPtr( m_obj );
    if ( n < 0 ) { n = 0; }
    if ( priv->textItem().lblPrecision() != n ) {
        priv->textItem().setLblPrecision( n );
        QMetaObject::invokeMethod( this, "labelPrecisionChanged", Qt::QueuedConnection );
    }
}

auto     IcQuickValueAxisItem :: labelDistOfv() const -> int
{ return T_PrivPtr( m_obj )->textItem().lblDistOfv(); }

auto     IcQuickValueAxisItem :: setLabelDistOfv( int n ) -> void
{
    auto *priv = T_PrivPtr( m_obj );
    if ( n < 0 ) { n = 0; }
    if ( priv->textItem().lblDistOfv() != n ) {
        priv->textItem().setLblDistOfv( n );
        QMetaObject::invokeMethod( this, "labelDistOfvChanged", Qt::QueuedConnection );
    }
}

auto     IcQuickValueAxisItem :: headRsvdPx() const -> qreal
{ return T_PrivPtr(m_obj)->uiLnParam().headRsvdPx(); }

auto     IcQuickValueAxisItem :: tailRsvdPx() const -> qreal
{ return T_PrivPtr(m_obj)->uiLnParam().tailRsvdPx(); }

auto     IcQuickValueAxisItem :: setHeadRsvdPx( qreal n ) -> void
{
    auto *priv = T_PrivPtr( m_obj );
    if ( n < 0 ) { n = 0; }
    if ( ! qFuzzyCompare( n, priv->uiLnParam().headRsvdPx() ) ) {
        priv->uiLnParam().setHeadRsvdPx( n );
        priv->onNewParamReady(); // redraw it
        priv->textItem().reqUpdate();
        QMetaObject::invokeMethod( this, "headRsvdPxChanged", Qt::QueuedConnection );
    }
}

auto     IcQuickValueAxisItem :: setTailRsvdPx( qreal n ) -> void
{
    auto *priv = T_PrivPtr( m_obj );
    if ( n < 0 ) { n = 0; }
    if ( ! qFuzzyCompare( n, priv->uiLnParam().tailRsvdPx())) {
        priv->uiLnParam().setTailRsvdPx( n );
        priv->onNewParamReady(); // redraw it
        priv->textItem().reqUpdate();
        QMetaObject::invokeMethod( this, "tailRsvdPxChanged", Qt::QueuedConnection );
    }
}

auto     IcQuickValueAxisItem :: labelVisible() const -> bool
{
    auto priv = T_PrivPtr( m_obj );
    return priv->textItem().isVisible();
}

auto     IcQuickValueAxisItem :: setLabelVisible( bool sw ) -> void
{
    auto priv = T_PrivPtr( m_obj );
    if ( priv->textItem().isVisible() != sw ) {
        priv->textItem().setVisible( sw );
        priv->textItem().reqUpdate();
        QMetaObject::invokeMethod( this, "labelVisibleChanged", Qt::QueuedConnection );
    }
}

// ============================================================================
// update the paint node ( QSG thread )
// ============================================================================
auto     IcQuickValueAxisItem :: updatePaintNode( QSGNode *old, UpdatePaintNodeData*) -> QSGNode*
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
#if    QT_VERSION_MAJOR < 6
auto     IcQuickValueAxisItem :: geometryChanged( const QRectF &new_geom, const QRectF &old_geom ) -> void
#else
auto     IcQuickValueAxisItem :: geometryChange ( const QRectF &new_geom, const QRectF &old_geom ) -> void
#endif
{
    auto *priv = T_PrivPtr( m_obj );
    priv->onNewParamReady(); // re-generate the vertex data
    priv->textItem().setSize( new_geom.size());
#if    QT_VERSION_MAJOR < 6
    QQuickItem::geometryChanged( new_geom, old_geom );
#else
    QQuickItem::geometryChange( new_geom, old_geom );
#endif
}


// ============================================================================
// [ static ] register in QML engine
// ============================================================================
auto     IcQuickValueAxisItem :: _reg() -> void
{  QxPack_IcQuickValueAxisItem_Reg(); }

}

#include "qxpack_ic_quickvalueaxisitem.moc"
#endif
