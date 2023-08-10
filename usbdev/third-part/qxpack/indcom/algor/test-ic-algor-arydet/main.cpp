#include <QCoreApplication>

#include "../qxpack_ic_algor_arydet.hpp"
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
    Q_SLOT  void  thrGtPosDetAry ();
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
auto     Tester :: thrGtPosDetAry() -> void
{
    QVector<int> data[] = {
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 2, 3, 4, 5, 6, 7 }
    };
    int  thr[] = {
        5
    };
    QVector<size_t> rsl[] = {
        { 6, 7, 8, 13, 14 }
    };

    for ( size_t i = 0; i < sizeof(data)/sizeof(data[0]); i ++ ) {
        QVector<size_t> vec( data[i].size() );
        auto num = IcAlgor::thrGtPosDetAry( data[i].constData(), size_t( data[i].size()), vec.data(), size_t( vec.size()), thr[i] );
        QVERIFY2( num == size_t( rsl->size()), "" );
        auto is_same = std::memcmp( rsl->constData(), vec.constData(), sizeof(int)*num );
        QVERIFY2( is_same == 0, "bad position result" );
    }

}


QTEST_MAIN( Tester )

#include "main.moc"

