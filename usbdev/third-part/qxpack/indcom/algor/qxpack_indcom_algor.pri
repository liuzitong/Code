
! contains( QXPACK_IC, indcom_algor ) {
    QXPACK_IC *= indcom_algor 
    CONFIG *= c++11

    contains( QT, core ) {
        DEFINES *= QXPACK_IC_QT_ENABLED
    }
    contains( CONFIG, qt ) {
        DEFINES *= QXPACK_IC_QT_ENABLED
    }

    INCLUDEPATH *= $$PWD/../../../

    ! contains( QXPACK_IC_CFG, indcom_algor_cfg_no_hdr ) {
        QXPACK_IC_HEADERS_ALGOR +=
        contains( DEFINES, QXPACK_IC_CFG_STATIC ) {

        }

        QXPACK_IC_HEADERS_ALGOR += \
            $$PWD/qxpack_ic_algor_base.hpp   \
            $$PWD/qxpack_ic_algor_errdev.hpp \
            $$PWD/qxpack_ic_algor_shuffle.hpp \
            $$PWD/qxpack_ic_algor_pot.hpp \
            $$PWD/qxpack_ic_algor_byteorder.hpp \
            \
            $$PWD/qxpack_ic_algor_ary.hpp    \
            $$PWD/qxpack_ic_algor_aryftr.hpp \
            $$PWD/qxpack_ic_algor_arycvt.hpp \
            $$PWD/qxpack_ic_algor_arydet.hpp \
            \
            $$PWD/qxpack_ic_algor_rdftsg.hpp \
            $$PWD/qxpack_ic_algor_fftsgsubrt.hpp \
            \ 
            $$PWD/qxpack_ic_algor_peak.hpp \
            \
            $$PWD/qxpack_ic_algor_pxgray.hpp \
            $$PWD/qxpack_ic_algor_pxmono.hpp \
            \
            $$PWD/qxpack_ic_algor_arystats.hpp \
               
        QXPACK_IC_HEADERS_COPY += $${QXPACK_IC_HEADERS_ALGOR}

        HEADERS += \
               $${QXPACK_IC_HEADERS_ALGOR} \

         message( qxpack_indcom_algor cfg. req. header )
     } else {
         message( qxpack_indcom_algor cfg. req. no header )
     }

     ! contains( QXPACK_IC_CFG, indcom_algor_cfg_no_src ) {
        SOURCES += \
               \
 

        win32 {
            message( qxpack_indcom_algor platform is win32 )
            DEFINES *= QXPACK_IC_PLATFORM_WIN32
            SOURCES += \
 

        } else:linux {
            !android {
                 message( qxpack_indcom_algor platform is linux )
                 DEFINES *= QXPACK_IC_PLATFORM_LINUX
                 SOURCES += \
 

                 LIBS *= -lpthread
            } else {
                 message( qxpack_indcom_algor platform is android )
                 DEFINES *= QXPACK_IC_PLATFORM_ANDROID
                 SOURCES += \
 
            }
        } else {
            message ( qxpack_indcom_algor platform is unknown )
            SOURCES += \
       
        }

        message( qxpack_indcom_algor cfg. req. source )
    } else {
        message( qxpack_indcom_algor cfg. req. no source )
    }

}

