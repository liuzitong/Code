// ////////////////////////////////////////////////////////////////////////////
// @author nightwing
// @history
//        2018/07/04  modified, add operator <
//        2018/08/22  modified, optimazie for const string
//        2018/09/21  fixed bugs, gFreeBa() do not check
//        2018/09/23  modified the static object initialize
//        2018/11/27  modified, fixed priviate object CTOR(COPY).
//        2018/12/20  modified, for GCC 5 compiler
//        2019/01     modified, fixed the string zero endian.
// ////////////////////////////////////////////////////////////////////////////
#ifndef QXPACK_IC_BYTEARRAY_CXX
#define QXPACK_IC_BYTEARRAY_CXX

#include <memory>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <deque>  // C++ 11
#include <mutex>  // C++ 11
#include <atomic> // C++ 11
#include <cassert>

#include "qxpack_ic_bytearray.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/common/qxpack_ic_dyncinit_priv.hxx"
#include "qxpack/indcom/common/qxpack_ic_logging.hxx"

//#ifdef __GNUC__ // in GCC 5, close below warnings
//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wpadded"
//#endif


namespace QxPack { /* static void  gFreeBa( void *obj );*/ }
//#define RecycleFreeBa( o, t )         do{ QxPack::gFreeBa( ( t *) o ); }while(0)
#define QXPACK_IcPImplPrivTemp_new    qxpack_ic_new
//#define QXPACK_IcPImplPrivTemp_delete RecycleFreeBa
#define QXPACK_IcPImplPrivTemp_delete qxpack_ic_delete
#include "qxpack_ic_pimplprivtemp.hpp"

#define USERDATA_SLOT_NUM  6
#define USERDATA_SIZE  ( sizeof( intptr_t ) * USERDATA_SLOT_NUM )

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
// private object
// ////////////////////////////////////////////////////////////////////////////
#define T_PrivPtr( o )  T_ObjCast( IcByteArrayPriv*, o )
class QXPACK_IC_HIDDEN  IcByteArrayPriv : public IcPImplPrivTemp< IcByteArrayPriv > {
private:
    char *m_data; int m_size;
    bool  m_is_const_str, m_is_ref_str;
    IcByteArray::CleanUpFunction  m_clean_up_func;
    void *m_clean_up_info;
    intptr_t  m_user_data[ USERDATA_SLOT_NUM ];

public :
    IcByteArrayPriv ( )
    {
        m_data = nullptr;          m_size = 0;
        m_is_const_str = false;    m_is_ref_str = false;
        m_clean_up_func = nullptr; m_clean_up_info = nullptr;
        std::memset( m_user_data, 0, sizeof( m_user_data ));
    }
    IcByteArrayPriv ( const IcByteArrayPriv & );
    virtual ~IcByteArrayPriv( );

    void  initRefStr( bool is_const, const char     *p, int size = -1, IcByteArray::CleanUpFunction cl_func = nullptr, void *cl_info = nullptr );
    void  initRefStr( bool is_const, const uint16_t *p, int num  = -1, IcByteArray::CleanUpFunction cl_func = nullptr, void *cl_info = nullptr );
    void  freeData  ( );
    void  resize    ( int size );

    inline  int    readCurrRefCntr( ) { return this->currRefCntr(); }
    inline  bool   isConstStr( ) { return m_is_const_str; }
    inline  bool   isRefStr( )   { return m_is_ref_str;   }
    inline  char*  data ( )      { return m_data; }
    inline  int    size ( )      { return m_size; }
    inline  char*  userData( )   { return T_PtrCast( char*, m_user_data ); }

    inline  bool  isLessThan( const IcByteArrayPriv & other )
    {
        // --------------------------------------------------------------------
        // rules
        // a) null data < null data     -- false
        // b) null data < non-null data -- true
        // c) short data < long data  ( existed some same segment ) -- true
        // --------------------------------------------------------------------
        if ( m_data != nullptr ) {
            if ( other.m_data == nullptr ) { return false; } // data >= nullptr
            int cmp_rsl = std::memcmp( m_data, other.m_data, size_t( m_size <= other.m_size ? m_size : other.m_size ));
            if ( cmp_rsl < 0 ) { return true;  } // m_data < other.m_data          
            if ( cmp_rsl > 0 ) { return false; } // m_data > other.m_data
            if ( m_size < other.m_size ) { return true; }
            return false; // other.m_size < m_size and prefix string is same.
        } else {
            return ( other.m_data != nullptr ? true : false );
        }
    }

    inline bool  isEqual( const IcByteArrayPriv & other )
    {
        // --------------------------------------------------------------------
        // rules
        // a) pointer are same -- true
        // b) content are same -- true
        // c) otherwise -- false
        // --------------------------------------------------------------------
        if ( m_data != nullptr && other.m_data != nullptr ) {
            if ( m_data == other.m_data ) { return true;  }
            if ( m_size != other.m_size ) { return false; }
            return ( 0 == std::memcmp( m_data, other.m_data, size_t( m_size )));
        } else {
            return false; // m_data is null or other.m_data is null.
        }
    }

    // set user data
    inline int  writeUserData ( const char *sp, int size )
    {
        int sz = ( size <=  int( USERDATA_SIZE ) ? size : USERDATA_SIZE );
        if ( sp != nullptr ) { std::memcpy( & m_user_data[0], sp, size_t( sz )); }
        return sz;
    }

    // read the user data
    inline int   readUserData(  char *dp, int size )
    {
        int sz = ( size <=  int( USERDATA_SIZE ) ? size : USERDATA_SIZE );
        if ( dp != nullptr ) { std::memcpy( dp, & m_user_data[0], size_t( sz )); }
        return sz;
    }
};

// ============================================================================
// ctor ( Copy )
// ============================================================================
IcByteArrayPriv :: IcByteArrayPriv ( const IcByteArrayPriv &other )
{
    // ------------------------------------------------------------------------
    // ocurred while IcByteArray do a write operation
    // this will perform a deep copy.
    // ------------------------------------------------------------------------
    if ( other.m_data != nullptr && other.m_size > 0 ) {
        // m_data = static_cast<char*>( qxpack_ic_alloc( m_size = other.m_size ));
        // nw: 20190102 append zero.
        m_data = T_PtrCast( char*, qxpack_ic_alloc( size_t( m_size = other.m_size ) + sizeof( wchar_t ) ));
        std::memset( & m_data[ m_size ], 0, sizeof( wchar_t ));
        std::memcpy( m_data, other.m_data, size_t( other.m_size ));
    } else {
        m_data = nullptr; m_size = 0;
    }
    m_is_const_str = false;    m_is_ref_str = false;
    m_clean_up_func = nullptr; m_clean_up_info = nullptr;
    std::memcpy( m_user_data, other.m_user_data, USERDATA_SIZE );
}

// ============================================================================
// dtor
// ============================================================================
IcByteArrayPriv :: ~IcByteArrayPriv ( )
{
    this->freeData( );
}

// ============================================================================
// init the object point to reference memory
// ============================================================================
void    IcByteArrayPriv :: initRefStr (
    bool is_const, const char *p, int size,
    IcByteArray::CleanUpFunction cl_func, void *cl_info
) {
    m_data = const_cast< char*>( p );
    m_size = ( size <= 0 ? int( std::strlen( p )) : size );
    m_is_const_str  = is_const;  m_is_ref_str = true;
    m_clean_up_func = cl_func;   m_clean_up_info = cl_info;
}

// ============================================================================
// init the object point to reference memory. ( override )
// ============================================================================
void    IcByteArrayPriv :: initRefStr(
    bool is_const, const uint16_t *p , int num,
    IcByteArray::CleanUpFunction cl_func, void *cl_info
) {
    m_data = T_PtrCast( char*, p );
    if ( num <= 0 ) {
        while ( *p != 0 ) { ++ p; }
        m_size = int( T_PtrCast( char *, p ) - m_data ); // diff is the byte number
    } else {
        m_size = num * 2; // uint16_t should be 2 bytes
    }
    m_is_const_str  = is_const; m_is_ref_str = true;
    m_clean_up_func = cl_func;  m_clean_up_info = cl_info;
}

// ============================================================================
// free the data
// ============================================================================
void    IcByteArrayPriv :: freeData()
{
    // ------------------------------------------------------------------------
    // free the data if not a point to reference data
    // NOTE: here do not touch the userdata.
    // ------------------------------------------------------------------------
    if ( ! m_is_ref_str ) {
        qxpack_ic_free( m_data );
    } else {
        if ( m_clean_up_func != nullptr ) {
            (* m_clean_up_func )( m_clean_up_info );
        }
    }
    m_data = nullptr; m_size = 0;
    m_is_const_str = false; m_is_ref_str = false;
}

// ============================================================================
// resize the byte array space
// ============================================================================
void    IcByteArrayPriv :: resize ( int size )
{
    // check if size is valid.
    if ( size <= 0 ) { this->freeData(); return; }

    // check if size is same
    if ( size == m_size ) { return; }

    // adjust
    if ( m_data == nullptr ) {     // no data
        //m_data = T_PtrCast( char*, qxpack_ic_alloc(  m_size = size ) );
        // nw: 20190102 append zero.
        m_data = T_PtrCast( char*, qxpack_ic_alloc( size_t( m_size = size ) + sizeof( wchar_t ) ) );
        std::memset( m_data, 0, size_t( size ) + sizeof( wchar_t ));

    } else if ( ! m_is_ref_str ) {
        if ( ! m_is_const_str ) { // normal data
            m_data = T_PtrCast( char*, qxpack_ic_realloc( m_data, ( m_size = size ) + sizeof( wchar_t ) ));
            std::memset( & m_data[ size ], 0, sizeof( wchar_t ));
        } else {                  // normal data, but const.
            // char *new_buff = T_PtrCast( char *, qxpack_ic_alloc( size ));
            // nw: 20190102 append zero.
            char *new_buff = T_PtrCast( char *, qxpack_ic_alloc( size_t( size ) + sizeof( wchar_t )) );
            std::memset( & new_buff[ size ], 0, sizeof( wchar_t ));

            std::memcpy( new_buff, m_data, size_t( size <= m_size ? size : m_size ) );
            this->freeData(); // free old memory
            m_data = new_buff;
            m_size = size; m_is_const_str = false;
        }
    } else {  // ref. data.  ( non-const and const )
        //char *new_buff =  T_PtrCast( char*, qxpack_ic_alloc( size ) );
        // nw: 20190102 append zero.
        char *new_buff =  T_PtrCast( char*, qxpack_ic_alloc( size_t( size ) + sizeof( wchar_t )) );
        std::memset( & new_buff[ size ], 0, sizeof( wchar_t ));

        std::memcpy( new_buff, m_data, size_t( size <= m_size ? size : m_size ) );
        this->freeData();  // free old ref. memory.
        m_data = new_buff;
        m_size = size;
        m_is_const_str = false; m_is_ref_str = false;
    }
}

// ////////////////////////////////////////////////////////////////////////////
// IcByteArray wrap
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcByteArray :: IcByteArray( ) { m_obj = nullptr; }

// ============================================================================
// ctor ( create )
// ============================================================================
IcByteArray :: IcByteArray( int size )
{
    m_obj = nullptr;
    if ( size > 0 ) {
        IcByteArrayPriv::buildIfNull( & m_obj );
        T_PrivPtr( m_obj )->resize( size );
    }
}

// ============================================================================
// create a object from another
// ============================================================================
IcByteArray :: IcByteArray( const IcByteArray &other )
{
    m_obj = nullptr;
    QXPACK_IcPImplPrivTemp_AttachPtr( IcByteArrayPriv, & m_obj, & other.m_obj );
}

// ============================================================================
// make object null
// ============================================================================
void           IcByteArray :: makeNull()
{
    if ( m_obj != nullptr ) {
        IcByteArrayPriv::attach( & m_obj, nullptr );
        m_obj = nullptr;
    }
}

// ============================================================================
// assign object to this object
// ============================================================================
IcByteArray &  IcByteArray :: operator = ( const IcByteArray &other )
{
    QXPACK_IcPImplPrivTemp_AttachPtr( IcByteArrayPriv, & m_obj, & other.m_obj );
    return *this;
}

// ============================================================================
// dtor
// ============================================================================
IcByteArray :: ~IcByteArray( )
{
    if ( m_obj != nullptr ) {
        IcByteArrayPriv::attach( & m_obj, nullptr );
        m_obj = nullptr;
    }
}

// ============================================================================
// check if current object is empty or not
// ============================================================================
bool  IcByteArray :: isEmpty( ) const
{
    if ( m_obj == nullptr ) { return true; }
    if ( T_PrivPtr( m_obj )->data() == nullptr ) { return true; }
    if ( T_PrivPtr( m_obj )->size() <= 0 ) { return true; }
    return false;
}

// ============================================================================
// check if current object is const data
// ============================================================================
bool  IcByteArray :: isConst() const
{
    return ( m_obj != nullptr ? T_PrivPtr( m_obj )->isConstStr() : true );
}

// ============================================================================
// check if current object is referenced data
// ============================================================================
bool  IcByteArray :: isRefData() const
{
    return ( m_obj != nullptr ? T_PrivPtr( m_obj )->isRefStr() : false );
}

// ============================================================================
// return the data size
// ============================================================================
int    IcByteArray :: size( ) const
{
    return ( m_obj != nullptr ? T_PrivPtr( m_obj )->size() : 0 );
}

// ============================================================================
// return the writable data pointer
// ============================================================================
char *  IcByteArray :: data( )
{
    if ( m_obj != nullptr ) {
        if ( ! T_PrivPtr( m_obj )->isRefStr()) {
            // non-ref. data, do COW( copy on write )
            return IcByteArrayPriv::instanceCow( & m_obj )->data();
        } else {
            if ( ! T_PrivPtr( m_obj )->isConstStr() ) {
                // ref. data, non-const, directly access
                return T_PrivPtr( m_obj )->data();
            } else {
                // ref. data, const, directly do ctor( copy )
                void *new_obj = IcByteArrayPriv::createInstance( m_obj );
                IcByteArrayPriv::attach( & m_obj, nullptr );
                m_obj = new_obj;
                return T_PrivPtr( m_obj )->data();
            }
        }
    } else {
        return nullptr;
    }
}

// ============================================================================
// return the const data pointer, assume user do not modify it
// ============================================================================
const char*  IcByteArray :: constData() const
{
    return ( m_obj != nullptr ? T_PrivPtr( m_obj )->data() : nullptr );
}

// ============================================================================
// return user data size
// ============================================================================
int   IcByteArray :: userDataSize ( ) const
{ return USERDATA_SIZE; }

// ============================================================================
// write user data.
// ============================================================================
int   IcByteArray :: writeUserData( const char* sp, int size )
{
    if ( sp == nullptr || size <= 0 ) { return 0; }

    if ( m_obj != nullptr ) {
        if ( ! T_PrivPtr( m_obj )->isRefStr()) {
            // non-ref, do COW
            return IcByteArrayPriv::instanceCow( & m_obj )->writeUserData( sp, size );
        } else {
            if ( ! T_PrivPtr( m_obj )->isConstStr()) {
                // ref.data, non-const, directly access
                return T_PrivPtr( m_obj )->writeUserData( sp, size );
            } else {
                // ref.data, const
                void *new_obj = IcByteArrayPriv::createInstance( m_obj );
                IcByteArrayPriv::attach( & m_obj, nullptr );
                m_obj = new_obj;
                return T_PrivPtr( m_obj )->writeUserData( sp, size );
            }
        }
    } else {
        return 0;
    }
}

// ============================================================================
// read user data
// ============================================================================
int   IcByteArray :: readUserData ( char* dp, int size )
{
    return ( m_obj != nullptr ? T_PrivPtr( m_obj )->readUserData( dp, size ) : 0 );
}

// ============================================================================
// change the size of this bytearray, if changed the buffer, need to make a deep copy
// ============================================================================
void    IcByteArray :: resize ( int size )
{
    // check if same size
    if ( m_obj != nullptr && T_PrivPtr( m_obj )->size() == size )
    { return; }

    // check if zero size, here drop data.
    if ( size <= 0 ) {
        if ( m_obj != nullptr ) {
            IcByteArrayPriv::attach( & m_obj, nullptr );
            m_obj = nullptr;
        }
        return;
    }

    // build object if null
    if ( m_obj == nullptr ) {
        IcByteArrayPriv::buildIfNull( & m_obj );
        T_PrivPtr( m_obj )->resize( size );
        return;
    }

    // resize directly if not shared with other
    if ( T_PrivPtr( m_obj )->readCurrRefCntr() == 1 ) {
        // private resize() can handle normal data, ref.data ( const and non-const )
        T_PrivPtr( m_obj )->resize( size );
    } else {
        // shared with other
        // for normal data, ref.data, build a new object, and copy data
        int old_size = T_PrivPtr( m_obj )->size();

        // rebuild one
        void *new_obj = nullptr;
        IcByteArrayPriv::buildIfNull( & new_obj );
        T_PrivPtr( new_obj )->resize( size );

        // copy all data
        std::memcpy (
            T_PrivPtr( new_obj )->data(),     T_PrivPtr( m_obj )->data(),
            size_t( size <= old_size ? size : old_size )
        );
        std::memcpy(
            T_PrivPtr( new_obj )->userData(), T_PrivPtr( m_obj )->userData(),
            USERDATA_SIZE
        );

        // drop old, attach to new
        IcByteArrayPriv::attach( & m_obj, nullptr );
        m_obj = new_obj;
    }
}

// ============================================================================
// Attempts to allocate memory for at least size bytes.
// ============================================================================
void     IcByteArray :: reserve( int size )
{
    if ( m_obj != nullptr && T_PrivPtr( m_obj )->size() >= size ) { return; }
    this->resize( size );
}

// ============================================================================
// do a user data copy constructor
// ============================================================================
IcByteArray :: IcByteArray ( const char *data, int len  )
{
    m_obj = nullptr;
    if ( len < 0 ) { len = int( std::strlen( data )); }
    if ( len > 0 ) {
        IcByteArrayPriv::buildIfNull( & m_obj );
        T_PrivPtr( m_obj )->resize( len );
        std::memcpy( T_PrivPtr( m_obj )->data( ), data, size_t( len ));
    }
}

// ============================================================================
// do a copy
// ============================================================================
IcByteArray   IcByteArray :: copy( ) const
{
    IcByteArray ba;
    if ( m_obj != nullptr ) {
        ba.m_obj = IcByteArrayPriv::createInstance( m_obj );
    }
    return ba;
}

// ============================================================================
// operator < override
// ============================================================================
bool   IcByteArray :: operator < ( const IcByteArray & other ) const
{
    // ------------------------------------------------------------------------
    // rules:
    // a) null < null  -- false
    // b) null < obj   -- true
    // c) obj cmp obj
    // ------------------------------------------------------------------------
    if ( m_obj != nullptr ) {
        if ( other.m_obj == nullptr ) { return false; }
        return T_PrivPtr( m_obj )->isLessThan( *( T_PrivPtr( other.m_obj )) );
    } else { // m_obj is null
        return ( other.m_obj != nullptr );  // null < obj if possible.
    }
}

// ============================================================================
// operator == override
// ============================================================================
bool   IcByteArray :: operator == ( const IcByteArray &other )
{
    if ( m_obj != nullptr ) {
        if ( other.m_obj == nullptr ) { return false; }
        return T_PrivPtr( m_obj )->isEqual( *( T_PrivPtr( other.m_obj )) );
    } else {
        return ( m_obj == other.m_obj ); // m_obj is nullptr
    }
}

// ////////////////////////////////////////////////////////////////////////////
//
// static buffer manager about ByteArray, used for fromConstString()
//
// ////////////////////////////////////////////////////////////////////////////
//static std::deque<void*>  g_free_ba;
//static std::mutex         g_locker;

//static void  gDeInitRes( )
//{
//    // do not do lock/unlock at dync.deinit()
//    while ( ! g_free_ba.empty() ) {
//        IcByteArrayPriv *ptr = T_ObjCast( IcByteArrayPriv*,  g_free_ba.front());
//        g_free_ba.pop_front();
//        qxpack_ic_delete( ptr, IcByteArrayPriv );
//    }
//}
//static IcDyncInit g_dync_init( nullptr, & gDeInitRes );

// ============================================================================
// allocate the buffer
// ============================================================================
//static IcByteArrayPriv *   gAllocFreeBa( )
//{
//    IcByteArrayPriv *ptr = nullptr;

//    // ------------------------------------------------------------------------
//    // do allocate from list or heap
//    // ------------------------------------------------------------------------
//    g_locker.lock();
//    if ( ! g_free_ba.empty() ) {
//        IcByteArrayPriv *tmp = T_ObjCast( IcByteArrayPriv*, g_free_ba.front()); g_free_ba.pop_front();
//        QXPACK_IcPImplPrivTemp_AttachPtr( IcByteArrayPriv, &ptr, &tmp );
//    }
//    g_locker.unlock();
//    if ( ptr == nullptr ) {
//        QXPACK_IcPImplPrivTemp_BuildIfNull( IcByteArrayPriv, &ptr );
//    }

//    return ptr;
//}

//// ============================================================================
//// free the buffer, this function is called form PImpl mech.
//// the obj must be zero referenced.
//// ============================================================================
//static void  gFreeBa( void *obj )
//{
//    if ( obj == nullptr ) { return; }
//    if ( ! T_PrivPtr( obj )->isConstStr()) {
//        // --------------------------------------------------------------------
//        // free any non-const string object
//        // --------------------------------------------------------------------
//        qxpack_ic_delete( obj, IcByteArrayPriv );

//    } else {
//        // --------------------------------------------------------------------
//        // recycle const string object for re-use
//        // --------------------------------------------------------------------
//        g_locker.lock( );
//        if ( g_free_ba.size() < 128 ) {
//            g_free_ba.push_back( obj );
//        } else {
//            qxpack_ic_delete( obj, IcByteArrayPriv );
//        }
//        g_locker.unlock();
//    }
//}

// ============================================================================
// point to the const data  [ static ]
// ============================================================================
IcByteArray  IcByteArray :: fromConstData( const char *data, int e_num )
{
    IcByteArray ba;
    IcByteArrayPriv::buildIfNull( &ba.m_obj ); // ba.m_obj = gAllocFreeBa();
    //if ( ba.m_obj != nullptr ) {
        T_PrivPtr( ba.m_obj )->initRefStr( true, data, e_num );
    //}
    return ba;
}

// ============================================================================
// point to the const data [ static ]
// ============================================================================
IcByteArray  IcByteArray :: fromConstData( const uint16_t *data, int num )
{
    IcByteArray ba;
    IcByteArrayPriv::buildIfNull( &ba.m_obj );  //ba.m_obj = gAllocFreeBa();
    //if ( ba.m_obj != nullptr ) {
        T_PrivPtr( ba.m_obj )->initRefStr( true, data, num );
    //}
    return ba;
}

// ============================================================================
// point to the const user data that can be cleanup [ static ]
// ============================================================================
IcByteArray  IcByteArray :: fromConstData (
    const char *data, int len, CleanUpFunction cl_func, void *cl_obj
) {
    IcByteArray ba;
    IcByteArrayPriv::buildIfNull( &ba.m_obj ); //ba.m_obj = gAllocFreeBa();
    //if ( ba.m_obj != nullptr ) {
        T_PrivPtr( ba.m_obj )->initRefStr( true, data, len, cl_func, cl_obj );
    //}
    return ba;
}

// ======================================================
// point to the user data that can be cleanup [ static ]
// ======================================================
IcByteArray   IcByteArray :: fromData (
    char *data, int len, CleanUpFunction cl_func, void *cl_obj
) {
    IcByteArray ba;
    IcByteArrayPriv::buildIfNull( &ba.m_obj ); //ba.m_obj = gAllocFreeBa();
    //if ( ba.m_obj != nullptr ) {
        T_PrivPtr( ba.m_obj )->initRefStr( false, data, len, cl_func, cl_obj );
    //}
    return ba;
}


// ======================================================
// free the cache [ static ]
// ======================================================
void   IcByteArray :: cleanupCache() {
//    g_locker.lock();
//    gDeInitRes();
//    g_locker.unlock();
}


}

//#ifdef __GNUC__ // in GCC 5, close below warnings
//#pragma GCC diagnostic pop
//#endif

#endif
