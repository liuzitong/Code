# ////////////////////////////////////////////////
# configure section
# ////////////////////////////////////////////////
TEMPLATE = lib
CONFIG += c++11 skip_target_version_ext
CONFIG -= app_bundle

DEFINES *= QT_MESSAGELOGCONTEXT

CONFIG(debug,debug|release) {
    DEFINES *= QXPACK_IC_CFG_DLL
} else {
    DEFINES *= QXPACK_IC_CFG_DLL
    DEFINES *= QT_NO_DEBUG_OUTPUT
}

QXIC_MODULE_TARGET   = qxicqt5ui
QXIC_MODULE_BASE_DIR = $$PWD/../
QXIC_MODULE_INSTALL_DIR = $$PWD/qxpackic
QXIC_MODULE_INCLUDE = $$QXIC_MODULE_INSTALL_DIR/qxpack/indcom
QXIC_MODULE_DESTDIR = $$QXIC_MODULE_INSTALL_DIR/qxpack/indcom
QXIC_MODULE_OTHER_COPY_FILES =

win32:VERSION = 0.3.3.0
else: VERSION = 0.3.3

# ////////////////////////////////////////////////
# sub modules section
# ////////////////////////////////////////////////
include( $$PWD/qxiccommon.pri )

QXPACK_IC      =
QXPACK_IC_CFG *= qxiccore_cfg_no_hdr qxiccore_cfg_only_lib \
                 qxicqt5_cfg_no_hdr qxicqt5_cfg_only_lib  \

include( $$QXIC_MODULE_INCLUDE/qxiccore.pri )
include( $$QXIC_MODULE_INCLUDE/qxicqt5.pri )

#QXPACK_IC_CFG *= qxicqt5ui_cfg_no_qml
include( $$PWD/../ui_qml_base/qxpack_indcom_ui_qml_base.pri )
include( $$PWD/../ui_qml_control/qxpack_indcom_ui_qml_control.pri )

# ////////////////////////////////////////////////
# generate a copy.pro file at install directory.
# ////////////////////////////////////////////////
QXIC_MODULE_TMP_COPY_PRO = $${QXIC_MODULE_TARGET}_tmp_cpy.pro
QXIC_MODULE_COPY_FILES = $$HEADERS  \
                      $$QXIC_MODULE_OTHER_COPY_FILES \
                      $$PWD/../ui_qml_base/qxpack_indcom_ui_qml_base.pri \
                      $$PWD/../ui_qml_control/qxpack_indcom_ui_qml_control.pri \
                      $$PWD/../qxicqt5ui.pri \
                      $$PWD/../qxicqt5ui_update.txt \

genTempCopyProFile( $$QXIC_MODULE_TMP_COPY_PRO, $$QXIC_MODULE_COPY_FILES, $$QXIC_MODULE_BASE_DIR, $$QXIC_MODULE_INSTALL_DIR, $$QXIC_MODULE_INCLUDE )

QXIC_MODULE_TMP_RMV_PRO = $${QXIC_MODULE_TARGET}_tmp_rmv.pro
QXIC_MODULE_RMV_FILES = \
  $$QXIC_MODULE_INCLUDE/$${QXIC_MODULE_TARGET}d.ilk \
  $$QXIC_MODULE_INCLUDE/$${QXIC_MODULE_TARGET}d.exp \
  $$QXIC_MODULE_INCLUDE/$${QXIC_MODULE_TARGET}.ilk  \
  $$QXIC_MODULE_INCLUDE/$${QXIC_MODULE_TARGET}.exp  \

genTempRmvProFile( $${QXIC_MODULE_INSTALL_DIR}/$$QXIC_MODULE_TMP_RMV_PRO, $$QXIC_MODULE_RMV_FILES )

tmp_copy_pro.commands = $$QMAKE_QMAKE $$QXIC_MODULE_INSTALL_DIR/$$QXIC_MODULE_TMP_COPY_PRO
tmp_rmv_pro.commands  = $$QMAKE_QMAKE $$QXIC_MODULE_INSTALL_DIR/$$QXIC_MODULE_TMP_RMV_PRO
QMAKE_EXTRA_TARGETS += tmp_copy_pro
POST_TARGETDEPS += tmp_copy_pro

QMAKE_POST_LINK += $${tmp_rmv_pro.commands}


# ////////////////////////////////////////////////
# linking section
# ////////////////////////////////////////////////
win32 {
    !contains( QMAKE_TARGET.arch, x86_64 ) {
    } else {
    }

    DESTDIR    = $$QXIC_MODULE_DESTDIR
#    DLLDESTDIR = $$QXIC_MODULE_DESTDIR

    CONFIG( debug, debug|release ) {
        TARGET = $${QXIC_MODULE_TARGET}d
        LIBS += kernel32.lib user32.lib gdi32.lib
    } else {
        TARGET = $${QXIC_MODULE_TARGET}
        LIBS += kernel32.lib user32.lib gdi32.lib
        QMAKE_CXXFLAGS *= /Zi
        QMAKE_CFLAGS *= /Zi
        QMAKE_LFLAGS *= /DEBUG
    }
}

unix:!android {
   TARGET = $${QXIC_MODULE_TARGET}
}

android {
   TARGET = $${QXIC_MODULE_TARGET}

   contains( QMAKE_HOST.os, Windows ) {
       cp_file_src = $${OUT_PWD}/lib$${TARGET}.so
       cp_file_dst = $${QXIC_MODULE_DESTDIR}/lib$${TARGET}.so
       cp_file_src = $$replace( cp_file_src, /, \\ )
       cp_file_dst = $$replace( cp_file_dst, /, \\ )
       dll_copy_tgt.commands = $$QMAKE_COPY_FILE $${cp_file_src} $${cp_file_dst}
       QMAKE_POST_LINK = $${dll_copy_tgt.commands} & $${QMAKE_POST_LINK}
   } else {
       message( you need copy $${QXPACK_MODULE_TARGET} manually. )
   }

}




