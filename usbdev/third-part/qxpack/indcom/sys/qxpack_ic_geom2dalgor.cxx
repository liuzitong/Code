#ifndef QXPACK_IC_GEOM2DALGOR_CXX
#define QXPACK_IC_GEOM2DALGOR_CXX

#include "qxpack_ic_geom2dalgor.hxx"
#include <QDebug>

namespace QxPack {
namespace IcGeom2DAlgor {

// ////////////////////////////////////////////////////////////////////////////
//
// API: make the Consecutive duplicate points information
//
// ////////////////////////////////////////////////////////////////////////////
QXPACK_IC_API QPair<QList<QPointF>,QList<int>>  mergeCDP( const QList<QPointF> &lst )
{
    if ( lst.size() < 2 ) { // just one point, need not to process
        return QPair<QList<QPointF>,QList<int>>( lst, QList<int>());
    }

    // check if has CDP
    bool found_cdp = false;
    auto p_itr = lst.constBegin();  auto c_itr = lst.constBegin() + 1;
    while ( c_itr != lst.constEnd()) {
        if ( *( c_itr ++ ) == *( p_itr ++ )) { found_cdp = true; break; }
    }
    if ( ! found_cdp ) { return QPair<QList<QPointF>,QList<int>>( lst, QList<int>()); }

    // make a new list
    int total_sz = lst.size();
    QList<QPointF> rsl; rsl.reserve( total_sz );
    QList<int>     rp;  rp.reserve ( total_sz );
    c_itr = lst.constBegin() + 1;  p_itr = lst.constBegin();

    rsl.append( *p_itr ); int rp_tm = 1;
    while ( c_itr != lst.constEnd()) {
        if ( *c_itr == *p_itr ) {
            ++ c_itr; ++ rp_tm;
        } else {                    // end for loop CDP, try next
            rp.append( rp_tm );
            rsl.append( *c_itr ); p_itr = c_itr ++; rp_tm = 1;
        }
    }
    rp.append( rp_tm );

    return QPair<QList<QPointF>,QList<int>>( rsl, rp );
}



// ////////////////////////////////////////////////////////////////////////////
//
// API: make polygon line contour
//
// ////////////////////////////////////////////////////////////////////////////
inline static void  makePolLnContour_ccwAndCwLine( const IcLine2Dfp &ln, IcLine2Dfp &ln_ccw, IcLine2Dfp &ln_cw, const double &tk )
{
    IcVector2Dfp ccw_vec = ln.vector().ccwVertVector( true ).scaled( tk * 0.5 );
    ln_ccw = ln.moved( ccw_vec );
    ln_cw  = ln.moved( ccw_vec.scale( -1 ) );
}

QXPACK_IC_API QList<QPointF>  makeSoildPolLnContour(  const QList<QPointF> &src, const double &ln_tk, bool req_endp_bold )
{
    int src_sz  = src.size();

    // ------------------------------------------------------------------------
    // the input series points maybe has Consecutive duplicate points, we need to
    // process it
    // ------------------------------------------------------------------------
    QPair<QList<QPointF>,QList<int>> cdp_pair = mergeCDP( src );
    const QList<QPointF> &pt_lst = cdp_pair.first;
    int pts_num = pt_lst.size();

    // ------------------------------------------------------------------------
    // check the case type
    // ------------------------------------------------------------------------
    enum CaseID { CaseID_None = 0, CaseID_3PtsThicknessLess1, CaseID_1or2Pts, CaseID_3PtsNorm  };
    CaseID case_id = CaseID_None;
    if        ( pts_num < 1 ) {    // no points, need not to process
        case_id = CaseID_None;
    } else if ( ln_tk  <= 1 ) {    // bad thickness, just do points copy
        case_id = ( pts_num > 2 ? CaseID_3PtsThicknessLess1 : CaseID_1or2Pts );
    } else {                       // thickness > 1 and pts > 1
        case_id = ( pts_num > 2 ? CaseID_3PtsNorm : CaseID_1or2Pts );
    }

    // ------------------------------------------------------------------------
    // process merged CDP array
    // ------------------------------------------------------------------------
    QList<QPointF> rsl; rsl.reserve( src_sz * 8 );
    switch( case_id ) {
    case CaseID_1or2Pts : {
        IcPoint2Dfp rsl_pts[4];
        if ( pts_num > 1 ) {  // just one point, generate 4 points
            IcGeom2DMath::boldLine ( rsl_pts, IcLine2Dfp ( pt_lst.first().x(), pt_lst.first().y(), pt_lst.last().x(), pt_lst.last().y() ), ln_tk, req_endp_bold );
        } else {              // just one line, generate 4 points like one point
            IcGeom2DMath::boldPoint( rsl_pts, IcPoint2Dfp( pt_lst.first().x(), pt_lst.first().y()), ln_tk );
        }
        for ( int i = 0; i < int( sizeof(rsl_pts)/sizeof(rsl_pts[0])); i ++ )
        { rsl.append( QPointF( rsl_pts[i].rx(), rsl_pts[i].ry() )); }
    } break;

    case CaseID_3PtsThicknessLess1 : {  // there are 3 points, but thickness < 1.0
        auto c_itr = pt_lst.constBegin();
        while ( c_itr != pt_lst.constEnd()) {
            rsl.append( *( c_itr )); rsl.append( *( c_itr ));
            ++ c_itr;
        }
    } break;

    case CaseID_3PtsNorm : { // normal case

        auto f_itr = pt_lst.constBegin();
        auto c_itr = pt_lst.constBegin() + 1; auto n_itr = pt_lst.constBegin() + 2;
        auto e_itr = pt_lst.constEnd()   - 1;
        IcLine2Dfp ln_p1p2, ln_p2p3, ln_ccw_sc, ln_cw_sc; double ln_len_sc_fact;
        IcLine2Dfp ln_p1p2_ccw, ln_p1p2_cw, ln_p2p3_ccw, ln_p2p3_cw;

        // push first point pair
        ln_p1p2 = IcLine2Dfp( f_itr->x(), f_itr->y(), c_itr->x(), c_itr->y());
        makePolLnContour_ccwAndCwLine( ln_p1p2, ln_p1p2_ccw, ln_p1p2_cw, ln_tk ); // endpoint do not scaled.
        if ( req_endp_bold ) {
            ln_len_sc_fact = ( ! ln_p1p2.isZeroLength() ? ln_tk / ln_p1p2.length() + 1 : 1 );
            ln_ccw_sc = ln_p1p2_ccw.scaled( ln_len_sc_fact );
            ln_cw_sc  = ln_p1p2_cw.scaled ( ln_len_sc_fact );
        } else {
            ln_ccw_sc = ln_p1p2_ccw; ln_cw_sc = ln_p1p2_cw;
        }
        rsl << QPointF( ln_ccw_sc.rx1(), ln_ccw_sc.ry1()) << QPointF( ln_cw_sc.rx1(), ln_cw_sc.ry1());

        // c_itr in [ 1 to last )
        while ( c_itr != e_itr ) {
            ln_p2p3 = IcLine2Dfp( c_itr->x(), c_itr->y(), n_itr->x(), n_itr->y());
            makePolLnContour_ccwAndCwLine( ln_p2p3, ln_p2p3_ccw, ln_p2p3_cw, ln_tk );

            if        ( ln_p2p3.isAtCCWdirOf( ln_p1p2 )) {
                rsl << QPointF( ln_p1p2_ccw.rx2(), ln_p1p2_ccw.ry2() ) << QPointF( ln_p1p2_cw.rx2(), ln_p1p2_cw.ry2());
                rsl << QPointF( ln_p1p2.rx2(), ln_p1p2.ry2())          << QPointF( ln_p1p2_cw.rx2(), ln_p1p2_cw.ry2());
                rsl << QPointF( ln_p1p2.rx2(), ln_p1p2.ry2())          << QPointF( ln_p2p3_cw.rx1(), ln_p2p3_cw.ry1());
                rsl << QPointF( ln_p2p3_ccw.rx1(), ln_p2p3_ccw.ry1())  << QPointF( ln_p2p3_cw.rx1(), ln_p2p3_cw.ry1() );
            } else if ( ln_p2p3.isAtCWdirOf( ln_p1p2 ))  {
                rsl << QPointF( ln_p1p2_ccw.rx2(), ln_p1p2_ccw.ry2() ) << QPointF( ln_p1p2_cw.rx2(), ln_p1p2_cw.ry2() );
                rsl << QPointF( ln_p1p2_ccw.rx2(), ln_p1p2_ccw.ry2() ) << QPointF( ln_p1p2.rx2(), ln_p1p2.ry2());
                rsl << QPointF( ln_p2p3_ccw.rx1(), ln_p2p3_ccw.ry1() ) << QPointF( ln_p1p2.rx2(), ln_p1p2.ry2());
                rsl << QPointF( ln_p2p3_ccw.rx1(), ln_p2p3_ccw.ry1() ) << QPointF( ln_p2p3_cw.rx1(), ln_p2p3_cw.ry1());
            } else {
                rsl << QPointF( ln_p1p2_ccw.rx2(), ln_p1p2_ccw.ry2() ) << QPointF( ln_p1p2_cw.rx2(),  ln_p1p2_cw.ry2());
            }

            ln_p1p2 = ln_p2p3; ln_p1p2_ccw = ln_p2p3_ccw; ln_p1p2_cw = ln_p2p3_cw;
            ++ c_itr; ++ n_itr;
        }

        // c_itr is [ last ], the last point, is a endpoint
        if ( req_endp_bold ) {
            ln_len_sc_fact = ( ! ln_p2p3.isZeroLength() ? ln_tk / ln_p2p3.length() + 1 : 1 );
            ln_ccw_sc = ln_p2p3_ccw.scaled( ln_len_sc_fact );
            ln_cw_sc  = ln_p2p3_cw.scaled ( ln_len_sc_fact );
        } else {
            ln_ccw_sc  = ln_p2p3_ccw; ln_cw_sc = ln_p2p3_cw;
        }
        rsl << QPointF( ln_ccw_sc.rx2(), ln_ccw_sc.ry2() );
        rsl << QPointF( ln_cw_sc.rx2(),  ln_cw_sc.ry2()  );

    } break;

    default: rsl = src; // none
    }

    return rsl;
}


}
}

#endif
