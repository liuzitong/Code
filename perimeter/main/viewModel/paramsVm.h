#ifndef PARAMSVM_H
#define PARAMSVM_H
#include <QObject>
#include "../model/Params.h"
#include <QArrayData>
#include <QSharedPointer>


class CommonParamsVM:public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<int> Range READ getRange WRITE setRange)
    Q_PROPERTY(int strategy READ getStrategy WRITE setStrategy NOTIFY strategyChanged)
    Q_PROPERTY(int strategyMode READ getStrategyMode WRITE setStrategyMode)
    Q_PROPERTY(int cursorColor READ getCursorColor WRITE setCursorColor)
    Q_PROPERTY(int cursorSize READ getCursorSize WRITE setCursorSize)
    Q_PROPERTY(int backGroundColor READ getBackGroundColor WRITE setBackGroundColor)
    Q_PROPERTY(bool blueYellowTest READ getBlueYellowTest WRITE setBlueYellowTest)
    Q_PROPERTY(bool responseAutoAdapt READ getResponseAutoAdapt WRITE setResponseAutoAdapt)
    Q_PROPERTY(int responseDelayTime READ getResponseDelayTime WRITE setResponseDelayTime)
    Q_PROPERTY(bool centerDotCheck READ getCenterDotCheck WRITE setCenterDotCheck)
    Q_PROPERTY(bool shortTermFluctuation READ getShortTermFluctuation WRITE setShortTermFluctuation)
    Q_PROPERTY(int fixationTarget READ getFixationTarget WRITE setFixationTarget)
    Q_PROPERTY(int fixationMonitor READ getFixationMonitor WRITE setFixationMonitor)
    Q_PROPERTY(bool blindDotTest READ getBlindDotTest WRITE setBlindDotTest)
public:
    CommonParamsVM()=default;
    CommonParamsVM(StaticParams::CommonParams* data){m_data=data;}
    CommonParamsVM& operator=(CommonParamsVM& other)
    {
        *m_data=*other.m_data;
        return *this;
    }
    QList<int> getRange(){return QList<int>{m_data->Range[0],m_data->Range[1]};} void setRange(QList<int> value){m_data->Range[0]=value[0],m_data->Range[1]=value[1];}
    int getStrategy(){return int(m_data->strategy);}void setStrategy(int value){m_data->strategy=StaticParams::CommonParams::Strategy(value);strategyChanged();}Q_SIGNAL void strategyChanged();
    int getStrategyMode(){return int(m_data->strategyMode);}void setStrategyMode(int value){m_data->strategyMode=StaticParams::CommonParams::StrategyMode(value);}
    int getCursorColor(){return int(m_data->cursorColor);}void setCursorColor(int value){m_data->cursorColor=CursorColor(value);}
    int getCursorSize(){return int(m_data->cursorSize);}void setCursorSize(int value){m_data->cursorSize=CursorSize(value);}
    int getBackGroundColor(){return int(m_data->backGroundColor);}void setBackGroundColor(int value){m_data->backGroundColor=BackGroundColor(value);}
    bool getBlueYellowTest(){return m_data->blueYellowTest;}void setBlueYellowTest(bool value){m_data->blueYellowTest=value;}
    bool getResponseAutoAdapt(){return m_data->responseAutoAdapt;}void setResponseAutoAdapt(bool value){m_data->responseAutoAdapt=value;}
    int getResponseDelayTime(){return m_data->responseDelayTime;}void setResponseDelayTime(int value){m_data->responseDelayTime=value;}
    bool getCenterDotCheck(){return m_data->centerDotCheck;}void setCenterDotCheck(bool value){m_data->centerDotCheck=value;}
    bool getShortTermFluctuation(){return m_data->shortTermFluctuation;}void setShortTermFluctuation(bool value){m_data->shortTermFluctuation=value;}
    int getFixationTarget(){return int(m_data->fixationTarget);}void setFixationTarget(int value){m_data->fixationTarget=FixationTarget(value);}
    int getFixationMonitor(){return int(m_data->fixationMonitor);}void setFixationMonitor(bool value){m_data->fixationMonitor=FixationMonitor(value);}
    bool getBlindDotTest(){return m_data->blindDotTest;}void setBlindDotTest(bool value){m_data->blindDotTest=value;}
private:
    StaticParams::CommonParams* m_data;
};

class FixedParamsVM:public QObject
{
    Q_OBJECT
    Q_PROPERTY(int stimulationTime READ getStimulationTime WRITE setStimulationTime)
    Q_PROPERTY(int intervalTime READ getIntervalTime WRITE setIntervalTime)
    Q_PROPERTY(int falsePositiveCycle READ getFalsePositiveCycle WRITE setFalsePositiveCycle)
    Q_PROPERTY(int falseNegativeCycle READ getFalseNegativeCycle WRITE setFalseNegativeCycle)
    Q_PROPERTY(int fixationViewLossCycle READ getFixationViewLossCycle WRITE setFixationViewLossCycle)
    Q_PROPERTY(int singleStimulationDB READ getSingleStimulationDB WRITE setSingleStimulationDB)
    Q_PROPERTY(int blindDotStimulationDB READ getBlindDotStimulationDB WRITE setBlindDotStimulationDB)
    Q_PROPERTY(int shortTermFluctuationCount READ getShortTermFluctuationCount WRITE setShortTermFluctuationCount)
public:
    FixedParamsVM()=default;
    FixedParamsVM(StaticParams::FixedParams* data){m_data=data;}
    FixedParamsVM& operator=(FixedParamsVM& other)
    {
        *m_data=*other.m_data;
        return *this;
    }
    int getStimulationTime(){return m_data->stimulationTime;}void setStimulationTime(int value){m_data->stimulationTime=value;}
    int getIntervalTime(){return m_data->intervalTime;}void setIntervalTime(int value){m_data->intervalTime=value;}
    int getFalsePositiveCycle(){return m_data->falsePositiveCycle;}void setFalsePositiveCycle(int value){m_data->falsePositiveCycle=value;}
    int getFalseNegativeCycle(){return m_data->falseNegativeCycle;}void setFalseNegativeCycle(int value){m_data->falseNegativeCycle=value;}
    int getFixationViewLossCycle(){return m_data->fixationViewLossCycle;}void setFixationViewLossCycle(int value){m_data->fixationViewLossCycle=value;}
    int getSingleStimulationDB(){return m_data->singleStimulationDB;}void setSingleStimulationDB(int value){m_data->singleStimulationDB=value;}
    int getBlindDotStimulationDB(){return m_data->blindDotStimulationDB;}void setBlindDotStimulationDB(int value){m_data->blindDotStimulationDB=value;}
    int getShortTermFluctuationCount(){return m_data->shortTermFluctuationCount;}void setShortTermFluctuationCount(int value){m_data->shortTermFluctuationCount=value;}
private:
    StaticParams::FixedParams* m_data;
};

class StaticParamsVM:public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* commonParams READ getCommonParams )
    Q_PROPERTY(QObject* fixedParams READ getFixedParams )
public:
    StaticParamsVM()=default;
    StaticParamsVM(StaticParams* staticParams)
    {
        m_commonParamsVM.reset(new CommonParamsVM(&staticParams->commonParams));
        m_fixedParamsVM.reset(new FixedParamsVM(&staticParams->fixedParams));
    }
    ~StaticParamsVM()=default;
    StaticParamsVM& operator =(StaticParamsVM& other){
        *m_commonParamsVM=*(other.m_commonParamsVM);
        *m_fixedParamsVM=*(other.m_fixedParamsVM);
        return *this;
    }
    CommonParamsVM* getCommonParams(){return m_commonParamsVM.data();}/*void setCommonParams(QObject* value){m_commonParams=value;}*/
    FixedParamsVM* getFixedParams(){return m_fixedParamsVM.data();};/*void setFixedParams(QObject* value){m_fixedParams=value;};*/
//    ~StaticParamsVM(){delete m_commonParamsVM;delete m_fixedParamsVM;/*delete m_staticParams;*/}
private:
//    StaticParams* m_staticParams;
    QSharedPointer<CommonParamsVM> m_commonParamsVM;
    QSharedPointer<FixedParamsVM> m_fixedParamsVM;
};

class DynamicParamsVM:public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<int> Range READ getRange WRITE setRange)
    Q_PROPERTY(int strategy READ getStrategy WRITE setStrategy)
    Q_PROPERTY(int cursorColor READ getCursorColor WRITE setCursorColor)
    Q_PROPERTY(int cursorSize READ getCursorSize WRITE setCursorSize)
    Q_PROPERTY(int backGroundColor READ getBackGroundColor WRITE setBackGroundColor)
//    Q_PROPERTY(int fixationTarget READ getFixationTarget WRITE setFixationTarget)
    Q_PROPERTY(int speed READ getSpeed WRITE setSpeed)
    Q_PROPERTY(int brightness READ getBrightness WRITE setBrightness)
    Q_PROPERTY(int dynamicMethod READ getDynamicMethod WRITE setDynamicMethod)
    Q_PROPERTY(int dynamicDistance READ getDynamicDistance WRITE setDynamicDistance)
    Q_PROPERTY(int fixationMonitor READ getFixationMonitor WRITE setFixationMonitor)

public:
    DynamicParamsVM()=default;
    DynamicParamsVM(DynamicParams* data){m_data=data;}
    DynamicParamsVM& operator=(DynamicParamsVM& other){*m_data=*other.m_data;return *this;}
    QList<int> getRange(){return QList<int>{m_data->Range[0],m_data->Range[1]};}void setRange(QList<int> value){m_data->Range[0]=value[0];m_data->Range[1]=value[1];}
    int getStrategy(){return int(m_data->strategy);}void setStrategy(int value){m_data->strategy=DynamicParams::Strategy(value);}
    int getCursorColor(){return int(m_data->cursorColor);}void setCursorColor(int value){m_data->cursorColor=CursorColor(value);}
    int getCursorSize(){return int(m_data->cursorSize);}void setCursorSize(int value){m_data->cursorSize=CursorSize(value);}
    int getBackGroundColor(){return int(m_data->backGroundColor);}void setBackGroundColor(int value){m_data->backGroundColor=BackGroundColor(value);}
//    int getFixationTarget(){return int(m_dynamicParams->fixationTarget);}void setFixationTarget(int value){m_dynamicParams->fixationTarget=FixationTarget(value);}
    int getSpeed(){return m_data->speed;}void setSpeed(float value){m_data->speed=value;}
    int getBrightness(){return m_data->brightness;}void setBrightness(int value){m_data->brightness=value;}
    int getDynamicMethod(){return int(m_data->dynamicMethod);}void setDynamicMethod(int value){m_data->dynamicMethod=DynamicParams::DynamicMethod(value);}
    int getDynamicDistance(){return int(m_data->dynamicDistance);}void setDynamicDistance(int value){m_data->dynamicDistance=DynamicParams::DynamicDistance(value);}
    int getFixationMonitor(){return int(m_data->fixationMonitor);}void setFixationMonitor(int value){m_data->fixationMonitor=FixationMonitor(value);}
private:
    DynamicParams* m_data;
};

#endif // PARAMSVM_H
