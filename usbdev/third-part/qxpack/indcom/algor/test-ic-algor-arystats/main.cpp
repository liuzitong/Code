#include <QCoreApplication>

#include "../qxpack_ic_algor_arystats.hpp"
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
    Q_SLOT  void  aryZeroCentered ( );
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
auto     Tester :: aryZeroCentered() -> void
{
    QVector<double> data[] = {
        { 5, 5, 5, 5, 25 }
    };
    QVector<double> rsl[] = {
        { -4, -4, -4, -4, 16 }
    };
    IcAlgor::aryZeroCentered( data[0].data(), size_t( data[0].size()) );

    qInfo() << data[0];
    qInfo() << rsl[0];

}


QTEST_MAIN( Tester )

#include "main.moc"

