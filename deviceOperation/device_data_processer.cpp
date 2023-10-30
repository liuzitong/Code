#include "device_data_processer.h"
#include <QDebug>
#include <usbdev/main/usbdev_devctl.hxx>
#include <device_operation.h>
#include <device_settings.h>

namespace DevOps{

//QSharedPointer<DeviceDataProcesser> DeviceDataProcesser::m_singleton=nullptr;
//bool DeviceDataProcesser::isMainDotInfoTable=true;

int DeviceDataProcesser::interpolation(int value[], QPointF loc)
{
    double secondVal[2];
    secondVal[0]=value[0]+(value[1]-value[0])*(loc.x()/6.0);
    secondVal[1]=value[2]+(value[3]-value[2])*(loc.x()/6.0);
    int ret=secondVal[0]+(secondVal[1]-secondVal[0])*(loc.y()/6.0);
    return ret;
}

int DeviceDataProcesser::getFocusMotorPosByDist(int focalDist, int spotSlot,int focalMotorPosCorrection)
{

//    auto config=DeviceOperation::getSingleton()->m_devCtl->config();
    auto localData=DeviceData::getSingleton()->m_localTableData;
//    if(config.isEmpty()) {return 0;}
    auto map = localData.m_focalLengthMotorPosMappingData;
    int indexDist= floor(focalDist/10)-8;
    int pos1=map(indexDist,spotSlot-1);
    int pos2=map(indexDist+1,spotSlot-1);
    int focalMotorPos=pos1+(pos2-pos1)*(focalDist%10)/10+focalMotorPosCorrection;
    return focalMotorPos;
}

CoordMotorPosFocalDistInfo DeviceDataProcesser::getXYMotorPosAndFocalDistFromCoord(const QPointF loc,bool isMainDotInfoTable)
{
    CoordMotorPosFocalDistInfo coordSpacePosInfo;
//    if(loc.x()<-30)
//    {
//        isMainDotInfoTable=true;
//    }
//    else if(loc.x()>30)
//    {
//        isMainDotInfoTable=false;
//    }
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
    fourDots[0]={tableData(y1*3,x1),tableData(y1*3+1,x1),tableData(y1*3+2,x1)};fourDots[1]={tableData(y1*3,x2),tableData(y1*3+1,x2),tableData(y1*3+2,x2)};
    fourDots[2]={tableData(y2*3,x1),tableData(y2*3+1,x1),tableData(y2*3+2,x1)};fourDots[3]={tableData(y2*3,x2),tableData(y2*3+1,x2),tableData(y2*3+2,x2)};
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

quint16 DeviceDataProcesser::calcCrc(quint8 *p_data, int data_len)
{
    int32_t i;
    uint16_t crc_value = 0xFFFF; /* 定义一个16位无符号类型的变量，并初始化为0xFFFF */

    while(data_len--)
    {
        /* 数据包中的字节与CRC变量中的低字节进行异或运算，结果存回CRC变量 */
        crc_value ^= *p_data++;
        for (i = 0; i < 8; i++) {
            if (crc_value & 0x0001) {
                /* 如果最低位为1：将CRC变量与固定值0xA001进行异或运算 */
                crc_value = (crc_value >> 1) ^ 0xA001;
            } else {
                /* 如果最低位为0：重复第3步(配合计算流程来阅读代码) */
                crc_value >>= 1;
            }
        }
    }

    return crc_value;

}



//QSharedPointer<DeviceDataProcesser> DeviceDataProcesser::getSingleton()
//{
//    if(m_singleton==nullptr)
//    {
//        m_singleton.reset(new DeviceDataProcesser());
//    }
//    return m_singleton;
//}


}




















