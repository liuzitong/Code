#include <QCoreApplication>
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include <QDebug>
#include <cstdlib>
#include <QObject>
#include <QtTest>
#include <string>
#include "qxpack/indcom/common/qxpack_ic_global.hxx"
#include "qxpack/indcom/common/qxpack_ic_threadpool.hxx"
#include <QString>
#include <QElapsedTimer>
#include <QTime>
#include <QSharedPointer>
#include <QThread>
#include <QtGlobal>

// ////////////////////////////////////////////////////////////////////////////
//
// test object
//
// ////////////////////////////////////////////////////////////////////////////
class  Test : public QObject {
    Q_OBJECT
private:
Q_SLOT  void  initTestCase( ) {  }
Q_SLOT  void  cleanupTestCase ( );
Q_SLOT  void  test_ctor ();
Q_SLOT  void  test_oneRo();
Q_SLOT  void  test_twoRo();
public :
    Test ( ) { }
    ~Test( ) { }
};

// ============================================================================
// final cleanup
// ============================================================================
void  Test :: cleanupTestCase()
{
    qInfo() << "current MemCntr:" <<
            QxPack::IcMemCntr::currNewCntr();
}

// ============================================================================
// test the ctor/dtor
// ============================================================================
void  Test :: test_ctor()
{
    //QxPack::IcMemCntr::enableMemTrace(true);
    int mem_cntr =  QxPack::IcMemCntr::currNewCntr();
    {
        QxPack::IcThreadPool thr_pool;
    }
    QxPack::IcMemCntr::saveTraceInfoToFile( "z:\\t.txt");
    int curr_cntr = QxPack::IcMemCntr::currNewCntr();
    QVERIFY2( mem_cntr == curr_cntr, "memory leak!");
}

// ============================================================================
// test one run object
// ============================================================================
void  Test :: test_oneRo()
{
    class RunObj : public QxPack::IcRunnable {
    private:
    public :
        explicit RunObj ( DelCallback cb, void *ctxt ) : QxPack::IcRunnable( cb, ctxt )
        { }
        virtual ~RunObj( ) override { }
        void  run( ) override
        {
            qInfo("runobj run:0x%x", this );
            QThread::msleep(1000);
        }

        static RunObj * newRunObj( ) { return qxpack_ic_new( RunObj, &deleteRunObj, nullptr ); }
        static void     deleteRunObj( QxPack::IcRunnable *ro, void* ) { qxpack_ic_delete( ro, QxPack::IcRunnable ); }
    };

    int mem_cntr =  QxPack::IcMemCntr::currNewCntr();
   // QxPack::IcMemCntr::enableMemTrace(true);
    {
        QxPack::IcThreadPool thr_pool;
        RunObj *ro_1 = RunObj::newRunObj();
        ro_1->setAutoDelete( true );
        thr_pool.start( ro_1 );
        thr_pool.waitForDone(-1);
    }
    QxPack::IcMemCntr::saveTraceInfoToFile( "z:\\t.txt");
    int curr_cntr = QxPack::IcMemCntr::currNewCntr();
    QVERIFY2( mem_cntr == curr_cntr, "memory leak!");
}

// ============================================================================
// test two run object
// ============================================================================
void  Test :: test_twoRo()
{
    class RunObj : public QxPack::IcRunnable {
    private:
    public :
        explicit RunObj ( DelCallback cb, void *ctxt ) : QxPack::IcRunnable( cb, ctxt )
        { }
        virtual ~RunObj( ) override { }
        void  run( ) override
        {
            QxPack::IcLCG lcg;
            int msleep = 3000 + ( lcg.value() % 2000 );
            qInfo("runobj run:0x%x, sleep:%d", this, msleep );
            QThread::msleep( uint( msleep ));
        }

        static RunObj * newRunObj( ) { return qxpack_ic_new( RunObj, &deleteRunObj, nullptr ); }
        static void     deleteRunObj( QxPack::IcRunnable *ro, void* ) { qxpack_ic_delete( ro, QxPack::IcRunnable ); }
    };

    int mem_cntr =  QxPack::IcMemCntr::currNewCntr();
    {
        QxPack::IcThreadPool thr_pool;
        RunObj *ro_1 = RunObj::newRunObj();
        RunObj *ro_2 = RunObj::newRunObj();
        thr_pool.start( ro_1 );
        thr_pool.start( ro_2 );
        thr_pool.waitForDone(-1);
        QThread::msleep(4000);
    }
    int curr_cntr = QxPack::IcMemCntr::currNewCntr();
    QVERIFY2( mem_cntr == curr_cntr, "memory leak!");
}



QTEST_MAIN( Test )
#include "main.moc"
