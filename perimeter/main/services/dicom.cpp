#include "dicom.h"
#include "qjsonarray.h"
#include "qjsonobject.h"

#include <QFile>
#include <QJsonDocument>
#include <QProcess>




Dicom* Dicom::singleton=nullptr;

Dicom::Dicom()
{
    QFile jsonFile(R"(./dicomSettings.json)");
    jsonFile.open(QIODevice::ReadOnly);
    QJsonParseError jsonParserError;
    auto JsonDoc = QJsonDocument::fromJson(jsonFile.readAll(),&jsonParserError);
    auto jsonArray=JsonDoc.array();
    jsonFile.close();

    auto jo=JsonDoc.object();
    serverIP=jo["serverIP"].toString();
    serverPort=jo["severPort"].toString();
    aec=jo["aec"].toString();
    aet=jo["aet"].toString();
}



Dicom *Dicom::getSingleton(){
    if(singleton==nullptr)
    {
        singleton=new Dicom();
    }
    return singleton;
}



QString Dicom::cmdPdfToDcm(QVector<QPair<DcmTagKey, QString>> infos)
{
    QString cmd=QString("pdf2dcm.exe")+" "+pdfFilePath+" "+dcmFilePath;
    for(auto &i:infos)
    {
        auto tagKey=i.first;
        auto value=i.second;
        auto group=tagKey.getGroup();
        auto element=tagKey.getElement();
        QString kv = QString(" ")+"-k "+QString::number(group,16)+","+QString::number(element,16)+"="+value;
        cmd+=kv;
    }
    cmd+=" -k 0x0008,0x0005=utf-8";
    return cmd;
}

bool Dicom::upLoadDcm(PatientModel patientModel)
{

    QVector<QPair<DcmTagKey, QString>> infos;
    infos=
        {
            {DCM_PatientName,patientModel.m_name},
            {DCM_PatientAge,QString::number(patientModel.m_age)},
            {DCM_PatientSex,patientModel.m_sex==sex::male?"M":patientModel.m_sex==sex::female?"F":"O"},
            {DCM_PatientID,patientModel.m_patientId},
            {DCM_PatientBirthDate,patientModel.m_birthDate.toString("yyyyMMdd")},
            {DCM_StudyDate,QDateTime::currentDateTime().date().toString("yyyyMMdd")},
            {DCM_StudyTime,QDateTime::currentDateTime().time().toString("hhmmss")},
        };
    QString cmdLine=cmdPdfToDcm(infos);
    QProcess p(nullptr);
    p.start(cmdLine);
    if(p.waitForFinished())
    {
        QString cmdUpload=QString("storescu.exe -xy "+serverIP+" "+serverPort+" -aec "+aec+" -aet "+aet+" "+dcmFilePath);
        p.start(cmdUpload);
        if(p.waitForFinished())
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

