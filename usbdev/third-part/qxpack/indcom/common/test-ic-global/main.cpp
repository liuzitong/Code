#include <QCoreApplication>
#include <qxpack/indcom/common/qxpack_ic_memcntr.hxx>
#include <QDebug>
#include <cstdlib>
#include <QObject>
#include <QtTest>
#include <string>
#include <qxpack/indcom/common/qxpack_ic_global.hxx>
#include <cmath>
#include <QString>
#include <QElapsedTimer>

// /////////////////////////////////////////////////
//
// test object
//
// /////////////////////////////////////////////////
class  Test : public QObject {
    Q_OBJECT
private:
Q_SLOT  void initTestCase( ) {  }
Q_SLOT  void cleanupTestCase ( );
//Q_SLOT  void testFastRound( );
//Q_SLOT  void testFastExp( );
Q_SLOT  void testLCG( );
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

// =================================================
// test the fast round function
// =================================================
#if 0
void  Test :: testFastRound()
{
    QVERIFY( QxPack::fastRound( 13.4999 ) == QxPack::round(13.4999) );
    QVERIFY( QxPack::fastRound( 13.5001 ) == QxPack::round(13.5001) );
    QVERIFY( QxPack::fastRound(-13.4999 ) == QxPack::round(-13.4999) );
    QVERIFY( QxPack::fastRound(-13.5001 ) == QxPack::round(-13.5001) );

    QVERIFY( QxPack::fastRound( 13.4999 ) == std::round(13.4999) );
    QVERIFY( QxPack::fastRound( 13.5001 ) == std::round(13.5001) );
    QVERIFY( QxPack::fastRound(-13.4999 ) == std::round(-13.4999) );
    QVERIFY( QxPack::fastRound(-13.5001 ) == std::round(-13.5001) );
}
#endif

// =================================================
// test the fastexp
// =================================================
#if 0
void  Test :: testFastExp()
{
    #define FASTEXP_STEP 0.001
    #define FASTEXP_TEST_TM 1000
    int count = 0;
    for( double x = -20; x < 20; x += FASTEXP_STEP ) { ++ count; }
    double *r0 = T_PtrCast( double *, std::malloc( sizeof( double ) * size_t( count )));
    double *r1 = T_PtrCast( double *, std::malloc( sizeof( double ) * size_t( count )));

    qint64 r1_elapsed = 0;
    {
        QElapsedTimer t; int idx = 0;
        t.start();
        for ( int tm = 0; tm < FASTEXP_TEST_TM; tm ++ ) {
            idx = 0;
            for ( double x = -20; x < 20; x += FASTEXP_STEP ) {
                r1[ idx ++ ] = QxPack::fastExp( x );
            }
        }
        r1_elapsed = t.elapsed();
    }

    qint64 r0_elapsed = 0;
    {
        QElapsedTimer t; int idx = 0;
        t.start();
        for ( int  tm = 0; tm < FASTEXP_TEST_TM; tm ++ ) {
            idx = 0;
            for ( double x = -20; x < 20 ;x += FASTEXP_STEP ) {
               r0[ idx ++ ] = std::exp( x );
            }
        }
        r0_elapsed = t.elapsed();
    }

    qInfo() << "r0_elapsed:" << r0_elapsed << " r1_elapsed:" << r1_elapsed;

    std::free( r0 );
    std::free( r1 );

//    for ( double x = -20; x < 20; x += 0.1 ) {
//        double r0 = QxPack::fastExp( x );
//        double r1 = std::exp( x );
//        double diff = std::fabs( r0 - r1 );
//        qInfo() << x <<": " <<  r0 << "," << r1 << "," << diff;
////        QVERIFY2(
////            diff < 0.5,
////            QString("the diff. is: %1  fastExp(%2)")
////                .arg(diff).arg(x).toUtf8().constData()
////        );
//    }
}
#endif

// ============================================================================
// test the LCG
// ============================================================================
void  Test :: testLCG( )
{
    QxPack::IcLCG lcg;
    for ( int x = 0; x < 100; x ++ ) {
        qInfo() << lcg.value();
    }

   #define prn( c, ... ) qInfo( "cond:" #c " ", ## __VA_ARGS__ )
  // prn( "show me " );
   QXPACK_IC_ASSERT( false, "show mmm" );
}



QTEST_MAIN( Test )
#include "main.moc"
