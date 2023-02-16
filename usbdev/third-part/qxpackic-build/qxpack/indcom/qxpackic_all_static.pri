# =============================================================================
#
#    This file used in user application 
#
# =============================================================================
! contains( QXPACK_IC_PRI, qxpackic_all_static ) {
    QXPACK_IC_PRI *= qxpackic_all_static
    CONFIG *= c++11
    INCLUDEPATH *= $$PWD/../../

    DEFINES *= QXPACK_IC_CFG_STATIC

    contains( QT, core ) {
        QT *= network gui
        DEFINES *= QXPACK_IC_QT_ENABLED
    }
    contains( QT, qml ) {
        message( qxpackic_all_static detected used QML module ) 
        DEFINES *= QXPACK_IC_DETECTED_QML
    }
    contains( CONFIG, qt ) {
        DEFINES *= QXPACK_IC_QT_ENABLED
    }

    # ------------------------------------------------------------------------- 
    # 'qxiccore_cfg_no_hdr'     : all modules, do not include headers. ( defined by parent )
    # -------------------------------------------------------------------------
    ! contains ( QXPACK_IC_CFG, qxiccore_cfg_hdr_and_src ) {
        QXPACK_IC_CFG *= indcom_common_cfg_no_src indcom_algor_cfg_no_src \
                         indcom_sys_cfg_no_src    indcom_afw_cfg_no_src  \
                         indcom_net_cfg_no_src    indcom_im_algor_cfg_no_src \
                         indcom_ui_qml_base_cfg_no_src  indcom_ui_qml_control_cfg_no_src \    
                         indcom_ui_qml_charts_cfg_no_src                    

        contains( QXPACK_IC_CFG, qxiccore_cfg_no_hdr ) {
            QXPACK_IC_CFG *= indcom_common_cfg_no_hdr indcom_algor_cfg_no_hdr \
                             indcom_sys_cfg_no_hdr    indcom_afw_cfg_no_hdr \
                             indcom_net_cfg_no_hdr    indcom_im_algor_cfg_no_hdr \
                             indcom_ui_qml_base_cfg_no_hdr   indcom_ui_qml_control_cfg_no_hdr \
                             indcom_ui_qml_charts_cfg_no_hdr 
        }
        QXPACK_IC_CFG *= qxpackic_all_cfg_only_lib

    } else {
        # here means include source and header
    }

    # -------------------------------------------------------------------------
    # 'qxpackic_all_cfg_only_lib': do not include all modules ( defined by parent )
    # -------------------------------------------------------------------------
    ! contains( QXPACK_IC_CFG, qxpackic_all_cfg_only_lib ) {
        include( $$PWD/common/qxpack_indcom_common.pri  )
        include( $$PWD/algor/qxpack_indcom_algor.pri  )
        include( $$PWD/sys/qxpack_indcom_sys.pri )
        include( $$PWD/afw/qxpack_indcom_afw.pri )
        include( $$PWD/net/qxpack_indcom_net.pri )

        contains( DEFINES, QXPACK_IC_DETECTED_QML ) { # fixed by 20201112/1140
            include( $$PWD/ui_qml_base/qxpack_indcom_ui_qml_base.pri )        
            include( $$PWD/ui_qml_control/qxpack_indcom_ui_qml_control.pri )
            include( $$PWD/ui_qml_charts/qxpack_indcom_ui_qml_charts.pri )
        }
    } else {
        message( qxpackic_all_static only contain library )

        QXPACK_IC_QT_BUILD = $$section( QMAKESPEC, /, -3, -3)
        QXPACK_IC_QT_VER   = $$section( QMAKESPEC, /, -4, -4)
        QXPACK_IC_QT_MMVER = $$section( QXPACK_IC_QT_VER, ., 0, 1 )
        QXPACK_IC_QT_MAJ   = $$section( QXPACK_IC_QT_MMVER, ., 0, 0 )
        LIBS *= -L"$$PWD/qt$$QXPACK_IC_QT_MMVER/$$QXPACK_IC_QT_BUILD"

        lessThan( QXPACK_IC_QT_MAJ, 6 ) {
            DEFINES *= QXPACK_IC_QT5
        } else {
            DEFINES *= QXPACK_IC_QT6
        }

 #       contains( DEFINES, QXPACK_IC_CFG_STATIC ) {
 #           RESOURCES += \
  #                $$PWD/ui_qml_base/qxpack_ic_ui_qml_base.qrc \
  #                $$PWD/ui_qml_control/qxpack_ic_ui_qml_control.qrc \
  #      }
        CONFIG( debug, debug|release ) {
            LIBS *= -lqxpackic_all_static_d
        } else {
            LIBS *= -lqxpackic_all_static
        }
    }

    QML_IMPORT_PATH *= $$PWD/../../
}
