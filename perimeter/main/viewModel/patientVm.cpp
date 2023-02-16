#include "PatientVm.h"
#include "perimeter/main/database/patient.h"
#include "perimeter/base/common/perimeter_def.h"
#include "perimeter/base/common/perimeter_memcntr.hxx"

#include "perimeter/main/model/utility.h"
#include <QDate>
#define T_PrivPtr( o )  perimeter_objcast( Patient*, o )

namespace Perimeter {
class RxVm:public QObject
{
    Q_OBJECT
    Q_PROPERTY(float rx1_l READ rx1_l WRITE setRx1_l NOTIFY rx1_lChanged)
    Q_PROPERTY(float rx2_l READ rx2_l WRITE setRx2_l NOTIFY rx2_lChanged)
    Q_PROPERTY(float rx3_l READ rx3_l WRITE setRx3_l NOTIFY rx3_lChanged)
    Q_PROPERTY(float visual_l READ visual_l WRITE setVisual_l NOTIFY visual_lChanged)
    Q_PROPERTY(float rx1_r READ rx1_r WRITE setRx1_r NOTIFY rx1_rChanged)
    Q_PROPERTY(float rx2_r READ rx2_r WRITE setRx2_r NOTIFY rx2_rChanged)
    Q_PROPERTY(float rx3_r READ rx3_r WRITE setRx3_r NOTIFY rx3_rChanged)
    Q_PROPERTY(float visual_r READ visual_r WRITE setVisual_r NOTIFY visual_rChanged)

public:
    Q_INVOKABLE explicit RxVm(Rx rx)
    {
        m_rx1_l=rx.rx1_l;
        m_rx2_l=rx.rx2_l;
        m_rx3_l=rx.rx3_l;
        m_visual_l=rx.visual_l;

        m_rx1_r=rx.rx1_r;
        m_rx2_r=rx.rx2_r;
        m_rx3_r=rx.rx3_r;
        m_visual_r=rx.visual_r;
    }

    Q_INVOKABLE virtual ~RxVm() Q_DECL_OVERRIDE=default;
    Rx getData()
    {
        return Rx{m_rx1_l,m_rx2_l,m_rx3_l,m_visual_l,m_rx1_r,m_rx2_r,m_rx3_r,m_visual_r};
    }
    float rx1_l(){return m_rx1_l;}void setRx1_l(float value){m_rx1_l=value;emit rx1_lChanged(value);}Q_SIGNAL void rx1_lChanged(float value);
    float rx2_l(){return m_rx2_l;}void setRx2_l(float value){m_rx2_l=value;emit rx2_lChanged(value);}Q_SIGNAL void rx2_lChanged(float value);
    float rx3_l(){return m_rx3_l;}void setRx3_l(float value){m_rx3_l=value;emit rx3_lChanged(value);}Q_SIGNAL void rx3_lChanged(float value);
    float visual_l(){return m_visual_l;}void setVisual_l(float value){m_visual_l=value;emit visual_lChanged(value);}Q_SIGNAL void visual_lChanged(float value);

    float rx1_r(){return m_rx1_r;}void setRx1_r(float value){m_rx1_r=value;emit rx1_rChanged(value);}Q_SIGNAL void rx1_rChanged(float value);
    float rx2_r(){return m_rx2_r;}void setRx2_r(float value){m_rx2_r=value;emit rx2_rChanged(value);}Q_SIGNAL void rx2_rChanged(float value);
    float rx3_r(){return m_rx3_r;}void setRx3_r(float value){m_rx3_r=value;emit rx3_rChanged(value);}Q_SIGNAL void rx3_rChanged(float value);
    float visual_r(){return m_visual_r;}void setVisual_r(float value){m_visual_r=value;emit visual_rChanged(value);}Q_SIGNAL void visual_rChanged(float value);



private:
    float m_rx1_l,m_rx2_l,m_rx3_l,m_visual_l,m_rx1_r,m_rx2_r,m_rx3_r,m_visual_r;
};


PatientVm::PatientVm(const QVariantList & args)
{
    qDebug()<<"constructor";
    if(args.count()==0)
    {
        return;
    }
    else
    {
        Patient_List Patient_List;
        Patient_ptr patient_ptr;
        int id=args[0].toInt();
        qx_query query("select * from patient where id=:id");
        query.bind(":id",id);
        QSqlError daoError = qx::dao::execute_query(query, Patient_List);
        if(Patient_List.count()==0)
            return;
        else
        {
            patient_ptr=Patient_List.first();
            m_data.reset(new PatientModel(patient_ptr));
            m_rx=new RxVm(Utility::QStringToEntity<Rx>(patient_ptr->m_rx));
        }
    }


}

PatientVm::~PatientVm()
{
    if(m_rx!=nullptr) {delete m_rx;m_rx=nullptr;}
}

void PatientVm::update()
{
    auto patient_ptr=getPatientData();
    qx::dao::update(patient_ptr);
}

void PatientVm::insert()
{
    auto patient_ptr=getPatientData();
    qx::dao::insert(patient_ptr);
}


long PatientVm::getID()
{
    return  m_data->m_id;
}

void PatientVm::setID(long value)
{
    m_data->m_id=value;
}

QString PatientVm::getPatientID()
{
   return  m_data->m_patientId;
}

void PatientVm::setPatientID(QString value)
{
    m_data->m_patientId=value;
}

QString PatientVm::getName()
{
    return  m_data->m_name;
}

void PatientVm::setName(QString value)
{
   m_data->m_name=value;
}

int PatientVm::getSex()
{
    return int(m_data->m_sex);
}

void PatientVm::setSex(int value)
{
    m_data->m_sex=sex(value);
}

QString PatientVm::getBirthDate()
{
    return  m_data->m_birthDate.toString("yyyy-MM-dd");
}

void PatientVm::setBirthDate(QString date)
{
    QList<QString> list=date.split("-");
    int year=list[0].toInt();
    int month=list[1].toInt();
    int day=list[2].toInt();
    m_data->m_birthDate.setDate(year,month,day);
    qDebug()<<year;
    qDebug()<<month;
    qDebug()<<day;
}

QDateTime PatientVm::getLastUpdate()
{
    return m_data->m_lastUpdate;
}

void Perimeter::PatientVm::setLastUpdate(QDateTime value)
{
    m_data->m_lastUpdate=value;
}

QSharedPointer<PatientModel> Perimeter::PatientVm::getModel()
{
    m_data->m_rx=static_cast<RxVm*>(m_rx)->getData();
    return m_data;
}

Patient_ptr PatientVm::getPatientData()
{
    m_data->m_rx=static_cast<RxVm*>(m_rx)->getData();
    return getModel()->ModelToDB();
}

//PatientVm &PatientVm::operator=(const PatientVm &other)
//{
//    this->m_patient=other.m_patient;
//    return *this;
//}

//PatientVm::PatientVm(PatientVm &&other)
//{
//    this->m_patient=other.m_patient;
//    other.m_patient=nullptr;
//}

//PatientVm::PatientVm(const PatientVm &other)
//{
//    this->m_patient=other.m_patient;
//}

//PatientVm::~PatientVm()
//{
//    if(m_patient!=nullptr)
//        perimeter_delete(m_patient,Patient);
//}
}

#include "patientVm.moc"
