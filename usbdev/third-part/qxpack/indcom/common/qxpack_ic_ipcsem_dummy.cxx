#ifndef QXPACK_IC_IPCSEM_DUMMY_CXX
#define QXPACK_IC_IPCSEM_DUMMY_CXX

#include "qxpack_ic_ipcsem.hxx"

namespace QxPack {

// ============================================================
// CTOR
// ============================================================
     IcIpcSem :: IcIpcSem () { m_obj = nullptr; }

// ============================================================
// CTOR with parameter
// ============================================================
     IcIpcSem :: IcIpcSem ( const char *share_name, int inital_num )
{
    QXPACK_IC_UNUSED(share_name);
    QXPACK_IC_UNUSED(inital_num);
    m_obj = nullptr;
}

// ===========================================================
// DTOR
// ===========================================================
    IcIpcSem :: ~IcIpcSem( ) { }

// ===========================================================
// check if syssem is work
// ============================================================
bool  IcIpcSem :: isWorked( ) const { return false; }

// ============================================================
// post a signal
// ============================================================
bool   IcIpcSem :: post( ) { return false; }

// ============================================================
// wait for a signal
// ============================================================
bool    IcIpcSem :: wait ( int time_out_ms )
{
    QXPACK_IC_UNUSED( time_out_ms );
    return false;
}

// ============================================================
// initalize the sem.
// ============================================================
bool     IcIpcSem :: init ( const char *share_name, int init_num )
{
    QXPACK_IC_UNUSED( share_name ); QXPACK_IC_UNUSED( init_num );
    return false;
}

// ============================================================
// deinit the system sem.
// ============================================================
void     IcIpcSem :: deinit ( ) { return; }


}

#endif
