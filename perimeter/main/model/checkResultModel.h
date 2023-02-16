#ifndef CHECKRESULTMODEL_H
#define CHECKRESULTMODEL_H
#include "Params.h"
#include "Point.h"
#include <QDate>
#include "../database/checkResult.h"
#include "../database/patient.h"
#include "../database/program.h"
#include "programModel.h"
#include "utility.h"

struct ResultData
{
    int alarm;
    float pupilDiameter;
    int testTimespan;                              //Check time
    std::vector<int> fixationDeviation;            //眼动曲线,普通点为偏移为正值,盲点,假阴,假阳等特殊点为负值.
};

struct StaticResultData:public ResultData
{

    int falsePositiveCount;
    int falsePositiveTestCount;
    int falseNegativeCount;
    int falseNegativeTestCount;
    int fixationLostCount;
    int fixationLostTestCount;
    std::vector<std::vector<int>> realTimeDB;   //realTimeDB[点序号][每个点图片序号]
    std::vector<int> checkData;

    template<class Archive>
    void serialize(Archive& archive, const unsigned int version)
    {
        archive & BOOST_SERIALIZATION_NVP(alarm);
        archive & BOOST_SERIALIZATION_NVP(pupilDiameter);
        archive & BOOST_SERIALIZATION_NVP(testTimespan);
        archive & BOOST_SERIALIZATION_NVP(fixationDeviation);
        archive & BOOST_SERIALIZATION_NVP(falsePositiveCount);
        archive & BOOST_SERIALIZATION_NVP(falsePositiveTestCount);
        archive & BOOST_SERIALIZATION_NVP(falseNegativeCount);
        archive & BOOST_SERIALIZATION_NVP(falseNegativeTestCount);
        archive & BOOST_SERIALIZATION_NVP(fixationLostCount);
        archive & BOOST_SERIALIZATION_NVP(fixationLostTestCount);
        archive & BOOST_SERIALIZATION_NVP(realTimeDB);
        archive & BOOST_SERIALIZATION_NVP(checkData);
    }
};

struct DynamicDataNode
{
    std::string name;
    Point start;
    Point end;
    bool isSeen;

    template<class Archive>
    void serialize(Archive& archive, const unsigned int version)
    {
        archive & BOOST_SERIALIZATION_NVP(name);
        archive & BOOST_SERIALIZATION_NVP(start);
        archive & BOOST_SERIALIZATION_NVP(end);
        archive & BOOST_SERIALIZATION_NVP(isSeen);
    }
};


struct DynamicResultData:public ResultData
{
    std::vector<DynamicDataNode> checkData;
    template<class Archive>
    void serialize(Archive& archive, const unsigned int version)
    {
        archive & BOOST_SERIALIZATION_NVP(alarm);
        archive & BOOST_SERIALIZATION_NVP(pupilDiameter);
        archive & BOOST_SERIALIZATION_NVP(testTimespan);
        archive & BOOST_SERIALIZATION_NVP(fixationDeviation);
        archive & BOOST_SERIALIZATION_NVP(checkData);
    }
};

struct CheckResultModel
{
    long m_id;
    Type m_type;
    int m_OS_OD;
    QString m_diagnosis;
    QDateTime m_time;
    int m_patient_id;
    int m_program_id;

    CheckResultModel()=default;

    CheckResultModel(CheckResult_ptr checkResult_ptr)
    {
        m_id=checkResult_ptr->m_id;
        m_type=static_cast<Type>(checkResult_ptr->m_type);
        m_OS_OD=checkResult_ptr->m_OS_OD;
        m_diagnosis=checkResult_ptr->m_diagnosis;
        m_time=checkResult_ptr->m_time;
        m_patient_id=checkResult_ptr->m_patient->m_id;
        m_program_id=checkResult_ptr->m_program->m_id;
    }

    CheckResult_ptr ModelToDB()
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

};

struct StaticCheckResultModel:public CheckResultModel
{
    StaticParams m_params;
    StaticResultData m_data;
    QByteArray m_blob;

    StaticCheckResultModel()=default;
    StaticCheckResultModel(CheckResult_ptr checkResult_ptr):CheckResultModel(checkResult_ptr)
    {
        m_params=Utility::QStringToEntity<StaticParams>(checkResult_ptr->m_params);
        m_data=Utility::QStringToEntity<StaticResultData>(checkResult_ptr->m_data);
        m_blob=checkResult_ptr->m_blob;
    }

    CheckResult_ptr ModelToDB()
    {
        auto pp=CheckResultModel::ModelToDB();
        pp->m_params=Utility::entityToQString(m_params);
        pp->m_data=Utility::entityToQString(m_data);
        pp->m_blob=m_blob;
        return pp;

    }
};

struct DynamicCheckResultModel:public CheckResultModel
{
    DynamicParams m_params;
    DynamicResultData m_data;

    DynamicCheckResultModel()=default;
    DynamicCheckResultModel(CheckResult_ptr checkResult_ptr):CheckResultModel(checkResult_ptr)
    {
        m_params=Utility::QStringToEntity<DynamicParams>(checkResult_ptr->m_params);
        m_data=Utility::QStringToEntity<DynamicResultData>(checkResult_ptr->m_data);
    }

    CheckResult_ptr ModelToDB()
    {
        auto pp=CheckResultModel::ModelToDB();
        pp->m_params=Utility::entityToQString(m_params);
        pp->m_data=Utility::entityToQString(m_data);
        return pp;
    }
};


#endif // CHECKRESULTMODEL_H
