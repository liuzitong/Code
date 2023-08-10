
#include <QDebug>
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include <QtTest>
#include <QGuiApplication>
#include <QtQuickTest/quicktest.h>
#include <QByteArray>
#include <QtTest/qtest.h>
#include <QtQuickTest/quicktestglobal.h>

// ////////////////////////////////////////////////////////////////////////////
//
// test object
//
// ////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[])
{
    Q_UNUSED(argc); Q_UNUSED(argv);
    QTEST_ADD_GPU_BLACKLIST_SUPPORT
    QTEST_SET_MAIN_SOURCE_PATH
    const char *tmp_argv[] = {
        "", "-input", APP_PWD,
        "-import", IMPORT_PWD,
        "-import", "qrc:/"
    };
    //QUICK_TEST_MAIN();
    return quick_test_main(sizeof(tmp_argv)/sizeof(char*), const_cast<char**>(tmp_argv),"ic-ui-modalpopupdialog", nullptr );
}
