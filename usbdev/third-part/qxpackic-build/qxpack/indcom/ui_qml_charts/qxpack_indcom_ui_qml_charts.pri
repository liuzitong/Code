! contains( QXPACK_IC, indcom_ui_qml_charts ) {
    QXPACK_IC *= indcom_ui_qml_charts

    QT *= gui qml quick
    INCLUDEPATH *= $$PWD/../../../

    IC_UI_QML_CHARTS_MODULE_QT_BUILD = $$section( QMAKESPEC, /, -3, -3)
    IC_UI_QML_CHARTS_MODULE_QT_VER   = $$section( QMAKESPEC, /, -4, -4)
    IC_UI_QML_CHARTS_MODULE_QT_MMVER = $$section( IC_UI_QML_CHARTS_MODULE_QT_VER, ., 0, 1 )
    IC_UI_QML_CHARTS_MODULE_QT_VER_MAJOR = $$section( IC_UI_QML_CHARTS_MODULE_QT_VER,., 0, 0 )
    IC_UI_QML_CHARTS_MODULE_QT_VER_MINOR = $$section( IC_UI_QML_CHARTS_MODULE_QT_VER,., 1, 1 )
    IC_UI_QML_CHARTS_MODULE_QT_VER_STEP  = $$section( IC_UI_QML_CHARTS_MODULE_QT_VER,., 2, 2 )

    DEFINES *= QXPACK_IC_QT_VER_INT=$$IC_UI_QML_CHARTS_MODULE_QT_VER_MAJOR$$IC_UI_QML_CHARTS_MODULE_QT_VER_MINOR$$IC_UI_QML_CHARTS_MODULE_QT_VER_STEP

    IC_UI_QML_CHARTS_MOULDE_QT_INC = $$section( QMAKESPEC,/,0,-3 )

    # -------------------------------------------------------------------------
    # if not contain QML module, need not to define the QML import path
    # -------------------------------------------------------------------------
    ! contains( QXPACK_IC_CFG, qxicqt5ui_cfg_no_qml ) {
        QML_IMPORT_PATH  *= $$PWD/../../../
    }

    # -------------------------------------------------------------------------
    # include the headers or not
    # -------------------------------------------------------------------------
    ! contains( QXPACK_IC_CFG, indcom_ui_qml_charts_cfg_no_hdr ) {
        # normal headers
        QXPACK_IC_HEADERS_UI_QML_CONTROL += \
            $$PWD/qxpack_ic_quicklineseriesitem.hxx \
            $$PWD/qxpack_ic_quickvalueaxisitem.hxx \
            $$PWD/qxpack_ic_quickparlnitem.hxx \
           # $$PWD/qxpack_ic_quickfbolineseriesitem.hxx \

        QXPACK_IC_HEADERS_COPY += $${QXPACK_IC_HEADERS_UI_QML_CONTROL}

        HEADERS += \
            $${QXPACK_IC_HEADERS_UI_QML_CONTROL} \
            $$PWD/qsgnode/qxpack_ic_qsglineseriesnode_p.hxx \
            $$PWD/qsgnode/qxpack_ic_qsgvalueaxisnode_p.hxx \
            $$PWD/qxpack_ic_quicklbllayoutsrc_p.hxx \
            $$PWD/qsgnode/qxpack_ic_qsglineseriesnode_glsl_4_0_p.hxx \
           # $$PWD/qsgnode/qxpack_ic_qsgvalueaxisnode_glsl_4_3_p.hxx \

        # other copy files ( used for QtCreator designer )
        QXIC_MODULE_OTHER_COPY_FILES *= \
            $$PWD/qmldir \
            $$PWD/qmldir_rls \
            $$PWD/icquicklineseriesitem_dummy.qml \
            $$PWD/icquickvalueaxisitem_dummy.qml \
            $$PWD/icquickparlnitem_dummy.qml \

        message( qxpack_indcom_ui_qml_charts cfg. req. header )
    } else {
        message( qxpack_indcom_ui_qml_charts cfg. req. no header )
    }

    # -------------------------------------------------------------------------
    # include the sources
    # -------------------------------------------------------------------------
    ! contains( QXPACK_IC_CFG, indcom_ui_qml_charts_cfg_no_src ) {

        # normal source
        SOURCES += \
            $$PWD/qxpack_ic_ui_qml_charts_p.cxx \
            $$PWD/qxpack_ic_quicklineseriesitem.cxx \
            $$PWD/qxpack_ic_quickvalueaxisitem.cxx \
            $$PWD/qxpack_ic_quickparlnitem.cxx \
            $$PWD/qsgnode/qxpack_ic_qsglineseriesnode_p.cxx \
            $$PWD/qsgnode/qxpack_ic_qsgvalueaxisnode_p.cxx \
            $$PWD/qsgnode/qxpack_ic_qsglineseriesnode_glsl_4_0_p.cxx \
           # $$PWD/qsgnode/qxpack_ic_qsgvalueaxisnode_glsl_4_3_p.cxx \
            $$PWD/qxpack_ic_quicklbllayoutsrc_p.cxx \

        # resources, maybe included QML?
        RESOURCES += \
              $$PWD/qxpack_ic_ui_qml_charts.qrc \

        message( qxpack_indcom_ui_qml_charts cfg. req. source )
    } else {
        contains( DEFINES, QXPACK_IC_CFG_STATIC ) {
            RESOURCES += \
                  $$PWD/qxpack_ic_ui_qml_charts.qrc \
        }
        message( qxpack_indcom_ui_qml_charts cfg. req. no source )
    }

}


