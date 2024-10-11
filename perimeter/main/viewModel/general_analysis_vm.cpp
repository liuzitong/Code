#include "general_analysis_vm.h"
#include <QMessageBox>
#include <QVariant>
#include <perimeter/main/services/analysis_svc.h>
#include <perimeter/main/model/checkResultModel.h>
#include <perimeter/main/model/patientmodel.h>
#include "perimeter/main/appctrl/perimeter_appctrl.hxx"
#include "perimeter/third-part/qxpack/indcom/ui_qml_base/qxpack_ic_ui_qml_api.hxx"
#include <perimeter/main/services/translate_svc.h>
#include <perimeter/main/services/dicom.h>




namespace Perimeter {
class ThresholdAnalysisResult : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float md READ getmd)
    Q_PROPERTY(float psd READ getpsd)
    Q_PROPERTY(float VFI READ getVFI)
    Q_PROPERTY(int GHT READ getGHT)
    Q_PROPERTY(float p_md READ getp_md)
    Q_PROPERTY(float p_psd READ getp_psd)

public:
    Q_INVOKABLE explicit ThresholdAnalysisResult(float md,float p_md, float psd,float p_psd, float VFI, int GHT,QObject *parent=nullptr):
        QObject(parent),m_md(md),m_p_md(p_md),m_psd(psd),m_p_psd(p_psd),m_VFI(VFI),m_GHT(GHT){};
    Q_INVOKABLE virtual ~ThresholdAnalysisResult(){qDebug()<<"delete";};
    Q_INVOKABLE void destroy(){this->~ThresholdAnalysisResult();}
    float getmd(){return m_md;}
    float getpsd(){return m_psd;}
    float getVFI(){return m_VFI;}
    float getGHT(){return m_GHT;}
    float getp_md(){return m_p_md;}
    float getp_psd(){return m_p_psd;}

private:
    float m_md,m_p_md,m_psd,m_p_psd,m_VFI;
    int m_GHT;
};

class ScreeningAnalysisResult : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int dotSeen READ getDotSeen)
    Q_PROPERTY(int dotWeakSeen READ getDotWeakSeen)
    Q_PROPERTY(int dotUnseen READ getDotUnseen)
public:
    Q_INVOKABLE explicit ScreeningAnalysisResult(int dotSeen,int dotWeakSeen,int dotUnseen, QObject *parent=nullptr):
        QObject(parent),m_dotSeen(dotSeen),m_dotWeakSeen(dotWeakSeen),m_dotUnseen(dotUnseen){};
    Q_INVOKABLE virtual ~ScreeningAnalysisResult(){qDebug()<<"delete";};
    Q_INVOKABLE void destroy(){this->~ScreeningAnalysisResult();}
    int getDotSeen(){return m_dotSeen;}
    int getDotWeakSeen(){return m_dotWeakSeen;}
    int getDotUnseen(){return m_dotUnseen;}

private:
    int m_dotSeen,m_dotWeakSeen,m_dotUnseen;
};


StaticAnalysisVm::StaticAnalysisVm(const QVariantList &args)
{

    int resultId=args[0].toInt();
    int imageSize=args[1].toInt();
    int report=args[2].toInt();
    CheckResult_ptr checkResult_ptr(new CheckResult());
    checkResult_ptr->m_id=resultId;
    qx::dao::fetch_by_id(checkResult_ptr);
    Program_ptr program_ptr(new Program());
    program_ptr->m_id=checkResult_ptr->m_program->m_id;
    qx::dao::fetch_by_id(program_ptr);
    Patient_ptr patient_ptr(new Patient());
    patient_ptr->m_id=checkResult_ptr->m_patient->m_id;
    qx::dao::fetch_by_id(patient_ptr);

//    StaticCheckResultModel checkResult(checkResult_ptr);
//    StaticProgramModel program(program_ptr);
//    PatientModel patient(patient_ptr);
    m_checkResult=StaticCheckResultModel(checkResult_ptr);
    m_program=StaticProgramModel(program_ptr);
    m_patient=PatientModel(patient_ptr);
    m_type=int(m_checkResult.m_type);
    m_values.resize(m_checkResult.m_data.checkData.size());
    for(int i=0;i<int(m_checkResult.m_data.checkData.size());i++)
    {
        m_values[i]=m_checkResult.m_data.checkData[i];
    }
    m_locs.resize(m_program.m_data.dots.size());
    for(int i=0;i<int(m_program.m_data.dots.size());i++)
    {
        m_locs[i]={m_program.m_data.dots[i].x,m_program.m_data.dots[i].y};
    }
    m_innerRange=m_program.m_params.commonParams.Range[0];
    m_range=m_program.m_params.commonParams.Range[1];
    m_OS_OD=m_checkResult.m_OS_OD;
    if(m_OS_OD!=0){for(auto&i:m_locs)  i.rx()=-i.rx();}
    m_fixationValues.resize(m_checkResult.m_data.fixationDeviation.size());
    for(int i=0;i<int(m_checkResult.m_data.fixationDeviation.size());i++)
    {
        m_fixationValues[i]=m_checkResult.m_data.fixationDeviation[i];
    }

    QImage img=QImage({imageSize,imageSize}, QImage::Format_RGB32);

    auto analysisMethodSvc=AnalysisSvc::getSingleton();

    if(m_type==0)
    {
        if(report==0)
        {
            analysisMethodSvc->ThresholdAnalysis(resultId,m_dev,m_mDev,m_peDev,m_peMDev,m_md,m_psd,m_VFI,m_GHT,m_p_md,m_p_psd);
            analysisMethodSvc->drawText(m_values,m_locs,m_range,m_OS_OD,img,DrawType::DB);img.save(m_previewFolder+"dBDiagram.bmp");
            analysisMethodSvc->drawGray(m_values,m_locs,m_range,m_innerRange,img);img.save(m_previewFolder+"gray.bmp");


            analysisMethodSvc->drawText(m_dev,m_locs,m_range,m_OS_OD,img,DrawType::Dev);img.save(m_previewFolder+"TotalDeviation.bmp");
            analysisMethodSvc->drawPE(m_peDev,m_locs,m_range,img);img.save(m_previewFolder+"TotalPE.bmp");
            if(m_md<-20)
            {
                analysisMethodSvc->drawWords(img,{tr("Pattern Deviation not"),tr("Shown for serverely"),tr("Depressed fields. Refer"),tr("to Total Deviation")},1.0,true);
                img.save(m_previewFolder+"PatternDeviation.bmp");;img.save(m_previewFolder+"PatternPE.bmp");
            }
            else
            {
                analysisMethodSvc->drawText(m_mDev,m_locs,m_range,m_OS_OD,img,DrawType::MDev);img.save(m_previewFolder+"PatternDeviation.bmp");
                analysisMethodSvc->drawPE(m_peMDev,m_locs,m_range,img);img.save(m_previewFolder+"PatternPE.bmp");
            }


        }
        else if(report==1)
        {
            analysisMethodSvc->ThreeInOneAnalysis(resultId,m_dev);
            analysisMethodSvc->drawText(m_values,m_locs,m_range,m_OS_OD,img,DrawType::DB);img.save(m_previewFolder+"dBDiagram.bmp");
            analysisMethodSvc->drawGray(m_values,m_locs,m_range,m_innerRange,img);img.save(m_previewFolder+"gray.bmp");
            analysisMethodSvc->drawDefectDepth(m_dev,m_locs,m_range,img);img.save(m_previewFolder+"defectDepth.bmp");
        }
        else if(report==2)
        {
            analysisMethodSvc->ThresholdAnalysis(resultId,m_dev,m_mDev,m_peDev,m_peMDev,m_md,m_psd,m_VFI,m_GHT,m_p_md,m_p_psd);
            analysisMethodSvc->drawText(m_values,m_locs,m_range,m_OS_OD,img,DrawType::DB);img.save(m_previewFolder+"dBDiagram.bmp");
            analysisMethodSvc->drawGray(m_values,m_locs,m_range,m_innerRange,img);img.save(m_previewFolder+"gray.bmp");
            analysisMethodSvc->drawPE(m_peDev,m_locs,m_range,img);img.save(m_previewFolder+"TotalPE.bmp");
            if(m_md<-5)
            {
                analysisMethodSvc->drawWords(img,{tr("Pattern Deviation not"),tr("Shown for serverely"),tr("Depressed fields. Refer"),tr("to Total Deviation")});
                img.save(m_previewFolder+"PatternPE.bmp");
            }
            else
                analysisMethodSvc->drawPE(m_peMDev,m_locs,m_range,img);img.save(m_previewFolder+"PatternPE.bmp");

        }
        else if(report==3)
        {
            analysisMethodSvc->ThreeInOneAnalysis(resultId,m_dev);
            analysisMethodSvc->drawText(m_values,m_locs,m_range,m_OS_OD,img,DrawType::DB,0.8);img.save(m_previewFolder+"dBDiagram.bmp");
            analysisMethodSvc->drawDefectDepth(m_dev,m_locs,m_range,img,0.8);img.save(m_previewFolder+"defectDepth.bmp");
        }
        else if(report==4)
        {
            analysisMethodSvc->drawText(m_values,m_locs,m_range,m_OS_OD,img,DrawType::DB,0.8);img.save(m_previewFolder+"dBDiagram.bmp");
        }
    }
    else
    {
        analysisMethodSvc->ScreeningAnalysis(resultId,m_dotSeen,m_dotWeakSeen,m_dotUnseen);
        analysisMethodSvc->drawScreening(m_values,m_locs,m_range,m_OS_OD,img,0.8);img.save(m_previewFolder+"Screening.bmp");

    }


}

StaticAnalysisVm::~StaticAnalysisVm()
{

}

QPointF StaticAnalysisVm::getClickDot(float MouseX, float MouseY, float width, float height)
{
//    qDebug()<<QString("x:%1,y:%2,w:%3,h:%4").arg(QString::number(MouseX)).arg(QString::number(MouseY)).arg(QString::number(width)).arg(QString::number(height));

    auto temp=m_selectedDotIndex;
    float x_degree=(MouseX/width-0.5)*m_range*2;
    float y_degree=-(MouseY/height-0.5)*m_range*2;

//    qDebug()<<QString("xdegre:%1,y:dgeree:%2").arg(QString::number(x_degree)).arg(QString::number(y_degree));

    QPointF nearestDot;
    float nearest_dist_squared=10000;
    for(int i=0;i<m_locs.length();i++)
    {
        auto dot=m_locs[i];
        float dist_squared=pow(dot.x()-x_degree,2)+pow(dot.y()-y_degree,2);
        if(dist_squared<nearest_dist_squared)
        {
            nearest_dist_squared=dist_squared;
            nearestDot=dot;
            m_selectedDotIndex=i;
        }
    }

    if(m_checkResult.m_params.commonParams.centerDotCheck)
    {
        if((pow(x_degree,2)+pow(y_degree,2))<nearest_dist_squared)
        {
            m_selectedDotIndex=2*m_locs.length();
            nearestDot={0,0};
        }
    }

    if(temp!=m_selectedDotIndex)
    {
        emit selectedDotIndexChanged();
    }
    return nearestDot;
}

QPointF StaticAnalysisVm::getPixFromPoint(QPointF point, float width, float height)
{
//    qDebug()<<QString("x:nearestDot:%1,y:nearestDot:%2").arg(QString::number(point.x())).arg(QString::number(point.y()));

    float pixPerDegW=float(width/2)/m_range;
    float pixPerDegH=float(height/2)/m_range;
    return QPointF(width/2+(point.x()+0.0)*pixPerDegW,height/2-(point.y()-0.0)*pixPerDegH);
}

void StaticAnalysisVm::showReport(int report,bool uploadDicom)
{
//    std::cout<<"***********************"<<std::endl;
//    QElapsedTimer timer;
//    timer.start();
    auto analysisMethodSvc=AnalysisSvc::getSingleton();
    QImage img480=QImage({480,480}, QImage::Format_RGB32);
    QImage img550=QImage({550,550}, QImage::Format_RGB32);
    QImage img800=QImage({800,800}, QImage::Format_RGB32);
    QImage img500=QImage({500,500}, QImage::Format_RGB32);
    QImage img1100=QImage({1100,1100}, QImage::Format_RGB32);
    QImage imgFixation=QImage({322*2,27*2}, QImage::Format_RGB32);

    if(m_type==0)
    {
        if(report==0)
        {
//            analysisMethodSvc->DrawThreshold(resultId)
            analysisMethodSvc->drawText(m_values,m_locs,m_range,m_OS_OD,img480,DrawType::DB,1.0,true);img480.save(m_reportFolder+"dBDiagram.bmp");
            analysisMethodSvc->drawGray(m_values,m_locs,m_range,m_innerRange,img480);img480.save(m_reportFolder+"gray.bmp");

            analysisMethodSvc->drawText(m_dev,m_locs,m_range,m_OS_OD,img480,DrawType::Dev,1.0,true);img480.save(m_reportFolder+"TotalDeviation.bmp");
            analysisMethodSvc->drawPE(m_peDev,m_locs,m_range,img480);img480.save(m_reportFolder+"TotalPE.bmp");
            if(m_md<-5)
            {
                analysisMethodSvc->drawWords(img480,{tr("Pattern Deviation not"),tr("Shown for serverely"),tr("Depressed fields. Refer"),tr("to Total Deviation")},1.0,true);
                img480.save(m_reportFolder+"PatternDeviation.bmp");img480.save(m_reportFolder+"PatternPE.bmp");
            }
            else
            {
                analysisMethodSvc->drawText(m_mDev,m_locs,m_range,m_OS_OD,img480,DrawType::MDev,1.0,true);img480.save(m_reportFolder+"PatternDeviation.bmp");
                analysisMethodSvc->drawPE(m_peMDev,m_locs,m_range,img480);img480.save(m_reportFolder+"PatternPE.bmp");
            }

        }
        else if(report==1)
        {
            analysisMethodSvc->drawText(m_values,m_locs,m_range,m_OS_OD,img480,DrawType::DB,1.0,true);img480.save(m_reportFolder+"dBDiagram.bmp");
            analysisMethodSvc->drawGray(m_values,m_locs,m_range,m_innerRange,img480);img480.save(m_reportFolder+"gray.bmp");
            analysisMethodSvc->drawDefectDepth(m_dev,m_locs,m_range,img480,1.0,true);img480.save(m_reportFolder+"defectDepth.bmp");
        }
        else if(report==2)
        {
            analysisMethodSvc->drawText(m_values,m_locs,m_range,m_OS_OD,img480,DrawType::DB,1.0,true);img480.save(m_reportFolder+"dBDiagram.bmp");
            analysisMethodSvc->drawGray(m_values,m_locs,m_range,m_innerRange,img480);img480.save(m_reportFolder+"gray.bmp");
            analysisMethodSvc->drawPE(m_peDev,m_locs,m_range,img480);img480.save(m_reportFolder+"TotalPE.bmp");
            if(m_md<-5)
            {
                analysisMethodSvc->drawWords(img480,{tr("Pattern Deviation not"),tr("Shown for serverely"),tr("Depressed fields. Refer"),tr("to Total Deviation")},1.0,true);
                img480.save(m_reportFolder+"PatternPE.bmp");
            }
            else
                analysisMethodSvc->drawPE(m_peMDev,m_locs,m_range,img480);img480.save(m_reportFolder+"PatternPE.bmp");
        }
        else if(report==3)
        {
            analysisMethodSvc->drawText(m_values,m_locs,m_range,m_OS_OD,img800,DrawType::DB,0.8,true);img800.save(m_reportFolder+"dBDiagram.bmp");
            analysisMethodSvc->drawDefectDepth(m_dev,m_locs,m_range,img800,0.8,true);img800.save(m_reportFolder+"defectDepth.bmp");
        }
        else if(report==4)
        {
            analysisMethodSvc->drawText(m_values,m_locs,m_range,m_OS_OD,img1100,DrawType::DB,0.8,true);img1100.save(m_reportFolder+"dBDiagram.bmp");
        }
    }
    else
    {
        analysisMethodSvc->drawScreening(m_values,m_locs,m_range,m_OS_OD,img1100,0.8,true);img1100.save(m_reportFolder+"Screening.bmp");
    }
    analysisMethodSvc->drawFixationDeviation(m_fixationValues,imgFixation);imgFixation.save(m_reportFolder+"FixationDeviation.bmp");

    QString filePath;
    if(m_type==0)
    {
        switch (report)
        {
            case 0:filePath="./reports/Single.lrxml";break;
            case 1:filePath="./reports/ThreeInOne.lrxml";break;
            case 2:filePath="./reports/OverView.lrxml";break;
            case 3:filePath="./reports/TwoInOne.lrxml";break;
            case 4:filePath="./reports/DB.lrxml";break;
        }
    }
    else if(m_type==1)
    {
        filePath="./reports/Screening.lrxml";
    }
    if(UtilitySvc::reportEngine==nullptr) UtilitySvc::reportEngine=new  LimeReport::ReportEngine();         //耗时长
    UtilitySvc::reportEngine->loadFromFile(filePath);                                                      //耗时长
    if(!TranslateController::isRuntimeLangEng())  UtilitySvc::reportEngine->setReportLanguage(QLocale::Chinese);
    auto manager=UtilitySvc::reportEngine->dataManager();
    manager->clearUserVariables();
    manager->setReportVariable("ProgramName",TranslateController::getTranlatedName(m_program.m_name));
    manager->setReportVariable("OS_OD",m_checkResult.m_OS_OD==0?"OS":"OD");
    manager->setReportVariable("hospitalName",QxPack::IcUiQmlApi::appCtrl()->property("settings").value<QObject*>()->property("hospitalName").toString());
    manager->setReportVariable("name",m_patient.m_name);
    manager->setReportVariable("birthDate",m_patient.m_birthDate.toString("yyyy/MM/dd"));
    manager->setReportVariable("checkDate", m_checkResult.m_time.date().toString("yyyy/MM/dd"));
    manager->setReportVariable("ID", m_patient.m_patientId);
    manager->setReportVariable("age", m_patient.m_age);
    manager->setReportVariable("checkTime", m_checkResult.m_time.time().toString("H:mm:ss"));
    manager->setReportVariable("sex", int(m_patient.m_sex)==0?tr("Male"):tr("Female"));
    auto rx=m_patient.m_rx;
    if(!m_OS_OD)
    {
        manager->setReportVariable("Rx_Ry","Rx:"+QString::number(rx.rx1_l,'f',2)+" DS:"+QString::number(rx.rx2_l,'f',2)+" DC:"+QString::number(rx.rx3_l,'f',2));
        manager->setReportVariable("visualAcuity",tr("Visual acuity")+QString(":")+QString::number(rx.visual_l,'f',2));
    }
    else
    {
        manager->setReportVariable("Rx_Ry","Rx:"+QString::number(rx.rx1_r,'f',2)+" DS:"+QString::number(rx.rx2_r,'f',2)+" DC:"+QString::number(rx.rx3_r,'f',2));
        manager->setReportVariable("visualAcuity",tr("Visual acuity")+QString(":")+QString::number(rx.visual_r,'f',2));
    }
    manager->setReportVariable("pupilDiameter",tr("Pupil diameter")+QString(":")+QString::number(m_checkResult.m_data.pupilDiameter,'f',2));

    auto commomParams=m_checkResult.m_params.commonParams;
    QString fixationMonitor;
    switch (int(commomParams.fixationMonitor)){case 0:fixationMonitor=tr("No remind"); break;case 1:fixationMonitor=tr("Only remind");break;case 2:fixationMonitor=tr("Remind and pause");break;}
    QString fixationTarget;
    switch (int(commomParams.fixationTarget)){case 0:fixationTarget=tr("Center dot");break;case 1:fixationTarget=tr("Small diamond");break;case 2:fixationTarget=tr("Big diamond");break;case 3:fixationTarget=tr("Bottom dot");break;}
    manager->setReportVariable("fixationMonitor",tr("Eye move remind mode")+QString(": ")+fixationMonitor);
    manager->setReportVariable("fixationTarget",tr("Fixation target")+QString(": ")+fixationTarget);
    auto resultData=m_checkResult.m_data;
    manager->setReportVariable("fixationLosses",tr("Fixation losses")+QString(": ")+QString::number(resultData.fixationLostCount)+"/"+QString::number(resultData.fixationLostTestCount));
    manager->setReportVariable("falsePositiveRate",tr("False positive rate")+QString(": ")+QString::number(qRound(float(resultData.falsePositiveCount)/resultData.falsePositiveTestCount*100))+"%");
    manager->setReportVariable("falseNegativeRate",tr("False negative rate")+QString(": ")+QString::number(qRound(float(resultData.falseNegativeCount)/resultData.falseNegativeTestCount*100))+"%");
    QTime time;time.setHMS(0,0,0);time=time.addSecs(resultData.testTimespan);
    manager->setReportVariable("checkTimespan",tr("Check timespan")+QString(": ")+time.toString("mm:ss"));
    manager->setReportVariable("centerDotCheck",tr("Center dot check")+QString(": ")+(commomParams.centerDotCheck?tr("On"):tr("Off")));
    QString cursorSize;switch(int(commomParams.cursorSize)){case 0:cursorSize="I";break;case 1:cursorSize="II";break;case 2:cursorSize="III";break;case 3:cursorSize="IV";break;case 4:cursorSize="V";break;}
    QString cursorColor;switch(int(commomParams.cursorColor)){case 0:cursorColor=tr("White");break;case 1:cursorColor=tr("Red");break;case 2:cursorColor=tr("Blue");break;}
    manager->setReportVariable("stimCursor",tr("Stimulus cursor")+QString(": ")+cursorSize+","+cursorColor);
    manager->setReportVariable("backgroundColor",tr("Background color")+QString(": ")+QString(int(commomParams.backGroundColor)==0?"31.5":"315")+" ASB");
    QString strategy;switch(int(commomParams.strategy)){case 0:strategy=tr("Full threshold");break;case 1:strategy=tr("Fast threshold");break;case 2:strategy=tr("Smart interactive");break;case 3:strategy=tr("Fast interactive");break;case 4:strategy=tr("One stage");break;case 5:strategy=tr("Two stages");break;case 6:strategy=tr("Quantify defects");break;case 7:strategy=tr("Single stimulation");break;}
    manager->setReportVariable("Strategy",tr("Strategy")+QString(": ")+strategy);
    manager->setReportVariable("VFI",QString(tr("VFI"))+": "+QString::number(qRound(m_VFI*100))+"%");
    QString GHT;switch (m_GHT){case 0:GHT=tr("Out of limits");break;case 1:GHT=tr("Low sensitivity");break;case 2:GHT=tr("Border of limits");break;case 3:GHT=tr("Within normal limits");case 4:tr("Abnormally high of sensitivity");break;}
    manager->setReportVariable("GHT",tr("GHT")+QString(": ")+GHT);
    manager->setReportVariable("MD",tr("MD")+QString(": ")+QString::number(m_md,'f',2)+(m_p_md<10-FLT_EPSILON?" (P<"+QString::number(m_p_md)+"%)":""));
    manager->setReportVariable("PSD",tr("PSD")+QString(": ")+QString::number(m_psd,'f',2)+(m_p_psd<10-FLT_EPSILON?" (P<"+QString::number(m_p_psd)+"%)":""));

    if(m_type==0)
    {
        switch (report)
        {
        case 0:
            manager->setReportVariable("DBImagePath","./reportImage/dBDiagram.bmp");
            manager->setReportVariable("GrayImagePath","./reportImage/gray.bmp");
            manager->setReportVariable("TotalDeviationImagePath","./reportImage/TotalDeviation.bmp");
            manager->setReportVariable("PatternDeviationImagePath","./reportImage/PatternDeviation.bmp");
            manager->setReportVariable("TotalPEImagePath","./reportImage/TotalPE.bmp");
            manager->setReportVariable("PatternPEImagePath","./reportImage/PatternPE.bmp");
            break;
        case 1:
            manager->setReportVariable("GrayImagePath","./reportImage/gray.bmp");
            manager->setReportVariable("DefectDepthImagePath","./reportImage/defectDepth.bmp");
            manager->setReportVariable("DBImagePath","./reportImage/dBDiagram.bmp");
            break;
        case 2:
            manager->setReportVariable("DBImagePath","./reportImage/dBDiagram.bmp");
            manager->setReportVariable("GrayImagePath","./reportImage/gray.bmp");
            manager->setReportVariable("TotalPEImagePath","./reportImage/TotalPE.bmp");
            manager->setReportVariable("PatternPEImagePath","./reportImage/PatternPE.bmp");
            break;
        case 3:
            manager->setReportVariable("DefectDepthImagePath","./reportImage/defectDepth.bmp");
            manager->setReportVariable("DBImagePath","./reportImage/dBDiagram.bmp");
            break;
        case 4:
            manager->setReportVariable("DBImagePath","./reportImage/dBDiagram.bmp");
            break;
        }
    }
    else
    {
        manager->setReportVariable("ScreeningImagePath","./reportImage/Screening.bmp");
        manager->setReportVariable("Seen",QString(tr("Seen"))+":"+QString::number(m_dotSeen)+"/"+QString::number(m_values.length()));
        manager->setReportVariable("WeakSeen",QString(tr("Weak seen"))+":"+QString::number(m_dotWeakSeen)+"/"+QString::number(m_values.length()));
        manager->setReportVariable("Unseen",QString(tr("Unseen"))+":"+QString::number(m_dotUnseen)+"/"+QString::number(m_values.length()));
    }


    manager->setReportVariable("FixationDeviationImagePath","./reportImage/FixationDeviation.bmp");
    //可能更新过,qml中的和这个不是同一个内存空间,需要重新从数据库刷新
    CheckResult_ptr checkResult_ptr(new CheckResult());
    checkResult_ptr->m_id=m_checkResult.m_id;
    qx::dao::fetch_by_id(checkResult_ptr);
    manager->setReportVariable("DiagnosisContent",checkResult_ptr->m_diagnosis);
    manager->setReportVariable("deviceInfo",tr("Device info")+QString(":")+QxPack::IcUiQmlApi::appCtrl()->property("settings").value<QObject*>()->property("deviceInfo").toString());
    manager->setReportVariable("version", tr("Version")+QString(":")+QxPack::IcUiQmlApi::appCtrl()->property("settings").value<QObject*>()->property("version").toString());

    if(!uploadDicom)
    {
        UtilitySvc::reportEngine->setPreviewScaleType(LimeReport::ScaleType::Percents,50);
        UtilitySvc::reportEngine->previewReport(/*LimeReport::PreviewHint::ShowAllPreviewBars*/);   //耗时非常长
    }
    else
    {
        UtilitySvc::reportEngine->printToPDF("./dicom");
        auto dicom=Dicom::getSingleton();
        bool res=dicom->upLoadDcm(m_patient);
        QString info=res?tr("Upload executed."):tr("Upload failed.");

        QMessageBox msgBox;
        msgBox.setText(info);
        msgBox.exec();
    }
}

QObject *StaticAnalysisVm::getResult()
{
    if(m_type==0)
        return new ThresholdAnalysisResult(m_md,m_p_md,m_psd,m_p_psd,m_VFI,m_GHT);
    else
        return new ScreeningAnalysisResult(m_dotSeen,m_dotWeakSeen,m_dotUnseen);
}




DynamicAnalysisVm::DynamicAnalysisVm(const QVariantList &args)
{
    int resultId=args[0].toInt();
    int imageSize=args[1].toInt();
    CheckResult_ptr checkResult_ptr(new CheckResult());
    checkResult_ptr->m_id=resultId;
    qx::dao::fetch_by_id(checkResult_ptr);
    Program_ptr program_ptr(new Program());
    program_ptr->m_id=checkResult_ptr->m_program->m_id;
    qx::dao::fetch_by_id(program_ptr);
    Patient_ptr patient_ptr(new Patient());
    patient_ptr->m_id=checkResult_ptr->m_patient->m_id;
    qx::dao::fetch_by_id(patient_ptr);

    m_checkResult=DynamicCheckResultModel(checkResult_ptr);
    m_program=DynamicProgramModel(program_ptr);
    m_patient=PatientModel(patient_ptr);
    m_type=int(m_program.m_type);

    auto analysisMethodSvc=AnalysisSvc::getSingleton();
    m_values.resize(m_checkResult.m_data.checkData.size());
    m_dotNames.resize(m_checkResult.m_data.checkData.size());
    for(int i=0;i<int(m_checkResult.m_data.checkData.size());i++)
    {
        m_values[i]={m_checkResult.m_data.checkData[i].end.x,m_checkResult.m_data.checkData[i].end.y};
        m_dotNames[i]=m_checkResult.m_data.checkData[i].name.c_str();
    }
    qDebug()<<m_values;
    m_range=m_program.m_params.Range[1];
//    m_fixationValues.resize(m_checkResult.m_data.fixationDeviation.size());
//    for(int i=0;i<int(m_checkResult.m_data.fixationDeviation.size());i++)
//    {
//        m_fixationValues[i]=m_checkResult.m_data.fixationDeviation[i];
//    }

    QImage img=QImage({imageSize,imageSize}, QImage::Format_RGB32);
    analysisMethodSvc->drawDynamic(m_values,m_dotNames,m_range,img);img.save(m_previewFolder+"Dynamic.bmp");

}

DynamicAnalysisVm::~DynamicAnalysisVm()
{

}

void DynamicAnalysisVm::showReport(int report,bool uploadDicom)
{
    auto analysisMethodSvc=AnalysisSvc::getSingleton();
    QImage img1100=QImage({1100,1100}, QImage::Format_RGB32);
    QImage imgFixation=QImage({322*2,27*2}, QImage::Format_RGB32);
    analysisMethodSvc->drawDynamic(m_values,m_dotNames,m_range,img1100,1.0,true);img1100.save(m_reportFolder+"Dynamic.bmp");
    analysisMethodSvc->drawFixationDeviation(m_fixationValues,imgFixation);imgFixation.save(m_reportFolder+"FixationDeviation.bmp");


    if(UtilitySvc::reportEngine==nullptr) UtilitySvc::reportEngine=new  LimeReport::ReportEngine();
    if(!TranslateController::isRuntimeLangEng())  UtilitySvc::reportEngine->setReportLanguage(QLocale::Chinese);
    if(report==0)
    {
        UtilitySvc::reportEngine->loadFromFile("./reports/Dynamic.lrxml");
    }
    else
    {
        UtilitySvc::reportEngine->loadFromFile("./reports/DynamicDotsInfo.lrxml");
    }
    auto manager=UtilitySvc::reportEngine->dataManager();
    manager->clearUserVariables();
    manager->setReportVariable("ProgramName",TranslateController::getTranlatedName(m_program.m_name));
    manager->setReportVariable("OS_OD",m_checkResult.m_OS_OD==0?"OS":"OD");
    manager->setReportVariable("hospitalName",QxPack::IcUiQmlApi::appCtrl()->property("settings").value<QObject*>()->property("hospitalName").toString());
    manager->setReportVariable("name",m_patient.m_name);
    manager->setReportVariable("birthDate",m_patient.m_birthDate.toString("yyyy/MM/dd"));
    manager->setReportVariable("checkDate", m_checkResult.m_time.date().toString("yyyy/MM/dd"));
    manager->setReportVariable("ID", m_patient.m_patientId);
    manager->setReportVariable("age", m_patient.m_age);
    manager->setReportVariable("checkTime", m_checkResult.m_time.time().toString("H:mm:ss"));
    manager->setReportVariable("sex", int(m_patient.m_sex)==0?tr("Male"):tr("Female"));
    auto rx=m_patient.m_rx;
    if(!m_checkResult.m_OS_OD)
    {
        manager->setReportVariable("Rx_Ry","Rx:"+QString::number(rx.rx1_l,'f',2)+" DS:"+QString::number(rx.rx2_l,'f',2)+" DC:"+QString::number(rx.rx3_l,'f',2));
        manager->setReportVariable("visualAcuity",tr("Visual acuity")+QString(":")+QString::number(rx.visual_l,'f',2));
    }
    else
    {
        manager->setReportVariable("Rx_Ry","Rx:"+QString::number(rx.rx1_r,'f',2)+" DS:"+QString::number(rx.rx2_r,'f',2)+" DC:"+QString::number(rx.rx3_r,'f',2));
        manager->setReportVariable("visualAcuity",tr("Visual acuity")+QString(":")+QString::number(rx.visual_r,'f',2));
    }
    manager->setReportVariable("pupilDiameter",tr("Pupil diameter")+QString(":")+QString::number(m_checkResult.m_data.pupilDiameter,'f',2));

    auto resultData=m_checkResult.m_data;
    QTime time;time.setHMS(0,0,0);time=time.addSecs(resultData.testTimespan);
    auto params=m_checkResult.m_params;
    QString cursorSize;switch(int(params.cursorSize)){case 0:cursorSize="I";break;case 1:cursorSize="II";break;case 2:cursorSize="III";break;case 3:cursorSize="IV";break;case 4:cursorSize="V";break;}
    QString cursorColor;switch(int(params.cursorColor)){case 0:cursorColor=tr("White");break;case 1:cursorColor=tr("Red");break;case 2:cursorColor=tr("Blue");break;}
    QString cursorSpeed=QString::number(params.speed);
    QString cursorBrightness=QString::number(params.brightness);
    manager->setReportVariable("checkTimespan",tr("Check timespan")+QString(": ")+time.toString("mm:ss"));
    manager->setReportVariable("stimCursor",tr("Stimulus cursor")+QString(": ")+cursorSize+","+cursorColor);


    if(report==0)
    {
        manager->setReportVariable("DynamicImagePath","./reportImage/Dynamic.bmp");
    }
    else
    {
//        manager->addModel("band",&model,false);
        auto data=m_checkResult.m_data.checkData;
        auto dataSource=manager->dataSource("dotinfos");
        auto model=dataSource->model();
        model->insertRows(0,data.size()-2);

        for(int i=0;i<int(data.size());i++)
        {
            model->setData( model->index(i,0),data[i].name.c_str());
            model->setData( model->index(i,1),QString::number(UtilitySvc::OrthToPolar(data[i].start.toQPointF()).x(),'f',0));
            model->setData( model->index(i,2),QString::number(UtilitySvc::OrthToPolar(data[i].start.toQPointF()).y(),'f',0));
            model->setData( model->index(i,3),QString::number(UtilitySvc::OrthToPolar(data[i].end.toQPointF()).x(),'f',0));
            model->setData( model->index(i,4),QString::number(UtilitySvc::OrthToPolar(data[i].end.toQPointF()).y(),'f',0));
            model->setData( model->index(i,5),data[i].isSeen?tr("Seen"):tr("Unseen"));
        }
    }

    CheckResult_ptr checkResult_ptr(new CheckResult());
    checkResult_ptr->m_id=m_checkResult.m_id;
    qx::dao::fetch_by_id(checkResult_ptr);
    manager->setReportVariable("DiagnosisContent",checkResult_ptr->m_diagnosis);

//    manager->setReportVariable("FixationDeviationImagePath","./reportImage/FixationDeviation.bmp");
    manager->setReportVariable("deviceInfo",tr("Device info")+QString(":")+QxPack::IcUiQmlApi::appCtrl()->property("settings").value<QObject*>()->property("deviceInfo").toString());
    manager->setReportVariable("version", tr("Version")+QString(":")+QxPack::IcUiQmlApi::appCtrl()->property("settings").value<QObject*>()->property("version").toString());


    if(!uploadDicom)
    {
        UtilitySvc::reportEngine->setShowProgressDialog(true);
        UtilitySvc::reportEngine->setPreviewScaleType(LimeReport::ScaleType::Percents,50);
        UtilitySvc::reportEngine->previewReport(/*LimeReport::PreviewHint::ShowAllPreviewBars*/);
    }
    else
    {
        UtilitySvc::reportEngine->printToPDF("./dicom");
        auto dicom=Dicom::getSingleton();
        bool res=dicom->upLoadDcm(m_patient);
        QString info=res?tr("Upload executed."):tr("Upload failed.");

        QMessageBox msgBox;
        msgBox.setText(info);
        msgBox.exec();
    }
}

StaticAnalysisOverViewVm::StaticAnalysisOverViewVm(const QVariantList &args)
{
//    auto id=args[0].toList();
    QList<int> id;
    for(auto& i:args[0].toList())
    {
        id.append(i.toInt());
    }
    int diagramWidth=args[1].toInt();
    m_overViewList.reset(new OverViewListVm(id,diagramWidth));
    m_patient=m_overViewList->m_patient;
}

StaticAnalysisOverViewVm::~StaticAnalysisOverViewVm()
{

}

void StaticAnalysisOverViewVm::showReport(int report,QString diagnosis,bool uploadDicom)
{
    qDebug()<<diagnosis;
    if(UtilitySvc::reportEngine==nullptr) UtilitySvc::reportEngine=new  LimeReport::ReportEngine();
    if(!TranslateController::isRuntimeLangEng())  UtilitySvc::reportEngine->setReportLanguage(QLocale::Chinese);
    UtilitySvc::reportEngine->loadFromFile("./reports/OverView.lrxml");
    auto manager=UtilitySvc::reportEngine->dataManager();
    manager->setReportVariable("hospitalName",QxPack::IcUiQmlApi::appCtrl()->property("settings").value<QObject*>()->property("hospitalName").toString());
    manager->setReportVariable("name",m_patient.m_name);
    manager->setReportVariable("birthDate",m_patient.m_birthDate.toString("yyyy/MM/dd"));
    manager->setReportVariable("ID", m_patient.m_patientId);
    manager->setReportVariable("age", m_patient.m_age);
    manager->setReportVariable("sex", int(m_patient.m_sex)==0?tr("Male"):tr("Female"));

    auto data=m_overViewList->m_data;
    auto dataSource=manager->dataSource("datalist");
    auto model=dataSource->model();
    model->insertRows(0,data.size()-2);
    for(int i=0;i<int(data.size());i++)
    {
        model->setData( model->index(i,0),QString(tr("program"))+":"+data[i].program);
        model->setData( model->index(i,1),QString(tr("check date"))+":"+data[i].checkDate.date().toString("yyyy/MM/dd"));
        QString strategy;switch(int(data[i].strategy)){case 0:strategy=tr("Full threshold");break;case 1:strategy=tr("Fast threshold");break;case 2:strategy=tr("Smart interactive");break;case 3:strategy=tr("Fast interactive");break;}
        model->setData( model->index(i,2),QString(tr("strategy"))+":"+strategy);
        QString GHT;switch (data[i].GHT){case 0:GHT=tr("Out of limits");break;case 1:GHT=tr("Low sensitivity");break;case 2:GHT=tr("Border of limits");break;case 3:GHT=tr("Within normal limits");break;case 4:tr("Abnormally high of sensitivity");break;}
        model->setData( model->index(i,3),QString(tr("GHT"))+":"+GHT);
        model->setData( model->index(i,4),QString(tr("eye kind"))+":"+(data[i].OS_OD==0?"OS":"OD"));
        model->setData( model->index(i,5),QString(tr("center dot check"))+":"+(data[i].centerDotCheck?tr("On"):tr("Off")));
        model->setData( model->index(i,6),tr("MD")+QString(": ")+QString::number(data[i].md,'f',2)+(data[i].p_md-10<=FLT_EPSILON?" (P<"+QString::number(data[i].p_md)+"%)":""));
        model->setData( model->index(i,7),tr("PSD")+QString(": ")+QString::number(data[i].psd,'f',2)+(data[i].p_psd<10-FLT_EPSILON?" (P<"+QString::number(data[i].p_psd)+"%)":""));
        model->setData( model->index(i,8),m_reportFolder+data[i].dBDiagramPicPath);
        model->setData( model->index(i,9),m_reportFolder+data[i].grayPicPath);
        model->setData( model->index(i,10),m_reportFolder+data[i].totalDeviationPicPath);
        model->setData( model->index(i,11),m_reportFolder+data[i].patternDeviationPicPath);
    }

    manager->setReportVariable("DiagnosisContent", diagnosis);
    manager->setReportVariable("deviceInfo",tr("Device info")+QString(":")+QxPack::IcUiQmlApi::appCtrl()->property("settings").value<QObject*>()->property("deviceInfo").toString());
    manager->setReportVariable("version", tr("Version")+QString(":")+QxPack::IcUiQmlApi::appCtrl()->property("settings").value<QObject*>()->property("version").toString());

    if(!uploadDicom)
    {
        UtilitySvc::reportEngine->setShowProgressDialog(true);
        UtilitySvc::reportEngine->setPreviewScaleType(LimeReport::ScaleType::Percents,50);
        UtilitySvc::reportEngine->previewReport(/*LimeReport::PreviewHint::ShowAllPreviewBars*/);
    }
    else
    {
        UtilitySvc::reportEngine->printToPDF("./dicom");
        auto dicom=Dicom::getSingleton();
        bool res=dicom->upLoadDcm(m_patient);
        QString info=res?tr("Upload executed."):tr("Upload failed.");

        QMessageBox msgBox;
        msgBox.setText(info);
        msgBox.exec();
    }
}


OverViewListVm::OverViewListVm(QList<int> ids,int diagramWidth)
{
    qDebug()<<ids;
    qDebug()<<diagramWidth;
    for(int i=0;i<ids.length();i++)
    {
        QVector<int> m_values,m_fixationValues,m_dev,m_mDev,m_peDev,m_peMDev;
        QVector<QPointF> m_locs;
        float m_md,m_psd,m_VFI,m_p_md,m_p_psd;
        int m_GHT,m_innerRange,m_range,m_OS_OD;

        StaticCheckResultModel m_checkResult;
        StaticProgramModel m_program;

        int resultId=ids[i];
        int imageSize=diagramWidth;

        CheckResult_ptr checkResult_ptr(new CheckResult());
        checkResult_ptr->m_id=resultId;
        qx::dao::fetch_by_id(checkResult_ptr);
        Program_ptr program_ptr(new Program());
        program_ptr->m_id=checkResult_ptr->m_program->m_id;
        qx::dao::fetch_by_id(program_ptr);
        m_checkResult=StaticCheckResultModel(checkResult_ptr);
        m_program=StaticProgramModel(program_ptr);

        if(i==0)
        {
            Patient_ptr patient_ptr(new Patient());
            patient_ptr->m_id=checkResult_ptr->m_patient->m_id;
            qx::dao::fetch_by_id(patient_ptr);
            m_patient=PatientModel(patient_ptr);
        }

        m_values.resize(m_checkResult.m_data.checkData.size());
        for(int i=0;i<int(m_checkResult.m_data.checkData.size());i++)
        {
            m_values[i]=m_checkResult.m_data.checkData[i];
        }
        m_locs.resize(m_program.m_data.dots.size());
        for(int i=0;i<int(m_program.m_data.dots.size());i++)
        {
            m_locs[i]={m_program.m_data.dots[i].x,m_program.m_data.dots[i].y};
        }
        m_innerRange=m_program.m_params.commonParams.Range[0];
        m_range=m_program.m_params.commonParams.Range[1];
        m_OS_OD=m_checkResult.m_OS_OD;
        if(m_OS_OD!=0){for(auto&i:m_locs)  i.rx()=-i.rx();}
        m_fixationValues.resize(m_checkResult.m_data.fixationDeviation.size());
        for(int i=0;i<int(m_checkResult.m_data.fixationDeviation.size());i++)
        {
            m_fixationValues[i]=m_checkResult.m_data.fixationDeviation[i];
        }
        QImage img=QImage({imageSize,imageSize}, QImage::Format_RGB32);
        auto analysisMethodSvc=AnalysisSvc::getSingleton();
        analysisMethodSvc->ThresholdAnalysis(resultId,m_dev,m_mDev,m_peDev,m_peMDev,m_md,m_psd,m_VFI,m_GHT,m_p_md,m_p_psd);

        QString dBDiagramPicPath="./overView_dBDiagram"+QString::number(i)+".bmp";
        QString grayPicPath="./overView_gray"+QString::number(i)+".bmp";
        QString TotalPEPicPath="./overView_TotalPE"+QString::number(i)+".bmp";
        QString PatternPEPicPath="./overView_PatternPE"+QString::number(i)+".bmp";

        QString dBDiagramPicPath1=m_previewFolder+"./overView_dBDiagram"+QString::number(i)+".bmp";
        QString grayPicPath1=m_previewFolder+"./overView_gray"+QString::number(i)+".bmp";
        QString TotalPEPicPath1=m_previewFolder+"./overView_TotalPE"+QString::number(i)+".bmp";
        QString PatternPEPicPath1=m_previewFolder+"./overView_PatternPE"+QString::number(i)+".bmp";
        analysisMethodSvc->drawText(m_values,m_locs,m_range,m_OS_OD,img,DrawType::DB);img.save(dBDiagramPicPath1);
        analysisMethodSvc->drawGray(m_values,m_locs,m_range,m_innerRange,img);img.save(grayPicPath1);
        analysisMethodSvc->drawPE(m_peDev,m_locs,m_range,img);img.save(TotalPEPicPath1);
        if(m_md<-5)
        {
            analysisMethodSvc->drawWords(img,{tr("Pattern Deviation not"),tr("Shown for serverely"),tr("Depressed fields. Refer"),tr("to Total Deviation")});
            img.save(PatternPEPicPath1);
        }
        else
            analysisMethodSvc->drawPE(m_peMDev,m_locs,m_range,img);img.save(PatternPEPicPath1);


        QImage img2=QImage({400,400}, QImage::Format_RGB32);
        QString dBDiagramPicPath2=m_reportFolder+"./overView_dBDiagram"+QString::number(i)+".bmp";
        QString grayPicPath2=m_reportFolder+"./overView_gray"+QString::number(i)+".bmp";
        QString TotalPEPicPath2=m_reportFolder+"./overView_TotalPE"+QString::number(i)+".bmp";
        QString PatternPEPicPath2=m_reportFolder+"./overView_PatternPE"+QString::number(i)+".bmp";
        analysisMethodSvc->drawText(m_values,m_locs,m_range,m_OS_OD,img2,DrawType::DB,1.0,true);img2.save(dBDiagramPicPath2);
        analysisMethodSvc->drawGray(m_values,m_locs,m_range,m_innerRange,img2);img2.save(grayPicPath2);
        analysisMethodSvc->drawPE(m_peDev,m_locs,m_range,img2);img2.save(TotalPEPicPath2);
        if(m_md<-5)
        {
            analysisMethodSvc->drawWords(img2,{tr("Pattern Deviation not"),tr("Shown for serverely"),tr("Depressed fields. Refer"),tr("to Total Deviation")},1.0,true);
            img2.save(PatternPEPicPath2);
        }
        else
            analysisMethodSvc->drawPE(m_peMDev,m_locs,m_range,img2);img2.save(PatternPEPicPath2);

        m_data.append(OverViewData{
                          m_program.m_name,
                          m_checkResult.m_time,
                          int(m_checkResult.m_params.commonParams.strategy),
                          m_GHT,
                          m_checkResult.m_OS_OD,
                          m_checkResult.m_params.commonParams.centerDotCheck,
                          m_md,m_psd,m_p_md,m_p_psd,
                          dBDiagramPicPath,
                          grayPicPath,
                          TotalPEPicPath,
                          PatternPEPicPath});
        std::sort(m_data.begin(),m_data.end(),[](OverViewData a,OverViewData b){return a.checkDate<b.checkDate;});
    }




//    auto dateTime = QDateTime::fromString("2013-05-16T13:24:33","yyyy-MM-ddTHH:mm:ss");
//    qDebug()<<dateTime.toString("yyyy/MM/dd");
//    m_data.append(OverViewData{"30-2",QDateTime::fromString("2013-05-16T13:24:33","yyyy-MM-ddThh:mm:ss"),1,1,0,false,2.14f,2.5f,12.0f,5.3f,"./grayPic1","./threshPic1","./totalDeviPic1","./patterDeviPic1"});
//    m_data.append(OverViewData{"30-2",QDateTime::fromString("2016-05-16T13:24:33","yyyy-MM-ddThh:mm:ss"),1,1,0,false,2.24f,2.5f,12.0f,5.3f,"./grayPic1","./threshPic1","./totalDeviPic1","./patterDeviPic1"});
//    m_data.append(OverViewData{"30-2",QDateTime::fromString("2017-05-16T13:24:33","yyyy-MM-ddThh:mm:ss"),1,1,0,false,2.34f,2.5f,12.0f,5.3f,"./grayPic1","./threshPic1","./totalDeviPic1","./patterDeviPic1"});
//    m_data.append(OverViewData{"30-2",QDateTime::fromString("2018-05-16T13:24:33","yyyy-MM-ddThh:mm:ss"),1,1,0,false,2.44f,2.5f,12.0f,5.3f,"./grayPic1","./threshPic1","./totalDeviPic1","./patterDeviPic1"});
//    m_data.append(OverViewData{"30-2",QDateTime::fromString("2019-05-16T13:24:33","yyyy-MM-ddThh:mm:ss"),1,1,0,false,2.54f,2.5f,12.0f,5.3f,"./grayPic1","./threshPic1","./totalDeviPic1","./patterDeviPic1"});
//    m_data.append(OverViewData{"30-2",QDateTime::fromString("2020-05-16T13:24:33","yyyy-MM-ddThh:mm:ss"),1,1,0,false,2.64f,2.5f,12.0f,5.3f,"./grayPic1","./threshPic1","./totalDeviPic1","./patterDeviPic1"});
//    m_data.append(OverViewData{"30-2",QDateTime::fromString("2021-05-16T13:24:33","yyyy-MM-ddThh:mm:ss"),1,1,0,false,2.74f,2.5f,12.0f,5.3f,"./grayPic1","./threshPic1","./totalDeviPic1","./patterDeviPic1"});
}

OverViewListVm::~OverViewListVm()
{

}

int OverViewListVm::rowCount(const QModelIndex &parent) const
{
    return m_data.length();
}

QVariant OverViewListVm::data(const QModelIndex &idx, int role) const
{
    int index=idx.row();
    auto data=m_data[index];
    switch (role)
    {
    case (OverViewRoles::program):return data.program;
    case (OverViewRoles::checkDate):return data.checkDate;
    case (OverViewRoles::strategy): return data.strategy;
    case (OverViewRoles::GHT): return data.GHT;
    case (OverViewRoles::OS_OD): return data.OS_OD;
    case (OverViewRoles::centerDotCheck): return data.centerDotCheck;
    case (OverViewRoles::psd): return data.psd;
    case (OverViewRoles::md): return data.md;
    case (OverViewRoles::p_psd): return data.p_psd;
    case (OverViewRoles::p_md): return data.p_md;
    case (OverViewRoles::grayPicPath): return m_previewFolder+data.grayPicPath;
    case (OverViewRoles::dBDiagramPicPath): return m_previewFolder+data.dBDiagramPicPath;
    case (OverViewRoles::totalDeviationPicPath): return m_previewFolder+data.totalDeviationPicPath;
    case (OverViewRoles::patternDeviationPicPath): return m_previewFolder+data.patternDeviationPicPath;
    default:return QVariant();
    }
}

QHash<int, QByteArray> OverViewListVm::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[program] = "program";
    roles[checkDate] = "checkDate";
    roles[strategy] = "strategy";
    roles[GHT] = "GHT";
    roles[OS_OD] = "OS_OD";
    roles[centerDotCheck]="centerDotCheck";
    roles[md]="md";
    roles[p_md]="p_md";
    roles[psd]="psd";
    roles[p_psd]="p_psd";
    roles[grayPicPath]="grayPicPath";
    roles[dBDiagramPicPath]="dBDiagramPicPath";
    roles[totalDeviationPicPath]="totalDeviationPicPath";
    roles[patternDeviationPicPath]="patternDeviationPicPath";
    return roles;
}



}

#include "general_analysis_vm.moc"
