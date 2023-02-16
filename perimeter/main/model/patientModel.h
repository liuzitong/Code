#ifndef PATIENTMODEL_H
#define PATIENTMODEL_H
#include <perimeter/main/database/patient.h>
#include <perimeter/main/model/utility.h>
struct Rx
{
    float rx1_l;
    float rx2_l;
    float rx3_l;
    float visual_l;
    float rx1_r;
    float rx2_r;
    float rx3_r;
    float visual_r;

//    Rx& operator =(Rx& other)=default;

    template<class Archive>
    void serialize(Archive& archive, const unsigned int version)
    {
        archive & BOOST_SERIALIZATION_NVP(rx1_r);
        archive & BOOST_SERIALIZATION_NVP(rx2_r);
        archive & BOOST_SERIALIZATION_NVP(rx3_r);
        archive & BOOST_SERIALIZATION_NVP(visual_r);
        archive & BOOST_SERIALIZATION_NVP(rx1_l);
        archive & BOOST_SERIALIZATION_NVP(rx2_l);
        archive & BOOST_SERIALIZATION_NVP(rx3_l);
        archive & BOOST_SERIALIZATION_NVP(visual_l);
    }
};

struct PatientModel
{
public:
    long m_id;
    QString m_patientId;
    QString m_name;
    sex m_sex;
    QDate m_birthDate;
    Rx  m_rx;
    QDateTime m_lastUpdate;
    int m_age;

    PatientModel()=default;
    PatientModel(Patient_ptr patient_ptr)
    {
        m_id=patient_ptr->m_id;
        m_patientId=patient_ptr->m_patientId;
        m_name=patient_ptr->m_name;
        m_sex=patient_ptr->m_sex;
        m_birthDate=patient_ptr->m_birthDate;
        m_rx=Utility::QStringToEntity<Rx>(patient_ptr->m_rx);
        m_lastUpdate=patient_ptr->m_lastUpdate;
        auto currentDate=QDateTime::currentDateTime().date();
        m_age = currentDate.year()- m_birthDate.year();
        if (currentDate.month() < m_birthDate.month() || (currentDate.month() == m_birthDate.month() && currentDate.day() < m_birthDate.day())) { m_age--;}
    }

    Patient_ptr ModelToDB()
    {
        auto patient_ptr=Patient_ptr(new Patient());
        patient_ptr->m_id=m_id;
        patient_ptr->m_patientId=m_patientId;
        patient_ptr->m_name=m_name;
        patient_ptr->m_sex=m_sex;
        patient_ptr->m_birthDate=m_birthDate;
        patient_ptr->m_rx=Utility::entityToQString(m_rx);
        patient_ptr->m_lastUpdate=m_lastUpdate;
        return patient_ptr;

    }
};



#endif // PATIENTMODEL_H


