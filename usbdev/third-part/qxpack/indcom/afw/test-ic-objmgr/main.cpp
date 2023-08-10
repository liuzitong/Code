#include <QCoreApplication>
#include <qDebug>
#include <cstdlib>
#include <QObject>
#include <QtTest>
#include <string>
#include <QThread>
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/afw/qxpack_ic_msgbus.hxx"
#include "qxpack/indcom/afw/qxpack_ic_objmgr.hxx"
#include "qxpack/indcom/sys/qxpack_ic_apptot_priv.hxx"
#include "qxpack/indcom/sys/qxpack_ic_sysevtqthread_priv.hxx"
#include "qxpack/indcom/sys/qxpack_ic_appdcl_priv.hxx"

#include <QMetaMethod>
#include <QMetaObject>
#include <QEventLoop>
#include <QElapsedTimer>

// ////////////////////////////////////////////////////////////////////////////
// delay action
// ////////////////////////////////////////////////////////////////////////////
class  DelayAction : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY( DelayAction )
public:
    typedef void (* ActFunc )( void *info );
    DelayAction ( ActFunc func, void *info_obj )
    { m_func = func; m_info = info_obj; }
    virtual ~DelayAction( ) Q_DECL_OVERRIDE { }

    Q_INVOKABLE void doAction( ) {
        if ( m_func != Q_NULLPTR ) { m_func( m_info ); }
    }
private:
    ActFunc  m_func; void *m_info;
};


// ////////////////////////////////////////////////////////////////////////////
// simple objec
// ////////////////////////////////////////////////////////////////////////////
class  SimpleObj : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY( SimpleObj )
public :
    Q_INVOKABLE explicit SimpleObj( const QVariantList & ) : QObject( Q_NULLPTR )
    {
        qInfo("[ SimpleObj ] created in QThread 0x%x", QThread::currentThread() );
        emit created();
    }
    virtual ~SimpleObj( ) Q_DECL_OVERRIDE
    {
        qInfo("[ SimpleObj ] deleted in QThread 0x%x", QThread::currentThread() );
        emit deleted();
    }

    Q_INVOKABLE void func() { }

    Q_SIGNAL void  created( );
    Q_SIGNAL void  deleted( );
};

// ////////////////////////////////////////////////////////////////////////////
//
// test object
//
// ////////////////////////////////////////////////////////////////////////////
class  Test : public QObject {
    Q_OBJECT
private:
Q_SLOT  void initTestCase( ) { qInfo("Caller thread is 0x%x", QThread::currentThread() ); }
Q_SLOT  void cleanupTestCase( );
Q_SLOT  void createObjInCallerThread( );
Q_SLOT  void createObjInRemoteThread( );
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
// test create module
// ============================================================================
void   Test :: createObjInCallerThread()
{
   int mem_cntr = QxPack::IcMemCntr::currNewCntr();
   {
       QxPack::IcObjMgr  obj_mgr;

       // register a module create method
       bool is_reg = obj_mgr.registerType<SimpleObj>(
           QStringLiteral("SimpleObj"), Q_NULLPTR
       );
       if ( ! is_reg ) {
           QVERIFY2( false, "can not register object." );
           return;
       }

       // attach a object in manager
       QObject *mod = obj_mgr.attachObj( QStringLiteral("SimpleObj"), true );
       if ( mod == Q_NULLPTR ) {
           QVERIFY2( false, "attach to object failed.");
           return;
       }

       // create a delay call
       DelayAction  *delay_act = new DelayAction(
           []( void *m ) {
               QxPack::IcObjMgr *mgr = static_cast<QxPack::IcObjMgr*>( m );
               mgr->detachObj( QStringLiteral("SimpleObj"), Q_NULLPTR );
               QCoreApplication::quit();
           }, & obj_mgr
       );

       QMetaObject::invokeMethod ( delay_act, "doAction", Qt::QueuedConnection );
       delay_act->deleteLater();

       // create wait timer, and enter in main loop
       QxPack::IcAppTotPriv( 10000, 64,
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

   }
   QxPack::IcAppDclPriv::barrier();

   QVERIFY2( mem_cntr == QxPack::IcMemCntr::currNewCntr(), "current memory counter is not equal" );
}

// ==================================================
// test create module
// ==================================================
void   Test :: createObjInRemoteThread()
{
   int mem_cntr = QxPack::IcMemCntr::currNewCntr();
   QThread  rmt_thread; rmt_thread.start();
   {
       QxPack::IcObjMgr  obj_mgr;

       // register a object create method
       bool is_reg = obj_mgr.registerType<SimpleObj>(
           QStringLiteral("SimpleObj"), &rmt_thread
       );
       if ( ! is_reg ) {
           QVERIFY2( false, "can not register object." );
           return;
       }

       // attach a object in manager
       QObject *mod = obj_mgr.attachObj( QStringLiteral("SimpleObj"), true );
       if ( mod == Q_NULLPTR ) {
           QVERIFY2( false, "attach to object failed.");
           return;
       }

       // create a delay call
       DelayAction  *delay_act = new DelayAction(
           []( void *m ) {
               QxPack::IcObjMgr *mgr = static_cast<QxPack::IcObjMgr*>( m );
               mgr->detachObj( QStringLiteral("SimpleObj"), Q_NULLPTR );
               QCoreApplication::quit();
           }, & obj_mgr
       );

       QMetaObject::invokeMethod(
           delay_act, "doAction", Qt::QueuedConnection
       );
       delay_act->deleteLater();

       // create wait timer, and enter in main loop
       QxPack::IcAppTotPriv( 10000, 64,
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
   }
   rmt_thread.quit();
   rmt_thread.wait();
   QxPack::IcAppDclPriv::barrier();

   QVERIFY2( mem_cntr == QxPack::IcMemCntr::currNewCntr(), "current memory counter is not equal" );
}




QTEST_MAIN( Test )
#include "main.moc"
