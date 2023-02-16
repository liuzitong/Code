#ifndef USBDEV_PROFILE_CXX
#define USBDEV_PROFILE_CXX

#include <QString>
#include "usbdev_profile.hxx"
#include "usbdev/common/usbdev_pimplprivtemp.hpp"
#include <QMetaType>
#include <QStringList>
#include <QDebug>

namespace UsbDev {

// ////////////////////////////////////////////////////////////////////////////
// register in Qt Meta system
// ////////////////////////////////////////////////////////////////////////////
static bool g_is_reg = false;
static void gRegInQt ( )
{
    if ( ! g_is_reg ) {
        qRegisterMetaType<UsbDev::Profile>(); g_is_reg = true;
    }
}

// ////////////////////////////////////////////////////////////////////////////
// private object
// ////////////////////////////////////////////////////////////////////////////
#define T_PrivPtr( o )  usbdev_objcast( ProfilePriv*, o )
class USBDEV_HIDDEN ProfilePriv : public PImplPrivTemp<ProfilePriv > {
private:
    quint32                             m_dev_type,
                                        m_dev_ver;
    QSize                               m_front_video_size;
    QPair<qint32,qint32>                m_x_motor_range,
                                        m_y_motor_range,
                                        m_focus_motor_range,
                                        m_color_motor_range,
                                        m_light_spot_motor_range,
                                        m_shutter_motor_range,
                                        m_x_chin_motor_range,
                                        m_y_chin_motor_range;

public :
    ProfilePriv ( );
    ProfilePriv ( const ProfilePriv & );
    virtual ~ProfilePriv( ) Q_DECL_OVERRIDE;
    inline auto devTypeRef( )              -> quint32&                 { return m_dev_type; }
    inline auto devVerRef( )               -> quint32&                 { return m_dev_ver; }
    inline auto VideoSizeRef()             -> QSize&                   { return m_front_video_size; }
    inline auto xMotorRangeRef()           -> QPair<qint32,qint32>&    { return m_x_motor_range; }
    inline auto yMotorRangeRef()           -> QPair<qint32,qint32>&    { return m_y_motor_range; }
    inline auto focusMotorRangeRef()       -> QPair<qint32,qint32>&    { return m_focus_motor_range; }
    inline auto colorMotorRangeRef()       -> QPair<qint32,qint32>&    { return m_color_motor_range; }
    inline auto lightSpotMotorRangeRef()   -> QPair<qint32,qint32>&    { return m_light_spot_motor_range; }
    inline auto shutterMoterRangeRef()     -> QPair<qint32,qint32>&    { return m_shutter_motor_range; }
    inline auto xChinMotorRangeRef()       -> QPair<qint32,qint32>&    { return m_x_chin_motor_range; }
    inline auto yChinMotorRangeRef()       -> QPair<qint32,qint32>&    { return m_y_chin_motor_range; }
};

ProfilePriv :: ~ProfilePriv ( ) { }

// ============================================================================
// ctor
// ============================================================================
ProfilePriv :: ProfilePriv ( )
{
    m_dev_type                                  = 0xffff;
    m_dev_ver                                   = 0xffff;
    m_front_video_size                          = {0,0};
    m_x_motor_range                             = {0,0};
    m_y_motor_range                             = {0,0};
    m_focus_motor_range                         = {0,0};
    m_color_motor_range                         = {0,0};
    m_light_spot_motor_range                    = {0,0};
    m_shutter_motor_range                       = {0,0};
    m_x_chin_motor_range                        = {0,0};
    m_y_chin_motor_range                        = {0,0};

}

// ============================================================================
// ctor( copy )
// ============================================================================
ProfilePriv :: ProfilePriv ( const ProfilePriv &o )
{
    m_dev_type                  =o.m_dev_type               ;
    m_dev_ver                   =o.m_dev_ver                ;
    m_front_video_size          =o.m_front_video_size       ;
    m_x_motor_range             =o.m_x_motor_range          ;
    m_y_motor_range             =o.m_y_motor_range          ;
    m_focus_motor_range         =o.m_focus_motor_range      ;
    m_color_motor_range         =o.m_color_motor_range      ;
    m_light_spot_motor_range    =o.m_light_spot_motor_range ;
    m_shutter_motor_range       =o.m_shutter_motor_range    ;
    m_x_chin_motor_range        =o.m_x_chin_motor_range     ;
    m_y_chin_motor_range        =o.m_y_chin_motor_range     ;
}

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
//  ctor ( empty )
// ============================================================================
Profile :: Profile ( )
{ gRegInQt(); m_obj = nullptr; }

// ============================================================================
//  ctor ( copy  )
// ============================================================================
Profile :: Profile ( const Profile &o )
{
    gRegInQt();  m_obj = nullptr;
    ProfilePriv::attach( &m_obj, const_cast<void**>( & o.m_obj));
}

// ============================================================================
//  assign
// ============================================================================
Profile &  Profile :: operator=( const Profile &o )
{
    gRegInQt();
    ProfilePriv::attach( &m_obj, const_cast<void**>( & o.m_obj));
    return *this;
}

// ============================================================================
// dtor
// ============================================================================
Profile :: ~Profile ( )
{
    if ( m_obj != nullptr ) {
        ProfilePriv::attach( &m_obj, nullptr );
    }
}

// ============================================================================
// ctor ( consturct by data )
// ============================================================================
Profile :: Profile ( const QByteArray &ba )
{
    gRegInQt(); m_obj = nullptr;
    if ( ba.size() < 512 ) { return; }

    // ------------------------------------------------------------------------
    // build data
    // ------------------------------------------------------------------------
    void *d = nullptr;
    const unsigned char *buff = reinterpret_cast<const unsigned char*>( ba.constData());
    ProfilePriv::buildIfNull( & d );
    auto *priv = T_PrivPtr( d );
    priv->devTypeRef()                      =gReadData_Le_I16(&buff[0]);
    priv->devVerRef()                       =gReadData_Le_I16(&buff[2]);
    priv->VideoSizeRef()                    ={gReadData_Le_I16(&buff[4]),gReadData_Le_I16(&buff[6])};
    priv->xMotorRangeRef()                  ={gReadData_Le_I32(&buff[8]),gReadData_Le_I32(&buff[12])};
    priv->yMotorRangeRef()                  ={gReadData_Le_I32(&buff[16]),gReadData_Le_I32(&buff[20])};
    priv->focusMotorRangeRef()              ={gReadData_Le_I32(&buff[24]),gReadData_Le_I32(&buff[28])};
    priv->colorMotorRangeRef()              ={gReadData_Le_I32(&buff[32]),gReadData_Le_I32(&buff[36])};
    priv->lightSpotMotorRangeRef()          ={gReadData_Le_I32(&buff[40]),gReadData_Le_I32(&buff[44])};
    priv->shutterMoterRangeRef()            ={gReadData_Le_I32(&buff[48]),gReadData_Le_I32(&buff[52])};
    priv->xChinMotorRangeRef()              ={gReadData_Le_I32(&buff[56]),gReadData_Le_I32(&buff[60])};
    priv->yChinMotorRangeRef()              ={gReadData_Le_I32(&buff[64]),gReadData_Le_I32(&buff[68])};

//    QString str;
//    for(int i=0;i<72;i++)
//    {
//        str+=QString::number(i)+":"+QString::number(buff[i],16)+" ";
//    }
    m_obj = d;
}

// ============================================================================
// method access
// ============================================================================
bool       Profile :: isEmpty() const { return ( m_obj == nullptr ); }

quint16    Profile :: devType( ) const
{ return ( m_obj != nullptr ? T_PrivPtr( m_obj )->devVerRef() :0xffff ); }

quint16    Profile :: devVersion( ) const
{ return ( m_obj != nullptr ? T_PrivPtr( m_obj )->devVerRef() : 0xffff ); }

QSize      Profile :: videoSize( ) const
{ return ( m_obj != nullptr ? T_PrivPtr( m_obj )->VideoSizeRef() : QSize()); }

QPair<qint32,qint32>  Profile :: motorRange( DevCtl::MotorId mot_id ) const
{
    if ( m_obj == nullptr ) { return QPair<qint32,qint32>(0,0); }
    switch( mot_id ) {
    case DevCtl::MotorId_X:             return T_PrivPtr(m_obj)->xMotorRangeRef();
    case DevCtl::MotorId_Y:             return T_PrivPtr(m_obj)->yMotorRangeRef();
    case DevCtl::MotorId_Focus:         return T_PrivPtr(m_obj)->focusMotorRangeRef();
    case DevCtl::MotorId_Color:         return T_PrivPtr(m_obj)->colorMotorRangeRef();
    case DevCtl::MotorId_Light_Spot:    return T_PrivPtr(m_obj)->lightSpotMotorRangeRef();
    case DevCtl::MotorId_Shutter:       return T_PrivPtr(m_obj)->shutterMoterRangeRef();
    case DevCtl::MotorId_Chin_Hoz:        return T_PrivPtr(m_obj)->xChinMotorRangeRef();
    case DevCtl::MotorId_Chin_Vert:        return T_PrivPtr(m_obj)->yChinMotorRangeRef();
    default:return QPair<qint32,qint32>(0,0);
    }

}


// ============================================================================
// make all profile as json object
// ============================================================================
//QJsonObject  Profile :: toJsonObject( ) const
//{
////    QJsonObject jo;
////    if ( isEmpty() ) { return jo; }

////    jo.insert( QStringLiteral("devVerStr"), T_PrivPtr( m_obj )->devVerRef() );
////    jo.insert( QStringLiteral("isFrontVideoCombined"), T_PrivPtr( m_obj )->isFrontVideoCombRef() );

////    { // front video width and height
////        QJsonObject jo2; QSize sz = T_PrivPtr( m_obj )->frontVideoSizeRef();
////        jo2.insert( QStringLiteral("width"),  sz.width() );
////        jo2.insert( QStringLiteral("height"), sz.height());
////        jo.insert( QStringLiteral("frontVideoSize"), jo2 );
////    }

////    jo.insert( QStringLiteral("frontVideoRowPitch"), int( T_PrivPtr( m_obj )->frontVideoRowPitchRef() ));

////    { // make all motor range value
////        QStringList mot_list; ProfilePriv *pp = T_PrivPtr( m_obj );
////        mot_list << "xMotorRange"    << "yMotorRange" << "zMotorRange" << "focusMotorRange"
////                 << "chinMotorRange" << "spectraMotorRange" << "ligthMotorRange" << "sampleMotorRange";
////        QList<QPair<qint32,qint32>> rg_list;
////        rg_list << pp->xMotorRangeRef() << pp->yMotorRangeRef() << pp->zMotorRangeRef() << pp->focusMotorRangeRef()
////                << pp->chinMotorRangeRef() << pp->spectraMotorRangeRef() << pp->lightMotorRangeRef() << pp->sampleMotorRangeRef();
////        int sz = mot_list.size();
////        for ( int i = 0; i < sz; i ++ ) {
////            QJsonObject jo2;
////            jo2.insert( QStringLiteral("min"), rg_list.at(i).first  );
////            jo2.insert( QStringLiteral("max"), rg_list.at(i).second );
////            jo.insert( mot_list.at(i), jo2 );
////        }
////    }

////    {  // vibra mirror x ad
////        QJsonObject jo2;
////        jo2.insert( QStringLiteral("min"), T_PrivPtr( m_obj )->vibraMirrorXadRangeRef().first  );
////        jo2.insert( QStringLiteral("max"), T_PrivPtr( m_obj )->vibraMirrorXadRangeRef().second );
////        jo.insert( QStringLiteral("vibraMirrorXadRange"), jo2 );
////    }
////    {  // vibra mirror y ad
////        QJsonObject jo2;
////        jo2.insert( QStringLiteral("min"), T_PrivPtr( m_obj )->vibraMirrorYadRangeRef().first  );
////        jo2.insert( QStringLiteral("max"), T_PrivPtr( m_obj )->vibraMirrorYadRangeRef().second );
////        jo.insert( QStringLiteral("vibraMirrorYadRange"), jo2 );
////    }
////    {  // global accel param range
////        QJsonObject jo2;
////        jo2.insert( QStringLiteral("min"), T_PrivPtr( m_obj )->globalAccelParamRangeRef().first );
////        jo2.insert( QStringLiteral("max"), T_PrivPtr( m_obj )->globalAccelParamRangeRef().second);
////        jo.insert( QStringLiteral("globalAccelParamRange"), jo2 );
////    }

////    { // extend version information
////        jo.insert( QStringLiteral("hasExtVerInfo"), T_PrivPtr(m_obj)->hasExtVerInfoRef());
////        jo.insert( QStringLiteral("dblCamCode"),    T_PrivPtr(m_obj)->dblCamCodeRef());
////        jo.insert( QStringLiteral("intrCamCode"), T_PrivPtr(m_obj)->intrCamCodeRef());
////        jo.insert( QStringLiteral("d3PlatCode"),  T_PrivPtr(m_obj)->d3PlatCodeRef());
////        jo.insert( QStringLiteral("physKeyCnt"),  T_PrivPtr(m_obj)->physKeyCntRef());
////        jo.insert( QStringLiteral("hasPhysKey"),  hasPhysKey() );
////    }

////    return jo;
//}


}

#endif
