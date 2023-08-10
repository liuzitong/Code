// ////////////////////////////////////////////////////////////////////////////
/*!
  @file    qxpack_ic_semaphore
  @author  night wing
  @date    2018/08
  @brief   A portable semaphore, required C++11
  @par     history
  @verbatim
          <author>   <time>   <version>  <desc>
         nightwing  2018/08   0.1.0     build
         nightwing  2018/08   0.2.0     changed counter to atomic type
         nightwing  2018/12   0.3.0     fixed in GCC 5
  @endverbatim
*/
// ////////////////////////////////////////////////////////////////////////////
#ifndef QXPACK_IC_SEMPHORE_CXX
#define QXPACK_IC_SEMPHORE_CXX

#include "qxpack_ic_semaphore.hxx"
#include "qxpack_ic_memcntr.hxx"

#include <stdint.h>
#include <mutex>              // C++11
#include <condition_variable> // C++11
#include <atomic>             // C++11
#include <climits>

#ifdef __GNUC__ // in GCC 5, close below warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif


namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//  private object
// ////////////////////////////////////////////////////////////////////////////
#define T_PrivPtr( o )  T_ObjCast( IcSemaphorePriv *, o )
class QXPACK_IC_HIDDEN  IcSemaphorePriv {

private:
    std::mutex               m_mtx;
    std::condition_variable  m_cv;
    std::atomic<int>         m_count;
public :
    IcSemaphorePriv  ( int init_cntr = 0 );
    ~IcSemaphorePriv ( ) {   }
    
    bool   post(  );
    bool   wait( int time_out = 0 );
    inline int snapCounter(  ) { return m_count.load(); }
};

// ============================================================================
// ctor
// ============================================================================
    IcSemaphorePriv :: IcSemaphorePriv ( int cntr )
{
    m_count.store( cntr < 0 ? 0 : cntr );
}

// ============================================================================
// post a semaphore to wake a thread
// ============================================================================
bool   IcSemaphorePriv :: post ( )
{
    std::unique_lock<std::mutex> lock( m_mtx ); // lock mutex until return.
    if ( m_count.load() < INT_MAX ) { m_count.fetch_add(1) ; }
    m_cv.notify_one( );
    return true;
}

// ============================================================================
// wait for a event
// ============================================================================
bool    IcSemaphorePriv :: wait ( int time_out )
{
    std::unique_lock<std::mutex> lock( m_mtx );
    bool ret = false;

    if ( time_out < 0 ) {
        while ( m_count.load() == 0 ) { m_cv.wait( lock ); }
        if ( m_count.load() > 0 ) { m_count.fetch_sub(1); ret = true; }
    } else {
        if ( time_out > 0  &&  m_count.load() <= 0 ) {
            if ( m_cv.wait_for( lock, std::chrono::milliseconds( time_out )) == std::cv_status::no_timeout ) {
                ret = true;
            }
        }
        if ( m_count.load() > 0 ) {  m_count.fetch_sub(1); ret = true; } else { ret = false; }
    }
    return ret;
}  

// ////////////////////////////////////////////////////////////////////////////
//
// IcSemaphore wrapper
//
// ////////////////////////////////////////////////////////////////////////////
     IcSemaphore :: IcSemaphore ( int cntr ) { m_obj = qxpack_ic_new( IcSemaphorePriv, cntr ); }
     IcSemaphore :: ~IcSemaphore( )          { qxpack_ic_delete( m_obj, IcSemaphorePriv );     }
bool IcSemaphore :: post ( )                 { return T_PrivPtr( m_obj )->post( ); }
bool IcSemaphore :: wait( int time_out )     { return T_PrivPtr( m_obj )->wait( time_out ); }
int  IcSemaphore :: snapCounter( )           { return T_PrivPtr( m_obj )->snapCounter( );   }
bool IcSemaphore :: notify( )                { return T_PrivPtr( m_obj )->post( ); }


}


#ifdef __GNUC__ // in GCC 5, close below warnings
#pragma GCC diagnostic pop
#endif

#endif
