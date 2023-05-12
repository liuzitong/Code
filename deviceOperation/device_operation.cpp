#include "device_settings.h"
#include "device_operation.h"
#include "device_data_processer.h"
#include <QObject>
#include <qobject.h>
#include <QtCore>
#include <array>
#include <QtMath>
#include <QApplication>
#include <thread>
#include <QtConcurrent/QtConcurrent>
#include <QtGlobal>
#include <iostream>

//#include <QQmlEngine>

#pragma execution_character_set("utf-8")
namespace DevOps{

QSharedPointer<DeviceOperation> DeviceOperation::m_singleton=nullptr;

DeviceOperation::DeviceOperation()
{
    m_config=DeviceData::getSingleton()->m_config;
    m_connectTimer.setInterval(120000);
    connect(&m_connectTimer,&QTimer::timeout,this,&DeviceOperation::connectOrdisConnectDev);
    m_connectTimer.start();
    connect(this,&DeviceOperation::updateDevInfo,[](QString str){std::cout<<str.toStdString()+"\n";});
    m_workStatusElapsedTimer.start();
}


DeviceOperation::~DeviceOperation()
{
    m_connectTimer.stop();
}

//被checkSvcWorker 调用
void DeviceOperation::connectDev()
{
    m_status={-1,-1};
    m_connectDev=true;
    connectOrdisConnectDev();
}

void DeviceOperation::disconnectDev()
{
    m_connectDev=false;
}

void DeviceOperation::connectOrdisConnectDev()
{
    if(m_connectDev&&m_devCtl==nullptr)
    {
        updateDevInfo("connecting.\n");
        auto deviceSettings=DeviceSettings::getSingleton();
        quint32 vid_pid=deviceSettings->m_VID.toInt(nullptr,16)<<16|deviceSettings->m_PID.toInt(nullptr,16);
        m_devCtl.reset(UsbDev::DevCtl::createInstance(vid_pid));
        connect(m_devCtl.data(),&UsbDev::DevCtl::workStatusChanged,this,&DeviceOperation::workOnWorkStatusChanged);
        connect(m_devCtl.data(),&UsbDev::DevCtl::updateInfo,this,&DeviceOperation::updateDevInfo);
//        connect(m_devCtl.data(),&UsbDev::DevCtl::updateIOInfo,this,&DeviceOperation::updateDevInfo);
        connect(m_devCtl.data(),&UsbDev::DevCtl::newStatusData,this,&DeviceOperation::workOnNewStatuData);
        connect(m_devCtl.data(),&UsbDev::DevCtl::newFrameData,this,&DeviceOperation::workOnNewFrameData);
        connect(m_devCtl.data(),&UsbDev::DevCtl::newProfile,this,&DeviceOperation::workOnNewProfile);
        connect(m_devCtl.data(),&UsbDev::DevCtl::newConfig,this,&DeviceOperation::workOnNewConfig);
        waitForSomeTime(8000);
        if(m_devCtl->workStatus()==UsbDev::DevCtl::WorkStatus::WorkStatus_S_OK)
        {
            qDebug()<<"Connect Successfully.";
            updateDevInfo("Connect Successfully.");
            adjustCastLight();
        }
        else
        {
            disconnect(m_devCtl.data(),&UsbDev::DevCtl::workStatusChanged,this,&DeviceOperation::workOnWorkStatusChanged);
            disconnect(m_devCtl.data(),&UsbDev::DevCtl::updateInfo,this,&DeviceOperation::updateDevInfo);
            disconnect(m_devCtl.data(),&UsbDev::DevCtl::newStatusData,this,&DeviceOperation::workOnNewStatuData);
            disconnect(m_devCtl.data(),&UsbDev::DevCtl::newFrameData,this,&DeviceOperation::workOnNewFrameData);
            disconnect(m_devCtl.data(),&UsbDev::DevCtl::newProfile,this,&DeviceOperation::workOnNewProfile);
            disconnect(m_devCtl.data(),&UsbDev::DevCtl::newConfig,this,&DeviceOperation::workOnNewConfig);
            m_devCtl.reset(nullptr);
        }
    }
    if(!m_connectDev&&m_devCtl!=nullptr)
    {
        qDebug()<<"stopConnect";
        disconnect(m_devCtl.data(),&UsbDev::DevCtl::workStatusChanged,this,&DeviceOperation::workOnWorkStatusChanged);
        disconnect(m_devCtl.data(),&UsbDev::DevCtl::updateInfo,this,&DeviceOperation::updateDevInfo);
        disconnect(m_devCtl.data(),&UsbDev::DevCtl::newStatusData,this,&DeviceOperation::workOnNewStatuData);
        disconnect(m_devCtl.data(),&UsbDev::DevCtl::newFrameData,this,&DeviceOperation::workOnNewFrameData);
        disconnect(m_devCtl.data(),&UsbDev::DevCtl::newProfile,this,&DeviceOperation::workOnNewProfile);
        disconnect(m_devCtl.data(),&UsbDev::DevCtl::newConfig,this,&DeviceOperation::workOnNewConfig);
        m_devCtl.reset(nullptr);
        setIsDeviceReady(false);
        m_connectDev=false;
    }
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
    if(!m_isDeviceReady) return;
    if(m_status.color==color&&m_status.spot==spot) return;             //变换到改变光斑颜色位置
    std::cout<<"setCursorColorAndCursorSize"<<std::endl;
    auto profile=m_profile;
    auto config=m_config;
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


    int colorPos=config.switchColorMotorPosPtr()[colorSlot];
    int sizePos=config.switchLightSpotMotorPosPtr()[spotSlot];
    int  color_Circl_Motor_Steps=profile.motorRange(UsbDev::DevCtl::MotorId_Color).second-profile.motorRange(UsbDev::DevCtl::MotorId_Color).first;
    int  spot_Circl_Motor_Steps=profile.motorRange(UsbDev::DevCtl::MotorId_Light_Spot).second-profile.motorRange(UsbDev::DevCtl::MotorId_Light_Spot).first;
    {
        int focalPos=config.focusPosForSpotAndColorChangeRef();
        int motorPos[5]{0,0,focalPos,m_statusData.motorPosition(UsbDev::DevCtl::MotorId::MotorId_Color)+color_Circl_Motor_Steps*10,m_statusData.motorPosition(UsbDev::DevCtl::MotorId::MotorId_Light_Spot)+spot_Circl_Motor_Steps*10};
        waitMotorStop({UsbDev::DevCtl::MotorId_Focus,UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Light_Spot});
         //移动焦距电机电机到联动位置,一边转动颜色和光斑
        m_devCtl->move5Motors(std::array<quint8, 5>{0,0,sps[2],1,1}.data(),motorPos);
        //焦距电机停止就停止,颜色和光斑
        waitMotorStop({UsbDev::DevCtl::MotorId_Focus});
        m_devCtl->move5Motors(std::array<quint8, 5>{1,1,1,1,1}.data(),std::array<int, 5>{0,0,0,0,0}.data(),UsbDev::DevCtl::MoveMethod::Relative);
    }
    {
        //移动光斑和颜色电机到目标位置
        int motorPos[5]{0,0,0,colorPos,sizePos}; //单个电机绝对位置不可行
        quint8 speed[5]{0,0,0,sps[3],sps[4]};
        waitMotorStop({UsbDev::DevCtl::MotorId_Light_Spot,UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Focus});
        m_devCtl->move5Motors(speed,motorPos);
    }

    {
         //转一圈拖动光斑和颜色
        int  motorPos[5]{0,0,0,color_Circl_Motor_Steps,spot_Circl_Motor_Steps};
        quint8 speed[5]{0,0,0,sps[3],sps[4]};
        waitMotorStop({UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Light_Spot});
        m_devCtl->move5Motors(speed,motorPos,UsbDev::DevCtl::Relative);
    }
    {
        //脱离防止干扰误差
        int motorPos[5]={0,0,0,-1000,-1000};
        quint8 speed[5]{0,0,0,sps[3],sps[4]};
        waitMotorStop({UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Light_Spot});
        m_devCtl->move5Motors(speed,motorPos,UsbDev::DevCtl::Relative);
    }
    {
         //焦距电机脱离颜色和光斑
        int motorPos[5]={0, 0, 10000, 0 ,0};
        quint8 speed[5]{0,0,sps[2],0,0};
        waitMotorStop({UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Light_Spot,UsbDev::DevCtl::MotorId_Focus});
        m_devCtl->move5Motors(speed,motorPos,UsbDev::DevCtl::Relative);
    }
    waitMotorStop({UsbDev::DevCtl::MotorId_Focus});
    m_status.color=color;
    m_status.spot=spot;
}

void DeviceOperation::setLamp(LampId id, int index, bool onOff)
{
    if(!m_isDeviceReady) return;
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
        case LampId::LampId_castLight:break;                                                //这个是
        }
    }
    m_devCtl->setLamp(UsbDev::DevCtl::LampId(id),index,da);
    waitForSomeTime(10);
}


void DeviceOperation::setWhiteLamp(bool onOff)
{
    if(!m_isDeviceReady) return;
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

bool DeviceOperation::getDynamicMoveStatus()
{
    return m_statusData.moveStutas();
}

QPointF DeviceOperation::getDyanmicAnswerPos()
{
    if(!m_isDeviceReady)
    {
        return m_lastDynamicCoordAndXYMotorPos[qrand()%m_lastDynamicCoordAndXYMotorPos.size()].first;
    }
    QPointF dot;
    int posX=m_statusData.motorPosition(UsbDev::DevCtl::MotorId::MotorId_X);
    int posY=m_statusData.motorPosition(UsbDev::DevCtl::MotorId::MotorId_Y);
    int nearestDist=INT32_MAX;
    for(auto&i:m_lastDynamicCoordAndXYMotorPos)
    {
        auto dist=pow(i.second.x()-posX,2)+pow(i.second.y()-posY,2);
        if(dist<nearestDist)
        {
            nearestDist=dist;
            dot=i.first;
        }
    }
    return dot;
}

bool DeviceOperation::getMotorsBusy(QVector<UsbDev::DevCtl::MotorId> motorIDs)
{
//        m_statusLock.lock();
    for(auto& motorId:motorIDs)
    {
        if(m_statusData.isMotorBusy(motorId))
        {
//                m_statusLock.unlock();
            return true;
        }
    }
    return false;
}

void DeviceOperation::setDB(int DB)
{
    if(!m_isDeviceReady) return;
//    if(m_status.DB==DB) return;
//    auto config=m_devCtl->config();
    UsbDev::Config config;
    config=m_config;
    quint8 sps[5]{0};
    int motorPos[5]{0};
    motorPos[3]=config.DbPosMappingPtr()[DB][0];
    motorPos[4]=config.DbPosMappingPtr()[DB][1];
    auto spsConfig=DeviceSettings::getSingleton()->m_5MotorSpeed;
    sps[3]=spsConfig[3];sps[4]=spsConfig[4];
    waitMotorStop({UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Light_Spot});
    m_devCtl->move5Motors(sps,motorPos);
//    m_status.DB=DB;
}


void DeviceOperation::moveToAdjustLight(int motorPosX,int motorPosY,int motorPosFocal)
{
    std::cout<<"move to adjustlight";
    int motorPos[5];
    motorPos[0]=motorPosX;
    motorPos[1]=motorPosY;
    motorPos[2]=motorPosFocal;
    motorPos[3]=m_config.DbPosMappingPtr()[0][0];
    motorPos[4]=m_config.DbPosMappingPtr()[0][1];

    bool isMotorMove[5]{true,true,true,true,true};
    waitMotorStop({UsbDev::DevCtl::MotorId_Color,
                   UsbDev::DevCtl::MotorId_Light_Spot,
                   UsbDev::DevCtl::MotorId_Focus,
                   UsbDev::DevCtl::MotorId_X,
                   UsbDev::DevCtl::MotorId_Y
                   });
    move5Motors(isMotorMove,motorPos);
}


void DeviceOperation::getReadyToStimulate(QPointF loc, int spotSize, int DB,bool isMainDotInfoTable)
{
    m_isMainTable=isMainDotInfoTable;
    if(!m_isDeviceReady) return;
    auto coordSpacePosInfo=DeviceDataProcesser::getXYMotorPosAndFocalDistFromCoord(loc,isMainDotInfoTable);
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

    int motorPos[5];
    motorPos[0]=coordSpacePosInfo.motorX;
    motorPos[1]=coordSpacePosInfo.motorY;
    motorPos[2]=focalMotorPos;
    motorPos[3]=config.DbPosMappingPtr()[DB][0];
    motorPos[4]=config.DbPosMappingPtr()[DB][1];

    bool isMotorMove[5]{true,true,true,true,true};
//    waitMotorStop({UsbDev::DevCtl::MotorId_Color,
//                   UsbDev::DevCtl::MotorId_Light_Spot,
//                   UsbDev::DevCtl::MotorId_Focus,
//                   UsbDev::DevCtl::MotorId_X,
//                   UsbDev::DevCtl::MotorId_Y
//                   });

//    std::cout<<"waiting shutter close"<<std::endl;
//    std::cout<<m_statusData.motorPosition(UsbDev::DevCtl::MotorId_Shutter)<<std::endl;;
//    std::cout<<m_config.shutterOpenPosRef()<<std::endl;
    waitForSomeTime(180);
    while(qAbs(m_statusData.motorPosition(UsbDev::DevCtl::MotorId_Shutter)-m_config.shutterOpenPosRef())<20)
    {
        QCoreApplication::processEvents();
    }
//    std::cout<<"done waiting"<<std::endl;
    move5Motors(isMotorMove,motorPos);
}



void DeviceOperation::adjustCastLight()
{
    std::cout<<"adjustCastLightStart"<<std::endl;
    int color=DeviceSettings::getSingleton()->m_castLightTargetColor;
    int size=DeviceSettings::getSingleton()->m_castLightTargetSize;
    setCursorColorAndCursorSize(color,size);
    moveToAdjustLight(m_config.xMotorPosForLightCorrectionRef(),m_config.yMotorPosForLightCorrectionRef(),m_config.focalLengthMotorPosForLightCorrectionRef());
    waitMotorStop({UsbDev::DevCtl::MotorId_Color,
                   UsbDev::DevCtl::MotorId_Light_Spot,
                   UsbDev::DevCtl::MotorId_Focus,
                   UsbDev::DevCtl::MotorId_X,
                   UsbDev::DevCtl::MotorId_Y
                   });
    waitForSomeTime(1000);
    openShutter(65535);
    m_currentCastLightDA=m_config.castLightADPresetRef();
    m_devCtl->setLamp(LampId::LampId_castLight,0,m_currentCastLightDA);
    waitForSomeTime(1000);
    m_castLightAdjustStatus=2;
    m_castLightAdjustElapsedTimer.start();
}


void DeviceOperation::dynamicStimulate(QPointF begin, QPointF end, int cursorSize,int speedLevel,bool isMainDotInfoTable)
{
    m_isMainTable=isMainDotInfoTable;
    auto spotSizeToSlot=DeviceSettings::getSingleton()->m_spotSizeToSlot;
    int spotSlot;
    for(auto&i:spotSizeToSlot)
    {
        if(i.first==cursorSize)
            spotSlot=i.second;
    }

    auto beginPos=DeviceDataProcesser::getXYMotorPosAndFocalDistFromCoord(begin,isMainDotInfoTable);
    auto focalMotorPos=DeviceDataProcesser::getFocusMotorPosByDist(beginPos.focalDist,spotSlot);
    bool isMotorMove[5]{true,true,true,false,false};
    int motorPos[5];
    motorPos[0]=beginPos.motorX;
    motorPos[1]=beginPos.motorY;
    motorPos[2]=focalMotorPos;
    waitMotorStop({UsbDev::DevCtl::MotorId_Color,
                   UsbDev::DevCtl::MotorId_Light_Spot,
                   UsbDev::DevCtl::MotorId_Focus,
                   UsbDev::DevCtl::MotorId_X,
                   UsbDev::DevCtl::MotorId_Y
                   });
    move5Motors(isMotorMove,motorPos);
    auto data=DeviceData::getSingleton()->m_localTableData.m_dynamicLenAndTimeData;
    auto stepLength=data(speedLevel,0)*0.01;
    auto stepTime=data(speedLevel,1);

    float stepLengthX,stepLengthY;
    float distX=end.x()-begin.x();
    float distY=end.y()-begin.y();
    int stepCount;

    if(std::abs(distX)>std::abs(distY))
    {
        distX>0?stepLengthX=stepLength:stepLengthX=-stepLength;
        stepCount=qCeil(distX/stepLengthX);
    }
    else
    {
        distY>0?stepLengthY=stepLength:stepLengthY=-stepLength;
        stepCount=qCeil(distY/stepLengthY);
    }

    stepLengthX=distX/stepCount;
    stepLengthY=distY/stepCount;

    int* dotArr=new int[stepCount*3];
    QPointF coordSpacePosInfoTemp=begin;
    CoordMotorPosFocalDistInfo coordMotorPosFocalDistInfoTemp;
    qDebug()<<"start:"<<begin;
    qDebug()<<"end:"<<end;
    qDebug()<<QString("分割为%1个点,X步长为%2,Y步长为%3.").arg(QString::number(stepCount)).arg(QString::number(stepLengthX)).arg(QString::number(stepLengthY));

    m_lastDynamicCoordAndXYMotorPos.resize(stepCount);


    for(int i=0;i<stepCount;i++)
    {
        coordSpacePosInfoTemp.rx()+=stepLengthX;
        coordSpacePosInfoTemp.ry()+=stepLengthY;
        coordMotorPosFocalDistInfoTemp=DeviceDataProcesser::getXYMotorPosAndFocalDistFromCoord(coordSpacePosInfoTemp,isMainDotInfoTable);
        dotArr[i*3+0]=coordMotorPosFocalDistInfoTemp.motorX;
        dotArr[i*3+1]=coordMotorPosFocalDistInfoTemp.motorY;
        dotArr[i*3+2]=DeviceDataProcesser::getFocusMotorPosByDist(coordMotorPosFocalDistInfoTemp.focalDist,spotSlot);
//        qDebug()<<QString("第%1个点,X坐标:%2,Y坐标:%3,X电机坐标%4,Y电机坐标%5,焦距电机坐标%6.")
//                   .arg(QString::number(i)).arg(QString::number( coordSpacePosInfoTemp.x())).arg(QString::number(coordSpacePosInfoTemp.y())).
//                    arg(QString::number( dotArr[i*3+0])).arg(QString::number( dotArr[i*3+1])).arg(QString::number( dotArr[i*3+2]));
        m_lastDynamicCoordAndXYMotorPos[i]={coordSpacePosInfoTemp,{coordMotorPosFocalDistInfoTemp.motorX,coordMotorPosFocalDistInfoTemp.motorY}};

    }





    qDebug()<<("发送移动数据");
    constexpr int maxPackageLen=512;
    constexpr int stepPerFrame=(maxPackageLen-8)/(4*3);
    int totalframe=ceil((float)stepCount/stepPerFrame);
    for(int i=0;i<totalframe-1;i++)
    {
        if(m_isDeviceReady)
            m_devCtl->sendDynamicData(totalframe,i,512,&dotArr[stepPerFrame*3*i]);                        //一般帧
    }

    int remainStep=stepCount-stepPerFrame*(totalframe-1);
    int dataLen= remainStep*3*4+8;
    constexpr int openForever=65535;
//    qDebug()<<totalframe;
//    qDebug()<<remainStep;
//    qDebug()<<dataLen;
//    qDebug()<<stepPerFrame*3*(totalframe-1);
    if(m_isDeviceReady)
        m_devCtl->sendDynamicData(totalframe,totalframe-1,dataLen,&dotArr[stepPerFrame*3*(totalframe-1)]);     //最后一帧
    waitForSomeTime(1000);
    qDebug()<<("开始移动");
    auto config=DeviceData::getSingleton()->m_config;
    if(m_isDeviceReady)
    {
        waitMotorStop({UsbDev::DevCtl::MotorId_Color,
                       UsbDev::DevCtl::MotorId_Light_Spot,
                       UsbDev::DevCtl::MotorId_Focus,
                       UsbDev::DevCtl::MotorId_X,
                       UsbDev::DevCtl::MotorId_Y
                       });
        openShutter(openForever);
        m_devCtl->startDynamic(speedLevel,speedLevel,speedLevel,stepTime,stepCount);    //开始
    }
//    thread()->sleep(12);
//    qDebug()<<"sleep is over";
    delete[] dotArr;
}

void DeviceOperation::stopDynamic()
{
    if(!m_isDeviceReady) return;
    std::cout<<"stop Dynamic"<<std::endl;
    m_devCtl->stopDyanmic();
}

void DeviceOperation::openShutter(int durationTime)
{
    if(!m_isDeviceReady) return;
    auto shutterPos=m_config.shutterOpenPosRef();
    std::cout<<"openShutter"<<std::endl;

    m_devCtl->openShutter(durationTime,shutterPos);
//    while(qAbs(m_statusData.motorPosition(UsbDev::DevCtl::MotorId_Shutter)-m_config.shutterOpenPosRef())<10)
//    {

//    }
//    m_shutterElapsedTimer.restart();
//    m_shutterElapsedTime=durationTime;
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
    if(m_isDeviceReady)
        m_devCtl->move5Motors(sps,MotorPoses);
}

void DeviceOperation::waitMotorStop(QVector<UsbDev::DevCtl::MotorId> motorIDs)
{
    if(!m_isDeviceReady)
        return;
    QElapsedTimer mstimer;
    mstimer.restart();//必须先等一会儿刷新状态
    do
    {
        QApplication::processEvents();
    }while(getMotorsBusy(motorIDs)||(mstimer.elapsed()<100));
}

void DeviceOperation::waitForSomeTime(int time)
{
    QElapsedTimer mstimer;
    mstimer.restart();
    do
    {

        QApplication::processEvents();
    }while(mstimer.elapsed()<time);
}

void DeviceOperation::moveChin(ChinMoveDirection direction)
{
    if(!m_isDeviceReady) return;
    auto profile=m_devCtl->profile();
    auto spsConfig=DeviceSettings::getSingleton()->m_motorChinSpeed;

    quint8 sps[2]{0,0};
    int motorPos[2]{0,0};
    switch(direction)
    {
    case ChinMoveDirection::Left:
    {
        sps[0]=spsConfig[0];
        motorPos[0]=profile.motorRange(UsbDev::DevCtl::MotorId_Chin_Hoz).second;
        m_devCtl->moveChinMotors(sps,motorPos);
        break;
    }
    case ChinMoveDirection::Right:
    {
        sps[0]=spsConfig[0];
        motorPos[0]=profile.motorRange(UsbDev::DevCtl::MotorId_Chin_Hoz).first;
        m_devCtl->moveChinMotors(sps,motorPos);
        break;
    }
    case ChinMoveDirection::Up:
    {
        sps[1]=spsConfig[1];
        motorPos[1]=profile.motorRange(UsbDev::DevCtl::MotorId_Chin_Vert).second;
        m_devCtl->moveChinMotors(sps,motorPos);
        break;
    }
    case ChinMoveDirection::Down:
    {
        sps[1]=spsConfig[1];
        motorPos[1]=profile.motorRange(UsbDev::DevCtl::MotorId_Chin_Vert).first;
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

void DeviceOperation::lightUpCastLight()
{
    if(m_isDeviceReady&&!m_castLightUp)
    {
        m_devCtl->setLamp(LampId::LampId_castLight,0,m_currentCastLightDA);
        m_castLightUp=true;
    }
}

void DeviceOperation::dimDownCastLight()
{
    if(m_isDeviceReady&&m_castLightUp)
    {
        m_devCtl->setLamp(LampId::LampId_castLight,0,m_currentCastLightDA*0.3);
        m_castLightUp=false;
    }
}



void DeviceOperation::workOnNewStatuData()
{

    if(m_workStatusElapsedTimer.elapsed()>=5000)
    {
        m_workStatusElapsedTimer.restart();
        updateDevInfo("receive new workStatus");
    }
    m_statusData=m_devCtl->takeNextPendingStatusData();
//    auto eyeglassStatus=m_statusData.eyeglassStatus();
//    if(m_isChecking)
//    {
//        if(m_eyeglassStatus!=eyeglassStatus||!m_eyeglassIntialize)
//        {
//            m_eyeglassStatus=eyeglassStatus;
//            m_eyeglassIntialize=true;
//            setLamp(LampId::LampId_eyeglassInfrared,0,eyeglassStatus);
//            setLamp(LampId::LampId_borderInfrared,0,!eyeglassStatus);
//        }
//    }



    if(m_statusData.answerpadStatus()&&m_isWaitingForStaticStimulationAnswer)
    {
//        if(m_isStaticCheckPausingTimer.elapsed()>200)
//        {
//            m_isStaticCheckPausing=true;
//        }
        m_staticStimulationAnswer=true;
    }
//    else
//    {
//        m_isStaticCheckPausing=false;
//        m_isStaticCheckPausingTimer.restart();
//    }





    if(m_videoOnOff!=m_statusData.cameraStatus())
        m_devCtl->setFrontVideo(m_videoOnOff);

    if(m_castLightAdjustStatus==2&&m_castLightAdjustElapsedTimer.elapsed()>=200&&m_isDeviceReady)
    {
        updateDevInfo("keep adjust castLightDa");
        int tagetDA=DeviceSettings::getSingleton()->m_castLightTagetDA;
        int DADiff=DeviceSettings::getSingleton()->m_castLightDADifference;
        int step=DeviceSettings::getSingleton()->m_castLightDAChangeStep;
        int currentDA=m_statusData.castLightDA();
        updateDevInfo("castlight Da:"+QString::number(m_currentCastLightDA));
        updateDevInfo("current da:"+QString::number(currentDA));
        if(qAbs(tagetDA-currentDA)>DADiff)
        {
            if(tagetDA>currentDA)
            {
                m_currentCastLightDA+=step;
            }
            else
            {
                m_currentCastLightDA-=step;
            }
            m_devCtl->setLamp(LampId::LampId_castLight,0,m_currentCastLightDA);
            m_castLightAdjustElapsedTimer.restart();
        }
        else
        {
            setCastLightAdjustStatus(3);
            openShutter(0);
            waitMotorStop({UsbDev::DevCtl::MotorId_Shutter});
            m_devCtl->setLamp(LampId::LampId_castLight,0,m_currentCastLightDA*0.3);
            waitForSomeTime(1000);              //防止摄像头刷不出来
        }
    }
    emit newStatusData();
}

void DeviceOperation::workOnNewFrameData()
{
    m_frameData=m_devCtl->takeNextPendingFrameData();
    m_frameRawData=m_frameData.rawData();
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
    qDebug()<<"work on new Profile";
    m_profile=m_devCtl->profile();
    m_videoSize=m_profile.videoSize();
    if(!m_profile.isEmpty()&&!m_config.isEmpty())
    {
        setIsDeviceReady(true);
    }


}

void DeviceOperation::workOnNewConfig()
{
    qDebug()<<"work on new config";
    m_config=m_devCtl->config();
    if(!m_profile.isEmpty()&&!m_config.isEmpty())
    {
        setIsDeviceReady(true);
    }
}



void DeviceOperation::workOnWorkStatusChanged()
{
    if(m_devCtl->workStatus()==UsbDev::DevCtl::WorkStatus::WorkStatus_S_Disconnected)
    {
        setIsDeviceReady(false);
        connectDev();
    }
    emit workStatusChanged();
}
}

