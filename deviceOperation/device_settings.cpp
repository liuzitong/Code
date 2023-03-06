#include "device_settings.h"
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <iostream>
#include <QFile>

QSharedPointer<DeviceSettings> DeviceSettings::m_singleton=nullptr;

DeviceSettings::DeviceSettings()
{
    QFile loadFile(R"(deviceData/settings.json)");

    if(!loadFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "could't open projects json";
        return;
    }

    QByteArray allData = loadFile.readAll();
    loadFile.close();

    QJsonParseError jsonError;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(allData, &jsonError));

    if(jsonError.error != QJsonParseError::NoError)
    {
        qDebug() << "json error!" << jsonError.errorString();
        return;
    }

    m_rootObj = jsonDoc.object();
    QStringList keys = m_rootObj.keys();
//    for(int i = 0; i < keys.size(); i++)
//    {
//        qDebug() << "key" << i << " is:" << keys.at(i);
//    }


    m_PID = m_rootObj.value("PID").toString();
    m_VID = m_rootObj.value("VID").toString();

    m_pupilAutoAlignPixelTolerance=m_rootObj.value("pupilAutoAlignPixelTolerance").toInt();

    localConfigPath=m_rootObj.value("localConfigPath").toString();
    localDataPath=m_rootObj.value("localDataPath").toString();

    m_pupilGreyLimit=m_rootObj.value("pupilGreyLimit").toInt();
    m_pupilPixelDiameterLimit=m_rootObj.value("pupilPixelDiameterLimit").toInt();
    m_pupilDiameterPixelToMillimeterConstant=m_rootObj.value("pupilDiameterPixelToMillimeterConstant").toDouble();
    m_pupilDiameterPixelToFixationDeviationConstant=m_rootObj.value("pupilDiameterPixelToFixationDeviationConstant").toDouble();
    auto motorSpeed=m_rootObj.value("motorSpeed").toArray();
    for(int i=0;i<motorSpeed.count();i++)
    {
        if(i<5) m_5MotorSpeed[i]=motorSpeed[i].toInt();
        else  m_motorChinSpeed[i-5]=motorSpeed[i-5].toInt();
    }

    QJsonArray spotSizeToSlot=m_rootObj.value("spotSizeToSlot").toArray();
    for(auto i:spotSizeToSlot)
    {
        QJsonObject obj=i.toObject();
        int spotSize=obj["spotSize"].toInt();
        int slot=obj["Slot"].toInt();
        m_spotSizeToSlot.append({spotSize,slot});
    }

    QJsonArray colorToSlot=m_rootObj.value("colorToSlot").toArray();
    for(auto i:colorToSlot)
    {
        QJsonObject obj=i.toObject();
        int color=obj["Color"].toInt();
        int slot=obj["Slot"].toInt();
        m_colorToSlot.append({color,slot});
    }
}

QSharedPointer<DeviceSettings> DeviceSettings::getSingleton()
{
    if (m_singleton==nullptr)
    {
        m_singleton.reset(new DeviceSettings());
    }
    return m_singleton;
}
