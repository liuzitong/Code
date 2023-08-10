#ifndef QXPACK_IC_UI_QML_CHARTS_P_CXX
#define QXPACK_IC_UI_QML_CHARTS_P_CXX

#include "../common/qxpack_ic_def.h"
#include <QtGlobal>
#include "qxpack_ic_quicklineseriesitem.hxx"
#include "qxpack_ic_quickvalueaxisitem.hxx"
#include "qxpack_ic_quickparlnitem.hxx"
//#include "qxpack_ic_quickfbolineseriesitem.hxx"

static void  gInitRes( )
{
#ifdef QXPACK_IC_CFG_STATIC_LIB
    Q_INIT_RESOURCE( qxpack_ic_ui_qml_charts );
#endif
}

namespace QxPack {

void  QXPACK_IC_HIDDEN  IcUiQmlCharts_staticInit()
{
    gInitRes();
    IcQuickLineSeriesItem::_reg();
    IcQuickValueAxisItem::_reg();
    IcQuickParLnItem::_reg();
  //  IcQuickFboLineSeriesItem::_reg();
}

}

#endif
