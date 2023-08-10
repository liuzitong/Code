#include <QCoreApplication>
#include <qxpack/indcom/common/qxpack_ic_memcntr.hxx>
#include <QDebug>
#include <cstdlib>
#include <QObject>
#include <QtTest>
#include <string>

// /////////////////////////////////////////////////
//
// test object
//
// /////////////////////////////////////////////////
class  Test : public QObject {
    Q_OBJECT
private:
Q_SLOT  void initTestCase( ) { }
Q_SLOT  void cleanupTestCase( );
Q_SLOT  void testCase( );
public :
    Test ( ) { }
    ~Test( ) { }
};

// =================================================
// final cleanup
// =================================================
void  Test :: cleanupTestCase()
{
    qInfo() << "current MemCntr:" <<
            QxPack::IcMemCntr::currNewCntr();
}

// ========================================
// test the timer
// ========================================
void   Test :: testCase()
{
    qxpack_ic_enable_mem_trace(true);
    int *t = qxpack_ic_new( int );
    qxpack_ic_delete( t, int );
    QString fn = QCoreApplication::applicationDirPath() + "/info.txt";
    QxPack::IcMemCntr::saveTraceInfoToFile( fn.replace("/","\\").toUtf8().constData() );
    QVERIFY( true );
}

QTEST_MAIN( Test )
#include "main.moc"
