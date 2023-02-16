#ifndef PATIENTMODELLIST_H
#define PATIENTMODELLIST_H
#include "perimeter/base/common/perimeter_def.h"
#include <QAbstractListModel>
#include <perimeter/main/database/patient.h>
#include <QDebug>
namespace Perimeter{
class PERIMETER_API PatientListModelVm:public QAbstractListModel
{
    Q_OBJECT
//    Q_PROPERTY(int currentValue READ current WRITE setCurrent NOTIFY currentChanged)
public:
    Q_INVOKABLE explicit PatientListModelVm(const QVariantList &);
    Q_INVOKABLE virtual ~PatientListModelVm() Q_DECL_OVERRIDE;
//    int current();
//    void setCurrent();
//    PatientListModelVm(Patient_List patient_list);

    Q_SIGNAL void patientListChanged();
    Q_INVOKABLE void getPatientListByTimeSpan(QDate from,QDate to);
    Q_INVOKABLE void getPatientListByPatientId(QString id);
    Q_INVOKABLE void getPatientListByName(QString name,QDate from,QDate to);
    Q_INVOKABLE void getPatientListBySex(int sex,QDate from,QDate to);
    Q_INVOKABLE void getPatientListByBirthDate(QDate date);
//    Q_INVOKABLE void updatePatient(long id,QString patientId, QString name, int sex, QDate date);
    Q_INVOKABLE void deletePatient(long id);
//    Q_INVOKABLE void addPatient(QString patientId,QString name,int sex,QDate date,QDateTime updateTime=QDateTime::currentDateTime());
//    Q_INVOKABLE void hello();
//    Q_SIGNAL void currentChanged();

    // ========================================================================
    // override functions
    // ========================================================================
    //! the total records number
    virtual int        rowCount( const QModelIndex &)  const Q_DECL_OVERRIDE;

    //! return the data object by index and role
    virtual QVariant   data    ( const QModelIndex &idx, int role ) const Q_DECL_OVERRIDE;

    //! return all role id ( and names ) supported by this model
    //! @note we used roles like "id","name","dob", so view can use name to access it.
    virtual QHash<int,QByteArray>  roleNames( ) const Q_DECL_OVERRIDE;

//    //! directly set data object at spec. index
//    virtual bool       setData ( const QModelIndex &, const QVariant &, int ) Q_DECL_OVERRIDE;

//    //! return flags ( characteristics )
//    virtual Qt::ItemFlags  flags ( const QModelIndex & ) const Q_DECL_OVERRIDE;

private:
    void setPatientList(Patient_List patient_list);


    void* m_obj;
    friend class PatientListModelVmPriv;
    Q_DISABLE_COPY(PatientListModelVm);
};
}
#endif // PATIENTMODELLIST_H
