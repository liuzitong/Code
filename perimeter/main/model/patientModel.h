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
    PatientModel(Patient_ptr patient_ptr);

    Patient_ptr ModelToDB();
};



#endif // PATIENTMODEL_H


