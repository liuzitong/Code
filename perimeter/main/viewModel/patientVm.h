#ifndef PATIENTVM_H
#define PATIENTVM_H
#include "perimeter/base/common/perimeter_def.h"
#include "perimeter/main/database/patient.h"
#include "perimeter/main/model/patientmodel.h"
//#include "qxpack/indcom/afw/qxpack_ic_viewmodelbase.hxx"
#include <memory>
#include <QDate>
#include <QDateTime>
#include "qxpack/indcom/afw/qxpack_ic_viewmodelbase.hxx"

namespace Perimeter{


class PERIMETER_API PatientVm: public QObject
{
    Q_OBJECT
    Q_PROPERTY(long id READ getID WRITE setID)
    Q_PROPERTY(QString patientId READ getPatientID WRITE setPatientID NOTIFY patientIdChanged)
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(int sex READ getSex WRITE setSex NOTIFY sexChanged)
    Q_PROPERTY(QString birthDate READ getBirthDate WRITE setBirthDate NOTIFY birthDateChanged)
    Q_PROPERTY(QObject* rx READ getRx WRITE setRx NOTIFY rxChanged)
    Q_PROPERTY(QDateTime lastUpdate READ getLastUpdate WRITE setLastUpdate)
    Q_PROPERTY(int age READ getAge)

public:
//    Q_INVOKABLE PatientVm();
    Q_INVOKABLE explicit PatientVm(const QVariantList &);
//    Q_INVOKABLE void deletePatient();
    Q_INVOKABLE virtual ~PatientVm() Q_DECL_OVERRIDE;
    Q_INVOKABLE void update();
    Q_INVOKABLE void insert();
    Q_INVOKABLE QObject* getLastCheckResult();
//    Q_INVOKABLE int age();
//    PatientVm& operator=(const PatientVm& other);
    long getID();void setID(long value);
    QString getPatientID();void setPatientID(QString value);Q_SIGNAL void patientIdChanged();
    QString getName();void setName(QString value);Q_SIGNAL void nameChanged();
    int getSex();void setSex(int value);Q_SIGNAL void sexChanged();
    QString getBirthDate();void setBirthDate(QString value);Q_SIGNAL void birthDateChanged();
    QObject* getRx(){return m_rx;}void setRx(QObject* value){m_rx=value;emit rxChanged(value);}Q_SIGNAL void rxChanged(QObject* value);
    QDateTime getLastUpdate();void setLastUpdate(QDateTime lastUpdate);
    int getAge(){return m_data->m_age;}
    QSharedPointer<PatientModel> getModel();

private:
    QSharedPointer<PatientModel> m_data;
    Patient_ptr getPatientData();
    QObject* m_rx=nullptr;

};
}
#endif // PATIENTVM_H
