QT *= quick qml
QT += sql
QT += multimedia
QT += core gui
QT += widgets
QT += printsupport
QT += xml
QT += datavisualization
CONFIG += thread
CONFIG += c++11
CONFIG -= app_bundle
DEFINES += QT_DEPRECATED_WARNINGS QT_MESSAGELOGCONTEXT _QX_NO_PRECOMPILED_HEADER
QMAKE_CFLAGS += /utf-8
QMAKE_CXXFLAGS += /utf-8

TRANSLATIONS +=  $$PWD/../../perimeter_zh_CN.ts
TR_EXCLUDE += $$PWD/../../../perimeter/third-part/*


INCLUDEPATH *= $$PWD/../../../
# ///////////////////////////////////////////////////////////////////////////
# sub function module
# ///////////////////////////////////////////////////////////////////////////
DEFINES *= PERIMETER_CFG_STATIC
include( $$PWD/../../base/perimeter_base.pri )
include( $$PWD/../../main/perimeter_main.pri )
include( $$PWD/../../third-part/qxpackic_all.pri )
#include( $$PWD/../../../deviceOperation-build/deviceOperation.pri )
# ///////////////////////////////////////////////////////////////////////////
# put the final app into /tmp for debug
# ///////////////////////////////////////////////////////////////////////////
APP_INSTALL_PATH=/tmp
target.path = $${APP_INSTALL_PATH}

INSTALLS += target


# 库相关配置
INCLUDEPATH += $$PWD/../../../perimeter/third-part/QxOrm/include
INCLUDEPATH += $$PWD/../../../perimeter/third-part/LimeReport/include
INCLUDEPATH += $$PWD/../../../perimeter/third-part/LimeReport/limereport/base
INCLUDEPATH += $$PWD/../../../perimeter/third-part/


LIBS += -L$$PWD/../../../perimeter/third-part/QxOrm/lib
LIBS += -L$$PWD/../../../perimeter/third-part/boost/lib



# 设置生成的目标名称、添加依赖库
CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../../../deviceOperation/bin/debug
    LIBS += -L$$PWD/../../../perimeter/third-part/LimeReport/build/5.9.7/win32/debug/lib
    LIBS += -l"QxOrmd" -l"limereportd" -l"QtZintd" -l"deviceOperationd"
#    CONFIG += qml_debug console
#    LIBS += -l"libboost_serialization-vc140-mt-gd-x32-1_78"
    DESTDIR=$$PWD/../../bin/debug
} else {
    LIBS += -L$$PWD/../../../deviceOperation/bin/release
    LIBS += -L$$PWD/../../../perimeter/third-part/LimeReport/build/5.9.7/win32/release/lib
    LIBS += -l"QxOrm" -l"limereport" -l"QtZint" -l"deviceOperation"
    CONFIG -= qml_debug
    CONFIG -= console
    DEFINES+=QT_QML_DEBUG_NO_WARNING
#    CONFIG -= qml_debug console
#    LIBS += -l"libboost_serialization-vc140-mt-x32-1_78"
    DESTDIR=$$PWD/../../bin/release
}

DEFINES += _BUILDING_QX_API

