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
public:
    DeviceOperation();
//    ~DeviceOperation();
    static void Initialize();
    static QSharedPointer<DeviceOperation> getSingleton();
    void connectDev();
    void staticStimulate(QPointF loc,int spotSize,int DB,int durationTime);
    void dynamicStimulate(QPointF begin, QPointF end, int speedLevel);
    bool waitForAnswer(int msecs);
    QByteArray getRealTimeStimulationEyeImage();
    void openShutter(int durationTime);
    void move5Motors(bool isMotorMove[],int MotorPoses[]);
    void setCursorColorAndCursorSize(int color, int size);
    void hello();

    bool m_devReady=false;
private:
    void setDB(int DB);
    void waitMotorStop(QVector<UsbDev::DevCtl::MotorId> motorIDs);



public slots:
    void onNewStatuData();
    void onNewFrameData();
signals:
    void workStatusChanged();
    void newStatusData();
    void newFrameData();
    void newProfile();
    void newConfig();
    void updateDevInfo(QString info);
    void devConStatusChanged();


public:
    QSharedPointer<UsbDev::DevCtl> m_devCtl;
    UsbDev::StatusData m_statusData;
    UsbDev::FrameData m_frameData;

private:
    static QSharedPointer<DeviceOperation> m_singleton;
    Status m_status={-1,-1,-1};
    bool isConfigUpdated=false,isProfileUpdate=false;
    bool isConnected=false;

//    QElapsedTimer m_elapsedTimer;
    QTimer m_statusTimer;
    QString word;
};
}

#endif // DEVICEOPERATION_H
