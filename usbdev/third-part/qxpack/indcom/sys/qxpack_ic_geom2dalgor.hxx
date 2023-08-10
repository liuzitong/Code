/* ///////////////////////////////////////////////////////////////////// */
/*!
  @file    IcGeom2DAlgor
  @author  night wing
  @date    2020/04/10
  @brief   The file implement the Qt relatived 2d algorithm
  @par     History
  @verbatim
          <author>   <time>   <version>  <desc>
         nightwing   2020/07   0.1.0     build this module
  @endverbatim
*/
/* ///////////////////////////////////////////////////////////////////// */
#ifndef QXPACK_IC_GEOM2DALGOR_HXX
#define QXPACK_IC_GEOM2DALGOR_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include "qxpack/indcom/common/qxpack_ic_geom2d.hxx"
#include "qxpack/indcom/common/qxpack_ic_geom2dmath.hxx"
#include <QPointF>
#include <QLineF>
#include <QVector>
#include <QList>
#include <QPair>

namespace QxPack {
namespace IcGeom2DAlgor {

//! @brief merge the Consecutive duplicate points
//! @return a pair result ( merged list, the repeated num )
//! @note  if has no consecutive duplicate points, the pair.first == lst and pair.second is empty list
QXPACK_IC_API QPair<QList<QPointF>,QList<int>>  mergeCDP( const QList<QPointF> &lst );


//! @brief make solid polygon line contour points
//! @param src      [in] the source polygon line points array
//! @param ln_thick [in] the line thickness, used for spec. contour range.
//! @param req_endp_bold [in] require bold the endpoint
//! @details 'ln_thick' >= 1.0;
//! @return the result list.
//! @note   points order is GL_QUARD_STRIP like points order.
QXPACK_IC_API QList<QPointF>  makeSoildPolLnContour( const QList<QPointF> &src, const double &ln_thick, bool req_endp_bold = true );

}
}
#endif
