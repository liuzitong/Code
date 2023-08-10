QT += core
QT -= gui

CONFIG += c++11

TARGET = icellipsefit
CONFIG += console
CONFIG -= app_bundle


CONFIG( debug, debug|release) {
    TARGET = diellipsefitd
    CMCF_DBG_RLS_VAL=debug
} else {
    TARGET = diellipsefit
    CMCF_DBG_RLS_VAL=release
}


TEMPLATE = app

HEADERS += $$PWD/../qxpack_ic_diellipsefit.hxx

SOURCES += main.cpp \
           $$PWD/../qxpack_ic_diellipsefit.cxx


# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include( $$PWD/../../qxpack_indcom_algor.pri )

DEFINES += APP_PWD=\\\"$${PWD}\\\"

win32 {
   !contains( QMAKE_TARGET.arch, x86_64 ) {
       CMCF_ARCHIVE_VAL=x86
   } else {
       CMCF_ARCHIVE_VAL=x64
   }
   CONFIG( debug, debug|release ) {
       QMAKE_POST_LINK *= cmake -DCMCF_BUILD_ROOT="$$OUT_PWD" -DCMCF_SRC_ROOT="$$PWD/" -DCMCF_OUT_ROOT="$$OUT_PWD/debug" -DCMCF_PLATFORM=windows -DCMCF_ARCHIVE="$$CMCF_ARCHIVE_VAL" -DCMCF_DBG_RLS="$$CMCF_DBG_RLS_VAL" -P "$$PWD/cp.txt"
   } else {
       QMAKE_POST_LINK *= cmake -DCMCF_BUILD_ROOT="$$OUT_PWD" -DCMCF_SRC_ROOT="$$PWD/" -DCMCF_OUT_ROOT="$$OUT_PWD/release" -DCMCF_PLATFORM=windows -DCMCF_ARCHIVE="$$CMCF_ARCHIVE_VAL" -DCMCF_DBG_RLS="$$CMCF_DBG_RLS_VAL" -P "$$PWD/cp.txt"
   }
}
