#ifndef QXPACK_IC_ATOMICINT_HXX
#define QXPACK_IC_ATOMICINT_HXX

#include "qxpack_ic_def.h"
#include <cstdint>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
// provide the atomic integer value
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcAtomicInt QXPACK_IC_FINAL {
public:
    explicit IcAtomicInt ( intptr_t v = 0 );
    ~IcAtomicInt();

    intptr_t  load() const;
    void      store( intptr_t  );
    intptr_t  add  ( intptr_t  );
    intptr_t  sub  ( intptr_t  );
    bool      cas  ( intptr_t old_val, intptr_t new_val );

    static bool canUseInShm();

private:
    intptr_t  m_obj;
    QXPACK_IC_DISABLE_COPY( IcAtomicInt )
};

}

#endif

