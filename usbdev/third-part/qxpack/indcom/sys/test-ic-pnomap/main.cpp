#include <QCoreApplication>
#include <QDebug>
#include <QtTest>
#include <QByteArray>
#include <QTimer>

#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/sys/qxpack_ic_apptot_priv.hxx"
#include "qxpack/indcom/sys/qxpack_ic_appdcl_priv.hxx"
#include "qxpack/indcom/sys/qxpack_ic_pnomap.hpp"



// ////////////////////////////////////////////////////////////////////////////
//
// test object
//
// ////////////////////////////////////////////////////////////////////////////
class  TestIcPnoMap : public QObject {
    Q_OBJECT
private:
Q_SLOT  void  initTestCase( ) {  }
Q_SLOT  void  cleanupTestCase  ( );
Q_SLOT  void  testInsert();
Q_SLOT  void  testRemove();
Q_SLOT  void  testFindValue();
Q_SLOT  void  testPrefixFindValue();
Q_SLOT  void  testEnum();
public :
    TestIcPnoMap ( );
};

// ============================================================================
// ctor
// ============================================================================
    TestIcPnoMap :: TestIcPnoMap ( ) {  }

// ============================================================================
// final cleanup
// ============================================================================
void  TestIcPnoMap :: cleanupTestCase()
{
    qInfo() << "current MemCntr:" << QxPack::IcMemCntr::currNewCntr();
}

// ============================================================================
// test the insert function
// ============================================================================
void   TestIcPnoMap :: testInsert()
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    //QxPack::IcMemCntr::enableMemTrace( true );
    {
        QxPack::IcPnoMap<int>  map;
        map.insert( "ccc", 0 );
        map.insert( "cc", 0 );
        map.insert( "abc", 0 );
        QVERIFY2( map.size() == 3, "insert failed!" );
    }
    QxPack::IcAppDclPriv::barrier();

    //QxPack::IcMemCntr::saveTraceInfoToFile("z:/t.txt");;
    QVERIFY2( QxPack::IcMemCntr::currNewCntr() == mem_cntr, "exist memory leak!" );
}

// ============================================================================
// test the remove function
// ============================================================================
void    TestIcPnoMap :: testRemove()
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    //QxPack::IcMemCntr::enableMemTrace( true );
    {
        QxPack::IcPnoMap<int>  map;
        map.insert( "ccc", 0 );
        map.insert( "cc", 0 );
        map.insert( "abc", 0 );
        map.remove( "cc");
        QVERIFY2( map.size() == 2, "remove failed!" );
    }
    QxPack::IcAppDclPriv::barrier();

    //QxPack::IcMemCntr::saveTraceInfoToFile("z:/t.txt");;
    QVERIFY2( QxPack::IcMemCntr::currNewCntr() == mem_cntr, "exist memory leak!" );
}

// ============================================================================
// test accurate find
// ============================================================================
void    TestIcPnoMap :: testFindValue()
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    //QxPack::IcMemCntr::enableMemTrace( true );
    {
        QxPack::IcPnoMap<int>  map;
        map.insert( "ccc", 0 );
        map.insert( "cc", 1 );
        map.insert( "abc", 2 );
        QVERIFY2( map.findValue("abc") == 2, "find failed!" );
    }
    QxPack::IcAppDclPriv::barrier();

    //QxPack::IcMemCntr::saveTraceInfoToFile("z:/t.txt");;
    QVERIFY2( QxPack::IcMemCntr::currNewCntr() == mem_cntr, "exist memory leak!" );
}

// ============================================================================
// test prefix find
// ============================================================================
void   TestIcPnoMap :: testPrefixFindValue()
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    //QxPack::IcMemCntr::enableMemTrace( true );
    {
        QxPack::IcPnoMap<int>  map;
        map.insert( "ccc", 0 );
        map.insert( "dabb", 4 );
        map.insert( "c", 3 );
        map.insert( "effg", 5);
        map.insert( "cc", 1 );
        map.insert( "abc", 2 );

        int c2c_v3 = map.prefixFindValue( "c2c" ); // should match ( "c", 3 )
        map.remove( "c");
        int ccc_v1 = map.prefixFindValue( "ccc" ); // should match ( "cc", 1 )
        map.remove( "cc");
        int ccc_v0 = map.prefixFindValue( "ccc" ); // should match ( "ccc", 0 )

        QVERIFY2( c2c_v3  == 3, "\"c2c\" doen't match \"c\" " );
        QVERIFY2( ccc_v1  == 1, "after remove \"c\",   \"ccc\" doen't match \"cc\" ");
        QVERIFY2( ccc_v0  == 0, "after remove \"cc\",  \"ccc\" doen't match \"ccc\" ");
    }
    QxPack::IcAppDclPriv::barrier();

    //QxPack::IcMemCntr::saveTraceInfoToFile("z:/t.txt");;
    QVERIFY2( QxPack::IcMemCntr::currNewCntr() == mem_cntr, "exist memory leak!" );
}

// ============================================================================
// test enum all member in the map
// ============================================================================
void    TestIcPnoMap :: testEnum()
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    //QxPack::IcMemCntr::enableMemTrace( true );
    {
        int     val[] = { 0, 1, 2, 3, 4, 5, 6 };

        QxPack::IcPnoMap<int>  map;
        for ( int i = 0; i < sizeof(val)/sizeof(int); i ++ ) {
            map.insert( QString().setNum( val[i] ), val[i] );
        }

        QxPack::IcPnoMap<int>::const_iterator citr = map.constBegin();
        while ( citr != map.constEnd()) {
            int v = citr.value(); citr ++;
            bool is_found = false;
            for ( int i = 0; i < sizeof(val)/sizeof(int); i ++ ) {
                if ( val[i] < 0 ) { continue; }
                if ( val[i] == v ) { is_found = true; val[i] = -1; break; } // found, mark it
            }
            QVERIFY2( is_found, QString("%1 value not found in orignal value array.").arg( v ).toUtf8().constData() );
        }

    }
    QxPack::IcAppDclPriv::barrier();
    //QxPack::IcMemCntr::saveTraceInfoToFile("z:/t.txt");;
    QVERIFY2( QxPack::IcMemCntr::currNewCntr() == mem_cntr, "exist memory leak!" );
}



QTEST_MAIN( TestIcPnoMap )
#include "main.moc"
