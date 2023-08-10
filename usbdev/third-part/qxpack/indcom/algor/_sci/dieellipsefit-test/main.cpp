#include <QCoreApplication>
#include <QDebug>
#include <QList>
#include <QPointF>
#include <QFile>
#include <QByteArray>
#include "../qxpack_ic_diellipsefit.hxx"
#include <iostream>

// =====================================================================
// load the vector data
// =====================================================================
static void loadVectorData( std::vector<QxPack::IcDiEllipseFit::PointF> &pt_list, const char* x_fn, const char * y_fn, int elem_size )
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
            pt_list.push_back( QxPack::IcDiEllipseFit::PointF( x_pt[n], y_pt[n]) );
        }
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString x_dat_file = QString( APP_PWD "/x0.dat");
    QString y_dat_file = QString( APP_PWD "/y0.dat");

    std::vector<QxPack::IcDiEllipseFit::PointF> pt_list;
    loadVectorData( pt_list, x_dat_file.toLocal8Bit(), y_dat_file.toLocal8Bit(),sizeof( double ));
    std::printf("load data num: %d\n", pt_list.size());

    QxPack::IcDiEllipseFit ell( pt_list );

    // output the ellipse info.
    std::printf("== ellipse result ==\n");
    std::printf("cx :%f  cy: %f\n", ell.cx(), ell.cy());
    std::printf("ru :%f  rv: %f\n", ell.ru(), ell.rv());
    std::printf("theta_radius:%f\n", ell.thetaRadius());
    return 0;
}
