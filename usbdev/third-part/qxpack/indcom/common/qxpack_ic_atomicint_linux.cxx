#ifndef QXPACK_IC_ATOMICINT_LINUX_CXX
#define QXPACK_IC_ATOMICINT_LINUX_CXX

#include "qxpack_ic_atomicint.hxx"

namespace QxPack {

IcAtomicInt :: IcAtomicInt ( intptr_t v ) : m_obj( v )
{ }

IcAtomicInt :: ~IcAtomicInt ( )
{ }

// ============================================================================
// load value from atomic
// ============================================================================
intptr_t   IcAtomicInt :: load() const
{
    auto *t_this = const_cast<IcAtomicInt*>(this);
    volatile intptr_t *vp = &t_this->m_obj;
    intptr_t  old;
    do {
        old = *vp;
        if ( __sync_bool_compare_and_swap( vp, old, old )) {
            break;
        }
    } while ( true );
    return old;
}

// ============================================================================
// store value to atomic value
// ============================================================================
void      IcAtomicInt :: store( intptr_t v )
{
    volatile intptr_t *vp = &m_obj;
    intptr_t old;
    do {
        old = *vp;
        if ( __sync_bool_compare_and_swap( vp, old, v ) ) { break; }
    } while ( true );
}

// ============================================================================
// add value on atomic value
// ============================================================================
intptr_t     IcAtomicInt :: add  ( intptr_t v )
{
    volatile intptr_t *vp = &m_obj;
    return intptr_t( __sync_add_and_fetch( vp, v ));
}

// ============================================================================
// substract value
// ============================================================================
intptr_t     IcAtomicInt :: sub  ( intptr_t v )
{   return add(-v); }

// ============================================================================
// CAS opr.
// ============================================================================
bool         IcAtomicInt :: cas  ( intptr_t old_v, intptr_t new_v )
{
    volatile intptr_t *vp = &m_obj;
    return __sync_bool_compare_and_swap( vp, old_v, new_v );
}

// ============================================================================
// [ static ] check if can used in shm
// =============================================================================
bool         IcAtomicInt :: canUseInShm()
{ return true; }


}

#endif
