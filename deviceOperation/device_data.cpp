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
}



QSharedPointer<DeviceData> DeviceData::m_singleton=nullptr;

DeviceData::DeviceData()
{
    auto settings=DeviceSettings::getSingleton();
    QString configPath=settings->localConfigPath;
    QString dataPath=settings->localDataPath;
    readLocalConfig(configPath);
    readLocalData(dataPath);
    qDebug()<<m_config.DbPosMappingPtr()[0][0];
    qDebug()<<m_config.DbPosMappingPtr()[0][1];
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
            memcpy(m_config.dataPtr(),data,m_config.dataLen());
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
