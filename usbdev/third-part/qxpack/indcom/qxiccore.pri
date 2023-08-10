# ==================================================
#
#    This file used in user application 
#
# ==================================================
! contains( QXPACK_IC_PRI, qxiccore ) {
    QXPACK_IC_PRI *= qxiccore
    CONFIG *= c++11
    INCLUDEPATH *= $$PWD/../../

    # ----------------------------------------------- 
    # 'indcom_common_cfg_no_src' : common module, do not include sources
    # 'qxiccore_cfg_no_hdr'     : all modules, do not include headers. ( defined by parent )
    # -----------------------------------------------
    ! contains ( QXPACK_IC_CFG, qxiccore_cfg_hdr_and_src ) {
        QXPACK_IC_CFG *= indcom_common_cfg_no_src  indcom_algor_cfg_no_src

        contains( QXPACK_IC_CFG, qxiccore_cfg_no_hdr ) {
            QXPACK_IC_CFG *= indcom_common_cfg_no_hdr indcom_algor_cfg_no_hdr
        }
        QXPACK_IC_CFG *= qxiccore_cfg_only_lib

    } else {
        # here means include source and header
    }

    # -----------------------------------------------
    # 'qxiccore_cfg_only_lib': do not include all modules ( defined by parent )
    # -----------------------------------------------
    ! contains( QXPACK_IC_CFG, qxiccore_cfg_only_lib ) {
        include( $$PWD/common/qxpack_indcom_common.pri  )
        include( $$PWD/algor/qxpack_indcom_algor.pri )

    } else {
        message( qxiccore only contain library )

        LIBS *= -L"$$PWD"
        win32 {
            CONFIG( debug, debug|release ) {
                LIBS *= -lqxiccored
            } else {
                LIBS *= -lqxiccore
            }
        } else {
            LIBS *= -lqxiccore
        }
    }
}
