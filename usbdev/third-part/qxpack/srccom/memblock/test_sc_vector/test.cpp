#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <memory>
#include <cassert>
#include <random>
#include "../qxpack_sc_memblock.hxx"
#include "../qxpack_sc_vector.hpp"

#define VERIFY( c, s ) do{ if( !(c) ) { std::printf("Ln(%d) failed! %s\n", __LINE__, s ); std::exit(-1); }}while(0)

//#define ENABLE_MYCLASS_HINT
#define ENABLE_MYCLASS_MOVE_CTOR
#define ENABLE_MYCLASS_MOVE_ASSIGN

// ////////////////////////////////////////////////////////////////////////////
//  helper class
// ////////////////////////////////////////////////////////////////////////////
static int g_mem_cntr = 0;
#ifdef ENABLE_MYCLASS_HINT
# define MYCLASS_CTOR       do{ std::printf("ctor\n"); }while(0)
# define MYCLASS_CTOR_N     do{ std::printf("ctor(n)\n"); }while(0)
# define MYCLASS_DTOR       do{ std::printf("dtor\n"); }while(0)
# define MYCLASS_CTOR_COPY  do{ std::printf("ctor(copy)\n"); }while(0)
# define MYCLASS_ASSIGN     do{ std::printf("assign\n"); }while(0)
# define MYCLASS_MOVE_CTOR  do{ std::printf("move ctor\n"); }while(0)
# define MYCLASS_MOVE_ASSIGN do{ std::printf("move assign\n"); }while(0)
#else
# define MYCLASS_CTOR
# define MYCLASS_CTOR_N
# define MYCLASS_DTOR
# define MYCLASS_CTOR_COPY
# define MYCLASS_ASSIGN
# define MYCLASS_MOVE_CTOR
# define MYCLASS_MOVE_ASSIGN
#endif

class MyClass {
private:
    int m_val; void *m_ptr;
public :
    explicit MyClass( )
    { ++ g_mem_cntr; m_val = 0; m_ptr = std::malloc(64); MYCLASS_CTOR; }

    MyClass( int n )
    { ++ g_mem_cntr; m_val = n; m_ptr = std::malloc(64); MYCLASS_CTOR_N; }

    MyClass( const MyClass &ot )
    { m_val = ot.m_val; ++ g_mem_cntr; m_ptr = std::malloc(64); MYCLASS_CTOR_COPY; }

    MyClass & operator = ( const MyClass &ot )
    {
        m_val = ot.m_val; MYCLASS_ASSIGN;
        return *this;
    }

    ~MyClass() {
        -- g_mem_cntr;
        std::free( m_ptr );
        m_ptr = nullptr; MYCLASS_DTOR;
    }

    MyClass ( MyClass && ot )
    {
      m_val = ot.m_val; ot.m_val = 0; ++ g_mem_cntr; MYCLASS_MOVE_CTOR;
    }

    MyClass & operator = ( MyClass && ot )
    {
       m_val = ot.m_val;  ot.m_val = 0;
       MYCLASS_MOVE_ASSIGN;
       return *this;
    }

    inline int&  value() { return m_val; }
    inline const int& value() const { return m_val; }

    bool operator == ( const MyClass &t ) const
    { return m_val == t.m_val; }
};

static void  randInitMem( int *dp, size_t n )
{
    std::default_random_engine eng;
    std::uniform_int_distribution<int> u( 0, INT_MAX/2 );
    for ( size_t i = 0; i < n; i ++ ) { dp[i] = int(u(eng));  }
}

static void  randInitMemClass( MyClass *dp, size_t n )
{
    std::default_random_engine eng;
    std::uniform_int_distribution<int> u( 0, INT_MAX/2 );
    for ( size_t i = 0; i < n; i ++ ) { dp[i].value() = int(u(eng));  }
}

static int   sumMem(const int *dp, size_t n )
{
    int sum = 0;
    for ( size_t i = 0; i < n; i ++ ) { sum += dp[i]; }
    return sum;
}

static int  sumMemClass( const MyClass *dp, size_t n )
{
    int sum = 0;
    for ( size_t i = 0; i < n; i ++ ) { sum += dp[i].value(); }
    return sum;
}

static void  prnMyClass( const MyClass *d, size_t n )
{
    std::printf("\n");
    for ( size_t i = 0; i < n; i ++ ) {
        std::printf("%d,", d[i].value());
    }
    std::printf("\n");
}

static void  prnMem( const int *d, size_t n )
{
    std::printf("\n");
    for ( size_t i = 0; i < n; i ++ ) {
        std::printf("%d,", d[i] );
    }
    std::printf("\n");
}

// ////////////////////////////////////////////////////////////////////////////
//
//                      ScVector test ( non-class data )
//
// ////////////////////////////////////////////////////////////////////////////
static void  test_VEC_ctor()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
        QxPack::ScVector<int> vec(16);
        VERIFY( !vec.isNull(), "ctor do nothing!");
    }
    {
        QxPack::ScVector<int> vec;
        VERIFY( vec.isNull(), "ctor empty failed!");
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    std::printf("passed\n");
}

static void  test_VEC_ctorCopy()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
        QxPack::ScVector<int>  vec(100);
        QxPack::ScVector<int>  vec2( vec );
        VERIFY( vec.constData() == vec2.constData(), "ctor(obj) do nothing!" );
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    std::printf("passed\n");
}

static void  test_VEC_assignCopy()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
        QxPack::ScVector<int>  vec(100);
        QxPack::ScVector<int>  vec2 = vec;
        VERIFY( vec.constData() == vec2.constData(), "assign copy do nothing!" );
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    std::printf("passed\n");
}

static void  test_VEC_dataCow()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
        QxPack::ScVector<double> vec(100);
        const double* cptr = vec.constData();
        double *ptr1 = vec.dataCow();
        VERIFY( cptr == ptr1, "vec called dataCow() and changed ptr, but it's not shared with other!");

        QxPack::ScVector<double> vec2( vec );
        double *ptr2 = vec2.dataCow();
        VERIFY( cptr != ptr2, "vec2 should d a copy due to it shared with other!" );
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    std::printf("passed\n");
}

static void  test_VEC_copy()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
        QxPack::ScVector<int> vec(9);
        for ( int i = 0, imax = int(vec.dim()); i < imax; i ++ ) {
            vec[i] = i;
        }
        QxPack::ScVector<int> vec2 = vec.copy(); // DO COPY
        VERIFY( vec.constData() != vec2.constData(), "copy() do nothing!");
        VERIFY( std::memcmp( vec.constData(), vec2.constData(), sizeof(int)*vec.size()) == 0, "copied but content not same!" );
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    std::printf("passed\n");
}

static void  test_VEC_constRead()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
        QxPack::ScVector<int> vec(9);
        for ( int i = 0, imax = int(vec.dim()); i < imax; i ++ ) {
            vec[i] = i;
        }
        QxPack::ScVector<int> vec2( vec ); // make a shared object

        const QxPack::ScVector<int> &vec_const = vec;
        const int *old_ptr = vec2.constData();
        for ( int i = 0, imax = int(vec.dim()); i < imax; i ++ ) {
            const int &n = vec_const[i];
            int n2       = vec_const[i];
            const int *new_ptr = vec.constData();
            VERIFY( n == i && n2 == i, "the content is bad!" );
            VERIFY( old_ptr == new_ptr, "const read data failed, a copy generated!");
        }
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    std::printf("passed\n");
}

static void  test_VEC_push_back()
{
    std::printf( "%s : ", __FUNCTION__ );
    { // push back data
        QxPack::ScVector<int> vec(4);
        size_t c1 = vec.capacity();
        vec.push_back( 33 );
        size_t c2 = vec.capacity();
        VERIFY( c2 > c1, "ScVector<int>::push_back should change capacity if full!");
    }
    { // push back the zero vector
        QxPack::ScVector<int> vec;
        vec.push_back( 99 );
        VERIFY( vec.size() == 1, "ScVector<int>::push_back to build a data failed!");
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    std::printf("passed\n");
}

static void  test_VEC_pop_back()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
        QxPack::ScVector<int> vec(3);
        vec.push_back( 66 );
        vec.pop_back();
        VERIFY( vec.size() == 3, "pop_back() do nothing!" );
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    std::printf("passed\n");
}

static void  test_VEC_push_front()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
        QxPack::ScVector<int> vec(3);
        randInitMem( vec.dataCow(), 3 );
        int sum = sumMem( vec.constData(), 3 );
        vec.push_front( 99 );
        VERIFY( vec.size() == 4, "push_front() not worked!" );
        VERIFY( vec.front() == 99, "push_front() pushed bad value!");
        int sum2 = sumMem( vec.constData()+1,3);
        VERIFY( sum2 == sum, "push_front() do not copy correct content!");
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    std::printf("passed\n");
}

static void  test_VEC_pop_front()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
        QxPack::ScVector<int> vec(3);
        randInitMem( vec.dataCow(), 3 );
        int f1 = vec.at(1);
        int f2 = vec.at(2);
        vec.pop_front();
        VERIFY( vec.front() == f1, "element not moved!" );
        vec.pop_front();
        VERIFY( vec.front() == f2, "element not moved!" );
        vec.pop_front();
        VERIFY( vec.empty(), "element not empty!");
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    std::printf("passed\n");
}

static void  test_VEC_front()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
        QxPack::ScVector<int> vec(4);
        vec.push_back( 77 );
        VERIFY( vec.front() != 77, "front() position is not right!");
        vec.push_front( 22 );
        VERIFY( vec.front() == 22, "front() position is not right!");
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    std::printf("passed\n");
}

static void  test_VEC_back()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
        QxPack::ScVector<int> vec(4);
        vec.push_back( 77 );
        vec.push_back( 99 );
        VERIFY( vec.back() == 99, "back() access failed!");
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    std::printf("passed\n");
}

static void  test_VEC_empty()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
        QxPack::ScVector<int> vec;
        VERIFY( vec.empty(), "build an empty vector but not empty!");
    }
    {
        QxPack::ScVector<int> vec(2);
        vec.pop_back();
        vec.pop_back();
        VERIFY( vec.empty(), "pop( pop_back ) all elements, but vector is not empty!");
    }
    {
        QxPack::ScVector<int> vec(10);
        for ( int i = 0; i < 10; i ++ ) { vec.pop_front(); }
        VERIFY( vec.empty(), "pop( pop_front ) all elements, but vector is not empty!");
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    std::printf("passed\n");
}

static void  test_VEC_reserve()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
        QxPack::ScVector<int> vec;
        vec.reserve(10);
        VERIFY( vec.size() == 0, "vec reserved, the elem. count should be 0!");
        VERIFY( vec.capacity() == 10, "vec.reserve(10), but capacity is not equal to 10!");
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    std::printf("passed\n");
}

static void  test_VEC_resize()
{
    std::printf( "%s : ", __FUNCTION__ );
    { // resize to large
        QxPack::ScVector<int> vec(2);
        vec.resize(8);
        VERIFY( vec.size() == 8, "vector resized (8), but not changed anything!");
    }
    { // resize to small
        QxPack::ScVector<int> vec(6);
        vec.resize(2);
        VERIFY( vec.size() == 2, "vector resized (2), but not changed anything!");
    }
    { // resize to 0
        QxPack::ScVector<int> vec(10);
        vec.resize(0);
        VERIFY( vec.isNull(), "vector resize to 0, but not drop content!");
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    std::printf("passed\n");
}

static void  test_VEC_insert()
{
    std::printf( "%s : ", __FUNCTION__ );
    { // insert at 0
        QxPack::ScVector<int> vec(3);
        randInitMem( vec.dataCow(), vec.size() );
        int sum = sumMem( vec.constData(), vec.size() );
        size_t old_sz = vec.size();
        vec.insert( 0, 22 ); vec.insert( 0, 33 );
        int sum2 = sumMem( &vec.constData()[2], old_sz );
        VERIFY( vec.size() == old_sz + 2, "insert at 0 failed!" );
        VERIFY( sum == sum2, "after insert, content is broken!" );
        VERIFY( vec.at(0) == 33  && vec.at(1) == 22, "after inserted, the order is broken!");
    }
    { // insert at middle
        QxPack::ScVector<int> vec(5);
        randInitMem( vec.dataCow(), vec.size() );
        size_t  rem_sz = vec.size() - 2;
        int sum  = sumMem( &vec.constData()[2] , rem_sz );
        vec.insert( 1, 99 ); vec.insert( 2, 100 );
        int sum2 = sumMem( &vec.constData()[4],  rem_sz );
        VERIFY( vec.size() == 7, "insert at middle failed!");
        VERIFY( sum == sum2, "insert broke the content!" );
        VERIFY( vec.at(1) == 99 && vec.at(2) == 100, "after inserted, the order is broken!");
    }
    { // insert at last
        QxPack::ScVector<int> vec(3);
        randInitMem( vec.dataCow(), vec.size());
        int sum = sumMem( vec.constData(), vec.size() );
        size_t old_sz = vec.size();
        vec.insert( vec.size(), 22 ); vec.insert( vec.size(), 33 );
        int sum2 = sumMem( vec.constData(), old_sz );
        VERIFY( vec.size() == old_sz + 2, "insert at 0 failed!" );
        VERIFY( sum == sum2, "after insert, content is broken!" );
        VERIFY( vec.at( vec.size() - 1 ) == 33  && vec.at( vec.size() - 2 ) == 22, "after inserted, the order is broken!");
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    std::printf("passed\n");
}

static void  test_VEC_remove( )
{
    std::printf( "%s : ", __FUNCTION__ );
    { // remove at 0
        QxPack::ScVector<int> vec(3);
        randInitMem( vec.dataCow(), vec.size() );
        size_t old_sz = vec.size();
       // prnMem( vec.constData(), vec.size());
        int sum = sumMem( vec.constData() + 1,  old_sz - 1 );
        vec.remove( 0 );
       // prnMem( vec.constData(), vec.size());
        int sum2= sumMem( vec.constData(), old_sz - 1 );
        VERIFY( vec.size() == 2, "remove failed!" );
        VERIFY( sum == sum2, "after removed, content changed!" );
    }
    { // remove middle
        QxPack::ScVector<int> vec(5);
        randInitMem( vec.dataCow(), vec.size() );
        size_t  rem_sz = vec.size() - 2;
        int sum  = sumMem( &vec.constData()[2] , rem_sz );
        vec.remove(1);
        int sum2 = sumMem( &vec.constData()[1],  rem_sz );
        VERIFY( vec.size() == 4, "insert at middle failed!");
        VERIFY( sum == sum2, "insert broke the content!" );
    }
    { // remove last
        QxPack::ScVector<int> vec(3);
        randInitMem( vec.dataCow(), vec.size());
        size_t old_sz = vec.size();
        vec.remove(vec.size()-1);
        VERIFY( vec.size() == old_sz - 1, "remove at last failed!" );
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    std::printf("passed\n");
}


// ////////////////////////////////////////////////////////////////////////////
//
//                      ScVector test ( class data )
//
// ////////////////////////////////////////////////////////////////////////////
static void  test_VEC_ctor_class()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
        QxPack::ScVector<MyClass> vec(2);
        VERIFY( !vec.isNull(), "ctor do nothing!");
    }
    VERIFY( g_mem_cntr == 0, "MyClass Refr. cntr != 0!" );
    {
        QxPack::ScVector<MyClass> vec;
        VERIFY( vec.isNull(), "ctor empty failed!");
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    VERIFY( g_mem_cntr == 0, "MyClass Refr. cntr != 0!" );
    std::printf("passed\n");
}

static void  test_VEC_ctorCopy_class()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
        QxPack::ScVector<MyClass>  vec(10);
        QxPack::ScVector<MyClass>  vec2( vec );
        VERIFY( vec.constData() == vec2.constData(), "ctor(obj) do nothing!" );
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    VERIFY( g_mem_cntr == 0, "MyClass Refr. cntr != 0!" );
    std::printf("passed\n");
}

static void  test_VEC_assignCopy_class()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
        QxPack::ScVector<MyClass>  vec(10);
        QxPack::ScVector<MyClass>  vec2 = vec;
        VERIFY( vec.constData() == vec2.constData(), "assign copy do nothing!" );
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    VERIFY( g_mem_cntr == 0, "MyClass Refr. cntr != 0!" );
    std::printf("passed\n");
}

static void  test_VEC_dataCow_class()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
        QxPack::ScVector<MyClass> vec(10);
        const MyClass* cptr = vec.constData();
        MyClass *ptr1 = vec.dataCow();
        VERIFY( cptr == ptr1, "vec called dataCow() and changed ptr, but it's not shared with other!");

        QxPack::ScVector<MyClass> vec2( vec );
        MyClass *ptr2 = vec2.dataCow();
        VERIFY( cptr != ptr2, "vec2 should d a copy due to it shared with other!" );
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    VERIFY( g_mem_cntr == 0, "MyClass Refr. cntr != 0!" );
    std::printf("passed\n");
}

static void  test_VEC_copy_class()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
        QxPack::ScVector<MyClass> vec(6);
        for ( int i = 0, imax = int(vec.dim()); i < imax; i ++ ) {
            vec[i] = MyClass(i);
        }
        QxPack::ScVector<MyClass> vec2 = vec.copy(); // DO COPY
        VERIFY( vec.constData() != vec2.constData(), "copy() do nothing!");

        const QxPack::ScVector<MyClass> &vec_const  = vec;
        const QxPack::ScVector<MyClass> &vec2_const = vec2;
        for ( int i = 0, imax = int(vec.dim()); i < imax; i ++ ) {
            VERIFY( vec_const[i] == vec2_const[i], "vec and vec2 compare failed!");
        }
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    VERIFY( g_mem_cntr == 0, "MyClass Refr. cntr != 0!" );
    std::printf("passed\n");
}

static void  test_VEC_constRead_class()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
        QxPack::ScVector<MyClass> vec(9);
        for ( int i = 0, imax = int(vec.dim()); i < imax; i ++ ) {
            vec[i] = MyClass(i);
        }
        QxPack::ScVector<MyClass> vec2( vec ); // make a shared object

        const QxPack::ScVector<MyClass> &vec_const = vec;
        const MyClass *old_ptr = vec2.constData();
        for ( int i = 0, imax = int(vec.dim()); i < imax; i ++ ) {
            const MyClass &n = vec_const[i];
            MyClass n2       = vec_const[i];
            const MyClass *new_ptr = vec.constData();
            VERIFY( n.value() == i &&  n2.value() == i , "the content is bad!" );
            VERIFY( old_ptr == new_ptr, "const read data failed, a copy generated!");
        }
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    VERIFY( g_mem_cntr == 0, "MyClass Refr. cntr != 0!" );
    std::printf("passed\n");
}

// since fix_step == 3
static void  test_VEC_push_back_class()
{
    std::printf( "%s : ", __FUNCTION__ );
    { // push back data
        QxPack::ScVector<MyClass> vec(4);
        size_t c1 = vec.capacity();
        vec.push_back( MyClass(44) );
        size_t c2 = vec.capacity();
        VERIFY( c2 > c1, "ScVector<int>::push_back should change capacity if full!");
    }
    { // push back the zero vector
        QxPack::ScVector<MyClass> vec;
        vec.push_back( MyClass(55) );
        VERIFY( vec.size() == 1, "ScVector<int>::push_back to build a data failed!");
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    VERIFY( g_mem_cntr == 0, "MyClass Refr. cntr != 0!" );
    std::printf("passed\n");
}

// fix_step == 3
static void  test_VEC_pop_back_class()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
        QxPack::ScVector<MyClass> vec(3);
        vec.push_back( MyClass(66) );
        vec.pop_back();
        VERIFY( vec.size() == 3, "pop_back do nothing!" );
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    VERIFY( g_mem_cntr == 0, "MyClass Refr. cntr != 0!" );
    std::printf("passed\n");
}

// fix_step == 3
static void  test_VEC_push_front_class()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
        QxPack::ScVector<MyClass> vec(3);
        randInitMemClass(  vec.dataCow(), vec.size() );
        size_t old_size = vec.size();
        int sum = sumMemClass( vec.constData(), old_size );
        vec.push_front( MyClass( 99 ) );
        vec.push_front( MyClass( 88 ) );
        VERIFY( vec.size() == 5, "push_front() not worked!" );
        VERIFY( vec.front().value() == 88, "push_front() pushed bad value!");
        int sum2 = sumMemClass( vec.constData()+2,old_size);
        VERIFY( sum2 == sum, "push_front() do not copy correct content!");
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    VERIFY( g_mem_cntr == 0, "MyClass Refr. cntr != 0!" );
    std::printf("passed\n");
}

// fix step == 3
static void  test_VEC_pop_front_class()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
        QxPack::ScVector<MyClass> vec(3);
        randInitMemClass( vec.dataCow(), 3 );
        int f1 = vec.at(1).value();
        int f2 = vec.at(2).value();
        vec.pop_front();
        VERIFY( vec.front().value() == f1, "element not moved!" );
        vec.pop_front();
        VERIFY( vec.front().value() == f2, "element not moved!" );
        vec.pop_front();
        VERIFY( vec.empty(), "element not empty!");
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    std::printf("passed\n");
}



// fix_step == 3
static void  test_VEC_front_class()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
        QxPack::ScVector<MyClass> vec(4);
        vec.push_back( MyClass(77) );
        VERIFY( vec.front().value() != 77, "front() position is not right!");
        vec.push_front( MyClass(22) );
        VERIFY( vec.front().value() == 22, "front() position is not right!");
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    VERIFY( g_mem_cntr == 0, "MyClass Refr. cntr != 0!" );
    std::printf("passed\n");
}

// fix_step == 3
static void  test_VEC_back_class()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
        QxPack::ScVector<MyClass> vec(4);
        vec.push_back( MyClass(77) );
        vec.push_back( MyClass(99) );
        VERIFY( vec.back().value() == 99, "back() access failed!");
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    VERIFY( g_mem_cntr == 0, "MyClass Refr. cntr != 0!" );
    std::printf("passed\n");
}

// fix_step == 3
static void  test_VEC_empty_class()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
        QxPack::ScVector<MyClass> vec;
        VERIFY( vec.empty(), "build an empty vector but not empty!");
        VERIFY( g_mem_cntr == 0, "MyClass Refr. cntr != 0!" );
    }
    {
        QxPack::ScVector<MyClass> vec(2);
        vec.pop_back();
        vec.pop_back();
        VERIFY( vec.empty(), "pop( pop_back ) all elements, but vector is not empty!");
        VERIFY( g_mem_cntr == 0, "MyClass Refr. cntr != 0!" );
    }
    {
        QxPack::ScVector<MyClass> vec(10);
        for ( int i = 0; i < 10; i ++ ) { vec.pop_front(); }
        VERIFY( vec.empty(), "pop( pop_front ) all elements, but vector is not empty!");
        VERIFY( g_mem_cntr == 0, "MyClass Refr. cntr != 0!" );
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    VERIFY( g_mem_cntr == 0, "MyClass Refr. cntr != 0!" );
    std::printf("passed\n");
}

// fix_step == 3
static void  test_VEC_reserve_class()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
        QxPack::ScVector<MyClass> vec;
        vec.reserve(10);
        VERIFY( vec.size() == 0, "vec reserved, the elem. count should be 0!");
        VERIFY( vec.capacity() == 10, "vec.reserve(10), but capacity is not equal to 10!");
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    VERIFY( g_mem_cntr == 0, "MyClass Refr. cntr != 0!" );
    std::printf("passed\n");
}

// fix_step == 3
static void  test_VEC_resize_class()
{
    std::printf( "%s : ", __FUNCTION__ );
    { // resize to large
        QxPack::ScVector<MyClass> vec(2);
        vec.resize(8);
        VERIFY( vec.size() == 8, "vector resized (8), but not changed anything!");
    }
    { // resize to small
        QxPack::ScVector<MyClass> vec(6);
        vec.resize(2);
        VERIFY( vec.size() == 2, "vector resized (2), but not changed anything!");
    }
    { // resize to 0
        QxPack::ScVector<MyClass> vec(10);
        vec.resize(0);
        VERIFY( vec.isNull(), "vector resize to 0, but not drop content!");
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    VERIFY( g_mem_cntr == 0, "MyClass Refr. cntr != 0!" );
    std::printf("passed\n");
}

static void  test_VEC_insert_class()
{
    std::printf( "%s : ", __FUNCTION__ );
    { // insert at 0
        QxPack::ScVector<MyClass> vec(3);
        randInitMemClass( vec.dataCow(), vec.size() );
        int sum = sumMemClass( vec.constData(), vec.size() );
        size_t old_sz = vec.size();
        vec.insert( 0, MyClass(22) ); vec.insert( 0, MyClass(33) );
        int sum2 = sumMemClass( &vec.constData()[2], old_sz );
        VERIFY( vec.size() == old_sz + 2, "insert at 0 failed!" );
        VERIFY( sum == sum2, "after insert, content is broken!" );
        VERIFY( vec.at(0).value() == 33  && vec.at(1).value() == 22,
                "after inserted, the order is broken!" );
    }
    { // insert at middle
        QxPack::ScVector<MyClass> vec(5);
        randInitMemClass( vec.dataCow(), vec.size() );
       // prnMyClass( vec.constData(), vec.size() );
        size_t  rem_sz = vec.size() - 2;
        int sum  = sumMemClass( &vec.constData()[2] , rem_sz );
        vec.insert( 1, MyClass(99) ); vec.insert( 2, MyClass(100) );
       // prnMyClass( vec.constData(), vec.size() );
        int sum2 = sumMemClass( &vec.constData()[4], rem_sz );
        VERIFY( vec.size() == 7, "insert at middle failed!");
        VERIFY( sum == sum2, "insert broke the content!" );
        VERIFY( vec.at(1).value() == 99 && vec.at(2).value() == 100, "after inserted, the order is broken!");
    }
    { // insert at last
        QxPack::ScVector<MyClass> vec(3);
        randInitMemClass( vec.dataCow(), vec.size());
        int sum = sumMemClass( vec.constData(), vec.size() );
        size_t old_sz = vec.size();
        vec.insert( vec.size(), MyClass(22) ); vec.insert( vec.size(), MyClass(33) );
        int sum2 = sumMemClass( vec.constData(), old_sz );
        VERIFY( vec.size() == old_sz + 2, "insert at 0 failed!" );
        VERIFY( sum == sum2, "after insert, content is broken!" );
        VERIFY( vec.at( vec.size() - 1 ).value() == 33  && vec.at( vec.size() - 2 ).value() == 22, "after inserted, the order is broken!");

    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    VERIFY( g_mem_cntr == 0, "MyClass Refr. cntr != 0!" );
    std::printf("passed\n");
}

static void  test_VEC_remove_class( )
{
    std::printf( "%s : ", __FUNCTION__ );
    { // remove at 0
        QxPack::ScVector<MyClass> vec(3);
        randInitMemClass( vec.dataCow(), vec.size() );
        size_t old_sz = vec.size();
        int sum = sumMemClass( vec.constData() + 1,  old_sz - 1 );
        vec.remove( 0 );
        int sum2= sumMemClass( vec.constData(), old_sz - 1 );
        VERIFY( vec.size() == 2, "remove failed!" );
        VERIFY( sum == sum2, "after removed, content changed!" );
    }
    { // remove middle
        QxPack::ScVector<MyClass> vec(6);
        randInitMemClass( vec.dataCow(), vec.size() );
        size_t  rem_sz = vec.size() - 2 - 1;
        int sum  = sumMemClass( &vec.constData()[3] , rem_sz );
        vec.remove(1); vec.remove(1);
        int sum2 = sumMemClass( &vec.constData()[1],  rem_sz );
        VERIFY( vec.size() == 4, "insert at middle failed!");
        VERIFY( sum == sum2, "insert broke the content!" );
    }
    { // remove last
        QxPack::ScVector<MyClass> vec(3);
        randInitMemClass( vec.dataCow(), vec.size());
        size_t old_sz = vec.size();
        vec.remove(vec.size()-1);
        VERIFY( vec.size() == old_sz - 1, "remove at last failed!" );
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    VERIFY( g_mem_cntr == 0, "MyClass Refr. cntr != 0!" );
    std::printf("passed\n");
}


// ////////////////////////////////////////////////////////////////////////////
// mixing
// ////////////////////////////////////////////////////////////////////////////
static void test_MIX_VecVec_ctor()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
       QxPack::ScVector<QxPack::ScVector<int>> vec2d(2);
       for ( size_t i = 0, imax = vec2d.capacity(); i < imax; i ++ ) {
           vec2d[i] = QxPack::ScVector<int>( 2 );
       }
       VERIFY( ! vec2d.isNull(), "ctor do nothing!");
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    std::printf("passed\n");
}

static void test_MIX_VecVec_writeAndRead()
{
    std::printf( "%s : ", __FUNCTION__ );
    {
       // build 2*2 elements
       QxPack::ScVector<QxPack::ScVector<int>> vec2d(2);
       for ( size_t i = 0, imax = vec2d.capacity(); i < imax; i ++ ) {
           vec2d[i] = QxPack::ScVector<int>( 2 );
       }
       VERIFY( ! vec2d.isNull(), "ctor do nothing!");

       // write
       for ( size_t i = 0; i < 2; i ++ ) {
           for ( size_t j = 0; j < 2; j ++ ) {
               vec2d[i][j] = int(i) * int(j);
           }
       }

       // for const read, use an const refr.
       const auto &c_vec2d = vec2d;
       for ( size_t i = 0; i < 2; i ++ ) {
           for ( size_t j = 0; j < 2; j ++ ) {
               VERIFY( c_vec2d[i][j] == int(i)*int(j), "bad content!" );
           }
       }
    }
    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "ScMemBlock mem cntr != 0!" );
    std::printf("passed\n");
}


// ////////////////////////////////////////////////////////////////////////////
// entry
// ///////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    (void)(argc); (void)(argv);

    std::printf("\nstart test non-class type in ScVector.\n");
    test_VEC_ctor();
    test_VEC_ctorCopy();
    test_VEC_assignCopy();
    test_VEC_dataCow();
    test_VEC_copy();
    test_VEC_constRead();
    test_VEC_push_back();
    test_VEC_pop_back();
    test_VEC_push_front();
    test_VEC_pop_front();
    test_VEC_front();
    test_VEC_back();
    test_VEC_empty();
    test_VEC_reserve();
    test_VEC_resize();
    test_VEC_insert();
    test_VEC_remove();
    std::printf("\nlast mem cntr: %d\n", QxPack::ScMemBlock::currMemCntr() );

    std::printf("\nstart test class type in ScVector.\n");
    test_VEC_ctor_class();
    test_VEC_ctorCopy_class();
    test_VEC_assignCopy_class();
    test_VEC_dataCow_class();
    test_VEC_copy_class();
    test_VEC_constRead_class();
    test_VEC_push_back_class();
    test_VEC_pop_back_class();
    test_VEC_push_front_class();
    test_VEC_pop_front_class();
    test_VEC_front_class();
    test_VEC_back_class();
    test_VEC_empty_class();
    test_VEC_reserve_class();
    test_VEC_resize_class();
    test_VEC_insert_class();
    test_VEC_remove_class();
    std::printf("\nlast mem cntr: %d\n", QxPack::ScMemBlock::currMemCntr() );

    std::printf("\nstart test mixing ScVector.\n");
    test_MIX_VecVec_ctor();
    test_MIX_VecVec_writeAndRead();

    std::printf("\nlast mem cntr: %d\n", QxPack::ScMemBlock::currMemCntr() );

    VERIFY( QxPack::ScMemBlock::currMemCntr() == 0, "memory leak!");
    std::printf("MyClass mem cntr: %d\n", g_mem_cntr );
    VERIFY( g_mem_cntr == 0, "the MyClass ref. bad!" );
    return 0;
}
