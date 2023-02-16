#ifndef QXPACK_IC_ATOMICINT_WIN32_CXX
#define QXPACK_IC_ATOMICINT_WIN32_CXX

#include "qxpack_ic_atomicint.hxx"
#include "qxpack_ic_global.hxx"
#include <windows.h>

namespace QxPack {

IcAtomicInt :: IcAtomicInt ( intptr_t v ) : m_obj( v )
{}

IcAtomicInt :: ~IcAtomicInt ( )
{}

// ============================================================================
// return current value from this atomic
// ============================================================================
intptr_t   IcAtomicInt :: load() const
{
    auto *t_this = const_cast<IcAtomicInt*>(this);
    intptr_t old; volatile intptr_t *vp = &t_this->m_obj;

    do {
        old = *vp;
        if ( ::InterlockedCompareExchangePointer(
                 reinterpret_cast< volatile PVOID *>( vp ),
            PVOID( old ), PVOID( old )) == PVOID( old )
        ) { break; }
    } while ( TRUE );

    return old;
}

// ============================================================================
// store the value into this atomic
// ============================================================================
void      IcAtomicInt :: store( intptr_t v )
{
    intptr_t old; volatile intptr_t *vp = &m_obj;

    do {
        old = *vp;
        if ( ::InterlockedCompareExchangePointer(
                 reinterpret_cast< volatile PVOID*>( vp ),
                 PVOID( v ), PVOID( old )
             ) == PVOID( old )  // if cas OK, return the current value ( old )
         ) { break; }
    } while ( true );
}

// ============================================================================
// add value on this atomic, return added value
// ============================================================================
intptr_t  IcAtomicInt :: add ( intptr_t v )
{
    volatile intptr_t *curr_atom = &m_obj;

    intptr_t curr_val;
    do {
        curr_val = *curr_atom;
    } while (
        ::InterlockedCompareExchangePointer(
                 reinterpret_cast< volatile PVOID *>( curr_atom ),
            PVOID( curr_val + v ), PVOID( curr_val )
        ) != PVOID( curr_val ) /* return curr_val means exchanged OK */
    );
    return curr_val + v;
}

// ============================================================================
// return substracted value.
// ============================================================================
intptr_t  IcAtomicInt :: sub ( intptr_t v )
{  return add( -v ); }

// ============================================================================
// compare and swap strongly
// ============================================================================
bool      IcAtomicInt :: cas ( intptr_t old_val, intptr_t new_val )
{
    return ::InterlockedCompareExchangePointer(
                reinterpret_cast< volatile PVOID *>( &m_obj ),
            PVOID( new_val ), PVOID( old_val )
    ) == PVOID( old_val );  /* return curr_val means exchanged OK */
}

// ============================================================================
// [ static ] check if can used in shm
// =============================================================================
bool         IcAtomicInt :: canUseInShm()
{ return true; }


}

#endif

