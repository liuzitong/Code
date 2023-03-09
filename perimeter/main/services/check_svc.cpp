#include "check_svc.h"
#include <QApplication>
#include <QThread>
#include <QTimer>
#include <QElapsedTimer>
#include <perimeter/main/services/utility_svc.h>
#include <QtMath>
#include <deviceOperation/device_operation.h>
#include <tuple>
#include "frame_provid_svc.h"
namespace Perimeter{
constexpr int MaxDB=52;
constexpr int MinDB=0;
class Check
{
public:
    Check()=default;
    ~Check()=default;
    QSharedPointer<PatientModel> m_patientModel;
    int m_checkedCount,m_totalCount;   //只计算通常点数,短周期,中心点,以及周期测试不计入内.m_checkedCount=m_totalCount就测试完成.在doWork里面结束测试循环.
    QSharedPointer<UtilitySvc> m_utilitySvc=UtilitySvc::getSingleton();
    QSharedPointer<DevOps::DeviceOperation> m_deviceOperation=DevOps::DeviceOperation::getSingleton();
    static constexpr int y_offsetDiamond=-8,y_offsetBottomPoint=-12;
    virtual void Checkprocess()=0;
    virtual void initialize()=0;
};

class StaticCheck:public Check
{
private:
    enum class LastCheckedDotType
    {
        commonCheckDot,
        locateBlindDot,
        blindDotTest,
        falsePositiveTest,
        falseNegativeTest
    };

    struct DotRecord
    {
        int index;
        QPointF loc;
        QVector<int> StimulationDBs;                     //每次亮灯的DB
        int DB;                                          //最后得到的DB;
        QVector<QByteArray> realTimeEyePosPic;           //实时眼位
        bool isBaseDot;                                  //fast smart interactive模式的最初4点
        bool checked;
        int lowerBound;
        int upperBound;
    };
    QVector<int> m_value_30d;
    QVector<int> m_value_60d;
    int m_y_offset;
    QVector<DotRecord> m_dotRecords;    //index为位置index
    bool m_isStartWithBaseDots;         //基础4点开始检查.
    bool m_isDoingBaseDotsCheck;        //处于给基础4点测试定值阶段
    DotRecord m_centerDotRecord;        //index 为2*程序总数+1
    QVector<int> m_answeredTimes;    //根据此处得到autoAdaptTime;
    int m_autoAdaptTime;
    QVector<int> m_lastShortTermCycleCheckedDotIndex;
    QVector<DotRecord> m_shortTermFlucRecords;          //index为程序点总数+位置index,里面存储了所有的短周期点
    int m_falsePosCycCount=qrand()%10,m_falseNegCycCount=qrand()%10,m_fiaxationViewLossCyc=qrand()%10;  //错开
    QVector<QPointF> m_blindDot;
    int m_blindDotLocateIndex=-1;
    int m_stimulationCount=0;                   //刺激次数到了测试盲点位置
    QVector<DotRecord*> m_lastCheckDotRecord;
    QVector<LastCheckedDotType> m_lastCheckeDotType;


public:
    QSharedPointer<StaticCheckResultModel> m_resultModel;
    QSharedPointer<StaticProgramModel> m_programModel;
    StaticCheck()=default;
//    ~StaticCheck()=default;
    ~StaticCheck(){
        qDebug()<<"check deleted";
    }
    virtual void initialize() override;
    virtual void Checkprocess() override;
    std::tuple<bool,QPointF,int> getCheckCycleLocAndDB();
private:

    DotRecord& getCheckDotRecordRef();

    void stimulate();

    void getReadyToStimulate(QPointF loc, int DB);

    void getProperValByRefToNearDotDB(DotRecord& dotRecord);

    bool waitForAnswer();

    void ProcessAnswer(bool answered);

    void waitAndProcessAnswer();
};

void StaticCheck::initialize()
{
    m_checkedCount=0;
    m_autoAdaptTime=0;
    m_resultModel->m_patient_id=m_patientModel->m_id;
    m_resultModel->m_program_id=m_programModel->m_id;
    m_totalCount=m_programModel->m_data.dots.size();
    m_resultModel->m_data.checkData=std::vector<int>(m_totalCount*2+1,-1);  //第一段程序点个数为测出点DB,第二段程序点个数为短波周期DB,第三段为中心点DB
    m_resultModel->m_data.realTimeDB.reserve(m_totalCount*2+1);
    auto cursorSize=m_programModel->m_params.commonParams.cursorSize;
    auto cursorColor=m_programModel->m_params.commonParams.cursorColor;
    m_value_30d=m_utilitySvc->getValue30d(int(cursorSize),int(cursorColor),m_patientModel->m_age);
    m_value_60d=m_utilitySvc->m_value_60d;
    switch (m_resultModel->m_params.commonParams.fixationTarget)
    {
    case FixationTarget::centerPoint:m_y_offset=0;break;
    case FixationTarget::bigDiamond:
    case FixationTarget::smallDiamond:m_y_offset=-y_offsetDiamond;break;
    case FixationTarget::bottomPoint:m_y_offset=-y_offsetBottomPoint;break;
    }

    if(m_resultModel->m_params.commonParams.fixationTarget==FixationTarget::centerPoint&&m_resultModel->m_params.commonParams.centerDotCheck==true)
    {
        m_centerDotRecord.loc={0,float(y_offsetDiamond)};
        m_centerDotRecord.index=m_totalCount*2+1;
    }

    int DBChanged=0;
    if(m_programModel->m_params.commonParams.strategy==StaticParams::CommonParams::Strategy::oneStage
            ||m_programModel->m_params.commonParams.strategy==StaticParams::CommonParams::Strategy::twoStages
            ||m_programModel->m_params.commonParams.strategy==StaticParams::CommonParams::Strategy::quantifyDefects)
    {
        DBChanged=-4;
    }

    if(m_programModel->m_params.commonParams.strategy==StaticParams::CommonParams::Strategy::smartInteractive
            ||m_programModel->m_params.commonParams.strategy==StaticParams::CommonParams::Strategy::fastInterative)
    {
        m_isStartWithBaseDots=true;
        m_isDoingBaseDotsCheck=true;
    }
    else
    {
        m_isStartWithBaseDots=false;
    }

    if(m_programModel->m_params.commonParams.strategy!=StaticParams::CommonParams::Strategy::singleStimulation)
    {
        for(int i=0;i<m_totalCount;i++)
        {
            bool isBaseDot=false;
            auto dot=m_programModel->m_data.dots[i];
            if(m_isStartWithBaseDots)
            {
                auto baseDots=m_programModel->m_data.baseDots;
                for(uint j=0;j<baseDots.size();j++)
                    if((qAbs(dot.x-baseDots[j].x)<FLT_EPSILON)&&(qAbs(dot.y-baseDots[j].y)<FLT_EPSILON)) isBaseDot=true;
            }
            m_dotRecords.push_back(DotRecord{i,QPointF{dot.x,dot.y},{m_utilitySvc->getExpectedDB(m_value_30d,{dot.x,dot.y},m_resultModel->m_OS_OD)+DBChanged},-1, {},isBaseDot,false,MinDB,MaxDB});
        }
        m_centerDotRecord=DotRecord{0,QPointF{0,0},{m_utilitySvc->getExpectedDB(m_value_30d,{0,0},m_resultModel->m_OS_OD)+DBChanged},-1,{},false,false,MinDB,MaxDB};
    }
    else                            //单刺激
    {
        auto DB=m_programModel->m_params.fixedParams.singleStimulationDB;
        for(int i=0;i<m_totalCount;i++)
        {
            auto dot=m_programModel->m_data.dots[i];
            m_dotRecords.push_back(DotRecord{i,QPointF{dot.x,dot.y},{DB},-1, {},false,false,MinDB,MaxDB});
        }
        m_centerDotRecord=DotRecord{0,QPointF{0,0},{DB},-1,{},false,false,MinDB,MaxDB};
    }
    m_deviceOperation->setCursorColorAndCursorSize(int(cursorColor),int(cursorSize));
//    m_deviceOperation->getReadyToStimulate({3,3},int(cursorSize),0);
//    m_deviceOperation->openShutter(20000);
}

void StaticCheck::Checkprocess()            //第一次先跑点,直接刺激,第二次跑点,等待上次刺激的应答,然后再刺激,再跑点,再等待上次的应答.(保证再等待应答的同时跑点.)
{
    auto checkCycleLocAndDB=getCheckCycleLocAndDB();                //存储LastdotType为各种检查
    if(std::get<0>(checkCycleLocAndDB))
    {
        qDebug()<<"checkCycleLocAndDB loc:"<<std::get<1>(checkCycleLocAndDB)<<" DB"<<std::get<2>(checkCycleLocAndDB);
        m_lastCheckDotRecord.push_back(nullptr);
        getReadyToStimulate(std::get<1>(checkCycleLocAndDB),std::get<2>(checkCycleLocAndDB));
    }
    else
    {
        m_lastCheckDotRecord.push_back(&getCheckDotRecordRef());   //存储lastDotType为commondot 并且存储指针
        auto dotRec=m_lastCheckDotRecord.last();
        qDebug()<<"getCheckDotRecordRef loc:"<<dotRec->loc<<" DB"<<dotRec->StimulationDBs.last();
        getReadyToStimulate(m_lastCheckDotRecord.last()->loc,m_lastCheckDotRecord.last()->StimulationDBs.last());
    }
    qDebug()<<"last Checked Dot length:"+QString::number(m_lastCheckeDotType.size())+".first:"+int(m_lastCheckeDotType.first())+".last:"+int(m_lastCheckeDotType.last());
    if(m_stimulationCount!=0)                               //最开始没刺激过就不需要处理
    {
        waitAndProcessAnswer();                             //取出commondot 并且取出指针
    }
    if(m_checkedCount<m_totalCount-1)                       //检查完毕
    {
        stimulate();
    }
}


StaticCheck::DotRecord &StaticCheck::getCheckDotRecordRef()
{
    m_lastCheckeDotType.push_back(LastCheckedDotType::commonCheckDot);

    if(m_resultModel->m_params.commonParams.centerDotCheck==true&&m_centerDotRecord.checked==false)
    {
        return m_centerDotRecord;
    }

    if(!m_shortTermFlucRecords.isEmpty())
    {
        if(m_shortTermFlucRecords.size()>=2)
        {
            if(m_shortTermFlucRecords[m_shortTermFlucRecords.size()-2].checked==false)
            {
                return m_shortTermFlucRecords[m_shortTermFlucRecords.size()-2];             //之前一个周期没测出短波要直接测完
            }
        }
        else
        {
            if(qrand()%(1+m_resultModel->m_params.fixedParams.fixationViewLossCycle)==0)    //周期内随机到了短波周期
            {

                return m_shortTermFlucRecords.last();
            }
        }
    }
    if(m_isStartWithBaseDots)
    {
        qsrand(QTime::currentTime().msec());
        QVector<DotRecord> zoneRightTop,zoneRightBottom,zoneLeftBottom,zoneLeftTop;
        QPointF zoneRightTopCenter,zoneRightBottomCenter,zoneLeftBottomCenter,zoneLeftTopCenter;
        QVector<int> zone;
        for(auto &i:m_dotRecords)
        {
            if(i.checked) continue;
            auto loc=i.loc;
            if(loc.x()>0&&loc.y()>0)
            {
                zoneRightTop.append(i);
                if(i.isBaseDot) zoneRightTopCenter=loc;
            }
            if(loc.x()>0&&loc.y()<0)
            {
                zoneRightBottom.append(i);
                if(i.isBaseDot) zoneRightBottomCenter=loc;
            }
            if(loc.x()<0&&loc.y()<0)
            {
                zoneLeftBottom.append(i);
                if(i.isBaseDot) zoneLeftBottomCenter=loc;
            }
            if(loc.x()<0&&loc.y()>0)
            {
                zoneLeftTop.append(i);
                if(i.isBaseDot) zoneLeftTopCenter=loc;
            }
        }

        if(zoneRightTop.count()>0) zone.push_back(0);
        if(zoneRightBottom.count()>0) zone.push_back(1);
        if(zoneLeftBottom.count()>0) zone.push_back(2);
        if(zoneLeftTop.count()>0) zone.push_back(3);

        auto zoneNumber=zone[qrand()%zone.size()];
        QVector<DotRecord> seletedZoneRecords;
        QPointF centerCoord;
        switch (zoneNumber)
        {
        case 0:seletedZoneRecords=zoneRightTop;centerCoord=zoneRightTopCenter;break;
        case 1:seletedZoneRecords=zoneRightBottom;centerCoord=zoneRightBottomCenter;break;
        case 2:seletedZoneRecords=zoneLeftBottom;centerCoord=zoneLeftBottomCenter;break;
        case 3:seletedZoneRecords=zoneLeftTop;centerCoord=zoneLeftTopCenter;break;
        }

        float nearestDist=FLT_MAX;
        QVector<DotRecord> nearestRecords;
        for(auto&i:seletedZoneRecords)
        {
            auto dist=sqrt(pow((i.loc.x()-centerCoord.x()),2)+pow((i.loc.y()-centerCoord.y()),2));
            if(qAbs(dist-nearestDist)<=FLT_EPSILON)
            {
                nearestRecords.append(i);
            }
            else if(dist<nearestDist)
            {
                nearestRecords.clear();
                nearestRecords.append(i);
                nearestDist=dist;
            }
        }
        auto& selectedDot=nearestRecords[qrand()%nearestRecords.count()];
        return selectedDot;
    }
    else
    {
        QVector<int> unchekedDotIndex;
        for(auto&i:m_dotRecords)
        {
            if(!i.checked) unchekedDotIndex.push_back(i.index);                                 //检查的编号加入容器
        }
        return m_dotRecords[unchekedDotIndex[qrand()%unchekedDotIndex.count()]];                //随机出一个
    }
}

void StaticCheck::stimulate()
{
    m_stimulationCount++;
    int durationTime=m_programModel->m_params.fixedParams.stimulationTime;
    if(m_lastCheckeDotType.last()!=LastCheckedDotType::falseNegativeTest)               //假阴不开快门
        m_deviceOperation->openShutter(durationTime);
    //实时眼位
    QByteArray ba=m_deviceOperation->getRealTimeStimulationEyeImage();
    if(!ba.isEmpty()){m_resultModel->m_blob.append(ba);}
}

void StaticCheck::getReadyToStimulate(QPointF loc, int DB)
{
    m_deviceOperation->getReadyToStimulate(loc,int(m_resultModel->m_params.commonParams.cursorSize),DB);
}

void StaticCheck::getProperValByRefToNearDotDB(StaticCheck::DotRecord &dotRecord)
{
    QVector<DotRecord> largerRadiusDotRecords;
    QVector<DotRecord> smallerRadiusDotRecords;
    auto selectedDotRadius=sqrt(pow(dotRecord.loc.x(),2)+pow(dotRecord.loc.y(),2));
    for(auto&i:m_dotRecords)
    {
        if(i.checked)
        {
            auto radius=sqrt(pow(i.loc.x(),2)+pow(i.loc.y(),2));
            if(qAbs(selectedDotRadius-radius)>FLT_EPSILON)
            {
                if(selectedDotRadius>radius)
                {
                    largerRadiusDotRecords.append(i);
                }
                else
                {
                    smallerRadiusDotRecords.append(i);
                }
            }
        }
    }

    QVector<DotRecord> lowerDBRefDotRecord;
    QVector<DotRecord> higherDBRefDotRecord;
    auto minDist=FLT_MAX;

    for(auto&i:largerRadiusDotRecords)
    {
        auto dist=sqrt(pow(dotRecord.loc.x()-i.loc.x(),2)+pow(dotRecord.loc.y()-i.loc.y(),2));
        if(dist<minDist)
        {
            minDist=dist;
            lowerDBRefDotRecord.append(i);
        }
    }

    for(auto&i:smallerRadiusDotRecords)
    {
        auto dist=sqrt(pow(dotRecord.loc.x()-i.loc.x(),2)+pow(dotRecord.loc.y()-i.loc.y(),2));
        if(dist<minDist)
        {
            minDist=dist;
            higherDBRefDotRecord.append(i);
        }
    }
    int lowerDB,higherDB;
    if(!lowerDBRefDotRecord.isEmpty()) lowerDB=lowerDBRefDotRecord[0].DB;
    if(!higherDBRefDotRecord.isEmpty()) higherDB=higherDBRefDotRecord[0].DB;
    auto averageDB=float(lowerDB+higherDB)/2;
    auto dist1=qAbs(dotRecord.lowerBound-averageDB);
    auto dist2=qAbs(dotRecord.lowerBound+1-averageDB);
    dist1<dist2?dotRecord.DB=dotRecord.lowerBound:dotRecord.DB=dotRecord.lowerBound+1;
}

std::tuple<bool, QPointF, int> StaticCheck::getCheckCycleLocAndDB()
{
    //测试盲点
    auto commomParams=m_resultModel->m_params.commonParams;
    auto fixedParams=m_resultModel->m_params.fixedParams;
    if(commomParams.blindDotTest==true)                 //盲点测试
    {
        //确定盲点,条件是要经历一点测试次数,而且盲不为空
        if(m_stimulationCount>UtilitySvc::getSingleton()->m_checkCountBeforeGetBlindDotCheck&&!m_blindDot.isEmpty())
        {
            m_lastCheckeDotType.push_back(LastCheckedDotType::locateBlindDot);
            QPoint blindDotLoc;
            qDebug()<<m_blindDotLocateIndex;
            qDebug()<<UtilitySvc::getSingleton()->m_left_blindDot;
            qDebug()<<UtilitySvc::getSingleton()->m_right_blindDot;
            if(m_resultModel->m_OS_OD)
                blindDotLoc=UtilitySvc::getSingleton()->m_left_blindDot[m_blindDotLocateIndex];
            else
                blindDotLoc=UtilitySvc::getSingleton()->m_right_blindDot[m_blindDotLocateIndex];

            return {true,blindDotLoc,m_stimulationCount>UtilitySvc::getSingleton()->m_blindDotTestDB};
        }

        //盲点不为空的时候到了周期测试盲点.
        if(!m_blindDot.isEmpty()&&m_stimulationCount%fixedParams.fixationViewLossCycle==0)
        {
            auto blindDB=UtilitySvc::getSingleton()->m_blindDotTestDB;
            m_lastCheckeDotType.push_back(LastCheckedDotType::blindDotTest);
            return {true,{m_blindDot[0].x(),m_blindDot[0].y()},blindDB};
        }
    }


    if(m_stimulationCount%fixedParams.falsePositiveCycle==0&&m_stimulationCount!=0)         //假阳
    {
        QVector<DotRecord> m_checkedRecords;
        for(auto& recordDot:m_dotRecords)
        {
            if(recordDot.checked)
            {
                m_checkedRecords.push_back(recordDot);
            }
        }
        if(m_checkedRecords.size()==0) return {false,{0,0},0};
        auto recordDot=m_checkedRecords[qrand()%m_checkedRecords.size()];
        m_lastCheckeDotType.push_back(LastCheckedDotType::falsePositiveTest);
        return {true,recordDot.loc,recordDot.DB-UtilitySvc::getSingleton()->m_falsePositiveDecDB};
    }


    if(m_stimulationCount%fixedParams.falseNegativeCycle==0&&m_stimulationCount!=0)         //假阴,随机点,到刺激的时候不开快门
    {
        auto locs=m_programModel->m_data.dots;
        auto loc=locs[qrand()%locs.size()];
        m_lastCheckeDotType.push_back(LastCheckedDotType::falseNegativeTest);
        return {true,{loc.x,loc.y},0};
    }
    return {false,{0,0},0};
}

bool StaticCheck::waitForAnswer()
{
    qDebug()<<"waitForAnswer";

    while(m_deviceOperation->getAnswerPadStatus())   //一直按着算暂停
    {
        qDebug()<<"pausing";
        QApplication::processEvents();
    }
    QElapsedTimer elapsedTimer;
    elapsedTimer.restart();
    int waitTime;
    auto commonParams=m_resultModel->m_params.commonParams;
    if(m_answeredTimes.size()<=10||commonParams.responseAutoAdapt==false)
    {
        auto fixedParams=m_resultModel->m_params.fixedParams;
        waitTime=fixedParams.stimulationTime+fixedParams.intervalTime;
    }
    else
    {
        int sum=0;
        for(auto&i:m_answeredTimes)
        {
            sum+=i;
        }
        waitTime=sum/(m_answeredTimes.size())+commonParams.responseDelayTime;
    }
    qDebug()<<"wait Time is:"+QString::number(waitTime);


    while(elapsedTimer.elapsed()<waitTime)   //应答时间内
    {
        if(m_deviceOperation->getAnswerPadStatus())
        {
            m_answeredTimes.append(elapsedTimer.elapsed());
            return true;                    //时间内应答
        }
        else QApplication::processEvents();
    }
    return false;                       //超出时间应答
}

void StaticCheck::ProcessAnswer(bool answered)
{
    qDebug()<<"Process Answer.";
    auto lastCheckedDot=m_lastCheckDotRecord.takeFirst();
    auto lastCheckedDotType=m_lastCheckeDotType.takeFirst();
    switch (lastCheckedDotType)
    {
    case LastCheckedDotType::locateBlindDot:
    {
        if(answered)
        {
            if(m_resultModel->m_OS_OD) m_blindDot.push_back(UtilitySvc::getSingleton()->m_left_blindDot[m_blindDotLocateIndex]);
            else m_blindDot.push_back(UtilitySvc::getSingleton()->m_right_blindDot[m_blindDotLocateIndex]);
        }
        else
        {
            m_blindDotLocateIndex++;
        }
        break;
    }
    case LastCheckedDotType::blindDotTest:
    {
        m_resultModel->m_data.fixationDeviation.push_back(-m_deviceOperation->m_deviation);
        m_resultModel->m_data.fixationLostTestCount++;
        if(answered)
            m_resultModel->m_data.fixationLostCount++;
        break;
    }
    case LastCheckedDotType::falsePositiveTest:
    {
        m_resultModel->m_data.fixationDeviation.push_back(-m_deviceOperation->m_deviation);
        m_resultModel->m_data.falsePositiveTestCount++;
        if(!answered)
            m_resultModel->m_data.falsePositiveCount++;
        break;
    }
    case LastCheckedDotType::falseNegativeTest:
    {
        m_resultModel->m_data.fixationDeviation.push_back(-m_deviceOperation->m_deviation);
        m_resultModel->m_data.falseNegativeTestCount++;
        if(answered)
        {
            m_resultModel->m_data.falseNegativeCount++;
        }
    }
    case LastCheckedDotType::commonCheckDot:
    {
        m_resultModel->m_data.fixationDeviation.push_back(m_deviceOperation->m_deviation);
        int dotDB=lastCheckedDot->StimulationDBs.last();
        answered?lastCheckedDot->lowerBound=dotDB:lastCheckedDot->upperBound=dotDB;
        int boundDistance=lastCheckedDot->upperBound-lastCheckedDot->lowerBound;
        switch (m_programModel->m_params.commonParams.strategy)
        {
        case StaticParams::CommonParams::Strategy::fullThreshold:
        {
            if(boundDistance>4)
            {
                answered?lastCheckedDot->StimulationDBs.push_back(qMin((lastCheckedDot->lowerBound+4),MaxDB)):lastCheckedDot->StimulationDBs.push_back(qMax(lastCheckedDot->upperBound-4,MinDB));
            }
            if(boundDistance==4)
            {
                lastCheckedDot->StimulationDBs.push_back(lastCheckedDot->lowerBound+2);
            }
            if(boundDistance==2)
            {
                lastCheckedDot->DB=lastCheckedDot->lowerBound;
                lastCheckedDot->checked=true;
            }
            break;
        }
        case StaticParams::CommonParams::Strategy::fastThreshold:
        {
            if(boundDistance>6)
            {
                answered?lastCheckedDot->StimulationDBs.push_back(lastCheckedDot->lowerBound+6):lastCheckedDot->StimulationDBs.push_back(lastCheckedDot->upperBound-6);
            }
            if(boundDistance==6)
            {
                lastCheckedDot->StimulationDBs.push_back(lastCheckedDot->lowerBound+3);
            }
            if(boundDistance==3)
            {
                lastCheckedDot->DB=lastCheckedDot->lowerBound+1;
                lastCheckedDot->checked=true;
            }
            break;
        }
        case StaticParams::CommonParams::Strategy::smartInteractive:
        {
            if(boundDistance>4)
            {
                answered?lastCheckedDot->StimulationDBs.push_back(lastCheckedDot->lowerBound+4):lastCheckedDot->StimulationDBs.push_back(lastCheckedDot->upperBound-4);
            }
            if(boundDistance==4)
            {
                lastCheckedDot->StimulationDBs.push_back(lastCheckedDot->lowerBound+2);
            }
            if(boundDistance==2)
            {
                if(lastCheckedDot->isBaseDot)
                    lastCheckedDot->StimulationDBs.push_back(lastCheckedDot->lowerBound+1);
                else{
                    getProperValByRefToNearDotDB(*lastCheckedDot);
                    lastCheckedDot->checked=true;
                }
            }
            if(boundDistance==1)                        //仅最初的4点和中心点会走到这一步.
            {
                if(answered)
                {
                    lastCheckedDot->StimulationDBs.push_back(lastCheckedDot->lowerBound);
                    lastCheckedDot->checked=true;
                }
            }
            break;
        }
        case StaticParams::CommonParams::Strategy::fastInterative:
        {
            if(boundDistance>3)
            {
                answered?lastCheckedDot->StimulationDBs.push_back(lastCheckedDot->lowerBound+3):lastCheckedDot->StimulationDBs.push_back(lastCheckedDot->upperBound-3);
            }
            if(boundDistance==3)
            {
                lastCheckedDot->DB=lastCheckedDot->lowerBound+1;
                lastCheckedDot->checked=true;
            }
            break;
        }
        case StaticParams::CommonParams::Strategy::oneStage:
        {
            answered?lastCheckedDot->DB=2:lastCheckedDot->DB=0;
            lastCheckedDot->checked=true;
        }
        case StaticParams::CommonParams::Strategy::twoStages:
        {
            auto lastStimulatDB=lastCheckedDot->StimulationDBs.last();
            if(answered)
            {
                if(lastStimulatDB!=0)
                {
                    lastCheckedDot->DB=2;
                }
                else{
                    lastCheckedDot->DB=1;
                }
            }
            else
            {
                if(lastStimulatDB!=0)
                {
                    lastCheckedDot->StimulationDBs.push_back(0);
                }
                else{
                    lastCheckedDot->DB=0;
                }
            }
        }
        case StaticParams::CommonParams::Strategy::quantifyDefects:
        {

            if(boundDistance>3)
            {
                if(answered)
                {
                    if(lastCheckedDot->StimulationDBs.size()==1)
                        lastCheckedDot->DB=2;
                }
                else
                    lastCheckedDot->StimulationDBs.push_back(lastCheckedDot->upperBound-3);
            }
            if(boundDistance==3)
            {
                lastCheckedDot->DB=lastCheckedDot->StimulationDBs.first()+4/*得到标准值*/-(lastCheckedDot->lowerBound+1);      //缺陷深度的值,4起步所以不与看到(2)的整数值冲突.
                lastCheckedDot->checked=true;
            }
            break;
        }
        case StaticParams::CommonParams::Strategy::singleStimulation:
        {
            answered?lastCheckedDot->DB=2:lastCheckedDot->DB=0;
            lastCheckedDot->checked=true;
        }
        }
        if(lastCheckedDot->checked==true)
        {
            if(lastCheckedDot->index<=int(m_programModel->m_data.dots.size()))                //非短周期
            {
                m_checkedCount++;
                m_resultModel->m_data.checkData[lastCheckedDot->index]=lastCheckedDot->DB;          //在check初始化的时候扩充了大小.
                m_resultModel->m_data.realTimeDB[lastCheckedDot->index]=lastCheckedDot->StimulationDBs.toStdVector();
                if(m_resultModel->m_params.commonParams.shortTermFluctuation)
                {
                    m_lastShortTermCycleCheckedDotIndex.push_back(lastCheckedDot->index);         //加入最近检测点的集合
                    if(m_lastShortTermCycleCheckedDotIndex.size()==m_programModel->m_params.fixedParams.fixationViewLossCycle)
                    {
                        auto selectedIndex=m_lastShortTermCycleCheckedDotIndex[qrand()%m_lastShortTermCycleCheckedDotIndex.size()];
                        m_lastShortTermCycleCheckedDotIndex.clear();
                        auto dotRecord=m_dotRecords[selectedIndex];
                        dotRecord.lowerBound=MinDB;
                        dotRecord.upperBound=MaxDB;
                        dotRecord.StimulationDBs.clear();
                        dotRecord.realTimeEyePosPic.clear();
                        dotRecord.checked=false;
                        DotRecord dotfluc;
                        dotfluc.index=m_programModel->m_data.dots.size()+dotRecord.index;   //短周期的话编号加上程序点个数
                        m_shortTermFlucRecords.push_back(dotfluc);
                    }
                }
            }
            else                                                                        //存储结果
            {
                m_resultModel->m_data.checkData[lastCheckedDot->index]=lastCheckedDot->DB;
            }
        }
    }
    }
}

void StaticCheck::waitAndProcessAnswer()
{
    auto answerResult=waitForAnswer();
    ProcessAnswer(answerResult);
}


class DynamicCheck:public Check
{

public:
    QSharedPointer<DynamicCheckResultModel> m_resultModel;
    QSharedPointer<DynamicProgramModel> m_programModel;
    DynamicCheck()=default;
    ~DynamicCheck()=default;
private:
    QSharedPointer<UtilitySvc> m_utilitySvc=UtilitySvc::getSingleton();
    QVector<int> unCheckedIndex;

    virtual void initialize() override;

    virtual void Checkprocess() override;
};


class CheckSvcWorker : public QObject
{
    Q_OBJECT
public:
    QElapsedTimer m_elapsedTimer;
    int* m_checkState;
    PatientVm* m_patientVm;
    ProgramVm* m_programVm;
    CheckResultVm* m_checkResultVm;
private:
    QSharedPointer<Check> m_check;
public:
    explicit CheckSvcWorker(){}
    virtual ~CheckSvcWorker() Q_DECL_OVERRIDE {}
    void initialize();
    void setCheckState(int value)
    {
        *m_checkState=value;
        emit checkStateChanged();
    }
public slots:
    void connectDev()
    {
        qDebug()<<"connect dev";
        DevOps::DeviceOperation::getSingleton()->connectDev();    //连接设备
    }
    void disconnectDev()
    {
        qDebug()<<"disconnect dev";
        DevOps::DeviceOperation::getSingleton()->disconnectDev();    //连接设备
    }
    void doWork();

signals:
    void checkStateChanged();
    void checkResultChanged();
    void checkProcessFinished();
    void checkedCountChanged(int count);
};

void CheckSvcWorker::initialize()
{
    qDebug()<<("initializing");
    qsrand(QTime::currentTime().second());
    int type=m_programVm->getType();
    qDebug()<<type;
    if(type!=2)
    {
        m_check.reset(new StaticCheck());
        m_check->m_patientModel=m_patientVm->getModel();
        ((StaticCheck*)m_check.data())->m_resultModel=static_cast<StaticCheckResultVm*>(m_checkResultVm)->getModel();
        qDebug()<<QString::number((((StaticCheck*)m_check.data())->m_resultModel)->m_id);
        ((StaticCheck*)m_check.data())->m_programModel=static_cast<StaticProgramVm*>(m_programVm)->getModel();
    }
    else
    {
        m_check.reset(new DynamicCheck());
        m_check->m_patientModel=m_patientVm->getModel();
        ((DynamicCheck*)m_check.data())->m_resultModel=static_cast<DynamicCheckResultVm*>(m_checkResultVm)->getModel();
        ((DynamicCheck*)m_check.data())->m_programModel=static_cast<DynamicProgramVm*>(m_programVm)->getModel();
    }
    emit checkedCountChanged(0);
    emit checkResultChanged();
}

void CheckSvcWorker::doWork()
{
    *m_checkState=0;
    while(true)
    {
        switch (*m_checkState)
        {
        case 0:                                             //start
        {
            initialize();
            m_check->initialize();
//            setCheckState(3);
//            return;
            setCheckState(1);
            break;
        }
        case 1:                                             //check
        {
            qDebug()<<("Checking");
            m_check->Checkprocess();

//            qDebug()<<m_check->m_checkedCount;
//            qDebug()<<m_check->m_totalCount;
            if(m_check->m_checkedCount==m_check->m_totalCount) setCheckState(4);
            emit checkResultChanged();
            break;
        }
        case 2:                                             //pause
        {
            qDebug()<<("pausing");
            UtilitySvc::wait(500);
            break;
        }
        case 3:                                             //stop
        {
            qDebug()<<("stopped");
            return;
        }
        case 4:                                             //finish
        {
            m_checkResultVm->insert();
            qDebug()<<("finished");
            emit checkProcessFinished();
            return;
        }
        };
        UtilitySvc::wait(1000);
    }
}




CheckSvc::CheckSvc(QObject *parent)
{
//    qDebug()<<"mianThread:"+QString::number(int(thread()->currentThread()),16);
    m_worker = new CheckSvcWorker();
    m_worker->moveToThread(&m_workerThread);
    DevOps::DeviceOperation::getSingleton()->moveToThread(&m_workerThread);
    connect(m_worker,&CheckSvcWorker::checkResultChanged,this, &CheckSvc::checkResultChanged);
    connect(m_worker,&CheckSvcWorker::checkStateChanged,this, &CheckSvc::checkStateChanged);
    connect(m_worker,&CheckSvcWorker::checkedCountChanged,this, &CheckSvc::setCheckedCount);
    connect(m_worker,&CheckSvcWorker::checkProcessFinished,this, [&](){m_checkResultVm->insert();});
    connect(DevOps::DeviceOperation::getSingleton().data(),&DevOps::DeviceOperation::isDeviceReadyChanged,this,&CheckSvc::devReadyChanged);
    connect(DevOps::DeviceOperation::getSingleton().data(),&DevOps::DeviceOperation::pupilDiameterChanged,this,&CheckSvc::pupilDiameterChanged);
//    connect(DevOps::DeviceOperation::getSingleton().data(),&DevOps::DeviceOperation::newFrameData,FrameProvidSvc::getSingleton().data(),&FrameProvidSvc::onNewVideoContentReceived);
    m_workerThread.start();
}

CheckSvc::~CheckSvc()
{
//    m_workerThread.quit();
    m_workerThread.terminate();
    m_worker->deleteLater();
}

void CheckSvc::start()
{
    if(m_checkResultVm==nullptr)
    {
        qDebug()<<QString::number(m_checkResultVm->getPatient_id());
    }
    m_worker->m_patientVm=m_patientVm;
    m_worker->m_programVm=m_programVm;
    m_worker->m_checkResultVm=m_checkResultVm;
    m_worker->m_checkState=&m_checkState;
    setCheckState(0);
    qDebug()<<"start command";
    QMetaObject::invokeMethod(m_worker,"doWork",Qt::QueuedConnection);
}

void CheckSvc::pause()
{
    setCheckState(2);
    qDebug()<<"pause command";
//    QMetaObject::invokeMethod(m_worker,"pause",Qt::QueuedConnection);
}

void CheckSvc::resume()
{
    setCheckState(1);
     qDebug()<<"resume command";
//    QMetaObject::invokeMethod(m_worker,"resume",Qt::QueuedConnection);
}

void CheckSvc::stop()
{
    setCheckState(3);
    qDebug()<<"stop command";
    //    QMetaObject::invokeMethod(m_worker,"stop",Qt::QueuedConnection);
}

void CheckSvc::connectDev()
{
//  有设备之后启用
    qDebug()<<"connect";
    QMetaObject::invokeMethod(m_worker,"connectDev",Qt::QueuedConnection);
}

void CheckSvc::disconnectDev()
{
    qDebug()<<"connect";
    QMetaObject::invokeMethod(m_worker,"disconnectDev",Qt::QueuedConnection);
}

void CheckSvc::moveChinUp()
{
    DevOps::DeviceOperation::getSingleton()->moveChinUp();
}

void CheckSvc::moveChinDown()
{
    DevOps::DeviceOperation::getSingleton()->moveChinDown();
}

void CheckSvc::moveChinLeft()
{
    DevOps::DeviceOperation::getSingleton()->moveChinLeft();
}

void CheckSvc::moveChinRight()
{
    DevOps::DeviceOperation::getSingleton()->moveChinRight();
}

void CheckSvc::stopMovingChin()
{
    DevOps::DeviceOperation::getSingleton()->stopMovingChin();
}

void CheckSvc::turnOnVideo()
{
    qDebug()<<"trunOnVideo";
    DevOps::DeviceOperation::getSingleton()->turnOnVideo();
}

void CheckSvc::turnOffVideo()
{
    qDebug()<<"trunOffVideo";
    DevOps::DeviceOperation::getSingleton()->turnOffVideo();
}

bool CheckSvc::getDevReady()
{
    return DevOps::DeviceOperation::getSingleton()->getIsDeviceReady();
}

bool CheckSvc::getAutoAlignPupil()
{
    return DevOps::DeviceOperation::getSingleton()->getAutoAlignPupil();
}

void CheckSvc::setAutoAlignPupil(bool autoAlign)
{
    DevOps::DeviceOperation::getSingleton()->setAutoAlignPupil(autoAlign);
}

float CheckSvc::getPupilDiameter()
{
    return DevOps::DeviceOperation::getSingleton()->getPupilDiameter();
}





void DynamicCheck::initialize()
{
    m_resultModel->m_patient_id=m_patientModel->m_id;
    m_resultModel->m_program_id=m_programModel->m_id;
    m_checkedCount=0;
    m_totalCount=m_programModel->m_data.dots.size();
    for(int i=0;i<m_totalCount;i++)
    {
        unCheckedIndex.push_back(i);
        m_resultModel->m_data.checkData.push_back(DynamicDataNode{std::to_string('A'+i),m_programModel->m_data.dots[i],{0,0},false});
    }
}

void DynamicCheck::Checkprocess()
{
    if(m_programModel->m_params.strategy==DynamicParams::Strategy::standard)
    {
        int dotIndex=unCheckedIndex.takeAt(qrand()%unCheckedIndex.size());
        auto dataNode=m_resultModel->m_data.checkData[dotIndex];
//        deviceSvc->dynamicStimulate(QPointF{dataNode.start.x,dataNode.start.y},QPointF{dataNode.end.x,dataNode.end.y},1);
//        bool isSeen=deviceSvc->waitForAnswer({4,5});             //TODO 填入motorIDS
        //            if(isSeen)
    }
}
}
#include "check_svc.moc"

