/* ///////////////////////////////////////////////////////////////////////// */
/*! 
  @file    qxpack_ic_semaphore.hxx
  @author  night wing
  @date    2018/06
  @brief   The IcSemphore  protocol
*/
/* ///////////////////////////////////////////////////////////////////////// */
#ifndef  QXPACK_IC_SEMAPHORE_HXX
#define  QXPACK_IC_SEMAPHORE_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include <cstdint>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief   the semaphore implicit by
 * @details manage the resource of shared.
*/
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API IcSemaphore QXPACK_IC_FINAL {

public:
    /*!
     * @brief  ctor
     * @param  init_val [in] the initalize value of the semaphore
     */
    IcSemaphore( int init_val = 0 );

    //! dtor
    ~IcSemaphore(  );

    /*!
     * @brief  post 1 resource counter
     * @return always return true.
     */
    bool   post( );

    /*!
     * @brief wait until the timeout
     * @param  time_out  [in]  how many millionseconds to wait for a resource
     * @return  true means get a resource.
     * @note if resource is avaliable, decrease the counter and return true, \n
     *       otherwise, return false
     */
    bool   wait( int time_out = 0 );

    //! return the number of current semaphore.
    int    snapCounter( );

    //! same as post()
    bool   notify( );

private:
    void *m_obj;
    QXPACK_IC_DISABLE_COPY( IcSemaphore )
};
    
}

#endif
