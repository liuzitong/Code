#include <QCoreApplication>

#include "../qxpack_ic_algor_ellfit.hpp"

#include <QString>
#include <QDebug>
#include <QtTest>
#include <QThread>
#include <QList>
#include <cstdlib>
#include <cstring>
#include <QTime>
#include <QVector>
#include <QImage>
#include <QPainter>
#include <cmath>

using namespace QxPack;

// =====================================================================
// load the vector data
// =====================================================================
static void loadVectorData( std::vector<IcAlgor::EllFitPoint<double>> &pt_list, const char* x_fn, const char * y_fn, int elem_size )
{
    QFile x_file( x_fn ); QFile y_file( y_fn );
    QByteArray xa, ya;
    if ( x_file.open( QFile::ReadOnly)) {
        xa = x_file.readAll(); x_file.close();
    }
    if ( y_file.open( QFile::ReadOnly)) {
        ya = y_file.readAll(); y_file.close();
    }
    if ( ! xa.isEmpty() && ! ya.isEmpty()) {
        int num = ( xa.size() <= ya.size() ? xa.size() : ya.size()) / sizeof( double );
        double *x_pt = ( double *) xa.constData();
        double *y_pt = ( double *) ya.constData();
        for ( int n = 0; n < num; n ++ ) {
            pt_list.push_back( IcAlgor::EllFitPoint<double>( x_pt[n], y_pt[n]) );
        }
    } else {
        qFatal("read file failed!");
    }
}


// ////////////////////////////////////////////////////////////////////////////
//
//   test the error deviation template
//
// ////////////////////////////////////////////////////////////////////////////
class  Tester : public QObject {
    Q_OBJECT
private:
    Q_SLOT  void  initTestCase( ) { }
    Q_SLOT  void  cleanupTestCase ( );
    Q_SLOT  void  fitEll();
    Q_SLOT  void  fitEll_deg0();
public :

};

// ============================================================================
// output the memory counter
// ============================================================================
auto     Tester :: cleanupTestCase( ) -> void
{

}

// ============================================================================
// fit ellipse
// ============================================================================
auto     Tester :: fitEll() -> void
{
    QString x_dat_file = QString( APP_PWD "/x0.dat");
    QString y_dat_file = QString( APP_PWD "/y0.dat");

    std::vector<IcAlgor::EllFitPoint<double>> pt_list;
    loadVectorData( pt_list, x_dat_file.toLocal8Bit(), y_dat_file.toLocal8Bit(),sizeof( double ));

    qInfo("load data num:%d", pt_list.size());

    auto ell = IcAlgor::fitEllipseDirect( pt_list.data(), pt_list.size() );

    qInfo("== ellipse result ==");
    qInfo("cx :%lf  cy: %lf", ell.cx(), ell.cy() );
    qInfo("ru :%lf  rv: %lf", ell.ru(), ell.rv() );
    qInfo("theta_radius:%lf", ell.angleRad()  );

//    QImage im(800,800,QImage::Format_Grayscale8);
//    QPainter patr(&im);
//  //  patr.rotate( ell.angleDeg() );
//    patr.drawEllipse(  QPointF( ell.cx(), ell.cy()),  ell.ru(), ell.rv());
//    im.mirrored(false,true).save( "z:/ell.bmp" );
}

// ============================================================================
// fit ell. deg 0
// ============================================================================
auto     Tester :: fitEll_deg0() -> void
{
    // the ellipse rad( w = 3, h = 2 ), cx, cy = 0
    // (x*x)/(a*a) + (y*y)/(b*b) = 1.
    // y = sqrt( b*b - (x*x*b*b)/(a*a))
    // or
    // x = a * cos(theta), y = b * sin( theta )
    //
    std::vector<IcAlgor::EllFitPoint<double>> pt_list;
    for ( int x = -3; x < 4; x ++ ) {
//        double x = 0;

    //    pt_list.push_back(  );
    }
}


QTEST_MAIN( Tester )

#include "main.moc"

