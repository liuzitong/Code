include( $$PWD/deviceOperation.pri )
include( $$PWD/../usbdev-build/usbdev/usbdev.pri )

QT       += core
QT *= gui
QT += widgets


# INCLUDEPATH += $$PWD/../opencv/build/include
# LIBS += -L$$PWD/../opencv/build/x64/vc16/lib

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    DESTDIR =$$PWD/bin/debug
    # LIBS += -l"opencv_world481d"
}
else{
    DESTDIR =$$PWD/bin/release
    # LIBS += -l"opencv_world481"
}


SRCFILE = $$DESTDIR/$${TARGET}.dll
CONFIG(debug,debug|release){
    PERIMETER_DIR = $$PWD/../perimeter/bin/debug/$${TARGET}.dll
#    PERIMETER_CONFIG_DIR = $$PWD/../PerimeterConfigAndTest/bin/debug/$${TARGET}.dll

}else {
    PERIMETER_DIR = $$PWD/../perimeter/bin/release/$${TARGET}.dll
#    PERIMETER_CONFIG_DIR = $$PWD/../PerimeterConfigAndTest/bin/release/$${TARGET}.dll
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
#copy to configer
#copy /Y $$SRCFILE_WIN $$PERIMETER_CONFIG_DIR_WIN & \











