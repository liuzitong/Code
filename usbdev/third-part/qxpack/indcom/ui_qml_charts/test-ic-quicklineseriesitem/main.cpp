#include <QDebug>
#include <QGuiApplication>
#include <QByteArray>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QThread>
#include <QSharedPointer>
#include <QList>
#include <QPointF>
#include <QTimer>
#include <QFile>
#include <QByteArray>
#include <QVariant>

#include "qxpack/indcom/common/qxpack_ic_global.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/ui_qml_base/qxpack_ic_quickxyseriesdata.hxx"

// ////////////////////////////////////////////////////////////////////////////
//                 load the data from file
// ////////////////////////////////////////////////////////////////////////////
static auto  gLoadData( QxPack::IcQuickXYSeriesData &ser_data ) -> bool
{
    QFile fi( APP_PWD "/data1.dat" );
    if ( ! fi.open( QFile::ReadOnly )) { return false; }
    QByteArray ba = fi.readAll();
    fi.close();
    auto dat_ptr = reinterpret_cast<quint16*>( ba.data());
    auto dat_num = ba.size() / int( sizeof(quint16));

    QList<QPointF> pts;
    pts.reserve( dat_num );
    for ( int i = 0; i < dat_num; i ++ ) { pts.append( QPointF( i, dat_ptr[i] )); }

    ser_data.setData( pts );
    return true;
}

// ////////////////////////////////////////////////////////////////////////////
//                 make the view port of series data
// ////////////////////////////////////////////////////////////////////////////
static auto  gMakeViewport( const QxPack::IcQuickXYSeriesData &ser_data ) -> QRectF
{
    qreal min_x =  DBL_MAX; qreal min_y =  DBL_MAX;
    qreal max_x = -DBL_MIN; qreal max_y = -DBL_MIN;
    auto lst = ser_data.data();
    auto itr = lst.constBegin();
    while ( itr != lst.constEnd()) {
        auto &pt = *( itr ++ );
        if ( min_x > pt.x()) { min_x = pt.x(); }
        if ( min_y > pt.y()) { min_y = pt.y(); }
        if ( max_x < pt.x()) { max_x = pt.x(); }
        if ( max_y < pt.y()) { max_y = pt.y(); }
    }
    return QRectF( min_x, max_y, max_x - min_x + 1, max_y - min_y + 1 );
}

// ////////////////////////////////////////////////////////////////////////////
//                 main entry
// ////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[])
{
    int ret;
    QGuiApplication app( argc, argv );
    qDebug() << "main thread id:" << QThread::currentThread();

   // QxPack::IcMemCntr::enableMemTrace( true );

    qInfo() << "the current mem cntr:" << QxPack::IcMemCntr::currNewCntr();
    {
        QxPack::IcQuickXYSeriesData xy_src( QxPack::IcQuickXYSeriesData::UseMode_Normal );
#if 1
        gLoadData( xy_src );
        QRectF vw_prt = gMakeViewport( xy_src );
#else
        xy_src.setData(
            QList<QPointF>() << QPointF(0,0) << QPointF(100,100)
        );
        QRectF vw_prt = QRectF( 0, 100, 100, 100 );
#endif

        QQmlApplicationEngine *eg = new QQmlApplicationEngine( );
        eg->rootContext()->setContextProperty("gWaveLineData", &xy_src );
        eg->rootContext()->setContextProperty("gViewport", QVariant(vw_prt ));
        eg->addImportPath("qrc:/");
        eg->load(QUrl(QStringLiteral("qrc:/main.qml")));
        ret = app.exec();
        delete eg;
    }

   //QxPack::IcMemCntr::saveTraceInfoToFile("z:/mem_cntr.txt" );
   //QxPack::IcMemCntr::enableMemTrace(false);
    qInfo() << " the last mem cntr:" << QxPack::IcMemCntr::currNewCntr();

    return ret;
}


