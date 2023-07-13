# =============================================================================
#
#    This file used in user application for client
#
# =============================================================================
! contains( JRPCPLATCLI_PRI, jrpcplat ) {
    JRPCPLATCLI_PRI *= jrpcplatcli
    CONFIG *= c++11
    INCLUDEPATH *= $$PWD/../

    # -------------------------------------------------------------------------
    # 'jrpcplat_cfg_no_hdr'     : all modules, do not include headers. ( defined by parent )
    # -------------------------------------------------------------------------
    ! contains ( JRPCPLAT_CFG, jrpcplat_cfg_hdr_and_src ) {
        JRPCPLAT_CFG *= jrpcplat_common_cfg_no_src  jrpcplat_base_cfg_no_src \
                        jrpcplat_clicfg_cfg_no_src  \
                        jrpcplat_client_cfg_no_src  
        
        contains( JRPCPLAT_CFG, jrpcplat_cfg_no_hdr ) { 
            JRPCPLAT_CFG *= jrpcplat_common_cfg_no_hdr jrpcplat_base_cfg_no_hdr \
                            jrpcplat_clicfg_cfg_no_hdr  \
                            jrpcplat_client_cfg_no_hdr
                             
        }
        
        JRPCPLAT_CFG *= jrpcplat_cfg_only_lib

    } else {
        # here means include source and header
    }

    # -------------------------------------------------------------------------
    # 'jrpcplat_cfg_only_lib': do not include all modules ( defined by parent )
    # -------------------------------------------------------------------------
    ! contains( JRPCPLAT_CFG, jrpcplat_cfg_only_lib ) {
        include( $$PWD/common/jrpcplat_common.pri  )
        include( $$PWD/base/jrpcplat_base.pri )
        include( $$PWD/clicfg/jrpcplat_clicfg.pri )
        include( $$PWD/client/jrpcplat_client.pri )
    } else {
        message( jrpcplat only contain library )

        JRPCPLATCLI_MODULE_QT_BUILD  = $$section( QMAKESPEC, /, -3, -3)
        JRPCPLATCLI_MODULE_QT_VER    = $$section( QMAKESPEC, /, -4, -4)
        JRPCPLATCLI_MODULE_QT_MMVER  = $$section( JRPCPLATCLI_MODULE_QT_VER, ., 0, 1 )

#        LIBS *= -L"$$PWD/qt$$JRPCPLATCLI_MODULE_QT_MMVER/$$JRPCPLATCLI_MODULE_QT_BUILD"
         LIBS *= -L"$$PWD/qt5.9/msvc2015_64"

        win32 {
            CONFIG( debug, debug|release ) {
                LIBS *= -ljrpcplatclid
            } else {
                LIBS *= -ljrpcplatcli
            }
        } else {
            LIBS *= -ljrpcplatcli
        }
    }
}
