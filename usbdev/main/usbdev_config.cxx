#ifndef USBDEV_CONFIG_CXX
#define USBDEV_CONFIG_CXX

#include <QString>
#include "usbdev_config.hxx"
#include "usbdev/common/usbdev_pimplprivtemp.hpp"
#include <QMetaType>
#include <QStringList>

namespace UsbDev {

// ////////////////////////////////////////////////////////////////////////////
// register in Qt Meta system
// ////////////////////////////////////////////////////////////////////////////
static bool g_is_reg = false;
static void gRegInQt ( )
{
    if ( ! g_is_reg ) {
        qRegisterMetaType<UsbDev::Config>();
        g_is_reg = true;
    }
}

// ////////////////////////////////////////////////////////////////////////////
// private object
// ////////////////////////////////////////////////////////////////////////////
#define T_PrivPtr( o )  usbdev_objcast( ConfigPriv*, o )
class USBDEV_HIDDEN ConfigPriv : public PImplPrivTemp<ConfigPriv > {
private:
    quint32                             m_crc_veryfication,
                                        m_device_ID;
    qint16                              m_center_fixation_lamp_DA,
                                        m_big_diamond_fixation_lamp_DA[4],
                                        m_small_diamond_fixation_lamp_DA[4],
                                        m_yellow_background_lamp_DA,
                                        m_white_background_lamp_DA[3],
                                        m_center_infrared_lamp_DA,
                                        m_border_infrared_lamp_DA,
                                        m_eyeglass_frame_lamp_DA,
                                        m_environment_light_alarm_DA[2],
                                        m_pupil_grey_threshold_DA[2];
    qint32                              m_switch_color_motor_Pos[5],
                                        m_switch_light_spot_Pos[6],
                                        m_focal_motor_pos_for_sport_and_color_change,                         //焦距联动位置
                                        m_shutter_open_Pos,
                                        m_main_table_center_x_correction,
                                        m_main_table_center_y_correction,
                                        m_secondary_table_center_x_correction,
                                        m_secondary_table_center_y_correction,
                                        m_focal_motor_pos_correction,
                                        m_maximun_projection_light_AD_preset,
                                        m_x_motor_Pos_for_light_correction,
                                        m_y_motor_Pos_for_light_correction,
                                        m_focal_motor_pos_for_light_correction,
//                                        m_x_motor_Pos_for_diamond_center_test,
//                                        m_y_motor_Pos_for_diamond_center_test,
//                                        m_focal_motor_Pos_for_diamond_center_test[6],                     //6光斑
//                                        m_focal_motor_Pos_mapping[25][6],                                 //25距离,6光斑
                                        m_Db_Pos_mapping[52][2];                                              //0~51DB,颜色电机,光斑电机
//                                        m_step_time[7];                                                       //7种速度对应的每步时间
//   float                                m_step_length;                                                        //步长以角度为单位

public :
    ConfigPriv ( );
    ConfigPriv ( const ConfigPriv & );
    virtual ~ConfigPriv( ) Q_DECL_OVERRIDE;
    inline auto crcVeryficationRef( )                               -> quint32&     {return m_crc_veryfication;}
    inline auto deviceIDRef( )                                      -> quint32&     {return m_device_ID;}
    inline auto centerFixationLampDARef()                           -> qint16&     {return m_center_fixation_lamp_DA;}
    inline auto bigDiamondfixationLampDAPtr()                       -> qint16*     {return m_big_diamond_fixation_lamp_DA;}
    inline auto smallDiamondFixationLampDAPtr()                     -> qint16*     {return m_small_diamond_fixation_lamp_DA;}
    inline auto yellowBackgroundLampDARef()                         -> qint16&     {return m_yellow_background_lamp_DA;}
    inline auto whiteBackgroundLampDAPtr()                          -> qint16*     {return m_white_background_lamp_DA;}
    inline auto centerInfraredLampDARef()                           -> qint16&     {return m_center_fixation_lamp_DA;}
    inline auto borderInfraredLampDARef()                           -> qint16&     {return m_border_infrared_lamp_DA;}
    inline auto eyeglassFrameLampDARef()                            -> qint16&     {return m_eyeglass_frame_lamp_DA;}
    inline auto environmentAlarmLightDAPtr()                        -> qint16*     {return m_environment_light_alarm_DA;}
    inline auto pupilGreyThresholdDAPtr()                           -> qint16*     {return m_pupil_grey_threshold_DA;}
    inline auto switchColorMotorPosPtr()                            -> int*         {return m_switch_color_motor_Pos;}
    inline auto switchLightSpotMotorPosPtr()                        -> int*         {return m_switch_light_spot_Pos;}
    inline auto focusPosForSpotAndColorChangeRef()                  -> qint32&      {return m_focal_motor_pos_for_sport_and_color_change;}
    inline auto shutterOpenPosRef()                                 -> qint32&      {return m_shutter_open_Pos;}
    inline auto mainTableCenterXCorrectionRef()                     -> qint32&      {return m_main_table_center_x_correction;}
    inline auto mainTableCenterYCorrectionRef()                     -> qint32&      {return m_main_table_center_y_correction;}
    inline auto secondaryTableCenterXCorrectionRef()                -> qint32&      {return m_secondary_table_center_x_correction;}
    inline auto secondaryTableCenterYCorrectionRef()                -> qint32&      {return m_secondary_table_center_y_correction;}
    inline auto focalPosCorrectionRef()                             -> qint32&      {return m_focal_motor_pos_correction;}
    inline auto maximunProjectionLightADPresetRef()                 -> qint32&      {return m_maximun_projection_light_AD_preset;}
    inline auto xMotorPosForLightCorrectionRef()                    -> qint32&      {return m_x_motor_Pos_for_light_correction;}
    inline auto yMotorPosForLightCorrectionRef()                    -> qint32&      {return m_y_motor_Pos_for_light_correction;}
    inline auto focalLengthMotorPosForLightCorrectionRef()          -> qint32&      {return m_focal_motor_pos_for_light_correction;}
//    inline auto xMotorPosForDiamondCenterTestRef()                  -> qint32&      {return m_x_motor_Pos_for_diamond_center_test;}
//    inline auto yMotorPosForDiamondCenterTestRef()                  -> qint32&      {return m_y_motor_Pos_for_diamond_center_test;}
//    inline auto focalLengthMotorPosForDiamondCenterTestPtr()        -> int*         {return m_focal_motor_Pos_for_diamond_center_test;}
//    inline auto focalLengthMotorPosMappingPtr()                     -> int(*)[6]    {return m_focal_motor_Pos_mapping;}
    inline auto DbPosMappingPtr()                                   -> int(*)[2]    {return m_Db_Pos_mapping;}
//    inline auto stepTimePtr()                                       -> int*         {return m_step_time;}
//    inline auto stepLengthRef()                                     -> float&       {return m_step_length;}
};

ConfigPriv :: ~ConfigPriv ( ) { }

// ============================================================================
// ctor
// ============================================================================
ConfigPriv :: ConfigPriv ( )
{
    m_crc_veryfication                                            = 0xffffffff;
    m_device_ID                                                   = 0xffffffff;
    m_center_fixation_lamp_DA                                     = 0;
    memset(m_big_diamond_fixation_lamp_DA,0,sizeof (m_big_diamond_fixation_lamp_DA));
    memset(m_small_diamond_fixation_lamp_DA,0,sizeof (m_small_diamond_fixation_lamp_DA));
    m_yellow_background_lamp_DA                                   = 0;
    memset(m_white_background_lamp_DA,0,sizeof (m_white_background_lamp_DA));
    m_center_infrared_lamp_DA                                     = 0;
    m_border_infrared_lamp_DA                                     = 0;
    m_eyeglass_frame_lamp_DA                                      = 0;
    memset(m_environment_light_alarm_DA,0,sizeof (m_environment_light_alarm_DA));
    memset(m_pupil_grey_threshold_DA,0,sizeof (m_pupil_grey_threshold_DA));
    memset(m_switch_color_motor_Pos,0,sizeof (m_switch_color_motor_Pos));
    memset(m_switch_light_spot_Pos,0,sizeof (m_switch_light_spot_Pos));
    m_focal_motor_pos_for_sport_and_color_change                   = 0;
    m_shutter_open_Pos                                            = 0;
    m_main_table_center_x_correction                              = 0;
    m_main_table_center_y_correction                              = 0;
    m_secondary_table_center_x_correction                         = 0;
    m_secondary_table_center_y_correction                         = 0;
    m_focal_motor_pos_correction                                  = 0;
    m_maximun_projection_light_AD_preset                          = 0;
    m_x_motor_Pos_for_light_correction                          = 0;
    m_y_motor_Pos_for_light_correction                          = 0;
    m_focal_motor_pos_for_light_correction               = 0;

//    m_x_motor_Pos_for_diamond_center_test                       = 0;
//    m_y_motor_Pos_for_diamond_center_test                       = 0;
//    memset(m_focal_motor_Pos_for_diamond_center_test,0,sizeof (m_focal_motor_Pos_for_diamond_center_test));
//    memset(m_focal_motor_Pos_mapping,0,sizeof (m_focal_motor_Pos_mapping));
    memset(m_Db_Pos_mapping,0,sizeof (m_Db_Pos_mapping));
//    memset(m_step_time,0,sizeof (m_step_time));
//    m_step_length                                               =0;
}

// ============================================================================
// ctor( copy )
// ============================================================================
ConfigPriv :: ConfigPriv ( const ConfigPriv &o )
{
    m_crc_veryfication                                            =o.m_crc_veryfication                      ;
    m_device_ID                                                   =o.m_device_ID                             ;
    m_center_fixation_lamp_DA                                     =o.m_center_fixation_lamp_DA               ;
    memcpy(m_big_diamond_fixation_lamp_DA,o.m_big_diamond_fixation_lamp_DA,sizeof (m_big_diamond_fixation_lamp_DA));
    memcpy(m_small_diamond_fixation_lamp_DA,o.m_small_diamond_fixation_lamp_DA,sizeof (m_small_diamond_fixation_lamp_DA));
    m_yellow_background_lamp_DA                                   =o.m_yellow_background_lamp_DA             ;
    memcpy(m_white_background_lamp_DA,o.m_white_background_lamp_DA,sizeof (m_white_background_lamp_DA));
    m_center_infrared_lamp_DA                                     =o.m_center_infrared_lamp_DA               ;
    m_border_infrared_lamp_DA                                     =o.m_border_infrared_lamp_DA               ;
    m_eyeglass_frame_lamp_DA                                      =o.m_eyeglass_frame_lamp_DA                ;
    memcpy(m_environment_light_alarm_DA,o.m_environment_light_alarm_DA,sizeof (m_environment_light_alarm_DA));
    memcpy(m_pupil_grey_threshold_DA,o.m_pupil_grey_threshold_DA,sizeof (m_pupil_grey_threshold_DA));
    memcpy(m_switch_color_motor_Pos,o.m_switch_color_motor_Pos,sizeof (m_switch_color_motor_Pos));
    memcpy(m_switch_light_spot_Pos,o.m_switch_light_spot_Pos,sizeof (m_switch_light_spot_Pos));
    m_focal_motor_pos_for_sport_and_color_change                      =o.m_focal_motor_pos_for_sport_and_color_change;
    m_shutter_open_Pos                                            =o.m_shutter_open_Pos                      ;
    m_main_table_center_x_correction                              =o.m_main_table_center_x_correction        ;
    m_main_table_center_y_correction                              =o.m_main_table_center_y_correction        ;
    m_secondary_table_center_x_correction                         =o.m_secondary_table_center_x_correction   ;
    m_secondary_table_center_y_correction                         =o.m_secondary_table_center_y_correction   ;
    m_focal_motor_pos_correction                                  =o.m_focal_motor_pos_correction;
    m_maximun_projection_light_AD_preset                          =o.m_maximun_projection_light_AD_preset    ;
    m_x_motor_Pos_for_light_correction                          =o.m_x_motor_Pos_for_light_correction    ;
    m_y_motor_Pos_for_light_correction                          =o.m_y_motor_Pos_for_light_correction    ;
    m_focal_motor_pos_for_light_correction               =o.m_focal_motor_pos_for_light_correction;
//    m_x_motor_Pos_for_diamond_center_test                       =o.m_x_motor_Pos_for_diamond_center_test ;
//    m_y_motor_Pos_for_diamond_center_test                       =o.m_y_motor_Pos_for_diamond_center_test ;
//    memcpy(m_focal_motor_Pos_for_diamond_center_test,o.m_focal_motor_Pos_for_diamond_center_test,sizeof (m_focal_motor_Pos_for_diamond_center_test));
//    memcpy(m_focal_motor_Pos_mapping,o.m_focal_motor_Pos_mapping,sizeof (m_focal_motor_Pos_mapping));
    memcpy(m_Db_Pos_mapping,o.m_Db_Pos_mapping,sizeof (m_Db_Pos_mapping));
//    memcpy(m_step_time,o.m_step_time,sizeof (m_step_time));
//    m_step_length                                               =o.m_step_length;
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
Config :: Config ( )
{
    gRegInQt();
    ConfigPriv::buildIfNull( & m_obj );
    memset(dataPtr(),-1,dataLen());
}

// ============================================================================
//  ctor ( copy  )
// ============================================================================
Config :: Config ( const Config &o )
{
    gRegInQt();  m_obj = nullptr;
    ConfigPriv::attach( &m_obj, const_cast<void**>( & o.m_obj));
}

// ============================================================================
//  assign
// ============================================================================
Config &  Config :: operator=( const Config &o )
{
    gRegInQt();
    ConfigPriv::attach( &m_obj, const_cast<void**>( & o.m_obj));
    return *this;
}

// ============================================================================
// dtor
// ============================================================================
Config :: ~Config ( )
{
    if ( m_obj != nullptr ) {
        ConfigPriv::attach( &m_obj, nullptr );
    }
}

// ============================================================================
// ctor ( consturct by data )
// ============================================================================
Config :: Config ( const QByteArray &ba )
{
    gRegInQt(); m_obj = nullptr;
    if ( ba.size() < 512 ) { return; }

    // ------------------------------------------------------------------------
    // build data
    // ------------------------------------------------------------------------
    void *d = nullptr;
    const unsigned char *buff = reinterpret_cast<const unsigned char*>( ba.constData());
    ConfigPriv::buildIfNull( & d );
    auto *priv = T_PrivPtr( d );
    priv->crcVeryficationRef()=gReadData_Le_I32(&buff[0]);
    priv->deviceIDRef()=gReadData_Le_I32(&buff[4]);
    priv->centerFixationLampDARef()=gReadData_Le_I16(&buff[8]);
    memcpy(priv->bigDiamondfixationLampDAPtr(),&buff[10],8);
    memcpy(priv->smallDiamondFixationLampDAPtr(),&buff[18],8);
    priv->yellowBackgroundLampDARef()=gReadData_Le_I16(&buff[26]);
    memcpy(priv->whiteBackgroundLampDAPtr(),&buff[28],6);
    priv->centerInfraredLampDARef()=gReadData_Le_I16(&buff[34]);
    priv->borderInfraredLampDARef()=gReadData_Le_I16(&buff[36]);
    priv->eyeglassFrameLampDARef()=gReadData_Le_I16(&buff[38]);
    memcpy(priv->environmentAlarmLightDAPtr(),&buff[40],4);
    memcpy(priv->pupilGreyThresholdDAPtr(),&buff[44],4);
    memcpy(priv->switchColorMotorPosPtr(),&buff[48],20);
    memcpy(priv->switchLightSpotMotorPosPtr(),&buff[68],24);
    priv->focusPosForSpotAndColorChangeRef()=gReadData_Le_I32(&buff[92]);
    priv->shutterOpenPosRef()=gReadData_Le_I32(&buff[96]);
    priv->mainTableCenterXCorrectionRef()=gReadData_Le_I32(&buff[100]);
    priv->mainTableCenterYCorrectionRef()=gReadData_Le_I32(&buff[104]);
    priv->secondaryTableCenterXCorrectionRef()=gReadData_Le_I32(&buff[108]);
    priv->secondaryTableCenterYCorrectionRef()=gReadData_Le_I32(&buff[112]);
    priv->maximunProjectionLightADPresetRef()=gReadData_Le_I32(&buff[116]);
    priv->xMotorPosForLightCorrectionRef()=gReadData_Le_I32(&buff[120]);
    priv->yMotorPosForLightCorrectionRef()=gReadData_Le_I32(&buff[124]);
    priv->focalLengthMotorPosForLightCorrectionRef()=gReadData_Le_I32(&buff[128]);
//    priv->xMotorPosForDiamondCenterTestRef()=gReadData_Le_I32(&buff[144]);
//    priv->yMotorPosForDiamondCenterTestRef()=gReadData_Le_I32(&buff[148]);
//    memcpy( priv->focalLengthMotorPosForDiamondCenterTestPtr(),&buff[152],28);
//    memcpy( priv->focalLengthMotorPosMappingPtr(),&buff[132],600);
    memcpy (priv->DbPosMappingPtr(),&buff[132],416);
//    memcpy(priv->stepTimePtr(),&buff[1296],28);
//    priv->stepLengthRef()=gReadData_Le_I32(&buff[1324]);
    m_obj = d;
}

// ============================================================================
// method access
// ============================================================================

bool       Config :: isEmpty() const { return ( m_obj == nullptr ); }

quint32&       Config :: crcVeryficationRef()
{ return T_PrivPtr( m_obj )->crcVeryficationRef(); }

quint32&       Config :: deviceIDRef()
{ return T_PrivPtr( m_obj )->deviceIDRef(); }

qint16&       Config :: centerFixationLampDARef()
{ return  T_PrivPtr( m_obj )->centerFixationLampDARef(); }

qint16*       Config :: bigDiamondfixationLampDAPtr()
{ return T_PrivPtr( m_obj )->bigDiamondfixationLampDAPtr(); }

qint16*       Config :: smallDiamondFixationLampDAPtr()
{ return T_PrivPtr( m_obj )->smallDiamondFixationLampDAPtr(); }

qint16&       Config :: yellowBackgroundLampDARef()
{ return  T_PrivPtr( m_obj )->yellowBackgroundLampDARef(); }

qint16*          Config :: whiteBackgroundLampDAPtr()
{ return T_PrivPtr( m_obj )->whiteBackgroundLampDAPtr(); }

qint16&       Config :: centerInfraredLampDARef()
{ return  T_PrivPtr( m_obj )->centerInfraredLampDARef(); }

qint16&       Config :: borderInfraredLampDARef()
{ return T_PrivPtr( m_obj )->borderInfraredLampDARef(); }

qint16&       Config :: eyeglassFrameLampDARef()
{ return  T_PrivPtr( m_obj )->eyeglassFrameLampDARef(); }

qint16*          Config :: environmentAlarmLightDAPtr()
{ return T_PrivPtr( m_obj )->environmentAlarmLightDAPtr(); }

qint16*        Config::pupilGreyThresholdDAPtr()
{ return T_PrivPtr( m_obj )->pupilGreyThresholdDAPtr(); }

int*          Config :: switchColorMotorPosPtr()
{ return  T_PrivPtr( m_obj )->switchColorMotorPosPtr(); }

int*          Config :: switchLightSpotMotorPosPtr()
{ return  T_PrivPtr( m_obj )->switchLightSpotMotorPosPtr(); }

qint32&       Config :: focusPosForSpotAndColorChangeRef()
{ return T_PrivPtr( m_obj )->focusPosForSpotAndColorChangeRef(); }

qint32&       Config :: shutterOpenPosRef()
{ return T_PrivPtr( m_obj )->shutterOpenPosRef(); }

qint32&       Config :: mainTableCenterXRef()
{ return T_PrivPtr( m_obj )->mainTableCenterXCorrectionRef(); }

qint32&       Config :: mainTableCenterYRef()
{ return T_PrivPtr( m_obj )->mainTableCenterYCorrectionRef(); }

qint32&       Config :: secondaryTableCenterXRef()
{ return T_PrivPtr( m_obj )->secondaryTableCenterXCorrectionRef(); }

qint32&       Config :: secondaryTableCenterYRef()
{ return T_PrivPtr( m_obj )->secondaryTableCenterYCorrectionRef(); }

qint32 &Config::focalMotorPosCorrectionRef()
{  return T_PrivPtr( m_obj )->focalPosCorrectionRef();}

qint32&       Config :: castLightADPresetRef()
{ return T_PrivPtr( m_obj )->maximunProjectionLightADPresetRef(); }

qint32&       Config :: focalLengthMotorPosForLightCorrectionRef()
{ return T_PrivPtr( m_obj )->focalLengthMotorPosForLightCorrectionRef(); }


qint32&       Config :: xMotorPosForLightCorrectionRef()
{ return T_PrivPtr( m_obj )->xMotorPosForLightCorrectionRef(); }

qint32&       Config :: yMotorPosForLightCorrectionRef()
{ return T_PrivPtr( m_obj )->yMotorPosForLightCorrectionRef(); }

//int&       Config :: xMotorPosForDiamondCenterTestRef()
//{ return T_PrivPtr( m_obj )->xMotorPosForDiamondCenterTestRef(); }

//int&       Config :: yMotorPosForDiamondCenterTestRef()
//{ return T_PrivPtr( m_obj )->yMotorPosForDiamondCenterTestRef(); }

//int*       Config :: focalLengthMotorPosForDiamondCenterTestPtr()
//{ return T_PrivPtr( m_obj )->focalLengthMotorPosForDiamondCenterTestPtr(); }

//int(*       Config :: focalLengthMotorPosMappingPtr())[6]
//{ return T_PrivPtr( m_obj )->focalLengthMotorPosMappingPtr(); }

int(*       Config :: DbPosMappingPtr())[2]
{ return T_PrivPtr( m_obj )->DbPosMappingPtr(); }

//int*  Config :: DbAngleDampingPtr()
//{ return T_PrivPtr( m_obj )->dbAngleDampingPtr(); }

//int *Config::stepTimePtr()
//{return T_PrivPtr( m_obj )->stepTimePtr(); }

//float& Config::stepLengthRef()
//{return T_PrivPtr( m_obj )->stepLengthRef(); }

void *Config::dataPtr()
{return (void*)&(T_PrivPtr( m_obj )->crcVeryficationRef());}

int Config::dataLen()
{
    return 552;
//    return sizeof(ConfigPriv); 继承了的东西有字段,不能用这种方法
}



//void* Config::GetData()
//{ return  (void*)&(T_PrivPtr( m_obj )->crcVeryficationRef()); }
// ============================================================================
// make all profile as json object
// ============================================================================
//QJsonObject  Config :: toJsonObject( ) const
//{
//    QJsonObject jo;
//    if ( isEmpty() ) { return jo; }

//    jo.insert( QStringLiteral("devVerStr"), T_PrivPtr( m_obj )->devVerRef() );
//    jo.insert( QStringLiteral("isFrontVideoCombined"), T_PrivPtr( m_obj )->isFrontVideoCombRef() );

//    { // front video width and height
//        QJsonObject jo2; QSize sz = T_PrivPtr( m_obj )->frontVideoSizeRef();
//        jo2.insert( QStringLiteral("width"),  sz.width() );
//        jo2.insert( QStringLiteral("height"), sz.height());
//        jo.insert( QStringLiteral("frontVideoSize"), jo2 );
//    }

//    jo.insert( QStringLiteral("frontVideoRowPitch"), int( T_PrivPtr( m_obj )->frontVideoRowPitchRef() ));

//    { // make all motor range value
//        QStringList mot_list; ConfigPriv *pp = T_PrivPtr( m_obj );
//        mot_list << "xMotorRange"    << "yMotorRange" << "zMotorRange" << "focusMotorRange"
//                 << "chinMotorRange" << "spectraMotorRange" << "ligthMotorRange" << "sampleMotorRange";
//        QList<QPair<qint32,qint32>> rg_list;
//        rg_list << pp->xMotorRangeRef() << pp->yMotorRangeRef() << pp->zMotorRangeRef() << pp->focusMotorRangeRef()
//                << pp->chinMotorRangeRef() << pp->spectraMotorRangeRef() << pp->lightMotorRangeRef() << pp->sampleMotorRangeRef();
//        int sz = mot_list.size();
//        for ( int i = 0; i < sz; i ++ ) {
//            QJsonObject jo2;
//            jo2.insert( QStringLiteral("min"), rg_list.at(i).first  );
//            jo2.insert( QStringLiteral("max"), rg_list.at(i).second );
//            jo.insert( mot_list.at(i), jo2 );
//        }
//    }

//    {  // vibra mirror x ad
//        QJsonObject jo2;
//        jo2.insert( QStringLiteral("min"), T_PrivPtr( m_obj )->vibraMirrorXadRangeRef().first  );
//        jo2.insert( QStringLiteral("max"), T_PrivPtr( m_obj )->vibraMirrorXadRangeRef().second );
//        jo.insert( QStringLiteral("vibraMirrorXadRange"), jo2 );
//    }
//    {  // vibra mirror y ad
//        QJsonObject jo2;
//        jo2.insert( QStringLiteral("min"), T_PrivPtr( m_obj )->vibraMirrorYadRangeRef().first  );
//        jo2.insert( QStringLiteral("max"), T_PrivPtr( m_obj )->vibraMirrorYadRangeRef().second );
//        jo.insert( QStringLiteral("vibraMirrorYadRange"), jo2 );
//    }
//    {  // global accel param range
//        QJsonObject jo2;
//        jo2.insert( QStringLiteral("min"), T_PrivPtr( m_obj )->globalAccelParamRangeRef().first );
//        jo2.insert( QStringLiteral("max"), T_PrivPtr( m_obj )->globalAccelParamRangeRef().second);
//        jo.insert( QStringLiteral("globalAccelParamRange"), jo2 );
//    }

//    { // extend version information
//        jo.insert( QStringLiteral("hasExtVerInfo"), T_PrivPtr(m_obj)->hasExtVerInfoRef());
//        jo.insert( QStringLiteral("dblCamCode"),    T_PrivPtr(m_obj)->dblCamCodeRef());
//        jo.insert( QStringLiteral("intrCamCode"), T_PrivPtr(m_obj)->intrCamCodeRef());
//        jo.insert( QStringLiteral("d3PlatCode"),  T_PrivPtr(m_obj)->d3PlatCodeRef());
//        jo.insert( QStringLiteral("physKeyCnt"),  T_PrivPtr(m_obj)->physKeyCntRef());
//        jo.insert( QStringLiteral("hasPhysKey"),  hasPhysKey() );
//    }

//    return jo;
//}


}

#endif
