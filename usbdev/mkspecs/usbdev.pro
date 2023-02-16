# ////////////////////////////////////////////////
# configure section
# ////////////////////////////////////////////////
TEMPLATE = lib
CONFIG += qt c++11 skip_target_version_ext
CONFIG -= app_bundle
QT *= gui
QT += core


DEFINES *= QT_MESSAGELOGCONTEXT

INCLUDEPATH +=$$PWD/../../include

CONFIG(debug,debug|release) {
    DEFINES *= USBDEV_CFG_DLL
} else {
    DEFINES *= USBDEV_CFG_DLL
    DEFINES *= QT_NO_DEBUG_OUTPUT
}

QXIC_MODULE_TARGET   = usbdev
QXIC_MODULE_QT_BUILD = $$section( QMAKESPEC, /, -3, -3)
QXIC_MODULE_QT_VER   = $$section( QMAKESPEC, /, -4, -4)
QXIC_MODULE_QT_MMVER = $$section( QXIC_MODULE_QT_VER, ., 0, 1 )
QXIC_MODULE_BASE_DIR = $$PWD/../
QXIC_MODULE_INSTALL_DIR = $$PWD/../../usbdev-build
QXIC_MODULE_INCLUDE = $$QXIC_MODULE_INSTALL_DIR/usbdev
QXIC_MODULE_DESTDIR = $$QXIC_MODULE_INSTALL_DIR/usbdev/qt$$QXIC_MODULE_QT_MMVER/$$QXIC_MODULE_QT_BUILD
QXIC_MODULE_OTHER_COPY_FILES =

win32:VERSION = 0.5.0.0
else: VERSION = 0.5.0

#SRCFILE = $$QXIC_MODULE_DESTDIR/$${TARGET}d.dll

CONFIG(debug,debug|release){
    PERIMETERDIR = $$PWD/../../PerimeterConfigAndTest/bin/debug/$${TARGET}d.dll
    SRCFILE = $$QXIC_MODULE_DESTDIR/$${TARGET}d.dll
}else {
    PERIMETERDIR = $$PWD/../../PerimeterConfigAndTest/bin/release/$${TARGET}.dll
    SRCFILE = $$QXIC_MODULE_DESTDIR/$${TARGET}.dll
}
message( $$SRCFILE)
message( $$PERIMETERDIR)

SRCFILE_WIN = $$replace(SRCFILE, "/", "\\")
PERIMETERDIR_WIN = $$replace(PERIMETERDIR, "/", "\\")
message( $$SRCFILE_WIN)
message( $$PERIMETERDIR_WIN)

QMAKE_POST_LINK += copy /Y $$SRCFILE_WIN $$PERIMETERDIR_WIN

# ////////////////////////////////////////////////
# sub modules section
# ////////////////////////////////////////////////
USBDEV_CFG =
USBDEV     =
include( $$PWD/qxiccommon.pri )
include( $$PWD/../common/usbdev_common.pri )
include( $$PWD/../main/usbdev_main.pri )
include( $$PWD/../third-part/qxpack_ic_inc.pri )
#include( $$PWD/../third-part/qxpack/indcom/qxpackic_all_static.pri )

# ////////////////////////////////////////////////
# generate a copy.pro file at install directory.
# ////////////////////////////////////////////////
QXIC_MODULE_TMP_COPY_PRO = $${QXIC_MODULE_TARGET}_tmp_cpy.pro
QXIC_MODULE_COPY_FILES = \
     $$HEADERS_COPY \
     $$PWD/../common/usbdev_common.pri \
     $$PWD/../main/usbdev_main.pri \
     $$PWD/../usbdev.pri \
     $$PWD/../usbdev_update.txt \

genTempCopyProFile( $$QXIC_MODULE_TMP_COPY_PRO, $$QXIC_MODULE_COPY_FILES, $$QXIC_MODULE_BASE_DIR, $$QXIC_MODULE_INSTALL_DIR, $$QXIC_MODULE_INCLUDE )

tmp_copy_pro.commands = $$QMAKE_QMAKE $$QXIC_MODULE_INSTALL_DIR/$$QXIC_MODULE_TMP_COPY_PRO

#export(first.depends)
#export(QXIC_MODULE_tmp_copy_pro.commands)
QMAKE_EXTRA_TARGETS += tmp_copy_pro
POST_TARGETDEPS += tmp_copy_pro


# ////////////////////////////////////////////////
# linking section
# ////////////////////////////////////////////////
win32 {
    DESTDIR = $$QXIC_MODULE_DESTDIR
    CONFIG( debug, debug|release ) {
        TARGET = $${QXIC_MODULE_TARGET}d
        LIBS += kernel32.lib user32.lib gdi32.lib setupapi.lib  winusb.lib
    } else {
        TARGET = $${QXIC_MODULE_TARGET}
        LIBS += kernel32.lib user32.lib gdi32.lib setupapi.lib  winusb.lib
    }
}

unix:!android {
   CONFIG(debug,debug|release) {
       TARGET = $${QXIC_MODULE_TARGET}d
   } else {
       TARGET = $${QXIC_MODULE_TARGET}
   }
}

android {
   CONFIG(debug,debug|release) {
       TARGET = $${QXIC_MODULE_TARGET}d
   } else {
       TARGET = $${QXIC_MODULE_TARGET}
   }
   contains( QMAKE_HOST.os, Windows ) {
       cp_file_src = $${OUT_PWD}/lib$${TARGET}.so
       cp_file_dst = $${DESTDIR}/lib$${TARGET}.so
       cp_file_src = $$replace( cp_file_src, /, \\ )
       cp_file_dst = $$replace( cp_file_dst, /, \\ )
       dll_copy_tgt.commands = $$QMAKE_COPY_FILE $${cp_file_src} $${cp_file_dst}
       QMAKE_POST_LINK = $${dll_copy_tgt.commands} & $${QMAKE_POST_LINK}
   } else {
       message( you need copy $${QXPACK_MODULE_TARGET} manually. )
   }

}


