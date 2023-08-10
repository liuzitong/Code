#include <QCoreApplication>
#include <QDebug>
#include <cstdlib>
#include <QObject>
#include <QtTest>
#include <QThread>
#include <QMap>
#include <QImage>
#include <QTimer>


#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/sys/qxpack_ic_usbstormon.hxx"
#include "qxpack/indcom/sys/qxpack_ic_ncstring.hxx"
#include "qxpack/indcom/sys/qxpack_ic_apptot_priv.hxx"
#include "qxpack/indcom/sys/qxpack_ic_sysevtqthread_priv.hxx"
#include "qxpack/indcom/sys/qxpack_ic_appdcl_priv.hxx"


// ////////////////////////////////////////////////////////////////////////////
// static functions
// ////////////////////////////////////////////////////////////////////////////




// ////////////////////////////////////////////////////////////////////////////
//
// test object
//
// ////////////////////////////////////////////////////////////////////////////
class  Test : public QObject {
    Q_OBJECT
private:
Q_SLOT  void initTestCase( ) { }
Q_SLOT  void cleanupTestCase( );
Q_SLOT  void testUsbStorMon( );

public :
    Test ( ) { }
    ~Test( ) { }
};

// ============================================================================
// final cleanup
// ============================================================================
void  Test :: cleanupTestCase()
{
    qInfo() << "current MemCntr:" << QxPack::IcMemCntr::currNewCntr();
}

// ============================================================================
// test first scan the file.
// ============================================================================
static void  gListUsbStorInfo( QxPack::IcUsbStorMon &mon )
{
    qInfo() << "";
    qInfo() << "List USB information:";
    QList<QxPack::IcUsbStorInfo> lst = mon.infoList();
    auto itr = lst.begin();
    while ( itr != lst.end()) {
        auto &info = *( itr ++ );
        qInfo() << "DISK :" << info.label();
        qInfo() << "  root path:" << info.rootPath();
        qInfo() << "  capacity :" << info.capacity();
        qInfo() << "  free spc :" << info.freeSpace();
    }
}

void   Test :: testUsbStorMon()
{
   int mem_cntr = QxPack::IcMemCntr::currNewCntr();
   //QxPack::IcMemCntr::enableMemTrace( true );

   {
       QxPack::IcUsbStorMon mon;

       QObject::connect(
           & mon, & QxPack::IcUsbStorMon::listChanged,
           [&mon](){ gListUsbStorInfo( mon ); }
       );

       // create wait timer, and enter in main loop
//       QxPack::IcAppTotPriv app_tot( 10000, 64,
//           []( void *, QxPack::IcAppTotPriv &tot, bool is_limit ) {
//               if ( is_limit ) {
//                   qInfo() << "TimeOut! quit MainEventLoop!";
//                   QCoreApplication::quit();
//               } else {
//                   if ( ! QxPack::IcSysEvtQThreadPriv::isAlive()) {
//                       tot.stopTimer();
//                       QCoreApplication::quit();
//                   }
//               }
//           }, nullptr
//       );
     //  QPushButton *btn = new QPushButton();
     //  btn->show();
       QTimer *tmr = new QTimer;
       tmr->setInterval(10000);
       QObject::connect(
           tmr, & QTimer::timeout,
           [&mon]() {
              mon.ejectVol("E:/");
           }
       );
       tmr->start();
       QCoreApplication::instance()->exec();
     //  delete btn;
    }
   QxPack::IcAppDclPriv::barrier();

   //QxPack::IcMemCntr::saveTraceInfoToFile("z:/mem_cntr.txt" );
//   QxPack::IcMemCntr::enableMemTrace(false);
//   qInfo() << "current counter is:" << QxPack::IcMemCntr::currNewCntr();
   QVERIFY2( mem_cntr == QxPack::IcMemCntr::currNewCntr(), "current memory counter is not equal" );
}




QTEST_MAIN( Test )
#include "main.moc"
