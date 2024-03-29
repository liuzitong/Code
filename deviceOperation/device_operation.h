#ifndef DEVICEOPERATION_H
#define DEVICEOPERATION_H

#include "device_operation_global.h"
#include <usbdev/main/usbdev_devctl.hxx>
#include <QSharedPointer>
#include <QString>
#include <QDebug>
#include <usbdev/main/usbdev_devctl.hxx>
#include <usbdev/main/usbdev_statusdata.hxx>
#include <usbdev/main/usbdev_framedata.hxx>
#include <usbdev/main/usbdev_profile.hxx>
#include <usbdev/main/usbdev_config.hxx>
#include <usbdev/main/usbdev_cache.hxx>
#include <QElapsedTimer>
#include <QTimer>
#include <QMutex>
#include <spdlog/spdlog.h>
#include "device_pupil_processor.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>

namespace DevOps{
using LampId=UsbDev::DevCtl::LampId;

struct Status
{
   int color;
   int spot;
//   int DB;
};

struct StatusDataOut
{
    int serialNo;
    bool answerpadStatus;
    bool cameraStatus;
    bool eyeglassStatus;
    bool xMotorBusy;
    bool yMotorBusy;
    bool focusMotorBusy;
    bool colorMotorBusy;
    bool lightSpotMotorBusy;
    bool shutterMotorBusy;
    bool xChinMotorBusy;
    bool yChinMotorBusy;
    int xMotorCmdCntr;
    int yMotorCmdCntr;
    int focusMotorCmdCntr;
    int colorMotorCmdCntr;
    int lightSpotMotorCmdCntr;
    int shutterMotorCmdCntr;
    int xChinMotorCmdCntr;
    int yChinMotorCmdCntr;
    bool moveStatus;
    int xMotorCurrPos;
    int yMotorCurrPos;
    int focusMotorCurrPos;
    int colorMotorCurrPos;
    int lightSpotMotorCurrPos;
    int shutterMotorCurrPos;
    int xChinMotorCurrPos;
    int yChinMotorCurrPos;
    int envLightDA;
    int castLightDA=0;
};


class DEVICEOPERATIONSHARED_EXPORT DeviceOperation:public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool autoAlignPupil READ getAutoAlignPupil WRITE setAutoAlignPupil NOTIFY autoAlignPupilChanged)
//    Q_PROPERTY(bool isDeviceReady READ getIsDeviceReady WRITE setIsDeviceReady NOTIFY isDeviceReadyChanged)
//    Q_PROPERTY(bool reconnecting READ getReconnecting WRITE setReconnecting NOTIFY reconnectingChanged)
    Q_PROPERTY(int deviceStatus READ getDeviceStatus WRITE setDeviceStatus NOTIFY deviceStatusChanged)
    Q_PROPERTY(int castLightAdjustStatus READ getCastLightAdjustStatus WRITE setCastLightAdjustStatus NOTIFY castLightAdjustStatusChanged)
    Q_PROPERTY(bool pupilDiameter READ getPupilDiameter WRITE setPupilDiameter NOTIFY pupilDiameterChanged)
    Q_PROPERTY(bool envLightAlarm READ getEnvLightAlarm WRITE setEnvLightAlarm NOTIFY envLightAlarmChanged)
    Q_PROPERTY(bool chinDistAlarm READ getChinDistAlarm WRITE setChinDistAlarm NOTIFY chinDistAlarmChanged)

public:

    enum class ChinMoveDirection
    {
        Left,
        Right,
        Up,
        Down,
        Stop,
    };

    enum class BackgroundLight
    {
        yellow,
        white
    };

    DeviceOperation();
    ~DeviceOperation();
    static QSharedPointer<DeviceOperation> getSingleton();
    static void createInstance();
//    static void Initialize();
    void moveChinUp(){if(m_deviceStatus==2) moveChin(ChinMoveDirection::Up);}
    void moveChinDown(){if(m_deviceStatus==2) moveChin(ChinMoveDirection::Down);}
    void moveChinLeft(){if(m_deviceStatus==2) moveChin(ChinMoveDirection::Left);}
    void moveChinRight(){if(m_deviceStatus==2) moveChin(ChinMoveDirection::Right);}
    void stopMovingChin(){if(m_deviceStatus==2) moveChin(ChinMoveDirection::Stop);}
    void enterCheckingPage(){m_isAtCheckingPage=true;}
    void leaveCheckingPage(){m_isAtCheckingPage=false;}

    void connectDev();
    void disconnectDev();
//    void connectOrdisConnectDev();
    void getReadyToStimulate(QPointF loc,int spotSize,int DB,bool isMainDotInfoTable);
    void adjustCastLight();
    void dynamicStimulate(QPointF begin, QPointF end, int cursorSize,int speedLevel,bool isMainDotInfoTable);
    void stopDynamic();
    void openShutter(int durationTime);
    void move5Motors(bool isMotorMove[],int MotorPoses[]);
    void setCursorColorAndCursorSize(int color, int size);
    void setDB(int DB);
    void setLamp(LampId id,int index,bool onOff);
    void setWhiteLamp(bool onOff);
    bool getAnswerPadStatus();
    bool getDynamicMoveStatus();
    QPointF getDyanmicAnswerPos();
    bool getMotorsBusy(QVector<UsbDev::DevCtl::MotorId> motorIDs);
    void waitForSomeTime(int time);
    void waitMotorStop(QVector<UsbDev::DevCtl::MotorId> motorIDs);
    void waitShutterClose();
    void lightUpCastLight();
    void dimDownCastLight();
    void resetMotors(QVector<UsbDev::DevCtl::MotorId> motorIDs);
    void beep(int count,int duration,int interval);
    void beepCheckOver();
    // void alarm();
    void clearPupilData();


private:
    void moveChin(ChinMoveDirection direction);       //0左,1不动,2右;0上,1不动,2下
public slots:
    void workOnNewStatuData();
    void workOnNewFrameData();
    void workOnNewProfile();
    void workOnNewConfig();
    void workOnWorkStatusChanged(int status);
    void reconnectDev();
signals:
    void workStatusChanged();
    void newStatusData();
    void newFrameData(QByteArray ba_origin,QByteArray ba_marked,bool valid);
    void newProfile();
    void newConfig();
    void newDeviceID(QString id);
    void newDeviceVersion(QString version);
    // void updateDevInfo(QString info);
    void newTargetCastLightSensorDA(int DA);

public:
    bool getAutoAlignPupil(){return m_autoAlignPupil;}void setAutoAlignPupil(bool value){m_autoAlignPupil=value;emit autoAlignPupilChanged();}Q_SIGNAL void autoAlignPupilChanged();
//    bool getIsDeviceReady(){return m_isDeviceReady;}void setIsDeviceReady(bool value){if(m_isDeviceReady!=value){m_isDeviceReady=value;emit isDeviceReadyChanged();}}Q_SIGNAL void isDeviceReadyChanged();
//    bool getReconnecting(){return m_reconnecting;}void setReconnecting(bool value){if(m_reconnecting!=value){m_reconnecting=value;emit reconnectingChanged();}}Q_SIGNAL void reconnectingChanged();
    int getDeviceStatus(){return m_deviceStatus;}void setDeviceStatus(int value){if(m_deviceStatus!=value){m_deviceStatus=value;emit deviceStatusChanged();}}Q_SIGNAL void deviceStatusChanged();
    float getPupilDiameter(){return m_devicePupilProcessor.m_pupilDiameter;}void setPupilDiameter(float value){m_devicePupilProcessor.m_pupilDiameter=value;emit pupilDiameterChanged();}Q_SIGNAL void pupilDiameterChanged();
    int getCastLightAdjustStatus(){return m_castLightAdjustStatus;}void setCastLightAdjustStatus(int value){m_castLightAdjustStatus=value;emit castLightAdjustStatusChanged();}Q_SIGNAL void castLightAdjustStatusChanged();
    bool getEnvLightAlarm(){return m_envLightAlarm;}void setEnvLightAlarm(bool value){if(m_envLightAlarm!=value){m_envLightAlarm=value;emit envLightAlarmChanged();}}Q_SIGNAL void envLightAlarmChanged();
    bool getChinDistAlarm(){return m_chinDistAlarm;}void setChinDistAlarm(bool value){if(m_chinDistAlarm!=value){m_chinDistAlarm=value;emit chinDistAlarmChanged();}}Q_SIGNAL void chinDistAlarmChanged();
public:
    DevicePupilProcessor m_devicePupilProcessor;
    Status m_status={-1,-1};
    QTimer m_reconnectTimer;
//    QElapsedTimer m_workStatusElapsedTimer;
//    bool m_isDeviceReady=false;
//    bool m_reconnecting=false;
    int m_deviceStatus=0;                    // 0:初始未连接 1:断联处于重联状态 2:已连接
    bool m_autoAlignPupil=false;
    bool m_eyeglassStatus;
    bool m_eyeglassIntialize=false;
//    bool m_isChecking=false;
//    bool m_connectDev=false;
//    float m_deviation=0;
//    bool m_deviation_valid;
    QSize m_videoSize;
    bool m_isAtCheckingPage=false;
    QSharedPointer<UsbDev::DevCtl> m_devCtl;
//    UsbDev::DevCtl* m_devCtl;
    UsbDev::Config m_config;
    UsbDev::Profile m_profile;
    UsbDev::StatusData m_statusData;
    StatusDataOut m_statusDataOut;


    QMutex m_statusLock;                      //防止多线程冲突,放入同线程不再需要
    UsbDev::FrameData m_frameData;
//    QPoint m_pupilCenterPoint;
//    int m_pupilRadius;
//    bool m_pupilResValid=false;
//    QElapsedTimer m_isStaticCheckPausingTimer;
//    bool m_isKeepingPressingAnswerPad;
//    bool m_isStaticCheckPausing;
    bool m_isWaitingForStaticStimulationAnswer;
    bool m_staticStimulationAnswer;
    int m_castLightAdjustStatus=0;    //0:未标准,1:调整光斑和位置,2:矫正的光强,3:校准完成
    int m_currentCastLightDA;
    bool m_castLightUp=false;
    QByteArray m_frameRawData;
    QMutex m_frameRawDataLock;
    BackgroundLight m_backgroundLight;
//    int m_stimulationTime=180;
//    bool m_pupilDiameterAcquired;

private:
    QElapsedTimer m_autoPupilElapsedTimer;
    QElapsedTimer m_reconnectingElapsedTimer;
    int m_autoPupilElapsedTime=400;
//    QTimer m_videoTimer;
    QElapsedTimer m_castLightAdjustElapsedTimer;
    QElapsedTimer m_castLightStablelizeWaitingElapsedTimer;
    bool m_envLightAlarm=false;
    bool m_chinDistAlarm=false;


    bool m_isMainTable;
    int m_waitingTime;
//    QPair<int,int> m_lastColorAndSpotPos;
//    QVector<float> m_pupilDiameterArr;
    //视岛坐标和电机坐标
    QVector<QPair<QPointF,QPoint>> m_lastDynamicCoordAndXYMotorPos;
    static QSharedPointer<DeviceOperation> m_singleton;
    void moveToAdjustLight(int motorPosX, int motorPosY, int motorPosFocal);



//    QTimer m_statusTimer;
};
}

#endif // DEVICEOPERATION_H
