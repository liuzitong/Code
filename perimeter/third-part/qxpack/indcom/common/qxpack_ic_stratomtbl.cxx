#ifndef QXPACK_IC_IcStrAtomTbl_CXX
#define QXPACK_IC_IcStrAtomTbl_CXX

#include "qxpack_ic_stratomtbl.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"

#include <cstdlib>
#include <cstring>
#include <new>
#include <atomic>
#include <mutex>
#include <set>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
// string atom item
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN StrAtomItem {
private:
    std::atomic<int>     m_ref_cntr; // refer. counter
    IcStrAtomTbl::CleanUp  m_cl_func; void *m_cl_ctxt;
    size_t  m_sz;  const char *m_str;
public :
    StrAtomItem( const char*,   size_t len, IcStrAtomTbl::CleanUp cl, void *cl_ctxt );
    StrAtomItem( const char *p, size_t len )
    { m_ref_cntr.store(1); m_cl_func = nullptr; m_cl_ctxt = nullptr; m_sz = len; m_str = p; }
    ~StrAtomItem( );

    inline auto  strPtr() const -> const char* { return m_str; }
    inline auto  strSize()const -> size_t      { return m_sz;  }
    inline auto  addRef()       -> int         { return m_ref_cntr.fetch_add(1) + 1; }
    inline auto  decRef()       -> int         { return m_ref_cntr.fetch_sub(1) - 1; }

    static auto  makeItem( const char*, size_t len, IcStrAtomTbl::CleanUp cl, void *cl_ctxt ) -> StrAtomItem*;
    static auto  freeItem( StrAtomItem* ) -> void;
};

StrAtomItem :: StrAtomItem ( const char *ptr, size_t len, IcStrAtomTbl::CleanUp cl, void *cl_ctxt )
{
    m_ref_cntr.store(1);
    m_str = ptr;
    m_sz  = ( len > 0 ? len : std::strlen( ptr ));
    m_cl_func = cl; m_cl_ctxt = cl_ctxt;
}

StrAtomItem :: ~StrAtomItem( )
{
    if ( m_cl_func != nullptr ) {
        m_cl_func( m_str, m_cl_ctxt );
    }
}

// ============================================================================
// [ static ] make a item
// ============================================================================
auto     StrAtomItem ::makeItem( const char *ptr, size_t len, IcStrAtomTbl::CleanUp cl, void *cl_ctxt ) -> StrAtomItem*
{
    if ( cl != nullptr ) {
        void* mem = qxpack_ic_alloc( sizeof( StrAtomItem ) );
        new (mem) StrAtomItem( ptr, len, cl, cl_ctxt );
        return reinterpret_cast<StrAtomItem*>( mem );

    } else {
        size_t sz = ( len > 0 ? len : std::strlen( ptr ));
        size_t alloc_sz = ( sz + 2 ) * sizeof(char) + sizeof( StrAtomItem );
        char* mem = reinterpret_cast<char*>( qxpack_ic_alloc( alloc_sz ));
        std::memset( mem, 0, alloc_sz );
        char *dup_str = mem + sizeof(StrAtomItem );
        std::memcpy( dup_str, ptr, sz );
        new (mem) StrAtomItem( dup_str, sz );
        return reinterpret_cast<StrAtomItem*>(mem);
    }
}

// ============================================================================
// [ static ] free a item
// ============================================================================
auto     StrAtomItem :: freeItem( StrAtomItem *item ) -> void
{
    if ( item != nullptr ) {
        item->~StrAtomItem();
        qxpack_ic_free( item );
    }
}

// ////////////////////////////////////////////////////////////////////////////
// private
// ////////////////////////////////////////////////////////////////////////////
#define T_PrivPtr( o )   T_ObjCast( IcStrAtomTblPriv*, o )
class QXPACK_IC_HIDDEN  IcStrAtomTblPriv {
    typedef StrAtomItem* StrAtomItemPtr;
    struct QXPACK_IC_HIDDEN LessCmp {
        inline bool operator ()( const StrAtomItemPtr &lhs , const StrAtomItemPtr &rhs ) const
        {
            if ( lhs == rhs ) { return false; }
            if ( lhs->strPtr()  == rhs->strPtr() ) { return false; }

            int cmp_rsl = std::memcmp( lhs->strPtr(), rhs->strPtr(), lhs->strSize() <= rhs->strSize() ? lhs->strSize() : rhs->strSize() );
            if ( cmp_rsl < 0 ) { return true;  } // lhs < rhs
            if ( cmp_rsl > 0 ) { return false; } // lhs > rhs
            if ( lhs->strSize() < rhs->strSize()) { return true; } // lhs == rhs
            return false; // lhs.size > rhs.size, and prefix segment are same...
        }
    };   
private:
    std::set<StrAtomItemPtr,LessCmp>  m_set;
    std::mutex  m_locker;
protected:
           auto  clear() -> void;
public:
    explicit  IcStrAtomTblPriv( );
    ~IcStrAtomTblPriv( );
    inline auto  lockerRef() -> std::mutex& { return m_locker; }
    inline auto  setRef()    -> std::set<StrAtomItemPtr,LessCmp>& { return m_set; }
};

// ============================================================================
// ctor
// ============================================================================
IcStrAtomTblPriv :: IcStrAtomTblPriv ( )
{

}

// ============================================================================
// dtor
// ============================================================================
IcStrAtomTblPriv :: ~IcStrAtomTblPriv ( )
{
    clear();
}

// ============================================================================
// clear all atom item in set
// ============================================================================
auto     IcStrAtomTblPriv :: clear( ) -> void
{
    m_locker.lock();

    auto itr = m_set.cbegin();
    while ( itr != m_set.cend()) {
        StrAtomItem::freeItem( *( itr ++ ) );
    }
    std::set<StrAtomItem*,LessCmp> tmp;
    m_set.swap( tmp );

    m_locker.unlock();
}



// ////////////////////////////////////////////////////////////////////////////
// public API
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcStrAtomTbl :: IcStrAtomTbl( )
{
    m_obj = qxpack_ic_new( IcStrAtomTblPriv );
}

// ============================================================================
// dtor
// ============================================================================
IcStrAtomTbl :: ~IcStrAtomTbl( )
{
    qxpack_ic_delete( m_obj, IcStrAtomTblPriv );
    m_obj = nullptr;
}

// ============================================================================
// the count of the string atoms
// ============================================================================
auto     IcStrAtomTbl :: count() const -> size_t
{
    auto *priv = T_PrivPtr(m_obj);

    priv->lockerRef().lock();
    size_t cnt = priv->setRef().size();
    priv->lockerRef().unlock();

    return cnt;
}

// ============================================================================
// add new atom string
// ============================================================================
auto     IcStrAtomTbl :: add( const char *ptr, size_t len, CleanUp cl, void *cl_ctxt ) -> uintptr_t
{
    auto *priv = T_PrivPtr( m_obj );
    auto &set  = priv->setRef();

    priv->lockerRef().lock();

    size_t sz = ( len > 0 ? len : std::strlen( ptr ));
    StrAtomItem tmp( ptr, sz ); // a temp. ( need not to free )
    std::set<StrAtomItem*>::iterator citr = set.find( &tmp );

    StrAtomItem *item = nullptr;
    if ( citr != set.end() ) {
        item = *citr;
        item->addRef();
        if ( cl != nullptr ) { cl( ptr, cl_ctxt ); }
    } else {
        // here need not to call cleanup due to added this item
        item = StrAtomItem::makeItem( ptr, sz, cl, cl_ctxt );
        set.insert( item );
    }

    priv->lockerRef().unlock();

    return reinterpret_cast<uintptr_t>(item);
}

auto     IcStrAtomTbl :: add ( const  char *ptr,  CleanUp cl, void *cl_ctxt  ) -> uintptr_t
{   return add ( ptr, 0, cl, cl_ctxt ); }

// ============================================================================
// remove the spec. string
// ============================================================================
auto     IcStrAtomTbl :: rmv( const char *ptr, size_t len, CleanUp cl, void *cl_ctxt ) -> void
{
    auto *priv = T_PrivPtr( m_obj );
    auto &set  = priv->setRef();

    priv->lockerRef().lock();

    size_t sz = ( len > 0 ? len : std::strlen( ptr ));
    StrAtomItem tmp( ptr, sz );
    auto itr = set.find( &tmp );
    if ( cl != nullptr ) { cl( ptr, cl_ctxt ); }

    if ( itr != set.end()) {
        auto item = *itr;
        if ( item->decRef() < 1 ) {
            set.erase( itr );
            StrAtomItem::freeItem( item );
        }
    }

    priv->lockerRef().unlock();
}

// ============================================================================
// remove by handle
// ============================================================================
auto     IcStrAtomTbl :: rmv ( uintptr_t hde ) -> void
{
    size_t len = 0;
    const char *sp = strPtr( hde, &len );
    rmv( sp, len );
}

// ============================================================================
// query the spec. string atom
// ============================================================================
auto     IcStrAtomTbl :: query( const char *ptr, size_t len, CleanUp cl, void *cl_ctxt ) const -> uintptr_t
{
    auto *priv = T_PrivPtr( m_obj );
    auto &set  = priv->setRef();

    priv->lockerRef().lock();

    size_t sz = ( len > 0 ? len : std::strlen( ptr ));
    StrAtomItem tmp( ptr, sz );
    std::set<StrAtomItem*>::const_iterator citr = set.find( &tmp );
    if ( cl != nullptr ) { cl( ptr, cl_ctxt ); }
    StrAtomItem *item = ( citr != set.cend() ? *citr : nullptr );

    priv->lockerRef().unlock();

    return ( item != nullptr ? reinterpret_cast<uintptr_t>(item) : IcStrAtomTbl::invalidValue());
}

// ============================================================================
//  query the string, if not existed, add it
// ============================================================================
auto     IcStrAtomTbl :: queryAdd( const char *ptr, size_t len, CleanUp cl, void *cl_ctxt ) -> uintptr_t
{
     auto *priv = T_PrivPtr( m_obj );
     auto &set  = priv->setRef();

     priv->lockerRef().lock();

     size_t sz = ( len > 0 ? len : std::strlen( ptr ));
     StrAtomItem tmp( ptr, sz );
     std::set<StrAtomItem*>::iterator itr = set.find(&tmp);

     StrAtomItem *item = nullptr;
     if ( itr != set.end() ) {
         if ( cl != nullptr ) { cl( ptr, cl_ctxt ); }
         item = *itr;
     } else {
         item = StrAtomItem::makeItem( ptr, sz, cl, cl_ctxt );
         set.insert( item );
     }

     priv->lockerRef().unlock();

     return reinterpret_cast<uintptr_t>(item);
}

// ============================================================================
// return the string pointer by spec. atom value
// ============================================================================
auto     IcStrAtomTbl :: strPtr( uintptr_t str_atom, size_t *len ) const -> const char*
{
    if ( str_atom == IcStrAtomTbl::invalidValue()) { return nullptr; }
    auto *item = reinterpret_cast<StrAtomItem*>( str_atom );
    if ( len != nullptr ) { *len = item->strSize(); }
    return item->strPtr();
}

// ============================================================================
// [ static ]  invalid value of atom
// ============================================================================
auto     IcStrAtomTbl :: invalidValue( ) -> uintptr_t
{   return 0; }

// ============================================================================
// [ static ] build an global instance
// ============================================================================
static std::atomic<void*>  g_ptr(nullptr);
static std::atomic<int>    g_ref_cntr(0);
static std::mutex          g_locker;

IcStrAtomTbl*   IcStrAtomTbl :: getInstance ( )
{
    g_locker.lock();

    IcStrAtomTbl *tbl = reinterpret_cast<IcStrAtomTbl*>( g_ptr.load( std::memory_order_seq_cst ));
    if ( tbl == nullptr ) {
        tbl = qxpack_ic_new( IcStrAtomTbl );
        g_ptr.store( tbl );
        g_ref_cntr.store(1);
    } else {
        g_ref_cntr.fetch_add(1);
    }

    g_locker.unlock();
    return tbl;
}

bool         IcStrAtomTbl :: freeInstance( )
{
    g_locker.lock();

    IcStrAtomTbl *tbl = reinterpret_cast<IcStrAtomTbl*>( g_ptr.load( std::memory_order_seq_cst ));
    if ( tbl != nullptr ) {
        if ( g_ref_cntr.fetch_sub(1) - 1 < 1 ) {
            g_ptr.store(nullptr);
            g_ref_cntr.store(0);
        } else {
            tbl = nullptr;
        }
    }

    g_locker.unlock();

    if ( tbl != nullptr ) { qxpack_ic_delete( tbl, IcStrAtomTbl ); }
    return ( tbl != nullptr );
}

}

#endif
