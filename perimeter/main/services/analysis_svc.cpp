#include "analysis_svc.h"
#include <QDebug>
#include <QImage>
#include <QPainter>
#include <QFont>
#include <QLine>
#include <QtMath>
#include <QPair>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include "perimeter/main/appctrl/perimeter_appctrl.hxx"
#include <QFileDialog>
#include <QApplication>
#include <QtMath>
#include "perimeter/third-part/qxpack/indcom/ui_qml_base/qxpack_ic_ui_qml_api.hxx"
#include <perimeter/main/model/patientmodel.h>
#include "utility_svc.h"

namespace Perimeter {

template <typename T1,typename T2>
class LeastSquare{
private:
    double k, b;
public:
    LeastSquare(const QVector<T1>& x, const QVector<T2>& y)
    {
        double t1=0, t2=0, t3=0, t4=0;
        for(int i=0; i<x.size(); ++i)
        {
            t1 += double(x[i])*double(x[i]);
            t2 += double(x[i]);
            t3 += double(x[i])*double(y[i]);
            t4 += double(y[i]);
        }
        k = (t3*x.size() - t2*t4) / (t1*x.size() - t2*t2);  // 求得β1
        b = (t1*t4 - t2*t3) / (t1*x.size() - t2*t2);        // 求得β2
    }



    double getY(const T2 x) const
    {
        return k*x + b;
    }

    double getSlope() const
    {
        return k;
    }
};

AnalysisSvc* AnalysisSvc::singleton=nullptr;


AnalysisSvc::AnalysisSvc()
{
    QFile jsonFile("./data.json");
    if( !jsonFile.open(QIODevice::ReadOnly)) {qDebug() << "read file error!";return;}
    QJsonParseError jsonParserError;
    auto JsonDoc = QJsonDocument::fromJson(jsonFile.readAll(),&jsonParserError);
    m_jsonArray=JsonDoc.array();
    jsonFile.close();

    auto jsonArrToVectorPoint=[&](QVector<QPoint>& vec,const QString& name,QJsonObject& jo)->void
    {
        if(jo["name"]==name)
        {
            auto arr=jo["data"].toArray();
            for(int i=0;i<arr.count();i++)
            {
                auto arr2=arr[i].toArray();
                QPoint point(arr2[0].toInt(),arr2[1].toInt());
                vec.push_back(point);
            }
        }
    };
    auto jsonArrToVectorInt=[&](QVector<int>& vec,const QString& name,QJsonObject& jo)->void
    {
        if(jo["name"]==name)
        {
            auto arr=jo["data"].toArray();
            for(int i=0;i<arr.count();i++){vec.push_back(arr[i].toInt());}
        }
    };


    for(auto i:m_jsonArray)
    {
        auto jo=i.toObject();

//        jsonArrToVectorPoint(m_pointLoc_30d,"XY_NORMAL_VALUE_30d",jo);
//        jsonArrToVectorPoint(m_pointLoc_60d,"XY_NORMAL_VALUE_60d",jo);

//        QVector<QVector<QString>> jsonObjNames={
//            {"NORMAL_VALUE36_45_B1_White","NORMAL_VALUE36_45_B1_Red","NORMAL_VALUE36_45_B1_Blue"},
//            {"NORMAL_VALUE36_45_B2_White","NORMAL_VALUE36_45_B2_Red","NORMAL_VALUE36_45_B2_Blue"},
//            {/*B3_WHITE采用该jsonObjNames2*/"NORMAL_VALUE36_45_B3_Red","NORMAL_VALUE36_45_B3_Blue"},
//            {"NORMAL_VALUE36_45_B4_White","NORMAL_VALUE36_45_B4_Red","NORMAL_VALUE36_45_B4_Blue"},
//            {"NORMAL_VALUE36_45_B5_White","NORMAL_VALUE36_45_B5_Red","NORMAL_VALUE36_45_B5_Blue"}
//        };

//        QVector<QString> jsonObjNames2={"NORMAL_VALUE15_35","NORMAL_VALUE36_45","NORMAL_VALUE46_55","NORMAL_VALUE56_65","NORMAL_VALUE66_75"};

//        m_value_30d_cursorSize_cursorColor.resize(jsonObjNames.length());
//        for(int i=0;i<jsonObjNames.length();i++)
//        {
//            m_value_30d_cursorSize_cursorColor[i].resize(jsonObjNames[i].length());
//            for(int j=0;j<jsonObjNames[i].length();j++)
//            {
//                 jsonArrToVectorInt(m_value_30d_cursorSize_cursorColor[i][j],jsonObjNames[i][j],jo);
//            }
//        }

//        m_value_30d_cursorSizeIII_ageCorrection.resize(jsonObjNames2.length());
//        for(int i=0;i<jsonObjNames2.length();i++)
//        {
//             jsonArrToVectorInt(m_value_30d_cursorSizeIII_ageCorrection[i],jsonObjNames2[i],jo);
//        }

//        jsonArrToVectorInt(m_value_60d,"NORMAL_VALUE15_35_60d",jo);

        jsonArrToVectorPoint(m_GHT1_RIGHT,"GHT1_RIGHT",jo);
        jsonArrToVectorPoint(m_GHT2_RIGHT,"GHT2_RIGHT",jo);
        jsonArrToVectorPoint(m_GHT3_RIGHT,"GHT3_RIGHT",jo);
        jsonArrToVectorPoint(m_GHT4_RIGHT,"GHT4_RIGHT",jo);
        jsonArrToVectorPoint(m_GHT5_RIGHT,"GHT5_RIGHT",jo);


        jsonArrToVectorInt(m_pe_v5[0],"PE_VALUE5",jo);
        jsonArrToVectorInt(m_pe_v2[0],"PE_VALUE2",jo);
        jsonArrToVectorInt(m_pe_v1[0],"PE_VALUE1",jo);
        jsonArrToVectorInt(m_pe_v05[0],"PE_VALUE05",jo);

        jsonArrToVectorInt(m_pe_v5[1],"PE_VALUE5_Blue_Yellow_B5",jo);
        jsonArrToVectorInt(m_pe_v2[1],"PE_VALUE2_Blue_Yellow_B5",jo);
        jsonArrToVectorInt(m_pe_v1[1],"PE_VALUE1_Blue_Yellow_B5",jo);
        jsonArrToVectorInt(m_pe_v05[1],"PE_VALUE05_Blue_Yellow_B5",jo);
    }
}

AnalysisSvc::~AnalysisSvc()
{
    delete singleton;
}


void AnalysisSvc::ThresholdAnalysis(int resultId,QVector<int>& dev,QVector<int>& mDev,QVector<int>& peDev,QVector<int>& peMDev,float& md,float& psd,float& VFI,int& GHT, float& p_md,float& p_psd)
{

    CheckResult_ptr checkResult_ptr(new CheckResult());
    checkResult_ptr->m_id=resultId;
    qx::dao::fetch_by_id(checkResult_ptr);
    Program_ptr program_ptr(new Program());
    program_ptr->m_id=checkResult_ptr->m_program->m_id;
    qx::dao::fetch_by_id(program_ptr);
    Patient_ptr patient_ptr(new Patient());
    patient_ptr->m_id=checkResult_ptr->m_patient->m_id;
    qx::dao::fetch_by_id(patient_ptr);

    StaticCheckResultModel checkResult(checkResult_ptr);
    StaticProgramModel program(program_ptr);
    PatientModel patient(patient_ptr);

    auto params=checkResult.m_params;
    int cursorSize=int(params.commonParams.cursorSize);
    int cursorColor=int(params.commonParams.cursorColor);
    int backGroundColor=int(params.commonParams.backGroundColor);

//    auto testDate=checkResult.m_time.date();
//    auto birthDate=patient.m_birthDate;
//    int testAge= testDate.year()- birthDate.year();
//    if (testDate.month() < birthDate.month() || (testDate.month() == birthDate.month() && testDate.day() < birthDate.day())) { testAge--;}


//    QVector<int> value_30d;
//    if(cursorSize==2)
//    {
//        if(cursorColor==0)
//            value_30d=m_value_30d_cursorSizeIII_ageCorrection[age_correction];
//        else
//            value_30d=m_value_30d_cursorSize_cursorColor[cursorSize][cursorColor-1];
//    }
//    else
//        value_30d=m_value_30d_cursorSize_cursorColor[cursorSize][cursorColor];


    QVector<int> value_30d=UtilitySvc::getSingleton()->getValue30d(cursorSize,cursorColor,patient.m_age);


//    qDebug()<<value_30d;

    QVector<int> pe_v5,pe_v2,pe_v1,pe_v05;
    if(cursorColor==2&&backGroundColor==1&&cursorSize==4)
    {  pe_v5=m_pe_v5[1]; pe_v2=m_pe_v2[1]; pe_v1=m_pe_v1[1]; pe_v05=m_pe_v05[1];}
    else
    {  pe_v5=m_pe_v5[0]; pe_v2=m_pe_v2[0]; pe_v1=m_pe_v1[0]; pe_v05=m_pe_v05[0];}


    auto dotList=program.m_data.dots;
    QVector<int> sv(dotList.size(),0);
    dev.fill(0,dotList.size());
    mDev.fill(0,dotList.size());
    peDev.fill(0,dotList.size());
    peMDev.fill(0,dotList.size());

    QVector<int> vfiRingStandard(5,0);
    QVector<int> vfiRingTest(5,0);

    float VFI_Weight[5]={3.29f,1.28f,0.79f,0.57f,0.43f};

    int age_correction;
    int testAge=patient.m_age;
    if(testAge<=35){age_correction=0;}
    else if(testAge<=45){age_correction=1;}
    else if(testAge<=55){age_correction=2;}
    else if(testAge<=65){age_correction=3;}
    else {age_correction=4;}

    qDebug()<<checkResult.m_OS_OD;
    for(int i=0;i<int(dotList.size());i++)
    {
        auto dot=dotList[i];
        float radius=sqrt(pow(dot.x,2)+pow(dot.y,2));
        int index;
//        if(radius<=30)
//        {
        index=UtilitySvc::getSingleton()->getIndex(QPointF{dot.x,dot.y},m_pointLoc_30d,checkResult.m_OS_OD);
        if(index==-1) continue;
        sv[i]=value_30d[index];
//        }
//        else if(radius<=60)                     //应该没有此情况，阈值分析都是radius<30
//        {
//            index=getIndex(QPointF{dot.x,dot.y},m_pointLoc_60d,checkResult.m_OS_OD);
//            if(index==-1) continue;
//            sv[i]=m_value_60d[index];
//        }
        if(!(cursorSize==2&&cursorColor==0))                //CURSORIII_white情况已经考虑年龄了所以不年龄修正
        {
            if(cursorColor==2)
            {
                if(sv[i]>0){sv[i]-=2*(age_correction-1);} else if(sv[i]<0){sv[i]+=2*(age_correction-1);}
            }
            else
            {
                if(sv[i]>0){sv[i]-=(age_correction-1);} else if(sv[i]<0){sv[i]+=(age_correction-1);}
            }
        }


//        qDebug()<<sv[i];


        if(sv[i]>=0)
        {
            auto val=checkResult.m_data.checkData[i];
            if(val>51) val=51;
            if(val<0) val=0;
            dev[i]=val-sv[i];
        }                                                   //dev 盲点
        else{ dev[i]=-99; }

//        qDebug()<<dev[i];


        if(dev[i]!=-99)
        {
            int v=-dev[i];
            if (v<=pe_v5[index]) peDev[i]=0;
            else if(v<=pe_v2[index]) peDev[i]=1;
            else if(v<=pe_v1[index]) peDev[i]=2;
            else if(v<=pe_v05[index]) peDev[i]=3;
            else peDev[i]=4;
        }
        else{
            peDev[i]=-99;
        }

//        qDebug()<<peDev[i];

        if(sv[i]>0)
        {
            int ringIndex=round(radius/6+0.4)-1;
            if(ringIndex>4) ringIndex=4;

            vfiRingStandard[ringIndex]+=sv[i];
            auto val=checkResult.m_data.checkData[i];
            if(val>51) val=51;
            if(val<0) val=0;
            vfiRingTest[ringIndex]+=val;
        }
    }

    float vfih=0,vfid=0;
    for(int i=0;i<5;i++)
    {
        if(vfiRingStandard[i]!=0)
        {
            float temp=float(vfiRingTest[i])/(vfiRingStandard[i]+0.0001);
            if(temp>1){temp=1;}
            vfih+=VFI_Weight[i]*temp;
            vfid+=VFI_Weight[i];
        }
    }
//    qDebug()<<vfih;
//    qDebug()<<vfid;

    VFI=vfih/vfid;                                     //vfi
//    qDebug()<<m_VFI;
    md=0;
    for(auto& i: dev)
    {
        if(i!=-99) md+=i;
    }
    md/=(dev.length()-2);    //扣除盲点

//    qDebug()<<m_md;
    psd=0;
    for(auto& i: dev)
    {
        if(i!=-99) psd+=pow(i-md,2);
    }
    psd=sqrt(psd/(dev.length()-2));

    if(psd>4.3) p_psd=0.5;
    else if(psd>3.7) p_psd=1;
    else if(psd>3.2) p_psd=2;
    else if(psd>2.5) p_psd=5;
    else if(psd>2.0) p_psd=10;
    else p_psd=10;

    if(md<-5.5)  p_md=0.5;
    else if(md<-3.5) p_md=1;
    else if(md<-2.6) p_md=2;
    else if(md<-2.0) p_md=5;
    else if(md<-1.5) p_md=10;
    else p_md=10;

//    qDebug()<<m_psd;

    int md_correction=round(md+psd);
//    qDebug()<<md;

    for(int i=0;i<dev.length();i++)
    {
        if(dev[i]==-99)
            mDev[i]=-99;
        else
            mDev[i]=dev[i]-md_correction;
    }


    for(int i=0;i<mDev.length();i++)
    {

        auto index=UtilitySvc::getSingleton()->getIndex(QPointF(dotList[i].x,dotList[i].y),m_pointLoc_30d,checkResult.m_OS_OD);
        if(index==-1) continue;
        float radius=sqrt(pow(dotList[i].x,2)+pow(dotList[i].y,2));
        if(radius<30)
        {
            if(mDev[i]==-99)
                peMDev[i]=-99;
            else
            {
                int v=-mDev[i];
                if (v<=pe_v5[index]) peMDev[i]=0;
                else if(v<=pe_v2[index]) peMDev[i]=1;
                else if(v<=pe_v1[index]) peMDev[i]=2;
                else if(v<=pe_v05[index]) peMDev[i]=3;
                else peMDev[i]=4;
            }
        }
    }

    auto isGHT=[](QVector<QPoint> GHTArr,QPoint point)->bool
    {
        for(auto&i:GHTArr)
        {
            if(i.x()==point.x()&&i.y()==point.y()) return true;
        }
        return false;
    };


    float ght[5][2];
    bool b27=false;
    memset(ght,0,sizeof(ght));
    for(int i=0;i<int(dotList.size());i++)
    {
        int v;QPoint dot;
        if(mDev[i]!=-99)
            v=-mDev[i];

        if(checkResult.m_OS_OD==0)
            dot={int(dotList[i].x),int(dotList[i].y)};
        else
           dot={int(-dotList[i].x),int(dotList[i].y)};

        if(isGHT(m_GHT1_RIGHT,dot)){ght[0][0]+=v;}
        if(isGHT(m_GHT2_RIGHT,dot)){if(v>=7&&abs(dot.x())==abs(dot.y())) b27=true;ght[1][0]+=v;}
        if(isGHT(m_GHT3_RIGHT,dot)){ght[2][0]+=v;}
        if(isGHT(m_GHT4_RIGHT,dot)){ght[3][0]+=v;}
        if(isGHT(m_GHT5_RIGHT,dot)){ght[4][0]+=v;}
        if(isGHT(m_GHT1_RIGHT,{dot.x(),-dot.y()})){ght[0][1]+=v;}
        if(isGHT(m_GHT2_RIGHT,{dot.x(),-dot.y()})){if(v>=7&&abs(dot.x())==abs(dot.y())) b27=true;ght[1][1]+=v;}
        if(isGHT(m_GHT3_RIGHT,{dot.x(),-dot.y()})){ght[2][1]+=v;}
        if(isGHT(m_GHT4_RIGHT,{dot.x(),-dot.y()})){ght[3][1]+=v;}
        if(isGHT(m_GHT5_RIGHT,{dot.x(),-dot.y()})){ght[4][1]+=v;}
    }

    ght[0][0] *= 1 / 3;
    ght[1][0] *= 1 / 4;
    ght[2][0] *= 1 / 5;
    ght[3][0] *= 1 / 6;
    ght[4][0] *= 1 / 4;

    ght[0][1] *= 1 / 3;
    ght[1][1] *= 1 / 4;
    ght[2][1] *= 1 / 5;
    ght[3][1] *= 1 / 6;
    ght[4][1] *= 1 / 4;

    GHT=3;
    if (ght[0][0] >= 5 || ght[0][1] >= 5 || ght[1][0] >= 5 || ght[1][1] >= 5 ||
        ght[2][0] >= 6 || ght[2][1] >= 6 || ght[3][0] >= 6 || ght[3][1] >= 6 || ght[4][0] >= 7 || ght[4][1] >= 7)
    {
        GHT = 0;
    }
    else if(b27)
    {
        GHT = 1;
    }
    else if(ght[0][0] > 3 || ght[0][1] > 3 || ght[2][0] >= 4 || ght[2][1] >= 4 || ght[3][0] >= 4 || ght[3][1] >= 4)
    {
        GHT = 2;
    }
}

void AnalysisSvc::ThreeInOneAnalysis(int resultId, QVector<int> &dev)
{
    CheckResult_ptr checkResult_ptr(new CheckResult());
    checkResult_ptr->m_id=resultId;
    qx::dao::fetch_by_id(checkResult_ptr);
    Program_ptr program_ptr(new Program());
    program_ptr->m_id=checkResult_ptr->m_program->m_id;
    qx::dao::fetch_by_id(program_ptr);
    Patient_ptr patient_ptr(new Patient());
    patient_ptr->m_id=checkResult_ptr->m_patient->m_id;
    qx::dao::fetch_by_id(patient_ptr);

    StaticCheckResultModel checkResult(checkResult_ptr);
    StaticProgramModel program(program_ptr);
    PatientModel patient(patient_ptr);

    auto params=checkResult.m_params;
    int cursorSize=int(params.commonParams.cursorSize);
    int cursorColor=int(params.commonParams.cursorColor);

    QVector<int> value_30d=UtilitySvc::getSingleton()->getValue30d(cursorSize,cursorColor,patient.m_age);
//    qDebug()<<value_30d;

    auto dotList=program.m_data.dots;
    QVector<int> sv(dotList.size(),0);
    dev.fill(0,dotList.size());



    int age_correction;
    int testAge=patient.m_age;
    if(testAge<=35){age_correction=0;}
    else if(testAge<=45){age_correction=1;}
    else if(testAge<=55){age_correction=2;}
    else if(testAge<=65){age_correction=3;}
    else {age_correction=4;}


    for(int i=0;i<int(dotList.size());i++)
    {
        auto dot=dotList[i];
        float radius=sqrt(pow(dot.x,2)+pow(dot.y,2));
        int index;
        if(radius<=30)
        {
            index=UtilitySvc::getSingleton()->getIndex(QPointF{dot.x,dot.y},m_pointLoc_30d,checkResult.m_OS_OD);
//            qDebug()<<index;
            if(index==-1) continue;
            sv[i]=value_30d[index];
        }

        else if(radius<=60)
        {
            index=UtilitySvc::getSingleton()->getIndex(QPointF{dot.x,dot.y},m_pointLoc_60d,checkResult.m_OS_OD);
            if(index==-1) continue;
            sv[i]=m_value_60d[index];
//            if(sv[i]>0) sv[i]-=age_correction;else if(sv[i]<0) sv[i]+=age_correction;
        }
        if(!(cursorSize==2&&cursorColor==0))                //CURSORIII_white情况已经考虑年龄了所以不年龄修正
        {
            if(cursorColor==2)
            {
                if(sv[i]>0){sv[i]-=2*(age_correction-1);} else if(sv[i]<0){sv[i]+=2*(age_correction-1);}
            }
            else
            {
                if(sv[i]>0){sv[i]-=(age_correction-1);} else if(sv[i]<0){sv[i]+=(age_correction-1);}
            }
        }
//        qDebug()<<sv[i];
//        qDebug()<<checkResult.m_data.checkData[i];
        if(sv[i]>0)
        {
            auto val=checkResult.m_data.checkData[i];
            if(val>51) val=51;
            if(val<0) val=0;
            dev[i]=val-sv[i];
        }                                                   //dev 盲点
        else{ dev[i]=-99; }           //盲点
//        qDebug()<<dev[i];
    }
}

void AnalysisSvc::ScreeningAnalysis(int resultId,int& dotSeen,int& dotWeakSeen,int& dotUnseen)
{
    CheckResult_ptr checkResult_ptr(new CheckResult());
    checkResult_ptr->m_id=resultId;
    qx::dao::fetch_by_id(checkResult_ptr);
    Program_ptr program_ptr(new Program());
    program_ptr->m_id=checkResult_ptr->m_program->m_id;
    qx::dao::fetch_by_id(program_ptr);

    StaticCheckResultModel checkResult(checkResult_ptr);
    StaticProgramModel program(program_ptr);

    if(checkResult.m_type==Type::Screening)
    {
        dotSeen=0,dotWeakSeen=0,dotUnseen=0;
        auto& checkData=checkResult.m_data.checkData;
        for(auto& i:checkData)
        {
            if(i==0) dotUnseen++;
            else if(i==1) dotWeakSeen++;
            else if(i==2) dotSeen++;
        }
    }
}

void AnalysisSvc::BaseLineAnalysis(const QVector<float> &mds, const QVector<int> &months, float &avgMd, float &progressSpeedBase, float &progressSpeedDeviation, int &slopeType)
{
    double md_total=0;
    for(auto i:mds){md_total+=i;}
    avgMd=md_total/mds.length();

    LeastSquare<int,float> ls(months,mds);
    double matching_slope=ls.getSlope();
    QVector<double> diff_slope_list;
    for(int i=0;i<mds.length()-1;i++)
    {
        double diff_slope=double(mds[i]-mds[i+1])/(months[i]-months[i+1])-matching_slope;
        diff_slope_list.append(diff_slope);
    }

    double diff_slope_squred_sum=0;
    for(int i=0;i<diff_slope_list.length()-1;i++)
    {
        diff_slope_squred_sum+=pow(diff_slope_list[i],2)*(months[i+1]-months[i]);
    }

    double slope_deviation=sqrt(diff_slope_squred_sum/(months.last()-months.first()));

    progressSpeedBase=matching_slope*12;
    progressSpeedDeviation=slope_deviation*12;
    progressSpeedBase<-2?slopeType=1:slopeType=0;   //1明显,0不能显.
}


//void AnalysisSvc::ProgressAnalysis(const QVector<int> (&mDev)[4], const QVector<QPointF> (&locs)[4], int OS_OD,QVector<int> &resultVal)         //第一个是最后近做的一个
//{
//    resultVal.fill(0,mDev[0].length());
//    for(int dot_number=0;dot_number<mDev[0].length();dot_number++)
//    {
//        int val_A=mDev[0][dot_number];
//        QPointF loc_0=locs[0][dot_number];
////        qDebug()<<dot_number;
////        qDebug()<<resultVal[dot_number];

//        for(int i=0;i<3;i++)
//        {

//            int index=getIndex(loc_0,locs[i+1]);
//            if(index!=-1)
//            {
//                int val_B=mDev[i+1][index];
//                auto pe=getpeMDev((val_A-val_B)*1.5,loc_0,OS_OD);
//                if(pe==0) {break;}
//                else if(pe==1)
//                {
//                    resultVal[dot_number]++;
//                    val_A=val_B;
//                }
//                else if(pe>1)
//                {
//                    resultVal[dot_number]=4;
//                    break;
//                }
//                resultVal[dot_number];
//            }
//        }
////        qDebug()<<resultVal[dot_number];
//    }
//}

void AnalysisSvc::ProgressAnalysis(const QVector<QVector<int> > &mDev,const  QVector<QVector<QPointF> > &locs, int OS_OD,QVector<QVector<QPointF> > &resultLocs, QVector<QVector<int> > &resultVal, QVector<QVector<int> > &resultPicVal,QVector<int>& progress)
{
    auto _mDev=const_cast<QVector<QVector<int> >&>(mDev);
    auto baseMDev1=_mDev.takeFirst();
    auto baseMDev2=_mDev.takeFirst();

    auto _locs=const_cast<QVector<QVector<QPointF> >&>(locs);
    auto baseLocs1=_locs.takeFirst();
    auto baseLocs2=_locs.takeFirst();

    QVector<QPointF> baseLocs;
    QVector<int> baseMDev;
    for(int i=0;i<baseLocs1.length();i++)
    {
        auto loc=baseLocs1[i];
        int j=UtilitySvc::getSingleton()->getIndex(loc,baseLocs2);
        if(j!=-1)
        {
            baseLocs.append(loc);
            baseMDev.append(qRound(float(baseMDev1[i]+baseMDev2[j])/2));
        }
    }

    _mDev.prepend(baseMDev);
    _locs.prepend(baseLocs);


    //进展差值计算
    for(int i=0;i<_mDev.length()-1;i++)
    {
        QVector<int> Val;
        QVector<QPointF> Loc;
        for(int j=0;j<_mDev[i+1].length();j++)
        {

            for(int k=i;k>=0;k--)
            {
                int index=UtilitySvc::getSingleton()->getIndex(_locs[i+1][j],_locs[k]);
                if(index!=-1)
                {
                    int MDev_diff=_mDev[i+1][j]-_mDev[k][index];
                    Loc.append(_locs[i+1][j]);
                    Val.append(MDev_diff);
                    break;
                }
            }
        }
        qDebug()<<resultVal.length();
        qDebug()<<resultLocs.length();
        resultVal.append(Val);
        resultLocs.append(Loc);
    }

    for(int i=0;i<resultVal.length();i++)
    {
        QVector<int> picVal;
        for(int j=0;j<resultVal[i].length();j++)
        {
            auto pe=getpeMDev(resultVal[i][j],resultLocs[i][j],OS_OD);
            if(i==0)
            {
                if(pe>0) picVal.append(1);
                else picVal.append(0);
            }
            else
            {
                if(pe>0)
                {
                    for(int k=i;k>=0;k--)
                    {
                        int index=UtilitySvc::getSingleton()->getIndex(resultLocs[i][j],resultLocs[k-1]);
                        if(index!=-1)
                        {
                            picVal.append(resultPicVal[k-1][index]+1);
                            break;
                        }
                    }
                }
                else picVal.append(0);
            }

        }
        resultPicVal.append(picVal);
    }


    for(int i=0;i<resultPicVal.length();i++)
    {
        int K2_count=0,K3_count=0;
        for(int j=0;j<resultPicVal[i].length();j++)
        {
            if(resultPicVal[i][j]==2) K2_count++;
            if(resultPicVal[i][j]==3) K3_count++;
        }
        if(K3_count>=3) progress.append(2);
        else if(K2_count>=3) progress.append(1);
        else progress.append(0);
    }
}


AnalysisSvc *AnalysisSvc::getSingleton()
{
    if (singleton==nullptr)
    {
        singleton=new AnalysisSvc();
    }
    return singleton;
}


void AnalysisSvc::drawPixScale(int range, QImage &img)
{
    int scale=img.width()<400?1:2;
    QPainter painter(&img);
    painter.setBrush(QBrush(QColor("black")));
    painter.setPen({Qt::black,float(scale)});
    painter.drawLine(QLine(0,img.height()/2,img.width(),img.height()/2));
    painter.drawLine(QLine(img.width()/2,0,img.width()/2,img.height()));

    int segmentCount=range/10;
    for(int i=0;i<=segmentCount*2;i++)
    {
        if(i!=segmentCount)
        {
            painter.drawLine(QLine((float(img.width()-scale))/(segmentCount*2)*i+scale/2,img.height()/2-2*scale,(float(img.width()-scale))/(segmentCount*2)*i+scale/2,img.height()/2+2*scale));
            painter.drawLine(QLine(img.width()/2-2*scale,(float(img.height()-scale))/(segmentCount*2)*i+scale/2,img.width()/2+2*scale,(float(img.height()-scale))/(segmentCount*2)*i+scale/2));
        }
    }
}

void AnalysisSvc::drawRoundCrossPixScale(int range, QImage &img)
{
    int scale=img.width()<500?1:2;
    QPainter painter(&img);

    painter.setBrush(QBrush(QColor("black")));
    painter.setPen({Qt::black,float(scale)});
    painter.drawLine(QLine(0,img.height()/2,img.width(),img.height()/2));
    painter.drawLine(QLine(img.width()/2,0,img.width()/2,img.height()));

//    auto drawDashLine=[]()->void{};
    int segmentCount=range/10;
    QPen pen;
    pen.setDashPattern({float(3*scale),float(5*scale)});
    pen.setWidth(float(scale));
    pen.setColor(Qt::black);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.setRenderHint(QPainter::RenderHint::Antialiasing);
    for(int i=1;i<=segmentCount;i++)
    {
        int radius=float(img.width())/2/segmentCount*i-scale/2;
        painter.drawEllipse({img.height()/2,img.width()/2},radius,radius);
    }

    qreal radius=img.width()/2;
    for(int i=1;i<12;i++)
    {
        if(i%3!=0)
        {
            auto angle=M_PI/6*i;
            painter.drawLine(QLine{img.width()/2,img.height()/2,int(img.width()/2+radius*qSin(angle)),int(img.height()/2+radius*qCos(angle))});
        }
    }
}

void AnalysisSvc::drawWords(QImage &img, QVector<QString> words,float minificationFactor,bool isReport)
{
    img.fill(qRgb(255, 255, 255));
    QPainter painter(&img);
    int fontPixSize=qMin(int(img.width()/18*minificationFactor),!isReport?16:32)*1.3;
    QFont font("consolas");
    font.setPixelSize(fontPixSize);
    painter.setFont(font);

    for(int i=0;i<words.length();i++)
    {
        const QRect rectangle = QRect(fontPixSize*0.5,fontPixSize*4+i*fontPixSize*1.5,img.width()-fontPixSize,fontPixSize*1.2);
        painter.drawText(rectangle,Qt::AlignLeft,words[i]);
    }
}

void AnalysisSvc::drawText(QVector<int> values,QVector<QPointF> locs,int range,int OS_OD,QImage& img,DrawType type,float minificationFactor,bool isReport)
{
    img.fill(qRgb(255, 255, 255));
    drawPixScale(range,img);
    QPainter painter(&img);
//    int fontPixSize=img.width()/17*minificationFactor;
    int fontPixSize=qMin(int(img.width()/18*minificationFactor),!isReport?16:32);
    QFont font("consolas");
    font.setPixelSize(fontPixSize);
    painter.setFont(font);
    QPoint blindDot;
    if(OS_OD==0) blindDot={-14,0};else blindDot={14,0};
    auto pixLoc=convertDegLocToPixLoc(blindDot,range,img);
    QImage blindImage(":/grays/SE3.bmp");
    float scale;
    if(img.width()<=400){ scale=1;}
    else scale=2;
    blindImage=blindImage.scaled(blindImage.width()*scale,blindImage.height()*scale);
    painter.drawImage(QPoint{pixLoc.x()-blindImage.width()/2,pixLoc.y()-blindImage.height()/2},blindImage);

    for(int i=0;i<values.length();i++)             //画DB图
    {
        if(type==DrawType::Dev||type==DrawType::MDev)
        {
            if(values[i]==-99) continue;
        }

        if(i<locs.length())
        {
            auto pixLoc=convertDegLocToPixLoc(locs[i],range,img);
            const QRect rectangle = QRect(pixLoc.x()-fontPixSize*1.6*0.45, pixLoc.y()-fontPixSize*0.8/2, fontPixSize*1.6,fontPixSize*0.8);
            if(type==DrawType::DB)
            {
                if(values[i]<0)
                    painter.drawText(rectangle,Qt::AlignCenter,"<0");
                else if(values[i]>51)
                    painter.drawText(rectangle,Qt::AlignCenter,">51");
                else
                    painter.drawText(rectangle,Qt::AlignCenter,QString::number(values[i]));
            }
            else
                painter.drawText(rectangle,Qt::AlignCenter,QString::number(values[i]));
//            img.setPixel(pixLoc.x(),pixLoc.y(),0xFFFF0000); //标个小红点
        }
        else if(i>=locs.length()&&i<2*locs.length())            //短周期
        {
            if(values[i]==-999) continue;
            auto pixLoc=convertDegLocToPixLoc(locs[i-locs.length()],range,img);
            const QRect rectangle = QRect(pixLoc.x()-fontPixSize*2.4*0.50, pixLoc.y()-fontPixSize*1.0/2+fontPixSize, fontPixSize*2.4,fontPixSize*1.0);
            if(pixLoc.y()+fontPixSize*1.5>img.height()) continue;  //防止超过图片范围,干脆不显示
            painter.fillRect(rectangle,QBrush{Qt::white});

            if(values[i]<0)
                painter.drawText(rectangle,Qt::AlignCenter,"(<0)");
            else if(values[i]>51)
                painter.drawText(rectangle,Qt::AlignCenter,"(>51)");
            else
                painter.drawText(rectangle,Qt::AlignCenter,"("+QString::number(values[i])+")");


        }
        else if(i==locs.length()*2)                           //中心点
        {
            if(values[i]==-999) continue;
            auto pixLoc=convertDegLocToPixLoc({0,0},range,img);
            const QRect rectangle = QRect(pixLoc.x()-fontPixSize*1.6*0.45, pixLoc.y()-fontPixSize*0.8/2, fontPixSize*1.6,fontPixSize*0.8);
            painter.fillRect(rectangle,QBrush{Qt::white});

            if(values[i]<0)
                painter.drawText(rectangle,Qt::AlignCenter,"<0");
            else if(values[i]>51)
                painter.drawText(rectangle,Qt::AlignCenter,">51");
            else
                painter.drawText(rectangle,Qt::AlignCenter,QString::number(values[i]));

        }
    }
}

void AnalysisSvc::drawGray(QVector<int> values, QVector<QPointF> locs, int range, int innerRange, QImage& img)
{
    img.fill(qRgb(255, 255, 255));
    drawPixScale(range,img);
    QPainter painter(&img);




    float gap= float(range)/15;
    int min_dist_index;
    int dotGap;
    {
        float min_dist=FLT_MAX;
        {
            QPointF tempLoc=locs[0];
            for(int i=1;i<locs.length();i++)
            {
                float dist=pow(tempLoc.rx()-locs[i].rx(),2)+pow(tempLoc.y()-locs[i].y(),2);
                if(dist<min_dist)
                {
                    min_dist=dist;
                    min_dist_index=i;
                }
            }
        }
    }
    dotGap=qMax(qAbs(locs[0].rx()-locs[min_dist_index].rx()),qAbs(locs[0].ry()-locs[min_dist_index].ry()));

    float left_x_axis=0,right_x_axis=0,up_y_axis=0,down_y_axis=0;
    for(auto &i:locs)
    {
        auto x=i.x();auto y=i.y();
        if(x<left_x_axis) left_x_axis=x;

        if(x>right_x_axis) right_x_axis=x;
        if(y>up_y_axis) up_y_axis=y;
        if(y<down_y_axis) down_y_axis=y;
    }

//    long double a=3;
//    long double b=2;
//    int x;
//    auto c=fmod(3,2);
    float rangeX=qMax(qAbs(left_x_axis),right_x_axis)+dotGap/2;;float rangeY=qMax(qAbs(down_y_axis),up_y_axis)+dotGap/2;

    QVector<QPointF> paintLocs;
    //为了给中间留白-1GAP,又起始点要减去0.5GAP,故减去1.5GAP
    for(float i=up_y_axis+(dotGap/2)/*-fmod(up_y_axis+(dotGap/2),gap)*/-1.5*gap;i>=down_y_axis-(dotGap/2)/*-fmod(down_y_axis-(dotGap/2),gap)*/+1.5*gap-FLT_MIN;i-=gap)
    {
        for(float j=left_x_axis-(dotGap/2)/*-fmod(left_x_axis-(dotGap/2),gap)*/+1.5*gap;j<=right_x_axis+(dotGap/2)/*-fmod(right_x_axis+(dotGap/2),gap)*/-1.5*gap+FLT_MIN;j+=gap)
        {
            float answerOutter=float(qPow(qAbs(i)+gap,2))/(rangeY*rangeY)+float(qPow(qAbs(j)+gap,2))/(rangeX*rangeX);
            float answerInner=1.0;
            if(innerRange!=0){answerInner=(float(qPow(qAbs(i)+gap,2))+float(qPow(qAbs(j)+gap,2)))/(innerRange*innerRange);}
            if((answerOutter<=1.0)&&(answerInner>=1.0))
                paintLocs.append(QPointF(j,i));
        }
    }


//    for(auto&i:paintLocs)
//    {
//        if(i.ry())
//    }
//    qDebug()<<paintLocs;


    for(int i=0;i<paintLocs.length();i++)
    {
        QVector<QPair<int,float>> distIndexList;
        for(int j=0;j<locs.length();j++)
        {
            auto dot=locs[j].toPoint();
            float dist=qPow(paintLocs[i].x()-dot.x(),2)+qPow(paintLocs[i].y()-dot.y(),2);
            distIndexList.append(QPair<int,float>(j,dist));
        }

        std::sort(distIndexList.begin(),distIndexList.end(),[](QPair<int,float> a,QPair<int,float> b){return a.second<b.second;});   //所有测试点按顺序和待画图点按距离从小到大排序.
        QVector<QPair<int,float>> interpolationDots;
        for(int j=0;j<4;j++)                                            //最近4个添加进差值点,且距离不得大于3*gap
        {
//            if(j<2)
//            {
//                interpolationDots.append(distIndexList[j]);
//            }
//            else
//            {
//                if(distIndexList[j].second<=2*qPow(3*gap,2))
//                    interpolationDots.append(distIndexList[j]);
//            }
            interpolationDots.append(distIndexList[j]);
        }

        float totalValue=0,totalDist=0;
        for(int j=0;j<interpolationDots.length();j++)                               //根据插值点算出插值
        {
            int index=interpolationDots[j].first;
            int tempVal=values[index];
            if(tempVal<0) tempVal=0;
            if(tempVal>51) tempVal=51;
            if(interpolationDots[j].second==0)
            {
                totalValue=tempVal;
                totalDist=1;
                break;
            }
            totalValue+=(float(tempVal)/qSqrt(interpolationDots[j].second));
            totalDist+=1.0f/qSqrt(interpolationDots[j].second);
        }
        float interpolVal=totalValue/totalDist;
        int grayVal=qCeil(interpolVal/5);                                           //根据插值算出灰度值
        if(grayVal>9) grayVal=9;



       QPointF tempPoint;
       if(paintLocs[i].x()>FLT_MIN) tempPoint.setX(paintLocs[i].x()+gap); else tempPoint.setX(paintLocs[i].x()-gap);   //偏移一个gap.呈现中间空白的效果
       if(paintLocs[i].y()>FLT_MIN) tempPoint.setY(paintLocs[i].y()+gap); else tempPoint.setY(paintLocs[i].y()-gap);
       auto pixLoc=convertDegLocToPixLoc(tempPoint,range,img);
       QString path=QString(":/grays/Gray")+QString::number(grayVal)+".bmp";
       QImage image(path);
       float scale;
       if(img.width()<400){scale=1;}
       else scale=2;
       auto scaledImage=image.scaled(image.width()*scale,image.height()*scale);
       QPoint tempPixLoc={pixLoc.x()-scaledImage.width()/2,pixLoc.y()-scaledImage.height()/2};
       painter.drawImage(tempPixLoc,scaledImage);
    }
}

void AnalysisSvc::drawPE(QVector<int> values, QVector<QPointF> locs, int range,QImage& img)
{
    img.fill(qRgb(255, 255, 255));
    drawPixScale(range,img);
    QPainter painter(&img);
    float scale;
    if(img.width()<=180) scale=1;
    else if(img.width()<300) scale=2;
    else scale=4;
//    if(img.width()<300) scale=1;
//    else scale=2;
    for(int i=0;i<locs.length()&&i<values.length();i++)             //画DB图
    {
        auto pixLoc=convertDegLocToPixLoc(locs[i],range,img);
        QString path=QString(":/grays/PE")+QString::number(values[i])+".bmp";
        QImage image(path);
        auto scaledImage=image.scaled(image.width()*scale,image.height()*scale);
        QPoint tempPixLoc={pixLoc.x()-scaledImage.width()/2,pixLoc.y()-scaledImage.height()/2};
        painter.drawImage(tempPixLoc,scaledImage);
    }
}

void AnalysisSvc::drawProgess(QVector<int> values, QVector<QPointF> locs, int range, QImage &img)
{
    img.fill(qRgb(255, 255, 255));
    drawPixScale(range,img);
    QPainter painter(&img);
    float scale;
//    if(img.width()<200) scale=1;
//    else if(img.width()<300) scale=2;
//    else scale=4;
    if(img.width()<300) scale=1;
    else scale=2;
    for(int i=0;i<locs.length()&&i<values.length();i++)             //画DB图
    {
        auto pixLoc=convertDegLocToPixLoc(locs[i],range,img);
        QString path=QString(":/grays/GPA")+QString::number(values[i])+".bmp";
//        QString path=QString(":/grays/GPA0.bmp");
        QImage image(path);
        auto scaledImage=image.scaled(image.width()*scale,image.height()*scale);
        QPoint tempPixLoc={pixLoc.x()-scaledImage.width()/2,pixLoc.y()-scaledImage.height()/2};
        painter.drawImage(tempPixLoc,scaledImage);
    }
}

void AnalysisSvc::drawDefectDepth(QVector<int> values, QVector<QPointF> locs, int range, QImage& img,float minificationFactor,bool isReport)
{
    img.fill(qRgb(255, 255, 255));
    drawPixScale(range,img);
    QPainter painter(&img);
//    int fontPixSize=img.width()/17*minificationFactor;
    int fontPixSize=qMin(int(img.width()/17*minificationFactor),!isReport?16:32);
    QFont font("consolas");
    font.setPixelSize(fontPixSize);
    painter.setFont(font);

    for(int i=0;i<locs.length()&&i<values.length();i++)             //画DB图
    {
        auto pixLoc=convertDegLocToPixLoc(locs[i],range,img);
        if(values[i]==-99) continue;
        if(values[i]>-4)
        {
            float scale;
            if(img.width()*minificationFactor<480)
            {
//                scale=qFloor(float(m_image.width())/240);
//                if(scale<1) scale=1;
                scale=1;
            }
            else scale=2;
            QImage image(":/grays/DE0.bmp");
            image=image.scaled(image.width()*scale,image.height()*scale);
            painter.drawImage(QPointF{pixLoc.x()-image.width()*0.48,pixLoc.y()-image.height()*0.50},image);
        }
        else
        {
            const QRect rectangle = QRect(pixLoc.x()-fontPixSize*1.6*0.45, pixLoc.y()-fontPixSize*0.8/2, fontPixSize*1.6,fontPixSize*0.8);
            painter.drawText(rectangle,Qt::AlignCenter,QString::number(-values[i]));
//            img.setPixel(pixLoc.x(),pixLoc.y(),0xFFFF0000);   //标个小红点
        }
    }
}

void AnalysisSvc::drawScreening(QVector<int> values, QVector<QPointF> locs, int range, int OS_OD,QImage& img,float minificationFactor,bool isReport)
{
    img.fill(qRgb(255, 255, 255));
    drawPixScale(range,img);
    QPainter painter(&img);
    QPoint blindDot;
    if(OS_OD==0) blindDot={-14,0};else blindDot={14,0};
    auto pixLoc=convertDegLocToPixLoc(blindDot,range,img);
    QImage blindImage(":/grays/SE3.bmp");
    QImage screenImageSE0(":/grays/SE0.bmp");
    QImage screenImageSE1(":/grays/SE1.bmp");
    QImage screenImageSE2(":/grays/SE2.bmp");
    int scale=img.width()<=900?1:2;
    int fontPixSize=qMin(int(img.width()/17*minificationFactor),!isReport?12:24);
    QFont font("consolas");
    font.setPixelSize(fontPixSize);
    painter.setFont(font);
//    int scale;
//    if(m_isPreview)
//    {
//        scale=qCeil(float(m_image.width())/500);
//    }
//    else scale=3;
    QImage image(":/grays/DE0.bmp");
    image=image.scaled(image.width()*scale,image.height()*scale);
    screenImageSE0=screenImageSE0.scaled(screenImageSE0.width()*scale,screenImageSE0.height()*scale);
    screenImageSE1=screenImageSE1.scaled(screenImageSE1.width()*scale,screenImageSE1.height()*scale);
    screenImageSE2=screenImageSE2.scaled(screenImageSE2.width()*scale,screenImageSE2.height()*scale);
    blindImage=blindImage.scaled(blindImage.width()*scale,blindImage.height()*scale);
    painter.drawImage(QPoint{pixLoc.x()-blindImage.width()/2,pixLoc.y()-blindImage.height()/2},blindImage);
    for(int i=0;i<values.length();i++)
    {
        if(i<locs.length())
        {
            auto pixLoc=convertDegLocToPixLoc(locs[i],range,img);
            auto imageLoc=QPoint{pixLoc.x()-screenImageSE0.width()/2,pixLoc.y()-screenImageSE0.height()/2};
            switch (values[i])
            {
            case 0:painter.drawImage(imageLoc,screenImageSE2);break;
            case 1:painter.drawImage(imageLoc,screenImageSE1);break;
            case 2:painter.drawImage(imageLoc,screenImageSE0);break;
            default:
            {
                const QRect rectangle = QRect(pixLoc.x()-fontPixSize*1.6*0.45, pixLoc.y()-fontPixSize*0.8/2, fontPixSize*1.6,fontPixSize*0.8);
                painter.drawText(rectangle,Qt::AlignCenter,QString::number(values[i]));
                break;
            }
            }
        }
        else if(i<2*locs.length())
        {
            auto pixLoc=convertDegLocToPixLoc(locs[i-locs.length()],range,img);
            auto imageLoc=QPoint{pixLoc.x()-screenImageSE0.width()/2,int(pixLoc.y()-screenImageSE0.height()/2+0.8*fontPixSize)};
            const QRect rectangle = QRect(pixLoc.x()-fontPixSize*2.0*0.47, pixLoc.y()-fontPixSize*0.8/2+0.6*fontPixSize, fontPixSize*2.0,fontPixSize*1.2);

            switch (values[i])
            {
            case -999:break;
            case 0:painter.drawText(rectangle,Qt::AlignCenter,"( )");painter.drawImage(imageLoc,screenImageSE2);break;
            case 1:painter.drawText(rectangle,Qt::AlignCenter,"( )");painter.drawImage(imageLoc,screenImageSE1);break;
            case 2:painter.drawText(rectangle,Qt::AlignCenter,"( )");painter.drawImage(imageLoc,screenImageSE0);break;
            default:
            {
//                const QRect rectangle = QRect(pixLoc.x()-fontPixSize*2.0*0.45, pixLoc.y()-fontPixSize*0.8/2+1.5*fontPixSize, fontPixSize*2.0,fontPixSize*0.8);
                painter.drawText(rectangle,Qt::AlignCenter,"( )");
                painter.drawText(rectangle,Qt::AlignCenter,QString::number(values[i]));
                break;
            }
            }
        }
        else if(i==2*locs.length())
        {
            QPoint pixLoc=convertDegLocToPixLoc({0,0},range,img);
            auto imageLoc=QPoint{pixLoc.x()-screenImageSE0.width()/2,pixLoc.y()-screenImageSE0.height()/2};
            switch (values[i])
            {
            case -999:break;
            case 0:painter.drawImage(imageLoc,screenImageSE2);break;
            case 1:painter.drawImage(imageLoc,screenImageSE1);break;
            case 2:painter.drawImage(imageLoc,screenImageSE0);break;
            default:
            {
                const QRect rectangle = QRect(pixLoc.x()-fontPixSize*1.6*0.45, pixLoc.y()-fontPixSize*0.8/2, fontPixSize*1.6,fontPixSize*0.8);
                painter.drawText(rectangle,Qt::AlignCenter,QString::number(values[i]));
                break;
            }
            }
        }
    }
}

void AnalysisSvc::drawDynamic(QVector<QPointF> values,QVector<QString> dotNames,int range, QImage& img,float minificationFactor,bool isReport)
{
//    QImage img=QImage({imageSize,imageSize}, QImage::Format_RGB32);
    img.fill(qRgb(255, 255, 255));
    drawRoundCrossPixScale(range,img);
    int scale=img.width()<=900?1:2;
    QPainter painter(&img);
    painter.setBackground(QBrush(QColor("white")));
    painter.setBrush(QBrush(QColor("black")));
    painter.setPen({Qt::black,float(scale)});

    int fontPixSize=qMin(int(img.width()/17*minificationFactor),!isReport?16:32);
    QFont font("consolas");
    font.setPixelSize(fontPixSize);
    painter.setFont(font);
    qDebug()<<values.length();

    for(int i=0;i<values.length();i++)
    {

        auto begin=convertDegLocToPixLoc((values[i]),range,img);
        auto end=convertDegLocToPixLoc((values[(i+1)%values.length()]),range,img);
        if(!dotNames.isEmpty())
        {
            painter.drawText(QPoint{begin.x()+fontPixSize/4,begin.y()-fontPixSize/4},dotNames[i]);
        }
        painter.drawLine(QLine(begin.x(),begin.y(),end.x(),end.y()));
        painter.drawEllipse(begin,3*scale,3*scale);
    }
}

void AnalysisSvc::drawVisionFieldIsland(QVector<int> values, QVector<QPointF> locs, int range, int innerRange, QImage &img)
{
    img.fill(qRgb(0, 0, 0));
    QPainter painter(&img);


    float gap= 1;
    int min_dist_index;
    int dotGap;
    {
        float min_dist=FLT_MAX;
        {
            QPointF tempLoc=locs[0];
            for(int i=1;i<locs.length();i++)
            {
                float dist=pow(tempLoc.rx()-locs[i].rx(),2)+pow(tempLoc.y()-locs[i].y(),2);
                if(dist<min_dist)
                {
                    min_dist=dist;
                    min_dist_index=i;
                }
            }
        }
    }
    dotGap=qMax(qAbs(locs[0].rx()-locs[min_dist_index].rx()),qAbs(locs[0].ry()-locs[min_dist_index].ry()));

    float left_x_axis=0,right_x_axis=0,up_y_axis=0,down_y_axis=0;
    for(auto &i:locs)
    {
        auto x=i.x();auto y=i.y();
        if(x<left_x_axis) left_x_axis=x;

        if(x>right_x_axis) right_x_axis=x;
        if(y>up_y_axis) up_y_axis=y;
        if(y<down_y_axis) down_y_axis=y;
    }


    float rangeX=qMax(qAbs(left_x_axis),right_x_axis)+dotGap/2;;float rangeY=qMax(qAbs(down_y_axis),up_y_axis)+dotGap/2;

    QVector<QPointF> paintLocs;
    for(float i=qMin(int(ceil(up_y_axis+(dotGap/2))),range)-0.5*gap/*-fmod(up_y_axis+(dotGap/2),gap)*/;i>=qMax(int(floor(down_y_axis-(dotGap/2))),-range)+0.5*gap/*-fmod(down_y_axis-(dotGap/2),gap)*/;i-=gap)
    {
        for(float j=qMax(int(floor(left_x_axis-(dotGap/2))),-range)+0.5*gap/*-fmod(left_x_axis-(dotGap/2),gap)*/;j<=qMax(int(ceil(right_x_axis+(dotGap/2))),range)-0.5*gap;j+=gap)
        {
            float answerOutter=float(qPow(qAbs(i),2))/(rangeY*rangeY)+float(qPow(qAbs(j),2))/(rangeX*rangeX);
            float answerInner=1.0;
            if(innerRange!=0){answerInner=(float(qPow(qAbs(i),2))+float(qPow(qAbs(j),2)))/(innerRange*innerRange);}
            if((answerOutter<=1.0)&&(answerInner>=1.0))
                paintLocs.append(QPointF(j,i));
        }
    }


//    for(auto&i:paintLocs)
//    {
//        if(i.ry())
//    }
    qDebug()<<paintLocs;


    for(int i=0;i<paintLocs.length();i++)
    {
        QVector<QPair<int,float>> distIndexList;
        for(int j=0;j<locs.length();j++)
        {
            auto dot=locs[j].toPoint();
            float dist=qPow(paintLocs[i].x()-dot.x(),2)+qPow(paintLocs[i].y()-dot.y(),2);
            distIndexList.append(QPair<int,float>(j,dist));
        }

        std::sort(distIndexList.begin(),distIndexList.end(),[](QPair<int,float> a,QPair<int,float> b){return a.second<b.second;});   //所有测试点按顺序和待画图点按距离从小到大排序.
        QVector<QPair<int,float>> interpolationDots;
        for(int j=0;j<4;j++)                                            //最近4个添加进差值点,且距离不得大于3*gap
        {
            interpolationDots.append(distIndexList[j]);
        }

        float totalValue=0,totalDist=0;
        for(int j=0;j<interpolationDots.length();j++)                               //根据插值点算出插值
        {
            int index=interpolationDots[j].first;
            if(interpolationDots[j].second==0)
            {
                totalValue=values[index];
                totalDist=1;
                break;
            }
            totalValue+=(float(values[index])/qSqrt(interpolationDots[j].second));
            totalDist+=1.0f/qSqrt(interpolationDots[j].second);
        }
        int interpolVal=round(totalValue/totalDist);

        int pixcel_x=floor(paintLocs[i].x()+range);
        int pixcel_y=floor(range-paintLocs[i].y());

        img.setPixel(pixcel_x,pixcel_y,qRgb(interpolVal,interpolVal,interpolVal));
    }
}

void AnalysisSvc::drawFixationDeviation(QVector<int> values, QImage& img)
{
    int scale=2;
//    m_isPreview?scale=1:scale=2;
//    auto dots=m_checkResult->getResultData()->getFixationDeviation();
//    dots={5,-5,5,-5,5,-5,13,13,13,-13,-13,-13};
//    QImage image(322*scale,27*scale, QImage::Format_RGB32);
    img.fill(qRgb(255, 255, 255));
    QPainter painter(&img);
    painter.setBackground(QBrush(QColor("white")));
    painter.setBrush(QBrush(QColor("black")));
    painter.setPen({Qt::black,float(scale)});
    painter.drawLine(QLine(scale/2,0,scale/2,27*scale));
    painter.drawLine(QLine(img.width()-scale/2,0,img.width()-scale/2,27*scale));
    painter.drawLine(QLine(0,27*scale/2,img.width()-scale/2,27*scale/2));
    auto convertY=[=](int p)->int{return 13-p;};
    for(int i=0;i<values.length();i++)
    {
        int dot=convertY(values[i]);
        painter.drawLine(QLine((i+1.5)*scale,27*scale/2,(i+1.5)*scale,dot*scale+1));
    }
}


QPoint AnalysisSvc::convertDegLocToPixLoc(QPointF DegLoc,int range,QImage img)
{
    float pixPerDegW=float(img.width()/2)/range;
    float pixPerDegH=float(img.height()/2)/range;
    return QPoint(img.width()/2+DegLoc.x()*pixPerDegW,img.height()/2-DegLoc.y()*pixPerDegH);
}

//QPointF AnalysisSvc::convertPolarToOrth(QPointF loc)
//{
//    auto radius=loc.x();
//    auto angle=loc.y();
//    auto x=radius*qCos(angle/180*M_PI);
//    auto y=radius*qSin(angle/180*M_PI);
//    return QPointF{x,y};
//}


void AnalysisSvc::drawBaseLine(QVector<float> mds,int startYear,int endYear,QVector<int> months, QImage &img,bool isReport)
{
    float scale;
    !isReport?scale=1:scale=2;
    img.fill(qRgb(255, 255, 255));
    QPainter painter(&img);
    painter.setBackground(QBrush(QColor("white")));
    painter.setBrush(QBrush(QColor("black")));
    painter.setPen({Qt::black,float(scale)});
    int fontPixSize=img.height()/20;
//    int fontPixSize=3;
    QFont font("consolas");
    font.setPixelSize(fontPixSize);
    painter.setFont(font);

    painter.drawLine(QLine(QPoint(60*scale,0.05*img.height()),QPoint(img.width()-60*scale,0.05*img.height())));  //600*0.8/24= 20year
    painter.drawLine(QLine(QPoint(60*scale,0.05*img.height()),QPoint(60*scale,0.95*img.height())));

    QRect rectangle = QRect(30*scale-fontPixSize*1.8*0.9, 0.5*img.height()-fontPixSize*0.5,fontPixSize*1.9, fontPixSize*0.8);
    painter.drawText(rectangle,Qt::AlignCenter|Qt::AlignVCenter,"MD");
    rectangle = QRect(30*scale-fontPixSize*1.8*0.9, 0.5*img.height()+fontPixSize*0.5,fontPixSize*1.9, fontPixSize*0.8);
    painter.drawText(rectangle,Qt::AlignCenter|Qt::AlignVCenter,"(DB)");


    int dbScale=1;
    for(int i=1;i<=16;i++)
    {
        const QRect rectangle = QRect(30*scale+fontPixSize*1.6*0.1, 0.06*img.height()*i-fontPixSize*0.5,fontPixSize*1.6, fontPixSize*0.8);
        painter.drawText(rectangle,Qt::AlignCenter|Qt::AlignVCenter,QString::number(dbScale));
        dbScale--;
    }


    int yearScale=startYear;
    for(int i=0;i<=endYear-startYear;i++)
    {
        const QRect rectangle = QRect(60*scale+24*scale*i-fontPixSize*1.6*0.5, fontPixSize*0.2,fontPixSize*1.2, fontPixSize*0.8);
        painter.drawText(rectangle,Qt::AlignCenter|Qt::AlignVCenter,QString::number(yearScale%100));
        yearScale++;
    }
    rectangle = QRect(img.width()-30*scale-fontPixSize*1.6*0.5, fontPixSize*0.2,fontPixSize*2.3, fontPixSize*0.8);
//    painter.drawText(QPoint(0.93*img.width(),fontPixSize*0.35),"(YEAR)");
    painter.drawText(rectangle,Qt::AlignCenter|Qt::AlignVCenter,"YEAR");

    QPen pen;
    pen.setDashPattern({float(3*scale),float(3*scale)});
    pen.setWidth(scale);
    pen.setColor(Qt::black);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);



    auto getMDPixy=[&](float md)->int{return int((0.05+0.06*(1-md))*img.height());};

    painter.drawLine(QLine(QPoint(60*scale,(0.05+0.06*(1-(-2)))*img.height()),QPoint(img.width()-60*scale,(0.05+0.06*(1-(-2)))*img.height())));
    painter.drawText(QPoint(img.width()-42*scale,(0.05+0.06*(1-(-2)))*img.height()+fontPixSize*0.35),"P<%5");
    painter.drawLine(QLine(QPoint(60*scale,(0.05+0.06*(1-(-3.5)))*img.height()),QPoint(img.width()-60*scale,(0.05+0.06*(1-(-3.5)))*img.height())));
    painter.drawText(QPoint(img.width()-42*scale,(0.05+0.06*(1-(-3.5)))*img.height()+fontPixSize*0.35),"P<%1");


    for(int i=0;i<mds.length();i++)
    {
        if(i!=mds.length()-1) painter.drawLine(QLine(QPoint{int(60*scale+months[i]*2*scale),getMDPixy(mds[i])},QPoint{int(60*scale+months[i+1]*2*scale),getMDPixy(mds[i+1])}));
        float diameter=0.02*img.height();
//        QPoint center={int(0.1*img.width()+months[i]*0.8*img.width()/240),getMDPixy(mds[i])};
        QPoint center={int(60*scale+months[i]*2*scale),getMDPixy(mds[i])};
        painter.setBrush(QBrush(Qt::black));
        QRectF rect(center.x()-diameter/2,center.y()-diameter/2,diameter,diameter);
        painter.drawEllipse(rect);
    }

//    for(int i=0;i<mds.length();i++)
//    {
//        if(i!=mds.length()-1) painter.drawLine(QLine(QPoint{int(60*scale+months[i]*20*scale),getMDPixy(mds[i])},QPoint{int(60*scale+months[i+1]*20*scale),getMDPixy(mds[i+1])}));
//        float diameter=0.02*img.height();
//        QPoint center={int(60*scale+months[i]*20*scale),getMDPixy(mds[i])};
//        painter.setBrush(QBrush(Qt::black));
//        QRectF rect(center.x()-diameter/2,center.y()-diameter/2,diameter,diameter);
//        painter.drawEllipse(rect);
//    }


}

//int AnalysisSvc::getIndex(const QPointF &dot, const QVector<QPointF> &pointLoc)
//{
//    int index=-1;
//    int distMin=INT_MAX;
//    for(int i=0;i<pointLoc.length();i++)
//    {
//        int dist=pow(pointLoc[i].x()-dot.x(),2)+pow(pointLoc[i].y()-dot.y(),2);
//        if(dist<FLT_EPSILON){index=i;break;}
//        else if(dist<distMin){distMin=dist;index=i;}
//    }
//    return index;
//}


//int AnalysisSvc::getIndex(const QPointF& dot,const QVector<QPoint> &pointLoc, int OS_OD)
//{
//    int index=-1;
//    int distMin=INT_MAX;

//    for(int i=0;i<pointLoc.length();i++)
//    {
//        int dist;
//        if(OS_OD==0)
//        {
//            dist=pow(pointLoc[i].x()-dot.x(),2)+pow(pointLoc[i].y()-dot.y(),2);
//        }
//        else
//        {
//            dist=pow(pointLoc[i].x()-(-dot.x()),2)+pow(pointLoc[i].y()-dot.y(),2);
//        }
//        if(dist<FLT_EPSILON)
//        {index=i;break;}
//        else if(dist<distMin){distMin=dist;index=i;}
//    }
//    return index;
//}

//int AnalysisSvc::getIndex(const QPointF& dot,const QVector<QPointF> &pointLoc, int OS_OD)
//{
//    int index=-1;

//    for(int i=0;i<pointLoc.length();i++)
//    {
//        int dist;
//        if(OS_OD==0)
//        {
//            dist=pow(pointLoc[i].x()-dot.x(),2)+pow(pointLoc[i].y()-dot.y(),2);
//        }
//        else
//        {
//            dist=pow(pointLoc[i].x()-(-dot.x()),2)+pow(pointLoc[i].y()-dot.y(),2);
//        }
//        if(dist<FLT_EPSILON){index=i;break;}
//    }
//    return index;
//}

int AnalysisSvc::getpeMDev(int MDev,QPointF loc,int OS_OD)
{
    auto index=UtilitySvc::getSingleton()->getIndex(loc,m_pointLoc_30d,OS_OD);
    int peMDev;

    int v=-MDev;
    if (v<=m_pe_v5[0][index]) peMDev=0;
    else if(v<=m_pe_v2[0][index]) peMDev=1;
    else if(v<=m_pe_v1[0][index]) peMDev=2;
    else if(v<=m_pe_v05[0][index]) peMDev=3;
    else peMDev=4;

    return peMDev;
}





//void AnalysisSvc::dynamicAnalysis()
//{

//}

}
