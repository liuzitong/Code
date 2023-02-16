#ifndef QXPACK_IC_UI_QML_CONTROL_P_CXX
#define QXPACK_IC_UI_QML_CONTROL_P_CXX

#include "../common/qxpack_ic_def.h"
#include <QtGlobal>
#include "qxpack_ic_quickimageitem.hxx"
#include "qxpack_ic_quickqtlogoitem.hxx"

static void  gInitRes( )
{
#ifdef QXPACK_IC_CFG_STATIC_LIB
    Q_INIT_RESOURCE( qxpack_ic_ui_qml_control );
#endif
}

namespace QxPack {

void  QXPACK_IC_HIDDEN  IcUiQmlControl_staticInit()
{
    gInitRes();
    IcQuickImageItem::_reg();
    IcQuickQtLogoItem::_reg();
}

}

#endif
