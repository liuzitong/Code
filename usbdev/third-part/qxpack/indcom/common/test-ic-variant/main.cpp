#include <QCoreApplication>
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/common/qxpack_ic_global.hxx"
#include "qxpack/indcom/common/qxpack_ic_pimplprivtemp.hpp"
#include "qxpack/indcom/common/qxpack_ic_variant.hxx"
#include <QDebug>
#include <cstdlib>
#include <QObject>
#include <QtTest>
#include <string>
#include <cmath>
#include <QString>
#include <QVariant>
#include <QElapsedTimer>

// ////////////////////////////////////////////////////////////////////////////
//
//  my struct
//
// ////////////////////////////////////////////////////////////////////////////
struct MyStruc {
    int a, b, c;
    MyStruc( ) {
     //   qInfo("MyStruc Ctor");
        a = 1; b = 2; c = 3; qxpack_ic_incr_new_cntr(1);
    }
    MyStruc( const MyStruc &other ) {
     //   qInfo("MyStruc Ctor( copy )");
        a = other.a; b = other.b; c = other.c; qxpack_ic_incr_new_cntr(1);
    }
    MyStruc &  operator = ( const MyStruc &other )
    {
     //   qInfo("MyStruc operator =");
        a = other.a; b = other.b; c = other.c; qxpack_ic_incr_new_cntr(1); return *this;
    }
    ~MyStruc( )
    {
     //   qInfo("MyStruc dtor");
        qxpack_ic_decr_new_cntr(1);
    }
    bool operator == ( const MyStruc &other ) { return ( a == other.a && b == other.b && c == other.c ); }
};


// ////////////////////////////////////////////////////////////////////////////
//
// test object
//
// ////////////////////////////////////////////////////////////////////////////
class  Test : public QObject {
    Q_OBJECT
private:
Q_SLOT  void initTestCase( ) {  }
Q_SLOT  void cleanupTestCase ( );
Q_SLOT  void testScalar();
Q_SLOT  void testStruc();
Q_SLOT  void testSetScalar();
public :
    Test ( ) { }
    ~Test( ) { }
};

// ============================================================================
// final cleanup
// ============================================================================
void  Test :: cleanupTestCase()
{
    qInfo() << "current MemCntr:" << QxPack::IcMemCntr::currNewCntr();
}

// ============================================================================
// test the variant scalar
// ============================================================================
void                    Test :: testScalar()
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    {
        bool bool_v = true;     double dbl_v = 123;
        signed int int_v = 123; unsigned int uint_v = 123;
        int64_t i64_v = 123;    uint64_t  ui64_v = 123;

        QxPack::IcVariant bool_var( bool_v ), dbl_var( dbl_v ),
             int_var( int_v ), uint_var( uint_v ),
             i64_var( i64_v ), ui64_var( ui64_v );

        QVERIFY2( bool_var.toBool() == bool_v, "bool value not equal!" );
        QVERIFY2( QxPack::fuzzyCompare( dbl_var.toDouble(), dbl_v ), "double value not equal!" );
        QVERIFY2( int_var.toInt() == int_v, "int value not equal!" );
        QVERIFY2( uint_var.toUInt() == uint_v, "uint value not equal!");
        QVERIFY2( i64_var.toLongLong() == i64_v, "long long value not equal!" );
        QVERIFY2( ui64_var.toULongLong() == ui64_v, "unsigned long long not equal!" );
    }
    int curr_cntr = QxPack::IcMemCntr::currNewCntr();
    QVERIFY2( mem_cntr == curr_cntr, "memory leak!");
}

// ============================================================================
// test the variant sturcture
// ============================================================================
void                    Test :: testStruc()
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    {

        MyStruc  my_struc;
        QxPack::IcVariant var; var.setValue( my_struc );
        QVERIFY2( var.value<MyStruc>() == my_struc, "struct not equal!" );
    }
    int curr_cntr = QxPack::IcMemCntr::currNewCntr();
    QVERIFY2( mem_cntr == curr_cntr, "memory leak!");
}

// ============================================================================
// test set the scalar
// ============================================================================
auto  Test :: testSetScalar() -> void
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    {
        QxPack::IcVariant var;
        var.setValue( 10 );
        var.setValue( 10.0f );

    }
    int curr_cntr = QxPack::IcMemCntr::currNewCntr();
    QVERIFY2( mem_cntr == curr_cntr, "memory leak!");
}



QTEST_MAIN( Test )
#include "main.moc"
