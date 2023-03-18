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
    Q_PROPERTY(bool pupilDiameter READ getPupilDiameter WRITE setPupilDiameter NOTIFY pupilDiameterChanged)
public:

    enum class ChinHozMoveDirection
    {
        Left,
        Stop,
        Right,
    };
    enum class ChinVertMoveDirection
    {
        Up,
        Stop,
        Down
    };
    DeviceOperation();
    ~DeviceOperation();
//    static void Initialize();
    void moveChinUp(){if(m_isDeviceReady) moveChin(ChinHozMoveDirection::Stop,ChinVertMoveDirection::Up);}
    void moveChinDown(){if(m_isDeviceReady) moveChin(ChinHozMoveDirection::Stop,ChinVertMoveDirection::Down);}
    void moveChinLeft(){if(m_isDeviceReady) moveChin(ChinHozMoveDirection::Left,ChinVertMoveDirection::Stop);}
    void moveChinRight(){if(m_isDeviceReady) moveChin(ChinHozMoveDirection::Right,ChinVertMoveDirection::Stop);}
    void stopMovingChin(){if(m_isDeviceReady) moveChin(ChinHozMoveDirection::Stop,ChinVertMoveDirection::Stop);}
    void turnOnVideo(){if(m_isDeviceReady) m_devCtl->setFrontVideo(true);}
    void turnOffVideo(){if(m_isDeviceReady) m_devCtl->setFrontVideo(false);}
    static QSharedPointer<DeviceOperation> getSingleton();
    void connectDev();
    void disconnectDev();
    void staticStimulate(QPointF loc,int spotSize,int DB,int durationTime);
    void getReadyToStimulate(QPointF loc,int spotSize,int DB);
    void dynamicStimulate(QPointF begin, QPointF end, int speedLevel);
    QByteArray getRealTimeStimulationEyeImage();
    void openShutter(int durationTime);
    void move5Motors(bool isMotorMove[],int MotorPoses[]);
    void setCursorColorAndCursorSize(int color, int size);
    bool getAnswerPadStatus();
    void waitForSomeTime(int time);
    void hello();




private:
    void setDB(int DB);
    void waitMotorStop(QVector<UsbDev::DevCtl::MotorId> motorIDs);
    void moveChin(ChinHozMoveDirection hozChin,ChinVertMoveDirection vertChin);                        //0左,1不动,2右;0上,1不动,2下
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
//    void devConStatusChanged();
//    void newFixationDeviation(int deviation)
public:
    bool getAutoAlignPupil(){return m_autoAlignPupil;}void setAutoAlignPupil(bool value){m_autoAlignPupil=value;emit autoAlignPupilChanged();}Q_SIGNAL void autoAlignPupilChanged();
    bool getIsDeviceReady(){return m_isDeviceReady;}void setIsDeviceReady(bool value){m_isDeviceReady=value;emit isDeviceReadyChanged();}Q_SIGNAL void isDeviceReadyChanged();
    float getPupilDiameter(){return m_pupilDiameter;}void setPupilDiameter(float value){m_pupilDiameter=value;emit pupilDiameterChanged();}Q_SIGNAL void pupilDiameterChanged();
public:
    Status m_status={-1,-1,-1};
    bool m_isDeviceReady=false,m_autoAlignPupil=true;
    float m_deviation=0;
//    bool m_deviation_valid;
    QSize m_videoSize;
    QSharedPointer<UsbDev::DevCtl> m_devCtl;
//    UsbDev::DevCtl* m_devCtl;
    UsbDev::Config m_config;
    UsbDev::Profile m_profile;
    UsbDev::StatusData m_statusData;
    QMutex m_statusLock;
    UsbDev::FrameData m_frameData;
    QByteArray m_frameRawData;
    QElapsedTimer m_shutterElapsedTimer;
    int m_shutterElapsedTime;
private:
    float m_pupilDiameter=-1;
    QVector<float> m_pupilDiameterArr;
    static QSharedPointer<DeviceOperation> m_singleton;

//    QTimer m_statusTimer;
    QString word;
};
}

#endif // DEVICEOPERATION_H
