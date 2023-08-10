# =============================================================================
#
#    This file used in user application 
#
# =============================================================================
! contains( QXPACK_IC_PRI, qxicqt5 ) {
    QXPACK_IC_PRI *= qxicqt5
    CONFIG *= c++11
    INCLUDEPATH *= $$PWD/../../

    contains( QT, core ) {
        QT *= network gui
        DEFINES *= QXPACK_IC_QT_ENABLE
    }
    contains( QT, qml ) {
        message( qxicqt5 detected used QML module ) 
        DEFINES *= QXPACK_IC_DETECTED_QML
    }
    contains( CONFIG, qt ) {
        DEFINES *= QXPACK_IC_QT_ENABLED
    }

    # -------------------------------------------------------------------------
    # 'indcom_sys_cfg_no_src': sys module,  do not include sources 
    # 'indcom_afw_cfg_no_src': afw module,  do not include sources
    # 'indcom_net_cfg_no_src': net module,  do not include sources
    # 'indcom_sys_cfg_no_hdr': all modules, do not include headers 
    # -------------------------------------------------------------------------
    ! contains( QXPACK_IC_CFG, qxicqt5_cfg_hdr_and_src ) {
        QXPACK_IC_CFG *= \
            indcom_sys_cfg_no_src indcom_afw_cfg_no_src  \
            indcom_net_cfg_no_src  \

        contains( QXPACK_IC_CFG, qxicqt5_cfg_no_hdr ) {
            QXPACK_IC_CFG *= \
                indcom_sys_cfg_no_hdr  indcom_afw_cfg_no_hdr \
                indcom_net_cfg_no_hdr   \
        }
        QXPACK_IC_CFG *= qxicqt5_cfg_only_lib
    } else {
        # here means include source and header
    }

    # -------------------------------------------------------------------------
    # 'qxicqt5_cfg_only_lib' : do not include all modules ( defined by parent ) 
    # -------------------------------------------------------------------------
    ! contains( QXPACK_IC_CFG, qxicqt5_cfg_only_lib ) {
        include( $$PWD/sys/qxpack_indcom_sys.pri )
        include( $$PWD/afw/qxpack_indcom_afw.pri )
        include( $$PWD/net/qxpack_indcom_net.pri )

    } else {
        message( qxicqt5 only contain library )
    
        LIBS *= -L"$$PWD"
        win32 {
            CONFIG( debug, debug|release ) {
                 LIBS *= -lqxicqt5d
            } else {
                 LIBS *= -lqxicqt5
            }
        } else {
            LIBS *= -lqxicqt5
        }
    }
}


