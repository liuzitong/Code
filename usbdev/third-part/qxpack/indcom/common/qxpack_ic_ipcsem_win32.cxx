#ifndef  QXPACK_IC_IPCSEM_WIN32_CXX
#define  QXPACK_IC_IPCSEM_WIN32_CXX

#include "qxpack/indcom/common/qxpack_ic_global.hxx"
#include "qxpack_ic_ipcsem.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"

#include <cstring>
#include <cstdio>
#include <climits>
#include <windows.h>

namespace QxPack {

// /////////////////////////////////////////////////////////////////////
//  local [ IpcSemaphorePriv ]
// /////////////////////////////////////////////////////////////////////
#define T_PrivPtr( o )  T_ObjCast( IcIpcSemPriv*, o )
class QXPACK_IC_HIDDEN  IcIpcSemPriv {
private:
    HANDLE  m_sem;
    LONG    m_last_value;
public:
    IcIpcSemPriv ( const char *share_name, int init_num );
    ~IcIpcSemPriv(  );
    bool post ( bool is_force = false );
    bool wait ( int time_out_ms );
    inline bool isWorked( ) const { return ( m_sem != NULL ? true : false ); }
};

// ====================================================================
// CTOR
// ====================================================================
        IcIpcSemPriv :: IcIpcSemPriv ( const char *share_name, int init_num )
{
    // 1) convert the name
    WCHAR  w_tmp[ 256 ];  // for Windows, wchar_t is always 2 bytes
    WCHAR *w_share_name = & w_tmp[0];
    int a_count = int( strlen( share_name ));
    int w_count = ::MultiByteToWideChar( CP_ACP, 0, share_name, a_count, NULL, 0 ) + 1;
    if ( w_count > 250 ) { w_share_name = ( wchar_t *) std::malloc( w_count * 2 ); }
    ::MultiByteToWideChar( CP_ACP, 0, share_name, a_count, w_share_name, w_count );
    w_share_name[ w_count - 1 ] = 0;    

    // 2) create or open the share name semaphore
    HANDLE sem = NULL; IcLCG lcg;
    do {
        // if the semaphore is already exist, GetLastError( ) will return ERROR_ALREADY_EXISTS
        sem = ::CreateSemaphoreW( NULL, init_num, LONG_MAX, w_share_name );
        if ( sem != NULL ) { break; }
        ::Sleep( 16 + ( lcg.value() % 10 ));
    } while ( true );
    if ( w_share_name != &w_tmp[0] ) { std::free( w_share_name ); }

    // 3) assign variable
    m_sem = sem; 
    m_last_value = 0;
}

// ==================================================================
// DTOR
// ==================================================================
        IcIpcSemPriv :: ~IcIpcSemPriv (  )
{
    if ( m_sem != NULL ) {
        ::CloseHandle( m_sem ); 
        m_sem = NULL; 
    }
}

// =================================================================
// post a signal 
// =================================================================
bool    IcIpcSemPriv :: post ( bool )
{
    if ( m_sem == NULL ) { return false; }
    ::ReleaseSemaphore ( m_sem, 1, & m_last_value );
    return true;
}

// ================================================================
// wait until time out
// ================================================================
bool     IcIpcSemPriv :: wait( int time_out_ms )
{
    if ( m_sem == NULL ) { return false; }

    // time out ms is 0, means quick try
    DWORD dwRet;
    if ( time_out_ms > 0 ) {
        for ( int x = 0; x < 16; x ++ ) {
            dwRet = ::WaitForSingleObject( m_sem, 0 );
            if ( dwRet == WAIT_OBJECT_0 ) { break; }
        }
        if ( dwRet != WAIT_OBJECT_0 ) {
            dwRet = ::WaitForSingleObject( m_sem, time_out_ms );
        }
    } else if ( time_out_ms < 0 ) {
        dwRet = ::WaitForSingleObject( m_sem, INFINITE );
    } else {
        dwRet = ::WaitForSingleObject( m_sem, 0 );
    }

    bool   ret   = false;
    switch ( dwRet ) {
    case WAIT_OBJECT_0 : ret = true; break;
    case WAIT_TIMEOUT  : ret = false;break;
    default: ret = false; break;
    }
    return ret;
}

// /////////////////////////////////////////////////////////////////////
//  exports
// /////////////////////////////////////////////////////////////////////
// =====================================================================
// create a specified semaphore
// =====================================================================
IcIpcSem :: IcIpcSem ( const char *share_name, int init_num )
{
    m_obj = nullptr;
    this->init ( share_name, init_num );
}

// =====================================================================
// create a null object
// =====================================================================
IcIpcSem :: IcIpcSem (  )
{
    m_obj = nullptr;
}
  
// =====================================================================
// dtor
// =====================================================================
IcIpcSem :: ~IcIpcSem (  )
{
    this->deinit(  );
}      

// =====================================================================
//! init the sempahore by name
// =====================================================================
bool         IcIpcSem :: init ( const char *share_name, int init_num )
{
    this->deinit( );
    m_obj = qxpack_ic_new( IcIpcSemPriv, share_name, init_num );
    if ( T_PrivPtr( m_obj )->isWorked( )) {
        return true;
    } else {
        this->deinit( );
        return false;
    }
}

// =====================================================================
// deinit the semaphore.
// =====================================================================
void          IcIpcSem :: deinit( )
{
    if ( m_obj != nullptr ) {
        qxpack_ic_delete( m_obj, IcIpcSemPriv );
        m_obj = nullptr;
    }
}

// =====================================================================
// post a wakup semaphore
// =====================================================================
bool          IcIpcSem :: post( )
{
    return ( m_obj != nullptr ? T_PrivPtr( m_obj )->post( false ) : false );
}

// =====================================================================
// wait a semaphore
// =====================================================================
bool          IcIpcSem :: wait ( int time_out_ms )
{
    return ( m_obj != nullptr ? T_PrivPtr( m_obj )->wait( time_out_ms ) : false );
}

// =====================================================================
// check if the semaphore is worked or not
// =====================================================================
bool          IcIpcSem :: isWorked ( ) const
{
    return ( m_obj != nullptr ? T_PrivPtr( m_obj )->isWorked( ) : false );
}



}

#endif
