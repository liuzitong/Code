// ////////////////////////////////////////////////////////////////////////////
//! @file     qxpack_sc_memoryblock.cxx
//! @author   night wing
//! @date     2020/08
//! @version  0.3.0
// ////////////////////////////////////////////////////////////////////////////
#ifndef QXPACK_SC_MEMBLOCK_CXX
#define QXPACK_SC_MEMBLOCK_CXX

#include "qxpack_sc_memblock.hxx"
#include <stdint.h>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <new>

// ////////////////////////////////////////////////////////////////////////////
//
//                 pre-macro decalre  
//
// ////////////////////////////////////////////////////////////////////////////
#if defined( __GNUC__ )
#  ifndef _GNU_SOURCE
#    define _GNU_SOURCE
#  endif
#elif defined( WIN32 ) || defined( _MSC_VER )
#  pragma comment( lib, "kernel32.lib")
#  pragma comment( lib, "user32.lib")
#  include <windows.h>
#else
#  error "this platform is not supported"
#endif

namespace QxPack {


// ////////////////////////////////////////////////////////////////////////////
//
//                 atomic functions
//
// ////////////////////////////////////////////////////////////////////////////

#if defined( __GNUC__ )   // linux/unix
static intptr_t  gLoadAtom( volatile intptr_t *vp )
{
    intptr_t  old;
    do {
        old = *vp;
        if ( __sync_bool_compare_and_swap( vp, old, old )) {
            break;
        }
    } while ( true );
    return old;
}

inline static intptr_t  gAddAtom( volatile intptr_t *vp, intptr_t v )
{
    return static_cast< intptr_t >( __sync_add_and_fetch( vp, v ));
}

#elif defined( WIN32 ) || defined( _MSC_VER )

static intptr_t  gLoadAtom( volatile intptr_t *vp )
{
    intptr_t old;
    do {
        old = *vp;
        if ( InterlockedCompareExchangePointer(
            reinterpret_cast< volatile PVOID *>( vp ),
            reinterpret_cast< PVOID >( old ), reinterpret_cast< PVOID >( old )) == reinterpret_cast< PVOID >( old )
        ) { break; }
    } while ( TRUE );
    return old;
}

static intptr_t  gAddAtom( volatile intptr_t *curr_atom, intptr_t v )
{
    intptr_t curr_val;
    do {
        curr_val = *curr_atom;
    } while (
        InterlockedCompareExchangePointer(
            reinterpret_cast< volatile PVOID *>( curr_atom ),
            reinterpret_cast< PVOID >( curr_val + v ), reinterpret_cast< PVOID >( curr_val )
        ) != reinterpret_cast< PVOID >( curr_val ) /* return curr_val means exchanged OK */
    );
    return curr_val + v;
}


#else
# error  "unsupported operation system"
#endif



// ////////////////////////////////////////////////////////////////////////////
//
//                    helper functions
//
// ////////////////////////////////////////////////////////////////////////////
static intptr_t  g_mem_cntr = 0;

static  void*  gAlloc( size_t sz )
{
    gAddAtom( &g_mem_cntr, 1 );
    void *ptr = std::malloc( sz );
    QXPACK_SC_FATAL( ptr != nullptr, "malloc return nullptr!" );
// < 0.2.0,  std::memset( ptr, 0, sz ); here cleared all mem.
    return ptr;
}

static  void   gFree ( void *p   )
{
    if ( p != nullptr ) {
        gAddAtom( &g_mem_cntr, -1 );
        std::free( p );
    }
}

inline static  size_t  gRound16( size_t n ) {  return ( n + 15 ) / 16 * 16; }

typedef void* (*MemAlloc   )( size_t );
typedef void  (*MemFree    )( void*  );
typedef void* (*MemReAlloc )( void*, size_t );
typedef bool  (*UserProcCb )( ScMemBlock::ActID, void*, void* );

// ////////////////////////////////////////////////////////////////////////////
//                     
//                     Memory Block Private
//
// ////////////////////////////////////////////////////////////////////////////
#define  SCMEMBLOCK_VERSION             3
#define  SCMEMBLOCK_FLAG_CAN_FREE      0x01
#define  SCMEMBLOCK_FLAG_CAN_DESTROY   0x02
#define  SCMEMBLOCK_TAIL_PADDING_SIZE  16   // DO NOT change it ! [0,1,2,3] is 16 bytes

#define T_MB_Priv( o )  reinterpret_cast<ScMemBlockPriv*>(o)
class  QXPACK_SC_HIDDEN  ScMemBlockPriv { // WARNING: member order fixed! DO NOT MOFIFY IT!
private:
    #define SCMEMBLOCKPRIV_LESS_2_SIZE  (8*sizeof(size_t)) // < 0.2.0, the header size is fixed.

    intptr_t  m_atom;
    size_t    m_version;
    size_t    m_size,  m_flags;
    MemAlloc  m_f_alloc; MemFree  m_f_free;

    // since 0.2.0 changed rsvd[2], the data offset used the last member, so we can add member in future.
    UserProcCb m_f_user_proc; intptr_t m_data_ofv;

protected:
    inline intptr_t    incrRef()  { return gAddAtom( & m_atom, 1 ); }
    inline intptr_t    decrRef()  { return gAddAtom( & m_atom, -1); }
    inline bool        canFree() const    { return ( m_flags & SCMEMBLOCK_FLAG_CAN_FREE    ) == SCMEMBLOCK_FLAG_CAN_FREE; }
    inline bool        canDestroy() const { return ( m_flags & SCMEMBLOCK_FLAG_CAN_DESTROY ) == SCMEMBLOCK_FLAG_CAN_DESTROY; }

    inline uint8_t*    tailPtr()  { return reinterpret_cast<uint8_t*>( & m_atom ) + ScMemBlockPriv::ensureMemSize( m_size ) - SCMEMBLOCK_TAIL_PADDING_SIZE; }

#if defined(_NDEBUG) || defined(NDEBUG)
           bool        checkTail() { return true; }
#else
           bool        checkTail()
           {
               uint8_t *tail = tailPtr(); bool ret = true;
               for ( int i = 0; ret && i < SCMEMBLOCK_TAIL_PADDING_SIZE; i ++ ) {
                   ret = ( tail[i] == 0xcd );
               }
               return ret;
           }
#endif

public :
    // ========================================================================
    // ctor
    // ========================================================================
    explicit ScMemBlockPriv( size_t sz, UserProcCb user_cb = nullptr )
    {  
        m_atom = 1;  m_version = SCMEMBLOCK_VERSION;
        m_size = sz; m_flags   = SCMEMBLOCK_FLAG_CAN_FREE | SCMEMBLOCK_FLAG_CAN_DESTROY;
        m_f_alloc     = & gAlloc;   m_f_free   = & gFree;
        m_f_user_proc = user_cb;    m_data_ofv = sizeof( ScMemBlockPriv );

        // fill the content
        if ( m_f_user_proc != nullptr ) {
            ScMemBlock::ActInitParam par = { data(), size() };
            m_f_user_proc( ScMemBlock::ActID_Init, &par, nullptr );
        } else {
            std::memset( data(), 0, sz );
        }

        // fill tail bytes
        std::memset( tailPtr(), 0xcd, SCMEMBLOCK_TAIL_PADDING_SIZE );
    }

    // ========================================================================
    // dtor
    // ========================================================================
    ~ScMemBlockPriv( )
    {
        // --------------------------------------------------------------------
        // < 0.2.0, m_f_user_proc must be 0 ( nullptr )
        // --------------------------------------------------------------------
        if ( m_f_user_proc != nullptr  &&  canDestroy() ) {
            ScMemBlock::ActDestroyParam par = { data(), size() };
            m_f_user_proc( ScMemBlock::ActID_Destroy, &par, nullptr );
        }
        QXPACK_SC_ASSERT( checkTail(), "tail bytes are broken!" );
    }

    // ========================================================================
    // member functions
    // ========================================================================
    inline MemAlloc    memAlloc()   { return m_f_alloc; }
    inline MemFree     memFree()    { return m_f_free;  }
    inline size_t &    version()    { return m_version; }
    inline size_t   &  size()       { return m_size;    }
    inline const void* constData()  { return reinterpret_cast<uint8_t*>(&m_atom) + ( m_version >= 2 ? size_t(m_data_ofv) : SCMEMBLOCKPRIV_LESS_2_SIZE ); }
    inline void*       data()       { return reinterpret_cast<uint8_t*>(&m_atom) + ( m_version >= 2 ? size_t(m_data_ofv) : SCMEMBLOCKPRIV_LESS_2_SIZE ); }
    inline intptr_t    currRef()    { return gLoadAtom( & m_atom ); }
    inline bool        isTailBroken() { return !checkTail(); }

    // ========================================================================
    // static functions
    // ========================================================================
    static size_t          ensureMemSize( size_t sz ) { return gRound16( sz + sizeof( ScMemBlockPriv ) + SCMEMBLOCK_TAIL_PADDING_SIZE ); }
    static bool            detach( void** );
    static ScMemBlockPriv* attach( void**, void** );
    static ScMemBlockPriv* duplicate   ( void** );
    static ScMemBlockPriv* dupIfShared ( void** );
    static void            resize_cp ( void**, size_t ); // since 0.2.0
};

// ============================================================================
// [ static ] detach ref., if zero, delete it
// ============================================================================
bool                    ScMemBlockPriv :: detach( void ** p )
{
    ScMemBlockPriv **pptr = reinterpret_cast<ScMemBlockPriv**>(p);
    if( (*pptr)->decrRef() == 0 ) {
        if ( (*pptr)->canFree()) {
            MemFree f_free = (*pptr)->m_f_free;
            QXPACK_SC_FATAL( f_free != nullptr, "the free function is missing!");
            (*pptr)->~ScMemBlockPriv(); // call dtor
            f_free( *pptr );
            *pptr = nullptr;
        }
        return true;
    } else {
        return false;
    }
}

// ============================================================================
// [ static ] attach s_pptr to d_pptr
// ============================================================================
ScMemBlockPriv*         ScMemBlockPriv :: attach( void **spp, void **dpp )
{
    ScMemBlockPriv **s_pptr = reinterpret_cast<ScMemBlockPriv**>(spp);
    ScMemBlockPriv **d_pptr = reinterpret_cast<ScMemBlockPriv**>(dpp);

    if ( spp == dpp ) { return *s_pptr; } // fixed in 0.3.0, can not attach to self
    if ( d_pptr != nullptr && *s_pptr == *d_pptr ) { return nullptr; } // source and dest. already point to one private object

    if ( *s_pptr != nullptr ) { ScMemBlockPriv::detach( reinterpret_cast<void**>( s_pptr )); *s_pptr = nullptr; } // free old content

    intptr_t tmp_ptr = 0x100; // DO NOT CHANGE THIS!
    if ( reinterpret_cast<void**>( d_pptr) > reinterpret_cast<void**>( tmp_ptr )) {
        if ( *d_pptr != nullptr ) { (*d_pptr)->incrRef(); *s_pptr = *d_pptr; }
    }

    return *s_pptr;
}

// ============================================================================
// [ static ] duplicate a object
// ============================================================================
ScMemBlockPriv *        ScMemBlockPriv :: duplicate( void **dpp )
{
    ScMemBlockPriv **d_pptr = reinterpret_cast<ScMemBlockPriv**>( dpp );
    if ( *d_pptr == nullptr ) { return nullptr; }

    // allocate new memory block
    // NOTE: here need not to call ActID_Init due to ActID_InitByCopy ocurred
    size_t sz = (*d_pptr)->size();
    ScMemBlockPriv *sp = reinterpret_cast<ScMemBlockPriv*>( gAlloc( ensureMemSize( sz ) ));
    new(sp) ScMemBlockPriv( sz, (*d_pptr)->m_f_user_proc );

    // copy old content to new
    bool user_proc_ret = false;
    if ( (*d_pptr)->m_f_user_proc != nullptr ) {
        ScMemBlock::ActInitByCopyParam par = { sp->data(), sp->size(), (*d_pptr)->data(), (*d_pptr)->size()};
        user_proc_ret = (*d_pptr)->m_f_user_proc( ScMemBlock::ActID_InitByCopy, &par, nullptr );
    }
    if ( ! user_proc_ret ) { // user do not handle it
        std::memcpy( const_cast<void*>(sp->constData()), (*d_pptr)->constData(), sz );
    }

    return sp;
}

// ============================================================================
// [ static ] duplicate if shared
// ============================================================================
ScMemBlockPriv *        ScMemBlockPriv :: dupIfShared( void **dpp )
{
    ScMemBlockPriv **d_pptr = reinterpret_cast<ScMemBlockPriv**>( dpp );
    if ( *d_pptr == nullptr ) { return nullptr; }

    if ( (*d_pptr)->currRef() > 1 ) {  // more than one refr.
        ScMemBlockPriv *dup = ScMemBlockPriv::duplicate( dpp );
        ScMemBlockPriv::detach( dpp ); // DO NOT CHANGE it!
        return dup;
    } else {
        return *d_pptr;
    }
}

// ============================================================================
// [ static ]  resize the memory block size ( since 0.2.0 )
// ============================================================================
void                    ScMemBlockPriv :: resize_cp ( void **dpp, size_t r_sz )
{
    ScMemBlockPriv **d_pptr = reinterpret_cast<ScMemBlockPriv**>( dpp );
    if ( r_sz > 0 ) {
        size_t d_sz = (*d_pptr)->size();
        if ( r_sz == d_sz ) { return; } // size is same

        // NOTE: create new object use inner memory manager
        //      here need not to call ActID_Init due to ActID_InitByCopy ocurred
        // HINT: resize always do a copy due to source maybe shared with other in multi-thread scenario
        //      In future, the reference 1 maybe considered
        ScMemBlockPriv *sp = reinterpret_cast<ScMemBlockPriv*>( gAlloc( ensureMemSize( r_sz ) ));
        new(sp) ScMemBlockPriv( r_sz, nullptr );
        sp->m_f_user_proc = (*d_pptr)->m_f_user_proc;

        // call user copy action
        bool user_proc_ret = false;
        if ( (*d_pptr)->m_f_user_proc != nullptr ) {
            ScMemBlock::ActInitByCopyParam par = { sp->data(), r_sz, (*d_pptr)->data(), d_sz };
            user_proc_ret = (*d_pptr)->m_f_user_proc( ScMemBlock::ActID_InitByCopy, &par, nullptr );
        }
        if ( ! user_proc_ret ) {
            std::memcpy( sp->data(), (*d_pptr)->constData(), ( r_sz > d_sz ? d_sz : r_sz ));
        }

        // NOTE: here must detach the d_pptr
        ScMemBlockPriv::detach( dpp );  // free old content
        *dpp = sp; // use new pointer

    } else {
        ScMemBlockPriv::detach( dpp ); *dpp = nullptr; // spec. size is 0, same as detach()
    }
}



// ////////////////////////////////////////////////////////////////////////////
//
//   warp API
//
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor empty
// ============================================================================
ScMemBlock :: ScMemBlock ( ) { m_obj = nullptr; }

// ============================================================================
// ctor
// ============================================================================
ScMemBlock :: ScMemBlock ( size_t sz )
{
    m_obj = gAlloc( ScMemBlockPriv::ensureMemSize( sz ) );
    new( m_obj ) ScMemBlockPriv( sz );
}

// ============================================================================
// ctor ( since 0.2.0 )
// ============================================================================
ScMemBlock :: ScMemBlock( size_t sz, ScMemBlock::UserProc proc, void* )
{
    m_obj = gAlloc( ScMemBlockPriv::ensureMemSize( sz ));
    new( m_obj ) ScMemBlockPriv( sz, proc );
}

// ============================================================================
// dtor
// ============================================================================
ScMemBlock :: ~ScMemBlock ( )
{
    if ( m_obj != nullptr ) {
        ScMemBlockPriv::detach( &m_obj );
    }
}

// ============================================================================
// ctor ( construct )
// ============================================================================
ScMemBlock :: ScMemBlock ( const ScMemBlock &ot )
{
    m_obj = nullptr;
    ScMemBlockPriv::attach( &m_obj, const_cast<void**>(& ot.m_obj) );
}

// ============================================================================
// operator =
// ============================================================================
ScMemBlock &            ScMemBlock :: operator = ( const ScMemBlock &ot )
{
    ScMemBlockPriv::attach( &m_obj, const_cast<void**>(& ot.m_obj));
    return *this;
}

void                    ScMemBlock :: makeNull()
{
    if ( m_obj != nullptr ) {
        ScMemBlockPriv::detach( &m_obj );
        m_obj = nullptr;
    }
}

// ============================================================================
// member functions
// ============================================================================
size_t                  ScMemBlock :: version() const { return SCMEMBLOCK_VERSION; }

size_t                  ScMemBlock :: size() const
{  return ( m_obj != nullptr ? size_t( T_MB_Priv( m_obj )->size()) : 0 ); }

const void*             ScMemBlock :: constData() const
{
    return ( m_obj != nullptr ? T_MB_Priv( m_obj )->constData() : nullptr );
}

ScMemBlock              ScMemBlock :: copy() const
{
    ScMemBlock mb; mb.m_obj = reinterpret_cast<void*>( ScMemBlockPriv::duplicate( const_cast<void**>( &m_obj )));
    return mb;
}

bool                    ScMemBlock :: dupIfShared()
{
    if ( isNull()) { return false; }
    void *old_obj = m_obj;
    m_obj = ScMemBlockPriv::dupIfShared( & m_obj );
    return ( m_obj != old_obj );
}

void *                  ScMemBlock :: dataCow()
{
    if ( isNull()) { return nullptr; }
    m_obj = ScMemBlockPriv::dupIfShared( &m_obj );
    return T_MB_Priv( m_obj )->data();
}

void                    ScMemBlock :: resize( size_t sz )
{
    if ( !isNull() ) {
        if ( sz > 0 ) {
            ScMemBlockPriv::resize_cp( &m_obj, sz );
        } else {
            makeNull();
        }
    } else {
        m_obj = gAlloc( ScMemBlockPriv::ensureMemSize( sz ) );
        new( m_obj ) ScMemBlockPriv( sz );
    }
}

bool                    ScMemBlock :: isTailBroken() const
{
    if ( !isNull()) {
        return T_MB_Priv(m_obj)->isTailBroken();
    } else {
        return false;
    }
}

bool                    ScMemBlock :: isSharedWithOther() const
{
    if ( !isNull()) {
        return T_MB_Priv(m_obj)->currRef() > 1;
    } else {
        return false;
    }
}

// ============================================================================
// [ static ] current memory counter
// ============================================================================
intptr_t                ScMemBlock :: currMemCntr()
{ return g_mem_cntr;  }

}

#endif
