
! contains( JRPCPLAT_MODULES, jrpcplat_common ) {
    JRPCPLAT_MODULES *= jrpcplat_common 
    CONFIG *= c++11

    contains( QT, core ) {
        DEFINES *= JRPCPLAT_QT5_ENABLED
    }
    contains( CONFIG, qt ) {
        DEFINES *= JRPCPLAT_QT5_ENABLED
    }

    INCLUDEPATH *= $$PWD/../../

    ! contains( JRPCPLAT_CFG, jrpcplat_common_cfg_no_hdr ) {

        HEADERS_COMMON += \
               $$PWD/jrpcplat_def.h \
               $$PWD/jrpcplat_memcntr.hxx \
               $$PWD/jrpcplat_logging.hxx \
               \
               $$PWD/jrpcplat_pimplprivtemp.hpp \
               $$PWD/jrpcplat_dequetemp.hpp \
               $$PWD/jrpcplat_semtemp.hpp \              

        HEADERS_COPY *= $${HEADERS_COMMON}

        HEADERS += \
            $${HEADERS_COMMON}

         message( jrpcplat_common cfg. req. header )
     } else {
         message( jrpcplat_common cfg. req. no header )
     }

     ! contains( JRPCPLAT_CFG, jrpcplat_common_cfg_no_src ) {
        SOURCES += \
               \
               $$PWD/jrpcplat_memcntr.cxx \
               $$PWD/jrpcplat_logging.cxx \

        win32 {
            message( jrpcplat_common platform is win32 )
            SOURCES += \

        } else:linux {
            !android {
                 message( jrpcplat_common platform is linux )
                 SOURCES += \

                 LIBS *= -lpthread
            } else {
                 message( jrpcplat_common platform is android )
                 SOURCES += \

            }
        } else {
            message ( jrpcplat_common platform is unknown )
            SOURCES += \
                $$PWD/qxpack_ic_ipcsem_dummy.cxx \
        }

        message( jrpcplat_common cfg. req. source )
    } else {
        message( jrpcplat_common cfg. req. no source )
    }

}

