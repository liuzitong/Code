#ifndef WORK_LIST_H
#define WORK_LIST_H

#include <QObject>
#include <fc2base_dicom3wklsrv.hxx>
#include <perimeter/main/viewModel/patientVm.h>


namespace Perimeter{
class work_list : public QObject
{
    Q_OBJECT
public:
    explicit work_list(QObject *parent = nullptr);
    Q_INVOKABLE void getPatient();
    static work_list* getSingleton();

public slots:
    void workOnJson(QJsonObject jo);

private:
    static work_list* singleton;
    QString serverIP;
    QString serverPort;
    QString aec;
    QString aet;
    Dicom3::Dicom3WklSrv m_dicom3_wkl_srv;
    void LoadSettings();
signals:
    void newPatient(PatientVm* patient);
};
}
#endif // WORK_LIST_H
