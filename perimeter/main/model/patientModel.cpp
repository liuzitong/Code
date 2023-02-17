#include "patientModel.h"

PatientModel::PatientModel(Patient_ptr patient_ptr)
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

Patient_ptr PatientModel::ModelToDB()
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
