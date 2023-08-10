#include <QCoreApplication>
#include <QDebug>
#include <QObject>
#include <QtTest>
#include <cstdlib>
#include <cstdio>
#include <memory>
#include "qxpack/indcom/common/qxpack_ic_ipcsem.hxx"
#include <QProcess>
#include <QByteArray>
#include <QString>
#include <QCoreApplication>
// ///////////////////////////////////////////////////////////
// main entry
// ///////////////////////////////////////////////////////////
int main( int argc, char**argv)
{
    QCoreApplication a(argc,argv);
    bool ret = false;

    {
        QxPack::IcIpcSem  sem;
        sem.init( "TestIcIpcSem", 0 ); // create the system semaphore
        qInfo() << "create sem:" << ( sem.isWorked() ? "OK" : "Err");

        QProcess  proc;

#if  defined( APP_WIN32 )
        proc.start( APP_PWD "/test-ic-syssem-client.exe" );
#elif defined( APP_LINUX )
        proc.start( QCoreApplication::applicationDirPath() + "/test-ic-syssem-client");
#else
#  error "this platform is not supported."
#endif
        qInfo() << "start the child process:" << proc.program();
        qInfo() << "start process result:" << proc.state();
        if ( proc.waitForStarted()) {
            qInfo("child process started, PID:0x%x", ( unsigned int ) proc.pid() );
            ret = sem.post();
            qInfo("post a sys-sem:%s", ( ret ? "OK" : "Err" ));
            if ( ret ) {
                qInfo("try wait for child process finished.");
                ret = proc.waitForFinished();
                qInfo("wait for child:%s", ( ret ? "OK" : "Err" ));
                if ( ret ) {
                    qInfo() << "the result:" << QString( proc.readAllStandardOutput());
                }
            }
            if ( ! ret ) { proc.kill(); }
        } else {
            qInfo() << "the result:" << QString( proc.readAllStandardOutput());
            ret = false;
        }
    }

    return 0;
}

