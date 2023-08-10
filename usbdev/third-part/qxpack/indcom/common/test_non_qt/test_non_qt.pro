TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
QT -= core

DEFINES *= QXPACK_CFG_STATIC
include( $$PWD/../qxpack_indcom_common.pri )

SOURCES += \
        main.cpp
