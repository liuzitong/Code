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

namespace DevOps{
struct Status
{
   int colorSlot;
   int spotSlot;
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
    Q_INVOKABLE void moveChinUp(){moveChin(ChinHozMoveDirection::Stop,ChinVertMoveDirection::Up);}
    Q_INVOKABLE void moveChinDown(){moveChin(ChinHozMoveDirection::Stop,ChinVertMoveDirection::Down);}
    Q_INVOKABLE void moveChinLeft(){moveChin(ChinHozMoveDirection::Left,ChinVertMoveDirection::Stop);}
    Q_INVOKABLE void moveChinRight(){moveChin(ChinHozMoveDirection::Right,ChinVertMoveDirection::Stop);}
    Q_INVOKABLE void stopMovingChin(){moveChin(ChinHozMoveDirection::Stop,ChinVertMoveDirection::Stop);}


    static QSharedPointer<DeviceOperation> getSingleton();
    void connectDev();
    void staticStimulate(QPointF loc,int spotSize,int DB,int durationTime);
    void getReadyToStimulate(QPointF loc,int spotSize,int DB);
    void dynamicStimulate(QPointF begin, QPointF end, int speedLevel);
    QByteArray getRealTimeStimulationEyeImage();
    void openShutter(int durationTime);
    void move5Motors(bool isMotorMove[],int MotorPoses[]);
    void setCursorColorAndCursorSize(int color, int size);
    bool getAnswerPadStatus();
    void hello();




private:
    void setDB(int DB);
    void waitMotorStop(QVector<UsbDev::DevCtl::MotorId> motorIDs);
    void moveChin(ChinHozMoveDirection hozChin,ChinVertMoveDirection vertChin);                        //0左,1不动,2右;0上,1不动,2下
public slots:
    void onNewStatuData();
    void onNewFrameData();
    void onWorkStatusChanged();
signals:
    void workStatusChanged();
    void newStatusData();
    void newFrameData();
    void newProfile();
    void newConfig();
    void updateDevInfo(QString info);
    void devConStatusChanged();

public:
    bool getAutoAlignPupil(){return m_autoAlignPupil;}void setAutoAlignPupil(bool value){m_autoAlignPupil=value;emit autoAlignPupilChanged();}Q_SIGNAL void autoAlignPupilChanged();
    bool getIsDeviceReady(){return m_isDeviceReady;}void setIsDeviceReady(bool value){m_isDeviceReady=value;emit isDeviceReadyChanged();}Q_SIGNAL void isDeviceReadyChanged();
    float getPupilDiameter(){return m_pupilDiameter;}void setPupilDiameter(float value){m_pupilDiameter=value;emit pupilDiameterChanged();}Q_SIGNAL void pupilDiameterChanged();
public:
    Status m_status={-1,-1,-1};
    bool m_isConfigUpdated=false,m_isProfileUpdate=false,m_isDeviceReady=false,m_autoAlignPupil=true;
    QSharedPointer<UsbDev::DevCtl> m_devCtl;
    UsbDev::StatusData m_statusData;
    UsbDev::FrameData m_frameData;
private:
    float m_pupilDiameter=-1;
    QVector<float> m_pupilDiameterArr;
    static QSharedPointer<DeviceOperation> m_singleton;

//    QTimer m_statusTimer;
    QString word;
};
}

#endif // DEVICEOPERATION_H
