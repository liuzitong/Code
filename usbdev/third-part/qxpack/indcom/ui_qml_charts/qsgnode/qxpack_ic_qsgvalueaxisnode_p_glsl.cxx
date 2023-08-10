#ifndef QXPACK_IC_QSGVALUEAXISNODE_P_CXX
#define QXPACK_IC_QSGVALUEAXISNODE_P_CXX

#include "qxpack_ic_qsgvalueaxisnode_p.hxx"

#include <QByteArray>
#include <QtMath>
#include <QDebug>

namespace QxPack {


// ////////////////////////////////////////////////////////////////////////////
//                 QSG Line Series Node
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor ( create it in QSG thread )
// ============================================================================
IcQSGValueAxisNode :: IcQSGValueAxisNode( QQuickItem *item )
{
    m_item = item;
    m_is_ln_par_chg = false;
}

// ============================================================================
// dtor
// ============================================================================
IcQSGValueAxisNode :: ~IcQSGValueAxisNode( )
{
}

// ============================================================================
// other functions
// ============================================================================
auto     IcQSGValueAxisNode :: estmLabelLayout( IcQuickLblLayoutSrc *lbl_layout ) -> void
{
    if ( m_tick_info.isValid()) {
        auto inv_rg_sz = 1.0;
        if ( m_vw_port.isValid() ) {
            inv_rg_sz /= m_vw_port.height();
        } else {
            auto max_min_gap = m_ln_par.max() - m_ln_par.min();
            inv_rg_sz /= ( ! qFuzzyIsNull( max_min_gap ) ? max_min_gap : DBL_MIN );
        }

        auto nr_min_idx = m_tick_info.nrMinTickIdxInt(); // nearest minimum tick index
        auto nr_max_idx = m_tick_info.nrMaxTickIdxInt(); // nearest maxmum  tick index
        auto rg_ofv = ( m_vw_port.isValid() ? m_vw_port.y() : m_ln_par.min() );

        auto mj_f_rd_v  = m_ln_par.minorTickCnt() + 1;
        auto mn_hf_rd_v = ( m_ln_par.minorHintStep() > 0 ? m_ln_par.minorHintStep() : mj_f_rd_v );

        if ( lbl_layout != nullptr ) {
            lbl_layout->init (
                nr_min_idx, nr_max_idx, rg_ofv, 1.0 / inv_rg_sz, m_tick_info.baseTickStep(), m_ln_par.min(),
                mj_f_rd_v, mn_hf_rd_v
            );
        }
    } else {
        if ( lbl_layout != nullptr ) { lbl_layout->invalidate(); }
    }
}




//// ============================================================================
//// just redraw the axis by new material
//// ============================================================================
//auto     IcQSGValueAxisNode :: qsg_updateMaterial( const Material &mater ) -> bool
//{
//    if ( m_mater != mater ) {
//        m_mater = mater;
//        m_geom.setLineWidth  ( mater.size );
//        m_flat_mater.setColor( mater.color );
//        markDirty( QSGNode::DirtyMaterial | QSGNode::DirtyGeometry );
//        return true;
//    } else {
//        m_flat_mater.setColor( mater.color );
//        markDirty( QSGNode::DirtyMaterial );
//        return false;
//    }
//}

//// ============================================================================
//// [ protected ] clear the geometry
//// ============================================================================
//auto     IcQSGValueAxisNode :: clearGeom( IcQuickLblLayoutSrc *lbl_layout ) -> void
//{
//     m_geom.allocate( 0 ); markDirty( QSGNode::DirtyGeometry );
//     if ( lbl_layout != nullptr ) { lbl_layout->invalidate(); }
//}

//// ============================================================================
//// [ static ] prepare the geometry
//// ============================================================================
//static auto  gPrepareGeom ( QSGGeometry &geom, const TickInfo &tick_info,  const IcQSGValueAxisNode::LnParam &ln_par ) -> void
//{
//    if ( tick_info.hasTickInVrg()) {
//        int  tick_sz = int( tick_info.nrMaxTickIdxInt() - tick_info.nrMinTickIdxInt() + 1 );
//        auto tick_cnt = ( ln_par.isShowBln() ? 2 : 0 ) + 2 * tick_sz;
//        if ( geom.vertexCount() != tick_cnt ) { geom.allocate( tick_cnt );  }
//    } else {
//        auto tick_cnt = ln_par.isShowBln() ? 2 : 0;
//        if ( geom.vertexCount() != tick_cnt ) { geom.allocate( tick_cnt );  }
//    }
//}

//// ============================================================================
//// draw the geometry by vertical direction
//// ============================================================================
//auto     IcQSGValueAxisNode :: qsg_updGeomVert (
//    const QRectF &bounds, const LnParam &ln_par, const QRectF &vw_port, const Material &mater, IcQuickLblLayoutSrc *lbl_layout
//) -> void
//{
//    Q_UNUSED( mater );
//    TickInfo tick_info( vw_port.y(), vw_port.bottom(), ln_par );

//    // ------------------------------------------------------------------------
//    // make sure has tick info. or clean the draw data
//    // ------------------------------------------------------------------------
//    if ( ! tick_info.isValid() ) { clearGeom( lbl_layout ); return; }

//    // ------------------------------------------------------------------------
//    // allocate the geometry buffer
//    // ------------------------------------------------------------------------
//    gPrepareGeom( m_geom, tick_info, ln_par );
//    if ( ! tick_info.hasTickInVrg() && ! ln_par.isShowBln()) { return; }

//    // ------------------------------------------------------------------------
//    // fill the geometry buffer
//    // ------------------------------------------------------------------------
//    auto inv_rg_sz = 1.0 / ( vw_port.isValid() ? vw_port.height() : ln_par.max() - ln_par.min() );
//    auto bd_ofv  = bounds.y(), bd_sz = bounds.height();
//    auto px_pos  = float( bounds.width() * 0.5 );
//    auto vtx     = m_geom.vertexDataAsPoint2D();
//    int  vtx_ofv = 0;

//    // a) generate the vertical base line two point
//    if ( ln_par.isShowBln() ) {   // NOTE: axis Y should be map to math coord.
//        auto rg_ofv  = ( vw_port.isValid() ? vw_port.y() : ln_par.blnMin() );
//        auto pct_pos = ( tick_info.brgMin() - rg_ofv ) * inv_rg_sz;
//        auto sc_pos  = ( bd_sz - pct_pos * bd_sz ) + bd_ofv;
//        vtx[0].set( px_pos, float( sc_pos ));

//        pct_pos = ( tick_info.brgMax() - rg_ofv ) * inv_rg_sz;
//        sc_pos  = ( bd_sz - pct_pos * bd_sz ) + bd_ofv;
//        vtx[1].set( px_pos, float( sc_pos ));

//        vtx_ofv = 2;
//    }

//    // b) generate the points for ticks
//    if ( tick_info.hasTickInVrg()) {
//        auto nr_min_idx = tick_info.nrMinTickIdxInt(); // nearest minimum tick index
//        auto nr_max_idx = tick_info.nrMaxTickIdxInt(); // nearest maxmum  tick index
//        auto rg_ofv = ( vw_port.isValid() ? vw_port.y() : ln_par.min() );

//        auto mj_f_rd_v  = ln_par.minorTickCnt() + 1;
//        auto mn_hf_rd_v = ( ln_par.minorHintStep() > 0 ? ln_par.minorHintStep() : mj_f_rd_v );
//        auto mj_f_cntr  = nr_min_idx % mj_f_rd_v;
//        auto mn_hf_cntr = nr_min_idx % mn_hf_rd_v;
//        if ( mj_f_cntr  == 0 ) { mj_f_cntr  = mj_f_rd_v; }
//        if ( mn_hf_cntr == 0 ) { mn_hf_cntr = mn_hf_rd_v; }

//        // setup label layout data
//        if ( lbl_layout != nullptr ) {
//            lbl_layout->init (
//                nr_min_idx, nr_max_idx, rg_ofv, 1.0 / inv_rg_sz, tick_info.baseTickStep(), ln_par.min(),
//                mj_f_rd_v, mn_hf_rd_v
//            );
//        }

//        // a direction trick for later use ( reduce the 'if' sentence )
//        float tk_ccw_sz = 0, tk_cw_sz = 0;
//        auto  tk_ccw_ptr = &tk_ccw_sz, tk_cw_ptr = &tk_cw_sz;
//        switch( ln_par.tickDir() ) {
//        case LnParam::TickDir_CCW : tk_cw_ptr  = tk_ccw_ptr; break;
//        case LnParam::TickDir_CW  : tk_ccw_ptr = tk_cw_ptr;  break;
//        default: break;
//        }

//        int  fst_mj_pos = -1, last_mj_pos = -1; // 20210313, added for record first and last major

//        for ( auto i = nr_min_idx; i <= nr_max_idx; i ++, mj_f_cntr ++, mn_hf_cntr ++ ) {

//           // overwite the tick size by ccw and cw, here used a pointer trick tech.
//           // Assume CCW and CW ticks are used.
//           *tk_ccw_ptr = float( ln_par.minorSize()); *tk_cw_ptr = *tk_ccw_ptr;
//           if ( mn_hf_cntr == mn_hf_rd_v ) {
//              *tk_ccw_ptr = float( ln_par.minorStepSize()); *tk_cw_ptr = *tk_ccw_ptr;
//              mn_hf_cntr = 0;
//           }
//           if ( mj_f_cntr == mj_f_rd_v ) {
//              *tk_ccw_ptr = float( ln_par.majorSize()); *tk_cw_ptr = *tk_ccw_ptr;
//              mj_f_cntr = 0;
//              if ( fst_mj_pos == -1 ) { fst_mj_pos = vtx_ofv; } // added 20210313
//              last_mj_pos = vtx_ofv;
//           }

//           // translate the math point to item coord.
//           auto pct_pos = ( tick_info.baseTickStep() * qreal(i) + ln_par.min() - rg_ofv ) * inv_rg_sz;
//           auto sc_pos  = ( bd_sz - pct_pos * bd_sz ) + bd_ofv;

//           if ( pct_pos >= 0 && pct_pos <= 1.0 ) {
//               vtx[ vtx_ofv + 0 ].set( px_pos - tk_ccw_sz, float( sc_pos ));
//               vtx[ vtx_ofv + 1 ].set( px_pos + tk_cw_sz,  float( sc_pos ));
//           } else {
//               vtx[ vtx_ofv + 0 ].set( 0, 0 );
//               vtx[ vtx_ofv + 1 ].set( 0, 0 );
//           }
//           vtx_ofv += 2;
//       }

//       if ( ! ln_par.showFirstMajor() && fst_mj_pos >= 0 ) {
//           vtx[ fst_mj_pos ].set( 0, 0 ); vtx[ fst_mj_pos + 1 ].set( 0, 0 );
//       }
//       if ( ! ln_par.showLastMajor() && last_mj_pos >= 0 ) {
//           vtx[ last_mj_pos ].set( 0, 0 ); vtx[ last_mj_pos + 1 ].set( 0, 0 );
//       }

//    } else {
//        if ( lbl_layout != nullptr ) { lbl_layout->invalidate(); }
//    }

//    markDirty( QSGNode::DirtyGeometry );
//}

//// ============================================================================
//// draw the geometry by horizontal direction
//// ============================================================================
//auto     IcQSGValueAxisNode :: qsg_updGeomHori(
//    const QRectF &bounds, const LnParam &ln_par, const QRectF &vw_port, const Material &mater, IcQuickLblLayoutSrc *lbl_layout
//) -> void
//{
//     Q_UNUSED( mater );
//     TickInfo tick_info( vw_port.x(), vw_port.right(), ln_par );

//    // ------------------------------------------------------------------------
//    // make sure has tick info. or clean the draw data
//    // ------------------------------------------------------------------------
//    if ( ! tick_info.isValid() ) { clearGeom( lbl_layout ); return; }

//    // ------------------------------------------------------------------------
//    // allocate the geometry buffer
//    // ------------------------------------------------------------------------
//    gPrepareGeom( m_geom, tick_info, ln_par );
//    if ( ! tick_info.hasTickInVrg() && ! ln_par.isShowBln()) { return; }

//    // ------------------------------------------------------------------------
//    // fill the geometry buffer
//    // ------------------------------------------------------------------------
//    auto inv_rg_sz = 1.0 / ( vw_port.isValid() ? vw_port.height() : ln_par.max() - ln_par.min() );
//    auto bd_ofv  = bounds.x(), bd_sz = bounds.width();
//    auto px_pos  = float( bounds.height() * 0.5 );
//    auto vtx     = m_geom.vertexDataAsPoint2D();
//    int  vtx_ofv = 0;

//    // a) generate the vertical base line two point
//    if ( ln_par.isShowBln() ) {   // NOTE: axis Y should be map to math coord.
//        auto rg_ofv  = ( vw_port.isValid() ? vw_port.x() : ln_par.blnMin() );
//        auto pct_pos = ( tick_info.brgMin() - rg_ofv ) * inv_rg_sz;
//        auto sc_pos  = pct_pos * bd_sz + bd_ofv;
//        vtx[0].set( float( sc_pos ), px_pos );

//        pct_pos = ( tick_info.brgMax() - rg_ofv ) * inv_rg_sz;
//        sc_pos  = pct_pos * bd_sz + bd_ofv;
//        vtx[1].set( float( sc_pos ), px_pos );

//        vtx_ofv = 2;
//    }

//    // b) generate the points for ticks
//    if ( tick_info.hasTickInVrg()) {
//        auto nr_min_idx = tick_info.nrMinTickIdxInt(); // nearest minimum tick index
//        auto nr_max_idx = tick_info.nrMaxTickIdxInt(); // nearest maxmum  tick index
//        auto rg_ofv = ( vw_port.isValid() ? vw_port.x() : ln_par.min() );

//        auto mj_f_rd_v  = ln_par.minorTickCnt() + 1;
//        auto mn_hf_rd_v = ( ln_par.minorHintStep() > 0 ? ln_par.minorHintStep() : mj_f_rd_v );
//        auto mj_f_cntr  = nr_min_idx % mj_f_rd_v;
//        auto mn_hf_cntr = nr_min_idx % mn_hf_rd_v;
//        if ( mj_f_cntr  == 0 ) { mj_f_cntr  = mj_f_rd_v; }
//        if ( mn_hf_cntr == 0 ) { mn_hf_cntr = mn_hf_rd_v; }

//        // setup label layout data
//        if ( lbl_layout != nullptr ) {
//            lbl_layout->init (
//                nr_min_idx, nr_max_idx, rg_ofv, 1.0 / inv_rg_sz, tick_info.baseTickStep(), ln_par.min(),
//                mj_f_rd_v, mn_hf_rd_v
//            );
//        }

//        // a direction trick for later use ( reduce the 'if' sentence )
//        float tk_ccw_sz = 0, tk_cw_sz = 0;
//        auto  tk_ccw_ptr = &tk_ccw_sz, tk_cw_ptr = &tk_cw_sz;
//        switch( ln_par.tickDir() ) {
//        case LnParam::TickDir_CCW : tk_cw_ptr  = tk_ccw_ptr; break;
//        case LnParam::TickDir_CW  : tk_ccw_ptr = tk_cw_ptr;  break;
//        default: break;
//        }

//        int  fst_mj_pos = -1, last_mj_pos = -1; // 20210313, added for record first and last major

//        for ( auto i = nr_min_idx; i <= nr_max_idx; i ++, mj_f_cntr ++, mn_hf_cntr ++ ) {

//           // overwite the tick size by ccw and cw, here used a pointer trick tech.
//           // Assume CCW and CW ticks are used.
//           *tk_ccw_ptr = float( ln_par.minorSize()); *tk_cw_ptr = *tk_ccw_ptr;
//           if ( mn_hf_cntr == mn_hf_rd_v ) {
//              *tk_ccw_ptr = float( ln_par.minorStepSize()); *tk_cw_ptr = *tk_ccw_ptr;
//              mn_hf_cntr = 0;
//           }
//           if ( mj_f_cntr == mj_f_rd_v ) {
//              *tk_ccw_ptr = float( ln_par.majorSize()); *tk_cw_ptr = *tk_ccw_ptr;
//              mj_f_cntr = 0;
//              if ( fst_mj_pos == -1 ) { fst_mj_pos = vtx_ofv; } // added 20210313
//              last_mj_pos = vtx_ofv;
//           }

//           // translate the math point to item coord.
//           auto pct_pos = ( tick_info.baseTickStep() * qreal(i) + ln_par.min() - rg_ofv ) * inv_rg_sz;
//           auto sc_pos  = pct_pos * bd_sz + bd_ofv;

//           if ( pct_pos >= 0 && pct_pos <= 1.0 ) {
//               vtx[ vtx_ofv + 0 ].set( float( sc_pos ), px_pos - tk_ccw_sz );
//               vtx[ vtx_ofv + 1 ].set( float( sc_pos ), px_pos + tk_cw_sz );
//           } else {
//               vtx[ vtx_ofv + 0 ].set( 0, 0 );
//               vtx[ vtx_ofv + 1 ].set( 0, 0 );
//           }
//           vtx_ofv += 2;
//       }

//       if ( ! ln_par.showFirstMajor() && fst_mj_pos >= 0 ) {
//           vtx[ fst_mj_pos ].set( 0, 0 ); vtx[ fst_mj_pos + 1 ].set( 0, 0 );
//       }
//       if ( ! ln_par.showLastMajor() && last_mj_pos >= 0 ) {
//           vtx[ last_mj_pos ].set( 0, 0 ); vtx[ last_mj_pos + 1 ].set( 0, 0 );
//       }

//    } else {
//        if ( lbl_layout != nullptr ) { lbl_layout->invalidate(); }
//    }

//    markDirty( QSGNode::DirtyGeometry );
//}



}
#endif
