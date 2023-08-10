#include <QCoreApplication>
#include <qxpack/indcom/common/qxpack_ic_bytearray.hxx>
#include <qxpack/indcom/common/qxpack_ic_logging.hxx>
#include <qxpack/indcom/common/qxpack_ic_minizip.hxx>

#include <QByteArray>
#include <QString>
#include <qDebug>
#include <cstdlib>


// ========================================
// output the byte array
// ========================================
void    gOutputByteArray( QxPack::IcByteArray &ba )
{
    int   sz = ba.size(); int count = 0;
    const char *dp = ba.constData();
    while ( sz -- > 0 ) {
        std::printf("%3d, ", ( uint )( *dp ++ ) & 255 );
        if ( ++ count % 8 == 0 ) { std::printf("\n"); }
    }
}

// ========================================
// main entry
// ========================================
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QxPack::IcByteArray src_text("the/rain/in/Spain/falls/mainly/on/the/plain/"
                                 "Good morning Dr. Chandra. This is Hal. I am ready for my first lesson"

                                 );

    QxPack::IcMiniZipComp  comp( src_text.constData(), src_text.size());
    QxPack::IcByteArray  comp_data = comp.result();

    qDebug("the source size is %d", src_text.size());
    qDebug("compresse size is %d",  comp_data.size());

    QxPack::IcMiniZipDecomp decomp( comp_data.constData(), comp_data.size());
    QxPack::IcByteArray  decomp_data = decomp.result();

    qDebug("decompress size is %d", decomp_data.size());
    qDebug("decompress data is:%s", decomp_data.constData());


    return 0;
}
