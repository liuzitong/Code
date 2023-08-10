#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <QThreadPool>
#include <QtTest>
#include <QJsonDocument>
#include <QByteArray>
#include <QTimer>

#include "qxpack/indcom/sys/qxpack_ic_jotaskworker.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/sys/qxpack_ic_apptot_priv.hxx"
#include "qxpack/indcom/sys/qxpack_ic_appdcl_priv.hxx"

// ////////////////////////////////////////////////////////////////////////////
//
// test object
//
// ////////////////////////////////////////////////////////////////////////////
class  TestJoTaskWorker : public QObject {
    Q_OBJECT
private:
Q_SLOT  void initTestCase( ) {  }
Q_SLOT  void cleanupTestCase( );
Q_SLOT  void testPostAndWaitForDone( );
Q_SLOT  void testPostAndNoWaitForDone( );
public :
    TestJoTaskWorker ( );
};

// ============================================================================
// ctor
// ============================================================================
    TestJoTaskWorker :: TestJoTaskWorker ( ) {  }

// ============================================================================
// final cleanup
// ============================================================================
void  TestJoTaskWorker :: cleanupTestCase()
{
    qInfo() << "current MemCntr:" << QxPack::IcMemCntr::currNewCntr();
}

// ============================================================================
// task handler
// ============================================================================
static void  gNormalTask( void*, const QJsonObject &jo, const QxPack::IcVarData&, QxPack::IcJoTaskWorker::HandlerType h_type )
{
    if ( h_type == QxPack::IcJoTaskWorker::HandlerType_Routine ) {
        QThread::msleep(800);
        qInfo() << "[ NORM ] do  : " << jo;
    } else {
        qInfo() << "[ NORM ] drop: " << jo;
    }
}
static void  gLongTask( void*, const QJsonObject &jo, const QxPack::IcVarData&, QxPack::IcJoTaskWorker::HandlerType h_type )
{
    if ( h_type == QxPack::IcJoTaskWorker::HandlerType_Routine ) {
        QThread::msleep(3000);
        qInfo() << "[ LONG ] do  : " << jo;
    } else {
        qInfo() << "[ LONG ] drop: " << jo;
    }
}
static void  gShortTask( void*, const QJsonObject &jo, const QxPack::IcVarData&, QxPack::IcJoTaskWorker::HandlerType h_type )
{
    if ( h_type == QxPack::IcJoTaskWorker::HandlerType_Routine ) {
        QThread::msleep(200);
        qInfo() << "[ SHORT] do  : " << jo;
    } else {
        qInfo() << "[ SHORT] drop: " << jo;
    }
}

// ============================================================================
//  test the call queue
// ============================================================================
void   TestJoTaskWorker :: testPostAndWaitForDone()
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    //QxPack::IcMemCntr::enableMemTrace( true );

    {
        QxPack::IcJoTaskWorker *worker = new QxPack::IcJoTaskWorker;
        worker->post(
           & gNormalTask, Q_NULLPTR, QJsonDocument::fromJson(QByteArray("{\"name\":\"task1\"}")).object()
        );
        worker->post(
           & gLongTask, Q_NULLPTR, QJsonDocument::fromJson(QByteArray("{\"name\":\"task2\"}")).object()
        );
        worker->post(
           & gShortTask, Q_NULLPTR, QJsonDocument::fromJson(QByteArray("{\"name\":\"task3\"}")).object()
        );
        worker->waitForAllDone();
        worker->deleteLater();

        QObject::connect(
            worker, & QxPack::IcJoTaskWorker::destroyed,
            [](QObject*){
                QCoreApplication::quit();
            }
        );
    }

    QxPack::IcAppTotPriv::setTot( 10000 );
    QCoreApplication::exec();
    QxPack::IcAppDclPriv::barrier();

    //QxPack::IcMemCntr::saveTraceInfoToFile("z:/t.txt");;
    QVERIFY2( QxPack::IcMemCntr::currNewCntr() == mem_cntr, "IcTimer::testCall() exist memory leak!" );

}

// ============================================================================
//  test the call queue
// ============================================================================
void   TestJoTaskWorker :: testPostAndNoWaitForDone()
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();

    {
        QxPack::IcJoTaskWorker *worker = new QxPack::IcJoTaskWorker(2);

        worker->post(
           & gNormalTask, Q_NULLPTR, QJsonDocument::fromJson(QByteArray("{\"name\":\"task1\"}")).object()
        );
        worker->post(
           & gShortTask, Q_NULLPTR, QJsonDocument::fromJson(QByteArray("{\"name\":\"task2\"}")).object()
        );
        worker->post(
           & gLongTask, Q_NULLPTR, QJsonDocument::fromJson(QByteArray("{\"name\":\"task3\"}")).object()
        );
        worker->post(
           & gShortTask, Q_NULLPTR, QJsonDocument::fromJson(QByteArray("{\"name\":\"task4\"}")).object()
        );
        worker->post(
           & gNormalTask, Q_NULLPTR, QJsonDocument::fromJson(QByteArray("{\"name\":\"task5\"}")).object()
        );
        worker->deleteLater();

        QObject::connect(
            worker, & QxPack::IcJoTaskWorker::destroyed, [](){ QCoreApplication::quit(); }
        );
    }
    QxPack::IcAppTotPriv::setTot(10000);
    QCoreApplication::exec();
    QxPack::IcAppDclPriv::barrier();

    QVERIFY2( QxPack::IcMemCntr::currNewCntr() == mem_cntr, "IcTimer::testCall() exist memory leak!" );
}





QTEST_MAIN( TestJoTaskWorker )
#include "main.moc"
