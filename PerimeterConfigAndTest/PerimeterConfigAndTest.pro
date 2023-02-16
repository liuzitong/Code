#-------------------------------------------------
#
# Project created by QtCreator 2021-10-20T10:05:46
#
#-------------------------------------------------

QT       += core gui concurrent opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS USE_OPENGL

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


CONFIG += c++11


CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    DESTDIR =$$PWD/bin/debug
#    CONFIG += console
}
else{
    DESTDIR =$$PWD/bin/release
}



SOURCES += \
    main.cpp \
    mainWindow.cpp \
    table_model.cpp \
    motor_pos_table.cpp \
    generic_table.cpp \
    RbTableHeaderView.cpp \
    settings.cpp \
    models.cpp \
    gl_image.cpp


HEADERS += \
    mainWindow.h \
    table_model.h \
    motor_pos_table.h \
    generic_table.h \
    RbTableHeaderView.h \
    settings.h \
    models.h \
    gl_image.h

FORMS += \
    mainwindow.ui \

INCLUDEPATH +=$$PWD/../include
CONFIG( debug, debug|release ) {
    LIBS += User32.lib
} else {
    LIBS += User32.lib
}


win32: LIBS += OpenGL32.lib GlU32.lib
include($$PWD/../usbdev-build/usbdev/usbdev.pri )
include($$PWD/UsbViewer/UsbViewerQt.pri)

# Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    permeterconfigandtest.qrc


