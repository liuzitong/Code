﻿#include "device_agorithm.h"
#include <QDebug>
#include <usbdev/main/usbdev_devctl.hxx>
#include <device_operation.h>
namespace DevOps{
int DeviceAgorithm::interpolation(int value[], QPointF loc)
{
    double secondVal[2];
    secondVal[0]=value[0]+(value[1]-value[0])*(loc.x()/6.0);
    secondVal[1]=value[2]+(value[3]-value[2])*(loc.x()/6.0);
    int ret=secondVal[0]+(secondVal[1]-secondVal[0])*(loc.y()/6.0);
    return ret;
}

int DeviceAgorithm::getFocusMotorPosByDist(int focalDist, int spotSlot)
{

//    auto config=DeviceOperation::getSingleton()->m_devCtl->config();
    auto localData=DeviceData::getSingleton()->m_localTableData;
//    if(config.isEmpty()) {return 0;}
    auto map = localData.m_focalLengthMotorPosMappingData;
    int indexDist= floor(focalDist/10)-8;
    int pos1=map(indexDist,spotSlot-1);
    int pos2=map(indexDist+1,spotSlot-1);
    int focalMotorPos=pos1+(pos2-pos1)*(focalDist%10)/10;
    return focalMotorPos;
}

CoordMotorPosFocalDistInfo DeviceAgorithm::getXYMotorPosAndFocalDistFromCoord(const QPointF loc)
{
    CoordMotorPosFocalDistInfo coordSpacePosInfo;
    static bool isMainDotInfoTable=true;
    //有15格,所以要加15,Y要反号
    int x1=floor(loc.x()/6.0f)+15;int x2=ceil(loc.x()/6.0f)+15;
    int y1=floor(-loc.y()/6.0f)+15;int y2=ceil(-loc.y()/6.0f)+15;
    qDebug()<<"x1:"<<x1<<"x2:"<<x2;
    qDebug()<<"y1:"<<y1<<"y2:"<<y2;
    auto data=DeviceData::getSingleton()->m_localTableData;
    SingleTableData tableData;
    isMainDotInfoTable?tableData=data.m_mainPosTableData:tableData=data.m_secondaryPosTableData;

    CoordMotorPosFocalDistInfo fourDots[4]              //周围四个坐标点的马达和焦距值
    {
        {tableData(y1*3,x1),tableData(y1*3+1,x1),tableData(y1*3+2,x1)},{tableData(y1*3,x2),tableData(y1*3+1,x2),tableData(y1*3+2,x2)},
        {tableData(y2*3,x1),tableData(y2*3+1,x1),tableData(y2*3+2,x1)},{tableData(y2*3,x2),tableData(y2*3+1,x2),tableData(y2*3+2,x2)},
    };

    if(!((fourDots[0].motorX!=-1)&&(fourDots[1].motorX!=-1)&&(fourDots[2].motorX!=-1)&&(fourDots[3].motorX!=-1)))
    {
        isMainDotInfoTable=!isMainDotInfoTable;
        isMainDotInfoTable?tableData=data.m_mainPosTableData:tableData=data.m_secondaryPosTableData;
        fourDots[0]={tableData(y1*3,x1),tableData(y1*3+1,x1),tableData(y1*3+2,x1)};
        fourDots[1]={tableData(y2*3,x1),tableData(y2*3+1,x1),tableData(y2*3+2,x1)};
        fourDots[2]={tableData(y1*3,x2),tableData(y1*3+1,x2),tableData(y1*3+2,x2)};
        fourDots[3]={tableData(y2*3,x2),tableData(y2*3+1,x2),tableData(y2*3+2,x2)};
        if(!((fourDots[0].motorX=!-1)||(fourDots[1].motorX=-1)||(fourDots[2].motorX=-1)||(fourDots[3].motorX=-1)))
        {
            isMainDotInfoTable=!isMainDotInfoTable;
            isMainDotInfoTable?tableData=data.m_mainPosTableData:tableData=data.m_secondaryPosTableData;
            fourDots[0]={tableData(y1*3,x1),tableData(y1*3+1,x1),tableData(y1*3+2,x1)};
            fourDots[1]={tableData(y2*3,x1),tableData(y2*3+1,x1),tableData(y2*3+2,x1)};
            fourDots[2]={tableData(y1*3,x2),tableData(y1*3+1,x2),tableData(y1*3+2,x2)};
            fourDots[3]={tableData(y2*3,x2),tableData(y2*3+1,x2),tableData(y2*3+2,x2)};
        }
        if(!((fourDots[0].motorX=!-1)||(fourDots[1].motorX=-1)||(fourDots[2].motorX=-1)||(fourDots[3].motorX=-1)))
        {
            return coordSpacePosInfo;
        }
    }


    QPointF locInterpol(loc.x()-(x1-15)*6,-loc.y()-(y1-15)*6);         //算出比格子位置多多少
    qDebug()<<"loc is:"<<loc;
    int arr[4];
    for(unsigned int i=0;i<sizeof(arr)/sizeof(int);i++) {arr[i]=fourDots[i].motorX;}            //四个格子差值计算
    coordSpacePosInfo.motorX=interpolation(arr,locInterpol);

    for(unsigned int i=0;i<sizeof(arr)/sizeof(int);i++) {arr[i]=fourDots[i].motorY;}
    coordSpacePosInfo.motorY=interpolation(arr,locInterpol);

    auto config=DeviceOperation::getSingleton()->m_devCtl->config();
    if(isMainDotInfoTable)
    {
        auto centerX=config.mainTableCenterXRef();
        auto centerY=config.mainTableCenterYRef();
        coordSpacePosInfo.motorX+=centerX;
        coordSpacePosInfo.motorY+=centerY;
    }
    else
    {
        auto secondaryCenterX=config.secondaryTableCenterXRef();
        auto secondaryCenterY=config.secondaryTableCenterYRef();
        coordSpacePosInfo.motorX+=secondaryCenterX;
        coordSpacePosInfo.motorY+=secondaryCenterY;
    }

    for(unsigned int i=0;i<sizeof(arr)/sizeof(int);i++) {arr[i]=fourDots[i].focalDist;}
    coordSpacePosInfo.focalDist=interpolation(arr,loc);
//    showDevInfo(QString("X电机:%1,Y电机:%2,焦距:%3.").
//                arg(QString::number(coordMotorPosFocalDistInfo.motorX)).
//                arg(QString::number(coordMotorPosFocalDistInfo.motorY)).
//                arg(QString::number(coordMotorPosFocalDistInfo.focalDist)));
    qDebug()<<QString("X电机:%1,Y电机:%2,焦距:%3.").
              arg(QString::number(coordSpacePosInfo.motorX)).
              arg(QString::number(coordSpacePosInfo.motorY)).
              arg(QString::number(coordSpacePosInfo.focalDist));
    return coordSpacePosInfo;
}
}
