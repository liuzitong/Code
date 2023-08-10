#include <QCoreApplication>
#include <qDebug>
#include <cstdlib>
#include <QObject>
#include <QtTest>
#include <string>
#include <QThread>
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/sys/qxpack_ic_eventloopbarrier.hxx"
#include "qxpack/indcom/sys/qxpack_ic_appdcl_priv.hxx"
#include <QMetaObject>
#include <QVariant>

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
Q_SLOT  void testBarrier( );
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
// test barrier
// ============================================================================
void   Test :: testBarrier()
{
   int mem_cntr = QxPack::IcMemCntr::currNewCntr();
   {
       // shed. a object delete scenario
       QObject *obj_a = new QObject();
       QObject *obj_b = new QObject();

       QObject::connect(
           obj_a, & QObject::destroyed,
           [](QObject*){
               qInfo() << "obj a deleted";
           }
       );
       QObject::connect(
           obj_b, & QObject::destroyed,
           [](QObject*){ qInfo() << "obj b deleted"; }
       );

       obj_a->deleteLater();
       QMetaObject::invokeMethod( QCoreApplication::instance(), "quit", Qt::QueuedConnection );
       QCoreApplication::instance()->exec();
       obj_b->deleteLater();
       QxPack::IcAppDclPriv::barrier();
   }

   QVERIFY2( mem_cntr == QxPack::IcMemCntr::currNewCntr(), "current memory counter is not equal" );
}




QTEST_MAIN( Test )
#include "main.moc"
