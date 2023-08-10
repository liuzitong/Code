#ifndef  QXPACK_IC_IPCSEM_LINUX_CXX
#define  QXPACK_IC_IPCSEM_LINUX_CXX

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <climits>
#include <cstring>

extern "C" {
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>
}

#include "qxpack_ic_ipcsem.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"

#ifdef __GNUC__ // in GCC 5, close below warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif

#define SEM_NAME_LEN 62

namespace QxPack {

// /////////////////////////////////////////////////////////////////////
//  local [ IpcSemaphorePriv ]
// /////////////////////////////////////////////////////////////////////
#define T_PrivPtr( o )  (( IcIpcSemPriv *) o )
class QXPACK_IC_HIDDEN  IcIpcSemPriv {
private:
    sem_t  *m_sem;
    char   *m_nm_ptr;
    char    m_name[ SEM_NAME_LEN + 2 ];
public:
    IcIpcSemPriv ( const char *share_name, int init_num );
    ~IcIpcSemPriv( );
    bool post ( bool is_force = false );
    bool wait ( int time_out_ms );
    inline bool isWorked( ) const { return ( m_sem != SEM_FAILED ? true : false ); }
};

// ====================================================================
// CTOR
// ====================================================================
        IcIpcSemPriv :: IcIpcSemPriv ( const char *share_name, int init_num )
{
    // 1) init value
    m_nm_ptr = m_name;
    std::memset( m_name, 0, sizeof( m_name ));
    m_sem = SEM_FAILED;

    // 2) copy share name
    size_t name_len = std::strlen( share_name );
    if ( name_len > SEM_NAME_LEN ) {
        m_nm_ptr = ( char *) qxpack_ic_alloc( name_len + 4 );
    }
    std::memcpy( m_nm_ptr + 1, share_name, name_len );
    m_nm_ptr[ name_len + 1 ] = 0;
    m_nm_ptr[0] = '/';  // the semaphore must start with character '/somename'

    // 3) create sem.
    if ( name_len > 0 ) {
        m_sem = sem_open( m_nm_ptr, O_CREAT, S_IRWXU, init_num );
    }
}

// ==================================================================
// DTOR
// ==================================================================
        IcIpcSemPriv :: ~IcIpcSemPriv (  )
{
    if ( m_sem != SEM_FAILED ) {
        sem_close ( m_sem );
        sem_unlink( m_nm_ptr ); // 20210714 think: maybe do not call this is better.
        m_sem = SEM_FAILED;
        if ( m_nm_ptr != m_name ) { std::free( m_nm_ptr ); }
    }
}

// =================================================================
// post a signal 
// =================================================================
bool    IcIpcSemPriv :: post ( bool )
{
    if ( m_sem == SEM_FAILED ) { return false; }
    return ( sem_post( m_sem ) == 0 ? true : false );
}

// ================================================================
// wait until time out
// ================================================================
bool     IcIpcSemPriv :: wait( int time_out_ms )
{
    if ( m_sem == SEM_FAILED ) { return false; }
    int ret = -1;

    if ( time_out_ms > 0 ) { // timed wait
        for ( int x = 0; x < 16; x ++ ) {
            ret = sem_trywait( m_sem );
            if ( ret == 0 ) { break; }
        }
        if ( ret != 0 ) {
            struct timespec ts = { 0, 0 };
            ts.tv_sec  = time_out_ms / 1000; // how many sec.
            ts.tv_nsec = ( time_out_ms % 1000 ) * 1000000;
            ret = sem_timedwait( m_sem, &ts );
        }
    } else if ( time_out_ms < 0 ) { // infinite
        ret = sem_wait( m_sem );
    } else {
        ret = sem_trywait( m_sem );
    }

    return ( ret == 0 ? true : false );
}

// /////////////////////////////////////////////////////////////////////
//  exports
// /////////////////////////////////////////////////////////////////////
// =====================================================================
//! create a specified semaphore
// =====================================================================
            IcIpcSem :: IcIpcSem ( const char *share_name, int max_num )
{
    m_obj = nullptr;
    this->init ( share_name, max_num );
}

// =====================================================================
//! create a null object
// =====================================================================
            IcIpcSem :: IcIpcSem (  )
{
    m_obj = nullptr;
}
  
// =====================================================================
//! DTOR
// =====================================================================
            IcIpcSem :: ~IcIpcSem (  )
{
    this->deinit( );
}      

// =====================================================================
//! init the sempahore by name
// =====================================================================
bool         IcIpcSem :: init ( const char *share_name, int max_num )
{
    this->deinit( );
    m_obj = qxpack_ic_new( IcIpcSemPriv, share_name, max_num );
    if ( T_PrivPtr( m_obj )->isWorked( )) {
        return true;
    } else {
        this->deinit( );
        return false;
    }
}

// =====================================================================
//! deinit the semaphore.
// =====================================================================
void          IcIpcSem :: deinit( )
{
    if ( m_obj != nullptr ) {
        qxpack_ic_delete( m_obj, IcIpcSemPriv );
        m_obj = nullptr;
    }
}

// =====================================================================
//! post a wakup semaphore
// =====================================================================
bool          IcIpcSem :: post( )
{
    return ( m_obj != nullptr ? T_PrivPtr( m_obj )->post( false ) : false );
}

// =====================================================================
//! wait a semaphore
// =====================================================================
bool          IcIpcSem :: wait ( int time_out_ms )
{
    return ( m_obj != nullptr ? T_PrivPtr( m_obj )->wait( time_out_ms ) : false );
}

// =====================================================================
//! check if the semaphore is worked or not
// =====================================================================
bool          IcIpcSem :: isWorked (  ) const
{
    return ( m_obj != nullptr ? T_PrivPtr( m_obj )->isWorked( ) : false );
}



}

#ifdef __GNUC__ // in GCC 5, close below warnings
#pragma GCC diagnostic pop
#endif

#endif
