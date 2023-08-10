#include <QCoreApplication>

#include "../qxpack_ic_algor_shuffle.hpp"

#include <QString>
#include <QDebug>
#include <QtTest>
#include <QThread>
#include <QList>
#include <cstdlib>
#include <cstring>


// ////////////////////////////////////////////////////////////////////////////
//
//   test the suffle template
//
// ////////////////////////////////////////////////////////////////////////////
class  Tester : public QObject {
    Q_OBJECT
private:
    Q_SLOT  void  initTestCase( ) { }
    Q_SLOT  void  cleanupTestCase ( );
    Q_SLOT  void  intShuffle( );
public :

};

// ============================================================================
// output the memory counter
// ============================================================================
auto     Tester :: cleanupTestCase( ) -> void
{

}

// ============================================================================
// thread function, generate spec. number into filo
// ============================================================================
auto     Tester :: intShuffle() -> void
{
     int data1[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 }; // ordered
     int data2[ sizeof( data1 ) / sizeof(int)];
     std::memcpy( data2, data1, sizeof(data1) );

     QxPack::IcAlgor::knuthShuffle<int>( data1, sizeof(data1)/sizeof(int) );
     QVERIFY2( std::memcmp( data1, data2, sizeof(data1)) != 0, "the data is same! not shuffled!" );
}



QTEST_MAIN( Tester )

#include "main.moc"

