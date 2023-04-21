#ifndef CHECKRESULTVM_H
#define CHECKRESULTVM_H
#include <QObject>
#include <perimeter/main/model/checkResultModel.h>
#include <perimeter/main/viewModel/paramsVm.h>
#include <QDebug>
namespace Perimeter
{
class ResultDataVm:public QObject
{
    Q_OBJECT
    Q_PROPERTY(int alarm READ getAlarm WRITE setAlarm)
    Q_PROPERTY(float pupilDiameter READ getPupilDiameter WRITE setPupilDiameter)
    Q_PROPERTY(int testTimespan READ getTestTimespan WRITE setTestTimespan)
    Q_PROPERTY(QVariantList fixationDeviation READ getFixationDeviation)
public:
    Q_INVOKABLE explicit ResultDataVm(ResultData* data){setData(data);};
    Q_INVOKABLE virtual ~ResultDataVm()=default;
    int getAlarm(){return m_data->alarm;}void setAlarm(int value){m_data->alarm=value;}
    float getPupilDiameter(){return m_data->pupilDiameter;}void setPupilDiameter(float value){m_data->pupilDiameter=value;}
    int getTestTimespan(){return m_data->testTimespan;}void setTestTimespan(int value){m_data->testTimespan=value;}
    QVariantList getFixationDeviation()
    {
        QVariantList list;
//        for(auto& i:m_data->fixationDeviation)
//        {
//            list.append(QVariant(i));
//        }
//        return list;
        for(uint i=0;i<m_data->fixationDeviation.size();i++)
            list.append(QVariant(m_data->fixationDeviation[i]));
        return list;
    }
    void setData(ResultData* data){m_data=data;}
private:
    ResultData* m_data;
};

class StaticResultDataVm:public ResultDataVm
{
    Q_OBJECT
    Q_PROPERTY(int falsePositiveCount READ getFalsePositiveCount WRITE setFalsePositiveCount)
    Q_PROPERTY(int falsePositiveTestCount READ getFalsePositiveTestCount WRITE setFalsePositiveTestCount)
    Q_PROPERTY(int falseNegativeCount READ getFalseNegativeCount WRITE setFalseNegativeCount)
    Q_PROPERTY(int falseNegativeTestCount READ getFalseNegativeTestCount WRITE setFalseNegativeTestCount)
    Q_PROPERTY(int fixationLostCount READ getFixationLostCount WRITE setFixationLostCount)
    Q_PROPERTY(int fixationLostTestCount READ getFixationLostTestCount WRITE setFixationLostTestCount)
    Q_PROPERTY(QVariantList checkData READ getCheckData NOTIFY checkDataChanged)
    Q_PROPERTY(QVariantList realTimeDB READ getRealTimeDB)

public:
    Q_INVOKABLE explicit StaticResultDataVm(StaticResultData* data):ResultDataVm(data){setData(data);}
    Q_INVOKABLE virtual ~StaticResultDataVm()=default;
    int getFalsePositiveCount(){return m_data->falsePositiveCount;}void setFalsePositiveCount(int value){m_data->falsePositiveCount=value;}
    int getFalsePositiveTestCount(){return m_data->falsePositiveTestCount;}void setFalsePositiveTestCount(int value){m_data->falsePositiveTestCount=value;}
    int getFalseNegativeCount(){return m_data->falseNegativeCount;}void setFalseNegativeCount(int value){m_data->falseNegativeCount=value;}
    int getFalseNegativeTestCount(){return m_data->falseNegativeTestCount;}void setFalseNegativeTestCount(int value){m_data->falseNegativeTestCount=value;}
    int getFixationLostCount(){return m_data->fixationLostCount;}void setFixationLostCount(int value){m_data->fixationLostCount=value;}
    int getFixationLostTestCount(){return m_data->fixationLostTestCount;}void setFixationLostTestCount(int value){m_data->fixationLostTestCount=value;}
    QVariantList getCheckData()
    {
        QVariantList list;
        for(auto& i:m_data->checkData)
        {
            list.append(QVariant(i));
        }
        return list;
    }
    Q_SIGNAL void checkDataChanged();
    QVariantList getRealTimeDB()
    {
        QVariantList list;
        for(auto& i:m_data->realTimeDB)
        {
            QVariantList list2;
            {
                for(auto& j:i)
                {
                    list2.append(j);
                }
            }
            list.append(QVariant::fromValue(list2));
        }
        return list;
    }

    void setData(StaticResultData* data){m_data=data;}
private:
    StaticResultData* m_data;
};


class DynamicDataNodeVm:public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ getName)
    Q_PROPERTY(QPointF start READ getStart)
    Q_PROPERTY(QPointF end READ getEnd)
    Q_PROPERTY(bool isSeen READ getIsSeen)
    Q_PROPERTY(bool isChecked READ getIsChecked)
public:
    DynamicDataNodeVm(DynamicDataNode* data){setData(data);}
    ~DynamicDataNodeVm()=default;
    QString getName(){return m_data->name.c_str();}
    QPointF getStart(){return QPointF{m_data->start.x,m_data->start.y};}
    QPointF getEnd(){return QPointF{m_data->end.x,m_data->end.y};}
    bool getIsSeen(){return m_data->isSeen;}
    bool getIsChecked(){return m_data->isChecked;}

    void setData(DynamicDataNode* data){m_data=data;}
private:
    DynamicDataNode* m_data;
};

class DynamicResultDataVm:public ResultDataVm
{
    Q_OBJECT
    Q_PROPERTY(QVariantList checkData READ getCheckData)
public:
    Q_INVOKABLE explicit DynamicResultDataVm(DynamicResultData* data):ResultDataVm(data){setData(data);};
    Q_INVOKABLE virtual ~DynamicResultDataVm(){for(auto&i: m_checkData){delete i.value<DynamicDataNodeVm*>();}}
    QVariantList getCheckData()
    {
        for(auto&i: m_checkData){delete i.value<DynamicDataNodeVm*>();}
        m_checkData.clear();
        if(m_data==nullptr)
        {
            qDebug()<<"nullptr";
        }
        auto& dataList=m_data->checkData;
        qDebug()<<dataList.size();
        for(auto& data:dataList)
        {
            QObject* vm=new DynamicDataNodeVm(&data);
            m_checkData.append(QVariant::fromValue(vm));
        }
        qDebug()<<m_data->checkData.size();
        return m_checkData;
    }

    void setData(DynamicResultData* data)
    {
        m_data=data;
    }
private:
    DynamicResultData* m_data=nullptr;
    QVariantList m_checkData;
};


class CheckResultVm: public QObject
{
    Q_OBJECT
    Q_PROPERTY(long id READ getID WRITE setID)
    Q_PROPERTY(int type READ getType WRITE setType)
    Q_PROPERTY(int OS_OD READ getOS_OD WRITE setOS_OD)
    Q_PROPERTY(QString diagnosis READ getDiagnosis WRITE setDiagnosis NOTIFY diagnosisChanged)
    Q_PROPERTY(QDateTime time READ getTime WRITE setTime)
    Q_PROPERTY(int patient_id READ getPatient_id WRITE setPatient_id)
    Q_PROPERTY(int program_id READ getProgram_id WRITE setProgram_id)

public:
    Q_INVOKABLE explicit CheckResultVm()=default;
    Q_INVOKABLE virtual ~CheckResultVm()=default;
    virtual void insert(){};
    virtual void update(){};
    int getID(){return m_data->m_id;}void setID(int value){m_data->m_id=value;}
    int getType(){return int(m_data->m_type);}void setType(int value){m_data->m_type=Type(value);}
    int getOS_OD(){return m_data->m_OS_OD;}void setOS_OD(int value){m_data->m_OS_OD=value;}
    QString getDiagnosis(){return m_data->m_diagnosis;}void setDiagnosis(QString value){m_data->m_diagnosis=value;emit diagnosisChanged(value);}Q_SIGNAL void diagnosisChanged(QString value);
    QDateTime getTime(){return m_data->m_time;}void setTime(QDateTime value){m_data->m_time=value;}
    int getPatient_id(){return m_data->m_patient_id;}void setPatient_id(int value){m_data->m_patient_id=value;}
    int getProgram_id(){return m_data->m_program_id;}void setProgram_id(int value){m_data->m_program_id=value;}
    void setData(CheckResultModel* data){m_data=data;}
private:
    CheckResultModel* m_data;
};


class StaticCheckResultVm:public CheckResultVm
{
    Q_OBJECT
    Q_PROPERTY(StaticParamsVM* params READ getParams WRITE setParams )
    Q_PROPERTY(StaticResultDataVm* resultData READ getResultData)

public:
    Q_INVOKABLE explicit StaticCheckResultVm(const QVariantList &);
    Q_INVOKABLE explicit StaticCheckResultVm(const CheckResult_ptr &);
    Q_INVOKABLE virtual ~StaticCheckResultVm()=default;
    Q_INVOKABLE void insert() override;
    Q_INVOKABLE void update() override;
    Q_INVOKABLE int drawRealTimeEyePosPic(int index);

    StaticParamsVM* getParams(){return m_params.data();}void setParams(StaticParamsVM* other){*m_params=*other;}
    StaticResultDataVm* getResultData(){return m_resultData.data();}

    QSharedPointer<StaticCheckResultModel> getModel(){return m_data;}
private:
    QSharedPointer<StaticCheckResultModel> m_data;
    QSharedPointer<StaticParamsVM> m_params;
    QSharedPointer<StaticResultDataVm> m_resultData;

};

class DynamicCheckResultVm:public CheckResultVm
{
    Q_OBJECT
    Q_PROPERTY(DynamicParamsVM* params READ getParams WRITE setParams )
    Q_PROPERTY(DynamicResultDataVm* resultData READ getResultData)

public:
    Q_INVOKABLE explicit DynamicCheckResultVm(const QVariantList &);
    Q_INVOKABLE explicit DynamicCheckResultVm(const CheckResult_ptr &);
    Q_INVOKABLE virtual ~DynamicCheckResultVm()=default;
    Q_INVOKABLE void insert() override;
    Q_INVOKABLE void update() override;

    DynamicParamsVM* getParams(){return m_params.data();}void setParams(DynamicParamsVM* other){*m_params=*other;}
    DynamicResultDataVm* getResultData(){return m_resultData.data();}

    QSharedPointer<DynamicCheckResultModel> getModel(){return m_data;}
private:
    QSharedPointer<DynamicCheckResultModel> m_data;
    QSharedPointer<DynamicParamsVM> m_params;
    QSharedPointer<DynamicResultDataVm> m_resultData;
};
}


Q_DECLARE_OPAQUE_POINTER(Perimeter::DynamicDataNodeVm);
#endif // CHECKRESULTVM_H
