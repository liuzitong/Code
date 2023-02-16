#ifndef USBDEV_STATUSDATA_CXX
#define USBDEV_STATUSDATA_CXX

#include "usbdev_statusdata.hxx"
#include "usbdev/common/usbdev_pimplprivtemp.hpp"
#include "usbdev_profile.hxx"

#include <QMetaType>

namespace UsbDev {

// ////////////////////////////////////////////////////////////////////////////
// register in Qt Meta system
// ////////////////////////////////////////////////////////////////////////////
static bool g_is_reg = false;
static void gRegInQt ( )
{
    if ( ! g_is_reg ) {
        qRegisterMetaType<UsbDev::StatusData>(); g_is_reg = true;
    }
}

// ////////////////////////////////////////////////////////////////////////////
// motor item
// ////////////////////////////////////////////////////////////////////////////
class USBDEV_HIDDEN StatusData_MotorItem {
private:
    bool m_is_busy;
    int  m_cmd_cntr;
    qint32 m_pos;
public :
    explicit StatusData_MotorItem( )
    {
        m_is_busy = false;
        m_cmd_cntr = 0;
        m_pos = 0;
    }
    StatusData_MotorItem & operator = ( const StatusData_MotorItem &o )
    {
        m_is_busy  = o.m_is_busy;
        m_cmd_cntr = o.m_cmd_cntr;
        m_pos      = o.m_pos;
        return *this;
    }

    inline auto  cmdCntrRef()   -> int&    { return m_cmd_cntr; }
    inline auto  positionRef()  -> qint32& { return m_pos; }
    inline auto  isBusyRef()    -> bool&   { return m_is_busy; }
};

// ////////////////////////////////////////////////////////////////////////////
// private object
// ////////////////////////////////////////////////////////////////////////////
#define T_PrivPtr( o )  usbdev_objcast( StatusDataPriv*, o  )
class USBDEV_HIDDEN StatusDataPriv : public PImplPrivTemp<StatusDataPriv> {
private:
    StatusData_MotorItem    m_x_motor,
                            m_y_motor,
                            m_focus_motor,
                            m_color_motor,
                            m_light_spot_motor,
                            m_shutter_motor,
                            m_hoz_chin_motor,
                            m_vert_chin_motor;
    int                     m_sn_no,
                            m_env_light_DA,
                            m_cast_light_DA;
    bool                    m_cache_normal_flag,
                            m_cache_move_flag,
                            m_answerpad_status,
                            m_camera_status,
                            m_eyeglass_status;
public :
    explicit StatusDataPriv ( );
    StatusDataPriv ( const StatusDataPriv & );
    virtual ~StatusDataPriv( ) Q_DECL_OVERRIDE { }

    inline auto  snNoRef()                      -> int&  { return m_sn_no; }
    inline auto  CacheNormalFlagRef()           -> bool& { return m_cache_normal_flag; }
    inline auto  CacheMoveFlagRef()             -> bool& { return m_cache_move_flag;  }
    inline auto  CameraStatusRef()              -> bool& { return m_camera_status; }
    inline auto  EyeglassStatusRef()            -> bool& { return m_eyeglass_status; }
    inline auto  AnswerPadStatusRef()           -> bool& { return m_answerpad_status; }
    inline auto  envLightDARef()                -> int&  { return m_env_light_DA; }
    inline auto  castLightDARef()               -> int&  { return m_cast_light_DA; }
    inline auto  xMotorRef()                    -> StatusData_MotorItem& { return m_x_motor; }
    inline auto  yMotorRef()                    -> StatusData_MotorItem& { return m_y_motor; }
    inline auto  focusMotorRef()                -> StatusData_MotorItem& { return m_focus_motor; }
    inline auto  colorMotorRef()                -> StatusData_MotorItem& { return m_color_motor;  }
    inline auto  lightSpotMotorRef()            -> StatusData_MotorItem& { return m_light_spot_motor; }
    inline auto  shutterMotorRef()              -> StatusData_MotorItem& { return m_shutter_motor; }
    inline auto  hozChinMotorRef()                -> StatusData_MotorItem& { return m_hoz_chin_motor; }
    inline auto  vertChinMotorRef()                -> StatusData_MotorItem& { return m_vert_chin_motor; }
};

// ============================================================================
// ctor
// ============================================================================
StatusDataPriv :: StatusDataPriv ( )
{
    m_sn_no=m_env_light_DA=m_cast_light_DA = 0;
    m_cache_normal_flag=m_cache_move_flag=m_answerpad_status=m_camera_status=m_eyeglass_status=false;
}

// ============================================================================
// ctor( copy )
// ============================================================================
StatusDataPriv :: StatusDataPriv ( const StatusDataPriv &o )
    :m_x_motor(o.m_x_motor),m_y_motor(o.m_y_motor),m_focus_motor(o.m_focus_motor),
     m_color_motor(o.m_color_motor),m_light_spot_motor(o.m_light_spot_motor),m_shutter_motor(o.m_shutter_motor),
     m_hoz_chin_motor(o.m_hoz_chin_motor),m_vert_chin_motor(o.m_vert_chin_motor), m_eyeglass_status(o.m_eyeglass_status)
{}

// ////////////////////////////////////////////////////////////////////////////
// help function
// ////////////////////////////////////////////////////////////////////////////
static qint32  gReadData_Le_I32( const unsigned char *buff )
{
    return  (  int( buff[0] ) & 255 ) | (( int( buff[1]) & 255 ) << 8 ) |
            (( int( buff[2] ) & 255 ) << 16 ) | (( int( buff[3]) & 255 ) << 24 );
}


static qint16  gReadData_Le_I16( const unsigned char *buff )
{
    return  qint16( ( int( buff[0] ) & 255 ) | (( int( buff[1] ) & 255 ) << 8 ));
}



// ////////////////////////////////////////////////////////////////////////////
// wrap API
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor ( empty )
// ============================================================================
StatusData :: StatusData( ) { gRegInQt(); m_obj = nullptr; }

// ============================================================================
// ctor ( construct )
// ============================================================================
StatusData :: StatusData ( const StatusData &o )
{
    gRegInQt(); m_obj = nullptr;
    StatusDataPriv::attach( & m_obj, const_cast<void**>(& o.m_obj));
}

// ============================================================================
// ctor ( assign )
// ============================================================================
StatusData &  StatusData :: operator = ( const StatusData &o )
{
    gRegInQt();
    StatusDataPriv::attach( & m_obj, const_cast<void**>( & o.m_obj ));
    return *this;
}

// ============================================================================
// dtor
// ============================================================================
StatusData :: ~StatusData( )
{
    if ( m_obj != nullptr ) {
        StatusDataPriv::attach( & m_obj, nullptr );
    }
}

// ============================================================================
// load the data
// ============================================================================
StatusData :: StatusData ( const QByteArray &ba )
{
    gRegInQt(); m_obj = nullptr;
    if ( ba.size() < 512 ) { return; }

    // ------------------------------------------------------------------------
    // build data
    // ------------------------------------------------------------------------
    void *d = nullptr;
    const unsigned char *buff = reinterpret_cast<const unsigned char*>( ba.constData());
    StatusDataPriv::buildIfNull( & d );

    auto priv = T_PrivPtr( d );

    priv->snNoRef()= buff[0];
    priv->CacheNormalFlagRef()=buff[1];
    priv->CacheMoveFlagRef()=buff[2];
    priv->AnswerPadStatusRef()=buff[3];
    priv->CameraStatusRef()=buff[4];
    priv->EyeglassStatusRef()=buff[5];
    priv->xMotorRef().isBusyRef()=buff[6];
    priv->yMotorRef().isBusyRef()=buff[7];
    priv->focusMotorRef().isBusyRef()=buff[8];
    priv->colorMotorRef().isBusyRef()=buff[9];
    priv->lightSpotMotorRef().isBusyRef()=buff[10];
    priv->shutterMotorRef().isBusyRef()=buff[11];
    priv->hozChinMotorRef().isBusyRef()=buff[12];
    priv->vertChinMotorRef().isBusyRef()=buff[13];
    priv->xMotorRef().cmdCntrRef()=buff[14];
    priv->yMotorRef().cmdCntrRef()=buff[15];
    priv->focusMotorRef().cmdCntrRef()=buff[16];
    priv->colorMotorRef().cmdCntrRef()=buff[17];
    priv->lightSpotMotorRef().cmdCntrRef()=buff[18];
    priv->shutterMotorRef().cmdCntrRef()=buff[19];
    priv->hozChinMotorRef().cmdCntrRef()=buff[20];
    priv->vertChinMotorRef().cmdCntrRef()=buff[21];
    priv->xMotorRef().positionRef()=gReadData_Le_I32(&buff[24]);
    priv->yMotorRef().positionRef()=gReadData_Le_I32(&buff[28]);
    priv->focusMotorRef().positionRef()=gReadData_Le_I32(&buff[32]);
    priv->colorMotorRef().positionRef()=gReadData_Le_I32(&buff[36]);
    priv->lightSpotMotorRef().positionRef()=gReadData_Le_I32(&buff[40]);
    priv->shutterMotorRef().positionRef()=gReadData_Le_I32(&buff[44]);
    priv->hozChinMotorRef().positionRef()=gReadData_Le_I32(&buff[48]);
    priv->vertChinMotorRef().positionRef()=gReadData_Le_I32(&buff[52]);
    priv->envLightDARef()=gReadData_Le_I32(&buff[56]);
    priv->castLightDARef()=gReadData_Le_I32(&buff[60]);

    m_obj = d;
}

// ============================================================================
// method access
// ============================================================================
bool     StatusData :: isEmpty() const
{ return ( m_obj == nullptr ); }

int      StatusData :: serialNO( ) const
{ return ( m_obj != nullptr ? T_PrivPtr( m_obj )->snNoRef() : 0 ); }

//bool     StatusData ::CameraStatus ( ) const
//{ return ( m_obj != nullptr ? T_PrivPtr( m_obj )->CameraStatusRef() : false ); }

bool     StatusData :: isMotorBusy( DevCtl::MotorId mot_id ) const
{
    if ( m_obj == nullptr ) { return false; }
    switch( mot_id ) {
    case DevCtl::MotorId_X :            return T_PrivPtr( m_obj )->xMotorRef().isBusyRef();
    case DevCtl::MotorId_Y :            return T_PrivPtr( m_obj )->yMotorRef().isBusyRef();
    case DevCtl::MotorId_Focus :        return T_PrivPtr( m_obj )->focusMotorRef().isBusyRef();
    case DevCtl::MotorId_Color:         return T_PrivPtr(m_obj)->colorMotorRef().isBusyRef();
    case DevCtl::MotorId_Light_Spot:    return T_PrivPtr(m_obj)->lightSpotMotorRef().isBusyRef();
    case DevCtl::MotorId_Shutter:       return T_PrivPtr(m_obj)->shutterMotorRef().isBusyRef();
    case DevCtl::MotorId_Chin_Hoz:      return T_PrivPtr(m_obj)->hozChinMotorRef().isBusyRef();
    case DevCtl::MotorId_Chin_Vert:     return T_PrivPtr(m_obj)->vertChinMotorRef().isBusyRef();
    default: return false;
    }
}



int      StatusData :: motorCmdCntr( DevCtl::MotorId  mot_id ) const
{
    if ( m_obj == nullptr ) { return false; }
    switch( mot_id ) {
    case DevCtl::MotorId_X :            return T_PrivPtr( m_obj )->xMotorRef().cmdCntrRef();
    case DevCtl::MotorId_Y :            return T_PrivPtr( m_obj )->yMotorRef().cmdCntrRef();
    case DevCtl::MotorId_Focus :        return T_PrivPtr( m_obj )->focusMotorRef().cmdCntrRef();
    case DevCtl::MotorId_Color:         return T_PrivPtr(m_obj)->colorMotorRef().cmdCntrRef();
    case DevCtl::MotorId_Light_Spot:    return T_PrivPtr(m_obj)->lightSpotMotorRef().cmdCntrRef();
    case DevCtl::MotorId_Shutter:       return T_PrivPtr(m_obj)->shutterMotorRef().cmdCntrRef();
    case DevCtl::MotorId_Chin_Hoz:      return T_PrivPtr(m_obj)->hozChinMotorRef().cmdCntrRef();
    case DevCtl::MotorId_Chin_Vert:     return T_PrivPtr(m_obj)->vertChinMotorRef().cmdCntrRef();
    default: return false;
    }
}

qint32   StatusData :: motorPosition( DevCtl::MotorId mot_id ) const
{
    if ( m_obj == nullptr ) { return false; }
    switch( mot_id ) {
    case DevCtl::MotorId_X :            return T_PrivPtr( m_obj )->xMotorRef().positionRef();
    case DevCtl::MotorId_Y :            return T_PrivPtr( m_obj )->yMotorRef().positionRef();
    case DevCtl::MotorId_Focus :        return T_PrivPtr( m_obj )->focusMotorRef().positionRef();
    case DevCtl::MotorId_Color:         return T_PrivPtr(m_obj)->colorMotorRef().positionRef();
    case DevCtl::MotorId_Light_Spot:    return T_PrivPtr(m_obj)->lightSpotMotorRef().positionRef();
    case DevCtl::MotorId_Shutter:       return T_PrivPtr(m_obj)->shutterMotorRef().positionRef();
    case DevCtl::MotorId_Chin_Hoz:      return T_PrivPtr(m_obj)->hozChinMotorRef().positionRef();
    case DevCtl::MotorId_Chin_Vert:     return T_PrivPtr(m_obj)->vertChinMotorRef().positionRef();
    default: return false;
    }
}

bool   StatusData :: cacheNormalFlag( ) const
{ return ( m_obj != nullptr ? T_PrivPtr( m_obj )->CacheNormalFlagRef() : 0); }

bool   StatusData :: cacheMoveFlag( ) const
{ return ( m_obj != nullptr ? T_PrivPtr( m_obj )->CacheMoveFlagRef() : 0 ); }

bool     StatusData :: answerpadStatus() const
{ return ( m_obj != nullptr ? T_PrivPtr( m_obj )->AnswerPadStatusRef() : false );}

bool     StatusData :: cameraStatus() const
{ return ( m_obj != nullptr ? T_PrivPtr( m_obj )->CameraStatusRef() : false ); }

bool StatusData::eyeglassStatus() const
{ return ( m_obj != nullptr ? T_PrivPtr( m_obj )->EyeglassStatusRef() : false ); }

qint32     StatusData :: envLightDA() const
{ return ( m_obj != nullptr ? T_PrivPtr( m_obj )->envLightDARef() : false ); }

qint32     StatusData :: castLightDA() const
{ return ( m_obj != nullptr ? T_PrivPtr( m_obj )->castLightDARef() : false ); }

}

#endif
