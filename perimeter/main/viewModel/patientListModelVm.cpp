#include "patientListModelVm.h"
#include "perimeter/base/common/perimeter_memcntr.hxx"
#include <QList>
#include <vector>

namespace Perimeter{

enum PatientRoles
{
    Id=Qt::UserRole + 1,
    patientId ,
    name,
    birthDate,
    sex,
    lastUpdate,
    isSelected
};


}
namespace Perimeter{
#define T_PrivPtr( o )  perimeter_objcast( PatientListModelVmPriv*, o )
class PatientListModelVmPriv
{
public:
    PatientListModelVm* m_parent;
    Patient_List m_list;
    QVector<bool> m_isSelected;

    PatientListModelVmPriv(PatientListModelVm* pa)
    {
        m_parent=pa;
    }
    inline int RowCount(){return m_list.count();}
    QVariant data(const QModelIndex &idx, int role) const
    {
        int index=idx.row();
        Patient_ptr pp=m_list[index];
        bool isSelected=m_isSelected[index];
        switch (role)
        {
        case (PatientRoles::Id):return (int)pp->m_id;
        case (PatientRoles::patientId): return pp->m_patientId;
        case (PatientRoles::name): return pp->m_name;
        case (PatientRoles::birthDate): return pp->m_birthDate.toString("yyyy/MM/dd");
        case (PatientRoles::sex): return static_cast<int>(pp->m_sex);
        case (PatientRoles::lastUpdate): return pp->m_lastUpdate;
        case (PatientRoles::isSelected): return isSelected;
        default:return QVariant();
        }
    }

    bool setData(const QModelIndex &idx, const QVariant &value, int role)
    {
        int index=idx.row();
        switch (role)
        {
        case (PatientRoles::isSelected):
        {
            m_isSelected[index]=value.toBool();
            qDebug()<<"set Index";
            return true;
        }
        }
        return false;
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
           roles[isSelected]="isSelected";
           return roles;
    }

    void setPatientList(Patient_List patient_list)
    {
        m_parent->beginResetModel();
        m_list=patient_list;
        m_isSelected=QVector<bool>(m_list.size(),false);
        m_parent->endResetModel();
    }

    int getSelectedPatientCount()
    {
        int count=0;
        for(auto&i:m_isSelected)
        {
            if(i==true)
            {
                count++;
            }
        }
        return count;
    }

};

PatientListModelVm::PatientListModelVm(const QVariantList &)
{
    m_obj = perimeter_new( PatientListModelVmPriv, this );
}

PatientListModelVm::~PatientListModelVm()
{
    if(m_obj!=nullptr)
        perimeter_delete(m_obj,PatientListModelVmPriv);
}



void PatientListModelVm::setPatientList(Patient_List patient_list)
{
    T_PrivPtr( m_obj )->setPatientList(patient_list);
}

bool PatientListModelVm::setData(const QModelIndex &idx, const QVariant &value, int role)
{
    T_PrivPtr( m_obj )->setData(idx,value,role);
    emit selectedCountChanged();
    return true;
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

int PatientListModelVm::getSelectedCount()
{
    return T_PrivPtr( m_obj )->getSelectedPatientCount();
}

void PatientListModelVm::deletePatients()
{
    auto& isSelected=T_PrivPtr( m_obj )->m_isSelected;
    auto patientList=T_PrivPtr( m_obj )->m_list;
    for(int i=0;i<isSelected.length();i++)
    {
        if(isSelected[i])
        {
            deletePatient(patientList[i]->m_id);
        }
    }
    isSelected.clear();
    emit selectedCountChanged();
}



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
    if(db.commit())
    {
        //删掉眼位图
        QString fileDir=R"(./savePics/)"+
                QString::number(id/100)+"/"+
                QString::number(id);
        QDir qdir(fileDir);
        qdir.removeRecursively();
        fileDir=R"(./savePics/)"+
            QString::number(id/100);
        QDir qdir2(fileDir);
//        auto list=qdir2.entryList(QDir::NoDotAndDotDot);
//        qDebug()<<list;
        if(qdir2.entryList(QDir::NoDotAndDotDot).length()==0)
        {
            qdir2.removeRecursively();
        }
    }
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
