
! contains( JRPCPLAT_MODULES, jrpcplat_base ) {
    JRPCPLAT_MODULES *= jrpcplat_base
    CONFIG *= c++11

    contains( QT, core ) {
        DEFINES *= JRPCPLAT_QT5_ENABLED
    }
    contains( CONFIG, qt ) {
        DEFINES *= JRPCPLAT_QT5_ENABLED
    }

    INCLUDEPATH *= $$PWD/../../

    ! contains( JRPCPLAT_CFG, jrpcplat_base_cfg_no_hdr ) {    
        HEADERS_BASE += \
            $$PWD/jrpcplat_pkgerror_def.hxx \
            $$PWD/jrpcplat_package.hxx \

        HEADERS_COPY *= \
            $${HEADERS_BASE} \

        HEADERS += \
            $${HEADERS_BASE} \
            \
            $$PWD/jrpcplat_global_p.hxx \
            $$PWD/jrpcplat_datapkg_p.hxx \
            $$PWD/jrpcplat_locsvrsckthr_p.hxx \
            $$PWD/jrpcplat_timestamp_p.hxx \

         message( jrpcplat_base cfg. req. header )
     } else {
         message( jrpcplat_base cfg. req. no header )
     }

     ! contains( JRPCPLAT_CFG, jrpcplat_base_cfg_no_src ) {
        SOURCES += \
          $$PWD/jrpcplat_package.cxx \
          \
          $$PWD/jrpcplat_global_p.cxx \
          $$PWD/jrpcplat_datapkg_p.cxx \
          $$PWD/jrpcplat_locsvrsckthr_p.cxx \
          $$PWD/jrpcplat_timestamp_p.cxx \

        win32 {
            message( jrpcplat_base platform is win32 )
            SOURCES += \

        } else:linux {
            !android {
                 message( jrpcplat_base platform is linux )
                 SOURCES += \

                 LIBS *= -lpthread
            } else {
                 message( jrpcplat_base platform is android )
                 SOURCES += \

            }
        } else {
            message ( jrpcplat_base platform is unknown )
            SOURCES += \

        }

        message( jrpcplat_base cfg. req. source )
    } else {
        message( jrpcplat_base cfg. req. no source )
    }

}

