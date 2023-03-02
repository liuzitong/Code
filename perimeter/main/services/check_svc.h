#ifndef CheckSvc_H
#define CheckSvc_H

#include <perimeter/main/viewModel/patientVm.h>
#include <perimeter/main/viewModel/programVm.h>
#include <perimeter/main/viewModel/checkResultVm.h>
#include <perimeter/main/model/patientModel.h>

namespace Perimeter{
class CheckSvcWorker;
class CheckSvc: public QObject
{
    Q_OBJECT
    Q_PROPERTY(PatientVm* patient READ getPatient WRITE setPatient)
    Q_PROPERTY(ProgramVm* program READ getProgram WRITE setProgram)
    Q_PROPERTY(CheckResultVm* checkResult READ getCheckResult WRITE setCheckResult NOTIFY checkResultChanged)
    Q_PROPERTY(int checkState READ getCheckState WRITE setCheckState NOTIFY checkStateChanged)
    Q_PROPERTY(int checkedCount READ getCheckedCount WRITE setCheckedCount NOTIFY checkedCountChanged)
    Q_PROPERTY(bool devReady READ getDevReady NOTIFY devReadyChanged)
    Q_PROPERTY(bool autoAlignPupil READ getAutoAlignPupil WRITE setAutoAlignPupil)
    Q_PROPERTY(bool pupilDiameter READ getPupilDiameter NOTIFY pupilDiameterChanged)
public:
    explicit CheckSvc(QObject *parent = nullptr);
    ~CheckSvc();
    Q_INVOKABLE void start();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void resume();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void connectDev();

//signals:
//  void setCheckState(int value);

public:
    PatientVm* getPatient(){return m_patientVm;}void setPatient(PatientVm* value){m_patientVm=value;}
    ProgramVm* getProgram(){return m_programVm;}void setProgram(ProgramVm* value){m_programVm=value;qDebug()<<m_programVm->getName();}
    CheckResultVm* getCheckResult(){return m_checkResultVm;}void setCheckResult(CheckResultVm* value){m_checkResultVm=value;} Q_SIGNAL void checkResultChanged();
    int getCheckState(){return m_checkState;} void setCheckState(int value){m_checkState=value;emit checkStateChanged();}Q_SIGNAL void checkStateChanged();
    int getCheckedCount(){return m_checkedCount;}void setCheckedCount(int value){m_checkedCount=value;emit checkedCountChanged();}Q_SIGNAL void checkedCountChanged();
    bool getDevReady();Q_SIGNAL void devReadyChanged();
    bool getAutoAlignPupil();void setAutoAlignPupil(bool autoAlign);
    float getPupilDiameter();Q_SIGNAL void pupilDiameterChanged();

private:
    CheckSvcWorker* m_worker;
    QThread m_workerThread;
    int m_checkState=3;                        //0:start,1:checking,2:pausing,3:stopped,4:finished
    int m_checkedCount=0;
    PatientVm* m_patientVm;
    ProgramVm* m_programVm;
    CheckResultVm* m_checkResultVm;

};
}
#endif // CheckSvc_H
