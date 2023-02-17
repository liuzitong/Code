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

    ProgramModel(Program_ptr pp);

    Program_ptr ModelToDB();
};

struct StaticProgramModel:public ProgramModel
{
public:
    StaticParams m_params;
    StaticProgramData m_data;
    StaticProgramModel()=default;

    StaticProgramModel(Program_ptr pp);;

    Program_ptr ModelToDB();
};

struct DynamicProgramModel:public ProgramModel
{
public:
    DynamicParams m_params;
    DynamicProgramData m_data;
    DynamicProgramModel()=default;

    DynamicProgramModel(Program_ptr pp);;

    Program_ptr ModelToDB();
};
#endif // PROGRAM_H

