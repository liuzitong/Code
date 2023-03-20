#-------------------------------------------------
#
# Project created by QtCreator 2023-02-17T10:33:51
#
#-------------------------------------------------

QT       -= gui

TARGET = deviceOperation
TEMPLATE = lib

DEFINES += DEVICEOPERATION_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    $$PWD/device_settings.cpp \
    $$PWD/device_operation.cpp \
    $$PWD/device_data.cpp \
    $$PWD/device_data_processer.cpp

HEADERS += \
    $$PWD/device_settings.h \
    $$PWD/device_operation_global.h \
    $$PWD/device_operation.h \
    $$PWD/device_data.h \
    $$PWD/device_data_processer.h

#HEADERS_COPY *= $${HEADERS}

unix {
    target.path = /usr/lib
    INSTALLS += target
}
