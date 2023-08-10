#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <memory>
#include <cassert>
#include <random>
#include "../qxpack_sc_memblock.hxx"

#define VERIFY( c, s ) do{ if( !(c) ) { std::printf("Ln(%d) failed! %s\n", __LINE__, s ); std::abort(); }}while(0)

// ////////////////////////////////////////////////////////////////////////////
//  helper
// ////////////////////////////////////////////////////////////////////////////
//static void prnMbInfo( const QxPack::ScMemBlock &mb )
//{
//    std::printf("== the MemBlock Info ==\n");
//    std::printf(" object addr : 0x%x\n", intptr_t(&mb));
//    std::printf(" isNull  : %s \n", mb.isNull() ? "true" : "false");
//    std::printf(" size    : %u \n", mb.size());
//    std::printf(" dataPtr : 0x%x\n", intptr_t(mb.constData()));
//}

static void  randInitMem( uint8_t *dp, size_t n )
{
    std::default_random_engine eng;
    std::uniform_int_distribution<unsigned> u(0, 255);
    for ( size_t i = 0; i < n; i ++ ) { dp[i] = uint8_t(u(eng));  }
}


// ////////////////////////////////////////////////////////////////////////////
//  ScMemBlock test functions
// ////////////////////////////////////////////////////////////////////////////
static void  test_MB_ctor()
{
    std::printf( "%s : ", __FUNCTION__ );
    { // normal ctor
        QxPack::ScMemBlock  mb(100);
        VERIFY( !mb.isNull(), "mb(100) is null!\n" );
    }
    { // empty ctor
        QxPack::ScMemBlock  mb;
        VERIFY( mb.isNull(), "mb is not null!\n");
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "memory leak!" );
    std::printf("passed\n");
}

static void  test_MB_ctorCopy()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
        QxPack::ScMemBlock  mb(100);
        QxPack::ScMemBlock  mb2( mb ); // construct copy
        VERIFY( mb.constData() == mb2.constData(), "ctor(obj) do nothing!" );
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "memory leak!");
    std::printf("passed\n");
}

static void  test_MB_assignCopy()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
        QxPack::ScMemBlock  mb(100);
        QxPack::ScMemBlock  mb2 = mb; // copy assign
        VERIFY( mb.constData() == mb2.constData(), "assign copy do nothing!" );
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "memory leak!" );
    std::printf("passed\n");
}

static void  test_MB_copy()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
        QxPack::ScMemBlock mb(100);
        randInitMem( reinterpret_cast<uint8_t*>( mb.dataCow()), mb.size() );
        QxPack::ScMemBlock mb2 = mb.copy();
        VERIFY( mb.constData() != mb2.constData(), "copy() do nothing!");
        VERIFY( std::memcmp( mb.constData(), mb2.constData(), mb.size() ) == 0, "the copied content is not same!" );
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "memory leak!" );
    std::printf("passed\n");
}

static void  test_MB_dupIfShared()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
        QxPack::ScMemBlock mb(100);
        randInitMem( reinterpret_cast<uint8_t*>( mb.dataCow()), mb.size() );
        QxPack::ScMemBlock mb2 = mb;
        bool ret = mb2.dupIfShared(); // mb2 make a copy
        VERIFY( ret, "mb2 duplicates do nothing!");
        VERIFY( mb2.constData() != mb.constData(), "mb2 duplicates failed!");
        VERIFY( std::memcmp( mb.constData(), mb2.constData(), mb.size()) == 0, "the " );
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "memory leak!" );
    std::printf("passed\n");
}

// since 0.1.0
static void  test_MB_dataCow()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
        QxPack::ScMemBlock mb(200);
        randInitMem( reinterpret_cast<uint8_t*>( mb.dataCow()), mb.size());
        const void* cptr = mb.constData();
        void *ptr1 = mb.dataCow();
        VERIFY( cptr == ptr1, "mb do a copy but it's not shared with other!");

        QxPack::ScMemBlock mb2(mb);
        void *ptr2 = mb.dataCow();
        VERIFY( cptr != ptr2, "mb should do a copy but do nothing!" );
        VERIFY( std::memcmp( mb.constData(), mb2.constData(), mb.size() ) == 0, "mb2 copied from mb, but not same!" );
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "memory leak!" );
    std::printf("passed\n");
}

// since 0.2.0
static void  test_MB_resize()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
        QxPack::ScMemBlock tmp(100);
        randInitMem( reinterpret_cast<uint8_t*>( tmp.dataCow()), tmp.size() );

        QxPack::ScMemBlock mb(100);
        std::memcpy( mb.dataCow(), tmp.dataCow(), 100 );
        const void *org_ptr = mb.constData();
        size_t org_sz = mb.size();

        mb.resize(200);
        const void *gt_ptr = mb.constData();
        size_t gt_sz = mb.size();
        VERIFY( org_ptr != gt_ptr, "the mb called resize(200), but nothing happen!" );
        VERIFY( org_sz == 100 && gt_sz == 200, "the mb called resize(200), but the size is not 200.");
        VERIFY( std::memcmp( gt_ptr, tmp.constData(), 100 ) == 0, "mb called resize(200), but do not copy the old content!" );

        mb.resize(50);
        const void *lt_ptr = mb.constData();
        size_t lt_sz = mb.size();
        VERIFY( gt_ptr != lt_ptr, "the mb resize shrink to 50, but nothing happend!");
        VERIFY( gt_sz == 200 && lt_sz == 50, "the mb resized shrink to 50, but the size is not meet condition.");
        VERIFY( std::memcmp( lt_ptr, tmp.constData(), 50 ) == 0, "mb called resize(50), but do not copy the old content!" );

        mb.resize(0);
        const void *z_ptr = mb.constData();
        size_t z_sz = mb.size();
        VERIFY( mb.isNull(), "the mb resize to 0, should be null!");
        VERIFY( z_ptr == nullptr, "the mb resize to 0, the pointer should be nullptr!");
        VERIFY( z_sz  == 0, "the mb resize to 0, the size should be 0");
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "memory leak!" );
    std::printf("passed\n");
}

// since 0.3.0
static void  test_MB_resize_shared()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
        QxPack::ScMemBlock mb(100);
        randInitMem( reinterpret_cast<uint8_t*>( mb.dataCow()), mb.size() );

        QxPack::ScMemBlock mb2( mb );
        VERIFY( mb.constData() == mb2.constData(), "mb2(mb), mb2 is not shared with mb!" );

        const void *org_ptr = mb.constData();
        mb2.resize(200);
        const void *gt_ptr  = mb2.constData();

        size_t  gt_sz   = mb2.size();
        VERIFY( org_ptr != gt_ptr, "resize failed!");
        VERIFY( gt_sz == 200, "mb2 resize to 200 failed!");
        VERIFY( std::memcmp( mb2.constData(), mb.constData(), 100 ) == 0, "the 100 bytes content is not same!"  );
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "memory leak!" );
    std::printf("passed\n");
}


// ////////////////////////////////////////////////////////////////////////////
//
//                      Entry
//
// ////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    (void)(argc); (void)(argv);

    test_MB_ctor();
    test_MB_ctorCopy();
    test_MB_assignCopy();
    test_MB_copy();
    test_MB_dupIfShared();
    test_MB_dataCow();
    test_MB_resize();
    test_MB_resize_shared();

    std::printf("\n last mem cntr: %d\n", QxPack::ScMemBlock::currMemCntr() );
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "memory leak!");
    return 0;
}
