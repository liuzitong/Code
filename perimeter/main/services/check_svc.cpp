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
#include <array>
#include <QMessageBox>
namespace Perimeter{
constexpr int MaxDB=51;
constexpr int MinDB=0;
class Check:public QObject
{
    Q_OBJECT
public:
    Check()=default;
    ~Check()=default;
    QSharedPointer<PatientModel> m_patientModel;
    int m_checkedCount,m_totalCount;   //只计算通常点数,短周期,中心点,以及周期测试不计入内.m_checkedCount=m_totalCount就测试完成.在doWork里面结束测试循环.
    int *m_checkState;
    bool m_error=false;
    QString m_errorInfo;
    QSharedPointer<UtilitySvc> m_utilitySvc=UtilitySvc::getSingleton();
    QSharedPointer<DevOps::DeviceOperation> m_deviceOperation=DevOps::DeviceOperation::getSingleton();
    static constexpr int y_offsetDiamond=-8,y_offsetBottomPoint=-12;
    virtual void Checkprocess()=0;
    virtual void initialize()=0;
    virtual void finished()=0;
//signals:
//    void  checkStateChanged();
private:
    virtual void setLight(bool onOff)=0;

};

class StaticCheck:public Check
{
    Q_OBJECT
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
        bool isBaseDot;                                  //fast smart interactive模式的最初4点
        bool checked;
        int lowerBound;
        int upperBound;
    };
    QVector<int> m_value_30d;
    QVector<int> m_value_60d;
    int m_y_offset;
    QVector<DotRecord> m_dotRecords;    //index为位置0~m_totalCount-1
    bool m_isStartWithBaseDots;         //基础4点开始检查.
    bool m_isDoingBaseDotsCheck;        //处于给基础4点测试定值阶段
    DotRecord m_centerDotRecord;        //index 为2*m_totalCount
    QVector<int> m_answeredTimes;    //根据此处得到autoAdaptTime;
    int m_autoAdaptTime;
    QVector<int> m_lastShortTermCycleCheckedDotIndex;
    QVector<DotRecord> m_shortTermFlucRecords;          //index为 m_totalCount~2*m_totalCount-1
//    int m_falsePosCycCount=qrand()%10,m_falseNegCycCount=qrand()%10,m_fiaxationViewLossCyc=qrand()%10;  //错开
    QVector<QPointF> m_blindDot;
    int m_blindDotLocateIndex=0;
    int m_stimulationCount=0;                   //刺激次数到了测试盲点位置
    bool m_stimulated;
    QVector<DotRecord*> m_lastCheckDotRecord;
    QVector<LastCheckedDotType> m_lastCheckeDotType;
    QElapsedTimer m_stimulationWaitingForAnswerElapsedTimer;      //开门开启等待应答
    bool m_alreadyChecked;

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
    virtual void finished() override;

signals:
    void currentCheckingDotChanged(int index);

private:
    std::tuple<bool,QPointF,int> getCheckCycleLocAndDB();

    DotRecord& getCheckDotRecordRef();

    void stimulate();

    void getReadyToStimulate(QPointF loc, int DB);

    void getProperValByRefToNearDotDB(DotRecord& dotRecord);

    bool waitForAnswer();

    void ProcessAnswer(bool answered);

    void waitAndProcessAnswer();

    void checkWaiting();

    virtual void setLight(bool onOff) override;


};

class DynamicCheck:public Check
{
    Q_OBJECT
    struct PathRecord
    {
        int index;
        QPointF beginLoc;
        QPointF endLoc;
        QPointF answeredLoc;
        bool isAnswered;
        bool checked;

//        void print()
//        {
//            qDebug()<<QString("index:%1,beginLoc:%2,endLoc:%3,answeredLoc%4,isAnswered:%5,checked:%6.")
//                      .arg(QString::number(index).arg(QString(beginLoc.)));
//        }
    };

public:
    QSharedPointer<DynamicCheckResultModel> m_resultModel;
    QSharedPointer<DynamicProgramModel> m_programModel;
    DynamicCheck()=default;
    ~DynamicCheck()=default;

private:
    QSharedPointer<UtilitySvc> m_utilitySvc=UtilitySvc::getSingleton();

    QVector<PathRecord> m_records;

    int m_speedLevel;

    int m_cursorSize;

    QTimer m_timer;

    virtual void initialize() override;

    virtual void Checkprocess() override;

    int getPathRecordIndex();

    void stimulate(QPointF begin,QPointF end);

    QVector<QPointF> waitForAnswer();

    void ProcessAnswer(QVector<QPointF> answerLocs,PathRecord& record);

    virtual void setLight(bool onOff) override;

    // Check interface
public:
    virtual void finished() override;

    QList<QPoint> m_dynamicSelectedDots;
};


class CheckSvcWorker : public QObject
{
    Q_OBJECT
public:
    QTimer m_timer;
    int m_time=0;
    int* m_checkState;
    PatientVm* m_patientVm;
    ProgramVm* m_programVm;
    CheckResultVm* m_checkResultVm;
    QList<QPoint> m_dynamicSelectedDots;

private:
    QSharedPointer<Check> m_check;
public:
    explicit CheckSvcWorker(){m_timer.setInterval(1000);connect(&m_timer,&QTimer::timeout,this,[&](){emit checkTimeChanged(m_time);m_time++;});}
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
        DevOps::DeviceOperation::getSingleton()->connectDev();    //连接设备
    }
    void disconnectDev()
    {
        DevOps::DeviceOperation::getSingleton()->disconnectDev();    //连接设备
    }
    void doWork();

signals:
    void checkStateChanged();
    void checkResultChanged();
    void checkProcessFinished();
    void checkedCountChanged(int count);
    void totalCountChanged(int count);
    void checkTimeChanged(int secs);
    void sendErrorInfo(QString error);
    void stop();
    void currentCheckingDotChanged(int index);

};

void StaticCheck::initialize()
{
    constexpr int initialNumber=999;
    m_checkedCount=0;
    m_autoAdaptTime=0;
    m_blindDotLocateIndex=0;
    m_stimulationCount=0;
    m_stimulated=false;
    m_alreadyChecked=false;
    m_isStartWithBaseDots=false;
    m_isDoingBaseDotsCheck=false;
    m_resultModel->m_patient_id=m_patientModel->m_id;
    m_resultModel->m_program_id=m_programModel->m_id;
    m_resultModel->m_videoSize=m_deviceOperation->m_videoSize;
    m_totalCount=m_programModel->m_data.dots.size();
    m_resultModel->m_data.checkData=std::vector<int>(m_totalCount*2+1,-initialNumber);  //第一段程序点个数为测出点DB,第二段程序点个数为短波周期DB,第三段为中心点DB
    m_resultModel->m_data.realTimeDB.resize(m_totalCount*2+1);
    m_resultModel->m_imgData.resize(m_totalCount*2+1);
    auto cursorSize=m_programModel->m_params.commonParams.cursorSize;
    auto cursorColor=m_programModel->m_params.commonParams.cursorColor;
    m_value_30d=m_utilitySvc->getValue30d(int(cursorSize),int(cursorColor),m_patientModel->m_age);
    m_value_60d=m_utilitySvc->m_value_60d;
    emit currentCheckingDotChanged(-1);
    switch (m_resultModel->m_params.commonParams.fixationTarget)
    {
    case FixationTarget::centerPoint:m_y_offset=0;break;
    case FixationTarget::bigDiamond:
    case FixationTarget::smallDiamond:m_y_offset=y_offsetDiamond;break;
    case FixationTarget::bottomPoint:m_y_offset=y_offsetBottomPoint;break;
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

//    QPointF centerDotLoc;
//    if(m_resultModel->m_params.commonParams.fixationTarget==FixationTarget::centerPoint)
//    {
//        centerDotLoc={0,float(y_offsetDiamond)};
//    }

    if(m_programModel->m_params.commonParams.strategy!=StaticParams::CommonParams::Strategy::singleStimulation)
    {
        for(int i=0;i<m_totalCount;i++)
        {
            bool isBaseDot=false;
            auto dot=m_programModel->m_data.dots[i];
            if(m_resultModel->m_OS_OD!=0) dot.x=-dot.x;
            if(m_isStartWithBaseDots)
            {
                auto baseDots=m_programModel->m_data.baseDots;
                for(uint j=0;j<baseDots.size();j++)
                    if((qAbs(dot.x-baseDots[j].x)<FLT_EPSILON)&&(qAbs(dot.y-baseDots[j].y)<FLT_EPSILON))
                        isBaseDot=true;
            }
            //非参考点初始测试DB设置为-999,之后选点的时候根据周围已经检查出的值赋值
            QVector<int> stimulationDBs;
            if(isBaseDot||!m_isStartWithBaseDots)
            {
                stimulationDBs={m_utilitySvc->getExpectedDB(m_value_30d,{dot.x,dot.y},m_resultModel->m_OS_OD)+DBChanged};
            }

            m_dotRecords.push_back(DotRecord{i,QPointF{dot.x,dot.y},stimulationDBs,-initialNumber,isBaseDot,false,-initialNumber,initialNumber});
        }

        m_centerDotRecord=DotRecord{m_totalCount*2,QPointF{0,0},{m_utilitySvc->getExpectedDB(m_value_30d,{0,0},m_resultModel->m_OS_OD)+DBChanged},-initialNumber,false,false,-initialNumber,initialNumber};
    }
    else                            //单刺激
    {
        auto DB=m_programModel->m_params.fixedParams.singleStimulationDB;
        for(int i=0;i<m_totalCount;i++)
        {
            auto dot=m_programModel->m_data.dots[i];
            if(m_resultModel->m_OS_OD!=0) dot.x=-dot.x;
            m_dotRecords.push_back(DotRecord{i,QPointF{dot.x,dot.y},{DB},-initialNumber, false,false,-initialNumber,initialNumber});
        }
        m_centerDotRecord=DotRecord{m_totalCount*2,QPointF{0,0},{DB},-initialNumber,false,false,-initialNumber,initialNumber};
    }

    setLight(true);
    m_deviceOperation->lightUpCastLight();
    m_deviceOperation->m_isChecking=true;
    m_deviceOperation->m_isWaitingForStaticStimulationAnswer=false;
    m_deviceOperation->m_staticStimulationAnswer=false;
//    m_deviceOperation->m_stimulationTime=m_programModel->m_params.fixedParams.stimulationTime;
    m_deviceOperation->setCursorColorAndCursorSize(int(cursorColor),int(cursorSize));
    m_deviceOperation->waitMotorStop({UsbDev::DevCtl::MotorId_Focus,UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Light_Spot,UsbDev::DevCtl::MotorId_X,UsbDev::DevCtl::MotorId_Y});
    UtilitySvc::wait(4000);    //等几秒启动
}

//第一次先跑点,直接刺激,第二次跑点,等待上次刺激的应答,然后再刺激,再跑点,再等待上次的应答.(保证再等待应答的同时跑点.)
void StaticCheck::Checkprocess()
{

    m_alreadyChecked=false;
    auto checkCycleLocAndDB=getCheckCycleLocAndDB();                //存储LastdotType为各种检查
    if(m_error==true) return;                                       //找不到盲点 退出检查
    if(std::get<0>(checkCycleLocAndDB))
    {
//        qDebug()<<"checkCycleLocAndDB loc:"<<std::get<1>(checkCycleLocAndDB)<<" DB"<<std::get<2>(checkCycleLocAndDB);
        m_lastCheckDotRecord.push_back(nullptr);
        getReadyToStimulate(std::get<1>(checkCycleLocAndDB),std::get<2>(checkCycleLocAndDB));
    }
    else
    {
        m_lastCheckDotRecord.push_back(&getCheckDotRecordRef());   //存储lastDotType为commondot 并且存储指针
//        auto dotRec=m_lastCheckDotRecord.last();
//        qDebug()<<"getCheckDotRecordRef loc:"<<dotRec->loc<<" DB:"<<QString::number(dotRec->StimulationDBs.last())<<"upper:"<<QString::number(dotRec->upperBound)<<"lower:"<<QString::number(dotRec->lowerBound);
        emit currentCheckingDotChanged(m_lastCheckDotRecord.last()->index);
        getReadyToStimulate(m_lastCheckDotRecord.last()->loc,m_lastCheckDotRecord.last()->StimulationDBs.last());
    }


    if(m_stimulated)                               //最开始没刺激过就不需要处理
    {
        m_stimulated=false;
        waitAndProcessAnswer();                             //取出commonDot 并且取出指针,处理的时候可能发现下一个是已经检查出结果的点,这个时候就选择不刺激置m_alreadyChecked为true
    }

    checkWaiting();

    if(m_checkedCount<m_totalCount&&!m_alreadyChecked)                         //如果测试完毕或者是已经得到结果的点就不刺激了
    {
        m_stimulationCount++;
        m_stimulated=true;
        stimulate();
    }
}

void StaticCheck::finished()
{
    m_deviceOperation->m_isChecking=false;
    m_deviceOperation->dimDownCastLight();
    setLight(false);
    m_deviceOperation->setLamp(DevOps::LampId::LampId_eyeglassInfrared,0,false);
    m_deviceOperation->setLamp(DevOps::LampId::LampId_borderInfrared,0,false);
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
            if(!m_shortTermFlucRecords[m_shortTermFlucRecords.size()-2].checked)
            {
                return m_shortTermFlucRecords[m_shortTermFlucRecords.size()-2];             //之前一个周期没测出短波要直接测完
            }
        }
        else
        {
            if(qrand()%(1+m_resultModel->m_params.fixedParams.shortTermFluctuationCount)==0||m_checkedCount==m_totalCount-1)    //周期内随机到了短波周期,最后一轮了,先
            {
                if(!m_shortTermFlucRecords.last().checked)
                    return m_shortTermFlucRecords.last();
            }
        }
    }
    if(m_isStartWithBaseDots)
    {
        qsrand(QTime::currentTime().msec());
        QVector<DotRecord> zoneRightTop,zoneRightBottom,zoneLeftBottom,zoneLeftTop;
        QVector<DotRecord> zoneCheckedRightTop,zoneCheckedRightBottom,zoneCheckedLeftBottom,zoneCheckedLeftTop;
        QPointF zoneRightTopCenter,zoneRightBottomCenter,zoneLeftBottomCenter,zoneLeftTopCenter;
        QVector<int> zone;
        for(auto &i:m_dotRecords)                               //得到四个区域的参考点,并且把未测四点放入4个区域
        {
            auto loc=i.loc;
            if(loc.x()>0&&loc.y()>0)
            {
                if(!i.checked) zoneRightTop.append(i);
                else zoneCheckedRightTop.append(i);
                if(i.isBaseDot) zoneRightTopCenter=loc;
            }
            if(loc.x()>0&&loc.y()<0)
            {
                if(!i.checked) zoneRightBottom.append(i);
                else zoneCheckedRightBottom.append(i);
                if(i.isBaseDot) zoneRightBottomCenter=loc;
            }
            if(loc.x()<0&&loc.y()<0)
            {
                if(!i.checked) zoneLeftBottom.append(i);
                else zoneCheckedLeftBottom.append(i);
                if(i.isBaseDot) zoneLeftBottomCenter=loc;
            }
            if(loc.x()<0&&loc.y()>0)
            {
                if(!i.checked) zoneLeftTop.append(i);
                else zoneCheckedLeftTop.append(i);
                if(i.isBaseDot) zoneLeftTopCenter=loc;
            }
        }

        if(zoneRightTop.count()>0) zone.push_back(0);
        if(zoneRightBottom.count()>0) zone.push_back(1);
        if(zoneLeftBottom.count()>0) zone.push_back(2);
        if(zoneLeftTop.count()>0) zone.push_back(3);

        auto zoneNumber=zone[qrand()%zone.size()];                                                  //随机出区域和参考点坐标
        QVector<DotRecord> seletedZoneRecords;
        QVector<DotRecord> seletedZoneCheckedRecords;
        QPointF centerCoord;
        switch (zoneNumber)
        {
        case 0:seletedZoneRecords=zoneRightTop;seletedZoneCheckedRecords=zoneCheckedRightTop;centerCoord=zoneRightTopCenter;break;
        case 1:seletedZoneRecords=zoneRightBottom;seletedZoneCheckedRecords=zoneCheckedRightBottom;centerCoord=zoneRightBottomCenter;break;
        case 2:seletedZoneRecords=zoneLeftBottom;seletedZoneCheckedRecords=zoneCheckedLeftBottom;centerCoord=zoneLeftBottomCenter;break;
        case 3:seletedZoneRecords=zoneLeftTop;seletedZoneCheckedRecords=zoneCheckedLeftTop;centerCoord=zoneLeftTopCenter;break;
        }
//        qDebug()<<"centerCoord:"<<centerCoord;


        float nearestDist=FLT_MAX;
        QVector<DotRecord> nearestRecords;                                                          //选出最近点
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

//        qDebug()<<nearestRecords.length();
//        qDebug()<<nearestRecords[0].loc;
        nearestDist=FLT_MAX;
        auto& selectedDot=nearestRecords[qrand()%nearestRecords.count()];
//        qDebug()<<"selectedDot loc:"<<selectedDot.loc;
        if(selectedDot.StimulationDBs.isEmpty())            //第一次检查要根据周围点的结果赋值,且说明这个点是非baseDot,baseDot会在开始的时候赋值
        {
//            qDebug()<<"empty stimDB";
            for(auto&i:seletedZoneCheckedRecords)
            {
//                qDebug()<<"got  checked";
                auto dist=sqrt(pow((i.loc.x()-selectedDot.loc.x()),2)+pow((i.loc.y()-selectedDot.loc.y()),2));
                if(dist<nearestDist)
                {
                    m_dotRecords[selectedDot.index].StimulationDBs.clear();
                    int DB=qMin(qMax(i.DB,0),51);
                    m_dotRecords[selectedDot.index].StimulationDBs.append(DB);
                    nearestDist=dist;
//                    qDebug()<<"appended DB";
                }
            }
        }
//        qDebug()<<"getCheckDotRecordRef loc inside:"<<m_dotRecords[selectedDot.index].loc<<"DB leng"<<QString::number(m_dotRecords[selectedDot.index].StimulationDBs.count())<<"upper:"<<QString::number(m_dotRecords[selectedDot.index].upperBound)<<"lower:"<<QString::number(m_dotRecords[selectedDot.index].lowerBound);
//        qDebug()<<" DB:"<<QString::number(m_dotRecords[selectedDot.index].StimulationDBs.last());
        return m_dotRecords[selectedDot.index];
    }
    else
    {
        QVector<int> uncheckedDotIndex;
        int selectedDotIndex;
        for(auto&i:m_dotRecords)
        {
            if(!i.checked)
            {

//                if(m_lastCheckDotRecord.isEmpty()||m_lastCheckDotRecord.first()==nullptr)
//                {
//                    unchekedDotIndex.push_back(i.index);
//                }
//                else
//                {
//                    if(i.index!=m_lastCheckDotRecord.first()->index)                        //必须排除上次的,连续两次可能导致同时是最后一次检查,连续的检查完毕同一个点
                    {
                        uncheckedDotIndex.push_back(i.index);                                 //检查的编号加入容器
                    }
//                }
            }
        }


        //就剩最后一个了
        if(uncheckedDotIndex.count()!=0)
        {
            selectedDotIndex=uncheckedDotIndex[qrand()%uncheckedDotIndex.count()];
        }
//        else
//        {
//            for(auto&i:m_dotRecords)
//            {
//                if(!i.checked)
//                {
//                   selectedDotIndex=i.index;
//                }
//            }
//        }



//        qDebug()<<"unchecked Dots:"<<uncheckedDotIndex;
        auto& selectDot=m_dotRecords[selectedDotIndex];
//        qDebug()<<"selected Dot:"<<selectDot.index;
        return selectDot;                //随机出一个
    }
}

void StaticCheck::stimulate()
{
    int durationTime=m_programModel->m_params.fixedParams.stimulationTime;
    auto lastCheckedDotType=m_lastCheckeDotType.last();
    if(lastCheckedDotType!=LastCheckedDotType::falseNegativeTest)               //假阴不开快门
    {
//        qDebug()<<QString("deviation is:")+QString::number(m_deviceOperation->m_deviation);
        m_deviceOperation->waitMotorStop({UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Light_Spot,UsbDev::DevCtl::MotorId_Focus,UsbDev::DevCtl::MotorId_X,UsbDev::DevCtl::MotorId_Y,UsbDev::DevCtl::MotorId_Shutter});
        m_deviceOperation->openShutter(durationTime);
        switch (lastCheckedDotType)
        {
        case LastCheckedDotType::blindDotTest:
        case LastCheckedDotType::locateBlindDot:
        case LastCheckedDotType::falsePositiveTest:m_resultModel->m_data.fixationDeviation.push_back(-m_deviceOperation->m_deviation);break;
        case LastCheckedDotType::commonCheckDot:
        {
            m_resultModel->m_data.fixationDeviation.push_back(m_deviceOperation->m_deviation);
            uint dotIndex=m_lastCheckDotRecord[0]->index;
            m_resultModel->m_data.realTimeDB[dotIndex]=m_lastCheckDotRecord[0]->StimulationDBs.toStdVector(); //在check初始化的时候扩充了大小.
            if(dotIndex<m_programModel->m_data.dots.size()||dotIndex==2*m_programModel->m_data.dots.size())
                m_resultModel->m_imgData[dotIndex].push_back(m_deviceOperation->m_frameRawData);
            break;
        }
        default:break;
        }
    }
    else
    {
        m_deviceOperation->waitForSomeTime(durationTime);           //假阴
        m_resultModel->m_data.fixationDeviation.push_back(-m_deviceOperation->m_deviation);
    }
    m_deviceOperation->m_isWaitingForStaticStimulationAnswer=true;
    m_stimulationWaitingForAnswerElapsedTimer.restart();
}

void StaticCheck::getReadyToStimulate(QPointF loc, int DB)
{
    static bool isMainTable=true;
    if(loc.x()==0&&loc.y()==0&&m_y_offset==0)
    {
        loc.ry()=y_offsetDiamond;                       //固视点为中心点时候的中心点检查
    }

    isMainTable=UtilitySvc::getIsMainTable(loc,isMainTable);
    if(DB<MinDB) DB=MinDB;
    if(DB>MaxDB) DB=MaxDB;
    m_deviceOperation->getReadyToStimulate({loc.x(),loc.y()+m_y_offset},int(m_resultModel->m_params.commonParams.cursorSize),DB,isMainTable);
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
    if(m_blindDotLocateIndex>=UtilitySvc::getSingleton()->m_left_blindDot.size())
    {
        m_error=true;
        m_errorInfo=tr("Cant't locate blind dot.Turn off blindDot check.");
        return {};
    }
    if(commomParams.blindDotTest==true)                 //盲点测试,size过大表示盲点寻找失败
    {
        //确定盲点,条件是要经历一点测试次数,而且盲为空
        if(m_stimulationCount>UtilitySvc::getSingleton()->m_checkCountBeforeGetBlindDotCheck&&m_blindDot.isEmpty())
        {
            QPoint blindDotLoc;
            if(m_resultModel->m_OS_OD==0)
                blindDotLoc=UtilitySvc::getSingleton()->m_left_blindDot[m_blindDotLocateIndex];
            else
                blindDotLoc=UtilitySvc::getSingleton()->m_right_blindDot[m_blindDotLocateIndex];

            m_lastCheckeDotType.push_back(LastCheckedDotType::locateBlindDot);
            return {true,blindDotLoc,m_stimulationCount>UtilitySvc::getSingleton()->m_blindDotTestDB};
        }

        //盲点不为空的时候到了周期测试盲点.
        if(!m_blindDot.isEmpty()&&m_stimulationCount%fixedParams.fixationViewLossCycle==qrand()%fixedParams.fixationViewLossCycle)
        {
            auto blindDB=UtilitySvc::getSingleton()->m_blindDotTestDB+UtilitySvc::getSingleton()->m_blindDotTestIncDB;
            m_lastCheckeDotType.push_back(LastCheckedDotType::blindDotTest);
            return {true,{m_blindDot[0].x(),m_blindDot[0].y()},blindDB};
        }
    }


    if(m_stimulationCount%fixedParams.falsePositiveCycle==qrand()%fixedParams.falsePositiveCycle)         //假阳
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


    if(m_stimulationCount%fixedParams.falseNegativeCycle==qrand()%fixedParams.falseNegativeCycle)         //假阴,随机点,到刺激的时候不开快门
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
//    qDebug()<<"waitForAnswer";
    int waitTime;
    auto commonParams=m_resultModel->m_params.commonParams;
    auto fixedParams=m_resultModel->m_params.fixedParams;
    if(m_answeredTimes.size()<=10||commonParams.responseAutoAdapt==false)
    {

        waitTime=fixedParams.stimulationTime+fixedParams.intervalTime;
//        qDebug()<<"fixxed wait Time is:"+QString::number(waitTime);
    }
    else
    {
        constexpr int maxWaitTime=4000;
        int sum=0;
        for(auto&i:m_answeredTimes) sum+=i;
        waitTime=qMin(sum/(m_answeredTimes.size())+commonParams.responseDelayTime,maxWaitTime);
        m_answeredTimes.pop_front();
//        qDebug()<<"autoAdapt wait Time is:"+QString::number(waitTime);
    }


    std::cout<<"start time count time"<<std::endl;
    bool answer=false;
    while((m_stimulationWaitingForAnswerElapsedTimer.elapsed()<waitTime)&&(!answer))   //应答时间内
    {
        if(m_deviceOperation->m_staticStimulationAnswer)
        {

            m_deviceOperation->m_isWaitingForStaticStimulationAnswer=false;
            m_deviceOperation->m_staticStimulationAnswer=false;
            answer=true;
            UtilitySvc::wait(m_programModel->m_params.fixedParams.leastWaitingTime);                //最小等待时间
        }
        else QApplication::processEvents();
    }


    std::cout<<"end time count time"<<std::endl;
    std::cout<<(QString("answer Time is:")+QString::number(m_stimulationWaitingForAnswerElapsedTimer.elapsed())).toStdString()<<std::endl;
    m_answeredTimes.append(m_stimulationWaitingForAnswerElapsedTimer.elapsed());
    return answer;                       //超出时间应答
}

void StaticCheck::ProcessAnswer(bool answered)
{

//    qDebug()<<"Process Answer.";
    auto lastCheckedDot=m_lastCheckDotRecord.takeFirst();
    auto lastCheckedDotType=m_lastCheckeDotType.takeFirst();
//    qDebug()<<(answered?"anwered":"not answered");
    switch (lastCheckedDotType)
    {
    case LastCheckedDotType::locateBlindDot:
    {
        if(!answered)
        {
            if(m_resultModel->m_OS_OD==0) m_blindDot.push_back(UtilitySvc::getSingleton()->m_left_blindDot[m_blindDotLocateIndex]);
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
        m_resultModel->m_data.fixationLostTestCount++;
        if(answered)
            m_resultModel->m_data.fixationLostCount++;
        break;
    }
    case LastCheckedDotType::falsePositiveTest:
    {
        m_resultModel->m_data.falsePositiveTestCount++;
        if(!answered)
            m_resultModel->m_data.falsePositiveCount++;
        break;
    }
    case LastCheckedDotType::falseNegativeTest:
    {
        m_resultModel->m_data.falseNegativeTestCount++;
        if(answered)
        {
            m_resultModel->m_data.falseNegativeCount++;
        }
        break;
    }
    case LastCheckedDotType::commonCheckDot:
    {
        int dotDB=lastCheckedDot->StimulationDBs.last();
//        m_resultModel->m_data.realTimeDB[lastCheckedDot->index]=lastCheckedDot->StimulationDBs.toStdVector(); //在check初始化的时候扩充了大小.
        answered?lastCheckedDot->lowerBound=dotDB:lastCheckedDot->upperBound=dotDB;
//        qDebug()<<"upper:"+QString::number(lastCheckedDot->upperBound);
//        qDebug()<<"lower:"+QString::number(lastCheckedDot->lowerBound);
        int boundDistance=lastCheckedDot->upperBound-lastCheckedDot->lowerBound;
        switch (m_programModel->m_params.commonParams.strategy)
        {
//        case StaticParams::CommonParams::Strategy::smartInteractive:
        case StaticParams::CommonParams::Strategy::fullThreshold:
        {
            if(answered&&(dotDB==MaxDB))
            {
                lastCheckedDot->DB=52;
                lastCheckedDot->checked=true;
            }
            else if(!answered&&(dotDB==MinDB))
            {
                lastCheckedDot->DB=-1;
                lastCheckedDot->checked=true;
            }
            else if(boundDistance>4)
            {
                answered?lastCheckedDot->StimulationDBs.push_back(qMin(lastCheckedDot->lowerBound+4,MaxDB)):lastCheckedDot->StimulationDBs.push_back(qMax(lastCheckedDot->upperBound-4,MinDB));
            }
            else if(boundDistance<=4&&boundDistance>2)
            {
                lastCheckedDot->StimulationDBs.push_back(lastCheckedDot->lowerBound+2);
            }
            else if(boundDistance<=2)
            {
                lastCheckedDot->DB=lastCheckedDot->lowerBound;
                lastCheckedDot->checked=true;
            }
            break;
        }
        case StaticParams::CommonParams::Strategy::fastThreshold:
        {
            if(answered&&(dotDB==MaxDB))
            {
                lastCheckedDot->DB=52;
                lastCheckedDot->checked=true;
            }
            else if(!answered&&(dotDB==MinDB))
            {
                lastCheckedDot->DB=-1;
                lastCheckedDot->checked=true;
            }
            else if(boundDistance>6)
            {
                answered?lastCheckedDot->StimulationDBs.push_back(qMin(lastCheckedDot->lowerBound+6,MaxDB)):lastCheckedDot->StimulationDBs.push_back(qMax(lastCheckedDot->upperBound-6,MinDB));
            }
            else if(boundDistance<=6&&boundDistance>3)
            {
                lastCheckedDot->StimulationDBs.push_back(lastCheckedDot->lowerBound+3);
            }
            else if(boundDistance<=3)
            {
                lastCheckedDot->DB=lastCheckedDot->lowerBound+1;
                lastCheckedDot->checked=true;
            }
            break;
        }
        case StaticParams::CommonParams::Strategy::smartInteractive:
        {
            if(answered&&(dotDB==MaxDB))
            {
                lastCheckedDot->DB=52;
                lastCheckedDot->checked=true;
            }
            else if(!answered&&(dotDB==MinDB))
            {
                lastCheckedDot->DB=-1;
                lastCheckedDot->checked=true;
            }
            else if(boundDistance>4)
            {
                answered?lastCheckedDot->StimulationDBs.push_back(qMin(lastCheckedDot->lowerBound+4,MaxDB)):lastCheckedDot->StimulationDBs.push_back(qMax(lastCheckedDot->upperBound-4,MinDB));
            }
            else if(boundDistance<=4&&boundDistance>2)
            {
                lastCheckedDot->StimulationDBs.push_back(lastCheckedDot->lowerBound+2);
            }
            else if(boundDistance==2)
            {
                if(lastCheckedDot->isBaseDot)
                    lastCheckedDot->StimulationDBs.push_back(lastCheckedDot->lowerBound+1);             //最初4点多测一次
                else
                {
                    getProperValByRefToNearDotDB(*lastCheckedDot);                                    //其它的通过算法
                    lastCheckedDot->checked=true;
                }

            }
            else if(boundDistance<=1)                        //仅最初的4点和中心点会走到这一步.
            {
                lastCheckedDot->DB=lastCheckedDot->lowerBound;
                lastCheckedDot->checked=true;
            }
            break;
        }
        case StaticParams::CommonParams::Strategy::fastInterative:
        {
            if(answered&&(dotDB==MaxDB))
            {
                lastCheckedDot->DB=52;
                lastCheckedDot->checked=true;
            }
            else if(!answered&&(dotDB==MinDB))
            {
                lastCheckedDot->DB=-1;
                lastCheckedDot->checked=true;
            }
            else if(boundDistance>3)
            {
                answered?lastCheckedDot->StimulationDBs.push_back(qMin(lastCheckedDot->lowerBound+3,MaxDB)):lastCheckedDot->StimulationDBs.push_back(qMax(lastCheckedDot->upperBound-3,MinDB));
            }
            else if(boundDistance<=3)
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
            break;
        }
        case StaticParams::CommonParams::Strategy::twoStages:
        {
            auto lastStimulatDB=lastCheckedDot->StimulationDBs.last();
            if(lastStimulatDB!=0)              //第一次.
            {
                if(answered)
                {
                    lastCheckedDot->DB=2;
                    lastCheckedDot->checked=true;
                }
                else lastCheckedDot->StimulationDBs.push_back(0);
            }
            else                            //第二次
            {
                if(answered)
                    lastCheckedDot->DB=1;
                else
                    lastCheckedDot->DB=0;
                lastCheckedDot->checked=true;
            }
            break;
        }
        case StaticParams::CommonParams::Strategy::quantifyDefects:
        {
            if(!answered&&(dotDB==MinDB))
            {
                lastCheckedDot->DB=lastCheckedDot->StimulationDBs.first()+4+1;
                lastCheckedDot->checked=true;
            }
            else if(boundDistance>3)
            {
                if(lastCheckedDot->StimulationDBs.size()==1&&answered)    //第一次
                {
                    lastCheckedDot->DB=2;
                    lastCheckedDot->checked=true;
                }
                else
//                    lastCheckedDot->StimulationDBs.push_back(lastCheckedDot->lowerBound+3);
                    answered?lastCheckedDot->StimulationDBs.push_back(qMin(lastCheckedDot->lowerBound+3,MaxDB)):lastCheckedDot->StimulationDBs.push_back(qMax(lastCheckedDot->upperBound-3,MinDB));
            }
            if(boundDistance<=3)
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
            break;
        }
        }
        if(lastCheckedDot->checked==true)
        {
            if(m_lastCheckDotRecord.last()!=nullptr&&m_lastCheckDotRecord.last()->index==lastCheckedDot->index)
            {
                m_alreadyChecked=true;
                if(m_deviceOperation->m_isDeviceReady)
                    m_deviceOperation->move5Motors(std::array<bool,5>{false,false,false,false,false}.data(),std::array<int,5>{0,0,0,0,0}.data());       //没必要跑点了,早点停止可以立即跑下一个点
                m_lastCheckDotRecord.pop_back();
                m_lastCheckeDotType.pop_back();
            }          //下次要刺激的点,是已经检查了的,所以要移除所有下次的点
            m_resultModel->m_data.checkData[lastCheckedDot->index]=lastCheckedDot->DB; //存储结果
            if(m_programModel->m_params.commonParams.fixationTarget==FixationTarget::centerPoint&&lastCheckedDot->index==int(m_programModel->m_data.dots.size()*2))                //中心点检查完后换灯
            {
                for(int i=0;i<4;i++)
                    m_deviceOperation->setLamp(DevOps::LampId::LampId_smallDiamond,i,false);
                m_deviceOperation->setLamp(DevOps::LampId::LampId_centerFixation,0,true);
            }

            if(lastCheckedDot->index<int(m_programModel->m_data.dots.size()))                //非短周期或者中心点
            {
                m_checkedCount++;
                if(m_resultModel->m_params.commonParams.shortTermFluctuation)
                {
                    m_lastShortTermCycleCheckedDotIndex.push_back(lastCheckedDot->index);         //加入最近检测点的集合
                    if(m_lastShortTermCycleCheckedDotIndex.size()==m_programModel->m_params.fixedParams.shortTermFluctuationCount)
                    {
                        auto selectedIndex=m_lastShortTermCycleCheckedDotIndex[qrand()%m_lastShortTermCycleCheckedDotIndex.size()];
                        m_lastShortTermCycleCheckedDotIndex.clear();
                        auto dotRecord=m_dotRecords[selectedIndex];
                        DotRecord dotfluc;
                        dotfluc.isBaseDot=false;
                        dotfluc.upperBound=MaxDB;
                        dotfluc.lowerBound=MinDB;
                        dotfluc.checked=false;
                        dotfluc.loc=dotRecord.loc;
                        dotfluc.DB=-1;
                        dotfluc.StimulationDBs.append(dotRecord.DB);
                        dotfluc.index=m_programModel->m_data.dots.size()+dotRecord.index;   //短周期的话编号加上程序点个数
                        m_shortTermFlucRecords.push_back(dotfluc);
                    }
                }
            }
        }
    }
    }
}

void StaticCheck::waitAndProcessAnswer()
{
    bool answerResult;
    if(m_deviceOperation->m_isDeviceReady)
         answerResult=waitForAnswer();
    else
    {
        answerResult=qrand()%100<50;
    //    answerResult=true;
        UtilitySvc::wait(100);
    }
    //    QThread::msleep(1000);
    qDebug()<<answerResult;
    ProcessAnswer(answerResult);
}

void StaticCheck::checkWaiting()
{
    bool isPaused=false;
    QElapsedTimer timer;
    timer.start();
    while(m_deviceOperation->getAnswerPadStatus())
    {
        QApplication::processEvents();
        if(timer.elapsed()>300)
        {
            std::cout<<"pausing";
            isPaused=true;
            timer.restart();
        }
    }

    if(isPaused)
    {
        UtilitySvc::wait(m_programModel->m_params.fixedParams.intervalTime);                //最小等待时间
    }
}

void StaticCheck::setLight(bool onOff)
{
    switch (m_programModel->m_params.commonParams.fixationTarget)
    {
    case FixationTarget::centerPoint:
    {
        if(onOff==true)
        {
            if(m_programModel->m_params.commonParams.centerDotCheck)
            {
                for(int i=0;i<4;i++)
                    m_deviceOperation->setLamp(DevOps::LampId::LampId_smallDiamond,i,true);
            }
            else
                m_deviceOperation->setLamp(DevOps::LampId::LampId_centerFixation,0,true);
        }
        else
        {
            if(m_programModel->m_params.commonParams.centerDotCheck)
            {
                for(int i=0;i<4;i++)
                    m_deviceOperation->setLamp(DevOps::LampId::LampId_smallDiamond,i,false);
            }
            m_deviceOperation->setLamp(DevOps::LampId::LampId_centerFixation,0,false);
        }
        break;
    }
    case FixationTarget::bigDiamond:
    {
        for(int i=0;i<4;i++)
            m_deviceOperation->setLamp(DevOps::LampId::LampId_bigDiamond,i,onOff); break;
        break;
    }
    case FixationTarget::smallDiamond:
    {
        for(int i=0;i<4;i++)
            m_deviceOperation->setLamp(DevOps::LampId::LampId_smallDiamond,i,onOff); break;
        break;
    }
    case FixationTarget::bottomPoint:
    {
        m_deviceOperation->setLamp(DevOps::LampId::LampId_bigDiamond,1,onOff); break;
        break;
    }
    }

    if(m_programModel->m_params.commonParams.backGroundColor==BackGroundColor::white)
    {
        m_deviceOperation->setWhiteLamp(onOff);
    }
    else
        m_deviceOperation->setLamp(DevOps::LampId::LampId_yellowBackground,0,onOff);
}


void DynamicCheck::initialize()
{
    m_resultModel->m_patient_id=m_patientModel->m_id;
    m_resultModel->m_program_id=m_programModel->m_id;
    qsrand(QTime::currentTime().msec());
    m_checkedCount=0;
    auto& os_od=m_resultModel->m_OS_OD;
    auto& params=m_programModel->m_params;
    m_speedLevel=params.speed;
    m_cursorSize=(int)params.cursorSize;
    switch(params.strategy)
    {
    case DynamicParams::Strategy::standard:
    {
        m_totalCount=m_programModel->m_data.dots.size();
        m_records.resize(m_totalCount);
        m_resultModel->m_data.checkData.resize(m_totalCount);
        for(int i=0;i<m_totalCount;i++)
        {
            auto& dot=m_programModel->m_data.dots[i];
            m_records[i].index=i;
            if(os_od!=0){dot.x=-dot.x;}
            m_records[i].beginLoc={dot.x,dot.y};
            m_records[i].endLoc={0,0};
            m_records[i].checked=false;
            m_records[i].isAnswered=false;

            auto& checkData=m_resultModel->m_data.checkData[i];
            checkData.start=dot;
            checkData.isSeen=false;
            checkData.isChecked=false;
        }
        break;
    }
    case DynamicParams::Strategy::blindArea:
    case DynamicParams::Strategy::darkArea:
    {
        switch (params.dynamicMethod)
        {
            case DynamicParams::DynamicMethod::_4Lines:m_totalCount=4;break;
            case DynamicParams::DynamicMethod::_6Lines:m_totalCount=6;break;
            case DynamicParams::DynamicMethod::_8Lines:m_totalCount=8;break;
        }
        m_records.resize(m_totalCount);
        int distance;
        switch (params.dynamicDistance)
        {
            case DynamicParams::DynamicDistance::_5:distance=5;break;
            case DynamicParams::DynamicDistance::_10:distance=10;break;
            case DynamicParams::DynamicDistance::_15:distance=15;break;
        }
        auto centerDot=m_dynamicSelectedDots[0];
//        if(os_od!=0){centerDot.rx()=-centerDot.rx();}

        m_resultModel->m_data.checkData.resize(m_totalCount);
        for(int i=0;i<m_totalCount;i++)
        {
            auto angle=2*M_PI*i/m_totalCount;
            m_records[i].beginLoc={centerDot.x()+distance*qCos(angle),centerDot.y()+distance*qSin(angle)};
            m_records[i].endLoc=centerDot;
            m_records[i].index=i;
            m_records[i].checked=false;
            m_records[i].isAnswered=false;

            auto& checkData=m_resultModel->m_data.checkData[i];
            checkData.start=m_records[i].beginLoc;
            checkData.isSeen=false;
            checkData.isChecked=false;
        }
        break;
    }
    case DynamicParams::Strategy::straightLine:
    {
        m_totalCount=1;
        m_resultModel->m_data.checkData.resize(2);
        auto beginDot=m_dynamicSelectedDots[0];
        auto endDot=m_dynamicSelectedDots[1];
//        if(os_od!=0)
//        {
//            beginDot.rx()=-beginDot.rx();
//            endDot.rx()=-endDot.rx();
//        }
        m_records.push_back(PathRecord{0,beginDot,endDot,{0,0},false,false});
        break;
    }
    }


    setLight(true);
    m_deviceOperation->lightUpCastLight();
    m_deviceOperation->m_isChecking=true;
    auto cursorSize=m_programModel->m_params.cursorSize;
    auto cursorColor=m_programModel->m_params.cursorColor;
//    qDebug()<<m_programModel->m_params.brightness;
    if(m_deviceOperation->m_isDeviceReady)
    {
        m_deviceOperation->setCursorColorAndCursorSize(int(cursorColor),int(cursorSize));
        m_deviceOperation->setDB(m_programModel->m_params.brightness);
    }
}

void DynamicCheck::Checkprocess()
{
    auto& record=m_records[getPathRecordIndex()];
    stimulate(record.beginLoc,record.endLoc);
    auto answerLoc=waitForAnswer();
    ProcessAnswer(answerLoc,record);
}

int DynamicCheck::getPathRecordIndex()
{
    QVector<int> indexes;
    for(auto&i:m_records)
    {
        if(!i.checked) indexes.push_back(i.index);
    }
    return indexes[qrand()%indexes.length()];
}

void DynamicCheck::stimulate(QPointF begin, QPointF end)
{
    static bool isMainTable=true;
    isMainTable=UtilitySvc::getIsMainTable(begin,isMainTable);
    m_deviceOperation->dynamicStimulate(begin,end,m_cursorSize,m_speedLevel,isMainTable);
}

QVector<QPointF> DynamicCheck::waitForAnswer()
{
    static int watiForAnswerCount=0;
    if(!m_deviceOperation->getSingleton()->getIsDeviceReady())
    {
        UtilitySvc::wait(50);  //刷新下状态
        UtilitySvc::wait(1000);  //虚拟测试表示耗费时间
        QVector<QPointF> answerLocs;
        if(m_programModel->m_params.strategy==DynamicParams::Strategy::straightLine)
        {
            int outCome=qrand()%10;
            if(outCome==0){}
            else if(outCome==1||outCome==2)
            {
                answerLocs.push_back(m_deviceOperation->getDyanmicAnswerPos());
            }
            else
            {
                answerLocs.push_back(m_deviceOperation->getDyanmicAnswerPos());
                answerLocs.push_back(m_deviceOperation->getDyanmicAnswerPos());
            }
        }
        else
        {
            int outCome=qrand()%20;
            if(outCome>1)
                answerLocs.push_back(m_deviceOperation->getDyanmicAnswerPos());
        }
        m_checkedCount++;
        return answerLocs;
    }
    while(!m_deviceOperation->getDynamicMoveStatus())
        QApplication::processEvents();          //等待刷新状态
    QVector<QPointF> answerLocs;
    if(m_programModel->m_params.strategy==DynamicParams::Strategy::straightLine)
    {
        while(m_deviceOperation->getDynamicMoveStatus())
        {
            if(m_deviceOperation->getAnswerPadStatus())
            {
                auto answerLoc=m_deviceOperation->getDyanmicAnswerPos();
                answerLocs.push_back(answerLoc);
                goto Wait;
            }
            QApplication::processEvents();
        }
        goto Exit;
        Wait:
        while(m_deviceOperation->getDynamicMoveStatus()) //等待松开
        {
            if(m_deviceOperation->getAnswerPadStatus())
            {
                QApplication::processEvents();
            }
            else
            {
                goto End;
            }
        }
        goto Exit;
        End:
        while(m_deviceOperation->getDynamicMoveStatus())
        {
            if(m_deviceOperation->getAnswerPadStatus())
            {
                auto answerLoc=m_deviceOperation->getDyanmicAnswerPos();
                answerLocs.push_back(answerLoc);
//                m_deviceOperation->stopDynamic();
                goto Exit;
            }
            QApplication::processEvents();
        }
        goto Exit;
    }
    else
    {
        while(true)
        {
            while(m_deviceOperation->getDynamicMoveStatus())
            {
                if(m_deviceOperation->getAnswerPadStatus())
                {
                    auto answerLoc=m_deviceOperation->getDyanmicAnswerPos();
                    answerLocs.push_back(answerLoc);
//                    m_deviceOperation->stopDynamic();
                    goto Exit;
                }
                QApplication::processEvents();
            }
//            qDebug()<<"dynamicMove stopped";
            QElapsedTimer elapsedTimer;
            elapsedTimer.start();
            while(!m_deviceOperation->getDynamicMoveStatus())
            {
                if(elapsedTimer.elapsed()>200)      //真的停止移动
                {
                    watiForAnswerCount++;
//                    qDebug()<<"dynamicMove really stopped:"+QString::number(watiForAnswerCount);
                    goto Exit;
                }
                QApplication::processEvents();
            }
        }
    }
    Exit:
    m_deviceOperation->openShutter(0);
    m_deviceOperation->waitMotorStop({UsbDev::DevCtl::MotorId_Shutter});
    if(m_deviceOperation->getDynamicMoveStatus())
        m_deviceOperation->stopDynamic();
    m_checkedCount++;
    return answerLocs;
}

void DynamicCheck::ProcessAnswer(QVector<QPointF> answerLoc,PathRecord& record)
{
    auto& nodeList=m_resultModel->m_data.checkData;
    if(m_programModel->m_params.strategy==DynamicParams::Strategy::straightLine)
    {
        DynamicDataNode startNode;
        DynamicDataNode endNode;
        startNode.name="start";
        endNode.name="end";
        startNode.start=m_records[0].beginLoc;
        endNode.start=m_records[0].beginLoc;
        if(answerLoc.length()==0)
        {
            startNode.isSeen=false;
            endNode.isSeen=false;
        }
        else if(answerLoc.length()==1)
        {
            startNode.isSeen=true;
            startNode.end=answerLoc[0];
            endNode.isSeen=false;
        }
        else if(answerLoc.length()==2)
        {
            startNode.isSeen=true;
            endNode.isSeen=true;
            startNode.end=answerLoc[0];
            endNode.end=answerLoc[1];
        }
        nodeList[0]=startNode;
        nodeList[1]=endNode;
//        qDebug()<<nodeList[0].end.toQPointF();
//        qDebug()<<nodeList[1].end.toQPointF();
    }
    else
    {
        DynamicDataNode node;
        if(answerLoc.length()==0)
        {
            record.isAnswered=false;
            record.answeredLoc=record.endLoc;
        }
        else if(answerLoc.length()==1)
        {
            record.isAnswered=true;
            record.answeredLoc=answerLoc[0];
        }
        record.checked=true;
        node.name=UtilitySvc::getDynamicDotEnglishName(m_checkedCount-1).toStdString();
        node.start=record.beginLoc;
        node.end=record.answeredLoc;
        node.isSeen=record.isAnswered;
        node.isChecked=true;
        nodeList[record.index]=node;
//        qDebug()<<m_resultModel->m_data.checkData.size();
//        for(auto&i:m_resultModel->m_data.checkData)
//        {
//            qDebug()<<i.start.toQPointF();
//            qDebug()<<i.end.toQPointF();
//        }

    }
}




void DynamicCheck::finished()
{
    m_deviceOperation->m_isChecking=false;
    setLight(false);
    m_deviceOperation->setLamp(DevOps::LampId::LampId_eyeglassInfrared,0,false);
    m_deviceOperation->setLamp(DevOps::LampId::LampId_borderInfrared,0,false);
    m_deviceOperation->dimDownCastLight();
}

void DynamicCheck::setLight(bool onOff)
{
    m_deviceOperation->setLamp(DevOps::LampId::LampId_centerFixation,0,onOff);
    if(m_programModel->m_params.backGroundColor==BackGroundColor::white)
    {
        m_deviceOperation->setWhiteLamp(onOff);
    }
    else
        m_deviceOperation->setLamp(DevOps::LampId::LampId_yellowBackground,0,onOff);

}

void CheckSvcWorker::initialize()
{
//    qDebug()<<("initializing");
    int type=m_programVm->getType();
//    qDebug()<<type;
    if(type!=2)
    {
        m_check.reset(new StaticCheck());
        m_check->m_checkState=m_checkState;
        m_check->m_patientModel=m_patientVm->getModel();
        ((StaticCheck*)m_check.data())->m_resultModel=static_cast<StaticCheckResultVm*>(m_checkResultVm)->getModel();
//        qDebug()<<QString::number((((StaticCheck*)m_check.data())->m_resultModel)->m_id);
        ((StaticCheck*)m_check.data())->m_programModel=static_cast<StaticProgramVm*>(m_programVm)->getModel();
        connect(((StaticCheck*)m_check.data()),&StaticCheck::currentCheckingDotChanged,this,&CheckSvcWorker::currentCheckingDotChanged);
    }
    else
    {
        m_check.reset(new DynamicCheck());
        m_check->m_checkState=m_checkState;
        m_check->m_patientModel=m_patientVm->getModel();
        ((DynamicCheck*)m_check.data())->m_resultModel=static_cast<DynamicCheckResultVm*>(m_checkResultVm)->getModel();
        ((DynamicCheck*)m_check.data())->m_programModel=static_cast<DynamicProgramVm*>(m_programVm)->getModel();
        ((DynamicCheck*)m_check.data())->m_dynamicSelectedDots=m_dynamicSelectedDots;
        connect(this,&CheckSvcWorker::stop,[&](){m_check->m_deviceOperation->stopDynamic();});
    }
}


//void CheckSvcWorker::doWork()
//{
//    initialize();
//    m_check->initialize();
//}

void CheckSvcWorker::doWork()
{
    *m_checkState=0;
    m_time=0;
    while(true)
    {
        switch (*m_checkState)
        {
        case 0:                                             //start
        {
            initialize();
            m_check->initialize();
//            qDebug()<<m_check->m_totalCount;
            emit totalCountChanged(m_check->m_totalCount);
            emit checkedCountChanged(0);
            emit checkResultChanged();
            m_timer.start();
            if(*m_checkState==0)                              //防止其它主线程选择退出,之后被覆盖
                setCheckState(1);
            break;
        }
        case 1:                                             //check
        {
//            qDebug()<<("Checking");
            m_check->Checkprocess();
            if(m_check->m_error)
            {
                setCheckState(3);
                emit sendErrorInfo(m_check->m_errorInfo);
            }
            if(*m_checkState!=3)                //有可能外部结束后,后面把它覆盖为完成,就剩1个点的时候出现此情况.
            {
                if(m_check->m_checkedCount==m_check->m_totalCount) setCheckState(4);
            }
            emit checkedCountChanged(m_check->m_checkedCount);
            emit checkResultChanged();
            QApplication::processEvents();
            break;
        }
        case 2:                                             //pause
        {
//            qDebug()<<("pausing");
            UtilitySvc::wait(500);
            break;
        }
        case 3:                                             //stop
        {
//            qDebug()<<("stopped");
            m_timer.stop();
            m_check->finished();
            return;
        }
        case 4:                                             //finish
        {
            int type=m_programVm->getType();
            m_check->finished();
            m_timer.stop();
            if(type!=2)
            {
                static_cast<StaticCheckResultVm*>(m_checkResultVm)->getResultData()->setTestTimespan(m_time);
            }
            else
            {
                static_cast<DynamicCheckResultVm*>(m_checkResultVm)->getResultData()->setTestTimespan(m_time);
            }
            m_checkResultVm->insert();
            qDebug()<<("finished");
            emit checkProcessFinished();
            return;
        }
        };
    }
}


CheckSvc::CheckSvc(QObject *parent)
{
    m_workerThread.start();
    m_worker = new CheckSvcWorker();
    m_worker->moveToThread(&m_workerThread);
    m_worker->m_timer.moveToThread(&m_workerThread);
    DevOps::DeviceOperation::getSingleton()->moveToThread(&m_workerThread);
    connect(m_worker,&CheckSvcWorker::checkResultChanged,this, &CheckSvc::checkResultChanged);
    connect(m_worker,&CheckSvcWorker::checkStateChanged,this, &CheckSvc::checkStateChanged);
    connect(m_worker,&CheckSvcWorker::checkedCountChanged,this, &CheckSvc::setCheckedCount);
    connect(m_worker,&CheckSvcWorker::totalCountChanged,this, &CheckSvc::setTotalCount);
    connect(m_worker,&CheckSvcWorker::checkTimeChanged,this, &CheckSvc::setCheckTime);
    connect(m_worker,&CheckSvcWorker::currentCheckingDotChanged,[&](int value){m_currentCheckingDotIndex=value;emit currentCheckingDotIndexChanged();});
    connect(m_worker,&CheckSvcWorker::sendErrorInfo,this, [](QString errorInfo)
    {
        QMessageBox msgBox;
        msgBox.setText(errorInfo);
        msgBox.exec();
    });
    connect(DevOps::DeviceOperation::getSingleton().data(),&DevOps::DeviceOperation::isDeviceReadyChanged,this,&CheckSvc::devReadyChanged);
    connect(DevOps::DeviceOperation::getSingleton().data(),&DevOps::DeviceOperation::castLightAdjustStatusChanged,this,&CheckSvc::castLightAdjustStatusChanged);
    connect(DevOps::DeviceOperation::getSingleton().data(),&DevOps::DeviceOperation::pupilDiameterChanged,this,&CheckSvc::pupilDiameterChanged);
//    connectDev();
}

CheckSvc::~CheckSvc()
{
//    m_workerThread.quit();
//    disconnectDev();
    m_workerThread.terminate();
    m_worker->deleteLater();
}

void CheckSvc::start()
{
//    if(m_checkResultVm==nullptr)
//    {
//        qDebug()<<QString::number(m_checkResultVm->getPatient_id());
//    }
    m_worker->m_patientVm=m_patientVm;
    m_worker->m_programVm=m_programVm;
    m_worker->m_checkResultVm=m_checkResultVm;
    m_worker->m_checkState=&m_checkState;
    if(m_programVm->getType()==2) m_worker->m_dynamicSelectedDots=m_dynamicSelectedDots;
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
    emit m_worker->stop();
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

int CheckSvc::getCastLightAdjustStatus()
{
    return DevOps::DeviceOperation::getSingleton()->getCastLightAdjustStatus();
}

bool CheckSvc::getAutoAlignPupil()
{
    return DevOps::DeviceOperation::getSingleton()->getAutoAlignPupil();
}

void CheckSvc::setAutoAlignPupil(bool autoAlign)
{
    DevOps::DeviceOperation::getSingleton()->setAutoAlignPupil(autoAlign);
    emit autoAlignPupilChanged();
}

float CheckSvc::getPupilDiameter()
{
    return DevOps::DeviceOperation::getSingleton()->getPupilDiameter();
}

void Perimeter::CheckSvc::setInputDots(QVariantList value)
{
    m_dynamicSelectedDots.clear();
    for(auto&i:value)
    {
        auto radius=i.toMap()["x"].toInt();
        auto angle=i.toMap()["y"].toInt();
        m_dynamicSelectedDots.push_back({radius,angle});
    }
}
}
#include "check_svc.moc"

