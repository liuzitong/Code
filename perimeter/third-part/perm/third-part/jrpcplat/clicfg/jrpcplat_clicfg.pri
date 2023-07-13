
! contains( JRPCPLAT_MODULES, jrpcplat_clicfg ) {
    JRPCPLAT_MODULES *= jrpcplat_clicfg
    CONFIG *= c++11

    contains( QT, core ) {
        DEFINES *= JRPCPLAT_QT5_ENABLED
    }
    contains( CONFIG, qt ) {
        DEFINES *= JRPCPLAT_QT5_ENABLED
    }

    INCLUDEPATH *= $$PWD/../../

    ! contains( JRPCPLAT_CFG, jrpcplat_clicfg_cfg_no_hdr ) {
        HEADERS_CLICFG += \
           $$PWD/jrpcplat_locclicfg.hxx \

        HEADERS_COPY *= \
            $${HEADERS_CLICFG}

        HEADERS += \
           $${HEADERS_CLICFG} \

         message( jrpcplat_clicfg cfg. req. header )
     } else {
         message( jrpcplat_clicfg cfg. req. no header )
     }

     ! contains( JRPCPLAT_CFG, jrpcplat_clicfg_cfg_no_src ) {
        SOURCES += \
           $$PWD/jrpcplat_locclicfg.cxx \

        win32 {
            message( jrpcplat_clicfg platform is win32 )
            SOURCES += \

        } else:linux {
            !android {
                 message( jrpcplat_clicfg platform is linux )
                 SOURCES += \

                 LIBS *= -lpthread
            } else {
                 message( jrpcplat_clicfg platform is android )
                 SOURCES += \

            }
        } else {
            message ( jrpcplat_clicfg platform is unknown )
            SOURCES += \

        }

        message( jrpcplat_clicfg cfg. req. source )
    } else {
        message( jrpcplat_clicfg cfg. req. no source )
    }

}

