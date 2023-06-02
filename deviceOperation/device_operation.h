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
//   int DB;
};


class DEVICEOPERATIONSHARED_EXPORT DeviceOperation:public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool autoAlignPupil READ getAutoAlignPupil WRITE setAutoAlignPupil NOTIFY autoAlignPupilChanged)
    Q_PROPERTY(bool isDeviceReady READ getIsDeviceReady WRITE setIsDeviceReady NOTIFY isDeviceReadyChanged)
    Q_PROPERTY(int castLightAdjustStatus READ getCastLightAdjustStatus WRITE setCastLightAdjustStatus NOTIFY castLightAdjustStatusChanged)
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
    void enterCheckingPage(){m_isAtCheckingPage=true;}
    void leaveCheckingPage(){m_isAtCheckingPage=false;}
    static QSharedPointer<DeviceOperation> getSingleton();
    void connectDev();
    void disconnectDev();
    void connectOrdisConnectDev();
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
    void lightUpCastLight();
    void dimDownCastLight();
    void resetMotors(QVector<UsbDev::DevCtl::MotorId> motorIDs);
    void beep();

private:
    void moveChin(ChinMoveDirection direction);       //0左,1不动,2右;0上,1不动,2下
public slots:
    void workOnNewStatuData();
    void workOnNewFrameData();
    void workOnNewProfile();
    void workOnNewConfig();
    void workOnWorkStatusChanged(int status);
signals:
    void workStatusChanged();
    void newStatusData();
    void newFrameData();
    void newProfile();
    void newConfig();
    void updateDevInfo(QString info);
public:
    bool getAutoAlignPupil(){return m_autoAlignPupil;}void setAutoAlignPupil(bool value){m_autoAlignPupil=value;emit autoAlignPupilChanged();}Q_SIGNAL void autoAlignPupilChanged();
    bool getIsDeviceReady(){return m_isDeviceReady;}void setIsDeviceReady(bool value){if(m_isDeviceReady!=value){m_isDeviceReady=value;emit isDeviceReadyChanged();}}Q_SIGNAL void isDeviceReadyChanged();
    float getPupilDiameter(){return m_pupilDiameter;}void setPupilDiameter(float value){m_pupilDiameter=value;emit pupilDiameterChanged();}Q_SIGNAL void pupilDiameterChanged();
    int getCastLightAdjustStatus(){return m_castLightAdjustStatus;}void setCastLightAdjustStatus(int value){m_castLightAdjustStatus=value;emit castLightAdjustStatusChanged();}Q_SIGNAL void castLightAdjustStatusChanged();
public:
    Status m_status;
    QElapsedTimer m_workStatusElapsedTimer;
    bool m_isDeviceReady=false;
    bool m_autoAlignPupil=true;
    bool m_eyeglassStatus;
    bool m_eyeglassIntialize=false;
//    bool m_isChecking=false;
    bool m_connectDev=false;
    float m_deviation=0;
//    bool m_deviation_valid;
    QSize m_videoSize;
    bool m_isAtCheckingPage=false;
    QSharedPointer<UsbDev::DevCtl> m_devCtl;
//    UsbDev::DevCtl* m_devCtl;
    UsbDev::Config m_config;
    UsbDev::Profile m_profile;
    UsbDev::StatusData m_statusData;
    QMutex m_statusLock;                      //防止多线程冲突,放入同线程不再需要
    UsbDev::FrameData m_frameData;
    QPoint m_pupilCenterPoint;
    int m_pupilRadius;
    bool m_pupilResValid=false;
    QMutex m_frameRawDataLock;
    QByteArray m_frameRawData;
    QTimer m_connectTimer;
    QTimer m_videoTimer;
//    QElapsedTimer m_isStaticCheckPausingTimer;
//    bool m_isKeepingPressingAnswerPad;
//    bool m_isStaticCheckPausing;
    bool m_isWaitingForStaticStimulationAnswer;
    bool m_staticStimulationAnswer;
    int m_castLightAdjustStatus=0;    //0:未标准,1:调整光斑和位置,2:矫正的光强,3:校准完成
    int m_currentCastLightDA;
    int m_castLightTargetDA;
    bool m_castLightUp=true;
//    int m_stimulationTime=180;
    QElapsedTimer m_castLightAdjustElapsedTimer;
    QElapsedTimer m_autoPupilElapsedTimer;
    float m_pupilDiameter=0;
//    bool m_pupilDiameterAcquired;

private:
    bool m_isMainTable;
    QVector<float> m_pupilDiameterArr;
    //视岛坐标和电机坐标
    QVector<QPair<QPointF,QPoint>> m_lastDynamicCoordAndXYMotorPos;
    static QSharedPointer<DeviceOperation> m_singleton;

//    QTimer m_statusTimer;
    QString word;
    void moveToAdjustLight(int motorPosX, int motorPosY, int motorPosFocal);
};
}

#endif // DEVICEOPERATION_H
