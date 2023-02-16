/* ///////////////////////////////////////////////////////////////////// */
/*! 
  @file    nwkusbobj2_win32.cxx
  @author  night wing
  @date    2016/12
  @brief   The file implement the NwkUsbObj2 class ( windows backend )
  @par     History
  @verbatim
          <author>   <time>   <version>  <desc>                                   
         nightwing   2016/9   0.1.1     build this module
         nightwing   2016/9   0.1.2     fixed.
         nightwing   2016/9   0.1.3     fixed. issues in loadDev( )
         nightwing   2016/10  0.1.4     fixed macro
         nightwing   2016/12  0.2.0     fixed.
         nightwing   2016/12  0.2.1     fixed.
  @endverbatim
*/
/* ///////////////////////////////////////////////////////////////////// */
#ifndef  SCIPACK_NWKUSBOBJ2_WIN32_CXX
#define  SCIPACK_NWKUSBOBJ2_WIN32_CXX

#if !defined( SCIPACK_CFG_PLATFORM_DEFINED )
#define SCIPACK_CFG_PLATFORM_WINDOWS
#define SCIPACK_CFG_PLATFORM_DEFINED
#endif
#if defined( SCIPACK_CFG_PLATFORM_WINDOWS )


// /////////////////////////////////////////////////////////////////////
//  config section
// ///////////////////////////////////////////////////////////////////// 
#include <atomic>
#include <stdio.h>
#include <wchar.h>

#if defined( SCIPACK_CFG_USE_MEM_CNTR )

static std::atomic<int> g_malloc_cntr(0);
static std::atomic<int> g_new_cntr(0);
static void  incr_malloc_cntr( ) { g_malloc_cntr.fetch_add(1); }
static void  decr_malloc_cntr( ) { g_malloc_cntr.fetch_sub(1); }
static void  incr_new_cntr( )    { g_new_cntr.fetch_add(1); }
static void  decr_new_cntr( )    { g_new_cntr.fetch_sub(1); }
static int   curr_malloc_cntr( ) { return g_malloc_cntr; }
static int   curr_new_cntr( )    { return g_new_cntr; }

#else
static void  incr_malloc_cntr( ) { }
static void  decr_malloc_cntr( ) { }
static void  incr_new_cntr( ) { }
static void  decr_new_cntr( ) { }
static int   curr_malloc_cntr( ) { return 0; }
static int   curr_new_cntr( ) { return 0; }
#endif

#ifndef scipack_new
#define scipack_new( t, ... )  ( incr_new_cntr( ), new t ( __VA_ARGS__ ))
#define scipack_delete( p,t )  do{ decr_new_cntr( ); delete( ( t *)( p )); }while(0)
#endif
#ifndef scipack_malloc
#define scipack_malloc( sz )   ( incr_malloc_cntr(), malloc( sz ))
#define scipack_free( p )      do{ decr_malloc_cntr( ); free( p ); }while(0)
#endif
#ifndef scipack_new_cntr
#define scipack_new_cntr    curr_new_cntr( )
#endif
#ifndef scipack_malloc_cntr
#define scipack_malloc_cntr curr_malloc_cntr( )
#endif

#if !defined( SCIPACK_STR_T )
#define  SCIPACK_STR_T(x) L ## x 
#endif
#if defined( SCIPACK_CFG_DIAG_USE_DBG_OUTPUT )
    #define scipack_dbg_print( s, ... )   do{ fprintf( stdout, "Ln[%d]:" s "\n", __LINE__, ##__VA_ARGS__ ); fflush( stdout ); }while(0) 
    #define scipack_dbg_wprint( s, ... )  do{ fwprintf( stdout, L"Ln[%d]:" s L"\n", __LINE__, ##__VA_ARGS__ ); fflush( stdout ); } while(0)
    #define scipack_dbg_err( s, ... )     do{ fprintf( stdout, "[%s]::[%s] Ln:%d\n" s "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__ ); fflush( stdout ); }while(0)
    #define scipack_dbg_werr( s, ... )    do{ fwprintf( stdout, L"[%s]::[%s] Ln:%d\n" s L"\n", SCIPACK_STR_T(__FILE__), SCIPACK_STR_T(__FUNCTION__), __LINE__, ##__VA_ARGS__ ); fflush( stdout ); }while(0)
#else
    #define scipack_dbg_print( s, ... )       
    #define scipack_dbg_wprint( s, ... )
    #define scipack_dbg_err( s, ... )     
    #define scipack_dbg_werr( s, ... )       
#endif

#if !defined( scipack_assert )
#include <assert.h>
#define scipack_assert assert
#endif

#if !defined( SCIPACK_S_OK )
#define SCIPACK_S_OK 0
#endif
#if !defined( SCIPACK_NULL_PTR )
#define SCIPACK_NULL_PTR 0
#endif
#if !defined( SCIPACK_E_UNEXPECTED )
#define SCIPACK_E_UNEXPECTED  0x81
#endif

// /////////////////////////////////////////////////////////////////////
//  definition & include
// ///////////////////////////////////////////////////////////////////// 

#include <limits.h>
#include <windows.h>
#include <winusb.h>
#include <setupapi.h>
#include <thread>  // for C++11
#include <mutex>   // for C++11
#include <memory.h>

#include "nwkusbobj2.hxx"

SCIPACK_NAMESPACE_BEGIN

// //////////////////////////////////////////////////////////////////////
//  local
// //////////////////////////////////////////////////////////////////////
#define  NWK_OVLP_STR_BULK_IN_NUM    8
#define  NWK_OVLP_STR_BULK_OUT_NUM   8
#define  NWK_OVLP_BLK_BULK_IN_NUM    8
#define  NWK_OVLP_BLK_BULK_OUT_NUM   8
#define  NWK_OVLP_INT_OUT_NUM        1
#define  NWK_OVLP_INT_IN_NUM         1

#define  NWK_OVLP_STR_BULK_IN_WKUP_NUM    1
#define  NWK_OVLP_STR_BULK_OUT_WKUP_NUM   1
#define  NWK_OVLP_BLK_BULK_IN_WKUP_NUM    1
#define  NWK_OVLP_BLK_BULK_OUT_WKUP_NUM   1
#define  NWK_OVLP_INT_OUT_WKUP_NUM        1
#define  NWK_OVLP_INT_IN_WKUP_NUM         1

class SCIPACK_HIDDEN NwkUsbObj2Private {
public:
    // ==============================================================
    // enum
    // ==============================================================
    enum  OvlpStateID {
        OvlpStateID_Not_Init, OvlpStateID_Ready, OvlpStateID_IoPending, OvlpStateID_CheckResult, OvlpStateID_WaitForPost, OvlpStateID_ResetEventFailed, OvlpStateID_PostFailed
    };
    enum  BulkStateID {
        BulkStateID_Ready, BulkStateID_IoPending, BulkStateID_WaitForPost, BulkStateID_PostFailed
    };

    // ==============================================================
    // Input Output Desc.
    // ==============================================================
    class  IoDesc {
    private:
        OVERLAPPED  *m_ovlp_ptr; HANDLE  *m_wt_ptr;  OvlpStateID *m_ovlp_st_ptr;  intptr_t *m_data_sz_ptr;
        BulkStateID  m_state;  uint8_t  m_pipe_id;
        intptr_t     m_max_trans_size, m_unit_num;
        NwkUsbObj2::PipeTypeID  m_pipe_type;
        bool         m_no_error, m_fatal;

    public:
        IoDesc(  )  {
            this->setPtrs( NULL, NULL, NULL, NULL );
            m_state = BulkStateID_Ready;  m_pipe_id = 0;
            m_max_trans_size = 64;        m_unit_num = 1;
            m_no_error = true;            m_fatal = false;
            m_pipe_type = NwkUsbObj2::PipeTypeID_Unknown;
        }
        ~IoDesc(  ) {  }

        void   setPtrs( OVERLAPPED *ovlp_ptr, HANDLE *wt_ptr, OvlpStateID *st_ptr, intptr_t *dsz_ptr ) {
            m_ovlp_ptr = ovlp_ptr; m_wt_ptr = wt_ptr; m_ovlp_st_ptr = st_ptr; m_data_sz_ptr = dsz_ptr;
        }

        inline intptr_t *   dataSizePtr( )    { return m_data_sz_ptr; }
        inline OvlpStateID* ovlpStateIDPtr( ) { return m_ovlp_st_ptr; }
        inline HANDLE*      waitEventPtr( )   { return m_wt_ptr; }
        inline OVERLAPPED*  ovlpPtr( )        { return m_ovlp_ptr; }
        inline intptr_t &   maxTransSize( )   { return m_max_trans_size; }
        inline intptr_t &   unitNum( ) { return  m_unit_num; }
        inline uint8_t &    pipeId( )  { return  m_pipe_id; }
        inline NwkUsbObj2::PipeTypeID&  pipeType( ) { return m_pipe_type; }
        inline bool &       noError( ) { return m_no_error; }
        inline bool &       fatal( )   { return m_fatal; }
        inline BulkStateID&  stateID( ) { return m_state; }
    };

private:
    // device information
    intptr_t  m_idx, m_cfg_id;
    uint32_t  m_if_set, m_if_id;    // if_set: 0xaabbccdd  aa -- if.class  bb -- sub class  cc -- protocol  dd -- alt set
    uint16_t  m_vid, m_pid, m_vendor_cmd;

    // Io Desc.
    IoDesc       m_int_in_io_desc;
    IoDesc       m_int_out_io_desc;
    IoDesc       m_str_bulk_in_io_desc;
    IoDesc       m_str_bulk_out_io_desc;
    IoDesc       m_blk_bulk_in_io_desc;
    IoDesc       m_blk_bulk_out_io_desc;

    // the device handle & status
    HANDLE                     m_h_dev;
    WINUSB_INTERFACE_HANDLE    m_h_usb;
    NwkUsbObj2::StatusID       m_status;
    int                        m_init_mask;
    NwkUsbObj2                *m_parent;

    // ----------------------------------------------
    // data array.
    // ----------------------------------------------
    OVERLAPPED   m_zero_overlapped;
    OVERLAPPED   m_ovlp_str_bulk_in [ NWK_OVLP_STR_BULK_IN_WKUP_NUM  + NWK_OVLP_STR_BULK_IN_NUM  ];
    OVERLAPPED   m_ovlp_str_bulk_out[ NWK_OVLP_STR_BULK_OUT_WKUP_NUM + NWK_OVLP_STR_BULK_OUT_NUM ];
    OVERLAPPED   m_ovlp_blk_bulk_in [ NWK_OVLP_BLK_BULK_IN_WKUP_NUM  + NWK_OVLP_BLK_BULK_IN_NUM  ];
    OVERLAPPED   m_ovlp_blk_bulk_out[ NWK_OVLP_BLK_BULK_OUT_WKUP_NUM + NWK_OVLP_BLK_BULK_OUT_NUM ];
    OVERLAPPED   m_ovlp_int_in      [ NWK_OVLP_INT_IN_WKUP_NUM       + NWK_OVLP_INT_IN_NUM       ];
    OVERLAPPED   m_ovlp_int_out      [ NWK_OVLP_INT_OUT_WKUP_NUM      + NWK_OVLP_INT_OUT_NUM      ];

    HANDLE       m_wait_str_bulk_in [ NWK_OVLP_STR_BULK_IN_WKUP_NUM  + NWK_OVLP_STR_BULK_IN_NUM  ];
    HANDLE       m_wait_str_bulk_out[ NWK_OVLP_STR_BULK_OUT_WKUP_NUM + NWK_OVLP_STR_BULK_OUT_NUM ];
    HANDLE       m_wait_blk_bulk_in [ NWK_OVLP_BLK_BULK_IN_WKUP_NUM  + NWK_OVLP_BLK_BULK_IN_NUM  ];
    HANDLE       m_wait_blk_bulk_out[ NWK_OVLP_BLK_BULK_OUT_WKUP_NUM + NWK_OVLP_BLK_BULK_OUT_NUM ];
    HANDLE       m_wait_int_in      [ NWK_OVLP_INT_IN_WKUP_NUM       + NWK_OVLP_INT_IN_NUM       ];
    HANDLE       m_wait_int_out     [ NWK_OVLP_INT_OUT_WKUP_NUM      + NWK_OVLP_INT_OUT_NUM      ];

    OvlpStateID  m_state_str_bulk_in [ NWK_OVLP_STR_BULK_IN_WKUP_NUM  + NWK_OVLP_STR_BULK_IN_NUM  ];
    OvlpStateID  m_state_str_bulk_out[ NWK_OVLP_STR_BULK_OUT_WKUP_NUM + NWK_OVLP_STR_BULK_OUT_NUM ];
    OvlpStateID  m_state_blk_bulk_in [ NWK_OVLP_BLK_BULK_IN_WKUP_NUM  + NWK_OVLP_BLK_BULK_IN_NUM  ];
    OvlpStateID  m_state_blk_bulk_out[ NWK_OVLP_BLK_BULK_OUT_WKUP_NUM + NWK_OVLP_BLK_BULK_OUT_NUM ];
    OvlpStateID  m_state_int_in      [ NWK_OVLP_INT_IN_WKUP_NUM       + NWK_OVLP_INT_IN_NUM       ];
    OvlpStateID  m_state_int_out     [ NWK_OVLP_INT_OUT_WKUP_NUM      + NWK_OVLP_INT_OUT_NUM      ];

    intptr_t  m_datasz_str_bulk_in [ NWK_OVLP_STR_BULK_IN_WKUP_NUM  + NWK_OVLP_STR_BULK_IN_NUM  ];
    intptr_t  m_datasz_str_bulk_out[ NWK_OVLP_STR_BULK_OUT_WKUP_NUM + NWK_OVLP_STR_BULK_OUT_NUM ];
    intptr_t  m_datasz_blk_bulk_in [ NWK_OVLP_BLK_BULK_IN_WKUP_NUM  + NWK_OVLP_BLK_BULK_IN_NUM  ];
    intptr_t  m_datasz_blk_bulk_out[ NWK_OVLP_BLK_BULK_OUT_WKUP_NUM + NWK_OVLP_BLK_BULK_OUT_NUM ];
    intptr_t  m_datasz_int_in      [ NWK_OVLP_INT_IN_WKUP_NUM       + NWK_OVLP_INT_IN_NUM       ];
    intptr_t  m_datasz_int_out     [ NWK_OVLP_INT_OUT_WKUP_NUM      + NWK_OVLP_INT_OUT_NUM      ];


public:
    // ==============================================================
    // CTOR
    // ==============================================================
    NwkUsbObj2Private( NwkUsbObj2 *pa, uint32_t vid_pid,  uint8_t cfg_id, uint32_t if_set, uint8_t if_id, intptr_t idx ) {

        m_parent = pa;
        m_vid = ( uint16_t )( vid_pid >> 16 ); m_pid = ( uint16_t )( vid_pid );
        m_idx = idx;  m_cfg_id = cfg_id;  m_if_set = if_set;  m_if_id = if_id;

        m_h_dev = INVALID_HANDLE_VALUE;  m_h_usb = INVALID_HANDLE_VALUE;
        m_init_mask = 0;
        m_status = NwkUsbObj2::StatusID_S_OK;

        memset( & m_zero_overlapped, 0, sizeof( m_zero_overlapped ));

        this->setupIoDescPtrs( );
        m_init_mask = 0;
        if ( this->loadDev( )  ) { m_init_mask |= 0x01; } else { goto L_Exit; }
        if ( this->configDev( )) { m_init_mask |= 0x02; } else { goto L_Exit; }
        if ( this->initOvlp( ) ) { m_init_mask |= 0x04; } else { goto L_Exit; }
    L_Exit:;
    }

    // =============================================================
    // DTOR
    // =============================================================
    ~NwkUsbObj2Private(  ) {
        if ( m_init_mask & 0x04 ) { this->deinitOvlp( );    }
        if ( m_init_mask & 0x01 ) { this->unloadDev( );     }
    }

private:
    // =============================================================
    // Functions
    // =============================================================

public:
    // =============================================================
    // Functions
    // =============================================================    
    bool  initOvlp(  );
    void  deinitOvlp( );
    void  setupIoDescPtrs(  );
    NwkUsbObj2::StatusID   status( ) { return m_status; }

    bool  isNwkDev( );
    bool  loadDev( );
    void  unloadDev(  );
    bool  configDev(  );
    void  cfgReadTrSize(  );
    void  cfgPipeTimeOut(  );
    void  resetPipe(  );

    // =============================================================
    // transfer functions
    // =============================================================
    intptr_t   ctlTransSync( NwkUsbObj2::SetupPacket *sp, NwkUsbObj2::DataPacket *dp, intptr_t *bytes_trans );
    bool       doIoRoutine ( IoDesc  *io_desc, NwkUsbObj2::DataPacket *dp );
    intptr_t   bulkTransSync( NwkUsbObj2::PipeTypeID  p_id, NwkUsbObj2::DataPacket *dp );
};

// =========================================================================
// helper function ( upper case the wchar )
// =========================================================================
static void  upper_case_wchar( wchar_t *str )
{
    while ( *str != 0 ) {
        if ( *str >= 'a' && *str <= 'z' ) { *str = *str - 'a' + 'A'; }
        ++ str;
    }
    return;
}

// =========================================================================
// helper function ( check the nwk object vid, pid )
// =========================================================================
static bool is_nwk_vid_pid( wchar_t *str, uint16_t vid, uint16_t pid )
{
    const static wchar_t *dst_vid_pid = L"\\\\?\\USB#VID_%x&PID_%x#";
    intptr_t  v, p; bool  ret = false;
    v = 0; p = 0;
    if ( swscanf( ( const wchar_t *)( str ), dst_vid_pid, &v, &p ) == 0 ) { v = 0; p = 0; }
    if ( v == vid  && p == pid ) { ret = true; }
    scipack_dbg_print(" is_nwk_vid_pid: check vid[%x], pid[%x].", v, p );
    return ret;
}

// =========================================================================
// helper function ( check the nwk device string )
// =========================================================================
bool          NwkUsbObj2Private :: isNwkDev(  )
{
    static const struct {
             uint8_t  m_size, m_type; int8_t m_magic[14], m_vendor_cmd, rsvd;
    } nwk_usb_dscr = {
         0x12, 0x03, { 'N',0,'T',0,'F',0,'T',0,'1',0,'0',0,'0',0 },0, 0
    };
    uint8_t dscr[0x12];  ULONG  recv_bytes;
    WINUSB_SETUP_PACKET  setup;
    BOOL ret = false;

    setup.RequestType = 0x80; setup.Request = 0x06; setup.Value  = 0x03ef; setup.Index = 0; setup.Length = 0x12;
    ret = WinUsb_ControlTransfer( m_h_usb, setup, & dscr[0], 0x12, & recv_bytes, 0 );
    if ( ret ) {
        if ( recv_bytes == 0x12 ) {
            if ( memcmp( ( const void *)( & dscr[0] ), ( const void *)( & nwk_usb_dscr ), 0x12 - 2 ) == 0 ) {
                scipack_dbg_print(" NwkUsbObj2 device string matched. Vendor cmd is 0x%x", dscr[0x10] );
                m_vendor_cmd = dscr[ 0x10 ];  ret = true;
            } else { scipack_dbg_err(" device string matched failed."); }
        } else { scipack_dbg_err(" get vendor string ok, but recvied %d bytes.", recv_bytes ); }
    } else {
        scipack_dbg_err(" try to get vendor string failed,err=0x%x.", GetLastError());
    }
    return ( ret ? true : false );
}

// =================================================================
// inti. the ovlp structure
// =================================================================
static void   clear_datagroup( NwkUsbObj2Private::OvlpStateID *st, HANDLE *h_wait, intptr_t *dsz, OVERLAPPED *ov, int num )
{
    for ( int x = 0; x < num; x ++ ) {
        st[x]  = NwkUsbObj2Private:: OvlpStateID_Not_Init;  
        dsz[x] = 0;
        h_wait[x]    = NULL;  
        ov[x].hEvent = NULL;
    }
}

static bool   create_events( NwkUsbObj2Private::OvlpStateID *st, HANDLE *h_wait, int num )
{
    bool ret = true;

    for ( int x = 0; x < num; x ++ ) {
        h_wait[x] = ::CreateEventW( NULL, TRUE, FALSE, NULL );
        if ( h_wait[x] != NULL ) { 
            st[x] = NwkUsbObj2Private::OvlpStateID_Ready; 
        } else {
            ret = false; scipack_dbg_err(" create event failed at %d", x );
            break;
        }
    }
    return ret;
}

bool      NwkUsbObj2Private :: initOvlp (   )
{
    bool ret = true;

    // --------------------------------------------------------------
    // clear all fields
    // --------------------------------------------------------------
    memset( m_ovlp_str_bulk_in,  0, sizeof( m_ovlp_str_bulk_in ));
    memset( m_ovlp_str_bulk_out, 0, sizeof( m_ovlp_str_bulk_out));
    memset( m_ovlp_blk_bulk_in,  0, sizeof( m_ovlp_blk_bulk_in ));
    memset( m_ovlp_blk_bulk_out, 0, sizeof( m_ovlp_blk_bulk_out));
    memset( m_ovlp_int_in,       0, sizeof( m_ovlp_int_in ));
    memset( m_ovlp_int_out,      0, sizeof( m_ovlp_int_out));

    clear_datagroup( m_state_str_bulk_in,  m_wait_str_bulk_in,  m_datasz_str_bulk_in,  m_ovlp_str_bulk_in,  NWK_OVLP_STR_BULK_IN_WKUP_NUM  + NWK_OVLP_STR_BULK_IN_NUM  );
    clear_datagroup( m_state_str_bulk_out, m_wait_str_bulk_out, m_datasz_str_bulk_out, m_ovlp_str_bulk_out, NWK_OVLP_STR_BULK_OUT_WKUP_NUM + NWK_OVLP_STR_BULK_OUT_NUM );
    clear_datagroup( m_state_blk_bulk_in,  m_wait_blk_bulk_in,  m_datasz_blk_bulk_in,  m_ovlp_blk_bulk_in,  NWK_OVLP_BLK_BULK_IN_WKUP_NUM  + NWK_OVLP_BLK_BULK_IN_NUM  );
    clear_datagroup( m_state_blk_bulk_out, m_wait_blk_bulk_out, m_datasz_blk_bulk_out, m_ovlp_blk_bulk_out, NWK_OVLP_BLK_BULK_OUT_WKUP_NUM + NWK_OVLP_BLK_BULK_OUT_NUM );
    clear_datagroup( m_state_int_in,       m_wait_int_in,       m_datasz_int_in,       m_ovlp_int_in,       NWK_OVLP_INT_IN_WKUP_NUM       + NWK_OVLP_INT_IN_NUM  );
    clear_datagroup( m_state_int_out,      m_wait_int_out,      m_datasz_int_out,      m_ovlp_int_out,      NWK_OVLP_INT_OUT_WKUP_NUM      + NWK_OVLP_INT_OUT_NUM );

    // -------------------------------------------------------------
    // create event
    // -------------------------------------------------------------
    if ( ret ) { ret = create_events( m_state_str_bulk_in,  m_wait_str_bulk_in,  NWK_OVLP_STR_BULK_IN_WKUP_NUM  + NWK_OVLP_STR_BULK_IN_NUM  ); }
    if ( ret ) { ret = create_events( m_state_str_bulk_out, m_wait_str_bulk_out, NWK_OVLP_STR_BULK_OUT_WKUP_NUM + NWK_OVLP_STR_BULK_OUT_NUM ); }
    if ( ret ) { ret = create_events( m_state_blk_bulk_in,  m_wait_blk_bulk_in,  NWK_OVLP_BLK_BULK_IN_WKUP_NUM  + NWK_OVLP_BLK_BULK_IN_NUM );  }
    if ( ret ) { ret = create_events( m_state_blk_bulk_out, m_wait_blk_bulk_out, NWK_OVLP_BLK_BULK_OUT_WKUP_NUM + NWK_OVLP_BLK_BULK_OUT_NUM ); }
    if ( ret ) { ret = create_events( m_state_int_in,       m_wait_int_in,       NWK_OVLP_INT_IN_WKUP_NUM       + NWK_OVLP_INT_IN_NUM );  }
    if ( ret ) { ret = create_events( m_state_int_out,      m_wait_int_out,      NWK_OVLP_INT_OUT_WKUP_NUM      + NWK_OVLP_INT_OUT_NUM ); }

    if ( !ret ) {
        this->deinitOvlp();
        m_status = NwkUsbObj2::StatusID_E_CreateEvent_Failed;
    }

    return ret;
}

// ================================================================
// deinit. the ovlp structure
// ================================================================
static void  deinit_ovlp( NwkUsbObj2Private::OvlpStateID *st, HANDLE *h_wait, intptr_t *dsz, OVERLAPPED *ov, int num )
{
    for ( int x = 0; x < num; x ++ ) {
        if ( h_wait[x] != NULL ) { ::CloseHandle( h_wait[x] ); h_wait[x] = NULL; }
        ov[x].hEvent = NULL;
        st[x]  = NwkUsbObj2Private::OvlpStateID_Not_Init;
        dsz[x] = 0;
    }  
}

void     NwkUsbObj2Private :: deinitOvlp(  )
{
    deinit_ovlp( m_state_str_bulk_in,  m_wait_str_bulk_in,  m_datasz_str_bulk_in,  m_ovlp_str_bulk_in,  NWK_OVLP_STR_BULK_IN_WKUP_NUM  + NWK_OVLP_STR_BULK_IN_NUM  );
    deinit_ovlp( m_state_str_bulk_out, m_wait_str_bulk_out, m_datasz_str_bulk_out, m_ovlp_str_bulk_out, NWK_OVLP_STR_BULK_OUT_WKUP_NUM + NWK_OVLP_STR_BULK_OUT_NUM );
    deinit_ovlp( m_state_blk_bulk_in,  m_wait_blk_bulk_in,  m_datasz_blk_bulk_in,  m_ovlp_blk_bulk_in,  NWK_OVLP_BLK_BULK_IN_WKUP_NUM  + NWK_OVLP_BLK_BULK_IN_NUM );
    deinit_ovlp( m_state_blk_bulk_out, m_wait_blk_bulk_out, m_datasz_blk_bulk_out, m_ovlp_blk_bulk_out, NWK_OVLP_BLK_BULK_OUT_WKUP_NUM + NWK_OVLP_BLK_BULK_OUT_NUM );
    deinit_ovlp( m_state_int_in,       m_wait_int_in,       m_datasz_int_in,       m_ovlp_int_in,       NWK_OVLP_INT_IN_WKUP_NUM       + NWK_OVLP_INT_IN_NUM );
    deinit_ovlp( m_state_int_out,      m_wait_int_out,      m_datasz_int_out,      m_ovlp_int_out,      NWK_OVLP_INT_OUT_WKUP_NUM      + NWK_OVLP_INT_OUT_NUM );
    return;
}

// ================================================================
// init the IoDesc
// ================================================================
void    NwkUsbObj2Private :: setupIoDescPtrs()
{
    m_int_in_io_desc.setPtrs( & m_ovlp_int_in[0], & m_wait_int_in[0], & m_state_int_in[0], & m_datasz_int_in[0] );
    m_int_in_io_desc.unitNum()  = NWK_OVLP_INT_IN_NUM;
    m_int_in_io_desc.pipeType() = NwkUsbObj2::PipeTypeID_IntIn;

    m_int_out_io_desc.setPtrs( & m_ovlp_int_out[0], & m_wait_int_out[0], & m_state_int_out[0], & m_datasz_int_out[0] );
    m_int_out_io_desc.unitNum()  = NWK_OVLP_INT_OUT_NUM;
    m_int_out_io_desc.pipeType() = NwkUsbObj2::PipeTypeID_IntOut;

    m_str_bulk_in_io_desc.setPtrs( & m_ovlp_str_bulk_in[0], & m_wait_str_bulk_in[0], & m_state_str_bulk_in[0], & m_datasz_str_bulk_in[0] );
    m_str_bulk_in_io_desc.unitNum()  = NWK_OVLP_STR_BULK_IN_NUM;
    m_str_bulk_in_io_desc.pipeType() = NwkUsbObj2::PipeTypeID_StrBulkIn;

    m_str_bulk_out_io_desc.setPtrs( & m_ovlp_str_bulk_out[0], & m_wait_str_bulk_out[0], & m_state_str_bulk_out[0], & m_datasz_str_bulk_out[0] );
    m_str_bulk_out_io_desc.unitNum()  = NWK_OVLP_STR_BULK_OUT_NUM;
    m_str_bulk_out_io_desc.pipeType() = NwkUsbObj2::PipeTypeID_StrBulkOut;

    m_blk_bulk_in_io_desc.setPtrs( & m_ovlp_blk_bulk_in[0], & m_wait_blk_bulk_in[0], & m_state_blk_bulk_in[0], & m_datasz_blk_bulk_in[0] );
    m_blk_bulk_in_io_desc.unitNum()  = NWK_OVLP_BLK_BULK_IN_NUM;
    m_blk_bulk_in_io_desc.pipeType() = NwkUsbObj2::PipeTypeID_BlkBulkIn;

    m_blk_bulk_out_io_desc.setPtrs( & m_ovlp_blk_bulk_out[0], & m_wait_blk_bulk_out[0], & m_state_blk_bulk_out[0], & m_datasz_blk_bulk_out[0] );
    m_blk_bulk_out_io_desc.unitNum()  = NWK_OVLP_BLK_BULK_OUT_NUM;
    m_blk_bulk_out_io_desc.pipeType() = NwkUsbObj2::PipeTypeID_BlkBulkOut;
}

// =================================================================
// load the device
// =================================================================
bool    NwkUsbObj2Private :: loadDev(  )
{
    HDEVINFO                            hwDevInfo;
    PSP_INTERFACE_DEVICE_DETAIL_DATA_W  funcClassDevData;
    ULONG       reqLen, preLen;         LONG        x;
    HANDLE      dev_hde;
    intptr_t    idx = m_idx;    bool ret = false;
    SP_DEVICE_INTERFACE_DATA    devIFData;
    WORD    buf[ 258 ];

    // --------------------------------------------------------------------
    // WinUsb Interface GUID ( USB_DEVICE )
    // --------------------------------------------------------------------
    static const GUID guid = { 0xA5DCBF10, 0x6530, 0x11D2, {0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED} };

    // -------------------------------------------------------------------
    // begin enum interface class device
    // -------------------------------------------------------------------
    dev_hde = INVALID_HANDLE_VALUE;
    hwDevInfo = SetupDiGetClassDevsW ( ( LPGUID ) & guid, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE );
    if ( hwDevInfo == INVALID_HANDLE_VALUE ) { scipack_dbg_err(" failed to get class devs, err=0x%x", GetLastError( )); goto L_Exit; }

    x = 0;
    devIFData.cbSize = sizeof ( SP_DEVICE_INTERFACE_DATA );
    while ( SetupDiEnumDeviceInterfaces ( hwDevInfo, 0, ( LPGUID ) & guid, x, & devIFData )) {
        //if ( SetupDiGetDeviceInterfaceDetailW ( hwDevInfo, & devIFData, NULL, 0, & reqLen, NULL ) != FALSE ) { reqLen = 256; } else { reqLen = reqLen; }
        //if ( reqLen > 256 ) { reqLen = 256; } if ( reqLen < 0 ) { reqLen = 32; }
        reqLen = 256; //nw: we use this to avoid query the required size.
        preLen = reqLen;
        buf[ reqLen - 1 ] = 0;  /** the end zero */

        funcClassDevData         = ( PSP_INTERFACE_DEVICE_DETAIL_DATA_W ) &buf[0];
        funcClassDevData->cbSize = sizeof ( SP_INTERFACE_DEVICE_DETAIL_DATA_W );
        devIFData.cbSize         = sizeof ( SP_DEVICE_INTERFACE_DATA );

        if ( SetupDiGetDeviceInterfaceDetailW ( hwDevInfo, & devIFData, funcClassDevData, preLen, & reqLen, NULL )) {
            upper_case_wchar( funcClassDevData->DevicePath );
            scipack_dbg_wprint( L" usb device path: \n %s", funcClassDevData->DevicePath );
            if ( is_nwk_vid_pid ( funcClassDevData->DevicePath, m_vid, m_pid ) ) {
                if ( idx -- <= 0 ) { // found
                    scipack_dbg_wprint( L" found the device, path is %s.", funcClassDevData->DevicePath );

                    // try to open device
                    m_h_dev = CreateFileW( funcClassDevData->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_WRITE_THROUGH | FILE_FLAG_OVERLAPPED, NULL);
                    if ( m_h_dev == INVALID_HANDLE_VALUE ) { scipack_dbg_err(" error in create file,err=0x%x\n", GetLastError()); break; }

                    // try to init. winusb handle
                    if ( ! WinUsb_Initialize ( m_h_dev, & m_h_usb ) ) {
                        scipack_dbg_err(" error in init. winusb. err=0x%x\n", GetLastError( ));
                        CloseHandle( m_h_dev ); m_h_dev = INVALID_HANDLE_VALUE;  m_h_usb = INVALID_HANDLE_VALUE;
                        break;  // error in init. device
                    }

                    // try to verify the nwk descriptor
                    if ( ! this->isNwkDev(  ) ) {
                        WinUsb_Free( m_h_usb ); CloseHandle( m_h_dev ); m_h_dev = INVALID_HANDLE_VALUE; m_h_usb = INVALID_HANDLE_VALUE;
                        break;  // the specified device is not nwk device
                    }

                    // all test passed.
                    ret = true; break;
                }
            }
        }

        // next data collection
        devIFData.cbSize = sizeof ( SP_DEVICE_INTERFACE_DATA );
        ++ x;
    }

    SetupDiDestroyDeviceInfoList ( hwDevInfo );
L_Exit:
    if ( ! ret ) {
        m_status = NwkUsbObj2::StatusID_E_Init_Failed;
    } else {
        scipack_dbg_print("loaded device, m_h_usb = 0x%x", m_h_usb );
    }

    return ret;
}

// =========================================================================
// unload the USB device
// =========================================================================
void       NwkUsbObj2Private :: unloadDev( )
{
    if ( m_h_usb != INVALID_HANDLE_VALUE ) { WinUsb_Free( m_h_usb ); }
    if ( m_h_dev != INVALID_HANDLE_VALUE ) { CloseHandle( m_h_dev ); }
    m_h_usb = INVALID_HANDLE_VALUE;
    m_h_dev = INVALID_HANDLE_VALUE;
}

// =========================================================================
// get the device information
// =========================================================================
bool       NwkUsbObj2Private :: configDev (  )
{
    WINUSB_SETUP_PACKET  setup;  ULONG  recv_bytes;
    BOOL  ret = TRUE;

    if ( m_h_usb == INVALID_HANDLE_VALUE ) { ret = false; goto L_Exit; }

    // -----------------------------------------------
    // 1) setting the active configuration number
    // -----------------------------------------------
    setup.RequestType = 0x00; setup.Request = 0x09; setup.Value = m_cfg_id; setup.Index = 0; setup.Length = 0;
    ret = WinUsb_ControlTransfer( m_h_usb, setup, NULL, 0, & recv_bytes, 0 );
    if ( !ret ) { scipack_dbg_err("set usb configuration id failed,err=0x%x", GetLastError()); goto L_Exit; }

    // ------------------------------------------------
    // 2) setting the active interface
    // ------------------------------------------------
    setup.RequestType = 0x01; setup.Request = 0x0b; setup.Value = ( uint8_t )( m_if_set & 256 ); setup.Index = m_if_id; setup.Length = 0;
    ret = WinUsb_ControlTransfer( m_h_usb, setup, NULL, 0, & recv_bytes, 0 );
    if ( !ret ) { scipack_dbg_err("set interface failed, err=0x%x", GetLastError()); goto L_Exit; }
    scipack_dbg_print("set interface index = %d, alt = %d", m_if_id, m_if_set & 256 );

    // --------------------------------------------------
    // 3) read the EP map descr.
    // --------------------------------------------------
    {
        struct { uint8_t m_sz, m_type; uint8_t m_ep_int_in, m_ep_int_out, m_ep_str_bulk_in, m_ep_str_bulk_out, m_ep_blk_bulk_in, m_ep_blk_bulk_out; } ep_map_desc;
        setup.RequestType = 0xc0; setup.Request = 0xb1; setup.Value = 0; setup.Index = 0xa0; setup.Length = sizeof( ep_map_desc );
        ret = WinUsb_ControlTransfer( m_h_usb, setup, ( PUCHAR ) &ep_map_desc, sizeof( ep_map_desc ), & recv_bytes, 0 );
        if ( ret ) {
            if ( ep_map_desc.m_type == 0xac ) {
                m_int_in_io_desc.pipeId()  = ep_map_desc.m_ep_int_in;
                m_int_out_io_desc.pipeId() = ep_map_desc.m_ep_int_out;
                m_str_bulk_in_io_desc.pipeId()  = ep_map_desc.m_ep_str_bulk_in;
                m_str_bulk_out_io_desc.pipeId() = ep_map_desc.m_ep_str_bulk_out;
                m_blk_bulk_in_io_desc.pipeId() = ep_map_desc.m_ep_blk_bulk_in;
                m_blk_bulk_out_io_desc.pipeId()= ep_map_desc.m_ep_blk_bulk_out;
                scipack_dbg_print(" found str pipe, IN:0x%x, OUT:0x%x", ep_map_desc.m_ep_str_bulk_in, ep_map_desc.m_ep_str_bulk_out);
                scipack_dbg_print(" found blk pipe, IN:0x%x, OUT:0x%x", ep_map_desc.m_ep_blk_bulk_in, ep_map_desc.m_ep_blk_bulk_out);
                scipack_dbg_print(" found int pipe, IN:0x%x, OUT:0x%x", ep_map_desc.m_ep_int_in,      ep_map_desc.m_ep_int_out     );

            } else {
                scipack_dbg_err("read EP map descr. ok, but type is 0x%x", ep_map_desc.m_type );
                ret = FALSE;
            }
        } else {
            scipack_dbg_err("read the EP map descr. failed, err=0x%x", GetLastError());
        }
    }
    if ( !ret ) { goto L_Exit; }

    this->cfgPipeTimeOut( );
    this->cfgReadTrSize( );
    this->resetPipe( );

    // clear resources.
L_Exit:
    if ( ret != TRUE ) { m_status = NwkUsbObj2::StatusID_E_Cfg_Failed; }

    return ( ret ? true : false );
}

// ======================================================================
// cfg: config the endpoint time out
// ======================================================================
void            NwkUsbObj2Private :: cfgPipeTimeOut( )
{
    ULONG  val = 500; BOOL ret = TRUE; BOOL v_ret;

    // time out
    if ( m_int_in_io_desc.pipeId()  != 0 ) { ret = WinUsb_SetPipePolicy( m_h_usb, m_int_in_io_desc.pipeId(), PIPE_TRANSFER_TIMEOUT, sizeof( val ), & val ); }
    if ( ! ret ) { scipack_dbg_err("set int_in pipe policy timeout failed, last err=0x%x", GetLastError( )); }
    if ( m_int_out_io_desc.pipeId() != 0 ) { ret = WinUsb_SetPipePolicy( m_h_usb, m_int_out_io_desc.pipeId(), PIPE_TRANSFER_TIMEOUT, sizeof( val ), & val ); }
    if ( ! ret ) { scipack_dbg_err("set int_out pipe policy timeout failed, last err=0x%x", GetLastError()); }
    val = 3000;
    if ( m_str_bulk_in_io_desc.pipeId() != 0 ) { ret = WinUsb_SetPipePolicy( m_h_usb, m_str_bulk_in_io_desc.pipeId(), PIPE_TRANSFER_TIMEOUT, sizeof( val ), & val ); }
    if ( ! ret ) { scipack_dbg_err("set str_bulk_in pipe policy timeout failed, last err=0x%x", GetLastError( )); }
    if ( m_str_bulk_out_io_desc.pipeId() != 0 ) { ret = WinUsb_SetPipePolicy( m_h_usb, m_str_bulk_out_io_desc.pipeId(), PIPE_TRANSFER_TIMEOUT, sizeof( val ), & val ); }
    if ( ! ret ) { scipack_dbg_err("set str_out pipe policy timeout failed, last err=0x%x", GetLastError()); }
    if ( m_blk_bulk_in_io_desc.pipeId()  != 0 ) { ret = WinUsb_SetPipePolicy( m_h_usb, m_blk_bulk_in_io_desc.pipeId(), PIPE_TRANSFER_TIMEOUT, sizeof( val ), & val ); }
    if ( ! ret ) { scipack_dbg_err("set blk_bulk_in pipe policy timeout failed, last err=0x%x", GetLastError( )); }
    if ( m_blk_bulk_out_io_desc.pipeId() != 0 ) { ret = WinUsb_SetPipePolicy( m_h_usb, m_blk_bulk_out_io_desc.pipeId(),PIPE_TRANSFER_TIMEOUT, sizeof( val ), & val ); }
    if ( ! ret ) { scipack_dbg_err("set blk_bulk_out pipe policy timeout failed, last err=0x%x", GetLastError()); }

    // raw io
    v_ret = TRUE;
    if ( m_str_bulk_in_io_desc.pipeId() != 0 ) { ret = WinUsb_SetPipePolicy( m_h_usb, m_str_bulk_in_io_desc.pipeId(), RAW_IO, sizeof( v_ret ), & v_ret ); }
    if ( ! ret ) { scipack_dbg_err("set str_bulk_in pipe policy RAW_IO failed, last err=0x%x", GetLastError()); }
    if ( m_blk_bulk_in_io_desc.pipeId() != 0 ) { ret = WinUsb_SetPipePolicy( m_h_usb, m_blk_bulk_in_io_desc.pipeId(), RAW_IO, sizeof( v_ret ), & v_ret ); }
    if ( ! ret ) { scipack_dbg_err("set blk_bulk_in pipe policy RAW_IO failed, last err=0x%x", GetLastError()); }
    if ( m_int_in_io_desc.pipeId() != 0 ) { ret = WinUsb_SetPipePolicy( m_h_usb, m_int_in_io_desc.pipeId(), RAW_IO, sizeof( v_ret ), & v_ret ); }
    if ( ! ret ) { scipack_dbg_err("set int_in pipe policy RAW_IO failed, last err=0x%x", GetLastError()); }

    return;
}

// ====================================================================
// cfg: read the maximum transfer size
// ====================================================================
void      NwkUsbObj2Private :: cfgReadTrSize( )
{
    ULONG  val; ULONG sz; BOOL ret = TRUE;

    sz = sizeof( ULONG ); val = 0;
    if ( m_str_bulk_in_io_desc.pipeId() != 0 ) { ret = WinUsb_GetPipePolicy( m_h_usb, m_str_bulk_in_io_desc.pipeId(), MAXIMUM_TRANSFER_SIZE,  & sz, & val ); }
    if ( ! ret ) { scipack_dbg_err("get str_bulk_in pipe policy MAXIMUM_TRANSFER_SIZE failed, last err=0x%x", GetLastError()); } else { m_str_bulk_in_io_desc.maxTransSize() = ( intptr_t )( val ); }
    scipack_dbg_print("get str_bulk_in MAX_TRANS_SIZE:%d", m_str_bulk_in_io_desc.maxTransSize());

    sz = sizeof( ULONG ); val = 0;
    if ( m_str_bulk_out_io_desc.pipeId() != 0) { ret = WinUsb_GetPipePolicy( m_h_usb, m_str_bulk_out_io_desc.pipeId(), MAXIMUM_TRANSFER_SIZE,  & sz, & val ); }
    if ( ! ret ) { scipack_dbg_err("get str_bulk_out pipe policy MAXIMUM_TRANSFER_SIZE failed, last err=0x%x", GetLastError()); } else { m_str_bulk_out_io_desc.maxTransSize() = ( intptr_t )( val ); }
    scipack_dbg_print("get str_bulk_out MAX_TRANS_SIZE:%d", m_str_bulk_out_io_desc.maxTransSize());

    sz = sizeof( ULONG ); val = 0;
    if ( m_blk_bulk_in_io_desc.pipeId()  != 0 ) { ret = WinUsb_GetPipePolicy( m_h_usb, m_blk_bulk_in_io_desc.pipeId(), MAXIMUM_TRANSFER_SIZE,  & sz, & val ); }
    if ( ! ret ) { scipack_dbg_err("get blk_bulk_in pipe policy MAXIMUM_TRANSFER_SIZE failed, last err=0x%x", GetLastError()); } else { m_blk_bulk_in_io_desc.maxTransSize() = ( intptr_t )( val ); }
    scipack_dbg_print("get blk_bulk_in MAX_TRANS_SIZE:%d", m_blk_bulk_in_io_desc.maxTransSize());

    sz = sizeof( ULONG ); val = 0;
    if ( m_blk_bulk_out_io_desc.pipeId() != 0) { ret = WinUsb_GetPipePolicy( m_h_usb, 0x08, MAXIMUM_TRANSFER_SIZE,  & sz, & val ); }
    if ( ! ret ) { scipack_dbg_err("get blk_bulk_out pipe policy MAXIMUM_TRANSFER_SIZE failed, last err=0x%x", GetLastError()); } else { m_blk_bulk_out_io_desc.maxTransSize() = ( intptr_t )( val ); }
    scipack_dbg_print("get blk_bulk_out MAX_TRANS_SIZE:%d", m_blk_bulk_out_io_desc.maxTransSize());

    sz = sizeof( ULONG ); val = 0;
    if ( m_int_in_io_desc.pipeId() != 0) { ret = WinUsb_GetPipePolicy( m_h_usb, m_int_in_io_desc.pipeId(), MAXIMUM_TRANSFER_SIZE,  & sz, & val ); }
    if ( ! ret ) { scipack_dbg_err("get int_in pipe policy MAXIMUM_TRANSFER_SIZE failed, last err=0x%x", GetLastError()); } else { m_int_in_io_desc.maxTransSize() = ( intptr_t )( val ); }
    scipack_dbg_print("get int_in MAX_TRANS_SIZE:%d", m_int_in_io_desc.maxTransSize());

    sz = sizeof( ULONG ); val = 0;
    if ( m_int_out_io_desc.pipeId() != 0) { ret = WinUsb_GetPipePolicy( m_h_usb, m_int_out_io_desc.pipeId(), MAXIMUM_TRANSFER_SIZE,  & sz, & val ); }
    if ( ! ret ) { scipack_dbg_err("get int_out pipe policy MAXIMUM_TRANSFER_SIZE failed, last err=0x%x", GetLastError()); } else { m_int_out_io_desc.maxTransSize() = ( intptr_t )( val ); }
    scipack_dbg_print("get int_out MAX_TRANS_SIZE:%d", m_int_out_io_desc.maxTransSize());

    return;
}

// ==================================================================
// reset the pipe
// ==================================================================
void       NwkUsbObj2Private :: resetPipe( )
{
    if ( m_int_in_io_desc.pipeId() != 0 ) { WinUsb_ResetPipe( m_h_usb, m_int_in_io_desc.pipeId() ); }
    if ( m_int_out_io_desc.pipeId()!= 0 ) { WinUsb_ResetPipe( m_h_usb, m_int_out_io_desc.pipeId() ); }
    if ( m_str_bulk_in_io_desc.pipeId() != 0) { WinUsb_ResetPipe( m_h_usb, m_str_bulk_in_io_desc.pipeId() );  }
    if ( m_str_bulk_out_io_desc.pipeId()!= 0) { WinUsb_ResetPipe( m_h_usb, m_str_bulk_out_io_desc.pipeId() ); }
    if ( m_blk_bulk_in_io_desc.pipeId() != 0) { WinUsb_ResetPipe( m_h_usb, m_blk_bulk_in_io_desc.pipeId());  }
    if ( m_blk_bulk_out_io_desc.pipeId()!= 0) { WinUsb_ResetPipe( m_h_usb, m_blk_bulk_out_io_desc.pipeId()); }
}


// ===================================================================
// control transfer ( sync. )
// ===================================================================
intptr_t    NwkUsbObj2Private:: ctlTransSync( NwkUsbObj2::SetupPacket *sp, NwkUsbObj2::DataPacket *dp, intptr_t *bytes_trans )
{
    intptr_t ret = SCIPACK_S_OK;
    if ( m_status != NwkUsbObj2::StatusID_S_OK  ) { ret = SCIPACK_E_UNEXPECTED; goto L_Exit; }

    WINUSB_SETUP_PACKET     u_setup;  BOOL w_ret; DWORD recv_bytes;
    u_setup.RequestType = sp->m_req_type;  u_setup.Request = sp->m_req;
    u_setup.Value       = sp->m_value;     u_setup.Index   = sp->m_index;
    u_setup.Length      = sp->m_length;

    if ( dp == SCIPACK_NULL_PTR ) {
        w_ret = WinUsb_ControlTransfer( m_h_usb, u_setup, NULL, 0, & recv_bytes, 0 );
    } else {
        if ( dp->dat_ptr == SCIPACK_NULL_PTR ) {
            w_ret = WinUsb_ControlTransfer( m_h_usb, u_setup, NULL, 0, & recv_bytes, 0 );
        } else {
            dp->trans_s_id = NwkUsbObj2::TransStatusID_S_Working;
            w_ret = WinUsb_ControlTransfer( m_h_usb, u_setup, ( PUCHAR )( dp->dat_ptr ), dp->dat_size, & recv_bytes, 0 );
            if ( w_ret == TRUE ) { 
                dp->trans_s_id = NwkUsbObj2::TransStatusID_S_OK;
            } else {
                dp->trans_s_id = NwkUsbObj2::TransStatusID_E_Trans_Failed;
            }
        }
    }
    if ( ! w_ret ) {
        ret = SCIPACK_E_UNEXPECTED;
        scipack_dbg_err("ctlTransSync failed, err=0x%x", GetLastError());
    } else {
        if ( bytes_trans != SCIPACK_NULL_PTR ) { *bytes_trans = ( intptr_t )( recv_bytes ); }
    }
L_Exit:
    return ret;
}

// =================================================================
// do the IoRoutine
// =================================================================
bool           NwkUsbObj2Private :: doIoRoutine( IoDesc *io_desc, NwkUsbObj2::DataPacket *dpkt )
{
     BOOL w_ret = TRUE; DWORD  trans_bytes; DWORD err_code;  bool is_req_wait_event = false;
     OVERLAPPED             *ovlp = io_desc->ovlpPtr();
     OvlpStateID            *st   = io_desc->ovlpStateIDPtr();
     intptr_t               *dsz  = io_desc->dataSizePtr();
     HANDLE                 *h_wt = io_desc->waitEventPtr();
     intptr_t                ready_trans_num = 0;      // ready transfer number + resetevent failed transfer number

    // -----------------------------------------------------
    // cond. check the pipe ID is valid.
    // -----------------------------------------------------
    if ( io_desc->pipeId() == 0 || h_wt == NULL ) { 
         dpkt->trans_s_id = NwkUsbObj2::TransStatusID_E_Unknown_Error;  
         goto L_Exit; 
    }

    // -----------------------------------------------------
    // stage: check dpk
    // -----------------------------------------------------
    if ( io_desc->stateID() == BulkStateID_Ready ) {
        if ( dpkt->dat_size > 0 ) { 
            io_desc->stateID() = BulkStateID_WaitForPost; io_desc->noError() = true; io_desc->fatal() = false; 
        }
    }

    // ------------------------------------------------------
    // stage: check for transfered IoPending result and count for ready transfer.
    // ------------------------------------------------------
    if ( io_desc->stateID() == BulkStateID_WaitForPost ||  io_desc->stateID() == BulkStateID_IoPending ) {
        ready_trans_num = 0;
        for ( intptr_t x = 0; x < io_desc->unitNum(); x ++ ) {
            if ( st[x] == OvlpStateID_Ready  || st[x] == OvlpStateID_ResetEventFailed ) { ++ ready_trans_num; }
            if ( st[x] != OvlpStateID_IoPending ) { continue; }                     // is the IoPending transfer ?
            err_code = WaitForSingleObject( h_wt[x], 0 );

            // check the result
            switch( err_code ) {
            case WAIT_ABANDONED: scipack_dbg_err("IoRoutine( pipe type:0x%x): %d event wait abandoned. err=0x%x, m_h_usb=0x%x\n", io_desc->pipeType(), x, GetLastError(), m_h_usb ); break;
            case WAIT_OBJECT_0 : break;
            case WAIT_TIMEOUT  : break;
            case WAIT_FAILED   : scipack_dbg_err("IoRoutine( pipe type:0x%x): %d event wait failed. err=0x%x, m_h_usb=0x%x\n", io_desc->pipeId(), x, GetLastError(), m_h_usb ); break;
            default: break;
            }
            if ( err_code != WAIT_OBJECT_0 ) { continue; } // the transfer is signed ?
            w_ret = WinUsb_GetOverlappedResult( m_h_usb, &ovlp[x], & trans_bytes, FALSE );
            if ( w_ret ) {
                if ( ResetEvent( h_wt[x] )) {
                    st[x] = OvlpStateID_Ready; ++ ready_trans_num;            // read successfully, reset it to ready.
                } else {
                    st[x] = OvlpStateID_ResetEventFailed; ++ ready_trans_num; // reset event failed. skip this transfer
                    scipack_dbg_err("IoRoutine( pipe type:0x%x ): reset %d event failed, err=0x%x, m_h_usb=0x%x", io_desc->pipeType(), x, GetLastError(),m_h_usb);
                }
            } else {
                if ( ResetEvent( h_wt[x] )) {
                    st[x] = OvlpStateID_Ready; ++ ready_trans_num;  io_desc->noError() = false; // try next time ( this transfer failed )
                } else {
                    st[x] = OvlpStateID_ResetEventFailed; ++ ready_trans_num;  io_desc->noError() = false;  io_desc->fatal() = true;  // some fatal error occured.
                    scipack_dbg_err("IoRoutine( pipe type:0x%x): reset %d event failed, err=0x%x, m_h_usb=0x%x", io_desc->pipeType(), x, GetLastError(), m_h_usb);
                }
            }
        }
    }

    // ------------------------------------------------------
    //3) stage: submit transfer
    // ------------------------------------------------------
    if ( io_desc->stateID() == BulkStateID_WaitForPost || io_desc->stateID() == BulkStateID_IoPending ) {
        if ( dpkt->dat_size > 0 ) {  // exist remain data
            // find a transfer to post
            for ( intptr_t x = 0; x < io_desc->unitNum() && dpkt->dat_size > 0; x ++ ) {
                if ( st[ x ] != OvlpStateID_Ready ) { continue; }
                dsz[ x ] = ( dpkt->dat_size > io_desc->maxTransSize() ? io_desc->maxTransSize() : dpkt->dat_size );

                // reset event and fill overlapped
                // memset( & ovlp[ x ], 0, sizeof( OVERLAPPED ) );
                memset( & ovlp[x], 0, sizeof( OVERLAPPED ));
                ovlp[x].hEvent = h_wt[x];

                // try post the transfer
                if ( ( io_desc->pipeType() & 0x01 )) {  // IN
                    w_ret = WinUsb_ReadPipe ( m_h_usb, io_desc->pipeId(),( PUCHAR )( dpkt->dat_ptr ), dsz[ x ], NULL, & ovlp[x] );
                } else {                               // OUT
                    w_ret = WinUsb_WritePipe( m_h_usb, io_desc->pipeId(), ( PUCHAR )( dpkt->dat_ptr ), dsz[ x ], NULL, & ovlp[x]);
                }
                if ( ! w_ret ) {
                    if ( ( err_code = GetLastError()) == ERROR_IO_PENDING ) { w_ret = TRUE; }
                }  // is io pending ?
                if ( w_ret ) {
                    dpkt->dat_size -= dsz[ x ];   dpkt->dat_ptr   = ( uint8_t *)( dpkt->dat_ptr ) + dsz[x];
                    st[ x ] = OvlpStateID_IoPending;  -- ready_trans_num;  // used a ready transfer.
                } else {
                    io_desc->fatal() = true;
                    scipack_dbg_err("IoRoutine( pipe type:0x%x): transfer ( %d ) submit failed, err = 0x%x, m_h_usb=0x%x", io_desc->pipeType(), x, GetLastError(), m_h_usb);
                }
            }
            // ------------------------------------------------------------------------------------------------------------------
            // fatal: some error occured
            // ready_trans_num == transfer unit number : we did not submit any transfer ( error ? )
            // ------------------------------------------------------------------------------------------------------------------
            if ( io_desc->fatal() ||  ready_trans_num == io_desc->unitNum() ) {
                scipack_dbg_err("IoRoutine( pipe type:0x%x): detected transfer failed, set status to StatusID_E_InnerError.", io_desc->pipeType() );
                io_desc->stateID() = BulkStateID_PostFailed; m_status = NwkUsbObj2::StatusID_E_Inner_Error;
                dpkt->trans_s_id = NwkUsbObj2::TransStatusID_E_Trans_Failed;  
            } else {
                if ( ready_trans_num != io_desc->unitNum() ) { is_req_wait_event = true; } // there is some block wait for event
            }
        } else {
            // ------------------------------------------------------------------------------------------------------------------
            // when all transfer finished, and no remain data. this frame is ok.
            // ( ready_trans_num equal to NWK_OVLP_BULK_N_NUM means no transfer is working )
            // ------------------------------------------------------------------------------------------------------------------
            if ( ready_trans_num == io_desc->unitNum() ) { // wait for last transfer finished ( ready or resetevent failed )
                if ( ! io_desc->fatal() ) { // no fatal
                    io_desc->stateID() = BulkStateID_Ready;
                    dpkt->trans_s_id = ( io_desc->noError() ? NwkUsbObj2::TransStatusID_S_OK : NwkUsbObj2::TransStatusID_E_Exist_BadBlock );
                } else {
                    scipack_dbg_err("IoRoutine( pipe type:0x%x): detected transfer failed while dpkt->dat_size == 0. set status to StatusID_InnerError.", io_desc->pipeType());
                    io_desc->stateID() = BulkStateID_PostFailed; m_status = NwkUsbObj2::StatusID_E_Trans_Error;
                    dpkt->trans_s_id = NwkUsbObj2::TransStatusID_E_Trans_Failed;
                }
            }
        }
    }

L_Exit:
    return is_req_wait_event;
}

// =================================================================
// do the bulk transfer
// =================================================================
intptr_t   NwkUsbObj2Private :: bulkTransSync ( NwkUsbObj2::PipeTypeID p_id, NwkUsbObj2::DataPacket *dp )
{
    intptr_t ret = SCIPACK_S_OK;
    if ( m_status != NwkUsbObj2::StatusID_S_OK || dp == SCIPACK_NULL_PTR  ) { ret = SCIPACK_E_UNEXPECTED; goto L_Exit; }

    // ---------------------------------------------------
    // stage: set datapacket flag, means start working
    // ---------------------------------------------------
    dp->trans_s_id = NwkUsbObj2::TransStatusID_S_Working;

    // ---------------------------------------------------
    // stage: select the io descriptor for p_id
    // ---------------------------------------------------
    IoDesc *io_desc;
    switch ( p_id ) {
    case NwkUsbObj2::PipeTypeID_IntIn :     io_desc = & m_int_in_io_desc;  break;
    case NwkUsbObj2::PipeTypeID_IntOut:     io_desc = & m_int_out_io_desc; break;
    case NwkUsbObj2::PipeTypeID_StrBulkIn : io_desc = & m_str_bulk_in_io_desc;  break;
    case NwkUsbObj2::PipeTypeID_StrBulkOut: io_desc = & m_str_bulk_out_io_desc; break;
    case NwkUsbObj2::PipeTypeID_BlkBulkIn : io_desc = & m_blk_bulk_in_io_desc;  break;
    case NwkUsbObj2::PipeTypeID_BlkBulkOut: io_desc = & m_blk_bulk_out_io_desc; break;
    default : io_desc = SCIPACK_NULL_PTR; break;
    }    

    // ---------------------------------------------------
    // stage: do the working
    // ---------------------------------------------------
    if ( io_desc != SCIPACK_NULL_PTR ) {
        volatile NwkUsbObj2::TransStatusID *s_id = &dp->trans_s_id;
        DWORD w_ret;
        while ( this->status( ) == NwkUsbObj2::StatusID_S_OK && *s_id == NwkUsbObj2::TransStatusID_S_Working ) {
            if ( this->doIoRoutine ( io_desc, dp ) ) {
                w_ret = WaitForMultipleObjectsEx( io_desc->unitNum( ) + 1, io_desc->waitEventPtr( ), FALSE, INFINITE, TRUE );
            }
        }
        if ( this->status( ) == NwkUsbObj2::StatusID_S_OK ) { 
            ret = ( *s_id == NwkUsbObj2::StatusID_S_OK ? SCIPACK_S_OK : SCIPACK_E_UNEXPECTED );
        }        
     } else {
        dp->trans_s_id = NwkUsbObj2::TransStatusID_E_Unknown_Error;
        ret = SCIPACK_E_UNEXPECTED;
     }

L_Exit:    
    return ret;
}


// /////////////////////////////////////////////////////////////////////
//  exports
// /////////////////////////////////////////////////////////////////////
// =====================================================================
//!  CTOR
/*!
  @param  vid_pid [ in ]  the vendor ID and product ID in USB spec. VID is the high part.
  @param  cfg_id  [ in ]  the configuration ID of the USB Device.
  @param  if_set  [ in ]  0xaabbccdd means 0xaa -- interface class  0xbb -- sub class  0xcc -- protocol id  0xdd -- alt. settings
  @param  if_id   [ in ]  the interface index.
  @param  idx     [ in ]  when mutliple USB device, this paramter is used to identify the index.
*/
// =====================================================================
                NwkUsbObj2 :: NwkUsbObj2 ( uint32_t  vid_pid, uint8_t cfg_id, uint32_t if_set, uint8_t if_id, intptr_t idx )
{
    NwkUsbObj2Private *t_this;

    m_rsvd = 0;
    t_this = scipack_new( NwkUsbObj2Private, this, vid_pid, cfg_id, if_set, if_id, idx );
    m_obj  = ( void *)( t_this );
}

// =====================================================================
//!  DTOR
// =====================================================================
                NwkUsbObj2 :: ~NwkUsbObj2 ( )
{
    NwkUsbObj2Private *t_this;
      
    scipack_assert( this != SCIPACK_NULL_PTR  &&  m_obj != SCIPACK_NULL_PTR );
    t_this = ( NwkUsbObj2Private *)( m_obj );
    scipack_delete( t_this, NwkUsbObj2Private );
}

// =====================================================================
//! return the current status ID
// =====================================================================
NwkUsbObj2::StatusID   NwkUsbObj2 :: status( )
{
    scipack_assert( this != SCIPACK_NULL_PTR  &&  m_obj != SCIPACK_NULL_PTR );
    return (( NwkUsbObj2Private *) m_obj )->status();
}
  
// =====================================================================
//! sync. control transfer
/*!
  @param  setup [ in ]  the USB Setup package
  @param  dat   [ in ]  the DataPack descr.
  @param  bytes_trans [ opt_out ]  recv. the bytes that transferred.
  @return SCIPACK_S_OK for no error.
*/
// =====================================================================
intptr_t              NwkUsbObj2 ::  ctlTransSync ( SetupPacket *setup, DataPacket *dat, intptr_t *bytes_trans )
{
    scipack_assert( this != SCIPACK_NULL_PTR  &&  m_obj != SCIPACK_NULL_PTR );
    return (( NwkUsbObj2Private *) m_obj )->ctlTransSync( setup, dat, bytes_trans );
}

// =====================================================================
//! bulk sync. transfer
/*!
  @param  p_id  [ in ]  pipe type id
  @param  dat   [ in ]  the DataPack descr.
  @return SCIPACK_S_OK for no error.
*/
// =====================================================================
intptr_t              NwkUsbObj2 :: bulkTransSync( NwkUsbObj2::PipeTypeID  p_id,  DataPacket *dat )
{
    scipack_assert( this != SCIPACK_NULL_PTR  &&  m_obj != SCIPACK_NULL_PTR );
    return (( NwkUsbObj2Private *) m_obj )->bulkTransSync( p_id, dat );
}


// ////////////////////////////////////////////////////////////////////////
// static
// ////////////////////////////////////////////////////////////////////////
int   NwkUsbObj2 :: currNewCntr( )    { return scipack_new_cntr; }
int   NwkUsbObj2 :: currMallocCntr( ) { return scipack_malloc_cntr; }



SCIPACK_NAMESPACE_END


#endif 
#endif
