# =============================================================================
#
#    This file used in user application 
#
# =============================================================================
! contains( QXPACK_IC_PRI, qxicqt5ui ) {
    QXPACK_IC_PRI *= qxicqt5ui
    CONFIG *= c++11
    INCLUDEPATH *= $$PWD/../../

    contains( QT, core ) {
        QT *= network gui
        DEFINES *= QXPACK_IC_QT_ENABLE
    }
    contains( QT, qml ) {
        message( qxicqt5ui detected used QML module ) 
        DEFINES *= QXPACK_IC_DETECTED_QML
    }
    contains( CONFIG, qt ) {
        DEFINES *= QXPACK_IC_QT_ENABLED
    }

    # -------------------------------------------------------------------------
    # if not contain QML module, need not to define the QML import path
    # -------------------------------------------------------------------------
    ! contains( QXPACK_IC_CFG, qxicqt5ui_cfg_no_qml ) {
        QML_IMPORT_PATH  *= $$PWD/../../
    }

    # -------------------------------------------------------------------------
    # 'indcom_ui_qml_base_cfg_no_src': ui_qml_base module, do not include sources
    # 'indcom_ui_qml_control_cfg_no_src' : ui_qml_control module, do not incldue sources
    # -------------------------------------------------------------------------
    ! contains( QXPACK_IC_CFG, qxicqt5ui_cfg_hdr_and_src ) {
        QXPACK_IC_CFG *= \
               indcom_ui_qml_base_cfg_no_src  \
               indcom_ui_qml_control_cfg_no_src \
               indcom_ui_qml_charts_cfg_no_src \
       
        contains( QXPACK_IC_CFG, qxicqt5ui_cfg_no_hdr ) {
            QXPACK_IC_CFG *= \
               indcom_ui_qml_base_cfg_no_hdr \
               indcom_ui_qml_control_cfg_no_hdr \
               indcom_ui_qml_charts_cfg_no_hdr \

        }
        QXPACK_IC_CFG *= qxicqt5ui_cfg_only_lib
    } else {
        # here means include source and header
    }

    # -------------------------------------------------------------------------    
    # 'qxicqt5ui_cfg_only_lib': do not include all modules
    # -------------------------------------------------------------------------
    ! contains( QXPACK_IC_CFG, qxicqt5ui_cfg_only_lib ) {
        include( $$PWD/ui_qml_base/qxpack_indcom_ui_qml_base.pri )
        include( $$PWD/ui_qml_control/qxpack_indcom_ui_qml_control.pri )
        include( $$PWD/ui_qml_charts/qxpack_indcom_ui_qml_charts.pri )
    } else {
        message( qxicqt5ui only contain library )

        LIBS *= -L"$$PWD"
        win32 {
            CONFIG( debug, debug|release ) {
                LIBS *= -lqxicqt5uid
            } else {
                LIBS *= -lqxicqt5ui
            }
        } else {
            LIBS *= -lqxicqt5ui
        }
    }
}

 
