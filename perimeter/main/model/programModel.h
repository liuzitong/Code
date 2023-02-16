#ifndef PROGRAM_MODEL
#define PROGRAM_MODEL
#include "Params.h"
#include "Point.h"
#include "../database/program.h"
#include "utility.h"


struct StaticProgramData
{
    std::vector<typename StaticParams::CommonParams::Strategy> strategies;
    std::vector<typename StaticParams::CommonParams::StrategyMode> strategyModes;
    std::vector<Point> baseDots;
    std::vector<Point> dots;
    template<class Archive>
    void serialize(Archive& archive, const unsigned int version)
    {
        archive & BOOST_SERIALIZATION_NVP(strategies);
        archive & BOOST_SERIALIZATION_NVP(strategyModes);
        archive & BOOST_SERIALIZATION_NVP(baseDots);
        archive & BOOST_SERIALIZATION_NVP(dots);
    }
};

struct DynamicProgramData
{
    std::vector<Point> dots;  //Only For Standard,other don't have this
    template<class Archive>
    void serialize(Archive& archive, const unsigned int version)
    {
        archive & BOOST_SERIALIZATION_NVP(dots);
    }
};


struct ProgramModel
{
public:
    long m_id;
    Type m_type;
    QString m_name;
    std::vector<int> m_report;
    Category m_category;

    ProgramModel()=default;

    ProgramModel(Program_ptr pp)
    {
        m_id=pp->m_id;
        m_type=static_cast<Type>(pp->m_type);
        m_name=pp->m_name;
        m_report=Utility::QStringToEntity<std::vector<int>>(pp->m_report);
        m_category=static_cast<Category>(pp->m_category);
    }

    Program_ptr ModelToDB()
    {
        auto pp=Program_ptr(new Program());
        pp->m_id=m_id;
        pp->m_type=static_cast<int>(m_type);
        pp->m_name=m_name;
        pp->m_report=Utility::entityToQString(m_report);
        pp->m_category=static_cast<int>(m_category);
        return pp;
    }
};

struct StaticProgramModel:public ProgramModel
{
public:
    StaticParams m_params;
    StaticProgramData m_data;
    StaticProgramModel()=default;

    StaticProgramModel(Program_ptr pp):ProgramModel(pp)
    {
        m_params=Utility::QStringToEntity<StaticParams>(pp->m_params);
        m_data=Utility::QStringToEntity<StaticProgramData>(pp->m_data);
    };

    Program_ptr ModelToDB()
    {
        auto pp=ProgramModel::ModelToDB();
        pp->m_params=Utility::entityToQString(m_params);
        pp->m_data=Utility::entityToQString(m_data);
        return pp;
    }
};

struct DynamicProgramModel:public ProgramModel
{
public:
    DynamicParams m_params;
    DynamicProgramData m_data;
    DynamicProgramModel()=default;

    DynamicProgramModel(Program_ptr pp):ProgramModel(pp)
    {
        m_params=Utility::QStringToEntity<DynamicParams>(pp->m_params);
        m_data=Utility::QStringToEntity<DynamicProgramData>(pp->m_data);
    };

    Program_ptr ModelToDB()
    {
        auto pp=ProgramModel::ModelToDB();
        pp->m_params=Utility::entityToQString(m_params);
        pp->m_data=Utility::entityToQString(m_data);
        return pp;
    }
};
#endif // PROGRAM_H

