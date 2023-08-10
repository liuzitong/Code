#include <QCoreApplication>

#include "qxpack/indcom/common/qxpack_ic_logging.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/common/qxpack_ic_heapvectortemp.hpp"

#include <QString>
#include <QDebug>
#include <QtTest>
#include <random>
#include <cstdlib>
#include <ctime>

// ////////////////////////////////////////////////////////////////////////////
//
// test heap vector temp
//
// ////////////////////////////////////////////////////////////////////////////
class  TestHeapVectorTemp : public QObject {
    Q_OBJECT
private:
    Q_SLOT  void initTestCase( ) { }
    Q_SLOT  void cleanupTestCase ( );
    Q_SLOT  void testMaxHeapSort ( );
    Q_SLOT  void testMiniHeapSort( );
    static  void outputBuff( int *buff, int size );
public :

};

// ============================================================================
// output the  buffer [ static ]
// ============================================================================
void   TestHeapVectorTemp :: outputBuff( int *buff, int size )
{
    std::printf("\n=== current buffer element ===\n");
    for ( int x = 0; x < size; x ++ ) {
        if ( x % 8 == 0 && x != 0 ) { std::printf("\n"); }
        std::printf("%6d, ", buff[x]);
    }
    std::fflush(stdout);
}

// ============================================================================
// output the memory counter
// ============================================================================
void   TestHeapVectorTemp :: cleanupTestCase( )
{
    qInfo() << "current MemCntr:" << QxPack::IcMemCntr::currNewCntr();
}

// ============================================================================
// test the minimal heap
// ============================================================================
void   TestHeapVectorTemp :: testMiniHeapSort( )
{
    QxPack::IcHeapVector<int,QxPack::IcHeapVectorLess<int>>  mini_heap( nullptr, nullptr );

    // randomize insert data
    std::random_device rd;
    std::mt19937   gen( rd() );
    std::uniform_int_distribution<> d(0,100);
    for ( int x = 0; x < 100; x ++ ) {
        mini_heap.insert( d(gen) % 255 );
    }

    int buff[100];
    for ( int x = 0; x < 100; x ++ ) {
        mini_heap.removeAt( 0, buff[x] );
    }
    // now check the data order..
    bool is_order = true;
    for ( int x = 0; x < 99; x ++ ) {
        if ( buff[x] > buff[x+1]) {
            is_order = false; break;
        }
    }
    if ( ! is_order ) {
        outputBuff( buff, sizeof(buff) / sizeof(int));
    }
    QVERIFY2( is_order, "mini sort failed!" );

}


// ============================================================================
// test heap vector by randomize insert number ( mini )
// ============================================================================
void  TestHeapVectorTemp :: testMaxHeapSort ( )
{
    QxPack::IcHeapVector<int,QxPack::IcHeapVectorGreat<int>>  max_heap(nullptr,nullptr);

    std::random_device rd;
    std::mt19937       gen(rd());
    std::uniform_int_distribution<> d(0,100);
    for ( int x = 0; x < 100; x ++ ) {
        max_heap.insert( d(gen) % 255 );
    }

    int buff[100];
    for ( int x = 0; x < 100; x ++ ) {
        max_heap.removeAt( 0, buff[x] );
    }

    // now check the data order..
    bool is_order = true;
    for ( int x = 0; x < 99; x ++ ) {
        if ( buff[x] < buff[x+1]) {
            is_order = false; break;
        }
    }
    QVERIFY2( is_order, "max sort failed!" );
    if ( ! is_order ) {
        outputBuff( buff, sizeof(buff) / sizeof(int));
    }
}

QTEST_MAIN( TestHeapVectorTemp )

#include "main.moc"

