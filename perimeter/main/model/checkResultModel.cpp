#include "checkResultModel.h"

CheckResultModel::CheckResultModel(CheckResult_ptr checkResult_ptr)
{
    m_id=checkResult_ptr->m_id;
    m_type=static_cast<Type>(checkResult_ptr->m_type);
    m_OS_OD=checkResult_ptr->m_OS_OD;
    m_diagnosis=checkResult_ptr->m_diagnosis;
    m_time=checkResult_ptr->m_time;
    m_patient_id=checkResult_ptr->m_patient->m_id;
    m_program_id=checkResult_ptr->m_program->m_id;
}

CheckResult_ptr CheckResultModel::ModelToDB()
{
    auto checkResult_ptr=CheckResult_ptr(new CheckResult());
    checkResult_ptr->m_patient=Patient_ptr(new Patient());
    checkResult_ptr->m_program=Program_ptr(new Program());
    checkResult_ptr->m_OS_OD=m_OS_OD;
    checkResult_ptr->m_id=m_id;
    checkResult_ptr->m_type=static_cast<int>(m_type);
    checkResult_ptr->m_time=m_time;
    checkResult_ptr->m_diagnosis=m_diagnosis;
    checkResult_ptr->m_patient->m_id=m_patient_id;
    checkResult_ptr->m_program->m_id=m_program_id;
    return checkResult_ptr;
}

StaticCheckResultModel::StaticCheckResultModel(CheckResult_ptr checkResult_ptr):CheckResultModel(checkResult_ptr)
{
    m_params=Utility::QStringToEntity<StaticParams>(checkResult_ptr->m_params);
    m_data=Utility::QStringToEntity<StaticResultData>(checkResult_ptr->m_data);
    m_blob=checkResult_ptr->m_blob;
}

CheckResult_ptr StaticCheckResultModel::ModelToDB()
{
    auto pp=CheckResultModel::ModelToDB();
    pp->m_params=Utility::entityToQString(m_params);
    pp->m_data=Utility::entityToQString(m_data);
    pp->m_blob=m_blob;
    return pp;

}

DynamicCheckResultModel::DynamicCheckResultModel(CheckResult_ptr checkResult_ptr):CheckResultModel(checkResult_ptr)
{
    m_params=Utility::QStringToEntity<DynamicParams>(checkResult_ptr->m_params);
    m_data=Utility::QStringToEntity<DynamicResultData>(checkResult_ptr->m_data);
}

CheckResult_ptr DynamicCheckResultModel::ModelToDB()
{
    auto pp=CheckResultModel::ModelToDB();
    pp->m_params=Utility::entityToQString(m_params);
    pp->m_data=Utility::entityToQString(m_data);
    return pp;
}
