/* ///////////////////////////////////////////////////////////////////// */
/*! 
  @file    NwkUsbObj2.hxx
  @author  night wing
  @date    2016/09
  @brief   The file declare the NwkUsbObj2 Protocol
  @par     History
  @verbatim
          <author>   <time>   <version>  <desc>                                   
         nightwing   2016/09   0.1.0     build this module             
  @endverbatim
*/
/* ///////////////////////////////////////////////////////////////////// */
#ifndef  SCIPACK_NWKUSBOBJ2_HXX
#define  SCIPACK_NWKUSBOBJ2_HXX


/*!
  @addtogroup  SciPack
  @{
*/

/*!
  @addtogroup  devel  
  @{
*/

/*!
  @addtogroup  devel_Exported_Classes
  @{
*/


/* /////////////////////////////////////////////////////////////////////
   config
   ///////////////////////////////////////////////////////////////////// 
*/
#if !defined( SCIPACK_HIDDEN )
#if defined( _WIN32 ) || defined( __CYGWIN__ )
  #define SCIPACK_HIDDEN
#else
  #if __GNUC__ >= 4
     #define SCIPACK_HIDDEN __attribute__ ((visibility ("hidden")))
  #else
     #define SCIPACK_HIDDEN
  #endif
#endif
#endif

#ifndef SCIPACK_NAMESPACE_BEGIN
#define SCIPACK_NAMESPACE_BEGIN  namespace SciPack {
#define SCIPACK_NAMESPACE_END    }
#endif

#ifndef SCIPACK_CLASS
#define SCIPACK_CLASS( c )  SciPack::##c
#endif

#ifndef SCIPACK_CLASS_PTR
#define SCIPACK_CLASS_PTR( c, obj )  (( SciPack::##c *)( obj ))
#endif

#ifndef SCIPACK_S_OK
#define SCIPACK_S_OK 0
#endif

#ifndef SCIPACK_E_UNEXPECTED
#define SCIPACK_E_UNEXPECTED  0x81
#endif


// /////////////////////////////////////////////////////////////////////
//  definition & include
// ///////////////////////////////////////////////////////////////////// 
#include <stdint.h>

// /////////////////////////////////////////////////////////////////////
//! NwkUsbObj2 ( user defined device ) 
/*!
   this class is a base USB Object transport-bridge, used for communicating with USB Device.
   this device has endpoints: Stream Bulk In * 1, block Bulk In * 1, block BulkOut * 1, interrupt IN * 1, interrupt OUT * 1
*/
// ///////////////////////////////////////////////////////////////////// 
SCIPACK_NAMESPACE_BEGIN

class SCIPACK_HIDDEN NwkUsbObj2 {
public:
    // =================================================================
    // Enum & Structure
    // =================================================================
    // the status ID
    enum StatusID {
        StatusID_S_OK = 0, 
        StatusID_E_Init_Failed, 
        StatusID_E_Cfg_Failed, 
        StatusID_E_CreateEvent_Failed,  // Win32 Specified
        StatusID_E_Trans_Error,
        StatusID_E_Inner_Error
    };  
       
    // control transfer status
    enum TransStatusID {
        TransStatusID_S_OK = 0,
        TransStatusID_S_Working = 1,
        TransStatusID_E_Unknown_Error  = 0xe0,
        TransStatusID_E_Trans_Failed   = 0xe1,
        TransStatusID_E_Gen_Failure    = 0xe2,
        TransStatusID_E_Exist_BadBlock = 0xe3,
        TransStatusID_E_Over_Time      = 0xe4
    };

    // the pipe ID
    enum PipeTypeID {
        PipeTypeID_Unknown   = 0x00,
        PipeTypeID_IntIn     = 0x01, PipeTypeID_IntOut     = 0x02,
        PipeTypeID_StrBulkIn = 0x03, PipeTypeID_StrBulkOut = 0x04,
        PipeTypeID_BlkBulkIn = 0x05, PipeTypeID_BlkBulkOut = 0x06
    };

    // the Setup
    struct  SetupPacket {
        uint8_t  m_req_type, m_req;  // req_type is 0x40 means OUT to device( vendor ), 0xc0 means read from device( vendor )
        uint16_t m_value,  m_index, m_length;
    };    
     
    // the data packet
    struct  DataPacket {
        void      *dat_ptr;
        intptr_t   dat_size; // when finished, dat_size is the remain size
        intptr_t   dat_id;
        TransStatusID   trans_s_id;
    };

    // =================================================================
    // CTOR
    // =================================================================
    NwkUsbObj2 ( uint32_t vid_pid, uint8_t cfg_id, uint32_t if_set, uint8_t if_id,  intptr_t  idx = 0 );
    virtual ~NwkUsbObj2 ( );
    
    // =================================================================
    // Funtions
    // =================================================================   
    intptr_t               ctlTransSync ( SetupPacket *setup, DataPacket *dat, intptr_t *bytes_trans );
    intptr_t               bulkTransSync( PipeTypeID  p_id, DataPacket *dat );
    NwkUsbObj2::StatusID   status( );


protected:    
    // =================================================================
    // Static Member
    // =================================================================
public:
    static int   currNewCntr( );
    static int   currMallocCntr( );

private:
    void  *m_rsvd;
    void  *m_obj;

friend class NwkUsbObj2Private;
private: NwkUsbObj2 ( const NwkUsbObj2 & ); NwkUsbObj2 & operator=( const NwkUsbObj2 & ); // disable copy and assign


};

SCIPACK_NAMESPACE_END


/*!
  @}
*/

/*!
  @}
*/

/*!
  @}
*/

#endif
