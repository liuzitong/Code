# =============================================================================
#
#    This file used in user application 
#
# =============================================================================
! contains( QXPACK_IC_PRI, qxpackic_all_static_noqt ) {
    QXPACK_IC_PRI *= qxpackic_all_static_noqt
    CONFIG *= c++11
    INCLUDEPATH *= $$PWD/../../

    DEFINES *= QXPACK_IC_CFG_STATIC

   
    # ------------------------------------------------------------------------- 
    # 'qxiccore_cfg_no_hdr'     : all modules, do not include headers. ( defined by parent )
    # -------------------------------------------------------------------------
    ! contains ( QXPACK_IC_CFG, qxiccore_cfg_hdr_and_src ) {
        QXPACK_IC_CFG *= indcom_common_cfg_no_src  indcom_algor_cfg_no_src \

        contains( QXPACK_IC_CFG, qxiccore_cfg_no_hdr ) {
            QXPACK_IC_CFG *= indcom_common_cfg_no_hdr  indcom_algor_cfg_no_hdr \
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
        
    } else {
        message( qxpackic_all_static_noqt only contain library )

        QXPACK_IC_QT_BUILD = $$section( QMAKESPEC, /, -3, -3)
        QXPACK_IC_QT_VER   = $$section( QMAKESPEC, /, -4, -4)
        QXPACK_IC_QT_MMVER = $$section( QXPACK_IC_QT_VER, ., 0, 1 )
        LIBS *= -L"$$PWD/qt$$QXPACK_IC_QT_MMVER/$$QXPACK_IC_QT_BUILD"

        CONFIG( debug, debug|release ) {
            LIBS *= -lqxpackic_all_static_noqt_d
        } else {
            LIBS *= -lqxpackic_all_static_noqt
        }
    }
}
