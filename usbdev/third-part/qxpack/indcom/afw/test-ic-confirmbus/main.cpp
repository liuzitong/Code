#include <QCoreApplication>
#include <QDebug>
#include <QObject>
#include <QtTest>
#include <QThread>
#include <QMetaObject>
#include <QEventLoop>
#include <QElapsedTimer>

#include <cstdlib>
#include <string>
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/afw/qxpack_ic_confirmbus.hxx"
#include "qxpack/indcom/sys/qxpack_ic_apptot_priv.hxx"
#include "qxpack/indcom/sys/qxpack_ic_appdcl_priv.hxx"
#include "qxpack/indcom/sys/qxpack_ic_sysevtqthread_priv.hxx"
#include "qxpack/indcom/sys/qxpack_ic_rmtobjcreator_priv.hxx"

// ////////////////////////////////////////////////////////////////////////////
//                    Confirm receiver
// ////////////////////////////////////////////////////////////////////////////
class  ConfirmRecv : public QObject {
    Q_OBJECT
private:  int  m_id;
public:
    ConfirmRecv( int id ) { m_id = id;  }

    Q_SLOT  void  IcConfirmBus_onReqConfirm( QxPack::IcConfirmBusPkg &pkg )
    { emit newConfirm( m_id, pkg ); }

    Q_SIGNAL void  newConfirm( int id, QxPack::IcConfirmBusPkg pkg );
};

// ////////////////////////////////////////////////////////////////////////////
//                     message emitter
// ////////////////////////////////////////////////////////////////////////////
class   MsgEmit : public QObject {
    Q_OBJECT
public :
    MsgEmit( ) {
        QObject::connect( QThread::currentThread(), SIGNAL(finished()), this, SLOT(deleteLater()));
    }
    virtual ~MsgEmit() {

    }
    Q_SLOT void  doEmit( void *cfm, const QxPack::IcConfirmBusPkg &pkg )
    {
        QxPack::IcConfirmBusPkg c_pkg = pkg;
        QxPack::IcConfirmBus *cfm_bus = reinterpret_cast<QxPack::IcConfirmBus*>( cfm );
        cfm_bus->reqConfirm( c_pkg );
        if ( ! pkg.isGotResult() ) { qFatal("get result failed."); }

        QxPack::IcRmtObjCreator::createObjInThread(
            QCoreApplication::instance()->thread(),
            [](void*)->QObject* {
                QCoreApplication::instance()->quit();
                return Q_NULLPTR;
            }, Q_NULLPTR, false
        );
    }
};


// ////////////////////////////////////////////////////////////////////////////
//   test object
// ////////////////////////////////////////////////////////////////////////////
class  Test : public QObject {
    Q_OBJECT
private:
Q_SLOT  void initTestCase( ) { }
Q_SLOT  void cleanupTestCase( );
Q_SLOT  void confirm_SameGroupAllCalled( );
Q_SLOT  void confirm_SameGroupFirstCallOnly( );
Q_SLOT  void confirm_BgThreadCall();
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
// confirm same group test
// ============================================================================
void   Test :: confirm_SameGroupAllCalled()
{
   int mem_cntr = QxPack::IcMemCntr::currNewCntr();
   qInfo("curr mem cntr: %d", mem_cntr );
   {
       QElapsedTimer *e_tmr = new QElapsedTimer;
       int  rcv_cntr = 0;

       // ---------------------------------------------------------------------
       // add item into group
       // ---------------------------------------------------------------------
       QxPack::IcConfirmBus *cfm_bus = new QxPack::IcConfirmBus;
       ConfirmRecv *cfm_recv1 = new ConfirmRecv( 1 );
       ConfirmRecv *cfm_recv2 = new ConfirmRecv( 2 );
       if ( ! cfm_bus->add( cfm_recv1, "G0" )) { qFatal("can not add the first member into confirm bus" );}
       if ( ! cfm_bus->add( cfm_recv2, "G0" )) { qFatal( "can not add the second member into confirm bus" );}

       // ---------------------------------------------------------------------
       // do the confirm
       // ---------------------------------------------------------------------
       QObject::connect (
           cfm_recv1, & ConfirmRecv::newConfirm,
           [ &rcv_cntr, e_tmr ]( int id, QxPack::IcConfirmBusPkg pkg ) {
               qInfo() << id << "recv:" << pkg.message() << " time cost:" << e_tmr->elapsed();
               if ( ++ rcv_cntr >= 2 ) { QCoreApplication::quit(); }
           }
       );
       QObject::connect(
           cfm_recv2, & ConfirmRecv::newConfirm,
           [ &rcv_cntr, e_tmr ]( int id, QxPack::IcConfirmBusPkg pkg ) {
               qInfo() << id << "recv:" << pkg.message() << " time cost:" << e_tmr->elapsed();
               if ( ++ rcv_cntr >= 2 ) { QCoreApplication::quit(); }
           }
       );

       // ---------------------------------------------------------------------
       // post a data package
       // ---------------------------------------------------------------------
       e_tmr->start();
       QxPack::IcConfirmBusPkg  pkg;
       pkg.setGroupName("G0");
       pkg.setMessage("can your show me the money?");

       // NOTE: here do a queued call, it is will be execute while event execute
       bool c_ret = QMetaObject::invokeMethod(
           cfm_bus, "reqConfirm", Qt::QueuedConnection,  // warning: Although here use QueuedConnection,
           Q_ARG( QxPack::IcConfirmBusPkg&, pkg )        // the 'pkg' parameter is like a global variable.
       );                                                // It is safely here.
       if ( ! c_ret ) { qFatal("call reqConfirm() failed!"); }

       // create wait timer, and enter in main loop
       QxPack::IcAppTotPriv app_tot(
           10000, 64,
           []( void *, QxPack::IcAppTotPriv &, bool is_limit ) {
               if ( is_limit ) {
                   qInfo() << "TimeOut! quit MainEventLoop!";
                   QCoreApplication::quit();
               }
           }, nullptr
       );
       QCoreApplication::instance()->exec();

       cfm_bus->rmv( cfm_recv1, "G0" );
       cfm_bus->rmv( cfm_recv2, "G0" );
       delete cfm_recv1;
       delete cfm_recv2;
       delete e_tmr;
       delete cfm_bus;
   }
   QxPack::IcAppDclPriv::barrier();

   QVERIFY2( mem_cntr == QxPack::IcMemCntr::currNewCntr(), "current memory counter is not equal" );
}

// ============================================================================
// group test, only first member can recv. the confirm
// ============================================================================
void   Test :: confirm_SameGroupFirstCallOnly()
{
   int mem_cntr = QxPack::IcMemCntr::currNewCntr();
   qInfo("curr mem cntr: %d", mem_cntr );
   {
       QElapsedTimer *e_tmr = new QElapsedTimer;

       // ---------------------------------------------------------------------
       // add item into group
       // ---------------------------------------------------------------------
       QxPack::IcConfirmBus *cfm_bus = new QxPack::IcConfirmBus;
       ConfirmRecv *cfm_recv1 = new ConfirmRecv(1);
       ConfirmRecv *cfm_recv2 = new ConfirmRecv(2);
       if ( ! cfm_bus->add( cfm_recv1, "G0" )) { qFatal("can not add the first member into confirm bus" ); }
       if ( ! cfm_bus->add( cfm_recv2, "G0" )) { qFatal("can not add the second. member into confirm bus" );}

       // ---------------------------------------------------------------------
       // recv. the message to handle
       // ---------------------------------------------------------------------
       QObject::connect ( // this recv is first in order.
           cfm_recv2, & ConfirmRecv::newConfirm,
           [ e_tmr ]( int id, QxPack::IcConfirmBusPkg pkg ) {
               qInfo() << id << "recv:" << pkg.message() << " time cost:" << e_tmr->elapsed();
               pkg.setResult(QJsonObject()); // set a empty result to stop continue walk to next recv.
               QCoreApplication::quit();
           }
       );
       QObject::connect (
           cfm_recv1, & ConfirmRecv::newConfirm,
           [ e_tmr ]( int id, QxPack::IcConfirmBusPkg pkg ) {
               qInfo() << id << "recv:" << pkg.message() << " time cost:" << e_tmr->elapsed();
               QVERIFY2( false, "the recv1 is at last position in confirm list, should not recv. confirm.");
           }
       );

       // ---------------------------------------------------------------------
       // post a data package
       // ---------------------------------------------------------------------
       e_tmr->start();
       QxPack::IcConfirmBusPkg  pkg;
       pkg.setGroupName("G0");
       pkg.setMessage("show me the money ?");

       // NOTE: here do a queued call, it is will be execute while event execute
       QMetaObject::invokeMethod(
           cfm_bus, "reqConfirm", Qt::QueuedConnection,  // warning: Although here use QueuedConnection,
           Q_ARG( QxPack::IcConfirmBusPkg&, pkg )        // the 'pkg' parameter is like a global variable.
       );                                                // It is safely here.

       // create wait timer, and enter in main loop
       QxPack::IcAppTotPriv app_tot(
           10000, 64,
           []( void *, QxPack::IcAppTotPriv &, bool is_limit ) {
               if ( is_limit ) {
                   qInfo() << "TimeOut! quit MainEventLoop!";
                   QCoreApplication::quit();
               }
           }, nullptr
       );
       QCoreApplication::instance()->exec();

       cfm_bus->rmv( cfm_recv1, "G0" );
       cfm_bus->rmv( cfm_recv2, "G0" );
       delete cfm_recv1;
       delete cfm_recv2;
       delete e_tmr;
       delete cfm_bus;
   }
   QxPack::IcAppDclPriv::barrier();

   QVERIFY2( mem_cntr == QxPack::IcMemCntr::currNewCntr(), "current memory counter is not equal" );
}

// ============================================================================
// test background thread call confirm.
// ============================================================================
void                    Test :: confirm_BgThreadCall()
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    qInfo("curr mem cntr: %d", mem_cntr );
    {
        QElapsedTimer *e_tmr = new QElapsedTimer;

       // ---------------------------------------------------------------------
       // add item into group
       // ---------------------------------------------------------------------
       QxPack::IcConfirmBus *cfm_bus = new QxPack::IcConfirmBus;
       ConfirmRecv *cfm_recv1 = new ConfirmRecv(1);
       if ( ! cfm_bus->add( cfm_recv1, "G0" )) { qFatal("can not add the first member into confirm bus" ); }

       // ---------------------------------------------------------------------
       // recv. the message to handle
       // ---------------------------------------------------------------------
       QObject::connect ( // this recv is first in order.
           cfm_recv1, & ConfirmRecv::newConfirm,
           [ e_tmr ]( int id, QxPack::IcConfirmBusPkg pkg ) {
               qInfo() << id << "recv:" << pkg.message() << " time cost:" << e_tmr->elapsed();
               pkg.setResult(QJsonObject()); // set a empty result to stop continue walk to next recv.
           }
       );

       // ---------------------------------------------------------------------
       // create a thread to do bg confirm call
       // ---------------------------------------------------------------------
       QThread  thr; thr.start();
       QObject *emitter = QxPack::IcRmtObjCreator::createObjInThread(
           &thr, [](void*)->QObject* { return new MsgEmit; }, Q_NULLPTR, false
       );

       QxPack::IcConfirmBusPkg  pkg;
       pkg.setGroupName("G0");
       pkg.setMessage("show me the money ?");

       e_tmr->start();
       bool ret = QMetaObject::invokeMethod(
           emitter, "doEmit", Qt::QueuedConnection,
           Q_ARG( void*, cfm_bus ), Q_ARG( const QxPack::IcConfirmBusPkg, pkg )
       );
       if ( ! ret ) { qFatal("call doEmit() failed."); }

       // create wait timer, and enter in main loop
       QxPack::IcAppTotPriv app_tot(
           10000, 64,
           []( void *, QxPack::IcAppTotPriv &, bool is_limit ) {
               if ( is_limit ) {
                   qInfo() << "TimeOut! quit MainEventLoop!";
                   QCoreApplication::quit();
               }
           }, nullptr
       );
       QCoreApplication::instance()->exec();

       thr.quit();
       thr.wait();
       cfm_bus->rmv( cfm_recv1, "G0" );
       delete cfm_recv1;
       delete e_tmr;
       delete cfm_bus;
    }
    QxPack::IcAppDclPriv::barrier();

    QVERIFY2( mem_cntr == QxPack::IcMemCntr::currNewCntr(), "current memory counter is not equal" );
}




QTEST_MAIN( Test )
#include "main.moc"
