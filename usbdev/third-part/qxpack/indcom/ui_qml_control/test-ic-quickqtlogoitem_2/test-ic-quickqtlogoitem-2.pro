QT += core gui qml

CONFIG += c++11

TARGET = example-ic-quickqtlogoitem-2

CONFIG += console warn_on  
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += $$PWD/main.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DEFINES *= QXPACK_CFG_STATIC

QXPACK_IC_CFG *= qxiccore_cfg_hdr_and_src
include( $$PWD/../../qxiccore.pri )
QXPACK_IC_CFG *= qxicqt5_cfg_hdr_and_src
include( $$PWD/../../qxicqt5.pri )
QXPACK_IC_CFG *= qxicqt5ui_cfg_hdr_and_src
include( $$PWD/../../qxicqt5ui.pri )

RESOURCES += qml.qrc

DEFINES += APP_PWD=\\\"$${PWD}\\\"
DEFINES += IMPORT_PWD=\\\"$${PWD}/../../../../../\\\"
DEFINES += QXPACK_IC_CFG_MEM_TRACE
