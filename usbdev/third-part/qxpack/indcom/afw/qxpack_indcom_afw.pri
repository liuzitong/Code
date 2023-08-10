 !contains( QXPACK_IC, indcom_afw ) {
    QXPACK_IC *= indcom_afw

    contains( QT, qml ) {
        message( qxpack_indcom_afw detect project used QML module )
        DEFINES *= QXPACK_IC_DETECTED_QML
    }

    !contains( QXPACK_IC_CFG, indcom_afw_cfg_no_hdr ) {
        QXPACK_IC_HEADERS_AFW += \
               $$PWD/qxpack_ic_appctrlbase.hxx \
               $$PWD/qxpack_ic_appsettingsbase.hxx \
               $$PWD/qxpack_ic_modmgrbase.hxx \
               $$PWD/qxpack_ic_svcbase.hxx \
               $$PWD/qxpack_ic_viewmodelbase.hxx \
               $$PWD/qxpack_ic_msgbus.hxx \
               $$PWD/qxpack_ic_objmgr.hxx \
               $$PWD/qxpack_ic_confirmbus.hxx \
               $$PWD/qxpack_ic_simpleactcaller.hxx \

        QXPACK_IC_HEADERS_COPY += $${QXPACK_IC_HEADERS_AFW}

        HEADERS += \
              $${QXPACK_IC_HEADERS_AFW} \

        message( qxpack_indcom_afw cfg. req. header )
    } else {
        message( qxpack_indcom_afw cfg. req. no header )
    }

    !contains( QXPACK_IC_CFG, indcom_afw_cfg_no_src ) {
       SOURCES += \
               $$PWD/qxpack_ic_appctrlbase.cxx \
               $$PWD/qxpack_ic_appsettingsbase.cxx \
               $$PWD/qxpack_ic_modmgrbase.cxx \
               $$PWD/qxpack_ic_svcbase.cxx \
               $$PWD/qxpack_ic_viewmodelbase.cxx \
               $$PWD/qxpack_ic_msgbus.cxx \
               $$PWD/qxpack_ic_objmgr.cxx \
               $$PWD/qxpack_ic_confirmbus.cxx \
               $$PWD/qxpack_ic_simpleactcaller.cxx \

         message( qxpack_indcom_afw cfg. req. source )
     } else {
         message( qxpack_indcom_afw cfg. req. no source )
     }
}

