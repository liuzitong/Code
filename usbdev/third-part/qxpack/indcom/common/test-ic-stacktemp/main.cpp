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

typedef QxPack::IcNodeStackTemp<int>  IntPtrStack;

// ////////////////////////////////////////////////////////////////////////////
//
// test NodeListStackTemp
//
// ////////////////////////////////////////////////////////////////////////////
class  TestNodeListStackTemp : public QObject {
    Q_OBJECT
private:
    Q_SLOT  void  initTestCase( ) { }
    Q_SLOT  void  cleanupTestCase ( );
    Q_SLOT  void  pushAndPop100( );

    static void pushFunc( int g_num, IntPtrStack *filo );
    static void popFunc ( int p_num, IntPtrStack *filo );
public :

};

// ============================================================================
// output the memory counter
// ============================================================================
void   TestNodeListStackTemp :: cleanupTestCase( )
{
    qInfo() << "current MemCntr:" << QxPack::IcMemCntr::currNewCntr();
}

// ============================================================================
// thread function, generate spec. number into filo
// ============================================================================
void   TestNodeListStackTemp :: pushFunc (
    int g_num, IntPtrStack *filo
) {
    std::random_device rd;
    std::mt19937   gen( rd() );
    std::uniform_int_distribution<> d(0,100);

    while ( g_num > 0 ) {
        int v = d(gen) % 100;
        IntPtrStack::Node *node = new IntPtrStack::Node(v);
        filo->takeAndPush( node );
        -- g_num;
        qInfo("%x pushed %d", QThread::currentThread(), v );

        int ms = d(gen)%50;
        QThread::msleep(ms);
    }
}

// ============================================================================
// popup the value from filo
// ============================================================================
void   TestNodeListStackTemp :: popFunc( int p_num, IntPtrStack *filo )
{
    std::random_device rd;
    std::mt19937   gen( rd() );
    std::uniform_int_distribution<> d(0,100);

    while ( p_num > 0 ) {
       IntPtrStack::Node *n = filo->pop();
       if ( n != nullptr ) {
           -- p_num;
           qInfo("%x pop %d", QThread::currentThread(), n->data() );
           delete n;
       } else {
           qInfo("%x pop failed, empty?", QThread::currentThread());
       }
       int ms = d(gen)%50;
       QThread::msleep(ms);
    }
}


// ============================================================================
// test push and pop 100 elements
// ============================================================================
void   TestNodeListStackTemp :: pushAndPop100()
{
    IntPtrStack filo(
        []( IntPtrStack::Node *n, void*) {
            if ( n != nullptr ) { delete n; }
        }, nullptr
    );

    std::thread t_push1( & pushFunc, 50, &filo );
    std::thread t_push2( & pushFunc, 50, &filo );
    std::thread t_pop1 ( & popFunc,  50, &filo );
    std::thread t_pop2 ( & popFunc,  50, &filo );

    t_push1.join();
    t_push2.join();
    t_pop1.join();
    t_pop2.join();
}


QTEST_MAIN( TestNodeListStackTemp )

#include "main.moc"

