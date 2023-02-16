#include "mainWindow.h"
#include <QtWidgets/QApplication>
#include <spdlog/spdlog.h>
#include "spdlog/sinks/basic_file_sink.h"
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/common.h>
#include <QDebug>
#include <settings.h>
#include <QSharedPointer>
#include <QStyleFactory>
#include <QThread>
#include <signal.h>


// ============================================================================
// handle the SIGTERM
// ============================================================================
static void  gSigTerm_Handler( int sig )
{
    if ( sig == SIGTERM ) {
        QCoreApplication::quit();
    }
}

static void gMsgHandler( QtMsgType type, const QMessageLogContext &ctxt, const QString &msg )
{
#ifdef _DEBUG
    QString  fmt_str;
    uint t_id = reinterpret_cast<uint>( QThread::currentThread());
//    char tmp[26]; QxPack::tailPath( ctxt.file, tmp, sizeof(tmp), 1 );

    switch( type ) {
    case QtDebugMsg : {
        fmt_str = QString("[Debug] 0x%1 %2 ( ln:%3, %4 %5 )\n")
                  .arg( t_id, 0, 16 ).arg( msg ).arg( ctxt.line ).arg( QString( ctxt.function )).arg(  ctxt.file );
    } break;

    case QtInfoMsg  : {
        fmt_str = QString("[Info ] 0x%1 %2 ( ln:%3, %4 %5 )\n")
                  .arg( t_id, 0, 16 ).arg( msg ).arg( ctxt.line ).arg( QString( ctxt.function )).arg(  ctxt.file );
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
                  .arg( t_id, 0, 16 ).arg( msg ).arg( ctxt.line ).arg( QString( ctxt.function )).arg(  ctxt.file );
    } break;

    case QtCriticalMsg : {
        char tmp[26];
        fmt_str = QString("[Criti] 0x%1 %2 ( ln:%3, %4 %5 )\n")
                  .arg( t_id, 0, 16 ).arg( msg ).arg( ctxt.line ).arg( QString( ctxt.function )).arg(  ctxt.file );
    } break;

    case QtFatalMsg : {
        fmt_str = QString("[Fatal] 0x%1 %2 ( ln:%3, %4, %5 )\n")
                  .arg( t_id, 0, 16 ).arg( msg ).arg( ctxt.line ).arg( QString( ctxt.function )).arg(  ctxt.file );
    }   abort();
    }


    #if defined(_MSC_VER)
        OutputDebugStringW( reinterpret_cast<LPCWSTR>( fmt_str.constData()) );
    #else
        std::fprintf( stderr, fmt_str.toUtf8().constData() );
    #endif
#endif

}


int main(int argc, char *argv[])
{
    signal( SIGTERM, & gSigTerm_Handler );
    qInstallMessageHandler( & gMsgHandler );
    QApplication::setStyle(QStyleFactory::create("Fusion"));
//    spdlog::info("Welcome to spdlog!");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();

}
