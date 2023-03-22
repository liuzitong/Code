#include "analysislobbylistvm.h"
#include <perimeter/main/database/patient.h>
#include <perimeter/main/database/program.h>
#include <perimeter/main/database/checkResult.h>
#include <perimeter/main/model/programModel.h>
#include <perimeter/main/model/checkResultModel.h>
#include <perimeter/main/services/analysis_svc.h>
#include <perimeter/main/services/translate_svc.h>
#include <perimeter/main/model/utility.h>

namespace Perimeter{
class SimpleCheckResultList:public QAbstractListModel
{
    friend class AnalysisLobbyListVm;
    enum SimpleCheckResultListRoles
    {
        checkResultId=Qt::UserRole+1,
        type,
        picName,
    };

    struct Data
    {
        int checkResultId;
        int type;
        QString picName;

    };
    Q_OBJECT
    // QAbstractItemModel interface
public:
    SimpleCheckResultList()=default;
//    SimpleCheckResultList(QVector<int> checkResultIdList,QVector<QString> picNameList):
//        m_checkResultIdList(checkResultIdList),m_picNameList(picNameList){}
    ~SimpleCheckResultList()=default;

    virtual int rowCount(const QModelIndex &parent) const override
    {
        return m_dataList.count();
    }
    virtual QVariant data(const QModelIndex &index, int role) const override
    {
//        QVector<QString> picName={"dBDiagram.bmp","defectDepth.bmp","gray.bmp","PatterDeviation.bmp"};
        switch (role)
        {
            case SimpleCheckResultListRoles::checkResultId:return m_dataList[index.row()].checkResultId;
            case SimpleCheckResultListRoles::picName:return m_dataList[index.row()].picName;
            case SimpleCheckResultListRoles::type:return m_dataList[index.row()].type;
        }
        return QVariant();
    }
    virtual QHash<int, QByteArray> roleNames() const override
    {
        QHash<int, QByteArray> roles;
        roles[checkResultId] = "checkResultId";
        roles[type]="type";
        roles[picName] = "picName";
        return roles;
    }
public:
    QVector<Data> m_dataList;

};





AnalysisLobbyListVm::AnalysisLobbyListVm(const QVariantList &args)
{
//    initialize();
    int id=args[0].toInt();
    m_imageHeight=args[1].toFloat();
    Patient_ptr patient_ptr(new Patient());patient_ptr->m_id=id;
    qx::dao::fetch_by_id(patient_ptr);
//    auto birthDate=patient_ptr->m_birthDate;
//    auto currentDate=QDateTime::currentDateTime().date();
//    m_patientAge = currentDate.year()- birthDate.year();
//    if (currentDate.month() < birthDate.month() || (currentDate.month() ==birthDate.month() && currentDate.day() < birthDate.day()))
//        { m_patientAge--;}

    qx_query query;
    query.where("patient_id").isEqualTo(id);
    CheckResult_List checkResult_list;
    qx::dao::fetch_by_query(query, checkResult_list);              //不能用execute_query
    generateModelListData(checkResult_list);
}

//按照日期分组
void AnalysisLobbyListVm::generateModelListData(CheckResult_List checkResult_list)
{
    QVector<QDate> dateList;
    for(auto &i:checkResult_list)
    {
        if(!dateList.contains(i->m_time.date()))
        {dateList.append(i->m_time.date());}
    }

    std::sort(dateList.begin(),dateList.end(),[](const QDate& d1,const QDate& d2){
        return  !(d1<d2);
    });

    for(int i=0;i<dateList.length();i++ )
    {
        Data data;
        data.year=QString::number(dateList[i].year());
        data.monthDay=dateList[i].toString("MM/dd");
        SimpleCheckResultList* simpleCheckResultList=new SimpleCheckResultList();
        for(int j=0;j<checkResult_list.length();j++)
        {
            if(checkResult_list[j]->m_time.date()==dateList[i])
            {
                auto imageFileName=drawImage(checkResult_list[j]);
                simpleCheckResultList->m_dataList.append({checkResult_list[j]->m_id,checkResult_list[j]->m_type,imageFileName});
                data.simpleCheckResult=simpleCheckResultList;
            }
        }
        m_dataList.append(data);
    }
}


QString AnalysisLobbyListVm::drawImage(CheckResult_ptr checkResult_ptr)
{
    auto analysisMethodSvc=AnalysisSvc::getSingleton();
    QString os_od=checkResult_ptr->m_OS_OD==0?"OS":"OD";
    QString checkTime=checkResult_ptr->m_time.time().toString("HH:mm:ss");
    Program_ptr program_ptr(new Program());
    program_ptr->m_id=checkResult_ptr->m_program->m_id;
    qx::dao::fetch_by_id(program_ptr);
    QString programName=program_ptr->m_name;
    programName=TranslateController::getTranlatedName(programName);
    auto img=QImage(m_imageHeight*0.8,m_imageHeight*0.8,QImage::Format_ARGB32);
    auto total=QImage(m_imageHeight*0.8,m_imageHeight,QImage::Format_ARGB32);
    auto info=QImage(m_imageHeight*0.8,m_imageHeight*0.2,QImage::Format_ARGB32);
//    qDebug()<<programName;
    auto program_reports= Utility::QStringToEntity<std::vector<int>>(program_ptr->m_report);
//    for(auto&i:program_reports)
//    {
//        qDebug()<<i;
//    }
    auto checkReport=[&](int report)->bool{return std::find(program_reports.begin(),program_reports.end(),report)!=program_reports.end();};
//    qDebug()<<checkReport(0);
//    qDebug()<<checkReport(2);
    if(checkResult_ptr->m_type!=2)
    {
        auto checkResult=StaticCheckResultModel(checkResult_ptr);
        auto program=StaticProgramModel(program_ptr);
        QVector<int> values,fixationValues,dev,mDev,peDev,peMDev;
        QVector<QPointF> locs;
        float md,psd,VFI,p_md,p_psd;
        int GHT;
        int range=program.m_params.commonParams.Range[1];
        int OS_OD=checkResult.m_OS_OD;
        values.resize(checkResult.m_data.checkData.size());
        for(int i=0;i<int(checkResult.m_data.checkData.size());i++)
        {
            values[i]=checkResult.m_data.checkData[i];
        }
        locs.resize(program.m_data.dots.size());
        for(int i=0;i<int(program.m_data.dots.size());i++)
        {
            if(OS_OD==0)
                locs[i]={program.m_data.dots[i].x,program.m_data.dots[i].y};
            else
                locs[i]={-program.m_data.dots[i].x,program.m_data.dots[i].y};
        }
        int resultId=checkResult.m_id;


        if(checkResult_ptr->m_type==0)
        {

            if(checkReport(0)||checkReport(2))        //常规分析和总览显示概率图
            {
                analysisMethodSvc->ThresholdAnalysis(resultId,dev,mDev,peDev,peMDev,md,psd,VFI,GHT,p_md,p_psd);
                analysisMethodSvc->drawPE(peMDev,locs,range,img);
            }
            else if(checkReport(1)||checkReport(3))      //三合一显示量化缺损
            {
                analysisMethodSvc->ThreeInOneAnalysis(resultId,dev);
                analysisMethodSvc->drawDefectDepth(dev,locs,range,img);
            }
            else if(checkReport(4))
            {
                analysisMethodSvc->drawText(values,locs,range,OS_OD,img,{-1});
            }
        }

        if(checkResult_ptr->m_type==1)//显示筛选
        {
            analysisMethodSvc->drawScreening(values,locs,range,OS_OD,img);
        }
    }
    else                         //移动
    {
        auto checkResult=DynamicCheckResultModel(checkResult_ptr);
        auto program=DynamicProgramModel(program_ptr);
        QVector<QPointF> values;
        values.resize(checkResult.m_data.checkData.size());
        for(int i=0;i<int(checkResult.m_data.checkData.size());i++)
        {
            values[i]={checkResult.m_data.checkData[i].end.x,checkResult.m_data.checkData[i].end.y};
        }
        int range=program.m_params.Range[1];
        QVector<int> fixationValues;
        fixationValues.resize(checkResult.m_data.fixationDeviation.size());
        for(int i=0;i<int(checkResult.m_data.fixationDeviation.size());i++)
        {
            fixationValues[i]=checkResult.m_data.fixationDeviation[i];
        }
        analysisMethodSvc->drawDynamic(values,{},range,img);
    }
    info.fill(qRgb(255, 255, 255));
    QPainter painter(&info);
//    int fontPixSize=info.height()*0.7;
//    font.setPixelSize(fontPixSize);
//    font.setBold(true);
    painter.setFont({"consolas",int(info.height()*0.5),QFont::Weight::Bold});
    painter.drawText(QRect{QPoint{int(info.width()*0.03),int(info.height()*0.2)},QPoint{int(info.width()*0.22),int(info.height()*0.8)}},Qt::AlignCenter,os_od);
    painter.drawLine(QPoint{int(info.width()*0.25),int(info.height()*0.1)},QPoint{int(info.width()*0.25),int(info.height()*0.9)});
    painter.setFont({"consolas",int(info.height()*0.30)});
    painter.drawText(QRect{QPoint{int(info.width()*0.28),int(info.height()*0.10)},QPoint{int(info.width()*0.95),int(info.height()*0.5)}},Qt::AlignCenter,programName);
    painter.drawText(QRect{QPoint{int(info.width()*0.28),int(info.height()*0.5)},QPoint{int(info.width()*0.95),int(info.height()*0.90)}},Qt::AlignCenter,checkTime);


    QString imageSavePath="./analysisLobbyImage/"+QString::number(m_imageSerial)+".bmp";
    m_imageSerial++;
    total.fill(qRgb(255, 255, 255));
    QPainter painter2(&total);
    painter2.drawImage((total.width()-img.width())/2,0,img);
    painter2.drawImage(0,total.height()*0.8+1,info);
    total.save(imageSavePath);
    return imageSavePath;
}


AnalysisLobbyListVm::~AnalysisLobbyListVm()
{
    for(auto& i:m_dataList)
    {
        if(i.simpleCheckResult!=nullptr)
        {
            auto temp=static_cast<SimpleCheckResultList*>(i.simpleCheckResult);
            if(temp!=nullptr) delete temp;
        }
    }
}

void AnalysisLobbyListVm::deleteCheckResult(int id)
{

    for(int i=0;i<m_dataList.size();i++)
    {

        auto simpleCheckResultList=static_cast<SimpleCheckResultList*>(m_dataList[i].simpleCheckResult);
        auto& dataList=simpleCheckResultList->m_dataList;
        for(int j=0;j<simpleCheckResultList->m_dataList.size();j++)
        {
            if(simpleCheckResultList->m_dataList[j].checkResultId==id)
            {
                simpleCheckResultList->beginResetModel();
                dataList.remove(j);
                simpleCheckResultList->endResetModel();
                if(dataList.size()==0)
                {
                    delete m_dataList[i].simpleCheckResult;
                    beginResetModel();
                        m_dataList.remove(i);
                    endResetModel();
                }
                goto Exit;
            }
        }
    }
    Exit:
        CheckResult_ptr checkResult_ptr(new CheckResult());
        checkResult_ptr->m_id=id;
        qx::dao::delete_by_id(checkResult_ptr);


}

void AnalysisLobbyListVm::refreshData()
{

}

int AnalysisLobbyListVm::rowCount(const QModelIndex &parent) const
{
    return m_dataList.length();
}

QVariant AnalysisLobbyListVm::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
    case year:return m_dataList[index.row()].year;
    case monthDay:return m_dataList[index.row()].monthDay;
    case simpleCheckResult:return QVariant::fromValue(m_dataList[index.row()].simpleCheckResult);
    }
    return QVariant();
}

QHash<int, QByteArray> AnalysisLobbyListVm::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[year] = "year";
    roles[monthDay]="monthDay";
    roles[simpleCheckResult] = "simpleCheckResult";
    return roles;
}






//template<Type type>
//void AnalysisLobbyListVm::drawImage(CheckResultModel<type> checkResultModel)
//{

//}
}
#include "analysislobbylistvm.moc"
