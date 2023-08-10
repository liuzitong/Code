#include <QCoreApplication>

#include "qxpack/indcom/common/qxpack_ic_logging.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/common/qxpack_ic_queuetemp.hpp"

#include <QString>
#include <QDebug>
#include <QtTest>
#include <QThread>

#include <random>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <list>

typedef QxPack::IcNodeQueueTemp<int>   IntQueue;
typedef std::atomic<int>  IntCntr;
typedef QxPack::IcArrayQueueTemp<int,1000> IntArrayQueue;

// ////////////////////////////////////////////////////////////////////////////
//
// test FiLoArraySpSc temp
//
// ////////////////////////////////////////////////////////////////////////////
class  TestNodeListQueueTemp : public QObject {
    Q_OBJECT
private:
    Q_SLOT  void  initTestCase( ) { }
    Q_SLOT  void  cleanupTestCase ( );
    Q_SLOT  void  enqAndClearNode ( );
    Q_SLOT  void  enqAndDeqNode ( );
    Q_SLOT  void  enqAndDeqArray( );
    Q_SLOT  void  enumNode();
    static void enqNodeFunc( int g_num, IntCntr *, IntQueue *q );
    static void deqNodeFunc( int p_num, IntQueue *q );
    static void enqArrayFunc( int g_num, IntCntr *, IntArrayQueue *q );
    static void deqArrayFunc( int g_num, IntArrayQueue *q );
public :

};

// ============================================================================
// output the memory counter
// ============================================================================
void   TestNodeListQueueTemp :: cleanupTestCase( )
{
    qInfo() << "current MemCntr:" << QxPack::IcMemCntr::currNewCntr();
}

// ============================================================================
// [ static ] thread function, generate spec. number into queue
// ============================================================================
void   TestNodeListQueueTemp :: enqNodeFunc ( int g_num, IntCntr *i, IntQueue *q )
{
    while ( g_num -- > 0 ) {
        int v = i->fetch_add(1);
        IntQueue::Node *node = new IntQueue::Node( v + 1 );
        q->takeAndEnqueue( node );
        //qInfo("%x IN %d",QThread::currentThread(), v );
    }
}

// ============================================================================
// [ static ] popup the value from queue
// ============================================================================
void   TestNodeListQueueTemp :: deqNodeFunc( int p_num, IntQueue *q )
{
    int v = 0;

    while ( p_num > 0 ) {
        IntQueue::Node *node = q->dequeue();
        if ( node != nullptr ) {
          // qInfo("%x OUT %d", QThread::currentThread(), v = node->data() );
           p_num --;
           delete node;
       } else {
         //  qInfo("dequeue failed, empty?");
       }
    }
}

// ============================================================================
// [ static ] thread function, generate spec. number into queue
// ============================================================================
void TestNodeListQueueTemp :: enqArrayFunc( int g_num, IntCntr *i, IntArrayQueue *q )
{
    while ( g_num -- > 0 ) {
        int v = i->fetch_add(1);
        q->enqueue( v );
        //qInfo("%x IN %d",QThread::currentThread(), v );
    }
}

// ============================================================================
// [ static ] thread function, generate spec. number into queue
// ============================================================================
void TestNodeListQueueTemp :: deqArrayFunc( int g_num, IntArrayQueue *q )
{
    while ( g_num > 0 ) {
        int v = 0;
        if ( q->dequeue( v )) {
           // qInfo("%x OUT %d",QThread::currentThread(), v );
            g_num --;
        } else {
          //  qInfo("dequeue failed, empty?");
        }
    }
}

// ============================================================================
// enqueue node and test clear function
// ============================================================================
void   TestNodeListQueueTemp :: enqAndClearNode()
{
    IntCntr ic(0);
    {
        IntQueue iq(
            []( IntQueue::Node *n, void *ctxt ) {
                IntCntr *ic_ptr = reinterpret_cast<IntCntr*>(ctxt);
                ic_ptr->fetch_sub(1);
                if ( n != nullptr ) { delete n; }
            }, &ic
        );

        std::thread t_enq1( & enqNodeFunc, 10, &ic, &iq );
        t_enq1.join();
    }
    QVERIFY2( ic.load() == 0, "not all nodes freed!" );
}

// ============================================================================
// test enqueue and dequeue 100 elements
// ============================================================================
void   TestNodeListQueueTemp :: enqAndDeqNode()
{
    IntQueue iq(
        []( IntQueue::Node *n, void*) {
            if ( n != nullptr ) { delete n; }
        }, nullptr
    );
    IntCntr ic(0);

    std::thread t_enq1( & enqNodeFunc, 50, &ic, &iq );
    std::thread t_enq2( & enqNodeFunc, 50, &ic, &iq );
    std::thread t_deq ( & deqNodeFunc,  100, &iq  );
    t_enq1.join();
    t_enq2.join();
    t_deq.join();;
}

//// ============================================================================
//// test push and pop 100 elements
//// ============================================================================
void   TestNodeListQueueTemp :: enqAndDeqArray()
{
    IntArrayQueue iq;
    IntCntr ic(0);

    std::thread t_enq1( & enqArrayFunc, 1000, &ic, &iq );
   // std::thread t_enq2( & enqArrayFunc, 50, &ic, &iq );
    std::thread t_deq ( & deqArrayFunc, 1000, &iq  );
    t_enq1.join();
   // t_enq2.join();
    t_deq.join();;
}

// ============================================================================
// enum all node
// ============================================================================
void  TestNodeListQueueTemp :: enumNode()
{
    IntQueue iq(
        []( IntQueue::Node *n, void*) {
            if ( n != nullptr ) { delete n; }
        }, nullptr
    );

    for ( int i = 0; i < 4; i ++ ) {
        iq.takeAndEnqueue( new IntQueue::Node( i ) );
    }

    iq.blockEnumAll (
        []( IntQueue::Node *n,void*) {
            qInfo("%d,", n->data());
        },this
    );

}



QTEST_MAIN( TestNodeListQueueTemp )

#include "main.moc"

