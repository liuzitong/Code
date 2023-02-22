#include "device_operation.h"
#include "device_settings.h"
#include "device_agorithm.h"
#include <QObject>
#include <qobject.h>
#include <QtCore>
#include <array>
//#include <QQmlEngine>
namespace DevOps{

QSharedPointer<DeviceOperation> DeviceOperation::m_singleton=nullptr;

DeviceOperation::DeviceOperation()
{
    m_statusTimer.setInterval(1000);
    connect(&m_statusTimer,&QTimer::timeout,[&](){m_devReady=false;emit devConStatusChanged();connectDev();});
//    connect(m_devCtl.data(),&UsbDev::DevCtl::newStatusData,this,&DeviceOperation::newStatusData);
//    connect(m_devCtl.data(),&UsbDev::DevCtl::newFrameData,this,&DeviceOperation::newFrameData);
    connect(this,&DeviceOperation::newFrameData,this,&DeviceOperation::onNewFrameData);
    connect(this,&DeviceOperation::newStatusData,this,&DeviceOperation::onNewStatuData);
    connect(this,&DeviceOperation::newFrameData,this,&DeviceOperation::onNewFrameData);
}

QSharedPointer<DeviceOperation> DeviceOperation::getSingleton()
{
    if(m_singleton==nullptr)
    {
        m_singleton.reset(new DeviceOperation());
    }
    return m_singleton;
}

void DeviceOperation::setCursorColorAndCursorSize(int color, int size)
{
    auto profile=m_devCtl->profile();
    auto config=m_devCtl->config();
    quint8 sps[5]={1};
    auto colorToSlot=DeviceSettings::getSingleton()->m_colorToSlot;
    auto spotSizeToSlot=DeviceSettings::getSingleton()->m_spotSizeToSlot;
    int colorSlot,spotSlot;
    for(auto&i:colorToSlot)
    {
        if(i.first==color)
            colorSlot=i.second;
    }
    for(auto&i:spotSizeToSlot)
    {
        if(i.first==size)
            spotSlot=i.second;
    }
    m_devCtl->resetMotor(UsbDev::DevCtl::MotorId_Color,sps[3]);
    m_devCtl->resetMotor(UsbDev::DevCtl::MotorId_Light_Spot,sps[4]);
    waitMotorStop({UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Light_Spot});
    int colorPos=config.switchColorMotorPosPtr()[colorSlot];
    int sizePos=config.switchLightSpotMotorPosPtr()[spotSlot];
    if(m_status.colorSlot!=colorSlot||m_status.spotSlot!=spotSlot)              //变换到改变光斑颜色位置
    {
        int  color_Circl_Motor_Steps=profile.motorRange(UsbDev::DevCtl::MotorId_Color).second-profile.motorRange(UsbDev::DevCtl::MotorId_Color).first;
        int  spot_Circl_Motor_Steps=profile.motorRange(UsbDev::DevCtl::MotorId_Light_Spot).second-profile.motorRange(UsbDev::DevCtl::MotorId_Light_Spot).first;
        //焦距电机
        {
            int focalPos=config.focusPosForSpotAndColorChangeRef();
            int motorPos[5]{0,0,focalPos,m_statusData.motorPosition(UsbDev::DevCtl::MotorId::MotorId_Color)+color_Circl_Motor_Steps*10,m_statusData.motorPosition(UsbDev::DevCtl::MotorId::MotorId_Light_Spot)+spot_Circl_Motor_Steps*10};
            waitMotorStop({UsbDev::DevCtl::MotorId_Focus,UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Light_Spot});
            m_devCtl->move5Motors(std::array<quint8, 5>{0,0,sps[2],1,1}.data(),motorPos);
            waitMotorStop({UsbDev::DevCtl::MotorId_Focus});
            m_devCtl->move5Motors(std::array<quint8, 5>{1,1,1,1,1}.data(),std::array<int, 5>{0,0,0,0,0}.data(),UsbDev::DevCtl::MoveMethod::Relative);
        }
        {
            int motorPos[5]{0,0,0,colorPos,sizePos}; //单个电机绝对位置不可行
            quint8 speed[5]{0,0,0,sps[3],sps[4]};
            waitMotorStop({UsbDev::DevCtl::MotorId_Light_Spot,UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Focus});
            m_devCtl->move5Motors(speed,motorPos);
        }

        {
            int  motorPos[5]{0,0,0,color_Circl_Motor_Steps,spot_Circl_Motor_Steps};
            quint8 speed[5]{0,0,0,sps[3],sps[4]};
            waitMotorStop({UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Light_Spot});          //转一圈拖动光斑和颜色
            m_devCtl->move5Motors(speed,motorPos,UsbDev::DevCtl::Relative);
        }
        {
            int motorPos[5]={0,0,0,-1000,-1000};
            quint8 speed[5]{0,0,0,sps[3],sps[4]};
            waitMotorStop({UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Light_Spot});
            m_devCtl->move5Motors(speed,motorPos,UsbDev::DevCtl::Relative);    //防止干扰误差
        }
        {
            int motorPos[5]={0, 0, 10000, 0 ,0};
            quint8 speed[5]{0,0,sps[2],0,0};
            waitMotorStop({UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Light_Spot,UsbDev::DevCtl::MotorId_Focus});
            m_devCtl->move5Motors(speed,motorPos,UsbDev::DevCtl::Relative);     //DB 脱离颜色和光斑
        }
        m_status.colorSlot=colorSlot;
        m_status.spotSlot=spotSlot;
    }
}

void DeviceOperation::setDB(int DB)
{
    auto config=m_devCtl->config();
    quint8 sps[5]{0};
    int motorPos[5]{0};
    motorPos[3]=config.DbPosMappingPtr()[DB][0];
    motorPos[4]=config.DbPosMappingPtr()[DB][1];
    auto spsConfig=DeviceSettings::getSingleton()->m_5MotorSpeed;
    sps[3]=spsConfig[3];sps[4]=spsConfig[4];
    m_devCtl->move5Motors(sps,motorPos);
}

void DeviceOperation::connectDev()
{
    connect(this,&DeviceOperation::updateDevInfo,[](QString info){qDebug()<<info;});
    auto deviceSettings=DeviceSettings::getSingleton();
    quint32 vid_pid=deviceSettings->m_VID.toInt(nullptr,16)<<16|deviceSettings->m_PID.toInt(nullptr,16);
    do{
        m_devCtl.reset(UsbDev::DevCtl::createInstance(vid_pid));
        if(m_devCtl->workStatus()==UsbDev::DevCtl::WorkStatus::WorkStatus_S_OK)
        {
            updateDevInfo("Connect Successfully.");
            connect(m_devCtl.data(),&UsbDev::DevCtl::workStatusChanged,this,&DeviceOperation::workStatusChanged);
            connect(m_devCtl.data(),&UsbDev::DevCtl::updateInfo,this,&DeviceOperation::updateDevInfo);
            connect(m_devCtl.data(),&UsbDev::DevCtl::newStatusData,this,&DeviceOperation::onNewStatuData);
            connect(m_devCtl.data(),&UsbDev::DevCtl::newFrameData,this,&DeviceOperation::onNewFrameData);
            connect(m_devCtl.data(),&UsbDev::DevCtl::newProfile,this,[&](){updateDevInfo("Profile updated successfully.");isProfileUpdate=true;});
            connect(m_devCtl.data(),&UsbDev::DevCtl::newConfig,this,[&](){updateDevInfo("Config updated successfully.");isConfigUpdated=true;});
            m_devReady=true;
            m_statusTimer.start();
            emit devConStatusChanged();
        }

        else
        {
            static QElapsedTimer elapsedTimer;
            static int retryTimes=0;
            updateDevInfo("Reconnecting...");
            elapsedTimer.restart();
            do{
                QCoreApplication::processEvents();
            }while((elapsedTimer.nsecsElapsed()/1000000<1000));//1000ms
            qDebug()<<retryTimes;
        }
    }while(m_devCtl->workStatus()==UsbDev::DevCtl::WorkStatus::WorkStatus_S_Disconnected);
}

void DeviceOperation::staticStimulate(QPointF loc,int spotSize,int DB,int durationTime)
{
    auto coordSpacePosInfo=DeviceAgorithm::getXYMotorPosAndFocalDistFromCoord(loc);
    auto spotSizeToSlot=DeviceSettings::getSingleton()->m_spotSizeToSlot;
    int spotSlot;
    for(auto&i:spotSizeToSlot)
    {
        if(i.first==spotSize)
            spotSlot=i.second;
    }
    auto focalMotorPos=DeviceAgorithm::getFocusMotorPosByDist(coordSpacePosInfo.focalDist,spotSlot);
    auto config=m_devCtl->config();
    int motorPos[5]{0};
    motorPos[0]=coordSpacePosInfo.motorX;
    motorPos[1]=coordSpacePosInfo.motorY;
    motorPos[2]=focalMotorPos;
    motorPos[3]=config.DbPosMappingPtr()[DB][0];
    motorPos[4]=config.DbPosMappingPtr()[DB][1];
    bool isMotorMove[5]{true};
    waitMotorStop({UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Light_Spot,UsbDev::DevCtl::MotorId_Focus,UsbDev::DevCtl::MotorId_X,UsbDev::DevCtl::MotorId_Y});
    move5Motors(isMotorMove,motorPos);
    waitMotorStop({UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Light_Spot,UsbDev::DevCtl::MotorId_Focus,UsbDev::DevCtl::MotorId_X,UsbDev::DevCtl::MotorId_Y,UsbDev::DevCtl::MotorId_Shutter});
    openShutter(durationTime);
    waitMotorStop({UsbDev::DevCtl::MotorId_Shutter});
}

void DeviceOperation::dynamicStimulate(QPointF begin, QPointF end, int speedLevel)
{

}

bool DeviceOperation::waitForAnswer(int msecs)
{
    return true;
}

QByteArray DeviceOperation::getRealTimeStimulationEyeImage()
{
    return QByteArray();
}

void DeviceOperation::openShutter(int durationTime)
{
    auto config=m_devCtl->config();
    auto shutterPos=config.shutterOpenPosRef();
    m_devCtl->openShutter(durationTime,shutterPos);
}

void DeviceOperation::move5Motors(bool isMotorMove[], int MotorPoses[])
{
    auto spsConfig=DeviceSettings::getSingleton()->m_5MotorSpeed;
    quint8 sps[5]{0};
    for(int i=0;i<5;i++)
    {
        if(isMotorMove[i])
        {
            sps[i]=spsConfig[i];
        }
    }
    m_devCtl->move5Motors(sps,MotorPoses);
}

void DeviceOperation::hello()
{
//   m_singleton->hello();
//    m_singleton->privHello();
    qDebug()<<"word";
}

void DeviceOperation::waitMotorStop(QVector<UsbDev::DevCtl::MotorId> motorIDs)
{
    auto getBusy=[&]()->bool
    {
        for(auto& motorId:motorIDs)
        {
            if(m_statusData.isMotorBusy(motorId))
            {
                qDebug()<<" motor "<<motorId<<" is busy";
                return true;
            }
        }
        qDebug()<<"no motor busy";
        return false;
    };
    QElapsedTimer mstimer;
    mstimer.restart();//必须先等一会儿刷新状态
    do
    {
        QCoreApplication::processEvents();
    }while(getBusy()||(mstimer.nsecsElapsed()/1000000<100)); //500ms
//    while(getBusy()){QCoreApplication::processEvents();}
}

void DeviceOperation::onNewStatuData()
{
    m_statusTimer.start();
    m_statusData=m_devCtl->takeNextPendingStatusData();
    emit newStatusData();
}

void DeviceOperation::onNewFrameData()
{
    m_frameData=m_devCtl->takeNextPendingFrameData();
    emit newFrameData();
}
}

