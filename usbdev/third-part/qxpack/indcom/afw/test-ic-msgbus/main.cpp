#include <QCoreApplication>
#include <QDebug>
#include <cstdlib>
#include <QObject>
#include <QtTest>
#include <string>
#include <QThread>
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/afw/qxpack_ic_msgbus.hxx"
#include "qxpack/indcom/sys/qxpack_ic_apptot_priv.hxx"
#include "qxpack/indcom/sys/qxpack_ic_appdcl_priv.hxx"
#include "qxpack/indcom/sys/qxpack_ic_sysevtqthread_priv.hxx"

#include <QMetaObject>
#include <QEventLoop>
#include <QElapsedTimer>

// ////////////////////////////////////////////////////////////////////////////
//
// msg receiver
//
// ////////////////////////////////////////////////////////////////////////////
class  MsgRecv : public QObject {
    Q_OBJECT
public:
    MsgRecv( ) {  }

    Q_SLOT void  IcMsgBus_onMsg( const QxPack::IcMsgBusPkg &pkg )
    {
        emit msgRecv( pkg.message() );
    }

    Q_SIGNAL void msgRecv( const QString &msg );
};

namespace MyNameSpec {

class  MsgRecv2 : public QObject {
    Q_OBJECT
public:
    MsgRecv2( ) {  }

    Q_SLOT void  IcMsgBus_onMsg( const QxPack::IcMsgBusPkg &pkg )
    {
        emit msgRecv( pkg.message() );
    }
    Q_SIGNAL void msgRecv( const QString &msg );
};


}

// ////////////////////////////////////////////////////////////////////////////
//
// test object
//
// ////////////////////////////////////////////////////////////////////////////
class  Test : public QObject {
    Q_OBJECT
private:
Q_SLOT  void initTestCase( ) {  }
Q_SLOT  void cleanupTestCase( );
Q_SLOT  void add ( );
Q_SLOT  void msg ( );
Q_SLOT  void msg_SameGroup( );
Q_SLOT  void msg_DiffGroup( );

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
// test no action thread, post call
// ============================================================================
void   Test :: add()
{
   int mem_cntr = QxPack::IcMemCntr::currNewCntr();
   //QxPack::IcMemCntr::enableMemTrace(true);
   {
       // add item into group
       QxPack::IcMsgBus *msg_bus = new QxPack::IcMsgBus;
       MsgRecv *msg_recv = new MsgRecv;
       bool is_added = msg_bus->add( msg_recv, "G0" );
       if ( ! is_added ) {
           QVERIFY2( false, "can not add the member into message bus" );
           return;
       }

       // delete object
       msg_bus->deleteLater();

       // create wait timer, and enter in main loop
       QxPack::IcAppTotPriv app_tot( 10000, 64,
           []( void *, QxPack::IcAppTotPriv &tot, bool is_limit ) {
               if ( is_limit ) {
                   qInfo() << "TimeOut! quit MainEventLoop!";
                   QCoreApplication::quit();
               } else {
                   if ( ! QxPack::IcSysEvtQThreadPriv::isAlive()) {
                       tot.stopTimer();
                       QCoreApplication::quit();
                   }
               }
           }, nullptr
       );
       QCoreApplication::instance()->exec();

       delete msg_recv;
   }
   QxPack::IcAppDclPriv::barrier(16);
   //QxPack::IcMemCntr::saveTraceInfoToFile("z:/t.txt");
   QVERIFY2( mem_cntr == QxPack::IcMemCntr::currNewCntr(), "current memory counter is not equal" );
}

// ============================================================================
// message
// ============================================================================
void   Test :: msg()
{
   int mem_cntr = QxPack::IcMemCntr::currNewCntr();
   {
       QElapsedTimer *e_tmr = new QElapsedTimer;

       // add item into group
       QxPack::IcMsgBus *msg_bus = new QxPack::IcMsgBus;
       MsgRecv *msg_recv = new MsgRecv;
       bool is_added = msg_bus->add( msg_recv, "G0" );
       if ( ! is_added ) {
           QVERIFY2( false, "can not add the member into message bus" );
           return;
       }

       // do the message
       QObject::connect(
           msg_recv, & MsgRecv::msgRecv,
           [ msg_bus, e_tmr ]( const QString &msg ) {
               qInfo() << "recv:" << msg << " time cost:" << e_tmr->elapsed();
               msg_bus->deleteLater();
           }
       );

       e_tmr->start();

       QxPack::IcMsgBusPkg  pkg;
       pkg.setGroupName("G0");
       pkg.setMessage("show me the money.");
       msg_bus->post( pkg );

       // create wait timer, and enter in main loop
       QxPack::IcAppTotPriv app_tot(
           10000, 64,
           []( void *, QxPack::IcAppTotPriv &tot, bool is_limit ) {
               if ( is_limit ) {
                   qInfo() << "TimeOut! quit MainEventLoop!";
                   QCoreApplication::quit();
               } else {
                   if ( ! QxPack::IcSysEvtQThreadPriv::isAlive()) {
                       tot.stopTimer();
                       QCoreApplication::quit();
                   }
               }
           }, nullptr
       );
       QCoreApplication::instance()->exec();

       delete msg_recv;
       delete e_tmr;
   }

   QVERIFY2( mem_cntr == QxPack::IcMemCntr::currNewCntr(), "current memory counter is not equal" );
}

// ============================================================================
// message same group test
// ============================================================================
void   Test :: msg_SameGroup()
{
   int mem_cntr = QxPack::IcMemCntr::currNewCntr();
   {
       QElapsedTimer *e_tmr = new QElapsedTimer;
       int recv_cntr = 0;

       // add item into group
       QxPack::IcMsgBus *msg_bus = new QxPack::IcMsgBus;
       MsgRecv *msg_recv = new MsgRecv;
       MsgRecv *msg_recv2= new MsgRecv;

       bool is_added = msg_bus->add( msg_recv, "G0" );
       if ( ! is_added ) {
           QVERIFY2( false, "can not add the first member into message bus" );
           return;
       }
       is_added = msg_bus->add( msg_recv2, "G0");
       if ( ! is_added ) {
           QVERIFY2( false, "can not add the sec. member into message bus" );
           return;
       }

       // do the message
       QObject::connect(
           msg_recv, & MsgRecv::msgRecv,
           [ msg_bus, e_tmr, &recv_cntr ]( const QString &msg ) {
               qInfo() << "recv:" << msg << " time cost:" << e_tmr->elapsed();
               if ( ++ recv_cntr >= 2 ) {
                   msg_bus->deleteLater();
               }
           }
       );
       QObject::connect(
           msg_recv2, & MsgRecv::msgRecv,
           [ msg_bus, e_tmr, &recv_cntr ]( const QString &msg ) {
               qInfo() << "recv:" << msg << " time cost:" << e_tmr->elapsed();
               if ( ++ recv_cntr >= 2 ) {
                   msg_bus->deleteLater();
               }
           }
       );

       // post a data package
       e_tmr->start();
       QxPack::IcMsgBusPkg  pkg;
       pkg.setGroupName("G0");
       pkg.setMessage("show me the money.");
       msg_bus->post( pkg );

       // create wait timer, and enter in main loop
       QxPack::IcAppTotPriv app_tot(
           10000, 64,
           []( void *, QxPack::IcAppTotPriv &tot, bool is_limit ) {
               if ( is_limit ) {
                   qInfo() << "TimeOut! quit MainEventLoop!";
                   QCoreApplication::quit();
               } else {
                   if ( ! QxPack::IcSysEvtQThreadPriv::isAlive()) {
                       tot.stopTimer();
                       QCoreApplication::quit();
                   }
               }
           }, nullptr
       );
       QCoreApplication::instance()->exec();

       delete msg_recv;
       delete msg_recv2;
       delete e_tmr;
   }

   QVERIFY2( mem_cntr == QxPack::IcMemCntr::currNewCntr(), "current memory counter is not equal" );
}

// ============================================================================
// message diff. group test
// ============================================================================
void   Test :: msg_DiffGroup()
{
   int mem_cntr = QxPack::IcMemCntr::currNewCntr();
   {
       QElapsedTimer *e_tmr = new QElapsedTimer;
       int recv_cntr = 0;

       // add item into group
       QxPack::IcMsgBus *msg_bus = new QxPack::IcMsgBus;
       MsgRecv *msg_recv = new MsgRecv;
       MsgRecv *msg_recv2= new MsgRecv;

       bool is_added = msg_bus->add( msg_recv, "G0" );
       if ( ! is_added ) {
           QVERIFY2( false, "can not add the first member into message bus" );
           return;
       }
       is_added = msg_bus->add( msg_recv2, "G1");
       if ( ! is_added ) {
           QVERIFY2( false, "can not add the sec. member into message bus" );
           return;
       }

       // do the message
       QObject::connect(
           msg_recv, & MsgRecv::msgRecv,
           [ msg_bus, e_tmr, &recv_cntr ]( const QString &msg ) {
               qInfo() << "recv:" << msg << " time cost:" << e_tmr->elapsed();
               if ( ++ recv_cntr == 2 ) {
                   msg_bus->deleteLater();
               }
           }
       );
       QObject::connect(
           msg_recv2, & MsgRecv::msgRecv,
           [ msg_bus, e_tmr, &recv_cntr ]( const QString &msg ) {
               qInfo() << "recv:" << msg << " time cost:" << e_tmr->elapsed();
               if ( ++ recv_cntr == 2 ) {
                   msg_bus->deleteLater();
               }
           }
       );

       // post a data package
       e_tmr->start();
       QxPack::IcMsgBusPkg  pkg;
       pkg.setGroupName("G0");
       pkg.setMessage("show me the money.");
       msg_bus->post( pkg );
       pkg.setMessage("hello, world.");
       msg_bus->post( pkg );

       // create wait timer, and enter in main loop
       QxPack::IcAppTotPriv app_tot(
           10000, 64,
           []( void *, QxPack::IcAppTotPriv &tot, bool is_limit ) {
               if ( is_limit ) {
                   qInfo() << "TimeOut! quit MainEventLoop!";
                   QCoreApplication::quit();
               } else {
                   if ( ! QxPack::IcSysEvtQThreadPriv::isAlive()) {
                       tot.stopTimer();
                       QCoreApplication::quit();
                   }
               }
           }, nullptr
       );
       QCoreApplication::instance()->exec();

       delete msg_recv;
       delete msg_recv2;
       delete e_tmr;
   }

   QVERIFY2( mem_cntr == QxPack::IcMemCntr::currNewCntr(), "current memory counter is not equal" );
}


QTEST_MAIN( Test )
#include "main.moc"
