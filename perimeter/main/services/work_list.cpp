#include "work_list.h"
#include <QProcess>
#include <fc2base_dicom3wklsrv.hxx>
#include "perimeter/main/viewModel/patientVm.h"

namespace Perimeter{

work_list* work_list::singleton=nullptr;

work_list::work_list(QObject *parent): QObject{parent}
{
    LoadSettings();
    connect(&m_dicom3_wkl_srv, &Dicom3::Dicom3WklSrv::saveDicom3Patient,this,&work_list::workOnJson);
}

void work_list::getPatient()
{
    static qint64 exec_time            = 0;
    if(QDateTime::currentSecsSinceEpoch() - exec_time <= 3 && QDateTime::currentSecsSinceEpoch() - exec_time > -10){
        return ;
    }
    exec_time                           = QDateTime::currentSecsSinceEpoch();
    Dicom3::Dicom3WklReqData req;
    Dicom3::Dicom3WklReqWklSrvData srv_data;
    srv_data.setIpAddress(serverIP);
    srv_data.setIpPort(serverPort.toInt());
    srv_data.setAeTitle(aec);
    srv_data.setHostAeTitle(aet);
    req.setSrvData( srv_data );
    m_dicom3_wkl_srv.post_ts(req);
}

work_list* work_list::getSingleton()
{
    if(singleton==nullptr)
    {
        singleton=new work_list();
    }
    return singleton;
}

void work_list::workOnJson(QJsonObject jo)
{

    qDebug()<<jo;
    if(jo["pid"].toString().isEmpty()) { return; }
    auto patientVm=new PatientVm(QVariantList{});


    // patientVm->setBirthDate();
    QString name,birthDate;
    int sex;
    if(jo["name"].toString().trimmed()=="")
    {
        name=jo["firstName"].toString()+" "+jo["lastName"].toString();
    }
    else
        name=jo["name"].toString();
    birthDate=jo["birthday"].toString();

    int sex_temp=jo["sex"].toInt();
    if(sex_temp==1) sex=(int)sex::male;
    else if(sex_temp==2) sex=(int)sex::female;
    else sex=(int)sex::unknown;
    // QDate date=QDate::fromString(birthDate,"yyyy-MM-dd");
    // birthDate=date.toString("yyyy/MM/dd");

    patientVm->setSex(sex);
    patientVm->setBirthDate(birthDate);
    patientVm->setName(name);
    patientVm->setPatientID(jo["pid"].toString());
    patientVm->insert();
    QProcess proc;
    proc.execute("taskkill /im ipckbdsrv.exe /f");
    emit newPatient(patientVm);
}

void work_list::LoadSettings()
{
    QFile jsonFile(R"(./workListSettings.json)");
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

}
