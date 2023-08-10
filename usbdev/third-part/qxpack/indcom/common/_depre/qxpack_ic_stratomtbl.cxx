#ifndef QXPACK_IC_STRATOMTBL_CXX
#define QXPACK_IC_STRATOMTBL_CXX

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
class QXPACK_IC_HIDDEN IcStrAtomItem {
private:
    std::atomic<int>     m_ref_cntr; // refer. counter
    IcStrAtomTbl::CleanUp  m_cl_func; void *m_cl_ctxt;
    size_t  m_sz;
    const char *m_str;

public :
    IcStrAtomItem( const char*, IcStrAtomTbl::CleanUp cl, void *cl_ctxt );
    IcStrAtomItem( const char *p, size_t len )
    { m_ref_cntr.store(1); m_cl_func = nullptr; m_cl_ctxt = nullptr; m_sz = len; m_str = p; }

    ~IcStrAtomItem( );
    inline const char*  strPtr() const { return m_str; }
    inline size_t       strSize()const { return m_sz;  }
    inline int          addRef() { return m_ref_cntr.fetch_add(1) + 1; }
    inline int          decRef() { return m_ref_cntr.fetch_sub(1) - 1; }

    static IcStrAtomItem*  makeItem( const char*, IcStrAtomTbl::CleanUp cl, void *cl_ctxt );
    static void          freeItem( IcStrAtomItem* );

};

IcStrAtomItem :: IcStrAtomItem ( const char *ptr, IcStrAtomTbl::CleanUp cl, void *cl_ctxt )
{
    m_ref_cntr.store(1);
    m_str = ptr;
    m_sz  = std::strlen( ptr );
    m_cl_func = cl; m_cl_ctxt = cl_ctxt;
}

IcStrAtomItem :: ~IcStrAtomItem( )
{
    if ( m_cl_func != nullptr ) {
        m_cl_func( m_str, m_cl_ctxt );
    }
}

// ============================================================================
// [ static ] make a item
// ============================================================================
IcStrAtomItem*   IcStrAtomItem ::makeItem( const char *ptr, IcStrAtomTbl::CleanUp cl, void *cl_ctxt )
{
    if ( cl != nullptr ) {
        void* mem = qxpack_ic_alloc( sizeof( IcStrAtomItem ) );
        new (mem) IcStrAtomItem( ptr, cl, cl_ctxt );
        return reinterpret_cast<IcStrAtomItem*>( mem );

    } else {
        size_t sz = std::strlen( ptr );
        size_t alloc_sz = sizeof( IcStrAtomItem ) + ( sz + 2 ) * sizeof(char);
        char* mem = reinterpret_cast<char*>( qxpack_ic_alloc( alloc_sz ));
        std::memset( mem, 0, alloc_sz );
        char *dup_str = mem + sizeof(IcStrAtomItem );
        std::memcpy( dup_str, ptr, sz );
        new (mem) IcStrAtomItem( dup_str, sz );
        return reinterpret_cast<IcStrAtomItem*>(mem);
    }
}

// ============================================================================
// [ static ] free a item
// ============================================================================
void          IcStrAtomItem :: freeItem( IcStrAtomItem *item )
{
    if ( item != nullptr ) {
        item->~IcStrAtomItem();
        qxpack_ic_free( item );
    }
}

// ////////////////////////////////////////////////////////////////////////////
// private
// ////////////////////////////////////////////////////////////////////////////
#define T_PrivPtr( o )   T_ObjCast( IcStrAtomTblPriv*, o )
class QXPACK_IC_HIDDEN  IcStrAtomTblPriv {
    typedef IcStrAtomItem* IcStrAtomItemPtr;
    struct QXPACK_IC_HIDDEN LessCmp {
        inline bool operator ()( const IcStrAtomItemPtr &lhs , const IcStrAtomItemPtr &rhs ) const
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
    std::set<IcStrAtomItemPtr,LessCmp>  m_set;
public:
    explicit  IcStrAtomTblPriv( );
    ~IcStrAtomTblPriv( );
    void      clear();
    inline std::set<IcStrAtomItemPtr,LessCmp>&  setRef() { return m_set; }
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
void         IcStrAtomTblPriv :: clear( )
{
    auto itr = m_set.cbegin();
    while ( itr != m_set.cend()) {
        auto *item = *( itr ++ );
        IcStrAtomItem::freeItem( item );
    }
    std::set<IcStrAtomItem*,LessCmp> tmp;
    m_set.swap( tmp );
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
// add new atom string
// ============================================================================
uintptr_t    IcStrAtomTbl :: add( const char *ptr, CleanUp cl, void *cl_ctxt )
{
    auto *priv = T_PrivPtr( m_obj );
    auto &set  = priv->setRef();

    size_t sz = std::strlen( ptr );
    IcStrAtomItem tmp( ptr, sz ); // a temp. ( need not to free )
    std::set<IcStrAtomItem*>::const_iterator citr = set.find( &tmp );
    if ( citr != set.cend() ) {
        auto *item = *citr;
        item->addRef();
        if ( cl != nullptr ) { cl( ptr, cl_ctxt ); }
        return reinterpret_cast<uintptr_t>(item);
    } else {
        auto *item = IcStrAtomItem::makeItem( ptr, cl, cl_ctxt );
        set.insert( item );
        return reinterpret_cast<uintptr_t>(item);
    }
}

// ============================================================================
// remove the spec. string
// ============================================================================
void       IcStrAtomTbl :: rmv( const char *ptr, CleanUp cl, void *cl_ctxt )
{
    auto *priv = T_PrivPtr( m_obj );
    auto &set  = priv->setRef();

    size_t sz = std::strlen( ptr );
    IcStrAtomItem tmp( ptr, sz );
    std::set<IcStrAtomItem*>::iterator itr = set.find( &tmp );

    if ( cl != nullptr ) { cl( ptr, cl_ctxt ); }

    if ( itr != set.end()) {
        auto *item = *itr;
        if ( item->decRef() < 1 ) {
            set.erase( itr );
            IcStrAtomItem::freeItem( item );
        }
    }

}

// ============================================================================
// query the spec. string atom
// ============================================================================
uintptr_t  IcStrAtomTbl :: query( const char *ptr, CleanUp cl, void *cl_ctxt ) const
{
    auto *priv = T_PrivPtr( m_obj );
    auto &set  = priv->setRef();

    size_t sz = std::strlen( ptr );
    IcStrAtomItem tmp( ptr, sz );
    std::set<IcStrAtomItem*>::const_iterator citr = set.find( &tmp );

    if ( cl != nullptr ) { cl( ptr, cl_ctxt ); }

    if ( citr != set.cend() ) {
        return reinterpret_cast<uintptr_t>(*citr);
    } else {
        return IcStrAtomTbl::invalidValue();
    }
}

// ============================================================================
// [ static ]  invalid value of atom
// ============================================================================
uintptr_t    IcStrAtomTbl :: invalidValue( )
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
            g_ref_cntr.store(0);
            g_ptr.store(nullptr);
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
