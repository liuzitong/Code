#include "device_operation.h"
#include "device_settings.h"
#include "device_data_processer.h"
#include <QObject>
#include <qobject.h>
#include <QtCore>
#include <array>
#include <QtMath>
//#include <QQmlEngine>
namespace DevOps{

QSharedPointer<DeviceOperation> DeviceOperation::m_singleton=nullptr;

DeviceOperation::DeviceOperation()
{
//    m_statusTimer.setInterval(1000);
//    connect(&m_statusTimer,&QTimer::timeout,[&](){m_isDeviceReady=false;emit devConStatusChanged();connectDev();});
//    connect(m_devCtl.data(),&UsbDev::DevCtl::newStatusData,this,&DeviceOperation::newStatusData);
//    connect(m_devCtl.data(),&UsbDev::DevCtl::newFrameData,this,&DeviceOperation::newFrameData);
//    connect(this,&DeviceOperation::updateDevInfo,[](QString info){qDebug()<<info;});
    m_config=DeviceData::getSingleton()->m_config;
//    qDebug()<<m_config.DbPosMappingPtr()[0][0];
//    qDebug()<<DeviceData::getSingleton()->m_config.DbPosMappingPtr()[0][0];
//    qDebug()<<DeviceData::getSingleton()->m_config.DbPosMappingPtr()[0][1];
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

void DeviceOperation::setCursorColorAndCursorSize(int color, int spot)
{
//    qDebug()<<"setCursorColorAndCursorSize:"+QString::number(int(thread()->currentThread()),16);
    if(m_status.color!=color||m_status.spot!=spot)              //变换到改变光斑颜色位置
    {
        auto profile=m_profile;
        auto config=DeviceData::getSingleton()->m_config;
        quint8 sps[5]={1,1,1,1,1};
        auto colorToSlot=DeviceSettings::getSingleton()->m_colorToSlot;
        auto spotSizeToSlot=DeviceSettings::getSingleton()->m_spotSizeToSlot;
        int colorSlot,spotSlot;
        for(auto&i:colorToSlot)
        {
            if(i.first==color)
            {
                colorSlot=i.second;
                break;
            }
        }
        for(auto&i:spotSizeToSlot)
        {
            if(i.first==spot)
            {
                spotSlot=i.second;
                break;
            }
        }
        m_devCtl->resetMotor(UsbDev::DevCtl::MotorId_Focus,sps[2]);
        m_devCtl->resetMotor(UsbDev::DevCtl::MotorId_Color,sps[3]);
        m_devCtl->resetMotor(UsbDev::DevCtl::MotorId_Light_Spot,sps[4]);
        waitMotorStop({UsbDev::DevCtl::MotorId_Focus,UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Light_Spot});
        int colorPos=config.switchColorMotorPosPtr()[colorSlot];
        int sizePos=config.switchLightSpotMotorPosPtr()[spotSlot];
        int  color_Circl_Motor_Steps=profile.motorRange(UsbDev::DevCtl::MotorId_Color).second-profile.motorRange(UsbDev::DevCtl::MotorId_Color).first;
        int  spot_Circl_Motor_Steps=profile.motorRange(UsbDev::DevCtl::MotorId_Light_Spot).second-profile.motorRange(UsbDev::DevCtl::MotorId_Light_Spot).first;
        //焦距电机
        {
            int focalPos=config.focusPosForSpotAndColorChangeRef();
            qDebug()<<"focalPos="+QString::number(focalPos);
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
        m_status.color=color;
        m_status.spot=spot;
    }
}

void DeviceOperation::setLamp(LampId id, int index, bool onOff)
{
    int da=0;
    if(onOff)
    {
        auto config=DeviceData::getSingleton()->m_config;
        switch (id)
        {
        case LampId::LampId_centerFixation:da=config.centerFixationLampDARef();break;
        case LampId::LampId_bigDiamond:da=config.bigDiamondfixationLampDAPtr()[index];break;
        case LampId::LampId_smallDiamond:da=config.smallDiamondFixationLampDAPtr()[index];break;
        case LampId::LampId_yellowBackground:da=config.yellowBackgroundLampDARef();break;
        case LampId::LampId_centerInfrared:da=config.centerFixationLampDARef();break;
        case LampId::LampId_borderInfrared:da=config.borderInfraredLampDARef();break;
        case LampId::LampId_eyeglassInfrared:da=config.eyeglassFrameLampDARef();break;
        case LampId::LampId_castLight:da=config.castLightADPresetRef();break;
        }
    }
    m_devCtl->setLamp(UsbDev::DevCtl::LampId(id),index,da);
}


void DeviceOperation::setWhiteLamp(bool onOff)
{
    auto config=DeviceData::getSingleton()->m_config;
    auto whiteLampDaPtr=config.whiteBackgroundLampDAPtr();
    if(onOff)
        m_devCtl->setWhiteLamp(whiteLampDaPtr[0],whiteLampDaPtr[1],whiteLampDaPtr[2]);
    else
        m_devCtl->setWhiteLamp(0,0,0);
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
    auto deviceSettings=DeviceSettings::getSingleton();
    quint32 vid_pid=deviceSettings->m_VID.toInt(nullptr,16)<<16|deviceSettings->m_PID.toInt(nullptr,16);
    m_devCtl.reset(UsbDev::DevCtl::createInstance(vid_pid));
    qDebug()<<QString::number(vid_pid,16);
//    connect(m_devCtl.data(),&UsbDev::DevCtl::workStatusChanged,this,&DeviceOperation::workOnWorkStatusChanged);
    connect(m_devCtl.data(),&UsbDev::DevCtl::updateInfo,this,&DeviceOperation::updateDevInfo);
    connect(m_devCtl.data(),&UsbDev::DevCtl::newStatusData,this,&DeviceOperation::workOnNewStatuData);
    connect(m_devCtl.data(),&UsbDev::DevCtl::newFrameData,this,&DeviceOperation::workOnNewFrameData);
    connect(m_devCtl.data(),&UsbDev::DevCtl::newProfile,this,&DeviceOperation::workOnNewProfile);
    connect(m_devCtl.data(),&UsbDev::DevCtl::newConfig,this,&DeviceOperation::workOnNewConfig);
//    connect(m_devCtl.data(),&UsbDev::DevCtl::newProfile,this,[&](){updateDevInfo("Profile updated successfully.");m_isProfileUpdate=true;if(m_isProfileUpdate/*&&m_isConfigUpdated*/){setIsDeviceReady(true);}});
//    connect(m_devCtl.data(),&UsbDev::DevCtl::newConfig,this,[&](){updateDevInfo("Config updated successfully.");m_isConfigUpdated=true;if(m_isProfileUpdate/*&&m_isConfigUpdated*/){setIsDeviceReady(true);}});

//    do{
//        m_devCtl.reset(UsbDev::DevCtl::createInstance(vid_pid));
//        {
//            static QElapsedTimer elapsedTimer;
//            elapsedTimer.restart();
//            do{
//                QCoreApplication::processEvents();
//            }while((elapsedTimer.elapsed()<3000));      //等待连接
//        }
//        if(m_devCtl->workStatus()==UsbDev::DevCtl::WorkStatus::WorkStatus_S_OK)
//        {
//            updateDevInfo("Connect Successfully.");
//            connect(m_devCtl.data(),&UsbDev::DevCtl::workStatusChanged,this,&DeviceOperation::workOnWorkStatusChanged);
//            connect(m_devCtl.data(),&UsbDev::DevCtl::updateInfo,this,&DeviceOperation::updateDevInfo);
//            connect(m_devCtl.data(),&UsbDev::DevCtl::newStatusData,this,&DeviceOperation::workOnNewStatuData);
//            connect(m_devCtl.data(),&UsbDev::DevCtl::newFrameData,this,&DeviceOperation::workOnNewFrameData);
//            connect(m_devCtl.data(),&UsbDev::DevCtl::newProfile,this,[&](){updateDevInfo("Profile updated successfully.");m_isProfileUpdate=true;});
//            connect(m_devCtl.data(),&UsbDev::DevCtl::newConfig,this,[&](){updateDevInfo("Config updated successfully.");m_isConfigUpdated=true;});
//            m_isDeviceReady=true;
//            emit devConStatusChanged();
//        }

//        else
//        {
//            static QElapsedTimer elapsedTimer;
//            static int retryTimes=0;
//            updateDevInfo("Reconnecting...");
//            elapsedTimer.restart();
//            do{
//                QCoreApplication::processEvents();
//            }while((elapsedTimer.elapsed()<3000));
//            qDebug()<<retryTimes;
//        }
    //    }while(m_devCtl==nullptr||m_devCtl->workStatus()!=UsbDev::DevCtl::WorkStatus::WorkStatus_S_OK);
}

void DeviceOperation::disconnectDev()
{
//    disconnect(m_devCtl.data(),&UsbDev::DevCtl::workStatusChanged,this,&DeviceOperation::workOnWorkStatusChanged);
    disconnect(m_devCtl.data(),&UsbDev::DevCtl::updateInfo,this,&DeviceOperation::updateDevInfo);
    disconnect(m_devCtl.data(),&UsbDev::DevCtl::newStatusData,this,&DeviceOperation::workOnNewStatuData);
    disconnect(m_devCtl.data(),&UsbDev::DevCtl::newFrameData,this,&DeviceOperation::workOnNewFrameData);
//    disconnect(m_devCtl.data(),&UsbDev::DevCtl::newProfile,this,[&](){updateDevInfo("Profile updated successfully.");m_isProfileUpdate=true;});
//    disconnect(m_devCtl.data(),&UsbDev::DevCtl::newConfig,this,[&](){updateDevInfo("Config updated successfully.");m_isConfigUpdated=true;});
    m_devCtl.reset(nullptr);
    setIsDeviceReady(false);
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
    bool isMotorMove[5]{true,true,true,true,true};
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
    UsbDev::Config config;
    config=m_config;
//    qDebug()<<DB;
//    qDebug()<<m_config.DbPosMappingPtr()[0][0];
//    qDebug()<<m_config.DbPosMappingPtr()[DB][0];
//    qDebug()<<config.DbPosMappingPtr()[0][0];
//    qDebug()<<config.DbPosMappingPtr()[DB][0];
//    auto config=m_devCtl->config();
    int motorPos[5];
    motorPos[0]=coordSpacePosInfo.motorX;
    motorPos[1]=coordSpacePosInfo.motorY;
    motorPos[2]=focalMotorPos;
    motorPos[3]=config.DbPosMappingPtr()[DB][0];
    motorPos[4]=config.DbPosMappingPtr()[DB][1];

//    qDebug()<<motorPos[0];
//    qDebug()<<motorPos[1];
//    qDebug()<<motorPos[2];
//    qDebug()<<motorPos[3];
//    qDebug()<<motorPos[4];
//    motorPos[0]=99358;
//    motorPos[1]=88903;
//    motorPos[2]=94200;
//    motorPos[3]=110000;
//    motorPos[4]=110000;
    bool isMotorMove[5]{true,true,true,true,true};
    waitMotorStop({UsbDev::DevCtl::MotorId_Color,
                   UsbDev::DevCtl::MotorId_Light_Spot,
                   UsbDev::DevCtl::MotorId_Focus,
                   UsbDev::DevCtl::MotorId_X,
                   UsbDev::DevCtl::MotorId_Y
                   });
    while(m_shutterElapsedTimer.elapsed()<=m_shutterElapsedTime+100)  //增加100 防止延迟
    {
        QCoreApplication::processEvents();
    }
    move5Motors(isMotorMove,motorPos);
//    quint8 sps[5]={1,1,1,1,1};
//    m_devCtl->move5Motors(sps,motorPos);
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
//    if(!m_frameData.isEmpty())
//        return m_frameData.rawData();
//    else return QByteArray();


    return QByteArray();
}

void DeviceOperation::openShutter(int durationTime)
{
//    auto config=m_devCtl->config();
    auto shutterPos=m_config.shutterOpenPosRef();
    waitMotorStop({UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Light_Spot,UsbDev::DevCtl::MotorId_Focus,UsbDev::DevCtl::MotorId_X,UsbDev::DevCtl::MotorId_Y,UsbDev::DevCtl::MotorId_Shutter});
    m_devCtl->openShutter(durationTime,shutterPos);
    m_shutterElapsedTimer.restart();
    m_shutterElapsedTime=durationTime;
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
    //次
    auto getBusy=[&]()->bool
    {
//        m_statusLock.lock();
//        qDebug()<<"waitMotorStop:"+QString::number(int(thread()->currentThread()),16);
        for(auto& motorId:motorIDs)
        {
            if(m_statusData.isMotorBusy(motorId))
            {
//                m_statusLock.unlock();
                return true;
            }
        }
//        m_statusLock.unlock();
        return false;
    };
    QElapsedTimer mstimer;
    mstimer.restart();//必须先等一会儿刷新状态
    do
    {
        QCoreApplication::processEvents();
    }while(getBusy()||(mstimer.elapsed()<50)); //50ms
    //    while(getBusy()){QCoreApplication::processEvents();}
}

void DeviceOperation::waitForSomeTime(int time)
{
    QElapsedTimer mstimer;
    mstimer.restart();
    do
    {
        QCoreApplication::processEvents();
    }while(mstimer.elapsed()<time);
}

void DeviceOperation::moveChin(ChinMoveDirection direction)
{
    auto profile=m_devCtl->profile();
    auto spsConfig=DeviceSettings::getSingleton()->m_motorChinSpeed;

    quint8 sps[2]{0,0};
    int motorPos[2]{0,0};
    switch(direction)
    {
    case ChinMoveDirection::Left:
    {
        sps[0]=spsConfig[0];
        motorPos[0]=profile.motorRange(UsbDev::DevCtl::MotorId_Chin_Hoz).first;
        m_devCtl->moveChinMotors(sps,motorPos);
        break;
    }
    case ChinMoveDirection::Right:
    {
        sps[0]=spsConfig[0];
        motorPos[0]=profile.motorRange(UsbDev::DevCtl::MotorId_Chin_Hoz).second;
        m_devCtl->moveChinMotors(sps,motorPos);
        break;
    }
    case ChinMoveDirection::Up:
    {
        sps[1]=spsConfig[1];
        motorPos[1]=profile.motorRange(UsbDev::DevCtl::MotorId_Chin_Vert).first;
        m_devCtl->moveChinMotors(sps,motorPos);
        break;
    }
    case ChinMoveDirection::Down:
    {
        sps[1]=spsConfig[1];
        motorPos[1]=profile.motorRange(UsbDev::DevCtl::MotorId_Chin_Vert).second;
        m_devCtl->moveChinMotors(sps,motorPos);
        break;
    }

    case ChinMoveDirection::Stop:
    {
        m_devCtl->moveChinMotors(std::array<quint8,2>{1,1}.data(),std::array<qint32,2>{0,0}.data(),UsbDev::DevCtl::MoveMethod::Relative);
        break;
    }
    }
}


void DeviceOperation::workOnNewStatuData()
{
//    //主
//    m_statusLock.lock();
    m_statusData=m_devCtl->takeNextPendingStatusData();
    auto eyeglassStatus=m_statusData.eyeglassStatus();
    if(m_isChecking)
    {
        setLamp(LampId::LampId_eyeglassInfrared,0,eyeglassStatus);
        setLamp(LampId::LampId_borderInfrared,0,!eyeglassStatus);
    }
    else
    {
        setLamp(LampId::LampId_eyeglassInfrared,0,false);
        setLamp(LampId::LampId_borderInfrared,0,false);
    }
//    m_statusLock.unlock();
//    static int count=1;
//    count++;
//    if(count%50==0)
//    {
//        qDebug()<<"workOnNewStatusData:"+QString::number(int(thread()->currentThread()),16);
//    }
    emit newStatusData();
}

void DeviceOperation::workOnNewFrameData()
{
    m_frameData=m_devCtl->takeNextPendingFrameData();
    m_frameRawData=m_frameData.rawData();
    qDebug()<<"work on new Frame."+QString::number(m_frameRawData.size());
    emit newFrameData();
    return;
    auto profile=m_profile;
    bool valid;
    auto vc=DeviceDataProcesser::caculatePupilDeviation(m_frameData.rawData(),profile.videoSize().width(),profile.videoSize().height(),valid);
    if(!valid){ return;}
    auto centerPoint=vc[0];
//    auto deviationPix=sqrt(pow(centerPoint.x(),2)+pow(centerPoint.y(),2));
    auto step=DeviceSettings::getSingleton()->m_pupilAutoAlignStep;
    m_deviation=DeviceDataProcesser::caculateFixationDeviation(centerPoint);
    if(m_autoAlignPupil)                //自动对眼位
    {
        int tolerance=DeviceSettings::getSingleton()->m_pupilAutoAlignPixelTolerance;
        auto spsConfig=DeviceSettings::getSingleton()->m_motorChinSpeed;
        quint8 sps[2]{spsConfig[0],spsConfig[1]};
        int motorPos[2]{0};
        if(centerPoint.x()>tolerance)
        {
            motorPos[0]=step;
        }
        if(centerPoint.x()<-tolerance)
        {
            motorPos[0]=-step;
        }
        if(centerPoint.y()>tolerance)
        {
            motorPos[1]=step;
        }
        if(centerPoint.y()<-tolerance)
        {
            motorPos[1]=-step;
        }
        m_devCtl->moveChinMotors(sps,motorPos,UsbDev::DevCtl::MoveMethod::Relative);

    }
    if(m_pupilDiameter<0)
    {
        auto pupilDiameter=DeviceDataProcesser::caculatePupilDiameter(vc[1],vc[2]);
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

void DeviceOperation::workOnNewProfile()
{
    m_profile=m_devCtl->profile();

    if(!m_profile.isEmpty()/*&&!m_config.isEmpty()*/){ waitForSomeTime(2000);setIsDeviceReady(true);}
    m_videoSize=m_profile.videoSize();
}

void DeviceOperation::workOnNewConfig()
{
    m_config=m_devCtl->config();
//    if(!m_profile.isEmpty()/*&&m_config.isEmpty()*/){setIsDeviceReady(true);}
}

//void DeviceOperation::workOnWorkStatusChanged()
//{
//    if(m_devCtl->workStatus()==UsbDev::DevCtl::WorkStatus::WorkStatus_S_Disconnected)
//    {
//        connectDev();
//    }
//    emit workStatusChanged();
//}
}

