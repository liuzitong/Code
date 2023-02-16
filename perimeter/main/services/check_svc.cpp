#include "check_svc.h"
#include <QApplication>
#include <QThread>
#include <QTimer>
#include <QElapsedTimer>
#include <perimeter/main/services/utility_svc.h>
#include "device_svc.h"
#include <QtMath>

namespace Perimeter{
//enum Step
//{
//    AdjustColorAndCursorSize,
//    CenterDotCheck,
//    CommonCheck
//};

constexpr int MaxDB=52;
constexpr int MinDB=0;
class Check:public QObject
{
    Q_OBJECT
public:
    Check()=default;
    ~Check()=default;
    QSharedPointer<PatientModel> m_patientModel;
    int m_checkedCount,m_totalCount;
    static constexpr int y_offsetDiamond=-8,y_offsetBottomPoint=-12;
    virtual void Checkprocess()=0;
    virtual void initialize()=0;
signals:
    void checkedCountChanged(int count);
};

class StaticCheck:public Check
{
private:

    struct DotRecord
    {
        int index;
        QPointF loc;
        QVector<int> StimulationDBs;                     //每次亮灯的DB
        int DB;                                          //最后得到的DB;
        QVector<QByteArray> realTimeEyePosPic;           //实时眼位
//        DBChangeDirection direction;
//        bool reversed;
        bool isBaseDot;
        bool checked;
        bool hasShortTermFluc;
        int lowerBound;
        int upperBound;
    };

    QVector<int> m_value_30d;
    QVector<int> m_value_60d;
    int m_y_offset;
    QElapsedTimer m_elapsedTimer;
    QVector<DotRecord> m_dotRecords;
    bool m_isStartWithBaseDots;
    bool m_isDoingBaseDotsCheck;
    DotRecord m_centerDotRecord;
    QVector<int> answeredTimes;    //根据此处得到autoAdaptTime;
    int autoAdaptTime;
    QVector<DotRecord> m_shortTermFlucRecords;
    QSharedPointer<UtilitySvc> utilitySvc=UtilitySvc::getSingleton();
    QSharedPointer<DeviceSvc> deviceSvc=DeviceSvc::getSingleton();
public:
    QSharedPointer<StaticCheckResultModel> m_resultModel;
    QSharedPointer<StaticProgramModel> m_programModel;
    StaticCheck()=default;
    ~StaticCheck()=default;
    virtual void initialize() override
    {
        m_checkedCount=0;
        autoAdaptTime=0;
        m_resultModel->m_patient_id=m_patientModel->m_id;
        m_resultModel->m_program_id=m_programModel->m_id;
        m_totalCount=m_programModel->m_data.dots.size();
        m_resultModel->m_data.checkData=std::vector<int>(m_totalCount,-1);
        auto cursorSize=m_programModel->m_params.commonParams.cursorSize;
        auto cursorColor=m_programModel->m_params.commonParams.cursorColor;
        m_value_30d=utilitySvc->getValue30d(int(cursorSize),int(cursorColor),m_patientModel->m_age);
        m_value_60d=utilitySvc->m_value_60d;
        switch (m_programModel->m_params.commonParams.fixationTarget)
        {
        case FixationTarget::centerPoint:m_y_offset=0;break;
        case FixationTarget::bigDiamond:
        case FixationTarget::smallDiamond:m_y_offset=-8;break;
        case FixationTarget::bottomPoint:m_y_offset=-12;break;
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
                    {
                        if((qAbs(dot.x-baseDots[j].x)<FLT_EPSILON)&&(qAbs(dot.y-baseDots[j].y)<FLT_EPSILON))
                        {
                            isBaseDot=true;
                        }
                    }
                }
                m_dotRecords.push_back(
                            DotRecord{i,QPointF{dot.x,dot.y},{utilitySvc->getExpectedDB(m_value_30d,{dot.x,dot.y},m_resultModel->m_OS_OD)+DBChanged},
                                      -1, {},isBaseDot,false,false,MinDB,MaxDB});


            }
            m_centerDotRecord=DotRecord{0,QPointF{0,0},{utilitySvc->getExpectedDB(m_value_30d,{0,0},m_resultModel->m_OS_OD)+DBChanged},-1,{},false,false,MinDB,MaxDB};
        }
        else                            //单刺激
        {
            auto DB=m_programModel->m_params.fixedParams.singleStimulationDB;
            for(int i=0;i<m_totalCount;i++)
            {
                auto dot=m_programModel->m_data.dots[i];
                m_dotRecords.push_back(
                            DotRecord{i,QPointF{dot.x,dot.y},{DB},
                                      -1, {},false,false,false,MinDB,MaxDB});

            }
            m_centerDotRecord=
                    DotRecord{0,QPointF{0,0},{DB},-1,{},false,false,false,MinDB,MaxDB};
        }

    }
    virtual void Checkprocess() override
    {

        if(m_programModel->m_params.commonParams.centerDotCheck&&!m_centerDotRecord.checked)
        {
            //Check center dot;
            int y_offset;
            m_programModel->m_params.commonParams.fixationTarget==FixationTarget::centerPoint?y_offset=y_offsetDiamond:y_offset=m_y_offset;
            CheckDot(m_centerDotRecord,y_offset,true);
        }

        if(m_isStartWithBaseDots&&m_isDoingBaseDotsCheck)
        {
            QVector<int> baseDotIndexes;
            for(int i=0;i<m_dotRecords.size();i++)
            {
                if(!m_dotRecords[i].checked&&m_dotRecords[i].isBaseDot)
                {
                    baseDotIndexes.push_back(i);
                }
            }
            if(baseDotIndexes.size()!=0)
            {
                auto index = baseDotIndexes[qrand()%baseDotIndexes.size()];
                CheckDot(m_dotRecords[index],m_y_offset,true);
            }
            else
            {
                m_isDoingBaseDotsCheck=false;
            }
        }

        CheckDot(m_dotRecords[GetCheckDotIndex()],m_y_offset,false);

    }
private:
    int GetCheckDotIndex()
    {
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
            auto selectedDot=nearestRecords[qrand()%nearestRecords.count()];
            return selectedDot.index;
        }
        else
        {
            QVector<int> unchekedDotIndex;
            for(auto&i:m_dotRecords)
            {
                if(!i.checked) unchekedDotIndex.push_back(i.index);
            }
            return unchekedDotIndex[qrand()%unchekedDotIndex.count()];
        }

    }

    void getProperValByRefToNearDotDB(DotRecord& dotRecord)
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

    void CheckDot(DotRecord& dotRecord,int y_offset,bool isInitialDot)
    {
        deviceSvc->stimulate(dotRecord.StimulationDBs.last(),{dotRecord.loc.x(),dotRecord.loc.y()+y_offset},m_programModel->m_params.fixedParams.stimulationTime);
        m_resultModel->m_data.realTimeDB[dotRecord.index].push_back(dotRecord.StimulationDBs.last());          //实时刺激的DB.
        m_resultModel->m_blob.append(deviceSvc->getRealTimeStimulationEyeImage());
        bool isAnswered=deviceSvc->waitForAnswer(m_programModel->m_params.fixedParams.intervalTime);
        int dotDB=dotRecord.StimulationDBs.last();
        isAnswered?dotRecord.lowerBound=dotDB:dotRecord.upperBound=dotDB;
        int boundDistance=dotRecord.upperBound-dotRecord.lowerBound;
        switch (m_programModel->m_params.commonParams.strategy)
        {
        case StaticParams::CommonParams::Strategy::fullThreshold:
        {
            if(boundDistance>4)
            {
                isAnswered?dotRecord.StimulationDBs.push_back(qMin((dotRecord.lowerBound+4),MaxDB)):dotRecord.StimulationDBs.push_back(qMax(dotRecord.upperBound-4,MinDB));
            }
            if(boundDistance==4)
            {
                dotRecord.StimulationDBs.push_back(dotRecord.lowerBound+2);
            }
            if(boundDistance==2)
            {
                dotRecord.DB=dotRecord.lowerBound;
                dotRecord.checked=true;
            }
            break;
        }
        case StaticParams::CommonParams::Strategy::fastThreshold:
        {
            if(boundDistance>6)
            {
                isAnswered?dotRecord.StimulationDBs.push_back(dotRecord.lowerBound+6):dotRecord.StimulationDBs.push_back(dotRecord.upperBound-6);
            }
            if(boundDistance==6)
            {
                dotRecord.StimulationDBs.push_back(dotRecord.lowerBound+3);
            }
            if(boundDistance==3)
            {
                dotRecord.DB=dotRecord.lowerBound+1;
                dotRecord.checked=true;
            }
            break;
        }
        case StaticParams::CommonParams::Strategy::smartInteractive:
        {
            if(boundDistance>4)
            {
                isAnswered?dotRecord.StimulationDBs.push_back(dotRecord.lowerBound+4):dotRecord.StimulationDBs.push_back(dotRecord.upperBound-4);
            }
            if(boundDistance==4)
            {
                dotRecord.StimulationDBs.push_back(dotRecord.lowerBound+2);
            }
            if(boundDistance==2)
            {
                if(isInitialDot)
                    dotRecord.StimulationDBs.push_back(dotRecord.lowerBound+1);
                else{
                    getProperValByRefToNearDotDB(dotRecord);
                    dotRecord.checked=true;
                }
            }
            if(boundDistance==1)                        //仅最初的4点和中心点会走到这一步.
            {
                if(isAnswered)
                {
                    dotRecord.StimulationDBs.push_back(dotRecord.lowerBound);
                    dotRecord.checked=true;
                }
            }
            break;
        }
        case StaticParams::CommonParams::Strategy::fastInterative:
        {
            if(boundDistance>3)
            {
                isAnswered?dotRecord.StimulationDBs.push_back(dotRecord.lowerBound+3):dotRecord.StimulationDBs.push_back(dotRecord.upperBound-3);
            }
            if(boundDistance==3)
            {
                dotRecord.DB=dotRecord.lowerBound+1;
                dotRecord.checked=true;
            }
            break;
        }
        case StaticParams::CommonParams::Strategy::oneStage:
        {
            isAnswered?dotRecord.DB=2:dotRecord.DB=0;
            dotRecord.checked=true;
        }
        case StaticParams::CommonParams::Strategy::twoStages:
        {
            auto lastStimulatDB=dotRecord.StimulationDBs.last();
            if(isAnswered)
            {
                if(lastStimulatDB!=0)
                {
                    dotRecord.DB=2;
                }
                else{
                    dotRecord.DB=1;
                }
            }
            else
            {
                if(lastStimulatDB!=0)
                {
                    dotRecord.StimulationDBs.push_back(0);
                }
                else{
                    dotRecord.DB=0;
                }
            }
        }
        case StaticParams::CommonParams::Strategy::quantifyDefects:
        {

            if(boundDistance>3)
            {
                if(isAnswered)
                {
                    if(dotRecord.StimulationDBs.size()==1)
                        dotRecord.DB=2;
                }
                else
                    dotRecord.StimulationDBs.push_back(dotRecord.upperBound-3);
            }
            if(boundDistance==3)
            {
                dotRecord.DB=dotRecord.StimulationDBs.first()+4/*得到标准值*/-(dotRecord.lowerBound+1);      //缺陷深度的值,4起步所以不与看到(2)的整数值冲突.
                dotRecord.checked=true;
            }
            break;
        }
        case StaticParams::CommonParams::Strategy::singleStimulation:
        {
            isAnswered?dotRecord.DB=2:dotRecord.DB=0;
            dotRecord.checked=true;
        }
        }
        if(dotRecord.checked==true)
        {
            m_checkedCount++;
            m_resultModel->m_data.checkData[dotRecord.index]=dotRecord.DB;
        }


    }

};

class DynamicCheck:public Check
{

public:
    QSharedPointer<DynamicCheckResultModel> m_resultModel;
    QSharedPointer<DynamicProgramModel> m_programModel;
    DynamicCheck()=default;
    ~DynamicCheck()=default;
private:
    QSharedPointer<UtilitySvc> utilitySvc=UtilitySvc::getSingleton();
    QSharedPointer<DeviceSvc> deviceSvc=DeviceSvc::getSingleton();
    QVector<int> unCheckedIndex;

    virtual void initialize() override
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

    virtual void Checkprocess() override
    {
        if(m_programModel->m_params.strategy==DynamicParams::Strategy::standard)
        {
            int dotIndex=unCheckedIndex.takeAt(qrand()%unCheckedIndex.size());
            auto dataNode=m_resultModel->m_data.checkData[dotIndex];
            deviceSvc->dynamicStimulate(QPointF{dataNode.start.x,dataNode.start.y},QPointF{dataNode.end.x,dataNode.end.y},1);
            bool isSeen=deviceSvc->waitForAnswer({4,5});             //TODO 填入motorIDS
//            if(isSeen)
        }
    }
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
    void initialize()
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
            ((StaticCheck*)m_check.data())->m_programModel=static_cast<StaticProgramVm*>(m_programVm)->getModel();
        }
        else
        {
            m_check.reset(new DynamicCheck());
            m_check->m_patientModel=m_patientVm->getModel();
            ((DynamicCheck*)m_check.data())->m_resultModel=static_cast<DynamicCheckResultVm*>(m_checkResultVm)->getModel();
            ((DynamicCheck*)m_check.data())->m_programModel=static_cast<DynamicProgramVm*>(m_programVm)->getModel();
        }
        m_check->initialize();
        emit checkedCountChanged(0);
        emit checkResultChanged();
    }


    void setCheckState(int value)
    {
        *m_checkState=value;
        emit checkStateChanged();
    }
public slots:
    void doWork()
    {
        *m_checkState=0;
        while(true)
        {
//            qDebug()<<*m_checkState;
            switch (*m_checkState)
            {
            case 0:                                             //start
            {
                initialize();

//                m_check->AdjustColorAndCursorSize();
                setCheckState(1);
                break;
            }
            case 1:                                             //check
            {
                qDebug()<<("Checking");
                m_check->Checkprocess();
//                QThread::msleep(500);
//                {
//                    QThread::msleep(500);
//                    QApplication::processEvents();
//                }
//                QApplication::processEvents();
                if(m_check->m_checkedCount==m_check->m_totalCount) setCheckState(4);
                emit checkResultChanged();
                break;
            }
            case 2:                                             //pause
            {
                qDebug()<<("pausing");
//                m_elapsedTimer.restart();
//                QThread::msleep(500);
//                while(m_elapsedTimer.elapsed()<=1000)
//                {
//                    QApplication::processEvents();
//                }
//                QApplication::processEvents();
                break;
            }
            case 3:                                             //stop
            {
                qDebug()<<("stopped");
//                goto exit;
                return;
            }
            case 4:                                             //finish
            {
                /*m_checkResultVm->insertCheckResult()*/
                qDebug()<<("finished");
                emit checkProcessFinished();
                return;
//                goto exit;
            }
            };
            UtilitySvc::wait(500);
        }
//        exit:
//            qDebug()<<("work end");
    }
//    void pause()
//    {
//        setCheckState(2);/*m_timer->stop();*/
//    }

//    void resume()
//    {
//        setCheckState(1);/*m_timer->start();*/
//    }

//    void stop()
//    {
//        qDebug()<<("stop worker");
//        setCheckState(3);/*m_timer->stop();*/
//    }

signals:
    void checkStateChanged();
    void checkResultChanged();
    void checkProcessFinished();
    void checkedCountChanged(int count);
};


CheckSvc::CheckSvc(QObject *parent)
{
    m_worker = new CheckSvcWorker();
    m_worker->moveToThread(&m_workerThread);
//    connect(&m_workerThread, &QThread::finished, m_worker, &QObject::deleteLater);
//    connect(&m_workerThread, &QThread::started, m_worker, &CheckSvcWorker::doWork);
    connect(m_worker,&CheckSvcWorker::checkResultChanged,this, &CheckSvc::checkResultChanged);
    connect(m_worker,&CheckSvcWorker::checkStateChanged,this, &CheckSvc::checkStateChanged);
    connect(m_worker,&CheckSvcWorker::checkedCountChanged,this, &CheckSvc::setCheckedCount);
    connect(m_worker,&CheckSvcWorker::checkProcessFinished,this, [&](){m_checkResultVm->insert();});
    m_workerThread.start();
}

CheckSvc::~CheckSvc()
{
    m_workerThread.terminate();
    m_worker->deleteLater();
}

void CheckSvc::start()
{
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












}
#include "check_svc.moc"

