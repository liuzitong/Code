// ////////////////////////////////////////////////////////////////////////////
// @author nightwing
// @verbatim
//      author     date           memo.
//      nightwing  2021/05/23     build
// @endvarbatim
// ////////////////////////////////////////////////////////////////////////////
#ifndef QXPACK_IC_QUICKFBOLINESERIESITEM_HXX
#define QXPACK_IC_QUICKFBOLINESERIESITEM_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include "qxpack/indcom/ui_qml_base/qxpack_ic_quickfborenderbase.hxx"

#include <QQuickItem>
#include <QSGNode>
#include <QPointF>
#include <QVector2D>
#include <QRectF>
#include <QPointF>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief  this item is used in Quick scene.
 * @details
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcQuickFboLineSeriesItem : public IcQuickFboRenderBase {
    Q_OBJECT


private:
public:
     //! ctor
     explicit IcQuickFboLineSeriesItem ( QQuickItem *pa = Q_NULLPTR );

     //! dtor
     virtual ~IcQuickFboLineSeriesItem () Q_DECL_OVERRIDE;

public :

     //! the hold API for static library
     static  void    _reg();

protected:

private:
    void *m_obj;
    Q_DISABLE_COPY( IcQuickFboLineSeriesItem )
};



}

#endif
