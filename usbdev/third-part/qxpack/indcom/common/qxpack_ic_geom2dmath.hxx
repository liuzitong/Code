/* ///////////////////////////////////////////////////////////////////// */
/*!
  @file    IcGeom2dMath
  @author  night wing
  @date    2020/04/10
  @brief   The file implement the vector2d algorithm
  @par     History
  @verbatim
          <author>   <time>   <version>  <desc>
         nightwing   2020/04   0.1.0     build this module
         nightwing   2020/07   0.2.0     re-designed
  @endverbatim
*/
/* ///////////////////////////////////////////////////////////////////// */
#ifndef QXPACK_IC_GEOM2DMATH_HXX
#define QXPACK_IC_GEOM2DMATH_HXX

#include "qxpack_ic_global.hxx"
#include <limits>
#include <cfloat>
#include <cmath>

namespace QxPack {

template <typename T> class IcPoint2D;
template <typename T> class IcVector2D;
template <typename T> class IcLine2D;

#if !defined( QXPACK_IC_GEOM2D_FP_DEFINED )
#define QXPACK_IC_GEOM2D_FP_DEFINED 1
typedef IcPoint2D<qxreal_t>   IcPoint2Dfp;
typedef IcVector2D<qxreal_t>  IcVector2Dfp;
typedef IcLine2D<qxreal_t>    IcLine2Dfp;
#endif

namespace IcGeom2DMath {

//! @brief   distance of two poinst
//! @return  the distance of two point
QXPACK_IC_API  double  distAB( const IcPoint2Dfp &a, const IcPoint2Dfp &b );


//! @brief distance of a line to a point
//! @param  ab  [in] the line ( point a to pont b )
//! @param  c   [in] the point
//! @param dist_ab_val  [opt_in]  the optional length of ab.
//! @return the length
QXPACK_IC_API  double  distLnToPt( const IcLine2Dfp &ab, const IcPoint2Dfp &c, qx_opt_in double *dist_ab_val );

//! @brief distance of the line segment to a point
//! @param  ab  [in] the line segment ( point a to point b )
//! @param  c   [in] the point
//! @param dist_ab_val  [opt_in]  the optional length of ab.
//! @note segment is not a line!  So if a point is outside the endpoint of the segment,
//!    this function calc. the distance between endpoint and point.
QXPACK_IC_API  double  distSegToPt( const IcLine2Dfp &ab, const IcPoint2Dfp &c, double *dist_ab_val );

//! @return the vertical vector of source vector
//! @note always return the anti-clock and vertical vector of 'src'
QXPACK_IC_API IcVector2Dfp  ccwVertVector ( const IcVector2Dfp &src, bool req_norm = true );

//! @brief find the cross point of two lines.
//! @param  pt   [out] the cross point
//! @param  ln_a [in]  the line a
//! @param  ln_b [in]  the line b
//! @return -1 -- not cross point found
//!         0 -- means find cross point, but it is not on any lines
//!         1 -- means find cross point, and it is on the first line
//!         2 -- means find cross point, and it is on the second line
//!         3 -- means find cross point, and it is on both lines
//! @warning  parallel lines ( a & b ) always return false.
QXPACK_IC_API int   findCrossPoint( IcPoint2Dfp &pt, const IcLine2Dfp &ln_a, const IcLine2Dfp &ln_b );

//! @brief bold a point as rectangle
//! @param b_pts [out] the four points
//! @param pt    [in]  the point
//! @param tk    [in]  the thickness ( > 1 )
//! @return none
//! @note  the points order is GL_TRANGLE_STRIP order
QXPACK_IC_API void  boldPoint( IcPoint2Dfp pts[4], const IcPoint2Dfp &pt, const double &thickness );

//! @brief bold a line as rectangle
//! @param b_pts [out] the four points
//! @param ln    [in]  the line
//! @param tk    [in]  the thickness ( > 1 )
//! @param req_endp_bold [in] check if req. endpoint bold.
//! @note pts[0] and pts[2] is the ln's ccw contour line;
//!       pts[1] and pts[3] is the ln's cw  contour line;
//! @note the points order is GL_TRANGLE_STRIP order
QXPACK_IC_API void  boldLine( IcPoint2Dfp pts[4], const IcLine2Dfp &ln, const double &thickness, bool req_endp_bold = true );


}
}
#endif
