#include <QCoreApplication>

#include "../qxpack_ic_algor_pxmono.hpp"

#include <QString>
#include <QDebug>
#include <QtTest>
#include <QThread>
#include <QList>
#include <cstdlib>
#include <cstring>
#include <QPainter>
#include <QVector>
#include <tuple>

using namespace QxPack;

// ////////////////////////////////////////////////////////////////////////////
// glboal functions
// ////////////////////////////////////////////////////////////////////////////
static auto  gFillGapMono( QImage &im ) -> void
{
    im.fill(1);
    QPainter pt( &im );
    auto px_h = im.height(); auto px_w = im.width();
    for ( int h = 0; h < px_h; h += 2 ) {
        pt.drawLine( 0, h, px_w, h );
    }
}

static auto  gCmpMono( const QImage &src, const QImage &dst ) -> bool
{
    auto px_h = ( src.height() <= dst.height() ? src.height() : dst.height());
    auto px_w = ( src.width()  <= dst.width()  ? src.width()  : dst.width() );
    if ( px_h < 0 || px_w < 0 ) { return false; }

    QImage b_src = src.convertToFormat( QImage::Format_Grayscale8 );
    QImage b_dst = dst.convertToFormat( QImage::Format_Grayscale8 );

    bool is_same = true;
    for ( int h = 0; is_same && h < px_h; h ++ ) {
        is_same = ( std::memcmp( b_src.scanLine(h), b_dst.scanLine(h), size_t( px_w )) == 0 );
    }
    return is_same;
}

static auto  gMakeWaveMono( int w, int h, QVector<int> &vec ) -> QImage
{
    QImage im( w, h, QImage::Format_Mono );
    im.fill(1);
    qsrand( uint(QTime::currentTime().elapsed()) );
    for ( int i = 0; i < w; i ++ ) {
        vec.append( qAbs(qrand()) % h );
        im.setPixel( i, vec.last(), 0 );
    }
    return im;
}


// ////////////////////////////////////////////////////////////////////////////
//   test the algorithm pxmono template
// ////////////////////////////////////////////////////////////////////////////
class  Tester : public QObject {
    Q_OBJECT
private:
    Q_SLOT  void  initTestCase( ) { }
    Q_SLOT  void  cleanupTestCase ( );
    Q_SLOT  void  monoFlipColor( );
    Q_SLOT  void  monoFlipVert ( );
    Q_SLOT  void  fillWaveAryByMonoMsbBad( );
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
auto     Tester :: monoFlipColor() -> void
{
    QImage im_org(10,4, QImage::Format_Mono ); // MSB Mono
    gFillGapMono( im_org );
    QImage im_flip = im_org.copy();

    IcAlgor::monoFlipColor(
        im_flip.bits(),  im_flip.bits(),
        size_t( im_flip.width()), size_t( im_flip.height()), size_t( im_flip.bytesPerLine())
    );

    auto im_org_2 = im_org.copy ( 0, 1, im_org.width(),  im_org.height() - 2 );
    auto im_flip_2= im_flip.copy( 0, 0, im_flip.width(), im_org.height() - 2 );
    auto is_same  = gCmpMono( im_org_2, im_flip_2 );
    QVERIFY2( is_same, "flip mono color failed!" );
}

// ============================================================================
// flip the mono height
// ============================================================================
auto     Tester :: monoFlipVert() -> void
{
    struct TestPar{ int width, height; };
    TestPar par[] = {
        { 10, 5 }, { 10, 4 }, { 10, 2 }
    };

    for ( int i = 0; i < int( sizeof(par)/sizeof(par[0])); i ++ ) {
        QImage im_org( par[i].width, par[i].height, QImage::Format_Mono );
        gFillGapMono( im_org );
        if ( par[i].height % 2 != 0 ) {
            auto rsl = im_org.copy();
            IcAlgor::monoFlipVert( rsl.bits(), size_t( rsl.width()), size_t( rsl.height() ), size_t( rsl.bytesPerLine()));
            auto ret = gCmpMono( im_org, rsl );
            QVERIFY2( ret, "flip image vertically failed!" );
            IcAlgor::monoFlipVert( rsl.bits(), size_t( rsl.width()), size_t( rsl.height() ), size_t( rsl.bytesPerLine()));
            ret = gCmpMono( im_org, rsl );
            QVERIFY2( ret, "flip image vertically failed!" );
        } else {
            auto rsl = im_org.copy();
            IcAlgor::monoFlipVert( rsl.bits(), size_t( rsl.width()), size_t( rsl.height() ), size_t( rsl.bytesPerLine()));
            auto ret = gCmpMono( im_org, rsl );
            QVERIFY2( !ret, "flip image vertically failed!" );
            IcAlgor::monoFlipVert( rsl.bits(), size_t( rsl.width()), size_t( rsl.height() ), size_t( rsl.bytesPerLine()));
            ret = gCmpMono( im_org, rsl );
            QVERIFY2( ret, "flip image vertically failed!" );
        }

    }
}

// ============================================================================
// fill wave array by mono msb order image
// ============================================================================
auto     Tester :: fillWaveAryByMonoMsbBad() -> void
{
    QVector<int> ary;
    auto im_bk = gMakeWaveMono( 100, 100, ary );
    auto im_wt = im_bk.copy();

    IcAlgor::monoFlipColor(
        im_bk.constBits(), im_wt.bits(),
        size_t( im_wt.width()), size_t( im_wt.height()), size_t( im_wt.bytesPerLine())
    );

//    im_bk.save( "z:/bk1.bmp");
//    im_wt.save( "z:/wt1.bmp");

    QVector<int> rsl_bk( im_bk.width(), 0 );
    IcAlgor::fillWaveAryByMonoMsb(
        im_bk.bits(), size_t( im_bk.width()), size_t( im_bk.height()), size_t( im_bk.bytesPerLine()),
        rsl_bk.data(), size_t( rsl_bk.size()), false, false
    );

    QVector<int> rsl_wt( im_wt.width(), 0 );
    IcAlgor::fillWaveAryByMonoMsb(
        im_wt.bits(), size_t( im_wt.width()), size_t( im_wt.height()), size_t( im_wt.bytesPerLine()),
        rsl_wt.data(), size_t( rsl_wt.size()), true, false
    );

//    qInfo() << ary;
//    qInfo() << rsl_bk;
//    qInfo() << rsl_wt;
    QVERIFY2( ary == rsl_bk, "bad rsl black as data!" );
    QVERIFY2( ary == rsl_wt, "bad rsl white as data!" );
}



QTEST_MAIN( Tester )

#include "main.moc"

