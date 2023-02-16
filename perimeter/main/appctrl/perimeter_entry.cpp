#include <QCoreApplication>
#include <QDebug>
#include <QMessageLogContext>
#include <QThread>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSharedPointer>
#include <QApplication>
#include <QIcon>

#include "perimeter/main/appctrl/perimeter_appctrl.hxx"
#include "perimeter/base/common/perimeter_memcntr.hxx"
#include "perimeter/main/services/translate_svc.h"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/common/qxpack_ic_global.hxx"
#include "qxpack/indcom/sys/qxpack_ic_eventloopbarrier.hxx"
#include "qxpack/indcom/sys/qxpack_ic_appdcl_priv.hxx"
#include "qxpack/indcom/ui_qml_base/qxpack_ic_ui_qml_api.hxx"



#include <cstdio>
#include <signal.h>
#if !defined(WIN32)
#include <unistd.h>
#endif
#if  defined(_MSC_VER )
#include <windows.h>
#endif

// ============================================================================
// handle the SIGTERM
// ============================================================================
static void  gSigTerm_Handler( int sig )
{
    if ( sig == SIGTERM ) {
        QCoreApplication::quit();
    }
}

// ============================================================================
// print out current memory counter
// ============================================================================
static void  gPrintMemCntr( const char *stage )
{
    qInfo("%s, current new counter = %d", stage, perimeter_curr_new_cntr );
    qInfo("%s, current qxpack ic counter=%d", stage, qxpack_ic_curr_new_cntr );
}

// ============================================================================
// the message handler
// ============================================================================
static void gMsgHandler( QtMsgType type, const QMessageLogContext &ctxt, const QString &msg )
{
#ifdef _DEBUG
    QString  fmt_str;
    uint t_id = reinterpret_cast<uint>( QThread::currentThread());
    char tmp[26]; QxPack::tailPath( ctxt.file, tmp, sizeof(tmp), 1 );

    switch( type ) {
    case QtDebugMsg : {
        fmt_str = QString("[Debug] 0x%1 %2 ( ln:%3, %4 %5 )\n")
                  .arg( t_id, 0, 16 ).arg( msg ).arg( ctxt.line ).arg( QString( ctxt.function )).arg( tmp );
    } break;

    case QtInfoMsg  : {
        fmt_str = QString("[Info ] 0x%1 %2 ( ln:%3, %4 %5 )\n")
                  .arg( t_id, 0, 16 ).arg( msg ).arg( ctxt.line ).arg( QString( ctxt.function )).arg( tmp );
    } break;

    case QtWarningMsg : {
        if  (
                msg.contains("QFont")||
                msg.contains(R"(Binding loop detected for property "margins")")||
                msg.contains(R"(Binding loop detected for property "font.pointSize")")||
                msg.contains("TypeError: Cannot read property")||
                msg.contains("depends on non-NOTIFYable properties")||
                QString(ctxt.function).contains("QQmlJavaScriptExpression")
            )
        break;

        fmt_str = QString("[Warn ] 0x%1 %2 ( ln:%3, %4 %5 )\n")
                  .arg( t_id, 0, 16 ).arg( msg ).arg( ctxt.line ).arg( QString( ctxt.function )).arg( tmp );
    } break;

    case QtCriticalMsg : {
        char tmp[26];
        fmt_str = QString("[Criti] 0x%1 %2 ( ln:%3, %4 %5 )\n")
                  .arg( t_id, 0, 16 ).arg( msg ).arg( ctxt.line ).arg( QString( ctxt.function )).arg( tmp );
    } break;

    case QtFatalMsg : {
        fmt_str = QString("[Fatal] 0x%1 %2 ( ln:%3, %4, %5 )\n")
                  .arg( t_id, 0, 16 ).arg( msg ).arg( ctxt.line ).arg( QString( ctxt.function )).arg( tmp );
    }   abort();
    }


    #if defined(_MSC_VER)
        OutputDebugStringW( reinterpret_cast<LPCWSTR>( fmt_str.constData()) );
    #else
        std::fprintf( stderr, fmt_str.toUtf8().constData() );
    #endif
#endif

}



// ============================================================================
// main entry
// ============================================================================
#include <qmath.h>

int  main ( int argc, char *argv[] )
{
    int ret = 0;
    //handle the terminate signal

    signal( SIGTERM, & gSigTerm_Handler );
    qInstallMessageHandler( & gMsgHandler );



    // start the application
    gPrintMemCntr("pre-stage");
    {
        QApplication app(argc, argv);

//        LimeReport::ReportEngine* report = new LimeReport::ReportEngine();

//        report->dataManager()->clearUserVariables();

//        report->setShowProgressDialog(false);
//        report->designReport();




        Perimeter::AppCtrl *app_ctrl = new Perimeter::AppCtrl;
        app_ctrl->doInit();

        // --------------------------------------------------------------------
        // share the application controller into QML
        // --------------------------------------------------------------------
        QSharedPointer<QObject> s_app_ctrl( app_ctrl, [](QObject*){});
        QxPack::IcUiQmlApi::setAppCtrl( s_app_ctrl );

//        auto transCon=Perimeter::TranslateController::instance();
//        transCon->loadLanguage(QLocale::Chinese);

        //  here create the main view
        // --------------------------------------------------------------------
        QQmlApplicationEngine *eng = new QQmlApplicationEngine;
        eng->rootContext()->setContextProperty("applicationDirPath", QGuiApplication::applicationDirPath());
        eng->addImportPath(QStringLiteral("qrc:/") );

        eng->load(QUrl(QLatin1String("qrc:/perimeter/main/view/Application.qml")));
        qmlRegisterSingletonType(QUrl("qrc:/perimeter/main/view/Utils/CusUtils.qml"), "perimeter.main.view.Utils", 1, 0, "CusUtils");

        gPrintMemCntr("enter eventloop stage");

        app.setWindowIcon(QIcon(":/Pics/base-svg/2logo_256_black.svg"));
        ret = app.exec();
        eng->deleteLater();


        QxPack::IcAppDclPriv::barrier( 32 );
        app_ctrl->doDeinit();
        delete app_ctrl;
        QxPack::IcAppDclPriv::barrier( 32 );
    }
    gPrintMemCntr("post-stage");


    return ret;
}

