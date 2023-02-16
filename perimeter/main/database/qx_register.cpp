#include "program.h"
#include "checkResult.h"
#include "patient.h"

QX_REGISTER_CPP_QX_API(Program)

template <> void qx::register_class(QxClass<Program> &t){

    t.id(&Program::m_id, "program_id");
    t.data(&Program::m_type, "type");
    t.data(&Program::m_name, "name");
    t.data(&Program::m_params,"params");
    t.data(&Program::m_data,"data");
    t.data(&Program::m_report,"report");
    t.data(&Program::m_category, "category");

    t.relationOneToMany(&Program::m_checkResultX,"CheckResult_List","program_id");
}

QX_REGISTER_CPP_QX_API(Patient)
template <> void qx::register_class(QxClass<Patient> &t){
    t.id(&Patient::m_id, "id");
    t.data(&Patient::m_patientId,"patientId");
    t.data(&Patient::m_name, "name");
    t.data(&Patient::m_birthDate, "birthDate");
    t.data(&Patient::m_sex,"sex");
    t.data(&Patient::m_rx,"rx");
    t.data(&Patient::m_lastUpdate, "lastUpdate");
    t.relationOneToMany(&Patient::m_checkResultX,"CheckResult_List","id");
}

QX_REGISTER_CPP_QX_API(CheckResult)
template <> void qx::register_class(QxClass<CheckResult> &t){
    t.id(&CheckResult::m_id,"checkResult_id");
    t.data(&CheckResult::m_type, "type");
    t.data(&CheckResult::m_OS_OD, "OS_OD");
    t.data(&CheckResult::m_params, "params");
    t.data(&CheckResult::m_data,"data");
    t.data(&CheckResult::m_blob,"blob");
    t.data(&CheckResult::m_diagnosis,"diagnosis");
    t.data(&CheckResult::m_time,"time");
    t.relationManyToOne(&CheckResult::m_patient,"patient_id");
    t.relationManyToOne(&CheckResult::m_program,"program_id");
}
