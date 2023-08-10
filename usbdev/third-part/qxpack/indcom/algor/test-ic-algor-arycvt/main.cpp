#include <QCoreApplication>

#include "../qxpack_ic_algor_arycvt.hpp"
#include "../qxpack_ic_algor_errdev.hpp"
#include <functional>

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

// ============================================================================
// make random value array
// ============================================================================
template <typename T>
static auto  gMakeRandAry( const T &rg, const T &end ) -> QVector<T>
{
    QVector<T> rsl( 2 + qAbs(qrand() % 1024 ), 0 );
    auto lmt = rsl.size();
    for ( int i = 0; i < lmt; i ++ ) {
        rsl[i] = ( T( qAbs( qrand()) % int( end - rg )) + rg );
    }
    return rsl;
}


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
    Q_SLOT  void  cvtTypeAry_DblToInt();
public :

};

// ============================================================================
// output the memory counter
// ============================================================================
auto     Tester :: cleanupTestCase( ) -> void
{
}

// ============================================================================
// convert type
// ============================================================================
auto     Tester :: cvtTypeAry_DblToInt() -> void
{
    QVector<double> data[] = {
        { 0.5, -0.5, 0.4, -0.4, 0.6, -0.6 }
    };
    QVector<int> rsl[] = {
        { 1, -1, 0, 0, 1, -1 }
    };

    for ( size_t i = 0; i < sizeof(data)/sizeof(data[0]); i ++ ) {
        QVector<int> vec( data[i].size() );
        IcAlgor::cvtTypeAry( data[i].constData(), vec.data(), size_t( data[i].size()) );
        QVERIFY2( vec == rsl[i], "bad convert!");
    }

}


QTEST_MAIN( Tester )

#include "main.moc"

