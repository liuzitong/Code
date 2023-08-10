#include <QCoreApplication>

#include "../qxpack_ic_algor_pot.hpp"

#include <QString>
#include <QDebug>
#include <QtTest>
#include <QThread>
#include <QList>
#include <cstdlib>
#include <cstring>
#include <QTime>
#include <QVector>

// ////////////////////////////////////////////////////////////////////////////
//
//   test the error deviation template
//
// ////////////////////////////////////////////////////////////////////////////
class  Tester : public QObject {
    Q_OBJECT
private:
    Q_SLOT  void  initTestCase( ) { }
    Q_SLOT  void  cleanupTestCase ( );
    Q_SLOT  void  test_upperToNextPOT( );
public :

};

// ============================================================================
// output the memory counter
// ============================================================================
auto     Tester :: cleanupTestCase( ) -> void
{

}

// ============================================================================
// round the value to power of 2
// ============================================================================
auto     Tester :: test_upperToNextPOT() -> void
{
    size_t n = -1UL;
    auto n_rsl = QxPack::IcAlgor::upperToNextPOT( n );
    QVERIFY2( n_rsl == 0, "upper -1UL failed!" );
    uint64_t n64 = 8;
    auto n64_rsl = QxPack::IcAlgor::upperToNextPOT( n64 );
    QVERIFY2( n64_rsl == 8, "upper 8 (uint64_t) failed!"  );
    uint v = 22;
    auto v_rsl = QxPack::IcAlgor::upperToNextPOT( v );
    QVERIFY2( v_rsl == 32, "upper 22 (uint) failed!"  );
    uint16_t v16 = 34;
    auto v16_rsl = QxPack::IcAlgor::upperToNextPOT( v16 );
    QVERIFY2( v16_rsl == 64, "upper 34( uint16_t ) failed!"  );

}


QTEST_MAIN( Tester )

#include "main.moc"

