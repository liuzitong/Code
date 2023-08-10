#ifndef QXPACK_IC_QSGLINESERIESNODE_P_CXX
#define QXPACK_IC_QSGLINESERIESNODE_P_CXX

#include "qxpack_ic_qsglineseriesnode_p.hxx"
#include "qxpack/indcom/sys/qxpack_ic_geom2dalgor.hxx"
#include <QByteArray>
#include <QSGSimpleMaterialShader>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
// QSG Line Series Node
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// reallocate geom
// ============================================================================
auto     IcQSGLineSeriesNode :: reallocGeom( int n ) -> void
{
    if ( m_geom != nullptr ) { delete m_geom; }
    m_geom = new QSGGeometry( QSGGeometry::defaultAttributes_Point2D(), n );
    m_geom->setLineWidth( m_mater.size );
#if QT_VERSION < QT_VERSION_CHECK(5,8,0 )
    m_geom->setDrawingMode( 0x0003 ); // GL_LINE_STRIP
#else
    m_geom->setDrawingMode( QSGGeometry::DrawLineStrip );
#endif
    m_geom->setVertexDataPattern( QSGGeometry::DynamicPattern );
    setGeometry( m_geom );
}

// ============================================================================
// ctor ( create it in QSG thread )
// ============================================================================
IcQSGLineSeriesNode :: IcQSGLineSeriesNode(  const Material &mater )
{
    m_mater = mater;  
    m_geom  = nullptr;

    m_flat_mater.setColor( mater.color );
    m_flat_mater.setFlag( QSGMaterial::Blending, true );
    setMaterial( & m_flat_mater );

    setFlags( QSGNode::OwnedByParent, true );
    setFlags( QSGNode::OwnsGeometry | QSGNode::OwnsMaterial, false );

    reallocGeom(0);

    markDirty( QSGNode::DirtyGeometry | QSGNode::DirtyMaterial  );
}

// ============================================================================
// dtor
// ============================================================================
IcQSGLineSeriesNode :: ~IcQSGLineSeriesNode( )
{
    setGeometry(nullptr);
    if ( m_geom != nullptr ) { delete m_geom; }
}

// ============================================================================
// change the material value
// ============================================================================
auto     IcQSGLineSeriesNode :: qsgUpdateMaterial( const Material &mater ) -> bool
{
    if ( m_mater != mater ) {
        m_mater = mater;
        m_geom->setLineWidth( mater.size );
        m_flat_mater.setColor( mater.color );
        markDirty( QSGNode::DirtyMaterial | QSGNode::DirtyGeometry );
        return true;
    } else {
        m_flat_mater.setColor( mater.color );
        markDirty( QSGNode::DirtyMaterial );
        return false;
    }
}

// ============================================================================
// draw the geometry
// ============================================================================
static auto  gNormData ( QSGGeometry::Point2D *pts, const QList<QPointF> &ser_dat, const QRectF &vw_prt ) -> void
{
    auto inv_width  = 1.0 / (vw_prt.width() > 0 ? vw_prt.width() : 1 );
    auto inv_height = 1.0 / (vw_prt.height() > 0 ? vw_prt.height() : 1 );
    auto vw_x = vw_prt.x();                  auto vw_y = vw_prt.y();

    auto citr = ser_dat.constBegin();   auto pts_ofv = 0;
    while ( citr != ser_dat.constEnd()) {
        auto &pt = *( citr ++ );
        // RectF x,y is a top-left, here convert the math coord. to UI coord.
        pts[ pts_ofv ++ ].set( float(( pt.x() - vw_x ) * inv_width  ), float(( vw_y - pt.y()) * inv_height ));
    }
}

// added 20210520, this solve the vector data
static auto  gNormData ( QSGGeometry::Point2D *pts, const QVector<float> &samples, float x_ofv, float x_step, const QRectF &vw_prt )-> void
{
    auto inv_width  = 1.0 / (vw_prt.width() > 0 ? vw_prt.width() : 1 );
    auto inv_height = 1.0 / (vw_prt.height()> 0 ? vw_prt.height() : 1 );
    auto vw_x = vw_prt.x();                  auto vw_y = vw_prt.y();

    auto num   = samples.size();  auto x_pos = qreal(x_ofv);  auto ser = samples.constData();
    for ( int i = 0; i < num; i ++ ) {
        pts[i].set( float(( x_pos - vw_x ) * inv_width ), float(( vw_y - qreal( ser[i] )) * inv_height ));
        x_pos = qreal( x_ofv ) + i * qreal( x_step );
    }
}

static auto  gScaleData( QSGGeometry::Point2D *pts, int n, const QRectF &bounds ) -> void
{
    auto x = float( bounds.x());      auto y = float( bounds.y());
    auto w = float( bounds.width());  auto h = float( bounds.height());
    for ( int i = 0; i < n; i ++ ) {
        pts[i].set( x + pts[i].x * w, y + pts[i].y * h );
    }
}

auto     IcQSGLineSeriesNode :: qsgUpdateGeometry( const QRectF &bounds, const QList<QPointF> &samples , const QRectF &vw_port, const Material &mater ) -> void
{
    // avoid empty data
    if ( samples.isEmpty() || ! vw_port.isValid() ) {
        reallocGeom(0); markDirty( QSGNode::DirtyGeometry );
        return;
    }

    // adjust the memory
    if ( m_geom->vertexCount() != samples.size()) {
        reallocGeom( samples.size());
    }
    gNormData ( m_geom->vertexDataAsPoint2D(), samples, vw_port );
    gScaleData( m_geom->vertexDataAsPoint2D(), m_geom->vertexCount(), bounds );

    // check if mater...
    if ( mater != m_mater ) { m_mater = mater; }
    if ( qFuzzyCompare( m_geom->lineWidth(), mater.size )) { m_geom->setLineWidth( mater.size ); }

    markDirty( QSGNode::DirtyGeometry );
}

auto     IcQSGLineSeriesNode :: qsgUpdateGeometry( const QRectF &bounds, const QVector<float> &samples, float x_ofv, float x_step, const QRectF &vw_port, const Material &mater ) -> void
{
    // avoid empty data
    if ( samples.isEmpty() || ! vw_port.isValid() ) {
        reallocGeom(0); markDirty( QSGNode::DirtyGeometry );
        return;
    }

    // adjust the memory
    if ( m_geom->vertexCount() != samples.size()) {
        reallocGeom( samples.size() );
    }
    gNormData ( m_geom->vertexDataAsPoint2D(), samples, x_ofv, x_step, vw_port );
    gScaleData( m_geom->vertexDataAsPoint2D(), m_geom->vertexCount(), bounds );

    // check if mater...
    if ( mater != m_mater ) { m_mater = mater; }
    if ( qFuzzyCompare( m_geom->lineWidth(), mater.size )) { m_geom->setLineWidth( mater.size ); }

    markDirty( QSGNode::DirtyGeometry );
}


}
#endif
