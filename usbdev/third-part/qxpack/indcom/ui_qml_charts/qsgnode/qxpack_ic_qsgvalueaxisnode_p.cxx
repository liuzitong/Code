#ifndef QXPACK_IC_QSGVALUEAXISNODE_P_CXX
#define QXPACK_IC_QSGVALUEAXISNODE_P_CXX

#include "qxpack_ic_qsgvalueaxisnode_p.hxx"
#include "qxpack/indcom/sys/qxpack_ic_geom2dalgor.hxx"
#include "qxpack/indcom/algor/qxpack_ic_algor_base.hpp"

#include <QByteArray>
//#include <QSGSimpleMaterialShader>
#include <QtMath>
#include <QPainter>
#include <QImage>
#include <QDebug>
#include <QDebug>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//                 Tick information
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN  TickInfo {
private:
    qreal    m_vrg_min,  m_vrg_max;    // tick visble value range
    qreal    m_brg_min,  m_brg_max;    // base line value range
    qreal    m_btk_step, m_mj_tk_step; // base tick step ( the minimal value step ),
    qreal    m_nr_min_tk_idx, m_nr_max_tk_idx; // nearest tick index in view port,
                                               // NOTE: the tick index is begin with parameter's minimal value.
    qintptr  m_btk_cnt;
public :

    explicit TickInfo( const qreal &vw_min, const qreal &vw_max, const IcQSGValueAxisNode::LnParam &ln_par )
    {
        // [ m_vrg_min, m_vrg_max ] is used for ticks ( visible for user )
        if ( vw_min <= vw_max ) {
            m_vrg_min = ( ln_par.min() <= vw_min ? vw_min : ( ln_par.min() <= vw_max ? ln_par.min() : vw_max + 1.0 ));
            m_vrg_max = ( ln_par.max() >= vw_max ? vw_max : ( ln_par.max() >= vw_min ? ln_par.max() : vw_min - 1.0 ));
        } else {
            m_vrg_min = ln_par.min();
            m_vrg_max = ln_par.max();
        }

        // [ m_brg_min, m_brg_max ] is used for base line
        if ( vw_min <= vw_max ) {
            m_brg_min = ( ln_par.blnMin() <= vw_min ? vw_min : ( ln_par.blnMin() <= vw_max ? ln_par.blnMin() : vw_max + 1.0 ));
            m_brg_max = ( ln_par.blnMax() >= vw_max ? vw_max : ( ln_par.blnMax() >= vw_min ? ln_par.blnMax() : vw_min - 1.0 ));
        } else {
            m_brg_max = ln_par.blnMax();
            m_brg_min = ln_par.blnMin();
        }

        m_btk_step = m_mj_tk_step = 0; m_btk_cnt = -1;
        m_nr_max_tk_idx = m_nr_max_tk_idx = -1;

        if ( isValid() ) {
            m_mj_tk_step = ( ln_par.max() - ln_par.min() ) / ( ln_par.majorTickCnt() - 1 );
            m_btk_cnt  = ( ln_par.majorTickCnt() - 1 ) * ln_par.minorTickCnt() + ln_par.majorTickCnt();
            m_btk_step = m_mj_tk_step / ( ln_par.minorTickCnt() + 1 );
            auto vrg_min_t = m_vrg_min - ln_par.min();
            auto vrg_max_t = m_vrg_max - ln_par.min();
            std::modf( vrg_min_t / m_btk_step, &m_nr_min_tk_idx );
            std::modf( vrg_max_t / m_btk_step, &m_nr_max_tk_idx );
            //if ( m_nr_min_tk_idx * m_btk_step > m_vrg_min ) { m_nr_min_tk_idx --; }
            if ( m_nr_max_tk_idx * m_btk_step < vrg_max_t ) { m_nr_max_tk_idx ++; }
        }
    }

    inline auto  isValid() const      -> bool { return m_brg_min < m_brg_max; }
    inline auto  hasTickInVrg() const -> bool { return m_nr_max_tk_idx >= m_nr_min_tk_idx; }
    inline auto  baseTickStep() const -> const qreal& { return m_btk_step; }
    inline auto  vrgMin() const -> const qreal& { return m_vrg_min; }
    inline auto  vrgMax() const -> const qreal& { return m_vrg_max; }
    inline auto  brgMin() const -> const qreal& { return m_brg_min; }
    inline auto  brgMax() const -> const qreal& { return m_brg_max; }
    inline auto  nrMinTickIdx() const -> const qreal& { return m_nr_min_tk_idx; }
    inline auto  nrMaxTickIdx() const -> const qreal& { return m_nr_max_tk_idx; }
    inline auto  nrMinTickIdxInt() const -> qint64 { return qint64( m_nr_min_tk_idx ); }
    inline auto  nrMaxTickIdxInt() const -> qint64 { return qint64( m_nr_max_tk_idx ); }
};



// ////////////////////////////////////////////////////////////////////////////
//                 QSG Line Series Node
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor ( create it in QSG thread )
// ============================================================================
IcQSGValueAxisNode :: IcQSGValueAxisNode(  const Material &mater )
    : m_geom( QSGGeometry::defaultAttributes_Point2D(), 0 )
{
    m_mater = mater;

    m_geom.setLineWidth( mater.size );

#if QT_VERSION < QT_VERSION_CHECK( 5,8,0 )
    m_geom.setDrawingMode(  0x0001 ); // GL_LINES
#else
    m_geom.setDrawingMode( QSGGeometry::DrawLines );
#endif

    m_geom.setVertexDataPattern( QSGGeometry::DynamicPattern );

    m_flat_mater.setColor( mater.color );
    m_flat_mater.setFlag( QSGMaterial::Blending, true );

    setGeometry( & m_geom );
    setMaterial( & m_flat_mater );
    setFlags( QSGNode::OwnedByParent, true );
    setFlags( QSGNode::OwnsGeometry | QSGNode::OwnsMaterial, false );

    markDirty( QSGNode::DirtyGeometry | QSGNode::DirtyMaterial  );
}

// ============================================================================
// dtor
// ============================================================================
IcQSGValueAxisNode :: ~IcQSGValueAxisNode( )
{
}

// ============================================================================
// just redraw the axis by new material
// ============================================================================
auto     IcQSGValueAxisNode :: qsg_updateMaterial( const Material &mater ) -> bool
{
    if ( m_mater != mater ) {
        m_mater = mater;
        m_geom.setLineWidth  ( mater.size );
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
// [ protected ] clear the geometry
// ============================================================================
auto     IcQSGValueAxisNode :: clearGeom( IcQuickLblLayoutSrc *lbl_layout ) -> void
{
     m_geom.allocate( 0 ); markDirty( QSGNode::DirtyGeometry );
     if ( lbl_layout != nullptr ) { lbl_layout->invalidate(); }
}

// ============================================================================
// [ static ] prepare the geometry
// ============================================================================
static auto  gPrepareGeom ( QSGGeometry &geom, const TickInfo &tick_info,  const IcQSGValueAxisNode::LnParam &ln_par ) -> void
{
    if ( tick_info.hasTickInVrg()) {
        int  tick_sz = int( tick_info.nrMaxTickIdxInt() - tick_info.nrMinTickIdxInt() + 1 );
        auto tick_cnt = ( ln_par.isShowBln() ? 2 : 0 ) + 2 * tick_sz;
        if ( geom.vertexCount() != tick_cnt ) { geom.allocate( tick_cnt );  }
    } else {
        auto tick_cnt = ln_par.isShowBln() ? 2 : 0;
        if ( geom.vertexCount() != tick_cnt ) { geom.allocate( tick_cnt );  }
    }
}

// ============================================================================
// draw the geometry by vertical direction
// ============================================================================
auto     IcQSGValueAxisNode :: qsg_updGeomVert (
    const QRectF &bounds, const LnParam &ln_par, const QRectF &vw_port, const Material &mater, IcQuickLblLayoutSrc *lbl_layout
) -> void
{
    Q_UNUSED( mater );
    TickInfo tick_info( vw_port.y(), vw_port.bottom(), ln_par );

    // ------------------------------------------------------------------------
    // make sure has tick info. or clean the draw data
    // ------------------------------------------------------------------------
    if ( ! tick_info.isValid() ) { clearGeom( lbl_layout ); return; }

    // ------------------------------------------------------------------------
    // allocate the geometry buffer
    // ------------------------------------------------------------------------
    gPrepareGeom( m_geom, tick_info, ln_par );
    if ( ! tick_info.hasTickInVrg() && ! ln_par.isShowBln()) { return; }

    // ------------------------------------------------------------------------
    // fill the geometry buffer
    // ------------------------------------------------------------------------
    auto inv_rg_sz = 1.0 / ( vw_port.isValid() ? vw_port.height() : ln_par.max() - ln_par.min() );
    auto bd_ofv  = bounds.y(), bd_sz = bounds.height();
    auto px_pos  = float( bounds.width() * 0.5 );
    auto vtx     = m_geom.vertexDataAsPoint2D();
    int  vtx_ofv = 0;

    // a) generate the vertical base line two point
    if ( ln_par.isShowBln() ) {   // NOTE: axis Y should be map to math coord.
        auto rg_ofv  = ( vw_port.isValid() ? vw_port.y() : ln_par.blnMin() );
        auto pct_pos = ( tick_info.brgMin() - rg_ofv ) * inv_rg_sz;
        auto sc_pos  = ( bd_sz - pct_pos * bd_sz ) + bd_ofv;
        vtx[0].set( px_pos, float( sc_pos ));

        pct_pos = ( tick_info.brgMax() - rg_ofv ) * inv_rg_sz;
        sc_pos  = ( bd_sz - pct_pos * bd_sz ) + bd_ofv;
        vtx[1].set( px_pos, float( sc_pos ));

        vtx_ofv = 2;
    }

    // b) generate the points for ticks
    if ( tick_info.hasTickInVrg()) {
        auto nr_min_idx = tick_info.nrMinTickIdxInt(); // nearest minimum tick index
        auto nr_max_idx = tick_info.nrMaxTickIdxInt(); // nearest maxmum  tick index
        auto rg_ofv = ( vw_port.isValid() ? vw_port.y() : ln_par.min() );

        auto mj_f_rd_v  = ln_par.minorTickCnt() + 1;
        auto mn_hf_rd_v = ( ln_par.minorHintStep() > 0 ? ln_par.minorHintStep() : mj_f_rd_v );
        auto mj_f_cntr  = nr_min_idx % mj_f_rd_v;
        auto mn_hf_cntr = nr_min_idx % mn_hf_rd_v;
        if ( mj_f_cntr  == 0 ) { mj_f_cntr  = mj_f_rd_v; }
        if ( mn_hf_cntr == 0 ) { mn_hf_cntr = mn_hf_rd_v; }

        // setup label layout data
        if ( lbl_layout != nullptr ) {
            lbl_layout->init (
                nr_min_idx, nr_max_idx, rg_ofv, 1.0 / inv_rg_sz, tick_info.baseTickStep(), ln_par.min(),
                mj_f_rd_v, mn_hf_rd_v
            );
        }

        // a direction trick for later use ( reduce the 'if' sentence )
        float tk_ccw_sz = 0, tk_cw_sz = 0;
        auto  tk_ccw_ptr = &tk_ccw_sz, tk_cw_ptr = &tk_cw_sz;
        switch( ln_par.tickDir() ) {
        case LnParam::TickDir_CCW : tk_cw_ptr  = tk_ccw_ptr; break;
        case LnParam::TickDir_CW  : tk_ccw_ptr = tk_cw_ptr;  break;
        default: break;
        }

        int  fst_mj_pos = -1, last_mj_pos = -1; // 20210313, added for record first and last major

        for ( auto i = nr_min_idx; i <= nr_max_idx; i ++, mj_f_cntr ++, mn_hf_cntr ++ ) {

           // overwite the tick size by ccw and cw, here used a pointer trick tech.
           // Assume CCW and CW ticks are used.
           *tk_ccw_ptr = float( ln_par.minorSize()); *tk_cw_ptr = *tk_ccw_ptr;
           if ( mn_hf_cntr == mn_hf_rd_v ) {
              *tk_ccw_ptr = float( ln_par.minorStepSize()); *tk_cw_ptr = *tk_ccw_ptr;
              mn_hf_cntr = 0;
           }
           if ( mj_f_cntr == mj_f_rd_v ) {
              *tk_ccw_ptr = float( ln_par.majorSize()); *tk_cw_ptr = *tk_ccw_ptr;
              mj_f_cntr = 0;
              if ( fst_mj_pos == -1 ) { fst_mj_pos = vtx_ofv; } // added 20210313
              last_mj_pos = vtx_ofv;
           }

           // translate the math point to item coord.
           auto pct_pos = ( tick_info.baseTickStep() * qreal(i) + ln_par.min() - rg_ofv ) * inv_rg_sz;
           auto sc_pos  = ( bd_sz - pct_pos * bd_sz ) + bd_ofv;

           if ( pct_pos >= 0 && pct_pos <= 1.0 ) {
               vtx[ vtx_ofv + 0 ].set( px_pos - tk_ccw_sz, float( sc_pos ));
               vtx[ vtx_ofv + 1 ].set( px_pos + tk_cw_sz,  float( sc_pos ));
           } else {
               vtx[ vtx_ofv + 0 ].set( 0, 0 );
               vtx[ vtx_ofv + 1 ].set( 0, 0 );
           }
           vtx_ofv += 2;
       }

       if ( ! ln_par.showFirstMajor() && fst_mj_pos >= 0 ) {
           vtx[ fst_mj_pos ].set( 0, 0 ); vtx[ fst_mj_pos + 1 ].set( 0, 0 );
       }
       if ( ! ln_par.showLastMajor() && last_mj_pos >= 0 ) {
           vtx[ last_mj_pos ].set( 0, 0 ); vtx[ last_mj_pos + 1 ].set( 0, 0 );
       }

    } else {
        if ( lbl_layout != nullptr ) { lbl_layout->invalidate(); }
    }

    markDirty( QSGNode::DirtyGeometry );
}

// ============================================================================
// draw the geometry by horizontal direction
// ============================================================================
auto     IcQSGValueAxisNode :: qsg_updGeomHori(
    const QRectF &bounds, const LnParam &ln_par, const QRectF &vw_port, const Material &mater, IcQuickLblLayoutSrc *lbl_layout
) -> void
{
     Q_UNUSED( mater );
     TickInfo tick_info( vw_port.x(), vw_port.right(), ln_par );

    // ------------------------------------------------------------------------
    // make sure has tick info. or clean the draw data
    // ------------------------------------------------------------------------
    if ( ! tick_info.isValid() ) { clearGeom( lbl_layout ); return; }

    // ------------------------------------------------------------------------
    // allocate the geometry buffer
    // ------------------------------------------------------------------------
    gPrepareGeom( m_geom, tick_info, ln_par );
    if ( ! tick_info.hasTickInVrg() && ! ln_par.isShowBln()) { return; }

    // ------------------------------------------------------------------------
    // fill the geometry buffer
    // ------------------------------------------------------------------------
    auto inv_rg_sz = 1.0 / ( vw_port.isValid() ? vw_port.height() : ln_par.max() - ln_par.min() );
    auto bd_ofv  = bounds.x(), bd_sz = bounds.width();
    auto px_pos  = float( bounds.height() * 0.5 );
    auto vtx     = m_geom.vertexDataAsPoint2D();
    int  vtx_ofv = 0;

    // a) generate the vertical base line two point
    if ( ln_par.isShowBln() ) {   // NOTE: axis Y should be map to math coord.
        auto rg_ofv  = ( vw_port.isValid() ? vw_port.x() : ln_par.blnMin() );
        auto pct_pos = ( tick_info.brgMin() - rg_ofv ) * inv_rg_sz;
        auto sc_pos  = pct_pos * bd_sz + bd_ofv;
        vtx[0].set( float( sc_pos ), px_pos );

        pct_pos = ( tick_info.brgMax() - rg_ofv ) * inv_rg_sz;
        sc_pos  = pct_pos * bd_sz + bd_ofv;
        vtx[1].set( float( sc_pos ), px_pos );

        vtx_ofv = 2;
    }

    // b) generate the points for ticks
    if ( tick_info.hasTickInVrg()) {
        auto nr_min_idx = tick_info.nrMinTickIdxInt(); // nearest minimum tick index
        auto nr_max_idx = tick_info.nrMaxTickIdxInt(); // nearest maxmum  tick index
        auto rg_ofv = ( vw_port.isValid() ? vw_port.x() : ln_par.min() );

        auto mj_f_rd_v  = ln_par.minorTickCnt() + 1;
        auto mn_hf_rd_v = ( ln_par.minorHintStep() > 0 ? ln_par.minorHintStep() : mj_f_rd_v );
        auto mj_f_cntr  = nr_min_idx % mj_f_rd_v;
        auto mn_hf_cntr = nr_min_idx % mn_hf_rd_v;
        if ( mj_f_cntr  == 0 ) { mj_f_cntr  = mj_f_rd_v; }
        if ( mn_hf_cntr == 0 ) { mn_hf_cntr = mn_hf_rd_v; }

        // setup label layout data
        if ( lbl_layout != nullptr ) {
            lbl_layout->init (
                nr_min_idx, nr_max_idx, rg_ofv, 1.0 / inv_rg_sz, tick_info.baseTickStep(), ln_par.min(),
                mj_f_rd_v, mn_hf_rd_v
            );
        }

        // a direction trick for later use ( reduce the 'if' sentence )
        float tk_ccw_sz = 0, tk_cw_sz = 0;
        auto  tk_ccw_ptr = &tk_ccw_sz, tk_cw_ptr = &tk_cw_sz;
        switch( ln_par.tickDir() ) {
        case LnParam::TickDir_CCW : tk_cw_ptr  = tk_ccw_ptr; break;
        case LnParam::TickDir_CW  : tk_ccw_ptr = tk_cw_ptr;  break;
        default: break;
        }

        int  fst_mj_pos = -1, last_mj_pos = -1; // 20210313, added for record first and last major

        for ( auto i = nr_min_idx; i <= nr_max_idx; i ++, mj_f_cntr ++, mn_hf_cntr ++ ) {

           // overwite the tick size by ccw and cw, here used a pointer trick tech.
           // Assume CCW and CW ticks are used.
           *tk_ccw_ptr = float( ln_par.minorSize()); *tk_cw_ptr = *tk_ccw_ptr;
           if ( mn_hf_cntr == mn_hf_rd_v ) {
              *tk_ccw_ptr = float( ln_par.minorStepSize()); *tk_cw_ptr = *tk_ccw_ptr;
              mn_hf_cntr = 0;
           }
           if ( mj_f_cntr == mj_f_rd_v ) {
              *tk_ccw_ptr = float( ln_par.majorSize()); *tk_cw_ptr = *tk_ccw_ptr;
              mj_f_cntr = 0;
              if ( fst_mj_pos == -1 ) { fst_mj_pos = vtx_ofv; } // added 20210313
              last_mj_pos = vtx_ofv;
           }

           // translate the math point to item coord.
           auto pct_pos = ( tick_info.baseTickStep() * qreal(i) + ln_par.min() - rg_ofv ) * inv_rg_sz;
           auto sc_pos  = pct_pos * bd_sz + bd_ofv;

           if ( pct_pos >= 0 && pct_pos <= 1.0 ) {
               vtx[ vtx_ofv + 0 ].set( float( sc_pos ), px_pos - tk_ccw_sz );
               vtx[ vtx_ofv + 1 ].set( float( sc_pos ), px_pos + tk_cw_sz );
           } else {
               vtx[ vtx_ofv + 0 ].set( 0, 0 );
               vtx[ vtx_ofv + 1 ].set( 0, 0 );
           }
           vtx_ofv += 2;
       }

       if ( ! ln_par.showFirstMajor() && fst_mj_pos >= 0 ) {
           vtx[ fst_mj_pos ].set( 0, 0 ); vtx[ fst_mj_pos + 1 ].set( 0, 0 );
       }
       if ( ! ln_par.showLastMajor() && last_mj_pos >= 0 ) {
           vtx[ last_mj_pos ].set( 0, 0 ); vtx[ last_mj_pos + 1 ].set( 0, 0 );
       }

    } else {
        if ( lbl_layout != nullptr ) { lbl_layout->invalidate(); }
    }

    markDirty( QSGNode::DirtyGeometry );
}



}
#endif
