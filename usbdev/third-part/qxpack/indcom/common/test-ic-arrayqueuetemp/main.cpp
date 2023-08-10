#include <QCoreApplication>

#include <qxpack/indcom/common/qxpack_ic_logging.hxx>
#include <qxpack/indcom/common/qxpack_ic_memcntr.hxx>
#include "qxpack/indcom/common/qxpack_ic_stacktemp.hpp"

#include <QString>
#include <QDebug>
#include <QtTest>
#include <QThread>

#include <random>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <list>


typedef QxPack::IcNodeStackTemp<int,10>  IntStackSpsc10;

// ////////////////////////////////////////////////////////////////////////////
//
// test FiLoArraySpSc temp
//
// ////////////////////////////////////////////////////////////////////////////
class  TestStackTemp : public QObject {
    Q_OBJECT
private:
    Q_SLOT  void  initTestCase( ) { }
    Q_SLOT  void  cleanupTestCase ( );
    Q_SLOT  void  pushAndPop100( );

    static void pushFunc( int g_num, IntStackSpsc10 *filo );
    static void popFunc ( int p_num, IntStackSpsc10 *filo );
public :

};

// ============================================================================
// output the memory counter
// ============================================================================
void   TestStackTemp :: cleanupTestCase( )
{
    qInfo() << "current MemCntr:" << QxPack::IcMemCntr::currNewCntr();
}

// ============================================================================
// thread function, generate spec. number into filo
// ============================================================================
void   TestStackTemp :: pushFunc (
    int g_num, IntStackSpsc10 *filo
) {
    std::random_device rd;
    std::mt19937   gen( rd() );
    std::uniform_int_distribution<> d(0,100);

    while ( g_num > 0 ) {
        int v = d(gen) % 100;
        if ( filo->push( v )) {
            -- g_num;
            qInfo("pushed %d", v );
        } else {
            qInfo("pushed failed, full?");
        }
        int ms = d(gen)%50;
        QThread::msleep(ms);
    }
}

// ============================================================================
// popup the value from filo
// ============================================================================
void   TestStackTemp :: popFunc(
    int p_num, IntStackSpsc10 *filo
) {
    std::random_device rd;
    std::mt19937   gen( rd() );
    std::uniform_int_distribution<> d(0,100);

    while ( p_num > 0 ) {
       int v = 0;
       if ( filo->pop( v )) {
           -- p_num;
           qInfo("pop %d", v );
       } else {
           qInfo("pop failed, empty?");
       }
       int ms = d(gen)%50;
       QThread::msleep(ms);
    }
}


// ============================================================================
// test push and pop 100 elements
// ============================================================================
void   TestStackTemp :: pushAndPop100()
{
    IntStackSpsc10 filo;

    std::thread t_push( & pushFunc, 100, &filo );
    std::thread t_pop ( & popFunc,  100, &filo  );
    t_push.join();
    t_pop.join();

}




QTEST_MAIN( TestStackTemp )

#include "main.moc"

