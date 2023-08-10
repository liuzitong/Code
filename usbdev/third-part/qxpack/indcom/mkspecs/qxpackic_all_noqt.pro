# /////////////////////////////////////////////////////////////////////////////
# configure section
# /////////////////////////////////////////////////////////////////////////////
TEMPLATE = lib
CONFIG += c++11 skip_target_version_ext  unversioned_libname  
CONFIG -= app_bundle
DEFINES *= QT_MESSAGELOGCONTEXT
QT *= core

CONFIG(debug,debug|release) {
    DEFINES *= QXPACK_IC_CFG_STATIC 
} else {
    DEFINES *= QXPACK_IC_CFG_STATIC  
    DEFINES *= QT_NO_DEBUG_OUTPUT
}

QXIC_MODULE_QT_BUILD    = $$section( QMAKESPEC, /, -3, -3)
QXIC_MODULE_QT_VER      = $$section( QMAKESPEC, /, -4, -4)
QXIC_MODULE_QT_MMVER    = $$section( QXIC_MODULE_QT_VER, ., 0, 1 )
QXIC_MODULE_TARGET      = qxpackic_all
QXIC_MODULE_BASE_DIR    = $$PWD/../
QXIC_MODULE_INSTALL_DIR = $$PWD/../../../qxpackic-build
QXIC_MODULE_INCLUDE  = $$QXIC_MODULE_INSTALL_DIR/qxpack/indcom
QXIC_MODULE_DESTDIR  = $$QXIC_MODULE_INSTALL_DIR/qxpack/indcom/qt$$QXIC_MODULE_QT_MMVER/$$QXIC_MODULE_QT_BUILD
QXIC_MODULE_OTHER_COPY_FILES =

win32:VERSION = 0.5.22.0
else: VERSION = 0.5.22

# /////////////////////////////////////////////////////////////////////////////
# sub modules section
# /////////////////////////////////////////////////////////////////////////////
QXPACK_IC_CFG =
QXPACK_IC     =
include( $$PWD/qxiccommon.pri )
include( $$PWD/../common/qxpack_indcom_common.pri   )
include( $$PWD/../algor/qxpack_indcom_algor.pri   )
#include( $$PWD/../algor/eigen/qxpack_ic_algor_3rd_eigen.pri )

# /////////////////////////////////////////////////////////////////////////////
# generate a copy.pro file at install directory.
# /////////////////////////////////////////////////////////////////////////////
QXIC_MODULE_TMP_COPY_PRO = $${QXIC_MODULE_TARGET}_tmp_cpy.pro
QXIC_MODULE_COPY_FILES = \
     $${QXIC_MODULE_OTHER_COPY_FILES} \
     $${QXPACK_IC_HEADERS_COPY} \
     $$PWD/../qxpackic_all_noqt.pri \
     \
     $$PWD/../common/qxpack_indcom_common.pri \
     $$PWD/../common/qxpack_indcom_algor.pri \
     \
     $$PWD/../update.txt \
     $$PWD/../readme.txt \


genTempCopyProFile( $$QXIC_MODULE_TMP_COPY_PRO, $$QXIC_MODULE_COPY_FILES, $$QXIC_MODULE_BASE_DIR, $$QXIC_MODULE_INSTALL_DIR, $$QXIC_MODULE_INCLUDE )

tmp_copy_pro.commands = $$QMAKE_QMAKE $$QXIC_MODULE_INSTALL_DIR/$$QXIC_MODULE_TMP_COPY_PRO
#export(first.depends)
#export(tmp_copy_pro.commands)
QMAKE_EXTRA_TARGETS += tmp_copy_pro
POST_TARGETDEPS     += tmp_copy_pro

#QMAKE_POST_LINK += $${tmp_rmv_pro.commands}

# /////////////////////////////////////////////////////////////////////////////
# linking section
# /////////////////////////////////////////////////////////////////////////////
DESTDIR = $$QXIC_MODULE_DESTDIR
win32 {
    CONFIG( debug, debug|release ) {
        TARGET = $${QXIC_MODULE_TARGET}_noqt_d
        LIBS += kernel32.lib user32.lib gdi32.lib
    } else {
        TARGET = $${QXIC_MODULE_TARGET}_noqt
        LIBS += kernel32.lib user32.lib gdi32.lib
      #  QMAKE_CXXFLAGS *= /Zi
      #  QMAKE_CFLAGS *= /Zi
      #  QMAKE_LFLAGS *= /DEBUG
    }
}

unix:!android {
   CONFIG(debug,debug|release) {
       TARGET = $${QXIC_MODULE_TARGET}_noqt_d
   } else {
       TARGET = $${QXIC_MODULE_TARGET}_noqt
   }
}

android {
   CONFIG(debug,debug|release) {
       TARGET = $${QXIC_MODULE_TARGET}_noqt_d
   } else {
       TARGET = $${QXIC_MODULE_TARGET}_noqt
   }
   contains( QMAKE_HOST.os, Windows ) {
       cp_file_src = $${OUT_PWD}/lib$${TARGET}.a
       cp_file_dst = $${DESTDIR}/lib$${TARGET}.a
       cp_file_src = $$replace( cp_file_src, /, \\ )
       cp_file_dst = $$replace( cp_file_dst, /, \\ )
       dll_copy_tgt.commands = $$QMAKE_COPY_FILE $${cp_file_src} $${cp_file_dst}
       QMAKE_POST_LINK = $${dll_copy_tgt.commands} & $${QMAKE_POST_LINK}
   } else {
       message( you need copy $${QXPACK_MODULE_TARGET} manually. )
   }
}




