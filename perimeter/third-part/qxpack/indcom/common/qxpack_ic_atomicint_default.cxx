#ifndef QXPACK_IC_ATOMICINT_DEFAULT_CXX
#define QXPACK_IC_ATOMICINT_DEFAULT_CXX

#include "qxpack_ic_atomicint.hxx"
#include "qxpack_ic_global.hxx"
#include <atomic>
#include <thread>
#include <chrono>

namespace QxPack {

IcAtomicInt :: IcAtomicInt ( intptr_t v )
{
    auto *ap = new std::atomic<intptr_t>();
    ap->store(v);
    m_obj = reinterpret_cast<intptr_t>(ap);
}

IcAtomicInt :: ~IcAtomicInt ( )
{
    auto *ap = reinterpret_cast<std::atomic<intptr_t>*>(m_obj);
    delete ap;
}

intptr_t   IcAtomicInt :: load() const
{
    auto *ap = reinterpret_cast<std::atomic<intptr_t>*>(m_obj);
    return ap->load();
}

void       IcAtomicInt :: store( intptr_t v )
{
    auto *ap = reinterpret_cast<std::atomic<intptr_t>*>(m_obj);
    ap->store(v);
}

intptr_t   IcAtomicInt ::  add  ( intptr_t v )
{
    auto *ap = reinterpret_cast<std::atomic<intptr_t>*>(m_obj);
    return ap->fetch_add(v, std::memory_order_seq_cst ) + 1;
}

intptr_t   IcAtomicInt :: sub  ( intptr_t v )
{
    auto *ap = reinterpret_cast<std::atomic<intptr_t>*>(m_obj);
    return ap->fetch_sub(v, std::memory_order_seq_cst) - 1;
}

bool       IcAtomicInt :: cas( intptr_t old_v, intptr_t new_v )
{
    auto *ap = reinterpret_cast<std::atomic<intptr_t>*>(m_obj);
    return ap->compare_exchange_strong( old_v, new_v );
}

// ============================================================================
// [ static ] check if this object can used in shared memory.
// ============================================================================
bool      IcAtomicInt :: canUseInShm() { return false; }

}
#endif
