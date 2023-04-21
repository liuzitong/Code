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

namespace DevOps{
using LampId=UsbDev::DevCtl::LampId;

struct Status
{
   int color;
   int spot;
   int DB;
};


class DEVICEOPERATIONSHARED_EXPORT DeviceOperation:public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool autoAlignPupil READ getAutoAlignPupil WRITE setAutoAlignPupil NOTIFY autoAlignPupilChanged)
    Q_PROPERTY(bool isDeviceReady READ getIsDeviceReady WRITE setIsDeviceReady NOTIFY isDeviceReadyChanged)
    Q_PROPERTY(bool isCastLightAdjusted READ getIsCastLightAdjusted WRITE setIsCastLightAdjusted NOTIFY isCastLightAdjustedChanged)
    Q_PROPERTY(bool pupilDiameter READ getPupilDiameter WRITE setPupilDiameter NOTIFY pupilDiameterChanged)

public:

    enum class ChinMoveDirection
    {
        Left,
        Right,
        Up,
        Down,
        Stop,
    };

    DeviceOperation();
    ~DeviceOperation();
//    static void Initialize();
    void moveChinUp(){if(m_isDeviceReady) moveChin(ChinMoveDirection::Up);}
    void moveChinDown(){if(m_isDeviceReady) moveChin(ChinMoveDirection::Down);}
    void moveChinLeft(){if(m_isDeviceReady) moveChin(ChinMoveDirection::Left);}
    void moveChinRight(){if(m_isDeviceReady) moveChin(ChinMoveDirection::Right);}
    void stopMovingChin(){if(m_isDeviceReady) moveChin(ChinMoveDirection::Stop);}
    void turnOnVideo(){if(m_isDeviceReady) m_devCtl->setFrontVideo(true);}
    void turnOffVideo(){if(m_isDeviceReady) m_devCtl->setFrontVideo(false);}
    static QSharedPointer<DeviceOperation> getSingleton();
    void connectDev();
    void disconnectDev();
    void getReadyToStimulate(QPointF loc,int spotSize,int DB,bool isMainDotInfoTable);
    void adjustCastLight();
    void dynamicStimulate(QPointF begin, QPointF end, int cursorSize,int speedLevel,bool isMainDotInfoTable);
    void stopDynamic();
    QByteArray getRealTimeStimulationEyeImage();
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
    void hello();
    void waitMotorStop(QVector<UsbDev::DevCtl::MotorId> motorIDs);
    void lightUpCastLight();
    void dimDownCastLight();
private:
    void moveChin(ChinMoveDirection direction);       //0左,1不动,2右;0上,1不动,2下
public slots:
    void workOnNewStatuData();
    void workOnNewFrameData();
    void workOnNewProfile();
    void workOnNewConfig();
    void workOnWorkStatusChanged();
signals:
    void workStatusChanged();
    void newStatusData();
    void newFrameData();
    void newProfile();
    void newConfig();
    void updateDevInfo(QString info);
//    void staticCursorLoc(QPointF loc);
//    void dynamicCursorLoc(QPointF loc);
//    void shutterStatus(bool status);
//    void devConStatusChanged();
//    void newFixationDeviation(int deviation)
public:
    bool getAutoAlignPupil(){return m_autoAlignPupil;}void setAutoAlignPupil(bool value){m_autoAlignPupil=value;emit autoAlignPupilChanged();}Q_SIGNAL void autoAlignPupilChanged();
    bool getIsDeviceReady(){return m_isDeviceReady;}void setIsDeviceReady(bool value){if(m_isDeviceReady!=value){m_isDeviceReady=value;emit isDeviceReadyChanged();}}Q_SIGNAL void isDeviceReadyChanged();
    float getPupilDiameter(){return m_pupilDiameter;}void setPupilDiameter(float value){m_pupilDiameter=value;emit pupilDiameterChanged();}Q_SIGNAL void pupilDiameterChanged();
    bool getIsCastLightAdjusted(){return m_isCastLightAdjusted;}void setIsCastLightAdjusted(bool value){m_isCastLightAdjusted=value;emit isCastLightAdjustedChanged();}Q_SIGNAL void isCastLightAdjustedChanged();
public:
    Status m_status;
    bool m_isDeviceReady=false,m_autoAlignPupil=true;
    bool m_isChecking=false;
    float m_deviation=0;
//    bool m_deviation_valid;
    QSize m_videoSize;
    QSharedPointer<UsbDev::DevCtl> m_devCtl;
//    UsbDev::DevCtl* m_devCtl;
    UsbDev::Config m_config;
    UsbDev::Profile m_profile;
    UsbDev::StatusData m_statusData;
    QMutex m_statusLock;                      //防止多线程冲突,放入同线程不再需要
    UsbDev::FrameData m_frameData;
    QByteArray m_frameRawData;
    QElapsedTimer m_shutterElapsedTimer;
    int m_shutterElapsedTime;
    bool m_isCastLightAdjusted=false;
    int m_currentCastLightDA=m_config.castLightADPresetRef();
    int m_castLightTargetDA;
    QElapsedTimer m_castLightAdjustElapsedTimer;

private:
    float m_pupilDiameter=-1;
    bool m_isMainTable;
    QVector<float> m_pupilDiameterArr;
    //视岛坐标和电机坐标
    QVector<QPair<QPointF,QPoint>> m_lastDynamicCoordAndXYMotorPos;
    static QSharedPointer<DeviceOperation> m_singleton;

//    QTimer m_statusTimer;
    QString word;
    void getReadyToStimulate(int motorPosX, int motorPosY, int motorPosFocal);
};
}

#endif // DEVICEOPERATION_H
