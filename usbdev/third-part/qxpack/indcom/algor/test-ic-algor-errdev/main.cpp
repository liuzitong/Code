#include <QCoreApplication>

#include "../qxpack_ic_algor_errdev.hpp"

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
    Q_SLOT  void  test_IcIsErrDevLess_T( );
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
auto     Tester :: test_IcIsErrDevLess_T() -> void
{
    QVector<double> data1 = { 0, 1, 2, 3, 4, 5, 6 };
    QVector<double> data2 = { 0, 1, 2, 3, 4, 5, 6 };

    // add error dev
    qsrand( uint( QTime::currentTime().elapsed()) );
    for ( int i = 0; i < data1.size(); i ++ ) {
        data2[i] += ( qrand() % 10 ) / 100.0;
    }

    auto rsl = QxPack::IcAlgor::isErrDevLess( data1.constData(), data2.constData(), 0.1, size_t( data1.size()) );
    QVERIFY2( rsl, "bad compare");
}



QTEST_MAIN( Tester )

#include "main.moc"

