#include <QCoreApplication>

#include "../qxpack_ic_algor_ary.hpp"
#include "../qxpack_ic_algor_arydet.hpp"
#include "../qxpack_ic_algor_peak.hpp"
#include "../qxpack_ic_algor_arypeakftr.hpp"
#include "../qxpack_ic_algor_arystats.hpp"
#include "../qxpack_ic_algor_peak.hpp"

#include <QFile>
#include <cstdlib>
#include <QDebug>
#include <QVector>

#include <QString>
#include <QDebug>
#include <QtTest>
#include <QThread>
#include <QList>
#include <cstdlib>
#include <cstring>
#include <QTime>


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
    Q_SLOT  void  makePeakAry();
    Q_SLOT  void  findAllPeaks();
public :

};

// ============================================================================
// output the memory counter
// ============================================================================
auto     Tester :: cleanupTestCase( ) -> void
{
}

// ============================================================================
// test make peak
// ============================================================================
static auto testPassVec( const QVector<IcAlgor::PeakDescr> &pk ) -> QVector<IcAlgor::PeakDescr>
{
    QVector<IcAlgor::PeakDescr> rsl;
    return rsl;
}

auto     Tester :: makePeakAry( ) -> void
{
    uint8_t dat1[] = { 1, 3, 3, 2, 4, 5  };
     int8_t rsl1[sizeof(dat1)/sizeof(dat1[0])];
     int8_t tgt1[] = { 2, -2, 0, 2, 0, -2 };
    IcAlgor::makePeakAry( dat1, rsl1, sizeof(dat1)/sizeof(dat1[0]) );
    auto ret = std::memcmp( rsl1, tgt1, sizeof(rsl1) );

    QVector<IcAlgor::PeakDescr> vec;
    auto pk2 = testPassVec( vec );
    QVERIFY2( ret == 0, "bad peak ary!");
}

// ============================================================================
// find all peaks
// ============================================================================
auto     Tester :: findAllPeaks() -> void
{
     uint8_t dat1[] = { 1, 3, 3, 2, 4, 5, 6 };
     int8_t  pa[ sizeof(dat1)/sizeof(dat1[0])];

     IcAlgor::makePeakAry( dat1, pa, sizeof(dat1)/sizeof(dat1[0]));
     size_t beg = 0;
     while ( true ) {
         auto pk = IcAlgor::findNextPeak( dat1, pa, sizeof(dat1)/sizeof(dat1[0]), beg );
         if ( ! pk.is_valid ) { break; }
         qInfo("pk[%d,%d,%d ] (half:%s) ", pk.begin, pk.middle, pk.end, ( pk.is_mid_need_add_pt5 ? "T":"F") );
     }

}


QTEST_MAIN( Tester )

#include "main.moc"

