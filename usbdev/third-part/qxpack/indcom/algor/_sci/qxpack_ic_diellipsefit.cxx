#ifndef QXPACK_IC_DIELLIPSEFIT_CXX
#define QXPACK_IC_DIELLIPSEFIT_CXX

#include "qxpack_ic_diellipsefit.hxx"

#include "../../eigen/Eigen/Dense"
#include "../../eigen/Eigen/Eigenvalues"
#include <cstdlib>
#include <iostream>
#include <memory>
#include <cstdio>

//#define QXPACK_IcPImplPrivTemp_new    qxpack_ic_new
//#define QXPACK_IcPImplPrivTemp_delete qxpack_ic_delete
#include "../../../common/qxpack_ic_pimplprivtemp.hpp"

#define T_PrivPtr( o )  (( IcDiEllipseFitPriv *) o )

// //////////////////////////////////////////////////////////////
//
// below type used in the ellipse, here need to declare them. 
//
// //////////////////////////////////////////////////////////////
namespace Eigen{
typedef Matrix<double,6,6>       ellMatrix6d;
typedef Matrix<double,6,1>       ellVector6d;
typedef Matrix<double,5,1>       ellGeomVect;
typedef Matrix<double,Dynamic,6> ellDesignMat;
typedef ellMatrix6d              ellScatterMat;
typedef ellMatrix6d              ellContraintMat;
typedef ellVector6d              ellParamVect;
}

namespace QxPack {

// /////////////////////////////////////////////////////////////
//
//  local functions
//
// /////////////////////////////////////////////////////////////
// ============================================================
// build the x and y vectors
// ============================================================
static void genVectorXyByVect ( Eigen::VectorXd &vx, Eigen::VectorXd &vy, const std::vector<IcDiEllipseFit::PointF> &pt_list )
{
    vx.resize ( pt_list.size() ); vy.resize ( pt_list.size() );

    std::vector<IcDiEllipseFit::PointF>::const_iterator c_itr = pt_list.cbegin(); int ofv = 0;
    while ( c_itr != pt_list.cend()) {
        const IcDiEllipseFit::PointF &pt = (* c_itr ++ );
        vx.coeffRef( ofv ) = pt.x; vy.coeffRef( ofv ) = pt.y;
        ++ ofv;        
    }
}

// =====================================================================
// build the x and y vectors by list
// =====================================================================
static void genVectorXyByList ( Eigen::VectorXd &vx, Eigen::VectorXd &vy, const std::list<IcDiEllipseFit::PointF> &pt_list )
{
    vx.resize( pt_list.size()); vy.resize( pt_list.size());

    std::list<IcDiEllipseFit::PointF>::const_iterator c_itr = pt_list.cbegin(); int ofv = 0;
    while ( c_itr != pt_list.cend()) {
        const IcDiEllipseFit::PointF &pt = (* c_itr ++ );
        vx.coeffRef( ofv ) = pt.x;  vy.coeffRef( ofv ) = pt.y;
        ++ ofv;
    }
}

// ====================================================================
// build the x and y vectors by raw data pointer
// ====================================================================
static void genVectorXyByMem ( Eigen::VectorXd &vx, Eigen::VectorXd &vy, const double *x, const double *y, int num )
{
    vx.resize( num ); vy.resize( num );
    std::memcpy( ( double *) vx.data(), x, sizeof( double ) * num );
    std::memcpy( ( double *) vy.data(), y, sizeof( double ) * num );
}

// ====================================================================
// build the x and y vector by raw data structure
// ====================================================================
static void genVectorXyByPointF( Eigen::VectorXd &vx, Eigen::VectorXd &vy, const IcDiEllipseFit::PointF *pts, int num )
{
    vx.resize( num ); vy.resize( num );
    for ( int n = 0; n < num; n ++ ) {
        vx.coeffRef( n ) = pts[ n ].x;  vy.coeffRef( n ) = pts[ n ].y;
    }
}

// =====================================================================
// normalize the data, and generate the mx,my,sx,sy
// =====================================================================
static double averageData( double *dp, int num )
{
    double avg, c_sum;
    avg = c_sum = 0;
    for ( int x = 0; x < num; x ++ ) {
        c_sum += dp[ x ];
        if ( c_sum >= DBL_MAX / 2 || c_sum <= -DBL_MAX / 2 ) {
            avg += c_sum / num; c_sum = 0;
        }
    }
    std::printf("sum: %lf\n", c_sum);

    avg += c_sum / num;
    return avg;
}

static double minData ( double *dp, int num )
{
    double min = DBL_MAX;
    for ( int x = 0; x < num; x ++ ) {
        if ( dp[ x ] < min ) { min = dp[ x ]; }
    }
    return min;
}

static double maxData ( double *dp, int num )
{
    double max = -DBL_MIN;
    for ( int x = 0; x < num; x ++ ) {
        if ( dp[ x ] > max ) { max = dp[ x ]; }
    }
    return max;
}

static void scaleData( double *dp, int num, double md, double inv_sd )
{
    for ( int x = 0; x < num; x ++ ) {
        dp[x] = ( dp[ x ] - md ) * inv_sd;
    }
}

static void normData ( double &mx, double &my, double &sx, double &sy,
                       double *vx, double *vy, int num )
{
    // ----------------------------------------------------------
    // linear normalize
    // X(norm) = ( X - X(min)) / ( X(max) - X(min))
    // X(norm) will in [0,1]

    // zero normalize
    // z = ( x - avg ) / variance
    // ----------------------------------------------------------
    mx = averageData( vx, num );
    my = averageData( vy, num );

    auto max_x = maxData( vx, num );
    auto max_y = maxData( vy, num );
    auto min_x = minData( vx, num );
    auto min_y = minData( vy, num );
    std::printf("maxData x: %lf, maxData y: %lf, minData x:%lf, minData y:%lf", max_x, max_y, min_x, min_y );

    sx = ( maxData( vx, num ) - minData( vx, num )) / 2;
    sy = ( maxData( vy, num ) - minData( vy, num )) / 2;
    scaleData( vx, num, mx, 1.0 / sx );
    scaleData( vy, num, my, 1.0 / sy );
}

// =====================================================================
// unnorm data
// =====================================================================
static void unNormData( Eigen::ellParamVect &par, const Eigen::ellVector6d &A,
                        double mx, double my, double sx, double sy )
{
    par.coeffRef( 0 ) =   A.coeff(0) * sy * sy;
    par.coeffRef( 1 ) =   A.coeff(1) * sx * sy;
    par.coeffRef( 2 ) =   A.coeff(2) * sx * sx;
    par.coeffRef( 3 ) = - 2 * A.coeff(0) * sy * sy * mx - A.coeff(1) * sx * sy * my
                        + A.coeff(3) * sx * sy * sy;
    par.coeffRef( 4 ) = - A.coeff(1) * sx * sy * mx - 2 * A.coeff(2) * sx * sx * my
                        + A.coeff(4) * sx * sx * sy;
    par.coeffRef( 5 ) =   A.coeff(0) * sy * sy * mx * mx + A.coeff(1) * sx * sy * mx * my
                        + A.coeff(2) * sx * sx * my * my - A.coeff(3) * sx * sy * sy * mx
                        - A.coeff(4) * sx * sx * sy * my + A.coeff(5) * sx * sx * sy * sy;
//    std::cout << "par=" << par << std::endl << std::endl;
}

// ============================================================
// build the design matrix
// ============================================================
static void genDesignMatrix( Eigen::ellDesignMat &m, const Eigen::VectorXd &vx, const Eigen::VectorXd &vy )
{
    // -------------------------------------------------
    // D = [ x.*x  x.*y  y.*y  x  y  ones(size(x)) ];
    // -------------------------------------------------
    m.resize ( vx.size(), 6 );
    m.setOnes( vx.size(), 6 );
    m.col(0) = vx.array() * vx.array();
    m.col(1) = vx.array() * vy.array();
    m.col(2) = vy.array() * vy.array();
    m.col(3) = vx;
    m.col(4) = vy;
}

// =====================================================================
// generate the Scatter Matrix
// =====================================================================
static void  genScatterMatrix( Eigen::ellScatterMat &s, const Eigen::ellDesignMat &d )
{
    // --------------------------------------------------
    // S = D' * D
    // --------------------------------------------------
    s = d.transpose() * d; // or use d.transpose();
}

// =====================================================================
// generate the constraint
// =====================================================================
static void  genContraintMatrix( Eigen::ellContraintMat &c)
{
    c.setZero(6,6);
    c.coeffRef( 0, 2 ) = -2;
    c.coeffRef( 1, 1 ) =  1;
    c.coeffRef( 2, 0 ) = -2;
}

// =====================================================================
// generate tempory matrix
// =====================================================================
static void  genTmpA( Eigen::Matrix3d &a, const Eigen::ellScatterMat &s )
{
    a = s.block< 3, 3 >( 0, 0 );
}
static void  genTmpB ( Eigen::Matrix3d &b, const Eigen::ellScatterMat &s )
{
    b = s.block< 3, 3 >( 0, 3 );
}
static void  genTmpC ( Eigen::Matrix3d &c, const Eigen::ellScatterMat &s )
{
    c = s.block< 3, 3 >( 3, 3 );
}
static void  genTmpD ( Eigen::Matrix3d &d, const Eigen::MatrixXd &c )
{
    d = c.block< 3, 3 >( 0, 0 );
}
static void  genTmpE ( Eigen::Matrix3d &e, const Eigen::Matrix3d &tmp_c, const Eigen::Matrix3d &tmp_b )
{
    e = tmp_c.inverse() * tmp_b.transpose();
}

// =====================================================================
// get eign value
// =====================================================================
static void  genEigen( Eigen::Matrix3d &eigD, Eigen::Matrix3d &eigV,
                       const Eigen::Matrix3d &tmpA, const Eigen::Matrix3d &tmpB,
                       const Eigen::Matrix3d &tmpD, const Eigen::Matrix3d &tmpE )
{
    Eigen::EigenSolver<Eigen::Matrix3d> es( tmpD.inverse() * ( tmpA - tmpB * tmpE ) );
    eigV = es.pseudoEigenvectors();
    eigD = es.pseudoEigenvalueMatrix();
}

// =====================================================================
// find the positive element in the index of the eigD
// =====================================================================
static int  minPositiveIndexOfEigD( const Eigen::Matrix3d &eigD )
{
    int idx = -1; double min = 1e-8;
    if ( eigD.coeff(0,0) < min &&  ! std::isinf( eigD.coeff(0,0)) ) { idx = 0; min = eigD.coeff(0,0); }
    if ( eigD.coeff(1,1) < min &&  ! std::isinf( eigD.coeff(1,1)) ) { idx = 1; min = eigD.coeff(1,1); }
    if ( eigD.coeff(2,2) < min &&  ! std::isinf( eigD.coeff(2,2)) ) { idx = 2; min = eigD.coeff(2,2); }
    return idx;
}

// =====================================================================
// convert to the geometry param
// =====================================================================
static void toGeometry( Eigen::ellGeomVect &ell, const Eigen::ellParamVect &par )
{
    double theta = 0.5 * std::atan2( par.coeff(1) + DBL_MIN, par.coeff(0) - par.coeff(2) + DBL_MIN );
    double cos_t = std::cos( theta );
    double sin_t = std::sin( theta );
    double sin_squ = sin_t * sin_t;
    double cos_squ = cos_t * cos_t;
    double cos_sin = sin_t * cos_t;

    double Ao = par.coeff(5);
    double Au = par.coeff(3) * cos_t + par.coeff(4) * sin_t;
    double Av =-par.coeff(3) * sin_t + par.coeff(4) * cos_t;
    double Auu= par.coeff(0) * cos_squ + par.coeff(2) * sin_squ + par.coeff(1) * cos_sin;
    double Avv= par.coeff(0) * sin_squ + par.coeff(2) * cos_squ - par.coeff(1) * cos_sin;

    double tuCentre = - Au / ( 2 * Auu + 1e-14);
    double tvCentre = - Av / ( 2 * Avv + 1e-14);
    double wCentre  = Ao - Auu * tuCentre * tuCentre - Avv * tvCentre * tvCentre;
    double uCentre  = tuCentre * cos_t - tvCentre * sin_t;
    double vCentre  = tuCentre * sin_t + tvCentre * cos_t;

    double Ru = -wCentre / ( Auu + 1e-14);
    double Rv = -wCentre / ( Avv + 1e-14);
    Ru = std::sqrt( std::abs(Ru)) * ( Ru >= 0 ? 1 : -1 );
    Rv = std::sqrt( std::abs(Rv)) * ( Rv >= 0 ? 1 : -1 );

    theta *= 180.0 / 3.141592653589793;

    if ( Ru < Rv ) {
       double t = Ru; Ru = Rv; Rv = t;
       theta += 90.0;
    }
    if ( theta <= -180 ) { theta += 360; }
    if ( theta >=  180 ) { theta -= 180; }
    theta *= 3.141592653589793 / 180.0;

    ell << uCentre, vCentre, Ru, Rv, theta;
}

// ==============================================================
// build ellipse parameter
// ==============================================================
static void  buildEllipse(  Eigen::VectorXd &x, Eigen::VectorXd &y, double ell_data[5] )
{
   // ...
    double mx, my, sx, sy;
    std::printf("x: %lf,%lf,%lf \n",x[0], x[1], x[2]);
    std::printf("y: %lf,%lf,%lf \n",y[0], y[1], y[2]);
    normData( mx, my, sx, sy, ( double *)x.data(), ( double *)y.data(), x.size() );

    std::printf("after normData\n");
    std::printf("x: %lf,%lf,%lf \n",x[0], x[1], x[2]);
    std::printf("y: %lf,%lf,%lf \n",y[0], y[1], y[2]);
    std::printf("mx:%lf, my:%lf, sx:%lf, sy:%lf\n", mx, my, sx, sy );

   // try get the  eigen value
   Eigen::ellDesignMat    D; genDesignMatrix   ( D, x, y );
   Eigen::ellScatterMat   S; genScatterMatrix  ( S, D );
   Eigen::ellContraintMat C; genContraintMatrix( C );
   Eigen::Matrix3d tmpA; genTmpA( tmpA, S );
   Eigen::Matrix3d tmpB; genTmpB( tmpB, S );
   Eigen::Matrix3d tmpC; genTmpC( tmpC, S );
   Eigen::Matrix3d tmpD; genTmpD( tmpD, C );
   Eigen::Matrix3d tmpE; genTmpE( tmpE, tmpC, tmpB );
   Eigen::Matrix3d eigD, eigV;

   std::printf("tmp a: %lf,%lf,%lf,\n", tmpA.coeffRef(0,0), tmpA.coeffRef(0,1), tmpA.coeffRef(0,2) );

   genEigen( eigD, eigV, tmpA, tmpB, tmpD, tmpE );

   // generate the paramter
   Eigen::ellVector6d A;
   int min_eig_pos = minPositiveIndexOfEigD( eigD );
   if ( min_eig_pos < 0 ) {
       QXPACK_IC_FATAL( "QxPack::IcDiEllipseFit::CTOR() could not found the right eigen value.");
   }
   Eigen::Vector3d A1  = eigV.col( min_eig_pos );
   Eigen::Vector3d A2 = - tmpE * A1;
   A << A1, A2;

   // unnorm
   Eigen::ellParamVect par;
   unNormData( par, A, mx, my, sx, sy );
   std::printf("par: %e,%e,%e, %e,%e, %e\n", par[0], par[1], par[2], par[3], par[4],par[5] );

   // convert the geometric radii, and centers.
   Eigen::ellGeomVect ell;
   toGeometry( ell, par );

   // fill result
   ell_data[0] = ell.coeff(0);  ell_data[1] = ell.coeff(1);
   ell_data[2] = ell.coeff(2);  ell_data[3] = ell.coeff(3);
   ell_data[4] = ell.coeff(4);
}

// //////////////////////////////////////////////////////////////
//
// attachable private class
//
// //////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN  IcDiEllipseFitPriv : public QxPack::IcPImplPrivTemp< IcDiEllipseFitPriv > {
private:
    double m_cx, m_cy, m_u, m_v, m_theta_rad;
protected:
    void  doFitByEigenVect( Eigen::VectorXd &x, Eigen::VectorXd &y );
public :
    IcDiEllipseFitPriv( );
    IcDiEllipseFitPriv( const IcDiEllipseFitPriv & );
    virtual ~IcDiEllipseFitPriv( ) { }

    inline double cx() const { return m_cx; }
    inline double cy() const { return m_cy; }
    inline double ru() const { return m_u;  }
    inline double rv() const { return m_v;  }
    inline double thetaRadius() const { return m_theta_rad; }

    void  doFit( const std::vector<IcDiEllipseFit::PointF> & );
    void  doFit( const std::list<IcDiEllipseFit::PointF> & );
    void  doFit( const double *x, const double *y, int num );
    void  doFit( const IcDiEllipseFit::PointF *pts, int num );
};

// =============================================================
// CTOR
// =============================================================
    IcDiEllipseFitPriv :: IcDiEllipseFitPriv ( )
{
    m_cx = m_cy = m_u = m_v = m_theta_rad = 0;
}

// =============================================================
// CTOR ( Copy )
// =============================================================
    IcDiEllipseFitPriv :: IcDiEllipseFitPriv ( const IcDiEllipseFitPriv &other )
{
    m_cx = other.m_cx; m_cy = other.m_cy;
    m_u  = other.m_u;  m_v  = other.m_v;
    m_theta_rad = other.m_theta_rad;
}

// =============================================================
// do the fit by eigen vector
// =============================================================
void  IcDiEllipseFitPriv :: doFitByEigenVect( Eigen::VectorXd &vx, Eigen::VectorXd &vy )
{
    double ell_param[5];
    buildEllipse( vx, vy, ell_param );
    m_cx = ell_param[0]; m_cy = ell_param[1];
    m_u  = ell_param[2]; m_v  = ell_param[3];
    m_theta_rad = ell_param[4];
}

// =============================================================
// do fitting by std::vect
// =============================================================
void  IcDiEllipseFitPriv :: doFit ( const std::vector<IcDiEllipseFit::PointF> &pt_list )
{
    Eigen::VectorXd vx, vy;
    genVectorXyByVect( vx, vy, pt_list );
    this->doFitByEigenVect( vx, vy );
}

// ===========================================================
// do fitting by std::list
// ===========================================================
void  IcDiEllipseFitPriv :: doFit( const std::list<IcDiEllipseFit::PointF> &pt_list )
{
    Eigen::VectorXd vx, vy;
    genVectorXyByList( vx, vy, pt_list);
    this->doFitByEigenVect( vx, vy );
}

// ===========================================================
// do fitting by mem..
// ===========================================================
void  IcDiEllipseFitPriv :: doFit( const double *x, const double *y, int num )
{
    Eigen::VectorXd vx, vy;
    genVectorXyByMem( vx, vy, x, y, num );
    this->doFitByEigenVect( vx, vy );
}

// ==========================================================
// do fitting by structure
// ==========================================================
void  IcDiEllipseFitPriv :: doFit( const IcDiEllipseFit::PointF *pts, int num )
{
    Eigen::VectorXd vx, vy;
    genVectorXyByPointF( vx, vy, pts, num );
    this->doFitByEigenVect( vx, vy );
}


// /////////////////////////////////////////////////////////////
//
//  Ellipse fit ( direct method )
//
// /////////////////////////////////////////////////////////////
// =============================================================
// CTOR
// =============================================================
    IcDiEllipseFit :: IcDiEllipseFit ( ) { m_obj = nullptr; }

    IcDiEllipseFit :: IcDiEllipseFit ( const IcDiEllipseFit &other )
{
    m_obj = nullptr; IcDiEllipseFitPriv::attach( & m_obj, ( void **) &other.m_obj );
}
IcDiEllipseFit &   IcDiEllipseFit :: operator = ( const IcDiEllipseFit &other )
{
    IcDiEllipseFitPriv::attach( & m_obj, ( void **) &other.m_obj );
    return *this;
}
    IcDiEllipseFit :: IcDiEllipseFit ( const std::vector<PointF> & pt_list )
{
    m_obj = nullptr;
    if ( pt_list.size() < 5 ) { return; } // the ellipse need more than 5 points
    IcDiEllipseFitPriv::buildIfNull( & m_obj );
    T_PrivPtr( m_obj )->doFit( pt_list );
}
    IcDiEllipseFit :: IcDiEllipseFit( const std::list<PointF> &pt_list )
{
    m_obj = nullptr;
    if ( pt_list.size() < 5 ) { return; }
    IcDiEllipseFitPriv::buildIfNull( & m_obj );
    T_PrivPtr( m_obj )->doFit( pt_list );
}
    IcDiEllipseFit :: IcDiEllipseFit( const double *x, const double *y, int num )
{
    m_obj = nullptr;
    if ( num < 5 ) { return; }
    IcDiEllipseFitPriv::buildIfNull( & m_obj );
    T_PrivPtr( m_obj )->doFit( x, y, num );
}
    IcDiEllipseFit :: IcDiEllipseFit( const IcDiEllipseFit::PointF *pt, int num )
{
    m_obj = nullptr;
    if ( num < 5 ) { return; }
    IcDiEllipseFitPriv::buildIfNull( & m_obj );
    T_PrivPtr( m_obj )->doFit( pt, num );
}

// =============================================================
// DTOR
// =============================================================
    IcDiEllipseFit :: ~IcDiEllipseFit ( )
{
    if ( m_obj != nullptr ) {
        IcDiEllipseFitPriv::attach( & m_obj, nullptr);
    }
}

// ==============================================================
// function access
// ==============================================================
bool    IcDiEllipseFit :: isEmpty( ) const { return ( m_obj == nullptr ); }
double  IcDiEllipseFit :: cx( ) const { return ( m_obj != nullptr ? T_PrivPtr( m_obj )->cx() : 0 ); }
double  IcDiEllipseFit :: cy( ) const { return ( m_obj != nullptr ? T_PrivPtr( m_obj )->cy() : 0 ); }
double  IcDiEllipseFit :: ru( ) const { return ( m_obj != nullptr ? T_PrivPtr( m_obj )->ru() : 0 ); }
double  IcDiEllipseFit :: rv( ) const { return ( m_obj != nullptr ? T_PrivPtr( m_obj )->rv() : 0 ); }
double  IcDiEllipseFit :: thetaRadius( ) const { return ( m_obj != nullptr ? T_PrivPtr( m_obj )->thetaRadius() : 0 ); }

}

#endif
