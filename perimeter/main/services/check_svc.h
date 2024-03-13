#ifndef CheckSvc_H
#define CheckSvc_H

#include <perimeter/main/viewModel/patientVm.h>
#include <perimeter/main/viewModel/programVm.h>
#include <perimeter/main/viewModel/checkResultVm.h>
#include <perimeter/main/model/patientModel.h>
#include <QVariantList>
#include <QTimer>
namespace Perimeter{
static QMutex checkStateLock;
class CheckSvcWorker;
class CheckSvc: public QObject
{
    Q_OBJECT
    Q_PROPERTY(PatientVm* patient READ getPatient WRITE setPatient)
    Q_PROPERTY(ProgramVm* program READ getProgram WRITE setProgram)
    Q_PROPERTY(CheckResultVm* checkResult READ getCheckResult WRITE setCheckResult NOTIFY checkResultChanged)
    Q_PROPERTY(int checkState READ getCheckState WRITE setCheckState NOTIFY checkStateChanged)
    Q_PROPERTY(int checkedCount READ getCheckedCount WRITE setCheckedCount NOTIFY checkedCountChanged)
    Q_PROPERTY(int totalCount READ getTotalCount WRITE setTotalCount NOTIFY totalCountChanged)
    Q_PROPERTY(int checkTime READ getCheckTime WRITE setCheckTime NOTIFY checkTimeChanged)
//    Q_PROPERTY(bool devReady READ getDevReady NOTIFY devReadyChanged)
//    Q_PROPERTY(bool devReconnecting READ getReconnecting NOTIFY reconnectingChanged)
    Q_PROPERTY(int deviceStatus READ getDeviceStatus NOTIFY deviceStatusChanged)
    Q_PROPERTY(int castLightAdjustStatus READ getCastLightAdjustStatus NOTIFY castLightAdjustStatusChanged)
    Q_PROPERTY(bool autoAlignPupil READ getAutoAlignPupil WRITE setAutoAlignPupil NOTIFY autoAlignPupilChanged)
    Q_PROPERTY(float pupilDiameter READ getPupilDiameter NOTIFY pupilDiameterChanged)
    Q_PROPERTY(QVariantList dynamicSelectedDots WRITE setInputDots)
    Q_PROPERTY(QPointF nextCheckingDotLoc READ getNextCheckingDotLoc NOTIFY nextCheckingDotLocChanged)
    Q_PROPERTY(QPointF currentCheckingDotLoc READ getCurrentCheckingDotLoc NOTIFY currentCheckingDotLocChanged)
    Q_PROPERTY(int currentCheckingDB READ getCurrentCheckingDB NOTIFY currentCheckingDBChanged)
    Q_PROPERTY(int currentCheckingDotAnswerStatus READ getCurrentCheckingDotAnswerStatus NOTIFY currentCheckingDotAnswerStatusChanged)
    Q_PROPERTY(bool readyToCheck READ getReadyToCheck WRITE setReadyToCheck NOTIFY readyToCheckChanged)
    Q_PROPERTY(QString tips READ getTips WRITE setTips NOTIFY tipsChanged)
    Q_PROPERTY(bool measurePupil READ getMeasurePupil WRITE setMeasurePupil NOTIFY measurePupilChanged)
    Q_PROPERTY(bool measureDeviation READ getMeasureDeviation WRITE setMeasureDeviation NOTIFY measureDeviationChanged)
    Q_PROPERTY(bool eyeMoveAlarm READ getEyeMoveAlarm WRITE setEyeMoveAlarm NOTIFY eyeMoveAlarmChanged)
    Q_PROPERTY(bool envLightAlarm READ getEnvLightAlarm NOTIFY envLightAlarmChanged)
    Q_PROPERTY(bool chinDistAlarm READ getChinDistAlarm NOTIFY chinDistAlarmChanged)
    Q_PROPERTY(bool debugMode READ getDebugMode)
    Q_PROPERTY(bool showCheckingDot READ getShowCheckingDot)
    Q_PROPERTY(QString deviceID READ getDeviceID WRITE setDeviceID NOTIFY deviceIDChanged)



//    Q_PROPERTY(CheckSvcWorker* worker READ getWorker)
public:
    explicit CheckSvc(QObject *parent = nullptr);
    ~CheckSvc();
    Q_INVOKABLE void start();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void resume();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void connectDev();
    Q_INVOKABLE void disconnectDev();
    Q_INVOKABLE void prepareToCheck();

    Q_INVOKABLE void moveChinUp();
    Q_INVOKABLE void moveChinDown();
    Q_INVOKABLE void moveChinLeft();
    Q_INVOKABLE void moveChinRight();
    Q_INVOKABLE void stopMovingChin();
    Q_INVOKABLE void enterCheck();
    Q_INVOKABLE void leaveCheck();
    Q_INVOKABLE void castlightUp();

//signals:
//  void setCheckState(int value);

public:
    PatientVm* getPatient(){return m_patientVm;}void setPatient(PatientVm* value){m_patientVm=value;}
    ProgramVm* getProgram(){return m_programVm;}void setProgram(ProgramVm* value){m_programVm=value;qDebug()<<m_programVm->getName();}
    CheckResultVm* getCheckResult(){return m_checkResultVm;}void setCheckResult(CheckResultVm* value){m_checkResultVm=value;} Q_SIGNAL void checkResultChanged();
    int getCheckState(){return m_checkState;} void setCheckState(int value){checkStateLock.lock();m_checkState=value;std::cout<<"checkState:"<<m_checkState<<std::endl;emit checkStateChanged();checkStateLock.unlock();}Q_SIGNAL void checkStateChanged();
    int getCheckedCount(){return m_checkedCount;}void setCheckedCount(int value){m_checkedCount=value;emit checkedCountChanged();}Q_SIGNAL void checkedCountChanged();
    int getTotalCount(){return m_totalCount;}void setTotalCount(int value){m_totalCount=value;emit totalCountChanged();}Q_SIGNAL void totalCountChanged();
    int getCheckTime(){return m_checkTime;}void setCheckTime(int value){m_checkTime=value;/*qDebug()<<"checkTime:"+QString::number(m_checkTime);*/emit checkTimeChanged();}Q_SIGNAL void checkTimeChanged();
//    bool getDevReady();Q_SIGNAL void devReadyChanged();
    int getDeviceStatus();Q_SIGNAL void deviceStatusChanged();
//    bool getReconnecting();Q_SIGNAL void reconnectingChanged();
    int getCastLightAdjustStatus();Q_SIGNAL void castLightAdjustStatusChanged();
    bool getAutoAlignPupil();void setAutoAlignPupil(bool autoAlign);Q_SIGNAL void autoAlignPupilChanged();
    float getPupilDiameter();Q_SIGNAL void pupilDiameterChanged();
    void setInputDots(QVariantList value);
    QPointF getCurrentCheckingDotLoc(){return m_currentCheckingDotLoc;}Q_SIGNAL void currentCheckingDotLocChanged();
    QPointF getNextCheckingDotLoc(){return m_nextCheckingDotLoc;}Q_SIGNAL void nextCheckingDotLocChanged();
    int getCurrentCheckingDB(){return m_currentCheckingDB;}Q_SIGNAL void currentCheckingDBChanged();
    int getCurrentCheckingDotAnswerStatus(){return m_currentCheckingDotAnswerStatus;}Q_SIGNAL void currentCheckingDotAnswerStatusChanged();
//    int getCurrentCheckingDot(){return m_currentChecking}
    bool getReadyToCheck(){return m_readyToCheck;}void setReadyToCheck(bool value){m_readyToCheck=value;emit readyToCheckChanged();} Q_SIGNAL void readyToCheckChanged();
    QString getTips(){return m_tips;}void setTips(QString value){m_tips=value;emit tipsChanged();}Q_SIGNAL void tipsChanged();
    bool getMeasurePupil();void setMeasurePupil(bool value);Q_SIGNAL void measurePupilChanged(bool value);
    bool getMeasureDeviation();void setMeasureDeviation(bool value);Q_SIGNAL void measureDeviationChanged(bool value);
    bool getEyeMoveAlarm();void setEyeMoveAlarm(bool value);Q_SIGNAL void eyeMoveAlarmChanged(bool value);
    bool getEnvLightAlarm();Q_SIGNAL void envLightAlarmChanged();
    bool getChinDistAlarm();Q_SIGNAL void chinDistAlarmChanged();
    bool getDebugMode();
    bool getShowCheckingDot();
    QString getDeviceID(){return m_deviceID;}void setDeviceID(QString id){m_deviceID=id;emit deviceIDChanged();};Q_SIGNAL void deviceIDChanged();
//    CheckSvcWorker* getWorker(){return m_worker;}

private:
    CheckSvcWorker* m_worker;
    QThread m_workerThread;
    QString m_tips;
    QAtomicInt m_checkState=5;                        //0:start,1:checking,2:pausing,3:stopping,4:finishing,5:stopped,6:finished.
    int m_checkedCount=0;
    int m_totalCount=0;
    int m_checkTime=0;
    QPointF m_currentCheckingDotLoc={999,999};
    QPointF m_nextCheckingDotLoc={999,999};
    int m_currentCheckingDB;
    int m_currentCheckingDotAnswerStatus;
    bool m_readyToCheck=false;
    bool m_atCheckingPage=false;
//    bool m_measurePupilDiameter;
//    bool m_measurePupilDeviation;
//    bool m_eyeMoveAlarm;

    QList<QPointF> m_dynamicSelectedDots;
    PatientVm* m_patientVm;
    ProgramVm* m_programVm;
    CheckResultVm* m_checkResultVm;
    QTimer m_castLightDimdownTimer;
    QString m_deviceID="Not Connected.";
};
}
#endif // CheckSvc_H
