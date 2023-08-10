

!contains( QXPACK_IC, indcom_sys ) {
    QXPACK_IC *= indcom_sys

    contains( QT, core ) {
        QT *= network gui
        DEFINES *= QXPACK_IC_QT_ENABLED
    }
    contains( CONFIG, qt ) {
        DEFINES *= QXPACK_IC_QT_ENABLED
    }
    win32 {
        LIBS *= setupapi.lib user32.lib kernel32.lib
    }

    INCLUDEPATH *= $$PWD/../../../

    IC_SYS_MODULE_QT_BUILD  = $$section( QMAKESPEC, /, -3, -3)
    IC_SYS_MODULE_QT_VER    = $$section( QMAKESPEC, /, -4, -4)
    IC_SYS_MODULE_QT_MAJ    = $$section( IC_SYS_MODULE_QT_VER, ., 0, 0 )

    lessThan( IC_SYS_MODULE_QT_MAJ, 6 ) {
        DEFINES *= QXIC_QT5
    } else {
        DEFINES *= QXIC_QT6
    }

    ! contains( QXPACK_IC_CFG, indcom_sys_cfg_no_hdr ) {
        QXPACK_IC_HEADERS_SYS +=
        contains( DEFINES, QXPACK_IC_CFG_STATIC ) {
            QXPACK_IC_HEADERS_SYS += \
               $$PWD/qxpack_ic_sharedthreadpool_priv.hxx \
               $$PWD/qxpack_ic_sysevtqthread_priv.hxx \

        }

        QXPACK_IC_HEADERS_SYS += \
               $$PWD/qxpack_ic_rmtobjcreator_priv.hxx \
               $$PWD/qxpack_ic_rmtobjdeletor_priv.hxx \
               $$PWD/qxpack_ic_rmtobjsigblocker_priv.hxx \
               $$PWD/qxpack_ic_rmtcallback_priv.hxx \
               $$PWD/qxpack_ic_ncstring.hxx \
               $$PWD/qxpack_ic_filemon.hxx  \
               $$PWD/qxpack_ic_stdioipc.hxx \
               $$PWD/qxpack_ic_jotaskworker.hxx \
               $$PWD/qxpack_ic_vardata.hxx \
               $$PWD/qxpack_ic_jsonrpc2.hxx \
               $$PWD/qxpack_ic_appdcl_priv.hxx \
               $$PWD/qxpack_ic_apptot_priv.hxx \
               $$PWD/qxpack_ic_eventloopbarrier.hxx \
               $$PWD/qxpack_ic_imagedata.hxx \
               $$PWD/qxpack_ic_simplefsm.hxx \
               $$PWD/qxpack_ic_pnomap.hpp \
               $$PWD/qxpack_ic_geom2dalgor.hxx \
               $$PWD/qxpack_ic_usbstormon.hxx \

         QXPACK_IC_HEADERS_COPY += $${QXPACK_IC_HEADERS_SYS}

         HEADERS += \
               $${QXPACK_IC_HEADERS_SYS}


        message( qxpack_indcom_sys cfg. req. header )
    } else {
        message( qxpack_indcom_sys cfg. req. no header )
    }

    ! contains( QXPACK_IC_CFG, indcom_sys_cfg_no_src ) {
        SOURCES += \
               $$PWD/qxpack_ic_rmtobjcreator_priv.cxx \
               $$PWD/qxpack_ic_rmtobjdeletor_priv.cxx \
               $$PWD/qxpack_ic_rmtobjsigblocker_priv.cxx \
               $$PWD/qxpack_ic_sharedthreadpool_priv.cxx \
               $$PWD/qxpack_ic_sysevtqthread_priv.cxx \
               $$PWD/qxpack_ic_rmtcallback_priv.cxx \
               \
               $$PWD/qxpack_ic_ncstring.cxx \
               $$PWD/qxpack_ic_filemon.cxx  \
               $$PWD/qxpack_ic_stdioipc.cxx \
               $$PWD/qxpack_ic_jotaskworker.cxx \
               $$PWD/qxpack_ic_vardata.cxx \
               $$PWD/qxpack_ic_jsonrpc2.cxx \
               $$PWD/qxpack_ic_eventloopbarrier.cxx \
               $$PWD/qxpack_ic_imagedata.cxx \
               $$PWD/qxpack_ic_simplefsm.cxx \
               $$PWD/qxpack_ic_geom2dalgor.cxx \
               $$PWD/qxpack_ic_usbstormon.cxx \

        win32 {
            SOURCES += $$PWD/qxpack_ic_usbstormon_win32.cxx
        } else {
            SOURCES += $$PWD/qxpack_ic_usbstormon_null.cxx
        }

        message( qxpack_indcom_sys cfg. req. source )
    } else {
        message( qxpack_indcom_sys cfg. req. no source )
    }

}




