#include <QCoreApplication>

#include "../qxpack_ic_algor_pxgray.hpp"

#include <QString>
#include <QDebug>
#include <QtTest>
#include <QThread>
#include <QList>
#include <cstdlib>
#include <cstring>
#include <QPainter>
#include <QImage>
#include <QVector>
#include <tuple>
#include <QDateTime>
#include <QColor>


using namespace QxPack;

// ////////////////////////////////////////////////////////////////////////////
// glboal functions
// ////////////////////////////////////////////////////////////////////////////
static auto  gCmpGray( const QImage &src, const QImage &dst ) -> bool
{
    if ( src.width() != dst.width() || src.height() != dst.height()) { return false; }

    auto px_h = src.height(); auto px_w = src.width();
    bool is_same = true;
    for ( int h = 0; is_same && h < px_h; h ++ ) {
        is_same = ( std::memcmp( src.scanLine(h), dst.scanLine(h), size_t( px_w )) == 0 );
    }
    return is_same;
}

static auto  gMakeRandGray( int w, int h ) -> QImage
{
    QImage im( w, h, QImage::Format_Grayscale8 );
    qsrand( uint(QDateTime::currentDateTime().toMSecsSinceEpoch()) );

    auto px_h = im.height(); auto px_w = im.width();
    for ( int h = 0; h < px_h; h ++ ) {
       for ( int w = 0; w < px_w; w ++ ) {
           auto px = quint8( qAbs(qrand()) % 255 );
           im.setPixel( w, h, ( 255UL << 24 ) | uint( px << 16 ) << uint( px << 8 ) | px );
       }
    }

    return im;
}

static auto  gMakeInvGrayTbl() -> QVector<quint8>
{
    QVector<quint8> tbl( 256, 0 );
    for ( int i = 0; i < 256; i ++ ) {
        tbl[ i ] = quint8( 256 - i );
    }
    return tbl;
}

static auto  gMakeGrayTbl() -> QVector<quint8>
{
    QVector<quint8> tbl(256,0);
    for ( int i = 0; i < 256; i ++ ) {
        tbl[ i ] = quint8( i );
    }
    return tbl;
}

static auto  gEvalThresh( const QImage &im ) -> uint8_t
{
    QVector<int> hist; hist.resize(256);

    // 0
    std::memset( hist.data(), 0, hist.size() * sizeof(int));
    auto im_w = im.width(); auto im_h = im.height();
    for ( int i = 0; i < im_h; i ++ ) {
        auto px = im.scanLine( i );
        for ( int j = 0; j < im_w; j ++ ) { ++ hist[ px[j] ]; }
    }

    // 1

}


//static auto  gMirrorExtendGrayEdge( const QImage &im, int bdr_sz ) -> QImage
//{
//    QImage rsl( im.width() + bdr_sz * 2, im.height() + bdr_sz * 2, im.format() );

//    QPainter painter( &rsl );
//    painter.drawImage( 0, 0, im.copy(  ) )

//    return rsl;
//}

// ////////////////////////////////////////////////////////////////////////////
//   test the algorithm pxmono template
// ////////////////////////////////////////////////////////////////////////////
class  Tester : public QObject {
    Q_OBJECT
private:
    Q_SLOT  void  initTestCase( ) { }
    Q_SLOT  void  cleanupTestCase ( );
    Q_SLOT  void  grayRemapColor_g8( );

public :

};

// ============================================================================
// output the memory counter
// ============================================================================
auto     Tester :: cleanupTestCase( ) -> void
{

}

// ============================================================================
// flip the mono color
// ============================================================================
auto     Tester :: grayRemapColor_g8() -> void
{
    auto im_org = gMakeRandGray( 100, 100 );
    auto im_rsl = im_org.copy();
    auto inv_tbl= gMakeInvGrayTbl();

    IcAlgor::grayRemapColor(
        im_org.bits(), im_rsl.bits(),
        size_t( im_org.width()),         size_t( im_org.height() ),
        size_t( im_org.bytesPerLine()),  size_t( im_rsl.bytesPerLine()),
        inv_tbl.data(), size_t( inv_tbl.size())
    );
    auto ret = gCmpGray( im_org, im_rsl );
    QVERIFY2( !ret, "remap color failed!");

    IcAlgor::grayRemapColor(
        im_rsl.bits(), im_rsl.bits(),
        size_t( im_rsl.width()),         size_t( im_rsl.height() ),
        size_t( im_rsl.bytesPerLine()),  size_t( im_rsl.bytesPerLine()),
        inv_tbl.data(), size_t( inv_tbl.size())
    );

    ret = gCmpGray( im_org, im_rsl );
    QVERIFY2(  ret, "remap color failed!");
}





QTEST_MAIN( Tester )

#include "main.moc"

