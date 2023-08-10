#ifndef QXPACK_IC_ALGOR_ELLFIT_HPP
#define QXPACK_IC_ALGOR_ELLFIT_HPP

#include <cstring>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <cmath>
#include <cstdio>
#include <cfloat>
#include <new> // c++11 replacement new

#include "qxpack_ic_algor_base.hpp"
#include "eigen/Eigen/Dense"
#include "eigen/Eigen/Eigenvalues"
#include "qxpack_ic_algor_alignmb.hpp"

#ifndef QXPACK_IC_CXX11
#if __cplusplus >= 201103L || ( defined(_MSC_VER) && _MSC_VER >= 1800 )
#  define QXPACK_IC_CXX11
#else
#  error "this template require C++11 !"
#endif
#endif

// ////////////////////////////////////////////////////////////////////////////
// : API List
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
// ////////////////////////////////////////////////////////////////////////////

namespace QxPack {
namespace IcAlgor {

// ////////////////////////////////////////////////////////////////////////////
// some helper functions
// ////////////////////////////////////////////////////////////////////////////
namespace EllFitHlp {

using EllMatrix6d   = Eigen::Matrix<double,6,6>;
using EllVector6d   = Eigen::Matrix<double,6,1>;
using EllGeomVect   = Eigen::Matrix<double,5,1>;
using EllDesignMat  = Eigen::Matrix<double,Eigen::Dynamic,6>;
using EllScatterMat = EllMatrix6d;
using EllParamVect  = EllVector6d;

struct NormDataRsl { double mx, my, sx, sy; };

}

class EllFitWorkspace {
private:
    struct InnerObj {
//        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        EllFitHlp::EllScatterMat scat_mat;
        Eigen::Matrix3d tmp_a, tmp_b, tmp_c, tmp_d, tmp_e, eig_d, eig_v;
        EllFitHlp::EllVector6d   A;
        EllFitHlp::EllParamVect  par;
        EllFitHlp::EllGeomVect   geom;
    };
    AlignedMemBlock  m_align_mb;  InnerObj *obj;
private:
    EllFitWorkspace( const EllFitWorkspace & );
    EllFitWorkspace& operator = ( const EllFitWorkspace & );
public :
    explicit EllFitWorkspace( ) : m_align_mb( 16, sizeof(InnerObj))
    {
        obj = reinterpret_cast<InnerObj*>( m_align_mb.dataPtr());
        new(obj) InnerObj;
    }
    ~EllFitWorkspace() { obj->~InnerObj(); }
    inline auto member() -> InnerObj* { return obj; }
};

template <typename T>
class  EllFitResult {
private:
    T  m_dat[5]; // m_cx, m_cy, m_ru, m_rv, m_angle_rad;
public :
    explicit EllFitResult ( ) { m_dat[0] = m_dat[1] = m_dat[2] = m_dat[3] = m_dat[4] = 0; }
    EllFitResult( const EllFitResult &ot ) {
        m_dat[0] = ot.m_dat[0]; m_dat[1] = ot.m_dat[1];
        m_dat[2] = ot.m_dat[2];
        m_dat[3] = ot.m_dat[3]; m_dat[4] = ot.m_dat[4];
    }
    EllFitResult & operator = ( const EllFitResult &ot )
    {
        m_dat[0] = ot.m_dat[0]; m_dat[1] = ot.m_dat[1];
        m_dat[2] = ot.m_dat[2];
        m_dat[3] = ot.m_dat[3]; m_dat[4] = ot.m_dat[4];
    }

    inline auto  isValid()-> bool { return ( m_dat[2] > 0 && m_dat[3] > 0 ); }
    inline auto  cx() -> const T& { return m_dat[0]; }
    inline auto  cy() -> const T& { return m_dat[1]; }
    inline auto  ru() -> const T& { return m_dat[2]; }
    inline auto  rv() -> const T& { return m_dat[3]; }
    inline auto  angleRad()-> const T& { return m_dat[4]; }
    inline auto  angleDeg()-> T     { return m_dat[4] * 180.0 / 3.141592653589793; }
    inline auto  bufferPtr()  -> T* { return &m_dat[0]; }
};

template <typename T>
struct  EllFitPoint {
    T x, y;
    EllFitPoint( const T& _x, const T& _y) : x(_x), y(_y){}
};


namespace EllFitHlp {

template <typename T> // NOTE: the dp should be jump 1 element
auto     genVecBySeriData( Eigen::VectorXd &v, const T *dp, size_t jn ) -> void
{
    v.resize( int(jn) );
    auto m4 = jn / 8 * 8; size_t i = 0; auto p = dp;
    auto v_dp = const_cast<double*>( v.data());

    for ( ; i < m4; i += 8, p += 2 * 8, v_dp += 8 ) {
        v_dp[0] = double( p[0] ); v_dp[1] = double( p[2 ] );
        v_dp[2] = double( p[4] ); v_dp[3] = double( p[6 ] );
        v_dp[4] = double( p[8] ); v_dp[5] = double( p[10] );
        v_dp[6] = double( p[12]); v_dp[7] = double( p[14] );
    }

    v_dp = const_cast<double*>( v.data());
    for ( ; i < jn; i ++, p += 2 ) { v_dp[i] = double( p[0] ); }
}

template <typename T, typename U>
auto     genVecXyByPts( Eigen::VectorXd &vx, Eigen::VectorXd &vy, const EllFitPoint<U> *pts, size_t num ) -> void
{
    genVecBySeriData( vx, & pts->x, num );
    genVecBySeriData( vy, & pts->y, num );
}

template <typename T>
auto     avgData ( const T *dat, size_t num ) -> double
{
     auto m4 = num / 4 * 4; size_t i = 0; auto p = dat;
     double sum[2]; sum[0] = 0; sum[1] = 0;

     for ( ; i < m4; i += 4, p += 4 ) {
         sum[0] += p[0] + p[1]; sum[1] += p[2] + p[3];
     }
     for ( ; i < num; i ++ ) { sum[0] += dat[i]; }

     return sum[0] / num  + sum[1] / num;
}

template <typename T>
auto     minData ( const T *dat, size_t num ) -> double
{
    auto min_p = &dat[0];
    for ( size_t i = 1; i < num; i ++ ) {
        if ( *min_p > dat[i] ) { min_p = &dat[i]; }
    }
    return *min_p;
}

template <typename T>
auto     maxData ( const T *dat, size_t num ) -> double
{
    auto max_p = &dat[0];
    for ( size_t i = 1; i < num; i ++ ) {
        if ( *max_p < dat[i] ) { max_p = &dat[i]; }
    }
    return *max_p;
}

template <typename T>
auto     scaleData ( T *dp, size_t num, const double &md, const double &inv_sd ) -> void
{
    auto m4 = num / 4 * 4; size_t i = 0; auto p = dp;
    for ( ; i < m4; i += 4, p += 4 ) {
        p[0] = ( p[0] - md ) * inv_sd;  p[1] = ( p[1] - md ) * inv_sd;
        p[2] = ( p[2] - md ) * inv_sd;  p[3] = ( p[3] - md ) * inv_sd;
    }
    for ( ; i < num; i ++ ) {
        dp[i] = ( dp[i] - md ) * inv_sd;
    }
}

template <typename T>
auto     normData ( NormDataRsl &rsl, T *vx, T *vy, size_t num )
{
    // ----------------------------------------------------------
    // linear normalize:
    //    X(norm) = ( X - X(min)) / ( X(max) - X(min))
    //    X(norm) will in [0,1]
    // zero normalize :
    //    z = ( x - avg ) / variance
    // ----------------------------------------------------------
    rsl.mx = avgData( vx, num );
    rsl.my = avgData( vy, num );
    rsl.sx = ( maxData( vx, num ) - minData( vx, num )) / 2;
    rsl.sy = ( maxData( vy, num ) - minData( vy, num )) / 2;
    if ( IcAlgor::fuzzyIsNull( rsl.sx )) { rsl.sx = DBL_MIN; }
    if ( IcAlgor::fuzzyIsNull( rsl.sy )) { rsl.sy = DBL_MIN; }
    scaleData( vx, num, rsl.mx, 1.0 / rsl.sx );
    scaleData( vy, num, rsl.my, 1.0 / rsl.sy );
}

template <typename T>
auto      unNormData (
    EllParamVect &par, const EllVector6d &A, const NormDataRsl &rsl, const T& = T()
) -> void
{
    par.coeffRef( 0 ) =   A.coeff(0) * rsl.sy * rsl.sy;
    par.coeffRef( 1 ) =   A.coeff(1) * rsl.sx * rsl.sy;
    par.coeffRef( 2 ) =   A.coeff(2) * rsl.sx * rsl.sx;
    par.coeffRef( 3 ) = - 2 * A.coeff(0) * rsl.sy * rsl.sy * rsl.mx - A.coeff(1) * rsl.sx * rsl.sy * rsl.my
                        + A.coeff(3) * rsl.sx * rsl.sy * rsl.sy;
    par.coeffRef( 4 ) = - A.coeff(1) * rsl.sx * rsl.sy * rsl.mx - 2 * A.coeff(2) * rsl.sx * rsl.sx * rsl.my
                        + A.coeff(4) * rsl.sx * rsl.sx * rsl.sy;
    par.coeffRef( 5 ) =   A.coeff(0) * rsl.sy * rsl.sy * rsl.mx * rsl.mx + A.coeff(1) * rsl.sx * rsl.sy * rsl.mx * rsl.my
                        + A.coeff(2) * rsl.sx * rsl.sx * rsl.my * rsl.my - A.coeff(3) * rsl.sx * rsl.sy * rsl.sy * rsl.mx
                        - A.coeff(4) * rsl.sx * rsl.sx * rsl.sy * rsl.my + A.coeff(5) * rsl.sx * rsl.sx * rsl.sy * rsl.sy;
}

template <typename T>
auto     genDesignMatrix( EllDesignMat &m, const Eigen::VectorXd &vx, const Eigen::VectorXd &vy, const T& = T() ) -> void
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

template <typename T>
auto     minPositiveIndexOfEigD( const Eigen::Matrix3d &eigD, const T& = T() ) -> int
{
    double min = 1e-8;  int idx = -1;
    if ( eigD.coeff(0,0) < min &&  ! std::isinf( eigD.coeff(0,0)) ) { idx = 0; min = eigD.coeff(0,0); }
    if ( eigD.coeff(1,1) < min &&  ! std::isinf( eigD.coeff(1,1)) ) { idx = 1; min = eigD.coeff(1,1); }
    if ( eigD.coeff(2,2) < min &&  ! std::isinf( eigD.coeff(2,2)) ) { idx = 2; min = eigD.coeff(2,2); }
    return idx;
}

template <typename T>
auto     toGeometry( double ell[5], const EllParamVect &par, const T& = T() ) -> void
{
    ell[4] = 0.5 * std::atan2( par.coeff(1), par.coeff(0) - par.coeff(2));
    double cos_t = std::cos( ell[4] );
    double sin_t = std::sin( ell[4] );

    double Ao, Au, Av, Auu, Avv;
    {
        double sin_squ = sin_t * sin_t;
        double cos_squ = cos_t * cos_t;
        double cos_sin = sin_t * cos_t;
        Ao = par.coeff(5);
        Au = par.coeff(3) * cos_t   + par.coeff(4) * sin_t;
        Av =-par.coeff(3) * sin_t   + par.coeff(4) * cos_t;
        Auu= par.coeff(0) * cos_squ + par.coeff(2) * sin_squ + par.coeff(1) * cos_sin;
        Avv= par.coeff(0) * sin_squ + par.coeff(2) * cos_squ - par.coeff(1) * cos_sin;
    }

    double wCentre;
    {
        double tuCentre = - Au / ( 2 * Auu + 1e-14);
        double tvCentre = - Av / ( 2 * Avv + 1e-14);

        wCentre  = Ao - Auu * tuCentre * tuCentre - Avv * tvCentre * tvCentre;
        ell[0]   = tuCentre * cos_t - tvCentre * sin_t;
        ell[1]   = tuCentre * sin_t + tvCentre * cos_t;
    }

    ell[2] = -wCentre / ( Auu + 1e-14);
    ell[3] = -wCentre / ( Avv + 1e-14);
    ell[2] = std::sqrt( std::abs( ell[2] )) * ( ell[2] >= 0 ? 1 : -1 );
    ell[3] = std::sqrt( std::abs( ell[3] )) * ( ell[3] >= 0 ? 1 : -1 );

    // fixing Ru,Rv ( ell[2],ell[3])
    ell[4] *= 180.0 / 3.141592653589793;
    if ( ell[2] < ell[3] ) {
       double t = ell[2]; ell[2] = ell[3]; ell[3] = t;
       ell[4] += 90.0;
    }
    if ( ell[4] <= -180 ) { ell[4] += 360; }
    if ( ell[4] >=  180 ) { ell[4] -= 180; }
    ell[4] *= 3.141592653589793 / 180.0;
}

template <typename T>
auto   fitEllipseDi ( Eigen::VectorXd &x, Eigen::VectorXd &y, double ell_data[5], EllFitWorkspace *wks_org = nullptr, const T& = T() ) ->  void
{   
    auto norm_rsl = reinterpret_cast<NormDataRsl*>( & ell_data[0] ); // temporarily save
    normData( *norm_rsl, const_cast<double*>( x.data()), const_cast<double*>( y.data() ), size_t( x.size()) );

    auto wks = wks_org;
    if ( wks == nullptr ) { wks = new EllFitWorkspace; }
    auto wks_mb = wks->member();
    {
        EllDesignMat D; genDesignMatrix<T> ( D, x, y );
        wks_mb->scat_mat = D.transpose() * D;               // S = D' * D
    }

    wks_mb->tmp_a = wks_mb->scat_mat.block<3,3>(0,0);
    wks_mb->tmp_b = wks_mb->scat_mat.block<3,3>(0,3);
    wks_mb->tmp_c = wks_mb->scat_mat.block<3,3>(3,3);
    wks_mb->tmp_d.setZero(3,3);
    wks_mb->tmp_d.coeffRef(0,2) = -2;
    wks_mb->tmp_d.coeffRef(1,1) =  1;
    wks_mb->tmp_d.coeffRef(2,0) = -2;
    wks_mb->tmp_e = wks_mb->tmp_c.inverse() * wks_mb->tmp_b.transpose();

    {
        Eigen::EigenSolver<Eigen::Matrix3d> es( wks_mb->tmp_d.inverse() * ( wks_mb->tmp_a - wks_mb->tmp_b * wks_mb->tmp_e ));
        wks_mb->eig_v = es.pseudoEigenvectors();
        wks_mb->eig_d = es.pseudoEigenvalueMatrix();
    }

    {
        auto min_eig_pos = minPositiveIndexOfEigD<T>( wks_mb->eig_d );
        if ( min_eig_pos < 0 ) { std::fprintf( stderr, "cound not found right eigen value!"); std::abort(); }
        Eigen::Vector3d A1 = wks_mb->eig_v.col( min_eig_pos );
        Eigen::Vector3d A2 = - wks_mb->tmp_e * A1;
        wks_mb->A << A1, A2;
    }

    unNormData<T>( wks_mb->par, wks_mb->A, *norm_rsl );
    toGeometry<T>( ell_data, wks_mb->par );

    if ( wks != wks_org ) { delete wks; }
}
}


// ============================================================================
// fit ellipse by points list
// ============================================================================
template <typename T>
auto   fitEllipseDirect( const EllFitPoint<T> *pts, size_t num, EllFitWorkspace *wks = nullptr ) -> EllFitResult<T>
{
    static_assert( std::is_arithmetic<T>::value, "T must be arithmetic type!" );
    if ( num < 1 ) { return EllFitResult<T>(); }

    Eigen::VectorXd x, y;
    EllFitHlp::genVecXyByPts<double, T>( x, y, pts, num );

    double ell[5];
    EllFitHlp::fitEllipseDi<double>( x, y, ell, wks );
    EllFitResult<T> rsl;
    rsl.bufferPtr()[0] = T( ell[0] ); rsl.bufferPtr()[1] = T( ell[1] );
    rsl.bufferPtr()[2] = T( ell[2] ); rsl.bufferPtr()[3] = T( ell[3] );
    rsl.bufferPtr()[4] = T( ell[4] );

    return rsl;
}











}
}
#endif
