#include "device_data_processer.h"
#include <QDebug>
#include <usbdev/main/usbdev_devctl.hxx>
#include <device_operation.h>
#include <device_settings.h>
namespace DevOps{

QSharedPointer<DeviceDataProcesser> DeviceDataProcesser::m_singleton=nullptr;
bool DeviceDataProcesser::isMainDotInfoTable=true;

int DeviceDataProcesser::interpolation(int value[], QPointF loc)
{
    double secondVal[2];
    secondVal[0]=value[0]+(value[1]-value[0])*(loc.x()/6.0);
    secondVal[1]=value[2]+(value[3]-value[2])*(loc.x()/6.0);
    int ret=secondVal[0]+(secondVal[1]-secondVal[0])*(loc.y()/6.0);
    return ret;
}

int DeviceDataProcesser::getFocusMotorPosByDist(int focalDist, int spotSlot)
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

CoordMotorPosFocalDistInfo DeviceDataProcesser::getXYMotorPosAndFocalDistFromCoord(const QPointF loc)
{
    CoordMotorPosFocalDistInfo coordSpacePosInfo;
    if(loc.x()<-30)
    {
        isMainDotInfoTable=true;
    }
    else if(loc.x()>30)
    {
        isMainDotInfoTable=false;
    }
    //有15格,所以要加15,Y要反号
    int x1=floor(loc.x()/6.0f)+15;int x2=ceil(loc.x()/6.0f)+15;
    int y1=floor(-loc.y()/6.0f)+15;int y2=ceil(-loc.y()/6.0f)+15;
//    qDebug()<<"x1:"<<x1<<"x2:"<<x2;
//    qDebug()<<"y1:"<<y1<<"y2:"<<y2;
    auto data=DeviceData::getSingleton()->m_localTableData;
    SingleTableData tableData;
    isMainDotInfoTable?tableData=data.m_mainPosTableData:tableData=data.m_secondaryPosTableData;

    CoordMotorPosFocalDistInfo fourDots[4]              //周围四个坐标点的马达和焦距值
    {
        {tableData(y1*3,x1),tableData(y1*3+1,x1),tableData(y1*3+2,x1)},{tableData(y1*3,x2),tableData(y1*3+1,x2),tableData(y1*3+2,x2)},
        {tableData(y2*3,x1),tableData(y2*3+1,x1),tableData(y2*3+2,x1)},{tableData(y2*3,x2),tableData(y2*3+1,x2),tableData(y2*3+2,x2)},
    };

    Q_ASSERT((fourDots[0].motorX!=-1)&&(fourDots[1].motorX!=-1)&&(fourDots[2].motorX!=-1)&&(fourDots[3].motorX!=-1));
    isMainDotInfoTable?tableData=data.m_mainPosTableData:tableData=data.m_secondaryPosTableData;
    fourDots[0]={tableData(y1*3,x1),tableData(y1*3+1,x1),tableData(y1*3+2,x1)};
    fourDots[1]={tableData(y2*3,x1),tableData(y2*3+1,x1),tableData(y2*3+2,x1)};
    fourDots[2]={tableData(y1*3,x2),tableData(y1*3+1,x2),tableData(y1*3+2,x2)};
    fourDots[3]={tableData(y2*3,x2),tableData(y2*3+1,x2),tableData(y2*3+2,x2)};
    QPointF locInterpol(loc.x()-(x1-15)*6,-loc.y()-(y1-15)*6);         //算出比格子位置多多少
//    qDebug()<<"loc is:"<<locInterpol;
    int arr[4];
    for(unsigned int i=0;i<sizeof(arr)/sizeof(int);i++) {arr[i]=fourDots[i].motorX;}            //四个格子差值计算
    coordSpacePosInfo.motorX=interpolation(arr,locInterpol);

    for(unsigned int i=0;i<sizeof(arr)/sizeof(int);i++) {arr[i]=fourDots[i].motorY;}
    coordSpacePosInfo.motorY=interpolation(arr,locInterpol);

//    auto config=DeviceOperation::getSingleton()->m_devCtl->config();
    auto config=DeviceOperation::getSingleton()->m_config;
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
    coordSpacePosInfo.focalDist=interpolation(arr,locInterpol);
//    qDebug()<<QString("X motor:%1,Y motor:%2,focal :%3.").
//              arg(QString::number(coordSpacePosInfo.motorX)).
//              arg(QString::number(coordSpacePosInfo.motorY)).
//              arg(QString::number(coordSpacePosInfo.focalDist));
    return coordSpacePosInfo;
}

QVector<QPoint> DeviceDataProcesser::caculatePupilDeviation(const QByteArray ba, int width, int height,bool& valid)
{
    int y_max=0;
    int y_min=UINT_MAX;
    int x_max=0;
    int x_min=UINT_MAX;
    QVector<int> x_vc;
    QVector<int> y_vc;
    QVector<int> x_vc2;
    QVector<int> y_vc2;
    auto pupilGreyLimit=DeviceSettings::getSingleton()->m_pupilGreyLimit;
    auto pupilPixelDiameterLimit=DeviceSettings::getSingleton()->m_pupilPixelDiameterLimit;
    int validCount=0;
    for(quint32 y=height*0.25;y<height*0.75;y++)
    {
        for(quint32 x=width*0.25;x<width*0.75;x++)
        {
            if(ba[x+width*y]<pupilGreyLimit)
            {
                qDebug()<<ba[x+width*y];
                x_vc.push_back(x);
                y_vc.push_back(y);
                validCount++;
            }
        }
    }
    if(validCount>0&&validCount<=100*100)
    {
        valid=true;
    }
    else
    {
        valid=false;
        return QVector<QPoint>{{0,0},{0,0},{0,0}};
    }

    int x_avg,y_avg,sum=0;
    for(int i=0;i<x_vc.length();i++)
    {
        sum+=x_vc[i];
    }
    x_avg=sum/x_vc.length();
    sum=0;
    for(int i=0;i<y_vc.length();i++)
    {
        sum+=y_vc[i];
    }
    y_avg=sum/y_vc.length();

    for(int i=0;i<x_vc.length();i++)
    {
        auto x=x_vc[i];
        if(qAbs(int(x-x_avg))<pupilPixelDiameterLimit)
        {
            if(x>x_max) x_max=x;
            if(x<x_min) x_min=x;
            x_vc2.push_back(x);
        }
    }

    for(int i=0;i<y_vc.length();i++)
    {
        auto y=y_vc[i];
        if(qAbs(int(y-y_avg))<pupilPixelDiameterLimit)
        {
            if(y>y_max) y_max=y;
            if(y<y_min) y_min=y;
            y_vc2.push_back(y);
        }

    }
    for(int i=0;i<x_vc2.length();i++)
    {
        sum+=x_vc2[i];
    }
    int x_avg2=sum/x_vc2.length();
    sum=0;
    for(int i=0;i<y_vc2.length();i++)
    {
        sum+=y_vc2[i];
    }
    int y_avg2=sum/y_vc2.length();
    QPoint center={int(x_avg2-width*0.5),int(y_avg2-height*0.5)};
    QPoint topLeft={x_min,y_max};
    QPoint bottomRight={x_max,y_min};
    return QVector<QPoint>{center,topLeft,bottomRight};
}

float DeviceDataProcesser::caculatePupilDiameter(QPoint topLeft,QPoint bottomRight)
{
    auto width=bottomRight.x()-topLeft.x();
    auto height=topLeft.y()-bottomRight.y();
    auto pixelDiameter=sqrt(width*height);
    auto diameter=pixelDiameter*DeviceSettings::getSingleton()->m_pupilDiameterPixelToMillimeterConstant;
    return diameter;
}

int DeviceDataProcesser::caculateFixationDeviation(QPoint point)
{
    auto deviation=sqrt(pow(point.x(),2)+pow(point.y(),2))*DeviceSettings::getSingleton()->m_pupilDiameterPixelToFixationDeviationConstant;
    return deviation;
}



QSharedPointer<DeviceDataProcesser> DeviceDataProcesser::getSingleton()
{
    if(m_singleton==nullptr)
    {
        m_singleton.reset(new DeviceDataProcesser());
    }
    return m_singleton;
}
}
