#ifndef QXPACK_IC_UI_QML_BASE_P_CXX
#define QXPACK_IC_UI_QML_BASE_P_CXX

#include "../common/qxpack_ic_def.h"
#include <QtGlobal>

static void  gInitRes( )
{
#ifdef QXPACK_IC_CFG_STATIC_LIB
    Q_INIT_RESOURCE( qxpack_ic_ui_qml_base );
#endif
}



namespace QxPack {

void  QXPACK_IC_HIDDEN  IcUiQmlBase_staticInit()
{
    gInitRes();
}

}

#endif
