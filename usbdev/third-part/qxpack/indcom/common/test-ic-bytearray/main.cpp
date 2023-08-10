#include <QCoreApplication>
#include <QDebug>
#include <cstdlib>
#include <QObject>
#include <QtTest>
#include <cstring>
#include <memory>
#include <map>
#include <QList>
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/common/qxpack_ic_bytearray.hxx"

// /////////////////////////////////////////////////
// test object
// /////////////////////////////////////////////////
class  Test : public QObject {
    Q_OBJECT
private:
Q_SLOT  void  initTestCase ( ) { }
Q_SLOT  void  cleanupTestCase ( );

Q_SLOT  void  create_NullObj ( );
Q_SLOT  void  create_SpecSize( );
Q_SLOT  void  create_ByOther ( );
Q_SLOT  void  create_ByAssign( );
Q_SLOT  void  create_CopyUserData ( );
Q_SLOT  void  create_PtrToConstStr( );
Q_SLOT  void  create_PtrToConstStrCleanup( );
Q_SLOT  void  create_PtrToStrCleanup( );

Q_SLOT  void  resize_NullObj ( );
Q_SLOT  void  resize_SpecSize( );
Q_SLOT  void  resize_Shared  ( );
Q_SLOT  void  resize_PtrToConstStr( );
Q_SLOT  void  resize_PtrToConstStrCleanup( );
Q_SLOT  void  resize_PtrToStrCleanup( );

Q_SLOT  void  dataPtr_InShared ( );
Q_SLOT  void  dataPtr_PtrToConstStr( );
Q_SLOT  void  dataPtr_PtrToConstStrCleanup( );
Q_SLOT  void  dataPtr_PtrToStrCleanup( );

//Q_SLOT  void  dataPtr_PtrToStr ( );

Q_SLOT  void  sharedTestSamePtr( );
Q_SLOT  void  sharedTestCow( );
Q_SLOT  void  searchInMap ( );

public :
    Test ( ) { }
    ~Test( ) { }
};

// =================================================
// final cleanup
// =================================================
void  Test :: cleanupTestCase()
{
    qInfo() << "current MemCntr:" <<
            QxPack::IcMemCntr::currNewCntr();
}

// =================================================
// create null object
// =================================================
void   Test :: create_NullObj()
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    bool is_empty_obj = false;
    {
        QxPack::IcByteArray  ba;
        is_empty_obj = ba.isEmpty();

    }
    int curr_cntr = QxPack::IcMemCntr::currNewCntr();
    QVERIFY2( mem_cntr == curr_cntr, "memory leak!");
    QVERIFY2( is_empty_obj, "the object is not null.");
}

// =================================================
// create object by size
// =================================================
void   Test :: create_SpecSize( )
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    int mem_size = 0;
    {
        QxPack::IcByteArray  ba(1024);
        mem_size = ba.size();
    }
    int curr_cntr = QxPack::IcMemCntr::currNewCntr();
    QVERIFY2( mem_cntr == curr_cntr, "memory leak!");
    QVERIFY2(
        mem_size == 1024,
        QString("create object by size failed, current is %1")
        .arg( mem_size ).toUtf8().constData()
    );
}

// =================================================
// create by another
// =================================================
void  Test :: create_ByOther( )
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    bool is_same_ptr = false;
    {
        QxPack::IcByteArray  ba1(1024);
        QxPack::IcByteArray  ba2(ba1);
        is_same_ptr = (  ba1.constData() == ba2.constData() );
    }
    int curr_cntr = QxPack::IcMemCntr::currNewCntr();
    QVERIFY2( mem_cntr == curr_cntr, "memory leak!");
    QVERIFY2( is_same_ptr, "pointer are not same" );
}

// =================================================
// create by assign
// =================================================
void   Test :: create_ByAssign()
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    bool is_same_ptr = false;
    {
        QxPack::IcByteArray  ba1(1024);
        QxPack::IcByteArray  ba2 = ba1;
        is_same_ptr = (  ba1.constData() == ba2.constData() );
    }
    int curr_cntr = QxPack::IcMemCntr::currNewCntr();
    QVERIFY2( mem_cntr == curr_cntr, "memory leak!");
    QVERIFY2( is_same_ptr, "pointer are not same" );
}

// =================================================
// create by point to const data
// =================================================
void    Test :: create_CopyUserData( )
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    const char *user_data = "hello, world!";
    bool is_same_ptr = false; bool is_mem_same = false;
    {
        QxPack::IcByteArray ba( user_data );
        is_same_ptr = ( ba.constData() == user_data );
        int user_data_len = int( std::strlen( user_data ));
        int ba_len = ba.size();

        is_mem_same = ( std::memcmp(
            user_data, ba.constData(),
            size_t( user_data_len <= ba_len ? user_data_len : ba_len )
        ) == 0 );
    }
    int curr_cntr = QxPack::IcMemCntr::currNewCntr();
    QVERIFY2( mem_cntr == curr_cntr, "memory leak!");
    QVERIFY2( ! is_same_ptr, "pointer are same" );
    QVERIFY2( is_mem_same,   "memory content are not same");
}

// =================================================
// create by point to const string
// =================================================
void   Test :: create_PtrToConstStr()
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    const char *u_data = "hello, world";
    bool is_same_ptr = false;
    {
        QxPack::IcByteArray  ba =
            QxPack::IcByteArray::fromConstData( u_data, -1 );
        is_same_ptr = (  ba.constData() == u_data );
    }
    QxPack::IcByteArray::cleanupCache();
    int curr_cntr = QxPack::IcMemCntr::currNewCntr();
    QVERIFY2( mem_cntr == curr_cntr, "memory leak!");
    QVERIFY2( is_same_ptr, "pointer are not same!" );
}

// ==================================================
// create by point to const string, cleanup
// ==================================================
void   Test :: create_PtrToConstStrCleanup()
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    const char *u_data = "hello, world";
    bool is_same_ptr = false; bool is_cleanup_called = false;
    bool is_const_attr = false; bool is_ref_attr = false;
    {
        QxPack::IcByteArray  ba =
            QxPack::IcByteArray::fromConstData( u_data, -1,
                [](void*info) {
                    * T_PtrCast( bool *, info ) = true;
                }, & is_cleanup_called
            );
        is_same_ptr = (  ba.constData() == u_data );
        is_const_attr = ba.isConst();
        is_ref_attr   = ba.isRefData();
    }
    QxPack::IcByteArray::cleanupCache();
    int curr_cntr = QxPack::IcMemCntr::currNewCntr();
    QVERIFY2( mem_cntr == curr_cntr, "memory leak!");
    QVERIFY2( is_same_ptr, "pointer are not same!" );
    QVERIFY2( is_cleanup_called, "cleanup not called.");
    QVERIFY2( is_const_attr, "miss const attribute.");
    QVERIFY2( is_ref_attr,   "miss ref.data attribute.");
}

// ==================================================
// create by point to   string, cleanup
// ==================================================
void   Test :: create_PtrToStrCleanup()
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    const char *u_data = "hello, world";
    bool is_same_ptr = false; bool is_cleanup_called = false;
    bool is_const_attr = false; bool is_ref_attr = false;
    {
        QxPack::IcByteArray  ba =
            QxPack::IcByteArray::fromData( T_PtrCast( char*, u_data ), -1,
                [](void*info) {
                    * T_PtrCast( bool *, info ) = true;
                }, & is_cleanup_called
            );
        is_same_ptr = (  ba.constData() == u_data );
        is_const_attr = ba.isConst();
        is_ref_attr   = ba.isRefData();
    }
    QxPack::IcByteArray::cleanupCache();
    int curr_cntr = QxPack::IcMemCntr::currNewCntr();
    QVERIFY2( mem_cntr == curr_cntr, "memory leak!");
    QVERIFY2( is_same_ptr, "pointer are not same!" );
    QVERIFY2( is_cleanup_called, "cleanup not called.");
    QVERIFY2( !is_const_attr, "invalid const attribute.");
    QVERIFY2( is_ref_attr,   "miss ref.data attribute.");
}


// =================================================
// resize, null object
// =================================================
void   Test :: resize_NullObj()
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    int mem_size = 0;
    {
        QxPack::IcByteArray  ba;
        ba.resize(1024);
        mem_size = ba.size();
    }
    int curr_cntr = QxPack::IcMemCntr::currNewCntr();
    QVERIFY2( mem_cntr == curr_cntr, "memory leak!");
    QVERIFY2(
        mem_size == 1024,
        QString("resize size size failed, current is %1")
        .arg( mem_size ).toUtf8().constData()
    );
}

// ===================================================
// resize, spec size
// ===================================================
void   Test :: resize_SpecSize()
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    int mem_size = 0;
    {
        QxPack::IcByteArray  ba(512);
        ba.resize(1024);
        mem_size = ba.size();
    }
    int curr_cntr = QxPack::IcMemCntr::currNewCntr();
    QVERIFY2( mem_cntr == curr_cntr, "memory leak!");
    QVERIFY2(
        mem_size == 1024,
        QString("resize size size failed, current is %1")
        .arg( mem_size ).toUtf8().constData()
    );
}

// =====================================================
// resize shared
// =====================================================
void    Test :: resize_Shared ( )
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    bool is_same_ptr = false;
    bool is_ba2_size_512 = false;
    bool is_ba1_size_1024 = false;
    {
        QxPack::IcByteArray  ba1(512);
        QxPack::IcByteArray  ba2 = ba1;
        ba1.resize(1024);
        is_same_ptr = ( ba1.constData() == ba2.constData());
        is_ba2_size_512  = ( ba2.size() == 512 );
        is_ba1_size_1024 = ( ba1.size() == 1024);
    }
    int curr_cntr = QxPack::IcMemCntr::currNewCntr();
    QVERIFY2( mem_cntr == curr_cntr, "memory leak!");
    QVERIFY2( ! is_same_ptr, "pointer is same!" );
    QVERIFY2( is_ba1_size_1024, "byte array 1 is not 1024" );
    QVERIFY2( is_ba2_size_512,  "byte array 2 is not 512" );
}

// ==================================================
// resize, ptr to const string
// ==================================================
void    Test :: resize_PtrToConstStr()
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    const char *user_data = "show me the money!";
    bool is_diff_ptr = false;
    int resized_len = 0;  bool is_content_ok = false;
    {
        QxPack::IcByteArray ba = QxPack::IcByteArray::fromConstData( user_data );
        ba.resize(4);
        is_diff_ptr = ( ba.constData() != user_data );
        resized_len = int( std::strlen( ba.constData()) );
        is_content_ok = (
            std::memcmp( user_data, ba.constData(), size_t(resized_len )) == 0
        );
    }
    QxPack::IcByteArray::cleanupCache();
    int curr_cntr = QxPack::IcMemCntr::currNewCntr();
    QVERIFY2( mem_cntr == curr_cntr, "memory leak!");
    QVERIFY2( is_diff_ptr, "resize() should allocate another memory.");
    QVERIFY2( resized_len == 4, "after resize, the string length should be 4" );
    QVERIFY2( is_content_ok, "after resize, content should be 'show'");
}

// ==================================================
// resize, ptr to const string
// ==================================================
void    Test :: resize_PtrToConstStrCleanup()
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    const char *user_data = "show me the money!";
    bool is_diff_ptr = false;
    int resized_len = 0;  bool is_content_ok = false;
    bool is_cleanup_called = false;
    {
        QxPack::IcByteArray ba =
            QxPack::IcByteArray::fromConstData( user_data, -1,
                []( void *info ) {
                    * T_PtrCast( bool *, info ) = true;
                }, & is_cleanup_called
            );
        ba.resize(4);
        is_diff_ptr = ( ba.constData() != user_data );
        resized_len = int( std::strlen( ba.constData()));
        is_content_ok = (
            std::memcmp( user_data, ba.constData(), size_t( resized_len )) == 0
        );
    }
    QxPack::IcByteArray::cleanupCache();
    int curr_cntr = QxPack::IcMemCntr::currNewCntr();
    QVERIFY2( mem_cntr == curr_cntr, "memory leak!");
    QVERIFY2( is_diff_ptr, "resize() should allocate another memory.");
    QVERIFY2( resized_len == 4, "after resize, the string length should be 4" );
    QVERIFY2( is_content_ok, "after resize, content should be 'show'");
    QVERIFY2( is_cleanup_called, "the cleanup function should be called.");
}

// ==================================================
// resize, ptr to string
// ==================================================
void    Test :: resize_PtrToStrCleanup()
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    const char *user_data = "show me the money!";
    bool is_diff_ptr = false;
    int resized_len = 0;  bool is_content_ok = false;
    bool is_cleanup_called = false;
    {
        QxPack::IcByteArray ba =
            QxPack::IcByteArray::fromConstData( T_PtrCast( char *, user_data ), -1,
                []( void *info ) {
                    * T_PtrCast( bool *, info ) = true;
                }, & is_cleanup_called
            );
        ba.resize(4);
        is_diff_ptr = ( ba.constData() != user_data );
        resized_len = int( std::strlen( ba.constData()));
        is_content_ok = (
            std::memcmp( user_data, ba.constData(), size_t( resized_len )) == 0
        );
    }
    QxPack::IcByteArray::cleanupCache();
    int curr_cntr = QxPack::IcMemCntr::currNewCntr();
    QVERIFY2( mem_cntr == curr_cntr, "memory leak!");
    QVERIFY2( is_diff_ptr, "resize() should allocate another memory.");
    QVERIFY2( resized_len == 4, "after resize, the string length should be 4" );
    QVERIFY2( is_content_ok, "after resize, content should be 'show'");
    QVERIFY2( is_cleanup_called, "the cleanup function should be called.");
}

// ==================================================
// data pointer in shared objects
// ==================================================
void   Test :: dataPtr_InShared()
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    void *ptr0, *ptr1, *ptr2;
    {
        QxPack::IcByteArray ba1(1024);
        QxPack::IcByteArray ba2 = ba1;
        ptr0 = T_PtrCast( void*, ba1.constData());
        ptr1 = ba1.data();
        ptr2 = ba2.data();
    }
    int curr_cntr = QxPack::IcMemCntr::currNewCntr();
    QVERIFY2( mem_cntr == curr_cntr, "memory leak!" );
    QVERIFY2( ptr1 != ptr2, "data() should return diffirent pointer in shared objects." );
    QVERIFY2( ptr0 == ptr2, "after called data(), the remain object should not changed." );
}

// =================================================
// data pointer, point to const string
// =================================================
void   Test :: dataPtr_PtrToConstStr()
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    const char *user_data = "show me the money!";
    void *ptr0, *ptr1;
    {
        QxPack::IcByteArray ba1 =
            QxPack::IcByteArray::fromConstData( user_data );
        ptr0 = T_PtrCast( void*, ba1.constData());
        ptr1 = ba1.data();
    }
    QxPack::IcByteArray::cleanupCache();
    int curr_cntr = QxPack::IcMemCntr::currNewCntr();
    QVERIFY2( mem_cntr == curr_cntr, "memory leak!" );
    QVERIFY2( ptr0 != ptr1, "data() should return diffirent pointer while object point to const string." );
}

// =================================================
// data pointer, point to const string and cleanup
// =================================================
void    Test :: dataPtr_PtrToConstStrCleanup()
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    const char *user_data = "show me the money!";
    bool is_cleanup_called = false;
    void *ptr0, *ptr1;
    {
        QxPack::IcByteArray ba1 =
            QxPack::IcByteArray::fromConstData (
                user_data, -1,
                [](void *info) {
                    * T_PtrCast( bool *, info ) = true;
                }, & is_cleanup_called
            );
        ptr0 = T_PtrCast( void*, ba1.constData() );
        ptr1 = ba1.data();
    }
    QxPack::IcByteArray::cleanupCache();
    int curr_cntr = QxPack::IcMemCntr::currNewCntr();
    QVERIFY2( mem_cntr == curr_cntr, "memory leak!" );
    QVERIFY2( ptr0 != ptr1, "data() should return diffirent pointer while object point to const string." );
    QVERIFY2( is_cleanup_called, "cleanup not called!");
}

// =================================================
// data pointer, point to const string and cleanup
// =================================================
void    Test :: dataPtr_PtrToStrCleanup()
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    const char *user_data = "show me the money!";
    bool is_cleanup_called = false;
    void *ptr0, *ptr1;
    {
        QxPack::IcByteArray ba1 =
            QxPack::IcByteArray::fromData (
                T_PtrCast( char *, user_data ), -1,
                [](void *info) {
                    * T_PtrCast( bool *, info ) = true;
                }, & is_cleanup_called
            );
        ptr0 = T_PtrCast( void*, ba1.constData());
        ptr1 = ba1.data();
    }
    QxPack::IcByteArray::cleanupCache();
    int curr_cntr = QxPack::IcMemCntr::currNewCntr();
    QVERIFY2( mem_cntr == curr_cntr, "memory leak!" );
    QVERIFY2( ptr0 == ptr1, "pointer should be same while point to writable user data." );
    QVERIFY2( is_cleanup_called, "cleanup not called!");
}

// =================================================
// shared test, same pointer
// =================================================
void   Test :: sharedTestSamePtr ( )
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    const void *ptr1, *ptr2;
    {
        QxPack::IcByteArray ba1(1024);
        ptr1 = ba1.constData();
        QxPack::IcByteArray ba2 = ba1;
        ptr2 = ba2.constData();
    }
    int curr_cntr = QxPack::IcMemCntr::currNewCntr();
    QVERIFY2( mem_cntr == curr_cntr, "memory leak!" );
    QVERIFY2( ptr1 == ptr2, "data pointers not equal!");
}

// =================================================
// shared test, Copy on write
// =================================================
void    Test :: sharedTestCow()
{

}



// =================================================
// test the timer
// =================================================
void   Test :: searchInMap()
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    bool is_found_in_map = false;
    {
        std::map<QxPack::IcByteArray, const char* >  map;
        map.insert( std::pair< QxPack::IcByteArray, const char* >( QxPack::IcByteArray::fromConstData("abc"),   "def1"));
        map.insert( std::pair< QxPack::IcByteArray, const char* >( QxPack::IcByteArray::fromConstData("deef"),  "def2"));
        map.insert( std::pair< QxPack::IcByteArray, const char* >( QxPack::IcByteArray::fromConstData("deef3"), "def3"));
        map.insert( std::pair< QxPack::IcByteArray, const char* >( QxPack::IcByteArray::fromConstData("deef33"), "def33"));

        std::map<QxPack::IcByteArray, const char*>::iterator itr = map.find( QxPack::IcByteArray::fromConstData("deef3"));
        if ( itr != map.end()) {
            qInfo("found the data key=%s, value=%s", itr->first.constData(), itr->second );
            is_found_in_map = true;
        }
        {
            std::map<QxPack::IcByteArray, const char*> tmp;
            map.swap( tmp );
        }
        QxPack::IcByteArray::cleanupCache(); // call it to cleanup cache
    }
    QVERIFY2( is_found_in_map, "Could not find object in the map!" );
    QVERIFY2( mem_cntr == QxPack::IcMemCntr::currNewCntr(), "detected memory leak!");
}




QTEST_MAIN( Test )
#include "main.moc"
