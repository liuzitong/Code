include( $$PWD/deviceOperation.pri )
include( $$PWD/../usbdev-build/usbdev/usbdev.pri )
#include( $$PWD/defines.pri )
QT       += core
QT *= gui

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    DESTDIR =$$PWD/bin/debug
#    CONFIG += console
}
else{
    DESTDIR =$$PWD/bin/release
}


SRCFILE = $$DESTDIR/$${TARGET}.dll
CONFIG(debug,debug|release){
    PERIMETER_DIR = $$PWD/../perimeter/bin/debug/$${TARGET}.dll
    PERIMETER_CONFIG_DIR = $$PWD/../PerimeterConfigAndTest/bin/debug/$${TARGET}.dll

}else {
    PERIMETER_DIR = $$PWD/../perimeter/bin/release/$${TARGET}.dll
    PERIMETER_CONFIG_DIR = $$PWD/../PerimeterConfigAndTest/bin/release/$${TARGET}.dll
}

#LIB_SRC_DIR = $$DESTDIR/$${TARGET}.lib
#LIB_DEST_DIR = $$PWD/../deviceOperation-build/lib/$${TARGET}.lib
SRCFILE_WIN = $$replace(SRCFILE, "/", "\\")
PERIMETER_DIR_WIN = $$replace(PERIMETER_DIR, "/", "\\")
PERIMETER_CONFIG_DIR_WIN = $$replace(PERIMETER_CONFIG_DIR, "/", "\\")
#LIB_SRC_DIR_WIN = $$replace(LIB_SRC_DIR, "/", "\\")
#LIB_DEST_DIR_WIN = $$replace(LIB_DEST_DIR, "/", "\\")
#CURRENTFILEPATH_WIN=$$replace(PWD, "/", "\\")

message( $$SRCFILE_WIN)
message( $$PERIMETER_DIR_WIN)
message( $$PERIMETER_CONFIG_DIR_WIN)
#message( $$LIB_SRC_DIR_WIN)
#message( $$LIB_DEST_DIR_WIN)
#message( $$CURRENTFILEPATH_WIN)

QMAKE_POST_LINK += copy /Y $$SRCFILE_WIN $$PERIMETER_DIR_WIN & \
copy /Y $$SRCFILE_WIN $$PERIMETER_CONFIG_DIR_WIN & \
#copy /Y $$LIB_SRC_DIR_WIN $$LIB_DEST_DIR_WIN & \
#copy /Y $${CURRENTFILEPATH_WIN}\\deviceoperation.h $${CURRENTFILEPATH_WIN}\\..\\deviceOperation-build\\deviceoperation.h

#QMAKE_POST_LINK +=copy /Y $$LIB_SRC_DIR_WIN $$LIB_DEST_DIR_WIN










