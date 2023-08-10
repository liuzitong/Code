#include <QCoreApplication>

#include "../qxpack_ic_algor_parseek.hpp"

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
//   test the error deviation template
//
// ////////////////////////////////////////////////////////////////////////////
class  Tester : public QObject {
    Q_OBJECT
private:
    Q_SLOT  void  initTestCase( ) { }
    Q_SLOT  void  cleanupTestCase ( );
    Q_SLOT  void  parSeekBs();
public :

};

// ============================================================================
// output the memory counter
// ============================================================================
auto     Tester :: cleanupTestCase( ) -> void
{
}

// ============================================================================
// parameter seek binary search
// ============================================================================
auto     Tester :: parSeekBs() -> void
{
    float dat[] = { 0, 1, 7, 10, 12, 22, 33, 99 };
    float tgt[] = { 0,     1,    7,    11,  13.331f };
    bool  rsl[] = { true, true, true, true, true };

    for ( int i = 0; i < sizeof(tgt)/sizeof(tgt[0]); i ++ ) {
        auto tgt_v = tgt[i];
        qInfo("try found tgt:%f", tgt_v);
        auto c_rsl =
             IcAlgor::paramSeekBs<float>(
                 dat[0], dat[sizeof(dat)/sizeof(dat[0]) - 1 ],
                 [&tgt_v]( const float &v ) -> int {
                     qInfo(" compare par:%e ( is zero %d)", v, qFuzzyIsNull(v) );
                     if ( qFuzzyCompare( v,tgt_v) ) { qInfo("found,break");return 0; }
                     return ( v > tgt_v ? 1 : -1 );
             }
        );
        if ( c_rsl.first != rsl[i]) {
            qInfo("bad!");
        }

    }

}


QTEST_MAIN( Tester )

#include "main.moc"

