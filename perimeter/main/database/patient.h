#ifndef Patient_H
#define Patient_H
#include "precompiled.h"
#include <QxOrm_Impl.h>
#include <QDate>
#include "perimeter/base/common/perimeter_def.h"
#include "qxpack/indcom/afw/qxpack_ic_viewmodelbase.hxx"
#include <QObject>
#include <QDateTime>

class CheckResult;
enum class sex { male, female, unknown };
class QX_DLL_EXPORT_API Patient/*:public QObject*/
{

//    Q_OBJECT
//    Q_PROPERTY(long id READ getID WRITE setID)
//    Q_PROPERTY(QString patientId READ getPatientID WRITE setPatientID)
//    Q_PROPERTY(QString name READ getName WRITE setName)
//    Q_PROPERTY(int sex READ getSex WRITE setSex)
//    Q_PROPERTY(QDate date READ getBirthDate WRITE setBirthDate)
//    Q_PROPERTY(QDateTime lastUpdate READ getLastUpdate WRITE setLastUpdate)

public:

// -- typedef
    typedef std::shared_ptr<CheckResult> CheckResult_ptr;
    typedef std::vector<CheckResult_ptr> CheckResult_List;

    long m_id;
    QString m_patientId;
    QString m_name;
    sex m_sex;
    QDate m_birthDate;
//    QString m_baselineDiagnosis;
//    QString m_threeFollowUpsDiagnosis;
    QString m_rx;
    QDateTime m_lastUpdate;
    CheckResult_List m_checkResultX;
    Patient() = default;
    Patient(QString patientId,QString name,sex sex,QDate date,QString rx,QDateTime lastUpdate=QDateTime::currentDateTime()):m_patientId(patientId),m_name(name),m_sex(sex),m_birthDate(date),m_rx(rx),m_lastUpdate(lastUpdate){}
    Patient(long id,QString patientId,QString name,sex sex,QDate date,QString rx,QDateTime lastUpdate=QDateTime::currentDateTime()):m_id(id),m_patientId(patientId),m_name(name),m_sex(sex),m_birthDate(date),m_rx(rx),m_lastUpdate(lastUpdate){}
    virtual ~Patient(){}

//    long getID(){return m_id;}                      void setID(long value){m_id=value;}
//    QString getPatientID(){return m_patientId;}     void setPatientID(QString value){m_patientId=value;}
//    QString getName(){return m_name;}               void setName(QString value){m_name=value;}
//    int getSex(){return static_cast<int>(m_sex);}   void setSex(int value){m_sex=static_cast<sex>(value);}
//    QDate getBirthDate(){return m_birthDate;}       void setBirthDate(QDate value){m_birthDate=value;}
//    QDateTime getLastUpdate(){return m_lastUpdate;} void setLastUpdate(QDateTime value){m_lastUpdate=value;}

};

typedef std::shared_ptr<Patient> Patient_ptr;
typedef QList<Patient_ptr> Patient_List;
QX_REGISTER_PRIMARY_KEY(Patient, long)
QX_REGISTER_HPP_QX_API(Patient, qx::trait::no_base_class_defined, 0)

#endif



