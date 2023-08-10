#include <stdio.h>
#include <stdlib.h>
#include <QImage>
#include <QCoreApplication>
#include <QVector>
#include <QColor>

#include "../qxpack_ic_randcolor_api.h"


// ////////////////////////////////////////////////////////////////////////////
// entry
// ////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    QCoreApplication app( argc, argv );

    QImage im( 256, 256, QImage::Format_RGBA8888 );

    QVector<QColor> tbl;
    tbl.reserve(65536);
    QxPack_IcRandColorByHue(
        ( int )( &tbl), 0.5, 0.95,
        [](void*ctxt, int r, int g, int b )->int{
            QVector<QColor> *t = reinterpret_cast<QVector<QColor>*>( ctxt );
            t->append( QColor( r, g, b ));
            return ( t->size() >= 65536 ? 0 : 1 );
        }, &tbl
    );

    for ( int i = 0; i < 256; i ++ ) {
        for ( int j = 0; j < 256; j ++ ) {
            const QColor &c = tbl.at( i * 256 + j );
            im.setPixelColor( j, i, c );
        }
    }

    im.save( "z:/t.bmp");

    return 0;
}
