! contains( PERIMETER_MODULES, qxpackic_all ) {
    PERIMETER_MODULES *= qxpackic_all

    DEFINES *= QXPACK_IC_CFG_STATIC QXPACK_IC_NO_UI_QML_CHARTS
    include( $$PWD/qxpack/indcom/common/qxpack_indcom_common.pri )
    include( $$PWD/qxpack/indcom/sys/qxpack_indcom_sys.pri )
    include( $$PWD/qxpack/indcom/afw/qxpack_indcom_afw.pri )
    include( $$PWD/qxpack/indcom/net/qxpack_indcom_net.pri )

    contains( QT, qml ) {
        include( $$PWD/qxpack/indcom/ui_qml_base/qxpack_indcom_ui_qml_base.pri )
        include( $$PWD/qxpack/indcom/ui_qml_control/qxpack_indcom_ui_qml_control.pri )
    }

}
