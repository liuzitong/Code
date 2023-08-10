#include <QCoreApplication>
#include <QProcess>
#include <QFile>
#include <QDir>
#include <qxpack/indcom/sys/qxpack_ic_stdioipc.hxx>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

#if !defined(_WIN32)
#error "do not support this platform"
#endif
    // check if execute file location
    QFile  exe_file( QCoreApplication::applicationDirPath() + "/stdio_reader.exe" );
    if ( ! exe_file.exists()) { exe_file.setFileName( QCoreApplication::applicationDirPath() + "/../reader/stdio_reader.exe" ); }
    if ( ! exe_file.exists()) { qInfo() << "stdio_reader does not existed."; return -1; }

    // call the caller for test stdio_write
    QProcess proc;
    proc.start( exe_file.fileName());
    if( proc.waitForStarted()) {
        QxPack::IcQProcStdioIpc ipc( proc  );
        QByteArray ba("show me the money");
        if( ipc.write( ba.constData(), ba.size(), true )) {
            QxPack::IcByteArray ba_ret = ipc.read( 64, nullptr );
            qInfo("first ret: %s\n", ba_ret.constData());
        }
    }
    proc.waitForFinished();

    return 0;
}
