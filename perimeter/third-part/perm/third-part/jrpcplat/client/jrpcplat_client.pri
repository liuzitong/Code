
! contains( JRPCPLAT_MODULES, jrpcplat_client ) {
    JRPCPLAT_MODULES *= jrpcplat_client
    CONFIG *= c++11

    contains( QT, core ) {
        DEFINES *= JRPCPLAT_QT5_ENABLED
    }
    contains( CONFIG, qt ) {
        DEFINES *= JRPCPLAT_QT5_ENABLED
    }

    INCLUDEPATH *= $$PWD/../../

    ! contains( JRPCPLAT_CFG, jrpcplat_client_cfg_no_hdr ) {
        HEADERS_CLIENT += \
           $$PWD/jrpcplat_loccli.hxx \

        HEADERS_COPY *= \
            $${HEADERS_CLIENT}

        HEADERS += \
           $${HEADERS_CLIENT} \
           $$PWD/jrpcplat_cliivkdscr_p.hxx \
           $$PWD/jrpcplat_cliworker_p.hxx \

         message( jrpcplat_client cfg. req. header )
     } else {
         message( jrpcplat_client cfg. req. no header )
     }

     ! contains( JRPCPLAT_CFG, jrpcplat_client_cfg_no_src ) {
        SOURCES += \
           $$PWD/jrpcplat_loccli.cxx \
           $$PWD/jrpcplat_cliivkdscr_p.cxx \
           $$PWD/jrpcplat_cliworker_p.cxx \

        win32 {
            message( jrpcplat_client platform is win32 )
            SOURCES += \

        } else:linux {
            !android {
                 message( jrpcplat_client platform is linux )
                 SOURCES += \

                 LIBS *= -lpthread
            } else {
                 message( jrpcplat_client platform is android )
                 SOURCES += \

            }
        } else {
            message ( jrpcplat_client platform is unknown )
            SOURCES += \

        }

        message( jrpcplat_client cfg. req. source )
    } else {
        message( jrpcplat_client cfg. req. no source )
    }

}

