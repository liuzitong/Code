#include "settings.h"
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <iostream>
#include <QFile>

Settings::Settings()
{
    QFile loadFile(R"(data/settings.json)");

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

    m_updateIOInfo=m_rootObj.value("updateIOInfo").toBool();
    m_updateRefreshInfo=m_rootObj.value("updateRefreshInfo").toBool();
    m_updateRefreshIOInfo=m_rootObj.value("updateRefreshIOInfo").toBool();

    localConfig=m_rootObj.value("localConfig").toString();
    localData=m_rootObj.value("localData").toString();

    QJsonArray spotSizeToSlot=m_rootObj.value("spotSizeToSlot").toArray();
    QJsonArray colorToSlot=m_rootObj.value("colorToSlot").toArray();

    for(auto i:spotSizeToSlot)
    {
        QJsonObject obj=i.toObject();
        QString spotSize=obj["spotSize"].toString();
        int slot=obj["Slot"].toInt();
        m_spotSizeToSlot.append({spotSize,slot});
    }

    for(auto i:colorToSlot)
    {
        QJsonObject obj=i.toObject();
        QString color=obj["Color"].toString();
        int slot=obj["Slot"].toInt();
        m_colorToSlot.append({color,slot});
    }
}







//void Settings::Write()
//{
//    QJsonObject obj;
//    obj.insert("VID",m_VID);
//    obj.insert("PID",m_PID);

//    QJsonArray array,array2;
//    for(auto&v:m_spotSizeToSlot){
//        QJsonObject spotSlot;
//        spotSlot.insert("spotSize",v.first);
//        spotSlot.insert("Slot",v.second);
//        array2.append(spotSlot);
//    }

//    for(auto&v:m_dotInfoList){
//        QJsonObject dotInfo;
//        dotInfo.insert("coordx",v.coordX);
//        dotInfo.insert("coordy",v.coordY);
//        dotInfo.insert("motorXPos",v.motorXPos);
//        dotInfo.insert("motorYPos",v.motorYPos);
//        dotInfo.insert("focalDistance",v.focalDistance);
//        array.append(dotInfo);
//    }
//    obj.insert("dotInfoList",array);
//    obj.insert("spotSizeToSlot",array2);
//    qDebug()<<obj;
//    QJsonDocument qjDoc(obj);
//    auto ba= qjDoc.toJson();
//    QFile file("config.json");
//    file.open(QIODevice::WriteOnly);
//    file.write(ba);
//    file.close();
//}




