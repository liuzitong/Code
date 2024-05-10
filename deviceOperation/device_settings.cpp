#include "device_settings.h"
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <iostream>
#include <QFile>
#include <QMutex>

QSharedPointer<DeviceSettings> DeviceSettings::m_singleton=nullptr;

DeviceSettings::DeviceSettings()
{
    QFile loadFile(R"(deviceData/settings.json)");
    loadFile.open(QIODevice::ReadOnly);
    QByteArray allData = loadFile.readAll();
    loadFile.close();

    QJsonParseError jsonError;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(allData, &jsonError));

    if(jsonError.error != QJsonParseError::NoError)
    {
        qDebug() << "json error!" << jsonError.errorString();
        return;
    }

    QJsonObject m_rootObj = jsonDoc.object();
    QStringList keys = m_rootObj.keys();
    for(int i = 0; i < keys.size(); i++)
//    {
//        qDebug() << "key" << i << " is:" << keys.at(i);
//    }


    m_PID = m_rootObj.value("PID").toString();
    m_VID = m_rootObj.value("VID").toString();

    m_pupilAutoAlignPixelTolerance=m_rootObj.value("pupilAutoAlignPixelTolerance").toInt();
    m_pupilAutoAlignStep=m_rootObj.value("pupilAutoAlignStep").toInt();
    localConfigPath=m_rootObj.value("localConfigPath").toString();
    localDataPath=m_rootObj.value("localDataPath").toString();

    m_pixelDistFromPupilCenterToMiddleReflectionDot=m_rootObj.value("pixelDistFromPupilCenterToMiddleReflectionDot").toInt();
    m_pupilDiameterPixelToMillimeterConstant=m_rootObj.value("pupilDiameterPixelToMillimeterConstant").toDouble();
    m_pupilDeviationPixelToNumberConstant=m_rootObj.value("pupilDeviationPixelToNumberConstant").toDouble();
    m_castLightDAChangeRate=m_rootObj.value("castLightDAChangeRate").toDouble();
    m_castLightDAChangeInterval=m_rootObj.value("castLightDAChangeInterval").toInt();
    std::cout<<"m_castLightDAChangeInterval:"<<m_castLightDAChangeInterval<<std::endl;
    m_castLightDAChangeMinStep=m_rootObj.value("castLightDAChangeMinStep").toInt();
    m_castLightTargetColor=m_rootObj.value("castLightTargetColor").toInt();
    m_castLightTargetSize=m_rootObj.value("castLightTargetSize").toInt();
    m_castLightDADifferenceTolerance=m_rootObj.value("castLightDADifferenceTolerance").toDouble();
    m_castLightStablizeWaitingTime=m_rootObj.value("castLightStablizeWaitingTime").toInt();
    std::cout<<"m_castLightStablizeWaitingTime:"<<m_castLightStablizeWaitingTime<<std::endl;
    m_skipAdjustCastLight=m_rootObj.value("skipAdjustCastLight").toBool();
    m_waitingTime=m_rootObj.value("waitingTime").toInt();
    m_beepCount=m_rootObj.value("beepCount").toInt();
    m_beepDuration=m_rootObj.value("beepDuration").toInt();
    m_beepInterval=m_rootObj.value("beepInterval").toInt();
    m_reflectDotDist=m_rootObj.value("reflectDotDist").toInt();
    m_reconnectTime=m_rootObj.value("reconnectTime").toInt();
    m_deviationCalibrationWatingTime=m_rootObj.value("castLightStablizeWaitingTime").toInt();

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

    QFile loadFile2(R"(deviceData/castLightAdjustStatus.json)");
    loadFile2.open(QIODevice::ReadOnly);
    QByteArray allData2 = loadFile2.readAll();
    loadFile2.close();

    QJsonParseError jsonError2;
    QJsonDocument jsonDoc2(QJsonDocument::fromJson(allData2, &jsonError2));

    if(jsonError2.error != QJsonParseError::NoError)
    {
        qDebug() << "json error!" << jsonError2.errorString();
        return;
    }

    QJsonObject m_rootObj2 = jsonDoc2.object();
    m_castLightDA=m_rootObj2.value("castLightDA").toInt();
    m_castLightLastAdjustedDate=m_rootObj2.value("castLightLastAdjustedDate").toString();

    QFile loadFile3(R"(deviceData/deviationCalibrationStatus.json)");
    loadFile3.open(QIODevice::ReadOnly);
    QByteArray allData3 = loadFile3.readAll();
    loadFile3.close();

    QJsonParseError jsonError3;
    QJsonDocument jsonDoc3(QJsonDocument::fromJson(allData3, &jsonError3));

    if(jsonError3.error != QJsonParseError::NoError)
    {
        qDebug() << "json error!" << jsonError3.errorString();
        return;
    }
    QJsonObject m_rootObj3 = jsonDoc3.object();
    m_deviationCalibrationXMotorDeviation=m_rootObj3.value("deviationCalibrationXMotorDeviation").toInt();
    m_deviationCalibrationYMotorDeviation=m_rootObj3.value("deviationCalibrationYMotorDeviation").toInt();
    m_deviationCalibrationStep=m_rootObj3.value("deviationCalibrationStep").toDouble();

}

QSharedPointer<DeviceSettings> DeviceSettings::getSingleton()
{
    static QMutex mutex;
    mutex.lock();
    if (m_singleton==nullptr)
    {
        m_singleton.reset(new DeviceSettings());
    }
    mutex.unlock();
    return m_singleton;
}

void DeviceSettings::saveCastLightAdjustStatus()
{
    QJsonObject rootObj;
    QJsonDocument jsonDoc(rootObj);
    rootObj["castLightDA"]=m_castLightDA;
    rootObj["castLightLastAdjustedDate"]=m_castLightLastAdjustedDate;
    auto data=jsonDoc.toJson();
    QFile loadFile(R"(deviceData/castLightAdjustStatus.json)");
    loadFile.open(QIODevice::WriteOnly);
    loadFile.write(data,data.length());
    loadFile.close();
}

void DeviceSettings::saveDeviationCalibrationStatus()
{
    QFile loadFile(R"(deviceData/deviationCalibrationStatus.json)");
    loadFile.open(QIODevice::ReadWrite);
    QByteArray allData = loadFile.readAll();
    QJsonParseError jsonError;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(allData, &jsonError));
    QJsonObject rootObj = jsonDoc.object();
    rootObj["deviationCalibrationXMotorDeviation"]=m_deviationCalibrationXMotorDeviation;
    rootObj["deviationCalibrationYMotorDeviation"]=m_deviationCalibrationYMotorDeviation;
    jsonDoc.setObject(rootObj);
    auto data=jsonDoc.toJson();
    loadFile.write(data,data.length());
    loadFile.close();
}

void DeviceSettings::saveReconTimes()
{
    QFile loadFile(R"(deviceData/reconTimes.json)");
    loadFile.open(QIODevice::ReadWrite);
    QByteArray allData = loadFile.readAll();
    QJsonParseError jsonError;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(allData, &jsonError));
    QJsonObject rootObj = jsonDoc.object();
    m_reconTimes+=rootObj["reconTimes"].toInt();
    rootObj["reconTimes"]=m_reconTimes;
    jsonDoc.setObject(rootObj);
    auto data=jsonDoc.toJson();
    loadFile.write(data,data.length());
    loadFile.close();
}
