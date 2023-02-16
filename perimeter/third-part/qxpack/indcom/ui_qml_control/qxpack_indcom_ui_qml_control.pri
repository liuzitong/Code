! contains( QXPACK_IC, indcom_ui_qml_control ) {
    QXPACK_IC *= indcom_ui_qml_control

    QT *= gui qml quick
    INCLUDEPATH *= $$PWD/../../../

    # -------------------------------------------------------------------------
    # if not contain QML module, need not to define the QML import path
    # -------------------------------------------------------------------------
    ! contains( QXPACK_IC_CFG, qxicqt5ui_cfg_no_qml ) {
        QML_IMPORT_PATH  *= $$PWD/../../../
    }

    # -------------------------------------------------------------------------
    # include the headers or not
    # -------------------------------------------------------------------------
    ! contains( QXPACK_IC_CFG, indcom_ui_qml_control_cfg_no_hdr ) {
        # normal headers
        QXPACK_IC_HEADERS_UI_QML_CONTROL += \
            $$PWD/qxpack_ic_quickqtlogoitem.hxx \
            $$PWD/qxpack_ic_quickimageitem.hxx \

        QXPACK_IC_HEADERS_COPY += $${QXPACK_IC_HEADERS_UI_QML_CONTROL}

        HEADERS += \
            $${QXPACK_IC_HEADERS_UI_QML_CONTROL} \

        # other copy files ( used for QtCreator designer )
        QXIC_MODULE_OTHER_COPY_FILES *= \
            $$PWD/qmldir \
            $$PWD/modalpopupdialog.qml \
            $$PWD/pagebase.qml \
            $$PWD/qmldir_rls \
            $$PWD/icquickimageitem_dummy.qml \

        message( qxpack_indcom_ui_qml_control cfg. req. header )
    } else {
        message( qxpack_indcom_ui_qml_control cfg. req. no header )
    }

    # -------------------------------------------------------------------------
    # include the sources
    # -------------------------------------------------------------------------
    ! contains( QXPACK_IC_CFG, indcom_ui_qml_control_cfg_no_src ) {

        # normal source
        SOURCES += \
            $$PWD/qxpack_ic_ui_qml_control_p.cxx \
            $$PWD/qxpack_ic_quickqtlogoitem.cxx \

        lessThan( IC_UI_QML_CONTROL_MODULE_QT_MAJOR, 6 ) {
            lessThan( IC_UI_QML_CONTROL_MODULE_QT_MINOR, 8 ) {
                SOURCES *= $$PWD/qxpack_ic_quickimageitem.cxx
            } else {
                SOURCES *= $$PWD/qxpack_ic_quickimageitem_5_8.cxx
            }
        } else {
            SOURCES *= $$PWD/qxpack_ic_quickimageitem_5_8.cxx
        }

        # resources, maybe included QML?
        RESOURCES += \
              $$PWD/qxpack_ic_ui_qml_control.qrc \

        message( qxpack_indcom_ui_qml_control cfg. req. source )
    } else {
        contains( DEFINES, QXPACK_IC_CFG_STATIC ) {
            RESOURCES += \
                  $$PWD/qxpack_ic_ui_qml_control.qrc \
        }
        message( qxpack_indcom_ui_qml_control cfg. req. no source )
    }

}


