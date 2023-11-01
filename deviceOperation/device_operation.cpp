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
#include <QPainter>
#include <QtGlobal>
#include <iostream>
#include <QtMath>

//#include <QQmlEngine>

#pragma execution_character_set("utf-8")
namespace DevOps{

QSharedPointer<DeviceOperation> DeviceOperation::m_singleton=/*QSharedPointer<DeviceOperation>(new DeviceOperation())*/nullptr;

DeviceOperation::DeviceOperation()
{
    connect(&m_reconnectTimer,&QTimer::timeout,this,&DeviceOperation::reconnectDev);
    connect(this,&DeviceOperation::updateDevInfo,[](QString str){qDebug()<<str;});
//    m_workStatusElapsedTimer.start();
    m_autoPupilElapsedTimer.start();
//    m_reconnectingElapsedTimer.start();
    m_reconnectTimer.setInterval(5000);                            //复位的时候会短暂收不到数据更新，时间不能太短
    m_waitingTime=DeviceSettings::getSingleton()->m_waitingTime;
    m_currentCastLightDA=DeviceSettings::getSingleton()->m_castLightDA;

//    qDebug()<<m_config.deviceIDRef();
//    m_reconnectTimer.start();
}


DeviceOperation::~DeviceOperation()
{

}

//被checkSvcWorker 调用
void DeviceOperation::connectDev()
{
    if(m_devCtl==nullptr)
    {
        updateDevInfo("connecting.");
#ifndef _DEBUG
        m_reconnectTimer.start();
#endif
        auto deviceSettings=DeviceSettings::getSingleton();
        quint32 vid_pid=deviceSettings->m_VID.toInt(nullptr,16)<<16|deviceSettings->m_PID.toInt(nullptr,16);
        m_devCtl.reset(UsbDev::DevCtl::createInstance(vid_pid));
        connect(m_devCtl.data(),&UsbDev::DevCtl::workStatusChanged,this,&DeviceOperation::workOnWorkStatusChanged);
    }
}

void DeviceOperation::disconnectDev()
{
    m_devCtl.reset(nullptr);
    m_status={-1,-1};
//    setIsDeviceReady(false);
    setDeviceStatus(0);
}

void DeviceOperation::reconnectDev()
{
//    if(m_reconnectingElapsedTimer.elapsed()<=10000) return;
//    m_reconnectingElapsedTimer.restart();
    m_eyeglassIntialize=false;
    qDebug()<<"reconnecting";
    m_devCtl.reset(nullptr);
    m_status={-1,-1};
    setDeviceStatus(1);
    connectDev();
}

QSharedPointer<DeviceOperation> DeviceOperation::getSingleton()
{
//    static QMutex mutex;
//    mutex.lock();
//    if(m_singleton==nullptr)
//    {
//        m_singleton.reset(new DeviceOperation());
//    }
//    mutex.unlock();
    return m_singleton;
}

void DeviceOperation::createInstance()
{
    m_singleton.reset(new DeviceOperation());
}

void DeviceOperation::setCursorColorAndCursorSize(int color, int spot)
{
    if(m_deviceStatus!=2) return;
    if(m_status.color==color&&m_status.spot==spot) return;             //变换到改变光斑颜色位置
#ifdef _DEBUG
    std::cout<<"setCursorColorAndCursorSize"<<std::endl;
#endif
    resetMotors({UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Light_Spot});
    waitForSomeTime(m_waitingTime);
    waitMotorStop({{UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Light_Spot}});
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
    qDebug()<<colorPos;
    int sizePos=config.switchLightSpotMotorPosPtr()[spotSlot];
    qDebug()<<sizePos;
    int  color_Circl_Motor_Steps=profile.motorRange(UsbDev::DevCtl::MotorId_Color).second-profile.motorRange(UsbDev::DevCtl::MotorId_Color).first;
    int  spot_Circl_Motor_Steps=profile.motorRange(UsbDev::DevCtl::MotorId_Light_Spot).second-profile.motorRange(UsbDev::DevCtl::MotorId_Light_Spot).first;
    {
        int focalPos=config.focusPosForSpotAndColorChangeRef();
//        int motorPos[5]{0,0,focalPos,color_Circl_Motor_Steps*10,spot_Circl_Motor_Steps*10};
        int motorPos[5]{0,0,focalPos,0,0};
        waitForSomeTime(m_waitingTime);
        waitMotorStop({UsbDev::DevCtl::MotorId_Focus});
         //移动焦距电机电机到联动位置,一边转动颜色和光斑
        m_devCtl->move5Motors(std::array<quint8, 5>{0,0,sps[2],0,0}.data(),motorPos);
//        m_devCtl->move5Motors(std::array<quint8, 5>{0,0,0,1,1}.data(),motorPos,UsbDev::DevCtl::MoveMethod::Relative);                 //取消转动颜色和光斑
        //焦距电机停止就停止,颜色和光斑
        waitForSomeTime(m_waitingTime);
//        waitMotorStop({UsbDev::DevCtl::MotorId_Focus});
//        m_devCtl->move5Motors(std::array<quint8, 5>{1,1,1,1,1}.data(),std::array<int, 5>{0,0,0,0,0}.data(),UsbDev::DevCtl::MoveMethod::Relative);
    }
    {
        //移动光斑和颜色电机到目标位置
        int motorPos[5]{0,0,0,colorPos,sizePos}; //单个电机绝对位置不可行
        quint8 speed[5]{0,0,0,sps[3],sps[4]};
        waitForSomeTime(m_waitingTime);
        waitMotorStop({UsbDev::DevCtl::MotorId_Light_Spot,UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Focus});
        m_devCtl->move5Motors(speed,motorPos);
    }

    {
         //转一圈拖动光斑和颜色
        int  motorPos[5]{0,0,0,color_Circl_Motor_Steps,spot_Circl_Motor_Steps};
        quint8 speed[5]{0,0,0,sps[3],sps[4]};
        waitForSomeTime(m_waitingTime);
        waitMotorStop({UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Light_Spot});
        m_devCtl->move5Motors(speed,motorPos,UsbDev::DevCtl::Relative);
    }
    {
        //脱离防止干扰误差
        int motorPos[5]={0,0,0,-1000,-1000};
        quint8 speed[5]{0,0,0,sps[3],sps[4]};
        waitForSomeTime(m_waitingTime);
        waitMotorStop({UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Light_Spot});
        m_devCtl->move5Motors(speed,motorPos,UsbDev::DevCtl::Relative);
    }
    {
         //焦距电机脱离颜色和光斑
        int motorPos[5]={0, 0, 10000, 0 ,0};
        quint8 speed[5]{0,0,sps[2],0,0};
        waitForSomeTime(m_waitingTime);
        waitMotorStop({UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Light_Spot,UsbDev::DevCtl::MotorId_Focus});
        m_devCtl->move5Motors(speed,motorPos,UsbDev::DevCtl::Relative);
    }
    waitForSomeTime(m_waitingTime);
    waitMotorStop({UsbDev::DevCtl::MotorId_Focus});
    m_status.color=color;
    m_status.spot=spot;
}

void DeviceOperation::setLamp(LampId id, int index, bool onOff)
{
    if(m_deviceStatus!=2) return;
    int da=0;
    if(onOff)
    {
        auto config=m_config;
        switch (id)
        {
        case LampId::LampId_centerFixation:da=config.centerFixationLampDARef();break;
        case LampId::LampId_bigDiamond:da=config.bigDiamondfixationLampDAPtr()[index];break;
        case LampId::LampId_smallDiamond:da=config.smallDiamondFixationLampDAPtr()[index];break;
        case LampId::LampId_yellowBackground:da=config.yellowBackgroundLampDARef();m_backgroundLight=BackgroundLight::yellow;break;
        case LampId::LampId_centerInfrared:da=config.centerFixationLampDARef();break;
        case LampId::LampId_borderInfrared:da=config.borderInfraredLampDARef();break;
        case LampId::LampId_eyeglassInfrared:da=config.eyeglassFrameLampDARef();break;
        case LampId::LampId_castLight:break;                                                //这个是
        }
    }
    m_devCtl->setLamp(UsbDev::DevCtl::LampId(id),index,da);
//    waitForSomeTime(10);
}


void DeviceOperation::setWhiteLamp(bool onOff)
{
    if(m_deviceStatus!=2) return;
//    auto config=DeviceData::getSingleton()->m_config;
    auto whiteLampDaPtr=m_config.whiteBackgroundLampDAPtr();
    if(onOff)
    {
        m_devCtl->setWhiteLamp(whiteLampDaPtr[0],whiteLampDaPtr[1],whiteLampDaPtr[2]);
        m_backgroundLight=BackgroundLight::white;
    }
    else
        m_devCtl->setWhiteLamp(0,0,0);
}

bool DeviceOperation::getAnswerPadStatus()
{
    return m_statusData.answerpadStatus();
}

bool DeviceOperation::getDynamicMoveStatus()
{
    return m_statusData.moveStatus();
}

QPointF DeviceOperation::getDyanmicAnswerPos()
{
    if(m_deviceStatus!=2)
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
    if(m_deviceStatus!=2) return;
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
#ifdef _DEBUG
    std::cout<<"move to adjustlight"<<std::endl;
#endif
    int DB=m_config.DBForLightCorrectionRef();
    int motorPos[5];
    motorPos[0]=motorPosX;
    motorPos[1]=motorPosY;
    motorPos[2]=motorPosFocal;
//    motorPos[3]=m_config.DbPosMappingPtr()[DB][0];
//    motorPos[4]=m_config.DbPosMappingPtr()[DB][1];
    motorPos[3]=m_config.DbPosMappingPtr()[DB][0];
    motorPos[4]=m_config.DbPosMappingPtr()[DB][1];
    qDebug()<<motorPos[3];
    qDebug()<<motorPos[4];

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
    if(m_deviceStatus!=2) return;
    auto coordSpacePosInfo=DeviceDataProcesser::getXYMotorPosAndFocalDistFromCoord(loc,isMainDotInfoTable);
    auto spotSizeToSlot=DeviceSettings::getSingleton()->m_spotSizeToSlot;
    int spotSlot;
    for(auto&i:spotSizeToSlot)
    {
        if(i.first==spotSize)
            spotSlot=i.second;
    }
    auto focalMotorPos=DeviceDataProcesser::getFocusMotorPosByDist(coordSpacePosInfo.focalDist,spotSlot,m_config.focalMotorPosCorrectionRef());
    UsbDev::Config config;
    config=m_config;

    int motorPos[5];
    motorPos[0]=coordSpacePosInfo.motorX;
    motorPos[1]=coordSpacePosInfo.motorY;
    motorPos[2]=focalMotorPos;
    motorPos[3]=config.DbPosMappingPtr()[DB][0];
    motorPos[4]=config.DbPosMappingPtr()[DB][1];
    bool isMotorMove[5]{true,true,true,true,true};
    waitForSomeTime(m_waitingTime);
    while(qAbs(m_statusData.motorPosition(UsbDev::DevCtl::MotorId_Shutter)-m_config.shutterOpenPosRef())<70)
    {
        if(m_deviceStatus!=2) return;
        QApplication::processEvents();
    }
    move5Motors(isMotorMove,motorPos);
    waitForSomeTime(m_waitingTime);
}



void DeviceOperation::adjustCastLight()
{
#ifdef _DEBUG
    std::cout<<"adjustCastLightStart"<<std::endl;
#endif
    int color=DeviceSettings::getSingleton()->m_castLightTargetColor;
    int size=DeviceSettings::getSingleton()->m_castLightTargetSize;
    setCursorColorAndCursorSize(color,size);
    qDebug()<<m_config.xMotorPosForLightCorrectionRef();
    qDebug()<<m_config.yMotorPosForLightCorrectionRef();
    qDebug()<<m_config.focalLengthMotorPosForLightCorrectionRef();
    moveToAdjustLight(m_config.xMotorPosForLightCorrectionRef(),m_config.yMotorPosForLightCorrectionRef(),m_config.focalLengthMotorPosForLightCorrectionRef());
    waitForSomeTime(m_waitingTime);
    waitMotorStop({UsbDev::DevCtl::MotorId_Color,
                   UsbDev::DevCtl::MotorId_Light_Spot,
                   UsbDev::DevCtl::MotorId_Focus,
                   UsbDev::DevCtl::MotorId_X,
                   UsbDev::DevCtl::MotorId_Y
                   });
    waitForSomeTime(m_waitingTime);
    openShutter(65535);
    qDebug()<<m_currentCastLightDA;
    m_devCtl->setLamp(LampId::LampId_castLight,0,m_currentCastLightDA);
    waitForSomeTime(m_waitingTime);
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
    auto focalMotorPos=DeviceDataProcesser::getFocusMotorPosByDist(beginPos.focalDist,spotSlot,m_config.focalMotorPosCorrectionRef());
    bool isMotorMove[5]{true,true,true,false,false};
    int motorPos[5];
    motorPos[0]=beginPos.motorX;
    motorPos[1]=beginPos.motorY;
    motorPos[2]=focalMotorPos;
    waitForSomeTime(m_waitingTime);
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
        dotArr[i*3+2]=DeviceDataProcesser::getFocusMotorPosByDist(coordMotorPosFocalDistInfoTemp.focalDist,spotSlot,m_config.focalMotorPosCorrectionRef());
        m_lastDynamicCoordAndXYMotorPos[i]={coordSpacePosInfoTemp,{coordMotorPosFocalDistInfoTemp.motorX,coordMotorPosFocalDistInfoTemp.motorY}};

    }

    qDebug()<<("发送移动数据");
    constexpr int maxPackageLen=512;
    constexpr int stepPerFrame=(maxPackageLen-8)/(4*3);
    int totalframe=ceil((float)stepCount/stepPerFrame);
    for(int i=0;i<totalframe-1;i++)
    {
        if(m_deviceStatus==2)
            m_devCtl->sendDynamicData(totalframe,i,512,&dotArr[stepPerFrame*3*i]);                        //一般帧
    }

    int remainStep=stepCount-stepPerFrame*(totalframe-1);
    int dataLen= remainStep*3*4+8;
    constexpr int openForever=65535;
//    qDebug()<<totalframe;
//    qDebug()<<remainStep;
//    qDebug()<<dataLen;
//    qDebug()<<stepPerFrame*3*(totalframe-1);
    if(m_deviceStatus==2)
        m_devCtl->sendDynamicData(totalframe,totalframe-1,dataLen,&dotArr[stepPerFrame*3*(totalframe-1)]);     //最后一帧
    waitForSomeTime(1000);
    qDebug()<<("开始移动");
    if(m_deviceStatus==2)
    {
        waitForSomeTime(m_waitingTime);
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
    if(m_deviceStatus!=2) return;
#ifdef _DEBUG
    std::cout<<"stop Dynamic"<<std::endl;
#endif
    m_devCtl->stopDyanmic();
}

void DeviceOperation::openShutter(int durationTime)
{
    if(m_deviceStatus!=2) return;
    auto shutterPos=m_config.shutterOpenPosRef();
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
    if(m_deviceStatus==2)
        m_devCtl->move5Motors(sps,MotorPoses);
}

void DeviceOperation::waitMotorStop(QVector<UsbDev::DevCtl::MotorId> motorIDs)
{
    if(m_deviceStatus!=2) return;
    QElapsedTimer mstimer;
    mstimer.restart();//必须先等一会儿刷新状态
    do
    {
        if(m_deviceStatus!=2) return;
        QApplication::processEvents();
    }while(getMotorsBusy(motorIDs)/*||(mstimer.elapsed()<100)*/);
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
    if(m_deviceStatus!=2) return;
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
    if(m_deviceStatus==2&&!m_castLightUp)
    {
        m_devCtl->setLamp(LampId::LampId_castLight,0,m_currentCastLightDA);
#ifdef _DEBUG
        std::cout<<"cast light Up:"<<m_currentCastLightDA<<std::endl;
#endif
        m_castLightUp=true;
    }
}

void DeviceOperation::dimDownCastLight()
{
    if(m_deviceStatus==2&&m_castLightUp)
    {
        m_devCtl->setLamp(LampId::LampId_castLight,0,m_currentCastLightDA*0.3);
#ifdef _DEBUG
        std::cout<<"cast light Down"<<m_currentCastLightDA*0.3<<std::endl;
#endif
        m_castLightUp=false;
    }
}

void DeviceOperation::resetMotors(QVector<UsbDev::DevCtl::MotorId> motorIDs)
{
    if(m_deviceStatus!=2) return;
    auto spsConfig=DeviceSettings::getSingleton()->m_motorChinSpeed;
    for(auto& motorID:motorIDs)
    {
        int speed=spsConfig[motorID];
        m_devCtl->resetMotor(motorID,speed);
    }
}

void DeviceOperation::beep()
{
    auto devSetting=DeviceSettings::getSingleton();
    if(m_deviceStatus==2)
    {
        m_devCtl->beep(devSetting->m_beepCount,devSetting->m_beepDuration,devSetting->m_beepInterval);
    }
}

void DeviceOperation::alarm()
{
    if(m_deviceStatus==2)
    {
        m_devCtl->beep(5,100,100);
    }
}

void DeviceOperation::clearPupilData()
{
    m_devicePupilProcessor.clearData();
}

void DeviceOperation::workOnNewStatuData()
{
#ifndef _DEBUG
    m_reconnectTimer.start();
#endif
//    if(m_workStatusElapsedTimer.elapsed()>=1000)
//    {
//        m_workStatusElapsedTimer.restart();
//        updateDevInfo("receive new workStatus");
//    }
    m_statusData=m_devCtl->takeNextPendingStatusData();

    using MotorId=UsbDev::DevCtl::MotorId;
    m_statusDataOut.serialNo=m_statusData.serialNO();
    m_statusDataOut.answerpadStatus=m_statusData.answerpadStatus();
    m_statusDataOut.cameraStatus=m_statusData.cameraStatus();
    m_statusDataOut.eyeglassStatus=m_statusData.eyeglassStatus();
    m_statusDataOut.xMotorBusy=m_statusData.isMotorBusy(MotorId::MotorId_X);
    m_statusDataOut.yMotorBusy=m_statusData.isMotorBusy(MotorId::MotorId_Y);
    m_statusDataOut.focusMotorBusy=m_statusData.isMotorBusy(MotorId::MotorId_Focus);
    m_statusDataOut.colorMotorBusy=m_statusData.isMotorBusy(MotorId::MotorId_Color);
    m_statusDataOut.lightSpotMotorBusy=m_statusData.isMotorBusy(MotorId::MotorId_Light_Spot);
    m_statusDataOut.shutterMotorBusy=m_statusData.isMotorBusy(MotorId::MotorId_Shutter);
    m_statusDataOut.xChinMotorBusy=m_statusData.isMotorBusy(MotorId::MotorId_Chin_Hoz);
    m_statusDataOut.yChinMotorBusy=m_statusData.isMotorBusy(MotorId::MotorId_Chin_Vert);
    m_statusDataOut.xMotorCmdCntr=m_statusData.motorCmdCntr(MotorId::MotorId_X);
    m_statusDataOut.yMotorCmdCntr=m_statusData.motorCmdCntr(MotorId::MotorId_Y);
    m_statusDataOut.focusMotorCmdCntr=m_statusData.motorCmdCntr(MotorId::MotorId_Focus);
    m_statusDataOut.colorMotorCmdCntr=m_statusData.motorCmdCntr(MotorId::MotorId_Color);
    m_statusDataOut.lightSpotMotorCmdCntr=m_statusData.motorCmdCntr(MotorId::MotorId_Light_Spot);
    m_statusDataOut.shutterMotorCmdCntr=m_statusData.motorCmdCntr(MotorId::MotorId_Shutter);
    m_statusDataOut.xChinMotorCmdCntr=m_statusData.motorCmdCntr(MotorId::MotorId_Chin_Hoz);
    m_statusDataOut.yChinMotorCmdCntr=m_statusData.motorCmdCntr(MotorId::MotorId_Chin_Vert);
    m_statusDataOut.moveStatus=m_statusData.moveStatus();
    m_statusDataOut.xMotorCurrPos=m_statusData.motorPosition(MotorId::MotorId_X);
    m_statusDataOut.yMotorCurrPos=m_statusData.motorPosition(MotorId::MotorId_Y);
    m_statusDataOut.focusMotorCurrPos=m_statusData.motorPosition(MotorId::MotorId_Focus);
    m_statusDataOut.colorMotorCurrPos=m_statusData.motorPosition(MotorId::MotorId_Color);
    m_statusDataOut.lightSpotMotorCurrPos=m_statusData.motorPosition(MotorId::MotorId_Light_Spot);
    m_statusDataOut.shutterMotorCurrPos=m_statusData.motorPosition(MotorId::MotorId_Shutter);
    m_statusDataOut.xChinMotorCurrPos=m_statusData.motorPosition(MotorId::MotorId_Chin_Hoz);
    m_statusDataOut.yChinMotorCurrPos=m_statusData.motorPosition(MotorId::MotorId_Chin_Vert);
    m_statusDataOut.envLightDA=m_statusData.envLightSensorDA();
    m_statusDataOut.castLightDA=m_statusData.castLightSensorDA();

    auto eyeglassStatus=m_statusData.eyeglassStatus();
    if(m_isAtCheckingPage)
    {
        if(!m_eyeglassIntialize)   //开红外
        {
            setLamp(LampId::LampId_centerInfrared,0,true);
            setLamp(LampId::LampId_eyeglassInfrared,0,eyeglassStatus);
            setLamp(LampId::LampId_borderInfrared,0,!eyeglassStatus);
#ifdef _DEBUG
            std::cout<<"open eyeglassInfrared infrared:"<<eyeglassStatus<<std::endl;
            std::cout<<"open borderInfrared infrared:"<<!eyeglassStatus<<std::endl;
#endif
            m_eyeglassIntialize=true;
            m_eyeglassStatus=eyeglassStatus;

        }
        if(m_eyeglassStatus!=eyeglassStatus)
        {
            m_eyeglassStatus=eyeglassStatus;
            setLamp(LampId::LampId_eyeglassInfrared,0,eyeglassStatus);
            setLamp(LampId::LampId_borderInfrared,0,!eyeglassStatus);
#ifdef _DEBUG
            std::cout<<"open eyeglassInfrared infrared:"<<eyeglassStatus<<std::endl;
            std::cout<<"open borderInfrared infrared:"<<!eyeglassStatus<<std::endl;
#endif
        }


        int envLightAlarmDA;
        if(m_backgroundLight==BackgroundLight::white)
        {
            envLightAlarmDA=m_config.environmentAlarmLightDAPtr()[0];
        }
        else
        {
            envLightAlarmDA=m_config.environmentAlarmLightDAPtr()[1];
        }
        setEnvLightAlarm(m_statusData.envLightSensorDA()>envLightAlarmDA);
    }
    else if(m_eyeglassIntialize)        //关红外
    {
        setLamp(LampId::LampId_centerInfrared,0,false);
        setLamp(LampId::LampId_eyeglassInfrared,0,false);
        setLamp(LampId::LampId_borderInfrared,0,false);
        m_eyeglassIntialize=false;
        std::cout<<"close allInfrared infrared"<<std::endl;
    }

    if(m_statusData.answerpadStatus()&&m_isWaitingForStaticStimulationAnswer)
    {
        m_staticStimulationAnswer=true;
    }

    if(m_isAtCheckingPage!=m_statusData.cameraStatus())
    {
        if(m_isAtCheckingPage)
        {
            m_devCtl->setFrontVideo(false);
            m_devCtl->setFrontVideo(false);
            m_devCtl->setFrontVideo(false);
            m_devCtl->setFrontVideo(true);
        }
        else
            m_devCtl->setFrontVideo(false);

    }

    if(m_castLightAdjustStatus==2&&m_castLightAdjustElapsedTimer.elapsed()>=500&&m_deviceStatus==2)
    {
        updateDevInfo("keep adjust castLightDa");
        int DADiffTolerance=DeviceSettings::getSingleton()->m_castLightDADifferenceTolerance;
        int step=DeviceSettings::getSingleton()->m_castLightDAChangeStep;
        int currentcastLightSensorDA=m_statusData.castLightSensorDA();
        int targetcastLightSensorDA=m_config.castLightSensorDAForLightCorrectionRef();
        updateDevInfo("castlight Da:"+QString::number(m_currentCastLightDA));
        updateDevInfo("current da:"+QString::number(currentcastLightSensorDA));
        if(qAbs(targetcastLightSensorDA-currentcastLightSensorDA)>DADiffTolerance)
        {
            if(targetcastLightSensorDA>currentcastLightSensorDA)
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
            waitForSomeTime(m_waitingTime);
            waitMotorStop({UsbDev::DevCtl::MotorId_Shutter});
            m_devCtl->setLamp(LampId::LampId_castLight,0,m_currentCastLightDA*0.3);

            DeviceSettings::getSingleton()->m_castLightLastAdjustedDate=QDate::currentDate().toString("yyyy/MM/dd");
            DeviceSettings::getSingleton()->m_castLightDA=m_currentCastLightDA;
            DeviceSettings::getSingleton()->saveSettings();
        }
    }
    emit newStatusData();
}

void DeviceOperation::workOnNewFrameData()
{
    m_frameData=m_devCtl->takeNextPendingFrameData();
    m_frameRawDataLock.lock();
    m_frameRawData=m_frameData.rawData();
    m_frameRawDataLock.unlock();
    auto data=m_frameData.rawData();
    m_devicePupilProcessor.processData((uchar*)data.data(),m_videoSize.width(),m_videoSize.height());
//    m_devicePupilProcessor.find_point((uchar*)data.data(),m_videoSize.width(),m_videoSize.height());
    QImage img((uchar*)data.data(),m_videoSize.width(),m_videoSize.height(),QImage::Format_Grayscale8);
    img=img.convertToFormat(QImage::Format_ARGB32);
    if(m_devicePupilProcessor.m_pupilResValid)
    {
        QPainter painter(&img);
        painter.setPen(QPen{Qt::red,2});
        painter.drawEllipse(m_devicePupilProcessor.m_pupilCenterPoint,m_devicePupilProcessor.m_pupilDiameterPix/2,m_devicePupilProcessor.m_pupilDiameterPix/2);
        if(m_devicePupilProcessor.m_reflectionResValid)
        {
            for(auto&i:m_devicePupilProcessor.m_reflectionDot)
            {
                painter.setPen(Qt::green);
                painter.drawEllipse({qRound(i.x()),qRound(i.y())},3,3);
            }
        }
    }
    QByteArray ba=QByteArray((char*)img.bits(),img.byteCount());

    if(m_autoPupilElapsedTimer.elapsed()>=m_autoPupilElapsedTime)
    {
        m_autoPupilElapsedTimer.restart();
        if(m_devicePupilProcessor.m_pupilResValid)
        {
            if(m_autoAlignPupil)                //自动对眼位
            {
                auto step=DeviceSettings::getSingleton()->m_pupilAutoAlignStep;
                int tolerance=DeviceSettings::getSingleton()->m_pupilAutoAlignPixelTolerance;
                auto spsConfig=DeviceSettings::getSingleton()->m_motorChinSpeed;
                quint8 sps[2]{spsConfig[0],spsConfig[1]};
                int motorPos[2]{0};

                QPointF pupilDeviation={m_devicePupilProcessor.m_pupilCenterPoint.x()-0.5*m_videoSize.width(),m_devicePupilProcessor.m_pupilCenterPoint.y()-0.5*m_videoSize.height()};
                if(!m_statusData.isMotorBusy(UsbDev::DevCtl::MotorId_Chin_Hoz)&&qAbs(pupilDeviation.x())>tolerance)
                {
                    motorPos[0]=-pupilDeviation.x()*step;
                    m_devCtl->moveChinMotors(sps,motorPos,UsbDev::DevCtl::MoveMethod::Relative);
                    m_autoPupilElapsedTime=200;
                }
                else
                {
                     m_autoPupilElapsedTime=100;
                }

                if(!m_statusData.isMotorBusy(UsbDev::DevCtl::MotorId_Chin_Vert)&&qAbs(pupilDeviation.y())>tolerance)
                {
                    motorPos[1]=pupilDeviation.y()*step;
                    m_devCtl->moveChinMotors(sps,motorPos,UsbDev::DevCtl::MoveMethod::Relative);
                    m_autoPupilElapsedTime=200;
                }
                else
                {
                    m_autoPupilElapsedTime=100;
                }
            }
        }
    }
    emit newFrameData(ba);
    emit pupilDiameterChanged();
}

void DeviceOperation::workOnNewProfile()
{
    qDebug()<<"work on new Profile";
    m_profile=m_devCtl->profile();
    m_videoSize=m_profile.videoSize();
}

void DeviceOperation::workOnNewConfig()
{
    qDebug()<<"work on new config";
    UsbDev::Config config=m_devCtl->config();
    quint32 crc=DeviceDataProcesser::calcCrc((quint8*)config.dataPtr()+4, config.dataLen()-4);
    if(DeviceData::getSingleton()->m_config.isEmpty())
    {
        QString filePath = R"(./deviceData/config.cfg)";
        QFile file(filePath);
        if(file.open(QIODevice::WriteOnly))
        {
            file.write((char*)config.dataPtr(),config.dataLen());
        }
        m_config=config;
    }
    else
    {
        if(crc==config.crcVeryficationRef())
        {
            m_config=config;
        }
        else
        {
            m_config=DeviceData::getSingleton()->m_config;
        }
    }

    m_devicePupilProcessor.m_pupilGreyLimit=m_config.pupilGreyThresholdDAPtr()[0];
    m_devicePupilProcessor.m_pupilReflectionDotWhiteLimit=m_config.pupilGreyThresholdDAPtr()[1];
    emit newDeviceID(QString(m_config.deviceIDRef()));
//    if(!m_profile.isEmpty()&&!m_config.isEmpty())
//    {
    auto date=QDate::currentDate();
    auto lastAdjustedDate=QDate::fromString(DeviceSettings::getSingleton()->m_castLightLastAdjustedDate,"yyyy/MM/dd");
    bool adjusted=((date.year()==lastAdjustedDate.year())&&(date.month()==lastAdjustedDate.month())&&(date.day()==lastAdjustedDate.day()));
    if(adjusted)
    {
        setCastLightAdjustStatus(3);
        dimDownCastLight();
    }
    else
        adjustCastLight();
//    }
}

void DeviceOperation::workOnWorkStatusChanged(int status)
{
    if(m_devCtl->workStatus()==UsbDev::DevCtl::WorkStatus::WorkStatus_S_ConnectToDev)
    {
        qDebug()<<"in connecting.";
    }
    if(m_devCtl->workStatus()==UsbDev::DevCtl::WorkStatus::WorkStatus_S_OK)
    {
        qDebug()<<"Connect Successfully.";
        updateDevInfo("Connect Successfully.");
//        m_connectTimer.stop();
        connect(m_devCtl.data(),&UsbDev::DevCtl::updateInfo,this,&DeviceOperation::updateDevInfo);
        connect(m_devCtl.data(),&UsbDev::DevCtl::newStatusData,this,&DeviceOperation::workOnNewStatuData);
        connect(m_devCtl.data(),&UsbDev::DevCtl::newFrameData,this,&DeviceOperation::workOnNewFrameData);
        connect(m_devCtl.data(),&UsbDev::DevCtl::newProfile,this,&DeviceOperation::workOnNewProfile);
        connect(m_devCtl.data(),&UsbDev::DevCtl::newConfig,this,&DeviceOperation::workOnNewConfig);

        m_devCtl->readProfile();
        while (m_profile.isEmpty()) {QApplication::processEvents();}
//        waitForSomeTime(500);                                          //等一下保证 config后读

        m_devCtl->readConfig();
//        adjustCastLight();
        setDeviceStatus(2);
//        setIsDeviceReady(true);

    }
    else if(/*m_reconnectTimer.elapsed()>=10000&&*/(m_devCtl->workStatus()==UsbDev::DevCtl::WorkStatus::WorkStatus_S_Disconnected||m_devCtl->workStatus()==UsbDev::DevCtl::WorkStatus::WorkStatus_S_Disconnected))
    {
        qDebug()<<"Disconnected";
        if(m_devCtl!=nullptr)
        {
            disconnect(m_devCtl.data(),&UsbDev::DevCtl::workStatusChanged,this,&DeviceOperation::workOnWorkStatusChanged);
            disconnect(m_devCtl.data(),&UsbDev::DevCtl::updateInfo,this,&DeviceOperation::updateDevInfo);
            disconnect(m_devCtl.data(),&UsbDev::DevCtl::newStatusData,this,&DeviceOperation::workOnNewStatuData);
            disconnect(m_devCtl.data(),&UsbDev::DevCtl::newFrameData,this,&DeviceOperation::workOnNewFrameData);
            disconnect(m_devCtl.data(),&UsbDev::DevCtl::newProfile,this,&DeviceOperation::workOnNewProfile);
            disconnect(m_devCtl.data(),&UsbDev::DevCtl::newConfig,this,&DeviceOperation::workOnNewConfig);
            m_devCtl.reset(nullptr);
        }
//        setIsDeviceReady(false);
//        setDeviceStatus(0);
//        if(m_connectDev)
//        {
//            waitForSomeTime(10000);
//            connectDev();
//        }
    }
    emit workStatusChanged();
}
}

