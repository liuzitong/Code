#ifndef QXPACK_IC_QUICKQTLOGOITEM_CXX
#define QXPACK_IC_QUICKQTLOGOITEM_CXX

#include "qxpack_ic_quickqtlogoitem.hxx"
#include "qxpack/indcom/ui_qml_base/qxpack_ic_qsgqtlogorender.hxx"

#include <QQmlEngine>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
// register functions, it will register object type in QML engine
// while QCoreApplication finish the ctor
// ////////////////////////////////////////////////////////////////////////////
static void QxPack_IcQuickQtLogoItem_Reg( )
{
    static bool is_reg = false;
    if ( ! is_reg ) {
        is_reg = true;
        qmlRegisterType<QxPack::IcQuickQtLogoItem>("qxpack.indcom.ui_qml_control", 1, 0, "IcQuickQtLogoItem");
    }
}
//Q_COREAPP_STARTUP_FUNCTION( QxPack_IcQuickQtLogoItem_Reg )


// ////////////////////////////////////////////////////////////////////////////
// quick qt logo item
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcQuickQtLogoItem :: IcQuickQtLogoItem ( QQuickItem *pa )
    : IcQuickFboRenderBase (
        [](void*,const QString &op, const QVariant &par )->QVariant {
            if ( op == QStringLiteral("createQSGRenderWorker")) {
                IcQSGRenderWorker *wkr = new IcQSGQtLogoRender();
                return QVariant::fromValue( static_cast<void*>(wkr));
            } else if ( op == QStringLiteral("deleteQSGRenderWorker")) {
                IcQSGRenderWorker *wkr = static_cast<QxPack::IcQSGRenderWorker*>( par.value<void*>());
                if ( wkr != Q_NULLPTR ) { delete wkr; }
                return QVariant();
            } else {
                return QVariant();
            }
        }, this, pa
    )
{
    m_obj = Q_NULLPTR;
}

// ============================================================================
// dtor
// ============================================================================
IcQuickQtLogoItem :: ~IcQuickQtLogoItem( )
{

}


void  IcQuickQtLogoItem :: _reg() {QxPack_IcQuickQtLogoItem_Reg(); }

}


#endif
