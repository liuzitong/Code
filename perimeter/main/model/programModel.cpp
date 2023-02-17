#include "programModel.h"

ProgramModel::ProgramModel(Program_ptr pp)
{
    m_id=pp->m_id;
    m_type=static_cast<Type>(pp->m_type);
    m_name=pp->m_name;
    m_report=Utility::QStringToEntity<std::vector<int>>(pp->m_report);
    m_category=static_cast<Category>(pp->m_category);
}

Program_ptr ProgramModel::ModelToDB()
{
    auto pp=Program_ptr(new Program());
    pp->m_id=m_id;
    pp->m_type=static_cast<int>(m_type);
    pp->m_name=m_name;
    pp->m_report=Utility::entityToQString(m_report);
    pp->m_category=static_cast<int>(m_category);
    return pp;
}

StaticProgramModel::StaticProgramModel(Program_ptr pp):ProgramModel(pp)
{
    m_params=Utility::QStringToEntity<StaticParams>(pp->m_params);
    m_data=Utility::QStringToEntity<StaticProgramData>(pp->m_data);
}

Program_ptr StaticProgramModel::ModelToDB()
{
    auto pp=ProgramModel::ModelToDB();
    pp->m_params=Utility::entityToQString(m_params);
    pp->m_data=Utility::entityToQString(m_data);
    return pp;
}

DynamicProgramModel::DynamicProgramModel(Program_ptr pp):ProgramModel(pp)
{
    m_params=Utility::QStringToEntity<DynamicParams>(pp->m_params);
    m_data=Utility::QStringToEntity<DynamicProgramData>(pp->m_data);
}

Program_ptr DynamicProgramModel::ModelToDB()
{
    auto pp=ProgramModel::ModelToDB();
    pp->m_params=Utility::entityToQString(m_params);
    pp->m_data=Utility::entityToQString(m_data);
    return pp;
}
