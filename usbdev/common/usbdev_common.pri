
! contains( USBDEV, usbdev_common ) {
    USBDEV *= usbdev_common
    CONFIG *= c++11

    contains( QT, core ) {
        DEFINES *= USBDEV_QT5_ENABLED
    }
    contains( CONFIG, qt ) {
        DEFINES *= USBDEV_QT5_ENABLED
    }

    INCLUDEPATH *= $$PWD/../../

    ! contains( USBDEV_CFG, usbdev_common_cfg_no_hdr ) {
        HEADERS_COMMON += \
               $$PWD/usbdev_def.h \
               $$PWD/usbdev_memcntr.hxx \
               $$PWD/usbdev_logging.hxx \
               \
               $$PWD/usbdev_pimplprivtemp.hpp \
               $$PWD/usbdev_dequetemp.hpp \
               $$PWD/usbdev_semtemp.hpp \

        HEADERS_COPY *= $${HEADERS_COMMON}

        HEADERS += \
               $${HEADERS_COMMON}

         message( usbdev_common cfg. req. header )
     } else {
         message( usbdev_common cfg. req. no header )
     }

     ! contains( USBDEV_CFG, usbdev_common_cfg_no_src ) {
        SOURCES += \
               \
               $$PWD/usbdev_memcntr.cxx \
               $$PWD/usbdev_logging.cxx \

        win32 {
            message( usbdev_common platform is win32 )
            SOURCES += \

        } else:linux {
            !android {
                 message( usbdev_common platform is linux )
                 SOURCES += \

                 LIBS *= -lpthread
            } else {
                 message( usbdev_common platform is android )
                 SOURCES += \

            }
        } else {
            message ( usbdev_common platform is unknown )
            SOURCES += \
                $$PWD/qxpack_ic_ipcsem_dummy.cxx \
        }

        message( usbdev_common cfg. req. source )
    } else {
        message( usbdev_common cfg. req. no source )
    }

}

