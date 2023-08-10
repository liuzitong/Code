#include <QCoreApplication>

#include "../qxpack_ic_algor_base.hpp"

#include <QString>
#include <QDebug>
#include <QtTest>
#include <QThread>
#include <QList>
#include <cstdlib>
#include <cstring>
#include <QTime>
#include <QVector>

using namespace QxPack;

// ////////////////////////////////////////////////////////////////////////////
//
//   test the algorithm base template
//
// ////////////////////////////////////////////////////////////////////////////
class  Tester : public QObject {
    Q_OBJECT
private:
    Q_SLOT  void  initTestCase( ) { }
    Q_SLOT  void  cleanupTestCase ( );
    Q_SLOT  void  fuzzyIsNull();
public :

};

// ============================================================================
// output the memory counter
// ============================================================================
auto     Tester :: cleanupTestCase( ) -> void
{
}

// ============================================================================
// test round
// ============================================================================
auto     Tester :: fuzzyIsNull( ) -> void
{
    auto t1_t = IcAlgor::fuzzyIsNull( double(0) );
    auto t1_f = IcAlgor::fuzzyIsNull( double(0.000001));
    auto t2_t = IcAlgor::fuzzyIsNull( int(0) );
    auto t2_f = IcAlgor::fuzzyIsNull( int(INT_MIN) );

    QVERIFY2( t1_t, "fuzzyIsNull(double(0)) should be true!");
    QVERIFY2(!t1_f, "fuzzyIsNull(double(0.000001) should be false!");
    QVERIFY2( t2_t, "fuzzyIsNull( int(0) ) should be true!");
    QVERIFY2(!t2_f, "fuzzyIsNull( int(INT_MIN) ) should be false!");

}


QTEST_MAIN( Tester )

#include "main.moc"

