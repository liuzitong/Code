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
#include <QRectF>
#include <QQuickWindow>
#include <QQuickView>
#include <QSurfaceFormat>

#include "qxpack/indcom/common/qxpack_ic_global.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/ui_qml_base/qxpack_ic_quickaryseriesdata.hxx"

// ////////////////////////////////////////////////////////////////////////////
//                 load the data from file
// ////////////////////////////////////////////////////////////////////////////
static auto  gLoadData( QxPack::IcQuickArySeriesData &ser_data ) -> bool
{
    QFile fi( APP_PWD "/sd.dat" );
    if ( ! fi.open( QFile::ReadOnly )) { return false; }
    QByteArray ba = fi.readAll();
    fi.close();
    auto dat_ptr = reinterpret_cast<int16_t*>( ba.data());
    auto dat_num = ba.size() / int( sizeof(int16_t));

    QVector<float> vec( dat_num ); auto dp = vec.data();
    for ( int i  = 0; i < dat_num; i ++ ) { dp[i] = dat_ptr[i]; }
//    std::memcpy( vec.data(), dat_ptr, dat_num * sizeof(float) );

    QxPack::IcQuickArySeriesData::SeriesData sd;
    sd.vec = vec;
    sd.x_ofv = 0;
    sd.step  = 1;
    ser_data.setData( sd );
    return true;
}

// ////////////////////////////////////////////////////////////////////////////
//                 make the view port of series data
// ////////////////////////////////////////////////////////////////////////////
static auto  gMakeViewport( const QxPack::IcQuickArySeriesData &ser_data ) -> QRectF
{
    qreal min_x =  DBL_MAX; qreal min_y =  DBL_MAX;
    qreal max_x = -DBL_MIN; qreal max_y = -DBL_MIN;

    auto sd = ser_data.data();
    auto dp = sd.vec.constData();
    auto num = sd.vec.size();
    for ( int i = 0; i < num; i ++ ) {
        if ( min_y > dp[i] ) { min_y = dp[i]; }
        if ( max_y < dp[i] ) { max_y = dp[i]; }
    }
    min_x = sd.x_ofv;
    max_x = sd.x_ofv + sd.step * ( num - 1 );


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



#if QT_VERSIOIN_MAJOR > 5
    QQuickWindow::setGraphicsApi( QSGRendererInterface::OpenGLRhi );
#else
    QSurfaceFormat fmt;
    fmt.setDepthBufferSize(24);
    fmt.setVersion(4,0);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setRenderableType(QSurfaceFormat::OpenGL);
    QSurfaceFormat::setDefaultFormat(fmt);
#endif
    //QxPack::IcMemCntr::enableMemTrace( true );

    qInfo() << "the current mem cntr:" << QxPack::IcMemCntr::currNewCntr();
    {
        QxPack::IcQuickArySeriesData ary_src( QxPack::IcQuickArySeriesData::UseMode_Normal );
#if 1
        gLoadData( ary_src );
        QRectF vw_prt = gMakeViewport( ary_src );
#else
        xy_src.setData(
            QList<QPointF>() << QPointF(0,0) << QPointF(100,100)
        );
        QRectF vw_prt = QRectF( 0, 100, 100, 100 );
#endif

//#if QT_VERSION_MAJOR < 6
        QQmlApplicationEngine *eg = new QQmlApplicationEngine( );

        eg->rootContext()->setContextProperty("gWaveLineData", &ary_src );
        eg->rootContext()->setContextProperty("gViewport", QVariant(vw_prt ));
        eg->addImportPath("qrc:/");
        eg->load(QUrl(QStringLiteral("qrc:/main.qml")));
        ret = app.exec();
        delete eg;
//#else
//        QQuickView  *view = new QQuickView;
////        view->setResizeMode(QQuickView::SizeRootObjectToView);
//        view->setWidth(800);
//        view->setHeight(600);
//        view->rootContext()->setContextProperty("gWaveLineData", &ary_src );
//        view->rootContext()->setContextProperty("gViewport", QVariant(vw_prt ));
//         view->setSource(QUrl("qrc:///main.qml"));
//        view->show();
//        ret = app.exec();
//        delete view;
//#endif
    }

 //  QxPack::IcMemCntr::saveTraceInfoToFile("z:/mem_cntr.txt" );
  // QxPack::IcMemCntr::enableMemTrace(false);
    qInfo() << " the last mem cntr:" << QxPack::IcMemCntr::currNewCntr();

    return ret;
}


