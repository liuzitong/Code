#include "device_data.h"
#include "device_settings.h"
#include <QFile>
#include <QDebug>

namespace DevOps{
LocalTableData::LocalTableData()
{
    m_data=QSharedPointer<int>(new int[dataLen]{0});
    memset(m_data.data(),-1,dataLen*sizeof (int));
    m_mainPosTableData=SingleTableData(31*3,31,m_data.data());
    m_secondaryPosTableData=SingleTableData(31*3,31,m_data.data()+31*3*31);
    m_dbAngleDampingTableData=SingleTableData(46,1,m_data.data()+31*3*31*2);
    m_xyDistTableData=SingleTableData(25,2,m_data.data()+31*3*31*2+46);
    m_focalLengthMotorPosMappingData=SingleTableData(25,6,m_data.data()+31*3*31*2+46+25*2);
    m_dynamicLenAndTimeData=SingleTableData(9,2,m_data.data()+31*3*31*2+46+25*2+25*6);
}


QSharedPointer<DeviceData> DeviceData::m_singleton=nullptr;

DeviceData::DeviceData()
{
    auto settings=DeviceSettings::getSingleton();
    QString configPath=settings->localConfigPath;
    qDebug()<<configPath;
    QString dataPath=settings->localDataPath;
    qDebug()<<dataPath;
    if(!readLocalConfig(configPath)) qDebug()<<"read config error!";
    if(!readLocalData(dataPath)) qDebug()<<"read local data error!";
//    qDebug()<<m_config.shutterOpenPosRef();
//    qDebug()<<m_config.DbPosMappingPtr()[0][0];
//    qDebug()<<m_config.DbPosMappingPtr()[0][1];
}

QSharedPointer<DeviceData> DeviceData::getSingleton()
{

    if(m_singleton==nullptr)
    {
        m_singleton.reset(new DeviceData());
    }
    return m_singleton;
}

bool DeviceData::readLocalConfig(QString filePath)
{
    QFile file(filePath);
    if(file.exists())
    {
        if(file.open(QIODevice::ReadOnly))
        {
            QByteArray data=file.readAll();
            if(data.length()!=m_config.dataLen())
            {
                return false;
            }
            m_config=UsbDev::Config(data);
        }
        file.flush();
        file.close();
    }
    return true;
}

bool DeviceData::readLocalData(QString filePath)
{
    QFile file(filePath);
    if(file.exists())
    {
        if(file.open(QIODevice::ReadOnly))
        {
            char* destPtr=(char*)m_localTableData.m_data.data();
            int dataLen=m_localTableData.dataLen;
            QByteArray data=file.readAll();
            if(data.length()!=dataLen)
            {
                return false;
            }
            memcpy(destPtr,data.data(),data.length());
        }
        file.flush();
        file.close();
    }
    return true;
}

}
