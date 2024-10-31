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
#include <pupilDetectApi.hxx>
#include <Windows.h>

#pragma execution_character_set("utf-8")
namespace DevOps{

QSharedPointer<DeviceOperation> DeviceOperation::m_singleton=/*QSharedPointer<DeviceOperation>(new DeviceOperation())*/nullptr;


// #include <windows.h>

// double get_cpu_usage()
// {
//     static _FILETIME pre_idle_time;
//     static _FILETIME pre_kernel_time;
//     static _FILETIME pre_user_time;

//     // 空闲时间
//     FILETIME idle_time;
//     // 内核时间
//     FILETIME kernel_time;
//     // 用户时间
//     FILETIME user_time;
//     BOOL ret = GetSystemTimes(&idle_time, &kernel_time, &user_time);

//     qint64 idle = CompareFileTime(&pre_idle_time, &idle_time);
//     qint64 kernel = CompareFileTime(&pre_kernel_time, &kernel_time);
//     qint64 user = CompareFileTime(&pre_user_time, &user_time);

//     float rate = (kernel + user - idle) / (1.0*(kernel + user));

//     pre_idle_time = idle_time;
//     pre_kernel_time = kernel_time;
//     pre_user_time = user_time;

//     return rate;
// }



DeviceOperation::DeviceOperation()
{
    connect(&m_reconnectTimer,&QTimer::timeout,this,&DeviceOperation::reconnectDev);
    m_autoPupilElapsedTimer.start();
    m_deviceSettings=DeviceSettings::getSingleton();
    m_reconnectTimer.setInterval(m_deviceSettings->m_reconnectTime);                            //复位的时候会短暂收不到数据更新，时间不能太短
    m_waitingTime=m_deviceSettings->m_waitingTime;
    m_currentCastLightDA=m_deviceSettings->m_castLightDA;
    m_config=DeviceData::getSingleton()->m_config;


    std::string PROJECT_DIR = ".";
    std::string pupilModelPath = PROJECT_DIR + "/v8_64_1_848_pupil_model_4.xml";
    std::string spotModelPath = PROJECT_DIR + "/v8_64_1_21100_spot_model_4.xml";
    std::string type = "CPU";
    ai::initializePupilDetector(pupilModelPath.c_str(), spotModelPath.c_str(), type.c_str());
    spdlog::rotating_logger_mt("logger", "logs/log.txt", 1024*1024*100, 30);
    spdlog::flush_on(spdlog::level::info);
}


DeviceOperation::~DeviceOperation()
{

}

//被checkSvcWorker 调用
void DeviceOperation::connectDev()
{
    auto log=spdlog::get("logger");
    log->info("conect device.");
    if(m_devCtl==nullptr)
    {
        // updateDevInfo("connecting.");
// #ifndef _DEBUG                               //release 情况下重连
        m_reconnectTimer.start();
// #endif
        auto deviceSettings=DeviceSettings::getSingleton();
        quint32 vid_pid=deviceSettings->m_VID.toInt(nullptr,16)<<16|deviceSettings->m_PID.toInt(nullptr,16);
        m_devCtl.reset(UsbDev::DevCtl::createInstance(vid_pid));
        connect(m_devCtl.data(),&UsbDev::DevCtl::workStatusChanged,this,&DeviceOperation::workOnWorkStatusChanged);
    }
}

void DeviceOperation::disconnectDev()
{
    auto log=spdlog::get("logger");
    log->info("disconnect device.");
    m_devCtl.reset(nullptr);
    m_status={-1,-1};
    setDeviceStatus(0);
}

void DeviceOperation::reconnectDev()
{
    auto log=spdlog::get("logger");
    log->info("reconnect to device.");
    m_eyeglassIntialize=false;
    m_devCtl.reset(nullptr);
    m_status={-1,-1};
    setDeviceStatus(1);
    m_deviceSettings->m_reconTimes++;
    m_deviceSettings->saveReconTimes();
    connectDev();
}

QSharedPointer<DeviceOperation> DeviceOperation::getSingleton()
{
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
    auto log=spdlog::get("logger");
    log->info("set cursor color and cursor size begins.");
    // resetMotors({UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Light_Spot});
    moveColorAndSpotMotorAvoidCollision();
    waitForSomeTime(m_waitingTime);
    waitMotorStop({{UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Light_Spot}});
    auto profile=m_profile;
    auto config=m_config;
    quint8 sps[5]={1,1,1,1,1};
    auto colorToSlot=m_deviceSettings->m_colorToSlot;
    auto spotSizeToSlot=m_deviceSettings->m_spotSizeToSlot;
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
        log->info("移动焦距电机到目标位置.");
        m_devCtl->move5Motors(std::array<quint8, 5>{0,0,sps[2],0,0}.data(),motorPos);
        waitForSomeTime(m_waitingTime);
    }
    {
        log->info("移动光斑和颜色电机到目标位置.");
        int motorPos[5]{0,0,0,colorPos,sizePos}; //单个电机绝对位置不可行
        quint8 speed[5]{0,0,0,sps[3],sps[4]};
        waitForSomeTime(m_waitingTime);
        waitMotorStop({UsbDev::DevCtl::MotorId_Light_Spot,UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Focus});
        m_devCtl->move5Motors(speed,motorPos);
    }

    {
        log->info("转一圈拖动光斑和颜色.");
        int  motorPos[5]{0,0,0,color_Circl_Motor_Steps,spot_Circl_Motor_Steps};
        quint8 speed[5]{0,0,0,sps[3],sps[4]};
        waitForSomeTime(m_waitingTime);
        waitMotorStop({UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Light_Spot});
        m_devCtl->move5Motors(speed,motorPos,UsbDev::DevCtl::Relative);
    }
    {
        log->info("反向相对转动1000，脱离防止干扰误差.");
        int motorPos[5]={0,0,0,-1000,-1000};
        quint8 speed[5]{0,0,0,sps[3],sps[4]};
        waitForSomeTime(m_waitingTime);
        waitMotorStop({UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Light_Spot});
        m_devCtl->move5Motors(speed,motorPos,UsbDev::DevCtl::Relative);
    }
    {
        log->info("焦距电机相对移动10000脱离颜色和光斑.");
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
    log->info("set cursor color and cursor size is over.");
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
}


void DeviceOperation::setWhiteLamp(bool onOff)
{
    if(m_deviceStatus!=2) return;
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
    for(auto& motorId:motorIDs)
    {
        if(m_statusData.isMotorBusy(motorId))
        {
            return true;
        }
    }
    return false;
}

void DeviceOperation::setDB(int DB)
{
    auto log=spdlog::get("logger");
    log->info("set DB {0}.",DB);
    if(m_deviceStatus!=2) return;
    UsbDev::Config config;
    config=m_config;
    quint8 sps[5]{0};
    int motorPos[5]{0};
    motorPos[3]=config.DbPosMappingPtr()[DB][0];
    motorPos[4]=config.DbPosMappingPtr()[DB][1];
    auto spsConfig=m_deviceSettings->m_5MotorSpeed;
    sps[3]=spsConfig[3];sps[4]=spsConfig[4];
    waitMotorStop({UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Light_Spot});
    m_devCtl->move5Motors(sps,motorPos);
}


void DeviceOperation::moveToAdjustLight(int motorPosX,int motorPosY,int motorPosFocal)
{
#ifdef _DEBUG
    std::cout<<"move to adjustlight"<<std::endl;
#endif
    auto log=spdlog::get("logger");
    log->info("move to adjust light positions.");
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
    auto spotSizeToSlot=m_deviceSettings->m_spotSizeToSlot;
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
    waitShutterClose();
    move5Motors(isMotorMove,motorPos);
    waitForSomeTime(m_waitingTime);
}



void DeviceOperation::adjustCastLight()
{
#ifdef _DEBUG
    std::cout<<"adjustCastLightStart"<<std::endl;
#endif
    auto log=spdlog::get("logger");
    log->info("Adjust cast light begins.");
    int color=m_deviceSettings->m_castLightTargetColor;
    int size=m_deviceSettings->m_castLightTargetSize;
    setCursorColorAndCursorSize(color,size);
    qDebug()<<m_config.xMotorPosForLightCorrectionRef();
    qDebug()<<m_config.yMotorPosForLightCorrectionRef();
    qDebug()<<m_config.focalLengthMotorPosForLightCorrectionRef();
    moveToAdjustLight(m_config.xMotorPosForLightCorrectionRef()+m_deviceSettings->m_deviationCalibrationXMotorDeviation,
                      m_config.yMotorPosForLightCorrectionRef()+m_deviceSettings->m_deviationCalibrationYMotorDeviation,
                      m_config.focalLengthMotorPosForLightCorrectionRef());
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
    setCastLightAdjustStatus(2);
    m_castLightAdjustElapsedTimer.restart();
    m_castLightStablelizeWaitingElapsedTimer.restart();
    m_deviationCalibrationTimer.restart();
    log->info("Adjust cast light is adjusting.");
}


void DeviceOperation::dynamicStimulate(QPointF begin, QPointF end, int cursorSize,int speedLevel,bool isMainDotInfoTable,double stepLengthFactor)
{
    auto log=spdlog::get("logger");
    log->info("dynamic stimulate.");
    m_isMainTable=isMainDotInfoTable;
    auto spotSizeToSlot=m_deviceSettings->m_spotSizeToSlot;
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
    auto stepLength=data(speedLevel,0)*stepLengthFactor;
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
    log->info("start:{0},{1},end:{2},{3}.",begin.x(),begin.y(),end.x(),end.y());
    log->info("分割为{0}个点,X步长为{1},Y步长为{2}.",stepCount,stepLengthX,stepLengthY);
    m_lastDynamicCoordAndXYMotorPos.resize(stepCount);

    for(int i=0;i<stepCount;i++)
    {
        coordSpacePosInfoTemp.rx()+=stepLengthX;
        coordSpacePosInfoTemp.ry()+=stepLengthY;
        if(abs(coordSpacePosInfoTemp.rx()-round(coordSpacePosInfoTemp.rx()))<0.001)             //防止整数角度误差，造成超界
        {
            coordSpacePosInfoTemp.rx()=round(coordSpacePosInfoTemp.rx());
        }
        if(abs(coordSpacePosInfoTemp.ry()-round(coordSpacePosInfoTemp.ry()))<0.001)
        {
            coordSpacePosInfoTemp.ry()=round(coordSpacePosInfoTemp.ry());
        }
        coordMotorPosFocalDistInfoTemp=DeviceDataProcesser::getXYMotorPosAndFocalDistFromCoord(coordSpacePosInfoTemp,isMainDotInfoTable);
        dotArr[i*3+0]=coordMotorPosFocalDistInfoTemp.motorX;
        dotArr[i*3+1]=coordMotorPosFocalDistInfoTemp.motorY;
        dotArr[i*3+2]=DeviceDataProcesser::getFocusMotorPosByDist(coordMotorPosFocalDistInfoTemp.focalDist,spotSlot,m_config.focalMotorPosCorrectionRef());
        m_lastDynamicCoordAndXYMotorPos[i]={coordSpacePosInfoTemp,{coordMotorPosFocalDistInfoTemp.motorX,coordMotorPosFocalDistInfoTemp.motorY}};

    }

    log->info("send move data");
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
    if(m_deviceStatus==2)
        m_devCtl->sendDynamicData(totalframe,totalframe-1,dataLen,&dotArr[stepPerFrame*3*(totalframe-1)]);     //最后一帧
    waitForSomeTime(1000);
    log->info("begin move");
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
    auto spsConfig=m_deviceSettings->m_5MotorSpeed;
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

void DeviceOperation::moveColorAndSpotMotorAvoidCollision()
{
    if(m_deviceStatus!=2) return;
    waitMotorStop({UsbDev::DevCtl::MotorId_Focus,UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Light_Spot});
    auto colorPos=m_config.switchColorMotorPosPtr()[0]+m_deviceSettings->m_stepOffset;
    auto spotPos=m_config.switchLightSpotMotorPosPtr()[0]+m_deviceSettings->m_stepOffset;

    int motorPos[5];
    motorPos[0]=0;
    motorPos[1]=0;
    motorPos[2]=0;
    motorPos[3]=colorPos;
    motorPos[4]=spotPos;
    bool isMotorMove[5]{false,false,false,true,true};
    move5Motors(isMotorMove,motorPos);
}

void DeviceOperation::waitMotorStop(QVector<UsbDev::DevCtl::MotorId> motorIDs)
{
    auto log=spdlog::get("logger");
    log->info("Wait for motor stop.");
    if(m_deviceStatus!=2) return;
    QElapsedTimer mstimer;
    mstimer.restart();//必须先等一会儿刷新状态
    do
    {
        if(m_deviceStatus!=2) return;
        QApplication::processEvents();
    }while(getMotorsBusy(motorIDs)/*||(mstimer.elapsed()<100)*/);
    log->info("Wait for motor stop is over");
}

void DeviceOperation::waitShutterClose()
{
    if(m_deviceStatus!=2) return;
    while(qAbs(m_statusData.motorPosition(UsbDev::DevCtl::MotorId_Shutter)-m_config.shutterOpenPosRef())<70)
    {
        QApplication::processEvents();
    }
}

void DeviceOperation::waitForSomeTime(int time)
{
    auto log=spdlog::get("logger");
    log->info("Wait for some time {0}ms.",time);
    QElapsedTimer mstimer;
    mstimer.restart();
    do
    {
        QApplication::processEvents();
    }while(mstimer.elapsed()<time);
    log->info("Wait for some time is over");
}

void DeviceOperation::moveChin(ChinMoveDirection direction)
{
    if(m_deviceStatus!=2) return;
    auto profile=m_devCtl->profile();
    auto spsConfig=m_deviceSettings->m_motorChinSpeed;

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
        auto log=spdlog::get("logger");
        log->info("light up castLight.");
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
        auto log=spdlog::get("logger");
        log->info("dim down castLight.");
        m_devCtl->setLamp(LampId::LampId_castLight,0,qFloor(m_currentCastLightDA*0.3));
#ifdef _DEBUG
        std::cout<<"cast light Down"<<qFloor(m_currentCastLightDA*0.3)<<std::endl;
#endif
        m_castLightUp=false;
    }
}

void DeviceOperation::resetMotors(QVector<UsbDev::DevCtl::MotorId> motorIDs)
{
    if(m_deviceStatus!=2) return;
    auto spsConfig=m_deviceSettings->m_motorChinSpeed;
    for(auto& motorID:motorIDs)
    {
        int speed=spsConfig[motorID];
        m_devCtl->resetMotor(motorID,speed);
    }
}

void DeviceOperation::beep(int count,int duration,int interval)
{
    auto devSetting=DeviceSettings::getSingleton();
    if(m_deviceStatus==2)
    {
        m_devCtl->beep(count,duration,interval);
    }
}

void DeviceOperation::beepCheckOver()
{
    auto devSetting=DeviceSettings::getSingleton();
    if(m_deviceStatus==2)
    {
        m_devCtl->beep(devSetting->m_beepCount,devSetting->m_beepDuration,devSetting->m_beepInterval);
    }
}

// void DeviceOperation::alarm()
// {
//     if(m_deviceStatus==2)
//     {
//         m_devCtl->beep(5,100,100);
//     }
// }

void DeviceOperation::clearPupilData()
{
    m_devicePupilProcessor.clearData();
}

void DeviceOperation::workOnNewStatuData()
{
// #ifndef _DEBUG                                                      //release 的情况下激活重连功能
    m_reconnectTimer.start();                                       //收到消息表示连接正常，重新计时
// #endif
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

    emit newStatusData();

    if(m_workingOnStatus) return;
    m_workingOnStatus=true;
    auto eyeglassStatus=m_statusData.eyeglassStatus();
    auto log=spdlog::get("logger");
    if(m_isAtCheckingPage)
    {
        log->info("开红外.");
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
        log->info("关红外.");
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
    // std::cout<<"castLightSensorDAForLightCorrectionRef:"<<m_config.castLightSensorDAForLightCorrectionRef()<<std::endl;

    bool motorBusy=getMotorsBusy({UsbDev::DevCtl::MotorId_X,UsbDev::DevCtl::MotorId_Y});
    int currentcastLightSensorDA=m_statusData.castLightSensorDA();
    int targetcastLightSensorDA=m_config.castLightSensorDAForLightCorrectionRef();

    if(m_deviationCalibrationStatus==0&&m_castLightAdjustStatus==2&&(m_castLightAdjustElapsedTimer.elapsed()>=m_deviceSettings->m_castLightDAChangeInterval)&&m_deviceStatus==2)
    {
        log->info("根据反馈的光位置校对光强.");
        double DADiffTolerance=m_deviceSettings->m_castLightDADifferenceTolerance*(targetcastLightSensorDA);
        int DADiff=qAbs(targetcastLightSensorDA-currentcastLightSensorDA);
        // std::cout<<"DADiff:"<<DADiff<<" DADiffTolerance:"<<DADiffTolerance<<std::endl;
        int minStep=m_deviceSettings->m_castLightDAChangeMinStep;
        int maxStep=qRound(DADiff*m_deviceSettings->m_castLightDAChangeRate);
        int step=qMax(minStep,maxStep);

        if(m_deviationCalibrationTimer.elapsed()>m_deviceSettings->m_deviationCalibrationWatingTime&&currentcastLightSensorDA<m_deviceSettings->m_deviationCalibrationDA)           //过了一段时间光强依然很低，所以就判定没照到
        {
            m_deviationCalibrationStatus=1;
            m_deviceSettings->m_deviationCalibrationXMotorDeviation=0;
            m_deviceSettings->m_deviationCalibrationYMotorDeviation=0;
            log->critical("start Deviation calibration fail.");
            setCastLightAdjustStatus(1);
            m_devCtl->setLamp(LampId::LampId_castLight,0,m_deviceSettings->m_castLightDA);
            m_currentCastLightDA=m_deviceSettings->m_castLightDA;
        }

        if(targetcastLightSensorDA>currentcastLightSensorDA&&currentcastLightSensorDA>m_deviceSettings->m_deviationCalibrationDA)                       //照到了但是达不到光强
        {
            if(m_currentCastLightDA==m_deviceSettings->m_castLightDALimit)                                                                              //超时判定灯盘粗了
            {
                log->critical("Bulb can't reach target brightness.");
                sendErroRInfo(tr("Bulb can't reach target brightness.Please Change bulb or contact customer service.This is serious"));

                setCastLightAdjustStatus(3);
                openShutter(0);
                waitForSomeTime(m_waitingTime);
                waitMotorStop({UsbDev::DevCtl::MotorId_Shutter});
                m_devCtl->setLamp(LampId::LampId_castLight,0,m_currentCastLightDA*0.3);

                m_deviceSettings->m_castLightLastAdjustedDate=QDate::currentDate().toString("yyyy/MM/dd");
                m_deviceSettings->m_castLightDA=m_currentCastLightDA;
                m_deviceSettings->saveCastLightAdjustStatus();
                goto End;
            }
            else
            {
                m_currentCastLightDA+=step;
                if(m_currentCastLightDA>=m_deviceSettings->m_castLightDALimit)
                {
                    m_currentCastLightDA=m_deviceSettings->m_castLightDALimit;
                }
            }
        }
        else
        {
            m_currentCastLightDA-=step;
        }


        m_devCtl->setLamp(LampId::LampId_castLight,0,m_currentCastLightDA);
        m_castLightAdjustElapsedTimer.restart();

        // std::cout<<"m_castLightStablelizeWaitingElapsedTimer:"<<m_castLightStablelizeWaitingElapsedTimer.elapsed()<<std::endl;

        if(DADiff>DADiffTolerance)
        {
            m_castLightStablelizeWaitingElapsedTimer.restart();
        }
        else if(m_castLightStablelizeWaitingElapsedTimer.elapsed()>m_deviceSettings->m_castLightStablizeWaitingTime)
        {
            log->info("完成校光.");
            setCastLightAdjustStatus(3);
            openShutter(0);
            waitForSomeTime(m_waitingTime);
            waitMotorStop({UsbDev::DevCtl::MotorId_Shutter});
            m_devCtl->setLamp(LampId::LampId_castLight,0,m_currentCastLightDA*0.3);

            m_deviceSettings->m_castLightLastAdjustedDate=QDate::currentDate().toString("yyyy/MM/dd");
            m_deviceSettings->m_castLightDA=m_currentCastLightDA;
            m_deviceSettings->saveCastLightAdjustStatus();
            goto End;
        }
    }

    if(m_deviationCalibrationStatus==1)
    {
        if(motorBusy)
        {
            auto coord=getDyanmicAnswerPos();
            qDebug()<<"coord: "<<coord<<" :x"<<m_statusData.motorPosition(MotorId::MotorId_X)<<" :y"<<m_statusData.motorPosition(MotorId::MotorId_Y)<<" da:"<<currentcastLightSensorDA;
            if(currentcastLightSensorDA>m_deviceSettings->m_deviationCalibrationDA&&m_deviationCalibrationXCoord.size()==0)
            {
                m_deviationCalibrationXCoord.push_back(coord.x());
                qDebug()<<m_deviationCalibrationXCoord;
            }
            if(currentcastLightSensorDA<m_deviceSettings->m_deviationCalibrationDA&&m_deviationCalibrationXCoord.size()==1)   //说明已经接触过感应器
            {
                m_deviationCalibrationXCoord.push_back(coord.x());
                m_deviationCalibrationStatus=2;
                stopDynamic();
                waitForSomeTime(50);
                qDebug()<<m_deviationCalibrationXCoord;
                //开始Y向扫描
                double xCoord=0.5*(m_deviationCalibrationXCoord[0]+m_deviationCalibrationXCoord[1]);
                dynamicStimulate({xCoord,30},{xCoord,42},m_deviceSettings->m_castLightTargetSize,1,true,0.003);
            }
        }
        else                     //继续X向扫描
        {
            static double direction=1.0f;
            dynamicStimulate({6*direction,m_deviationYCoord},{-6*direction,m_deviationYCoord},m_deviceSettings->m_castLightTargetSize,1,true,0.003);
            direction=-direction;
            m_deviationYCoord+=m_deviceSettings->m_deviationCalibrationStep;
            if(m_deviationYCoord>42)
            {
                openShutter(0);
                waitForSomeTime(m_waitingTime);
                waitMotorStop({UsbDev::DevCtl::MotorId_Shutter});
                m_devCtl->setLamp(LampId::LampId_castLight,0,m_currentCastLightDA*0.3);
                m_deviceSettings->m_deviationCalibrationFail=true;
                sendErroRInfo(tr("Deviation calibration fail.Please contact customer service.This is serious."));
                log->critical("Deviation calibration fail.");
                m_deviceSettings->saveDeviationCalibrationStatus();
                setCastLightAdjustStatus(3);
                m_deviationCalibrationStatus=0;
            }

        }
    }


    if(m_deviationCalibrationStatus==2)
    {
        if(motorBusy)
        {
            auto coord=getDyanmicAnswerPos();
            if(currentcastLightSensorDA>m_deviceSettings->m_deviationCalibrationDA&&m_deviationCalibrationYCoord.size()==0)
            {
                m_deviationCalibrationYCoord.push_back(coord.y());
                qDebug()<<m_deviationCalibrationYCoord;
            }
            if(currentcastLightSensorDA<m_deviceSettings->m_deviationCalibrationDA&&m_deviationCalibrationYCoord.size()==1)   //说明已经接触过感应器
            {
                m_deviationCalibrationYCoord.push_back(coord.y());
                qDebug()<<m_deviationCalibrationYCoord;
                m_deviationCalibrationStatus=0;
                stopDynamic();
                waitForSomeTime(50);
                QPointF coord={0.5*(m_deviationCalibrationXCoord[0]+m_deviationCalibrationXCoord[1]),0.5*(m_deviationCalibrationYCoord[0]+m_deviationCalibrationYCoord[1])};
                auto coordSpacePosInfo=DeviceDataProcesser::getXYMotorPosAndFocalDistFromCoord(coord,true);
                m_deviceSettings->m_deviationCalibrationXMotorDeviation=coordSpacePosInfo.motorX-m_config.xMotorPosForLightCorrectionRef();
                m_deviceSettings->m_deviationCalibrationYMotorDeviation=coordSpacePosInfo.motorY-m_config.yMotorPosForLightCorrectionRef();
                m_deviceSettings->saveDeviationCalibrationStatus();
                adjustCastLight();
            }
        }
        else
        {
            m_deviationCalibrationStatus=0;
            stopDynamic();
            waitForSomeTime(50);
            QPointF coord={0.5*(m_deviationCalibrationXCoord[0]+m_deviationCalibrationXCoord[1]),m_deviationCalibrationYCoord[0]};
            auto coordSpacePosInfo=DeviceDataProcesser::getXYMotorPosAndFocalDistFromCoord(coord,true);
            m_deviceSettings->m_deviationCalibrationXMotorDeviation=coordSpacePosInfo.motorX-m_config.xMotorPosForLightCorrectionRef();
            m_deviceSettings->m_deviationCalibrationYMotorDeviation=coordSpacePosInfo.motorY-m_config.yMotorPosForLightCorrectionRef();
            m_deviceSettings->saveDeviationCalibrationStatus();
            adjustCastLight();
        }
    }
    End:
    m_workingOnStatus=false;

}

void DeviceOperation::workOnNewFrameData()
{
    // double usage=get_cpu_usage();
    // std::cout<<usage<<std::endl;
    // if(usage>=80.0f) return;
    m_frameData=m_devCtl->takeNextPendingFrameData();
    m_frameRawDataLock.lock();
    m_frameRawData=m_frameData.rawData();
    m_frameRawDataLock.unlock();
    auto data=m_frameData.rawData();

    ai::Result res;
    ai::Image image{m_videoSize.width(),m_videoSize.height(),data.data()};
    getPupilResultByImage(image,&res);
    QImage img((uchar*)data.data(),m_videoSize.width(),m_videoSize.height(),QImage::Format_Grayscale8);
    img=img.convertToFormat(QImage::Format_ARGB32);
    QByteArray ba1=QByteArray((char*)img.bits(),img.byteCount());

    QPainter painter(&img);
    painter.setPen(QPen{Qt::yellow,2});
    auto& pupil=res.pupil;

    if(pupil.center.x>0)
    {
        painter.drawEllipse({pupil.center.x,pupil.center.y},pupil.long_axis/2,pupil.short_axis/2);
        for(auto& dot:res.reflectDots)
        {
            painter.setPen(Qt::green);
            painter.drawEllipse({qRound(dot.x),qRound(dot.y)},3,3);
        }
    }
    QByteArray ba2=QByteArray((char*)img.bits(),img.byteCount());
    emit newFrameData(ba1,ba2,pupil.center.x>0);

    m_devicePupilProcessor.processData(&res);
    setChinDistAlarm(m_devicePupilProcessor.m_isTooFar);
    if(m_autoPupilElapsedTimer.elapsed()>=m_autoPupilElapsedTime)
    {
        m_autoPupilElapsedTimer.restart();
        if(m_devicePupilProcessor.m_pupilResValid)
        {
            if(m_autoAlignPupil)                //自动对眼位
            {
                auto step=m_deviceSettings->m_pupilAutoAlignStep;
                int tolerance=m_deviceSettings->m_pupilAutoAlignPixelTolerance;
                auto spsConfig=m_deviceSettings->m_motorChinSpeed;
                quint8 sps[2]{spsConfig[0],spsConfig[1]};
                int motorPos[2]{0};

                QPointF pupilDeviation={res.pupil.center.x-0.5*m_videoSize.width(),res.pupil.center.y-0.5*m_videoSize.height()};

                if(!m_statusData.isMotorBusy(UsbDev::DevCtl::MotorId_Chin_Hoz)&&qAbs(pupilDeviation.x())>tolerance)
                {
                    motorPos[0]=-pupilDeviation.x()*step;
                    m_devCtl->moveChinMotors(sps,motorPos,UsbDev::DevCtl::MoveMethod::Relative);
                    m_autoPupilElapsedTime=400;
                }
                else
                {
                    m_autoPupilElapsedTime=200;
                }

                if(!m_statusData.isMotorBusy(UsbDev::DevCtl::MotorId_Chin_Vert)&&qAbs(pupilDeviation.y())>tolerance)
                {
                    motorPos[1]=pupilDeviation.y()*step;
                    m_devCtl->moveChinMotors(sps,motorPos,UsbDev::DevCtl::MoveMethod::Relative);
                    m_autoPupilElapsedTime=400;
                }
                else
                {
                    m_autoPupilElapsedTime=200;
                }
            }
        }
    }
    emit pupilDiameterChanged();
}

void DeviceOperation::workOnNewProfile()
{
    m_profile=m_devCtl->profile();
    emit newDeviceVersion(QString::number(m_profile.devVersion(),16));
    m_videoSize=m_profile.videoSize();
}

void DeviceOperation::workOnNewConfig()
{
    auto log=spdlog::get("logger");
    log->info("接受到新的Config.");
    UsbDev::Config config=m_devCtl->config();
    quint32 crc=DeviceDataProcesser::calcCrc((quint8*)config.dataPtr()+4, config.dataLen()-4);
    if(DeviceData::getSingleton()->m_config.isEmpty())
    {
        log->info("本地没有Config,保存config.");
        QString filePath = R"(./deviceData/config.cfg)";
        QFile file(filePath);
        if(file.open(QIODevice::WriteOnly))
        {
            file.write((char*)config.dataPtr(),config.dataLen());
        }
        log->info("使用下位机config");
        m_config=config;
    }
    else
    {
        log->info("本地有Config,下位机config计算得到crc:{0}, 记录crc:{1}.",crc,config.crcVeryficationRef());
        if(crc==config.crcVeryficationRef())
        {
            log->info("使用下位机config");
            m_config=config;
        }
    }

    // m_devicePupilProcessor.m_pupilGreyLimit=m_config.pupilGreyThresholdDAPtr()[0];
    // m_devicePupilProcessor.m_pupilReflectionDotWhiteLimit=m_config.pupilGreyThresholdDAPtr()[1];
    emit newDeviceID(QString(m_config.deviceIDRef()));
    auto date=QDate::currentDate();
    auto lastAdjustedDate=QDate::fromString(m_deviceSettings->m_castLightLastAdjustedDate,"yyyy/MM/dd");
    bool adjusted=((date.year()==lastAdjustedDate.year())&&(date.month()==lastAdjustedDate.month())&&(date.day()==lastAdjustedDate.day()));
    adjusted=false;    //现在改成每次都校光
    bool skipAdjustCastLight=m_deviceSettings->m_skipAdjustCastLight;
    bool deviationCalibrationFail=m_deviceSettings->m_deviationCalibrationFail;
    if(deviationCalibrationFail)
    {
        sendErroRInfo(tr("Deviation calibration fail.Please contact customer service.This is serious."));
    }
    if(adjusted||skipAdjustCastLight||deviationCalibrationFail)
    {
        setCastLightAdjustStatus(3);
        dimDownCastLight();
    }
    else
        adjustCastLight();
    emit newTargetCastLightSensorDA(m_config.castLightSensorDAForLightCorrectionRef());
    // std::cout<<"m_config targetDa:"<<m_config.castLightSensorDAForLightCorrectionRef()<<std::endl;;
}

void DeviceOperation::workOnWorkStatusChanged(int status)
{
    auto log=spdlog::get("logger");
    log->info("work status changed.");
    if(m_devCtl->workStatus()==UsbDev::DevCtl::WorkStatus::WorkStatus_S_ConnectToDev)
    {
        log->info("in connecting.");
        qDebug()<<"in connecting.";
    }
    if(m_devCtl->workStatus()==UsbDev::DevCtl::WorkStatus::WorkStatus_S_OK)
    {
        log->info("Connect Successfully.");
        qDebug()<<"Connect Successfully.";
        // updateDevInfo("Connect Successfully.");
        // connect(m_devCtl.data(),&UsbDev::DevCtl::updateInfo,this,&DeviceOperation::updateDevInfo);
        connect(m_devCtl.data(),&UsbDev::DevCtl::newStatusData,this,&DeviceOperation::workOnNewStatuData);
        connect(m_devCtl.data(),&UsbDev::DevCtl::newFrameData,this,&DeviceOperation::workOnNewFrameData);
        connect(m_devCtl.data(),&UsbDev::DevCtl::newProfile,this,&DeviceOperation::workOnNewProfile);
        connect(m_devCtl.data(),&UsbDev::DevCtl::newConfig,this,&DeviceOperation::workOnNewConfig);

        m_devCtl->readProfile();
        while (m_profile.isEmpty()) {QApplication::processEvents();}

        m_devCtl->readConfig();
        setDeviceStatus(2);

    }
    else if(/*m_reconnectTimer.elapsed()>=10000&&*/(m_devCtl->workStatus()==UsbDev::DevCtl::WorkStatus::WorkStatus_S_Disconnected||m_devCtl->workStatus()==UsbDev::DevCtl::WorkStatus::WorkStatus_S_Disconnected))
    {
        log->info("Disconnected.");
        qDebug()<<"Disconnected";
        if(m_devCtl!=nullptr)
        {
            disconnect(m_devCtl.data(),&UsbDev::DevCtl::workStatusChanged,this,&DeviceOperation::workOnWorkStatusChanged);
            // disconnect(m_devCtl.data(),&UsbDev::DevCtl::updateInfo,this,&DeviceOperation::updateDevInfo);
            disconnect(m_devCtl.data(),&UsbDev::DevCtl::newStatusData,this,&DeviceOperation::workOnNewStatuData);
            disconnect(m_devCtl.data(),&UsbDev::DevCtl::newFrameData,this,&DeviceOperation::workOnNewFrameData);
            disconnect(m_devCtl.data(),&UsbDev::DevCtl::newProfile,this,&DeviceOperation::workOnNewProfile);
            disconnect(m_devCtl.data(),&UsbDev::DevCtl::newConfig,this,&DeviceOperation::workOnNewConfig);
            m_devCtl.reset(nullptr);
        }
    }
    emit workStatusChanged();
}
}

