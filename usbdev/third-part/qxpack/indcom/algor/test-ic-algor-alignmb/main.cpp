#include <QCoreApplication>

#include "../qxpack_ic_algor_alignmb.hpp"

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
    Q_SLOT  void  alignedMemBlock();
public :

};

// ============================================================================
// output the memory counter
// ============================================================================
auto     Tester :: cleanupTestCase( ) -> void
{

}

// ============================================================================
// round the value to power of 2
// ============================================================================
auto     Tester :: alignedMemBlock() -> void
{
    size_t N[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 16 };

    for ( size_t i = 0; i < sizeof(N)/sizeof(N[0]); i ++ ) {
        IcAlgor::AlignedMemBlock mb( N[i], 1024 );
        qDebug("req align: %d, alloc raw ptr: 0x%x, return data ptr: 0x%x",
            N[i], mb.rawPtr(), mb.dataPtr());
    }
}


QTEST_MAIN( Tester )

#include "main.moc"

