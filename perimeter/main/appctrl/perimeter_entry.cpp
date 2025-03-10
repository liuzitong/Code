﻿#include <QCoreApplication>
#include <QDebug>
#include <QMessageLogContext>
#include <QThread>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSharedPointer>
#include <QApplication>
#include <QIcon>
#include <QProcess>
#include <QSurfaceFormat>
#include <QTextCodec>

#include "perimeter/main/appctrl/perimeter_appctrl.hxx"
#include "perimeter/base/common/perimeter_memcntr.hxx"
#include "perimeter/main/services/translate_svc.h"
#include "perimeter/main/services/keyboard_filter.h"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/common/qxpack_ic_global.hxx"
#include "qxpack/indcom/sys/qxpack_ic_eventloopbarrier.hxx"
#include "qxpack/indcom/sys/qxpack_ic_appdcl_priv.hxx"
#include "qxpack/indcom/ui_qml_base/qxpack_ic_ui_qml_api.hxx"
#include "perimeter/main/services/JRpcServerSvc.hxx"
#include "perimeter/base/common/perimeter_guns.hxx"
#include "perimeter/third-part/perm/local-part/resimprov/perm_improv.hxx"
#include "perimeter/third-part/perm/perm_mod.hxx"
#include "../ipckbd/build/include/common/ipcclientevtmon.hxx"
#include "../ipckbd/build/include/common/regcommonitem_api.hxx"
#include <signal.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <perimeter/main/services/dicom.h>
#include <perimeter/main/services/datevalidator.h>
#if !defined(WIN32)
#include <unistd.h>
#endif
#if  defined(_MSC_VER )
#include <windows.h>
#endif

#if defined( QT_DEBUG )
#  if defined( Q_OS_WIN32 ) || defined( Q_OS_WIN )
#    define TEXT_PermExe  QStringLiteral("rpcpermd.exe")
#    define TEXT_KbdExe  QStringLiteral("ipckbdsrvd.exe")
#  else
#    define TEXT_PermExe  QStringLiteral("rpcpermd")
#    define TEXT_KbdExe  QStringLiteral("ipckbdsrvd")
#  endif
#else
#  if defined( Q_OS_WIN32 ) || defined( Q_OS_WIN )
#    define TEXT_PermExe  QStringLiteral("rpcperm.exe")
#    define TEXT_KbdExe  QStringLiteral("ipckbdsrv.exe")
#  else
#    define TEXT_PermExe  QStringLiteral("rpcperm")
#    define TEXT_KbdExe  QStringLiteral("ipckbdsrv")
#  endif
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

bool  checkMyselfExist()//如果程序已经有一个在运行，则返回true
{
    HANDLE  hMutex = CreateMutex(NULL, FALSE, L"perimeter");
    if (hMutex && (GetLastError() == ERROR_ALREADY_EXISTS))
    {
        CloseHandle(hMutex);
        hMutex = NULL;
        return true;
    }
    else{
        return false;
    }
}


// ============================================================================
// main entry
// ============================================================================
#include <qmath.h>

int  main ( int argc, char *argv[] )
{
    if(checkMyselfExist()) return 0;
    int ret = 0;
    //handle the terminate signal
    signal( SIGTERM, & gSigTerm_Handler );
    qInstallMessageHandler( & gMsgHandler );

    spdlog::rotating_logger_mt("logger", "logs/log.txt", 1024*1024*100, 30);
    spdlog::flush_on(spdlog::level::info);

    QProcess p;
    p.execute("taskkill /im jrpcplatsvr.exe /f");
    p.execute("taskkill /im "+ TEXT_KbdExe+" /f");
    p.execute("taskkill /im "+ TEXT_PermExe+" /f");
    p.close();

    // start the application
    gPrintMemCntr("pre-stage");
    {
        QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
        QSurfaceFormat fmt;
        fmt.setDepthBufferSize(16); fmt.setAlphaBufferSize(8); fmt.setRedBufferSize(8); fmt.setGreenBufferSize(8); fmt.setBlueBufferSize(8);
        fmt.setStencilBufferSize(16);
        fmt.setVersion(3,0);
        QSurfaceFormat::setDefaultFormat(fmt);

        QCoreApplication::setAttribute(Qt::AA_DisableShaderDiskCache);

        QApplication app(argc, argv);

        JRpcServerSvc jprc_svc; jprc_svc.start(GUNS_JRPC_SVC_NAME);
        Perimeter::DateValidator dateValidater;
        Perimeter::AppCtrl *app_ctrl = new Perimeter::AppCtrl;
        app_ctrl->doInit();

        // --------------------------------------------------------------------
        // share the application controller into QML
        // --------------------------------------------------------------------
        QSharedPointer<QObject> s_app_ctrl( app_ctrl, [](QObject*){});
        QxPack::IcUiQmlApi::setAppCtrl( s_app_ctrl );
        FcPerm::PermMod perm_mod; perm_mod.registerTypesEx(app_ctrl);
        IpcKbd::regCommonItem();
        IpcKbd::IpcClientEvtMon  *cli_mon = new IpcKbd::IpcClientEvtMon( "CE_IPC_KBD", & app );
        app.installEventFilter( cli_mon );

        //  here create the main view
        // --------------------------------------------------------------------
        QQmlApplicationEngine *eng = new QQmlApplicationEngine;
        eng->rootContext()->setContextProperty("applicationDirPath", QGuiApplication::applicationDirPath());
        eng->rootContext()->setContextProperty( QStringLiteral("gVkbd"), qobject_cast< QObject* >( cli_mon ));
        eng->rootContext()->setContextProperty("dateValidator",&dateValidater);
        eng->addImportPath(QStringLiteral("qrc:/") );
        eng->addImageProvider( QStringLiteral("PermImProv"), new FcPerm::PermImProv( ) );

        eng->load(QUrl(QLatin1String("qrc:/perimeter/main/view/Application.qml")));
        qmlRegisterSingletonType(QUrl("qrc:/perimeter/main/view/Utils/CusUtils.qml"), "perimeter.main.view.Utils", 1, 0, "CusUtils");

        gPrintMemCntr("enter eventloop stage");


        app.setWindowIcon(QIcon(":/Pics/base-svg/2logo_256_black.svg"));
        auto filter= Perimeter::KeyBoardFilter::getSingleton();

        app.installEventFilter(filter.data());
        ret = app.exec();
        eng->deleteLater();

        jprc_svc.stop();
        QxPack::IcAppDclPriv::barrier( 32 );
        app_ctrl->doDeinit();
        delete app_ctrl;
        QxPack::IcAppDclPriv::barrier( 32 );
    }
    gPrintMemCntr("post-stage");

    return ret;
}

