#include "patientListModelVm.h"
#include "perimeter/base/common/perimeter_memcntr.hxx"
#include <QList>


namespace Perimeter{

enum PatientRoles
{
    Id=Qt::UserRole + 1,
    patientId ,
    name,
    birthDate,
    sex,
    lastUpdate
};

}
namespace Perimeter{
#define T_PrivPtr( o )  perimeter_objcast( PatientListModelVmPriv*, o )
class PERIMETER_API PatientListModelVmPriv
{
private:
    PatientListModelVm* m_parent;
    Patient_List m_list;
public:
    PatientListModelVmPriv(PatientListModelVm* pa)
    {
        m_parent=pa;
    }
    inline int RowCount(){return m_list.count();}
    QVariant data(const QModelIndex &idx, int role) const
    {
        int index=idx.row();
        Patient_ptr pp=m_list[index];
        switch (role)
        {
        case (PatientRoles::Id):return (int)pp->m_id;
        case (PatientRoles::patientId): return pp->m_patientId;
        case (PatientRoles::name): return pp->m_name;
        case (PatientRoles::birthDate): return pp->m_birthDate.toString("yyyy/MM/dd");
        case (PatientRoles::sex): return static_cast<int>(pp->m_sex);
        case (PatientRoles::lastUpdate): return pp->m_lastUpdate;
        default:return QVariant();
        }
    }

    QHash<int, QByteArray> roleNames() const
    {
        QHash<int, QByteArray> roles;
           roles[Id] = "Id";
           roles[patientId] = "patientId";
           roles[name] = "name";
           roles[birthDate] = "birthDate";
           roles[sex]="sex";
           roles[lastUpdate]="lastUpdate";
           return roles;
    }

    void setPatientList(Patient_List patient_list)
    {
        m_parent->beginResetModel();
        m_list=patient_list;
        m_parent->endResetModel();
    }
};

PatientListModelVm::PatientListModelVm(const QVariantList &)
{
    m_obj = perimeter_new( PatientListModelVmPriv, this );
//    qDebug()<<"PatientListModelVm gogogogogo";
}

PatientListModelVm::~PatientListModelVm()
{
    if(m_obj!=nullptr)
        perimeter_delete(m_obj,PatientListModelVmPriv);
}

//PatientListModel::PatientListModel(Patient_List patient_list)
//{
//    m_obj = perimeter_new( PatientListModelPriv, this );
//    setPatientList(patient_list);
//}

void PatientListModelVm::setPatientList(Patient_List patient_list)
{
    T_PrivPtr( m_obj )->setPatientList(patient_list);
}

int PatientListModelVm::rowCount(const QModelIndex &) const
{
    return T_PrivPtr( m_obj )->RowCount();
}

QVariant PatientListModelVm::data(const QModelIndex &idx, int role) const
{
    return T_PrivPtr( m_obj )->data(idx,role);
}

QHash<int, QByteArray> PatientListModelVm::roleNames() const
{
    return T_PrivPtr( m_obj )->roleNames();
}

//void PatientListModelVm::addPatient(QString patientId, QString name, int sex, QDate date,QDateTime updateTime)
//{
//    Patient_ptr patient_ptr(new Patient(patientId,name,Patient::sex(sex),date,updateTime));
//    QSqlError daoError = qx::dao::insert(patient_ptr);

//}

//void PatientListModelVm::hello()
//{
// qDebug()<<"patientModelList says hello";
//}

void PatientListModelVm:: deletePatient(long id)
{
    QSqlDatabase db = qx::QxSqlDatabase::getDatabase();
    bool bCommit = db.transaction();
    QSqlError daoError;qx_query query;
    query.query("delete from Patient where id=:id");
    query.bind(":id",(int)id);
    daoError = qx::dao::call_query(query);
    query.query("delete from CheckResult where patient_id=:id");
    query.bind(":id",(int)id);
    daoError = qx::dao::call_query(query);
    bCommit = (bCommit && ! daoError.isValid());
    qAssert(bCommit);
    db.commit();
}

void PatientListModelVm::getPatientListByTimeSpan(QDate from,QDate to)
{
    if(from.toString()=="") from.setDate(1900,1,1);
    if(to.toString()=="") to=QDate::currentDate().addYears(100);
    qx_query query("select * from patient where lastUpdate>=:from and lastUpdate<=:to ORDER BY lastUpdate DESC");
    query.bind(":from",from.toString("yyyy-MM-dd"));
    query.bind(":to",to.toString("yyyy-MM-dd"));
    Patient_List Patient_List;
    QSqlError daoError = qx::dao::execute_query(query, Patient_List);
    setPatientList(Patient_List);
    emit patientListChanged();
}

void PatientListModelVm::getPatientListByPatientId(QString id)
{
    qx_query query("select * from patient where patientId=:patientId");
    query.bind(":patientId",id);
    Patient_List Patient_List;
    QSqlError daoError = qx::dao::execute_query(query, Patient_List);
    setPatientList(Patient_List);
    emit patientListChanged();
}

void PatientListModelVm::getPatientListByName(QString name, QDate from, QDate to)
{
    if(from.toString()=="") from.setDate(1900,1,1);
    if(to.toString()=="") to=QDate::currentDate().addDays(1);
    qx_query query("select * from patient where name=:name and lastUpdate>=:from and lastUpdate<=:to ORDER BY lastUpdate DESC");
    query.bind(":name",name);query.bind(":from",from.toString("yyyy-MM-dd"));query.bind(":to",to.toString("yyyy-MM-dd"));
    Patient_List Patient_List;
    QSqlError daoError = qx::dao::execute_query(query, Patient_List);
    setPatientList(Patient_List);
    emit patientListChanged();
}

void PatientListModelVm::getPatientListBySex(int sex, QDate from, QDate to)
{
    if(from.toString()=="") from.setDate(1900,1,1);
    if(to.toString()=="") to=QDate::currentDate().addDays(1);
    qx_query query("select * from patient where sex=:sex and lastUpdate>=:from and lastUpdate<=:to ORDER BY lastUpdate DESC");
    query.bind(":sex",sex);

    query.bind(":from",from.toString("yyyy-MM-dd"));
    query.bind(":to",to.toString("yyyy-MM-dd"));
    Patient_List Patient_List;
    QSqlError daoError = qx::dao::execute_query(query, Patient_List);
    setPatientList(Patient_List);
    emit patientListChanged();
}

void PatientListModelVm::getPatientListByBirthDate(QDate date)
{
    qx_query query("select * from patient where birthDate=:birthDate ORDER BY lastUpdate DESC");
    query.bind(":birthDate",date.toString("yyyy-MM-dd"));
    Patient_List Patient_List;
    QSqlError daoError = qx::dao::execute_query(query, Patient_List);
    setPatientList(Patient_List);
    emit patientListChanged();
}


//void PatientListModelVm::updatePatient(long id,QString patientId, QString name, int sex, QDate date)
//{
//    Patient_List Patient_List;
//    qx_query query("select * from patient where id=:id");
//    query.bind(":id",QString::number(id));
//    QSqlError daoError = qx::dao::execute_query(query, Patient_List);;
//    Patient_ptr pp = Patient_List.front();
//    pp->m_patientId=patientId;pp->m_name=name;pp->m_sex=Patient::sex(sex);pp->m_birthDate=date;pp->m_lastUpdate=QDateTime::currentDateTime();
//    daoError = qx::dao::update(pp);
//    emit patientListChanged();
//}


//bool PatientListModel::setData(const QModelIndex &, const QVariant &, int)
//{

//}

//Qt::ItemFlags PatientListModel::flags(const QModelIndex &) const
//{

//}
}
