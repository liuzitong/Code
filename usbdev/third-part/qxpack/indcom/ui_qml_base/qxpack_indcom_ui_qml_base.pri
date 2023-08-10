! contains( QXPACK_IC, indcom_ui_qml_base ) {
    QXPACK_IC *= indcom_ui_qml_base

    QT *= gui qml quick
    INCLUDEPATH *= $$PWD/../../../

    # -------------------------------------------------------------------------
    # if not contain QML module, need not to define the QML import path
    # -------------------------------------------------------------------------
    ! contains( QXPACK_IC_CFG, qxicqt5ui_cfg_no_qml ) {
        QML_IMPORT_PATH  *= $$PWD/../../../
        #message( $$QML_IMPORT_PATH )
    }

    IC_UI_QML_BASE_MODULE_QT_BUILD = $$section( QMAKESPEC, /, -3, -3)
    IC_UI_QML_BASE_MODULE_QT_VER   = $$section( QMAKESPEC, /, -4, -4)
    IC_UI_QML_BASE_MODULE_QT_MAJ   = $$section( IC_UI_QML_BASE_MODULE_QT_VER, ., 0, 0 )

    lessThan( IC_UI_QML_BASE_MODULE_QT_MAJ, 6 ) {
        DEFINES *= QXPACK_IC_QT5
    } else {
        DEFINES *= QXPACK_IC_QT6
    }

    # -------------------------------------------------------------------------
    # include the headers or not
    # -------------------------------------------------------------------------
    ! contains( QXPACK_IC_CFG, indcom_ui_qml_base_cfg_no_hdr ) {
        # used data
        QXPACK_IC_HEADERS_UI_QML_BASE += \
             $$PWD/qxpack_ic_ui_qml_api.hxx \
             $$PWD/qxpack_ic_qsgimtexture.hxx \
             $$PWD/qxpack_ic_qsgtextureprov.hxx \
             $$PWD/qxpack_ic_qsgrenderworker.hxx \
             $$PWD/qxpack_ic_quickimagedata.hxx \
             $$PWD/qxpack_ic_quickfborenderbase.hxx \
             $$PWD/qxpack_ic_qsgqtlogorender.hxx \
             $$PWD/qxpack_ic_quickxyseriesdata.hxx \
             $$PWD/qxpack_ic_quickaryseriesdata.hxx \     # added: 2021/05/19
             \


        QXPACK_IC_HEADERS_COPY += $${QXPACK_IC_HEADERS_UI_QML_BASE}

        HEADERS += \
             $${QXPACK_IC_HEADERS_UI_QML_BASE} \
             \
             $$PWD/qxpack_ic_qsgoglshaderhlp_p.hxx \


        # other copy files ( used for QtCreator designer )
        QXIC_MODULE_OTHER_COPY_FILES *= \
            $$PWD/qmldir \
            $$PWD/icuiqmlapi_dummy.qml \
            $$PWD/qmldir_rls \

        message( qxpack_indcom_ui_qml_base cfg. req. header )
    } else {
        message( qxpack_indcom_ui_qml_base cfg. req. no header )
    }

    # -------------------------------------------------------------------------
    # include the sources
    # -------------------------------------------------------------------------
    ! contains( QXPACK_IC_CFG, indcom_ui_qml_base_cfg_no_src ) {

        # normal source
        SOURCES += \
              $$PWD/qxpack_ic_ui_qml_base_p.cxx \
              $$PWD/qxpack_ic_ui_qml_api.cxx \
              $$PWD/qxpack_ic_qsgtextureprov.cxx \
              $$PWD/qxpack_ic_qsgrenderworker.cxx \
              $$PWD/qxpack_ic_quickimagedata.cxx \
              $$PWD/qxpack_ic_quickfborenderbase.cxx \
              $$PWD/qxpack_ic_qsgqtlogorender.cxx \
              $$PWD/qxpack_ic_quickxyseriesdata.cxx \
              $$PWD/qxpack_ic_quickaryseriesdata.cxx \
              \
              $$PWD/qxpack_ic_qsgoglshaderhlp_p.cxx \

        lessThan( IC_UI_QML_BASE_MODULE_QT_MAJ, 6 ) {
            SOURCES += $$PWD/qxpack_ic_qsgimtexture.cxx
        } else {
            SOURCES += $$PWD/qxpack_ic_qsgimtexture_6_0.cxx
        }

        # resources, maybe included QML?
        RESOURCES += \
              $$PWD/qxpack_ic_ui_qml_base.qrc \

        message( qxpack_indcom_ui_qml_base cfg. req. source )
    } else {

        contains( DEFINES, QXPACK_IC_CFG_STATIC ) {
            RESOURCES += \
                  $$PWD/qxpack_ic_ui_qml_base.qrc \
        }

        message( qxpack_indcom_ui_qml_base cfg. req. no source )
    }

}


