#ifndef QXPACK_IC_STRATOMTBL_HXX
#define QXPACK_IC_STRATOMTBL_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include <cstdint>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
// the string atom table
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcStrAtomTbl {
public:
    typedef void (* CleanUp )( const char *, void* );

    explicit IcStrAtomTbl( );

    ~IcStrAtomTbl( );

    uintptr_t  add( const char *, CleanUp cl = nullptr, void *cl_ctxt = nullptr );

    void       rmv( const char *, CleanUp cl = nullptr, void *cl_ctxt = nullptr );

    uintptr_t  query( const char*, CleanUp cl = nullptr, void *cl_ctxt = nullptr ) const;

    static uintptr_t     invalidValue( );

    static IcStrAtomTbl *  getInstance ( );

    static bool          freeInstance( );

private:
    void *m_obj;
    QXPACK_IC_DISABLE_COPY( IcStrAtomTbl )
};


}

#endif

