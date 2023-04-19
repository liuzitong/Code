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
    std::vector<int> checkData;         //第一段程序点个数为测出点DB,第二段程序点个数为短波周期DB,第三段为中心点DB

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
    bool isChecked;

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

    CheckResultModel(CheckResult_ptr checkResult_ptr);

    CheckResult_ptr ModelToDB();

};

struct StaticCheckResultModel:public CheckResultModel
{
    StaticParams m_params;
    StaticResultData m_data;
    QSize m_videoSize;
    QByteArray m_blob;
    QVector<QVector<QByteArray>> m_imgData;

    StaticCheckResultModel()=default;
    StaticCheckResultModel(CheckResult_ptr checkResult_ptr);

    CheckResult_ptr ModelToDB();
};

struct DynamicCheckResultModel:public CheckResultModel
{
    DynamicParams m_params;
    DynamicResultData m_data;

    DynamicCheckResultModel()=default;
    DynamicCheckResultModel(CheckResult_ptr checkResult_ptr);

    CheckResult_ptr ModelToDB();
};


#endif // CHECKRESULTMODEL_H
