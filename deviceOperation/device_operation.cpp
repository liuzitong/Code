#include "device_operation.h"
#include "device_settings.h"
#include "device_data_processer.h"
#include <QObject>
#include <qobject.h>
#include <QtCore>
#include <array>
//#include <QQmlEngine>
namespace DevOps{

QSharedPointer<DeviceOperation> DeviceOperation::m_singleton=nullptr;

DeviceOperation::DeviceOperation()
{
//    m_statusTimer.setInterval(1000);
//    connect(&m_statusTimer,&QTimer::timeout,[&](){m_isDeviceReady=false;emit devConStatusChanged();connectDev();});
//    connect(m_devCtl.data(),&UsbDev::DevCtl::newStatusData,this,&DeviceOperation::newStatusData);
//    connect(m_devCtl.data(),&UsbDev::DevCtl::newFrameData,this,&DeviceOperation::newFrameData);
    connect(this,&DeviceOperation::newFrameData,this,&DeviceOperation::onNewFrameData);
    connect(this,&DeviceOperation::newStatusData,this,&DeviceOperation::onNewStatuData);
    connect(this,&DeviceOperation::newFrameData,this,&DeviceOperation::onNewFrameData);
}

DeviceOperation::~DeviceOperation()
{
//    m_statusTimer.stop();
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

bool DeviceOperation::getAnswerPadStatus()
{
    return m_statusData.answerpadStatus();
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
//被checkSvcWorker 调用
void DeviceOperation::connectDev()
{
//    connect(this,&DeviceOperation::updateDevInfo,[](QString info){qDebug()<<info;});
//    auto deviceSettings=DeviceSettings::getSingleton();
//    quint32 vid_pid=deviceSettings->m_VID.toInt(nullptr,16)<<16|deviceSettings->m_PID.toInt(nullptr,16);
//    m_devCtl.reset(UsbDev::DevCtl::createInstance(vid_pid));
//    if(m_devCtl->workStatus()==UsbDev::DevCtl::WorkStatus::WorkStatus_S_OK)
//    {
//        updateDevInfo("Connect Successfully.");
//        connect(m_devCtl.data(),&UsbDev::DevCtl::workStatusChanged,this,&DeviceOperation::workStatusChanged);
//        connect(m_devCtl.data(),&UsbDev::DevCtl::updateInfo,this,&DeviceOperation::updateDevInfo);
//        connect(m_devCtl.data(),&UsbDev::DevCtl::newStatusData,this,&DeviceOperation::onNewStatuData);
//        connect(m_devCtl.data(),&UsbDev::DevCtl::newFrameData,this,&DeviceOperation::onNewFrameData);
//        connect(m_devCtl.data(),&UsbDev::DevCtl::newProfile,this,[&](){updateDevInfo("Profile updated successfully.");m_isProfileUpdate=true;});
//        connect(m_devCtl.data(),&UsbDev::DevCtl::newConfig,this,[&](){updateDevInfo("Config updated successfully.");m_isConfigUpdated=true;});
//        m_isDeviceReady=true;
//        emit devConStatusChanged();
//    }

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
            connect(m_devCtl.data(),&UsbDev::DevCtl::newProfile,this,[&](){updateDevInfo("Profile updated successfully.");m_isProfileUpdate=true;});
            connect(m_devCtl.data(),&UsbDev::DevCtl::newConfig,this,[&](){updateDevInfo("Config updated successfully.");m_isConfigUpdated=true;});
            m_isDeviceReady=true;
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
            }while((elapsedTimer.nsecsElapsed()/10000000<1000));//10000ms
            qDebug()<<retryTimes;
        }
    }while(m_devCtl->workStatus()==UsbDev::DevCtl::WorkStatus::WorkStatus_S_Disconnected);
}

void DeviceOperation::staticStimulate(QPointF loc,int spotSize,int DB,int durationTime)
{
    auto coordSpacePosInfo=DeviceDataProcesser::getXYMotorPosAndFocalDistFromCoord(loc);
    auto spotSizeToSlot=DeviceSettings::getSingleton()->m_spotSizeToSlot;
    int spotSlot;
    for(auto&i:spotSizeToSlot)
    {
        if(i.first==spotSize)
            spotSlot=i.second;
    }
    auto focalMotorPos=DeviceDataProcesser::getFocusMotorPosByDist(coordSpacePosInfo.focalDist,spotSlot);
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
    if(durationTime!=0) openShutter(durationTime);
}

void DeviceOperation::getReadyToStimulate(QPointF loc, int spotSize, int DB)
{
    auto coordSpacePosInfo=DeviceDataProcesser::getXYMotorPosAndFocalDistFromCoord(loc);
    auto spotSizeToSlot=DeviceSettings::getSingleton()->m_spotSizeToSlot;
    int spotSlot;
    for(auto&i:spotSizeToSlot)
    {
        if(i.first==spotSize)
            spotSlot=i.second;
    }
    auto focalMotorPos=DeviceDataProcesser::getFocusMotorPosByDist(coordSpacePosInfo.focalDist,spotSlot);
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
}

void DeviceOperation::dynamicStimulate(QPointF begin, QPointF end, int speedLevel)
{

}

//bool DeviceOperation::waitForAnswer(int msecs)
//{
//    QElapsedTimer elapsedTimer;
//    while(!m_statusData.answerpadStatus()&&elapsedTimer.elapsed()<msecs)
//    {
//        QApplication::
//    }
//    return false;
//}

QByteArray DeviceOperation::getRealTimeStimulationEyeImage()
{
    return m_frameData.rawData();
}

void DeviceOperation::openShutter(int durationTime)
{
    auto config=m_devCtl->config();
    auto shutterPos=config.shutterOpenPosRef();
    waitMotorStop({UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Light_Spot,UsbDev::DevCtl::MotorId_Focus,UsbDev::DevCtl::MotorId_X,UsbDev::DevCtl::MotorId_Y,UsbDev::DevCtl::MotorId_Shutter});
    m_devCtl->openShutter(durationTime,shutterPos);
}

void DeviceOperation::move5Motors(bool isMotorMove[], int MotorPoses[])
{
    auto spsConfig=DeviceSettings::getSingleton()->m_5MotorSpeed;
    quint8 sps[7]{0};
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

void DeviceOperation::moveChin(ChinHozMoveDirection hozChin, ChinVertMoveDirection vertChin)
{
    auto profile=m_devCtl->profile();
    quint8 sps[2]{0};
    int motorPos[2]{0};
    auto spsConfig=DeviceSettings::getSingleton()->m_motorChinSpeed;
    switch(hozChin)
    {
    case ChinHozMoveDirection::Left:
    {
        motorPos[0]=profile.motorRange(UsbDev::DevCtl::MotorId_Chin_Hoz).first;
        sps[0]=spsConfig[0];
        break;
    }
    case ChinHozMoveDirection::Right:
    {
        motorPos[0]=profile.motorRange(UsbDev::DevCtl::MotorId_Chin_Hoz).second;
        sps[0]=spsConfig[0];
        break;
    }
    case ChinHozMoveDirection::Stop:
    {
        break;
    }
    }

    switch(vertChin)
    {
    case ChinVertMoveDirection::Up:
    {
        motorPos[1]=profile.motorRange(UsbDev::DevCtl::MotorId_Chin_Vert).first;
        sps[1]=spsConfig[1];
        break;
    }
    case ChinVertMoveDirection::Down:
    {
        motorPos[1]=profile.motorRange(UsbDev::DevCtl::MotorId_Chin_Vert).second;
        sps[1]=spsConfig[1];
        break;
    }
    case ChinVertMoveDirection::Stop:
    {
        break;
    }
    }
    m_devCtl->moveChinMotors(sps,motorPos);
}


void DeviceOperation::onNewStatuData()
{
//    m_statusTimer.start();
    m_statusData=m_devCtl->takeNextPendingStatusData();
    emit newStatusData();
}

void DeviceOperation::onNewFrameData()
{

    m_frameData=m_devCtl->takeNextPendingFrameData();
    emit newFrameData();
    auto profile=m_devCtl->profile();
    if(m_autoAlignPupil)                //自动对眼位
    {
        int tolerance=DeviceSettings::getSingleton()->m_pupilAutoAlignPixelTolerance;

        auto deviation=DeviceDataProcesser::caculatePupilDeviation(m_frameData.rawData(),profile.videoSize().width(),profile.videoSize().height());
        quint8 sps[2]{0};
        int motorPos[2]{0};
        auto spsConfig=DeviceSettings::getSingleton()->m_motorChinSpeed;
        if(deviation.x()>tolerance)
        {
            sps[0]=spsConfig[0];
            motorPos[0]=profile.motorRange(UsbDev::DevCtl::MotorId_Chin_Hoz).first;
        }
        if(deviation.x()<-tolerance)
        {
            sps[0]=spsConfig[0];
            motorPos[0]=profile.motorRange(UsbDev::DevCtl::MotorId_Chin_Hoz).second;
        }
        if(deviation.y()>tolerance)
        {
            sps[1]=spsConfig[1];
            motorPos[1]=profile.motorRange(UsbDev::DevCtl::MotorId_Chin_Vert).first;
        }
        if(deviation.y()<-tolerance)
        {
            sps[1]=spsConfig[1];
            motorPos[1]=profile.motorRange(UsbDev::DevCtl::MotorId_Chin_Vert).second;
        }
        m_devCtl->moveChinMotors(sps,motorPos);
    }
    if(m_pupilDiameter<0)
    {
        auto pupilDiameter=DeviceDataProcesser::caculatePupilDiameter(m_frameData.rawData(),profile.videoSize().width(),profile.videoSize().height());
        m_pupilDiameterArr.push_back(pupilDiameter);
        if(m_pupilDiameterArr.size()>=50)
        {
            float sum=0;
            for(auto&i:m_pupilDiameterArr)
            {
                sum+=i;
            }
            m_pupilDiameter=sum/m_pupilDiameterArr.size();
        }
    }

}

void DeviceOperation::onWorkStatusChanged()
{
    if(m_devCtl->workStatus()==UsbDev::DevCtl::WorkStatus::WorkStatus_S_Disconnected)
    {
        connectDev();
    }
}
}

