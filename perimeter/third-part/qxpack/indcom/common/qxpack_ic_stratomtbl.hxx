#ifndef QXPACK_IC_IcStrAtomTbl_HXX
#define QXPACK_IC_IcStrAtomTbl_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"

#include <cstdint>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
//                 String atom table [ thread-safe ]
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcStrAtomTbl  QXPACK_IC_FINAL {
public:
    typedef void (* CleanUp )( const char *, void* );

    explicit IcStrAtomTbl( );

    ~IcStrAtomTbl( );

    size_t     count() const;

    uintptr_t  add( const char *, CleanUp cl, void *cl_ctxt = nullptr );

    uintptr_t  add( const char *,  size_t len = 0, CleanUp cl = nullptr, void *cl_ctxt = nullptr );

    void       rmv( const char *,  size_t len = 0, CleanUp cl = nullptr, void *cl_ctxt = nullptr );

    void       rmv( uintptr_t );

    uintptr_t  query( const char*, size_t len = 0, CleanUp cl = nullptr, void *cl_ctxt = nullptr ) const;

    uintptr_t  queryAdd( const char*, size_t len = 0, CleanUp cl = nullptr, void *cl_ctxt = nullptr );

    // return the atom string pointer
    // NOTE: the atom must be valid atom.!
    const char*  strPtr( uintptr_t, size_t *len ) const;

    static uintptr_t     invalidValue( );

    static IcStrAtomTbl *  getInstance ( );

    static bool          freeInstance( );

private:
    void *m_obj;
    QXPACK_IC_DISABLE_COPY( IcStrAtomTbl )
};


}

#endif

