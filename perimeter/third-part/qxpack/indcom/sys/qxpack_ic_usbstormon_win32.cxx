#ifndef QXPACK_IC_USBSTORMON_WIN32_CXX
#define QXPACK_IC_USBSTORMON_WIN32_CXX

#include "qxpack_ic_usbstormon.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"

#include <windows.h>
#include <SetupAPI.h>
#include <cfgmgr32.h>
#include <Dbt.h>

#include <QWindow>
#include <QMap>
#include <QAbstractNativeEventFilter>
#include <QByteArray>
#include <QMetaObject>

#ifdef __GNUC__ // in GCC 5, close below warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif


namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//  platform spec. API
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// open the volume
// ============================================================================
static HANDLE  gOpenVolumeByLetter( char drv )
{
    wchar_t drv_path[16];
    swprintf_s( drv_path, L"\\\\.\\%c:", drv );
    return ::CreateFileW( drv_path, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
}

// ============================================================================
// close the volume
// ============================================================================
static void   gCloseVolByHandle( HANDLE obj )
{
    HANDLE hVol = obj;
    if ( hVol != INVALID_HANDLE_VALUE ) { ::CloseHandle( hVol ); }
}

// ============================================================================
// get the volume device number
// ============================================================================
static DWORD  gGetDevNumberByDevHandle( HANDLE obj )
{
    STORAGE_DEVICE_NUMBER  sdn;
    DWORD  bytes_return = 0;
    BOOL   ret = ::DeviceIoControl( obj, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL, 0, &sdn, sizeof(sdn), &bytes_return,NULL);
    return ( ret ? sdn.DeviceNumber : DWORD(-1));
}

// ============================================================================
// make < drv_num, dev_num > pair
// ============================================================================
static bool   gMake_DrvNum_DevNum_Map( QMap<DWORD,DWORD> &dd_map )
{
    DWORD  m = ::GetLogicalDrives();
    dd_map.clear();

    for ( size_t i = 0; i < sizeof(DWORD) * 8; i ++, m >>= 1 ) {
        if (( m & 1 ) == 0 ) { continue; }

        HANDLE h = gOpenVolumeByLetter( char( i + 'A') );
        if ( h == INVALID_HANDLE_VALUE ) { continue; }
        DWORD  dev_num = gGetDevNumberByDevHandle( h );
        gCloseVolByHandle( h );
        dd_map.insert( DWORD(i), dev_num );
    }
    return ! dd_map.isEmpty();
}

// ============================================================================
// the length of wchar_t string
// ============================================================================
static size_t  gWStrLen( const wchar_t *str )
{
    const wchar_t *curr = str;  while ( *( curr ++ ) != 0 ) { }
    return size_t( curr - str );
}

// ============================================================================
// lower the string
// ============================================================================
static void    gMakeWStrLower( wchar_t *dst )
{
    while( *dst != 0 ) {
        if ( *dst >= 'A' && *dst <= 'Z') { *dst = *dst - 'A' + 'a'; }
        ++ dst;
    }
}

// ============================================================================
// make < dev_num, dev_inst > pair
// ============================================================================
static const wchar_t*  gUsbStorStr = L"\\\\?\\usbstor#disk";
static bool   gMake_DevNum_DevInst_Map( QMap<DWORD,DWORD> &di_map )
{
    di_map.clear();
    typedef GUID* GUIDPTR;
    GUIDPTR   guid    = GUIDPTR( &GUID_DEVINTERFACE_DISK );
  //  GUIDPTR   guid    = GUIDPTR( &GUID_DEVINTERFACE_USBSTOR );
    HDEVINFO hDevInfo = ::SetupDiGetClassDevsW( guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE );
    if ( hDevInfo == INVALID_HANDLE_VALUE ) { return false; }

    PSP_DEVICE_INTERFACE_DETAIL_DATA_W psp_didd =
            reinterpret_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA_W>( std::malloc( 4096 ));
    SP_DEVICE_INTERFACE_DATA  sp_did;
    SP_DEVINFO_DATA           sp_dd;
    DWORD    index    = 0;
    BOOL     ret      = FALSE;
    const size_t  uss_len = gWStrLen( gUsbStorStr );

    while ( true ) {
        sp_did.cbSize = sizeof( sp_did );
        ret = ::SetupDiEnumDeviceInterfaces( hDevInfo, NULL, guid, index ++, &sp_did );
        if ( ret != TRUE ) { break; }

        DWORD size = 0;
        ::SetupDiGetDeviceInterfaceDetailW( hDevInfo, &sp_did, NULL, 0, &size, NULL );
        if ( size > 0 && size < 4096 ) {
            psp_didd->cbSize = sizeof( *psp_didd );
            sp_dd.cbSize     = sizeof( sp_dd );
            ret = ::SetupDiGetDeviceInterfaceDetailW( hDevInfo, &sp_did, psp_didd, size, &size, &sp_dd );
           // wprintf(L"path:%s\n", psp_didd->DevicePath);
            if ( gWStrLen( psp_didd->DevicePath ) < uss_len ) { continue; }

            wchar_t tmp[ 20 ]; tmp[ uss_len ] = 0;
            std::memcpy( tmp, psp_didd->DevicePath, uss_len );
            gMakeWStrLower( tmp );

            int is_same = std::memcmp( tmp, gUsbStorStr, uss_len );
            if ( is_same == 0 ) {
                HANDLE hDrv = ::CreateFileW( psp_didd->DevicePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
                if ( hDrv != INVALID_HANDLE_VALUE ) {
                    DWORD dev_num = gGetDevNumberByDevHandle( hDrv );
                    ::CloseHandle( hDrv );
                    di_map.insert( dev_num, sp_dd.DevInst );
                }
            }
        }
    }
    ::SetupDiDestroyDeviceInfoList( hDevInfo );

    std::free( psp_didd );
    return true;
}

// ============================================================================
// get the disk label by driver letter
// ============================================================================
static QString  gGetDiskLabelByDrvLetter( char drv, wchar_t *lbl_buff, size_t lbl_buff_sz )
{
    wchar_t drv_path[8];
    swprintf_s( drv_path, L"%c:\\", drv );
    QString  disk_lbl;
    if ( ::GetVolumeInformationW( drv_path, lbl_buff, DWORD(lbl_buff_sz), NULL, NULL, NULL, NULL, 0 ) == TRUE ) {
        disk_lbl = QString::fromWCharArray( lbl_buff );
    } else {
        disk_lbl = QString( QObject::tr("USB DISK") );
    }
    return disk_lbl;
}

// ============================================================================
// get the disk cap. and free size
// ============================================================================
static QPair<quint64,quint64>  gGetDiskSizeInfo( char drv )
{
    wchar_t drv_path[8];
    swprintf_s( drv_path, L"%c:\\", drv );
    QPair<quint64,quint64>  sz_info;

    ULARGE_INTEGER user_tob, user_fob;
    if ( ::GetDiskFreeSpaceExW( drv_path, NULL, &user_tob, &user_fob ) == TRUE ) {
        sz_info.first = user_tob.QuadPart;
        sz_info.second= user_fob.QuadPart;
    } else {
        sz_info.first = 0; sz_info.second = 0;
    }

    return sz_info;
}


// ////////////////////////////////////////////////////////////////////////////
// private object
// ////////////////////////////////////////////////////////////////////////////
#define T_PrivPtr( o )  T_ObjCast( IcUsbStorMonPriv*, o )
class QXPACK_IC_HIDDEN  IcUsbStorMonPriv : public QObject {
    Q_OBJECT
private:
    QList<IcUsbStorInfo>  m_stor_list;
    QMap<DWORD,DWORD>     m_devn_devi_map, m_drv_devn_map;
    DWORD    m_vol_mask;
    QWindow *m_msg_win;
    HANDLE   m_dev_ntf;
    LONG_PTR m_msg_win_proc, m_msg_win_userdata;
protected:
                bool  makeVolMask ( );
                void  makeVolInfoList ( );
    Q_INVOKABLE void  flushVolInfo( );

    static LRESULT CALLBACK MsgWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );
public :
    explicit IcUsbStorMonPriv ( );
    virtual ~IcUsbStorMonPriv ( ) Q_DECL_OVERRIDE;

    inline QList<QxPack::IcUsbStorInfo> &   storListRef() { return m_stor_list; }
    inline QMap<DWORD,DWORD>&  devNumDevInstMapRef() { return m_devn_devi_map; }
    inline QMap<DWORD,DWORD>&  drvNumDevNumMapRef()  { return m_drv_devn_map;  }
           bool  queryDevInstByDrvNum( int drv_num, DWORD & );

    Q_SIGNAL void listChanged();
};

// ============================================================================
// ctor
//=============================================================================
IcUsbStorMonPriv :: IcUsbStorMonPriv ( )
{
    m_vol_mask = 0;
    flushVolInfo();

    m_msg_win = new QWindow;
    m_msg_win->setWidth(1); m_msg_win->setHeight(1);
    m_msg_win->setVisible(false);
    m_msg_win_userdata = ::SetWindowLongPtrW( HWND( m_msg_win->winId() ), GWLP_USERDATA, LONG_PTR(this));
    m_msg_win_proc     = ::SetWindowLongPtrW( HWND( m_msg_win->winId() ), GWLP_WNDPROC,  LONG_PTR(&MsgWndProc));

    DEV_BROADCAST_DEVICEINTERFACE ntf_ftr;
    ZeroMemory( &ntf_ftr, sizeof(ntf_ftr));
    ntf_ftr.dbcc_size = sizeof(ntf_ftr);
    ntf_ftr.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    ntf_ftr.dbcc_classguid  = GUID_DEVINTERFACE_DISK;
    m_dev_ntf = ::RegisterDeviceNotificationW( HANDLE( m_msg_win->winId() ), &ntf_ftr, DEVICE_NOTIFY_WINDOW_HANDLE );
}

// ============================================================================
// dtor
// ============================================================================
IcUsbStorMonPriv :: ~IcUsbStorMonPriv ( )
{
    ::SetWindowLongPtrW( HWND( m_msg_win->winId()), GWLP_USERDATA, m_msg_win_userdata );
    ::SetWindowLongPtrW( HWND( m_msg_win->winId()), GWLP_WNDPROC,  m_msg_win_proc );
    if ( m_dev_ntf != INVALID_HANDLE_VALUE ) {
        ::UnregisterDeviceNotification( m_dev_ntf );
    }
    delete m_msg_win;
}

// ============================================================================
// make volume mask
// ============================================================================
bool      IcUsbStorMonPriv :: makeVolMask( )
{
    DWORD  drv_mask = 0;

    QMap<DWORD,DWORD> dd_map, di_map;
    gMake_DevNum_DevInst_Map( di_map );
    gMake_DrvNum_DevNum_Map ( dd_map );

    for ( int i = sizeof(DWORD)*8-1; i >= 0; i -- ) {
        drv_mask <<= 1;
        const auto dd_itr = dd_map.constFind( DWORD(i) );
        if ( dd_itr == dd_map.constEnd()) { continue; }
        const auto di_itr = di_map.constFind( dd_itr.value() );
        if ( di_itr == di_map.constEnd()) { continue; }
        drv_mask |= 1;
    }

    bool is_chg = ( m_vol_mask != drv_mask );
    m_vol_mask      = drv_mask;
    m_drv_devn_map  = dd_map;
    m_devn_devi_map = di_map;

    return is_chg;
}

// ============================================================================
// make the volume information
// ============================================================================
void      IcUsbStorMonPriv :: makeVolInfoList( )
{
    QList<IcUsbStorInfo>  lst;
    wchar_t *lbl_buff = reinterpret_cast<wchar_t*>( std::malloc(( MAX_PATH + 1 ) * sizeof(wchar_t)) );

    DWORD  drv_mask = m_vol_mask;
    for ( size_t i = 0; i < sizeof(DWORD) * 8; i ++, drv_mask >>= 1 ) {
        if (( drv_mask & 1 ) == 0 ) { continue; }

        IcUsbStorInfo  info;
        info.setRootPath( QString("%1:/").arg( QChar(char( i + 'A')) ));
        info.setLabel   ( gGetDiskLabelByDrvLetter( char(i) + 'A', lbl_buff, MAX_PATH+1 ));
        QPair<quint64,quint64> sz_info = gGetDiskSizeInfo( char(i) + 'A');
        info.setCapacity ( sz_info.first  );
        info.setFreeSpace( sz_info.second );

        lst.append( info );
    }
    std::free( lbl_buff );

    m_stor_list = lst;
}

// ============================================================================
// get the device instance by drive letter
// ============================================================================
bool   IcUsbStorMonPriv :: queryDevInstByDrvNum( int drv_num, DWORD &inst )
{
    const auto &dd_itr = m_drv_devn_map.constFind( DWORD(drv_num) );
    if ( dd_itr == m_drv_devn_map.constEnd() ) { return false; }
    const auto &di_itr = m_devn_devi_map.constFind( DWORD(dd_itr.value()));
    if ( di_itr != m_devn_devi_map.constEnd()) {
        inst = di_itr.value();
        return true;
    } else {
        return false;
    }
}

// ============================================================================
// [ ivk ] enum all USB storage information
// ============================================================================
void    IcUsbStorMonPriv :: flushVolInfo( )
{
    bool is_chg = makeVolMask();
    if ( is_chg ) {
        makeVolInfoList();
        QMetaObject::invokeMethod( this, "listChanged", Qt::QueuedConnection );
    }
}

// ============================================================================
// [ static ] the msg window procedure
// ============================================================================
LRESULT CALLBACK  IcUsbStorMonPriv :: MsgWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    // ------------------------------------------------------------------------
    // filter the device change message
    // ------------------------------------------------------------------------
    LONG_PTR ud = 0;
    if ( msg == WM_DEVICECHANGE &&
        ( wParam == DBT_DEVICEARRIVAL || wParam == DBT_DEVICEREMOVECOMPLETE ))
    {
        PDEV_BROADCAST_HDR dev_bc_hdr = PDEV_BROADCAST_HDR( lParam );
        if ( dev_bc_hdr->dbch_devicetype == DBT_DEVTYP_VOLUME ) {
            ud = ::GetWindowLongPtrW( hwnd, GWLP_USERDATA );
            if ( ud != 0 ) {
                auto *t_this = reinterpret_cast<IcUsbStorMonPriv*>(ud);
                QMetaObject::invokeMethod( t_this, "flushVolInfo", Qt::QueuedConnection );
            }
        }
    }

    // ------------------------------------------------------------------------
    // call default window process
    // ------------------------------------------------------------------------
    LRESULT ret = 0;
    if ( ud == 0 ) { ud = ::GetWindowLongPtrW( hwnd, GWLP_USERDATA ); }
    if ( ud != 0 ) {
        auto *t_this = reinterpret_cast<IcUsbStorMonPriv*>(ud);
        ::SetWindowLongPtrW ( hwnd, GWLP_USERDATA, t_this->m_msg_win_userdata ); // restore old user data
        ret = ::CallWindowProcW( reinterpret_cast<WNDPROC>( t_this->m_msg_win_proc ), hwnd, msg, wParam, lParam );
        ::SetWindowLongPtrW ( hwnd, GWLP_USERDATA, LONG_PTR(t_this));
    }

    return ret;
}



// ////////////////////////////////////////////////////////////////////////////
// wrap API
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcUsbStorMon :: IcUsbStorMon ( QObject *pa ) : QObject( pa )
{
    m_obj = qxpack_ic_new( IcUsbStorMonPriv );
    QObject::connect( T_PrivPtr( m_obj ), SIGNAL(listChanged()), this, SIGNAL(listChanged()));
}

// ============================================================================
// dtor
// ============================================================================
IcUsbStorMon :: ~IcUsbStorMon ( )
{
    this->blockSignals(true);
    qxpack_ic_delete( m_obj, IcUsbStorMonPriv );
}

// ============================================================================
// return the usb storage information list
// ============================================================================
QList<QxPack::IcUsbStorInfo> IcUsbStorMon :: infoList() const
{ return ( m_obj != nullptr ? T_PrivPtr(m_obj)->storListRef() : QList<QxPack::IcUsbStorInfo>() ); }

// ============================================================================
// try eject spec. volume
// ============================================================================
bool           IcUsbStorMon :: ejectVol ( const QString &vol_path )
{
    if ( vol_path.isEmpty()) { return false; }
    int sz = vol_path.size();
    if ( sz < 3 ) { return false; }

    QChar drv_letter = vol_path.at(0);
    QChar drv_colon  = vol_path.at(1);
    QChar drv_slash  = vol_path.at(2);
    if ( drv_colon != ':' ) { return false; }
    if ( drv_slash  != '\\' && drv_slash  != '/' ) { return false; }

    if ( drv_letter >= 'a'  &&  drv_letter <= 'z' ) { drv_letter = char( drv_letter.toLatin1() - 'a' + 'A' ); }
    if ( drv_letter < 'A' || drv_letter > 'Z' ) { return false; }

    DWORD dev_inst = 0;
    if ( ! T_PrivPtr(m_obj)->queryDevInstByDrvNum( drv_letter.toLatin1() - 'A', dev_inst )) { return false; }

    DEVINST   dev_inst_parent = 0;
    CONFIGRET cfg_ret = CM_Get_Parent( &dev_inst_parent, dev_inst, 0 );
    if ( cfg_ret == CR_SUCCESS ) {
        cfg_ret = CM_Request_Device_EjectW( dev_inst_parent, NULL, NULL, 0, 0 );
    }
    bool ret = ( cfg_ret == CR_SUCCESS );
    qDebug() << "eject USB Storage: " << vol_path << " " << ( ret ? "OK" : "Failed");
    return ret;
//    QString corr_vol_path;
//    const QChar *dat = vol_path.constData();
//    if ( dat[ vol_path.length() - 1 ] == '/' ) {
//        corr_vol_path =
//    }

}

}



#ifdef __GNUC__ // in GCC 5, close below warnings
#pragma GCC diagnostic pop
#endif

#include "qxpack_ic_usbstormon_win32.moc"
#endif
