#include <QCoreApplication>
#include <qxpack/indcom/sys/qxpack_ic_stdioipc.hxx>

// ===================================================
// read string from stdin
// ===================================================
static void  gReadString( )
{
    bool is_read_ok;
    QxPack::IcByteArray ba = QxPack::IcStdioIpc().readAll( 64, &is_read_ok );
    if ( ba.size() > 0 ) {
        QxPack::IcStdioIpc().write( ba.constData(), ba.size());
    } else {
        printf("zero package recv.");
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    gReadString();
    return 0;
}
