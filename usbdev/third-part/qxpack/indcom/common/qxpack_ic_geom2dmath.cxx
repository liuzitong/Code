#ifndef QXPACK_IC_GEOM2DMATH_CXX
#define QXPACK_IC_GEOM2DMATH_CXX

#include "qxpack_ic_geom2dmath.hxx"
#include "qxpack_ic_geom2d.hxx"

namespace QxPack {
namespace IcGeom2DMath {

//! @brief vector 2d dot product of AB.BC
//! @return the scalar value
static inline  double  dotABBC ( const IcPoint2Dfp &a, const IcPoint2Dfp &b, const IcPoint2Dfp &c )
{
    double ab_x = b.x() - a.x(); double ab_y = b.y() - a.y();
    double bc_x = c.x() - b.x(); double bc_y = c.y() - b.y();
    return double( ab_x * bc_x + ab_y * bc_y );
}

//! @brief vector 2d cross product of AB.AC
//! @return  the preso vector value
static inline  double  crossABAC ( const IcPoint2Dfp &a, const IcPoint2Dfp &b, const IcPoint2Dfp &c )
{
    double ab_x = b.x() - a.x(); double ab_y = b.y() - a.y();
    double ac_x = c.x() - a.x(); double ac_y = c.y() - a.y();
    return double(  ab_x * ac_y - ab_y * ac_x );
}

// ////////////////////////////////////////////////////////////////////////////
//                local  objects
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN  sIcLineFormula {
private:   double m_a, m_b, m_c;
public :
   sIcLineFormula ( const double &a, const double &b, const double &c )
   : m_a(a), m_b(b), m_c(c){ }

   explicit sIcLineFormula ( const IcPoint2Dfp &pt_a, const IcPoint2Dfp &pt_b )
   {
       // -------------------------------------------------
       // in topcoder doc. used Ax + By = C
       // A = y2 - y1;  B = x1 - x2;  C = A * x1 + B * y1;
       // -------------------------------------------------
       m_a = pt_b.y() - pt_a.y();
       m_b = pt_a.x() - pt_b.x();
       m_c = m_a * pt_a.x() + m_b * pt_a.y();
   }
   inline double &  A() { return m_a; }
   inline double &  B() { return m_b; }
   inline double &  C() { return m_c; }
};


// ////////////////////////////////////////////////////////////////////////////
//
//                 public  APIs
//
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// two points length
// ============================================================================
QXPACK_IC_API double  distAB ( const IcPoint2Dfp &a, const IcPoint2Dfp &b )
{
    double dx = a.x() - b.x();  double dy = a.y() - b.y();
    dx = abs( dx ); dy = abs( dy );
    if ( dx > dy ) { dy /= dx; return dx * std::sqrt( 1.0 + dy * dy ); }
    if ( dy > 0 )  { dx /= dy; return dy * std::sqrt( 1.0 + dx * dx ); }
    return 0;
}

// ============================================================================
// length of line to a point
// ============================================================================
QXPACK_IC_API  double  distLnToPt( const IcLine2Dfp &ab, const IcPoint2Dfp &c, double *dist_ab_val )
{
    double dist_ab = ( dist_ab_val != nullptr ? *dist_ab_val : ab.length() );
    double rsl = ( ! fuzzyIsNull( dist_ab ) ? crossABAC( ab.p1(), ab.p2(), c ) / dist_ab: DBL_MAX );
    return abs( rsl );
}

// ============================================================================
// length of segment to a point
// ============================================================================
QXPACK_IC_API double   distSegToPt( const IcLine2Dfp &ab, const IcPoint2Dfp &c, double *dist_ab_val )
{
    double dist_ab = ( dist_ab_val != nullptr ? *dist_ab_val : ab.length() );
    IcPoint2Dfp a = ab.p1(); IcPoint2Dfp b = ab.p2();

    double rsl = ( ! fuzzyIsNull( dist_ab ) ? crossABAC( a, b, c ) / dist_ab: DBL_MAX );

    if ( dotABBC( a, b, c ) > 0 ) { // vect.AB, vect.BC they are same direction or not, if same, C is nearest to B
        rsl = distAB( b, c );
    } else if ( dotABBC( b, a, c ) > 0 ){ // C is nearest to A
        rsl = distAB( a, c );
    } else { }  // C is perpendicular to A or B
    return abs( rsl );
}

// ============================================================================
// calc the vertical vector of current vector
// ============================================================================
QXPACK_IC_API  IcVector2Dfp  ccwVertVector ( const IcVector2Dfp &vec, bool req_norm )
{
    // ------------------------------------------------------------------------
    //    A dot B = A.x * B.x + A.y * B.y  ( result is 0 means vertical )
    // let vec is (A):
    //   a) assume B.x == A.x, got
    //      A.x * B.x + A.y * B.y = 0   ==>  A.x * A.x = - A.y * B.y
    //      B.y = -( A.x * A.x / A.y )
    // ------------------------------------------------------------------------
    double rsl_x = 0, rsl_y = 0;
    if ( fuzzyIsNull( vec.x() ) ){
        if ( fuzzyIsNull( vec.y() )) { return vec; } // zero vector
        rsl_x = double( -vec.y()); rsl_y = 0;

    } else if ( fuzzyIsNull( vec.y() )) {
        rsl_x = 0; rsl_y = double( vec.x());

    } else { // x & y not zero
        rsl_x = double( vec.x());
        double rsl_t = rsl_x / double( vec.y());
        rsl_y = -( rsl_t * rsl_t * vec.y() );

        if ( vec.x() > 0 ) {
            if ( vec.y() > 0 ) {
                rsl_x = -rsl_x;  rsl_y = abs(rsl_y); // rsl: -x, +y
            } else {
                rsl_y = abs(rsl_y);  // rsl: +x, +y
            }
        } else { // vec.x() < 0
            if ( vec.y() > 0 ) {
                ;    // rsl: -x, -y
            } else { // rsl: +x, -y
                rsl_x = -rsl_x; rsl_y = ( rsl_y < 0 ? rsl_y : -rsl_y );
            }
        }
    }

    if ( req_norm ) {
        return IcVector2Dfp( rsl_x, rsl_y ).normalize();
    } else {
        return IcVector2Dfp( rsl_x, rsl_y );
    }
}

// ============================================================================
// find the cross point of two line
// ============================================================================
QXPACK_IC_API int   findCrossPoint( IcPoint2Dfp &pt, const IcLine2Dfp &ln_a, const IcLine2Dfp &ln_b )
{
    sIcLineFormula ln_pa( ln_a.p1(), ln_a.p2() );
    sIcLineFormula ln_pb( ln_b.p1(), ln_b.p2() );

    #define LN_A1 ln_pa.A()
    #define LN_B1 ln_pa.B()
    #define LN_C1 ln_pa.C()
    #define LN_A2 ln_pb.A()
    #define LN_B2 ln_pb.B()
    #define LN_C2 ln_pb.C()

    double det = double( LN_A1 * LN_B2 - LN_A2 * LN_B1 ); // det = ( A1 * B2 - A2 * B1 )

    if ( fuzzyIsNull( det )) {
        // lines are parallel, check if exist overlapped point
        pt = IcPoint2Dfp();  // NOT FOUND cross point. lines are parallel
        return -1;

    } else {
        double x = double( LN_B2 * LN_C1 - LN_B1 * LN_C2 ) / det; // ( B2 * C1 - B1 * C2 ) / det
        double y = double( LN_A1 * LN_C2 - LN_A2 * LN_C1 ) / det; // ( A1 * C2 - A2 * C1 ) / det

        bool is_on_ln_a = ( x >= min( ln_a.x1(), ln_a.x2() ))  &&  ( x <= max( ln_a.x1(), ln_a.x2() )) &&
                          ( y >= min( ln_a.y1(), ln_a.y2() ))  &&  ( y <= max( ln_a.y1(), ln_a.y2() ));

        bool is_on_ln_b = ( x >= min( ln_b.x1(), ln_b.x2() ))  &&  ( x <= max( ln_b.x1(), ln_b.x2() )) &&
                          ( y >= min( ln_b.y1(), ln_b.y2() ))  &&  ( y <= max( ln_b.y1(), ln_b.y2() ));

        pt = IcPoint2Dfp( qxreal_t( x ), qxreal_t( y ));
        if ( is_on_ln_a ) {
            return ( is_on_ln_b ? 3 : 1 );
        } else if ( is_on_ln_b ) {
            return 2;
        } else {
            return 0;
        }
    }

    #undef LN_A1
    #undef LN_B1
    #undef LN_C1
    #undef LN_A2
    #undef LN_B2
    #undef LN_C2
}

// ============================================================================
// bold a point
// ============================================================================
QXPACK_IC_API void  boldPoint( IcPoint2Dfp pts[4], const IcPoint2Dfp &pt, const double &thickness )
{
    if ( thickness > 1 ) {
        double half_tk = thickness * 0.5;
        double neg_x = pt.x() - half_tk;  double pos_x = pt.x() + half_tk;
        double neg_y = pt.y() - half_tk;  double pos_y = pt.y() + half_tk;
        pts[0].set( qxreal_t( neg_x ), qxreal_t( pos_y ) );
        pts[1].set( qxreal_t( neg_x ), qxreal_t( neg_y ) );
        pts[2].set( qxreal_t( pos_x ), qxreal_t( pos_y ) );
        pts[3].set( qxreal_t( pos_x ), qxreal_t( neg_y ) );
    } else {
        pts[0] = pt; pts[1] = pt; pts[2] = pt; pts[3] = pt;
    }
}

// ============================================================================
// bold a line
// ============================================================================
QXPACK_IC_API void  boldLine( IcPoint2Dfp pts[4], const IcLine2Dfp &s_ln, const double &thickness, bool req_endp_bold )
{
    if ( thickness > 1 ) {
        if ( ! s_ln.isZeroLength() ) {
            IcLine2Dfp ln = IcLine2Dfp(
                s_ln.p1ConstRef().x(), s_ln.p1ConstRef().y(), s_ln.p2ConstRef().x(), s_ln.p2ConstRef().y()
            );
            IcLine2Dfp ln_act;
            if ( req_endp_bold ) {
                double sc_fact = thickness / ln.length() + 1.0;
                ln_act = ln.scaled( sc_fact );
            } else {
                ln_act = ln;
            }
            IcVector2Dfp ccw_vec = ln_act.vector().ccwVertVector( true ).scaled(( thickness ) * 0.5 );
            IcVector2Dfp cw_vec  = ccw_vec.scaled( -1 );
            IcLine2Dfp ccw_ln = ln_act.moved( ccw_vec );
            IcLine2Dfp cw_ln  = ln_act.moved( cw_vec  );

            pts[0] = ccw_ln.p1();   pts[1] = cw_ln.p1();
            pts[2] = ccw_ln.p2();   pts[3] = cw_ln.p2();

        } else { // like a point
            boldPoint( pts, s_ln.p1(), thickness );
        }
    } else {
        pts[0] = s_ln.p1();  pts[1] = s_ln.p1();
        pts[2] = s_ln.p2();  pts[3] = s_ln.p2();
    }
}



}
}

#endif
