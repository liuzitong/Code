/* ///////////////////////////////////////////////////////////////////////// */
/*! 
  @file    qxpack_ic_ipcsem.hxx
  @author  night wing
  @date    2019/1, 2019/05
  @brief   The System semaphore object
  @par     History
  @verbatim
          <author>    <time>   <version>  <desc>                                   
         nightwing   2019/01   0.1.0     build
  @endverbatim
*/
/* ///////////////////////////////////////////////////////////////////////// */
#ifndef  QXPACK_IC_IPCSEM_HXX
#define  QXPACK_IC_IPCSEM_HXX

#include <stdint.h>
#include "qxpack/indcom/common/qxpack_ic_def.h"

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//! IcIpcSem
/*!
   the system semaphore, used for communicate between 2 process
*/
// //////////////////////////////////////////////////////////////////////////// 
class QXPACK_IC_API IcIpcSem QXPACK_IC_FINAL {

public:
    /*!
     * @brief create a semaphore and initalize it
     * @param share_name [in] the name of the Semaphore
     * @param inital_num [in] the initalize number of the semaphore
     */
    IcIpcSem ( const char *share_name, int inital_num );

    /*!
     * @brief create a default sempahore
     * @note  before use, call init() function to initalize it
     */
    explicit IcIpcSem( );

    /*!
     * @brief destroy semaphore
     */
    ~IcIpcSem( );

    //! return the semaphore is worked or not
    bool  isWorked( ) const;

    //! increase the sempahore counter
    bool  post( );

    /*!
     * @brief  wait for a resource
     * @param  time_out_ms [in] the maximum million-second to wait
     * @return true for there is a resource
     * @note   'time_out_ms' set to 0 means check it only. < 0 means infinite wait time.
     */
    bool  wait ( int time_out_ms = 0 );

    /*!
     * @brief initalize the sempahore
     * @param share_name [in] the shared name of the semaphore
     * @param init_num   [in] the initalize number of the semaphore
     * @return true for initalize it
     * @note  if the sempahore is worked, then it will detach from the old \n
     *    system semaphore, and initalize as new sempahore
     */
    bool  init ( const char *share_name, int init_num );

    //! detach from system semaphore
    void  deinit ( );

private:
    // =================================================================
    // var.
    // =================================================================
    void *m_obj;
    QXPACK_IC_DISABLE_COPY( IcIpcSem )
};
    
}

#endif
