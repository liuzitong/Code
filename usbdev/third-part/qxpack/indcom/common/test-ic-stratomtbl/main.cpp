#include <QCoreApplication>
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include <QDebug>
#include <cstdlib>
#include <QObject>
#include <QtTest>
#include <string>
#include "qxpack/indcom/common/qxpack_ic_global.hxx"
#include "qxpack/indcom/common/qxpack_ic_stratomtbl.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include <QString>
#include <QElapsedTimer>
#include <QTime>
#include <QSharedPointer>
#include <QThread>
#include <QtGlobal>

// ////////////////////////////////////////////////////////////////////////////
//
// test object
//
// ////////////////////////////////////////////////////////////////////////////
class  Test : public QObject {
    Q_OBJECT
private:
Q_SLOT  void  initTestCase( ) {  }
Q_SLOT  void  cleanupTestCase ( );
Q_SLOT  void  test_add_dyncStr();
Q_SLOT  void  test_add_staticConstStr();
Q_SLOT  void  test_add_mixingStr();
Q_SLOT  void  test_query_dyncStr();

public :
    Test ( ) { }
    ~Test( ) { }
};

// ============================================================================
// final cleanup
// ============================================================================
void  Test :: cleanupTestCase()
{
    qInfo() << "current MemCntr:" <<
            QxPack::IcMemCntr::currNewCntr();
}

// ============================================================================
// test add dync allocated memory
// ============================================================================
static const char *  makeAllocStr( const char *sp )
{
   size_t len = std::strlen( sp );
   char *mp = reinterpret_cast<char*>( std::malloc( len + 2 ));
   std::memset( mp, 0, len + 2 );
   std::memcpy( mp, sp, len );
   qxpack_ic_incr_new_cntr(1);
   return mp;
}

static void  freeAllocStr( const char *sp, void* )
{
    if ( sp != nullptr ) {
        std::free( const_cast<char*>( sp ));
        qxpack_ic_decr_new_cntr(1);
    }
}

void  Test :: test_add_dyncStr()
{
    int mem_cntr =  QxPack::IcMemCntr::currNewCntr();
    {
        QxPack::IcStrAtomTbl atom_tbl;

        uintptr_t v1 = atom_tbl.add( makeAllocStr( "black sheep wall!"), &freeAllocStr );
        uintptr_t v2 = atom_tbl.add( makeAllocStr( "black sheep wall!"), &freeAllocStr );
        uintptr_t v3 = atom_tbl.add( makeAllocStr( "black sheep wall 2!"), &freeAllocStr );
        qInfo("v1 = %ud, v2 = %ud, v3 = %ud", v1, v2, v3 );

        QVERIFY2( v1 == v2, "add same string return diff value!" );
        QVERIFY2( v1 != v3, "add diff string return same value!" );
    }
    int curr_cntr = QxPack::IcMemCntr::currNewCntr();
    QVERIFY2( mem_cntr == curr_cntr, "memory leak!");
}

// ============================================================================
// test add static const data
// ============================================================================
void   Test :: test_add_staticConstStr()
{
    int mem_cntr =  QxPack::IcMemCntr::currNewCntr();
    {
        QxPack::IcStrAtomTbl atom_tbl;

        uintptr_t v1 = atom_tbl.add( "black sheep wall!" );
        uintptr_t v2 = atom_tbl.add( "black sheep wall!" );
        uintptr_t v3 = atom_tbl.add( "black sheep wall 2!" );
        qInfo("v1 = %ud, v2 = %ud, v3 = %ud", v1, v2, v3 );

        QVERIFY2( v1 == v2, "add same string return diff value!" );
        QVERIFY2( v1 != v3, "add diff string return same value!" );
    }
    int curr_cntr = QxPack::IcMemCntr::currNewCntr();
    QVERIFY2( mem_cntr == curr_cntr, "memory leak!");
}

// ============================================================================
// test add mixing string
// ============================================================================
void   Test :: test_add_mixingStr()
{
    int mem_cntr =  QxPack::IcMemCntr::currNewCntr();
    {
        QxPack::IcStrAtomTbl atom_tbl;

        uintptr_t v1 = atom_tbl.add( "black sheep wall!" );
        uintptr_t v2 = atom_tbl.add( makeAllocStr( "black sheep wall!"), &freeAllocStr );
        uintptr_t v3 = atom_tbl.add( "black sheep wall 2!" );
        qInfo("v1 = %ud, v2 = %ud, v3 = %ud", v1, v2, v3 );

        QVERIFY2( v1 == v2, "add same string return diff value!" );
        QVERIFY2( v1 != v3, "add diff string return same value!" );
    }
    int curr_cntr = QxPack::IcMemCntr::currNewCntr();
    QVERIFY2( mem_cntr == curr_cntr, "memory leak!");
}

// ============================================================================
// test query dync string
// ============================================================================
void   Test :: test_query_dyncStr()
{
    int mem_cntr =  QxPack::IcMemCntr::currNewCntr();
    {
        QxPack::IcStrAtomTbl atom_tbl;

        uintptr_t v1 = atom_tbl.add( "black sheep wall!" );
        uintptr_t v2 = atom_tbl.add( makeAllocStr( "black sheep wall!"), &freeAllocStr );
        uintptr_t v3 = atom_tbl.add( "black sheep wall 2!" );

        uintptr_t vq = atom_tbl.query( "black sheep wall!" );
        qInfo("v1 = %ud, v2 = %ud, v3 = %ud", v1, v2, v3 );
        qInfo("queryed vq = %ud", vq );

        uintptr_t vq2 = atom_tbl.query( "black sheep wall3!");

        QVERIFY2( v1 == vq, "can not query spec. string!");
        QVERIFY2( vq2 == QxPack::IcStrAtomTbl::invalidValue(), "query non existed string, but it existed!");
    }
    int curr_cntr = QxPack::IcMemCntr::currNewCntr();
    QVERIFY2( mem_cntr == curr_cntr, "memory leak!");
}


QTEST_MAIN( Test )
#include "main.moc"
