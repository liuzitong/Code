#ifndef USBDEV_DEVCTL_HXX
#define USBDEV_DEVCTL_HXX

#include "usbdev/common/usbdev_def.h"
#include <QObject>
#include <QByteArray>

#include <QFile>
#include <QDebug>
namespace UsbDev {

class Profile;
class StatusData;
class FrameData;
class Config;
class StaticCache;
class MoveCache;


// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief the device controller
 * @details this class will do communication between host and usb device. \n
 *   the class inner used a thread, so NEED NOT to put it into a thread.
 */
// ////////////////////////////////////////////////////////////////////////////
class USBDEV_API  DevCtl : public QObject {
    Q_OBJECT
public:
    static void hello(){qDebug()<<"hello from dev";}


    //! enumeric
    enum WorkStatus {
        WorkStatus_S_Disconnected = 0,     //!< device has been disconnect from device
        
        WorkStatus_S_ConnectToDev = 1,      //!< device has been connect to device
        WorkStatus_S_OK = 2,                //!< the device has been connected and all are OK
        WorkStatus_E_UnExpected   = 0x81,   //!< some error ocurred
    };

    //! lamp id
    enum LampId {
        LampId_centerFixation = 0,
        LampId_bigDiamond,
        LampId_smallDiamond,
        LampId_yellowBackground,
        LampId_centerInfrared,
        LampId_borderInfrared,
        LampId_eyeglassInfrared,
        LampId_castLight
    };

    //! the motor ID
    enum MotorId {
        MotorId_X = 1,
        MotorId_Y = 2,
        MotorId_Focus = 3,
        MotorId_Color = 4,
        MotorId_Light_Spot = 5,
        MotorId_Shutter = 6,
        MotorId_Chin_Hoz = 7,
        MotorId_Chin_Vert = 8
    };

    //! sample motor control stage
    enum  SampleMotorCtl_Stage {
        SampleMotorCtl_Stage_Stop = 0,     //!< same as moveMotor( 0 )
        SampleMotorCtl_Stage_Prepare = 1,
        SampleMotorCtl_Stage_Work = 2
    };

    enum MoveMethod{
        Relative=0,
        Abosolute=1
    };


    //! dtor
    virtual ~DevCtl( ) Q_DECL_OVERRIDE;

    //! initialize, call this to do init.
    //! @note this function will drop old context and retry connect to device
    void   reinit( );

    //! return the work status
    WorkStatus  workStatus( ) const;

    /*!
     * \brief control the lamp on off
     * \param lamp   [in] the Lamp Id pre-defined
     * \param level  [in] light level, range is 0 to 255, rsvd
     * \param on_off [in] true means lamp turn on.
     */
    void  setLamp( LampId lampId,quint8 lampNumber, quint16 da);

    void  setWhiteLamp(quint8 r,quint8 g,quint8 b);

    void  openShutter(quint16 durationTime,qint32 coord_shutter);

    void  beep(quint16 repeatCount,quint16 durationTime,quint16 intervalTime);

    void  moveChinMotors( quint8* sps, qint32* value,MoveMethod method=MoveMethod::Abosolute);

    /*!
     * \brief move spec. Motor
     * \param mot      [in]  the motor id pre-defined
     * \param sps      [in]  speed pre seconds. How many motor steps moved in 1 seconds.
     * \param dist     [in]  relative distance.
     * \param acc_flag [in]  a flag used to means use acceleration or not.
     *                       0x01 means use acceleration
     * \note  'sps' parameter can be use 0 or < 0, it will automatically use \n
     *    default speed.
     * \note  while 'acc_flag' is 0, 'sps' should be [ 0, 400 ]
     *        while 'acc_flga' is 1, 'sps' should be [ 0, 800 ]
     *
     */
    void  move5Motors( quint8*  sps, qint32*  value,MoveMethod method=MoveMethod::Abosolute);

    void sendDynamicData(quint8 totalFrame,quint8 frameNumber,quint32 dataLen,qint32* posData);

    void startDynamic(quint8 spsX,quint8 spsY,quint8 spsF,quint32 stepTime,quint32 totalStepCount);

    void stopDyanmic();
    /*!
     * \brief reset Motor, the motor moved to reset position
     * \param mot      [in]  the motor id pre-defined
     * \param sps      [in]  speed pre seconds. How many motor steps moved in 1 seconds.
     * \param pos      [in]  absolute position
     * \param acc_flag [in]  a flag used to means use acceleration or not.
     *                       0x01 means use acceleration
     * \note  'sps' parameter can be use 0 or < 0, it will automatically use \n
     *    default speed.
     */
    void  resetMotor( MotorId mot,quint8 speed);

    /*!
     * \brief save the motor configuration data
     * \param mot [in] the motor id pre-defined
     * \param cfg [in] the configruation data, user defined.
     * \note  the cfg data is a raw binary stream, but the usb board maybe do not \n
     *   save all data. Maximum data size is 500 bytes.
     */
    void  saveConfig(Config &cfg );

    /*!
     * \brief contol the sample motor
     * \param stage [in] the sample motor moving stage.
     * \param sps   [in] steps per seconds
     * \param acc_flag [in] 0x01 means use acceleration
     * \note  the 'stage' has 3 value \n
     *    'Stop'    -- this is same as call motorMove() with distance 0
     *    'Prepare' -- make the sample motor moving to prepare position. while it moving \n
     *                 the busy flag in StatusData will be set to 1.
     *    'work'    -- make the sample motor working. while it moving, the busy flag is true
     */
    void  sampleMotorCtl( SampleMotorCtl_Stage stage, qint32 sps, quint8 acc_flag );


    /*!
     * \brief setFrontVideo
     * \param on_off [in] true means on, false means off
     * \note  while turn on the front video, the old framedata will \n
     *   be cleared automatically. front video is a double video that owned \n
     *   2 images.
     */
    void  setFrontVideo( bool on_off );

    StaticCache* readStaticCache() const;

    MoveCache* readMoveCache() const;

    void readConfig();

    void sendBinaryCommand(QByteArray ba,int dataLen);



    /*!
     * @brief get the current profile of the device
     * @note while device workstatus is OK, user can take it
     */
    Profile  profile( ) const;



    /*!
     * @brief get the current config of the device
     * @note while device workstatus is OK, user can take it
     */
    Config  config( ) const;

    /*!
     * \brief take the pending status data
     * \return non empty object returned if succ.
     */
    StatusData  takeNextPendingStatusData( );

    /*!
     * \brief take the pending frame data
     * \return non empty object returned if succ.
     */
    FrameData   takeNextPendingFrameData( );



    /*!
     * @brief this signal emited while work status changed.
     */
    Q_SIGNAL  void  workStatusChanged( int );

    /*!
     * @brief while a new status data generated, this signals emited
     * @note user should call takeNextPendingStatus() to get status data
     */
    Q_SIGNAL  void  newStatusData( );

    /*!
     * @brief while a new frame data generated, this signals emited
     * @note user should call takeNextPendingFrameData() to get frame data
     */
    Q_SIGNAL  void  newFrameData( );

    /*!
     * @brief while the DevCtl loaded profile from hardware, it will be emited.
     */
    Q_SIGNAL  void  newProfile( );

    Q_SIGNAL  void  newConfig( );

    Q_SIGNAL  void  updateInfo(QString);

    Q_SIGNAL  void updateIOInfo(QString);

    Q_SIGNAL void updateRefreshInfo(QString);

    Q_SIGNAL void updateRefreshIOInfo(QString);


    static DevCtl*  createInstance( quint32 vid_pid, quint32 cfg_id = 1 );

    //! @brief create the instance ( wait version )
    //!
    //! @since 0.2.1
    //! @param vid_pid [in] the specified vid pid.
    //! @param cfg_id  [in] optional configuration id. current is 0x01
    //! @note  the default VID_PID is pre-defined by user, current is \n
    //!    0xffffa60e
    static  DevCtl*  createInstanceSync( quint32 vid_pid, quint32 cfg_id = 1 );



     //! \since 0.5
    //! \param buff_ptr    [in] the data want to write
    //! \param eeprom_addr [in] eeprom address
    //! \param fk [in] factory key, should be ''
    bool   readUsbEEPROM (       char *buff_ptr, int size, int eeprom_addr );

    //! \since 0.5
    bool   writeUsbEEPROM( const char *buff_ptr, int size, int eeprom_addr );



protected:
    //! ctor
    explicit DevCtl( quint32 vid_pid, quint32 cfg_id );

    DevCtl( quint32 vid_pid, quint32 cfg_id, bool );

private:
    void *m_obj;
    Q_DISABLE_COPY( DevCtl )
    friend class DevCtlPriv;

};
}
#endif
