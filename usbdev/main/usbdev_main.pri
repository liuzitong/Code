
! contains( USBDEV, usbdev_main ) {
    USBDEV *= usbdev_main
    CONFIG *= c++11

    contains( QT, core ) {
        DEFINES *= USBDEV_QT5_ENABLED
    }
    contains( CONFIG, qt ) {
        DEFINES *= USBDEV_QT5_ENABLED
    }

    INCLUDEPATH *= $$PWD/../../
#    INCLUDEPATH *= $$PWD/../third-part
#    SOURCES += \
#            $$PWD/../third-part/qxpack/indcom/

    ! contains( USBDEV_CFG, usbdev_main_cfg_no_hdr ) {   
        HEADERS_MAIN += \
              $$PWD/usbdev_config.hxx \
              $$PWD/usbdev_devctl.hxx \
              $$PWD/usbdev_profile.hxx \
              $$PWD/usbdev_statusdata.hxx \
              $$PWD/usbdev_framedata.hxx \
              $$PWD/usbdev_cache.hxx \

        HEADERS_COPY *= $${HEADERS_MAIN}

        HEADERS += \
              $${HEADERS_MAIN} \
              \
              $$PWD/nwkusbobj2.hxx \

         message( usbdev_main cfg. req. header )
     } else {
         message( usbdev_main cfg. req. no header )
     }

     ! contains( USBDEV_CFG, usbdev_main_cfg_no_src ) {
        SOURCES += \
               $$PWD/usbdev_config.cxx \
               $$PWD/usbdev_devctl.cxx \
               $$PWD/usbdev_profile.cxx \
               $$PWD/usbdev_statusdata.cxx \
               $$PWD/usbdev_framedata.cxx \
               \

        win32 {
            message( usbdev_main platform is win32 )
            SOURCES += \
                    $$PWD/nwkusbobj2_win32.cxx \

        } else:linux {
            !android {
                 message( usbdev_main platform is linux )
                 SOURCES += \

                 LIBS *= -lpthread
            } else {
                 message( usbdev_main platform is android )
                 SOURCES += \

            }
        } else {
            message ( usbdev_main platform is unknown )
            SOURCES += \

        }

        message( usbdev_main cfg. req. source )
    } else {
        message( usbdev_main cfg. req. no source )
    }

}

HEADERS += \
    $$PWD/usbdev_cache.hxx

SOURCES += \
    $$PWD/usbdev_cache.cpp

