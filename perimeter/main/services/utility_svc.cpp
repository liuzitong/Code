#include "utility_svc.h"
#include <QtMath>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QDebug>
#include <QElapsedTimer>
#include <QThread>
#include <QApplication>

namespace Perimeter
{


UtilitySvc::UtilitySvc()
{
    QFile jsonFile("./data.json");
    if( !jsonFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "read file error!";
    }
    QJsonParseError jsonParserError;
    auto JsonDoc = QJsonDocument::fromJson(jsonFile.readAll(),&jsonParserError);
    auto jsonArray=JsonDoc.array();
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


    for(auto i:jsonArray)
    {
        auto jo=i.toObject();

        jsonArrToVectorPoint(m_pointLoc_30d,"XY_NORMAL_VALUE_30d",jo);
        jsonArrToVectorPoint(m_pointLoc_60d,"XY_NORMAL_VALUE_60d",jo);

        QVector<QVector<QString>> jsonObjNames={
            {"NORMAL_VALUE36_45_B1_White","NORMAL_VALUE36_45_B1_Red","NORMAL_VALUE36_45_B1_Blue"},
            {"NORMAL_VALUE36_45_B2_White","NORMAL_VALUE36_45_B2_Red","NORMAL_VALUE36_45_B2_Blue"},
            {/*B3_WHITE采用该jsonObjNames2*/"NORMAL_VALUE36_45_B3_Red","NORMAL_VALUE36_45_B3_Blue"},
            {"NORMAL_VALUE36_45_B4_White","NORMAL_VALUE36_45_B4_Red","NORMAL_VALUE36_45_B4_Blue"},
            {"NORMAL_VALUE36_45_B5_White","NORMAL_VALUE36_45_B5_Red","NORMAL_VALUE36_45_B5_Blue"}
        };

        QVector<QString> jsonObjNames2={"NORMAL_VALUE15_35","NORMAL_VALUE36_45","NORMAL_VALUE46_55","NORMAL_VALUE56_65","NORMAL_VALUE66_75"};

        m_value_30d_cursorSize_cursorColor.resize(jsonObjNames.length());
        for(int i=0;i<jsonObjNames.length();i++)
        {
            m_value_30d_cursorSize_cursorColor[i].resize(jsonObjNames[i].length());
            for(int j=0;j<jsonObjNames[i].length();j++)
            {
                 jsonArrToVectorInt(m_value_30d_cursorSize_cursorColor[i][j],jsonObjNames[i][j],jo);
            }
        }

        m_value_30d_cursorSizeIII_ageCorrection.resize(jsonObjNames2.length());
        for(int i=0;i<jsonObjNames2.length();i++)
        {
             jsonArrToVectorInt(m_value_30d_cursorSizeIII_ageCorrection[i],jsonObjNames2[i],jo);
        }

        jsonArrToVectorInt(m_value_60d,"NORMAL_VALUE15_35_60d",jo);

    }
}

QPointF UtilitySvc::convertPolarToOrth(QPointF loc)
{
    auto radius=loc.x();
    auto angle=loc.y();
    auto x=radius*qCos(angle/180*M_PI);
    auto y=radius*qSin(angle/180*M_PI);
    return QPointF{x,y};
}

int UtilitySvc::getIndex(const QPointF &dot, const QVector<QPointF> &pointLoc)
{
    int index=-1;
    int distMin=INT_MAX;
    for(int i=0;i<pointLoc.length();i++)
    {
        int dist=pow(pointLoc[i].x()-dot.x(),2)+pow(pointLoc[i].y()-dot.y(),2);
        if(dist<FLT_EPSILON){index=i;break;}
        else if(dist<distMin){distMin=dist;index=i;}
    }
    return index;
}

int UtilitySvc::getIndex(const QPointF &dot, const QVector<QPoint> &pointLoc, int OS_OD)
{
    int index=-1;
    int distMin=INT_MAX;

    for(int i=0;i<pointLoc.length();i++)
    {
        int dist;
        if(OS_OD==0)
        {
            dist=pow(pointLoc[i].x()-dot.x(),2)+pow(pointLoc[i].y()-dot.y(),2);
        }
        else
        {
            dist=pow(pointLoc[i].x()-(-dot.x()),2)+pow(pointLoc[i].y()-dot.y(),2);
        }
        if(dist<FLT_EPSILON)
        {index=i;break;}
        else if(dist<distMin){distMin=dist;index=i;}
    }
    return index;
}


int UtilitySvc::getIndex(const QPointF &dot, const QVector<QPointF> &pointLoc, int OS_OD)
{
    int index=-1;

    for(int i=0;i<pointLoc.length();i++)
    {
        int dist;
        if(OS_OD==0)
        {
            dist=pow(pointLoc[i].x()-dot.x(),2)+pow(pointLoc[i].y()-dot.y(),2);
        }
        else
        {
            dist=pow(pointLoc[i].x()-(-dot.x()),2)+pow(pointLoc[i].y()-dot.y(),2);
        }
        if(dist<FLT_EPSILON){index=i;break;}
    }
    return index;
}

void UtilitySvc::wait(int msecs)
{
    QElapsedTimer elapsedTimer;
    elapsedTimer.restart();
    while(elapsedTimer.elapsed()<=msecs)
    {
        QApplication::processEvents();
    }
}

int UtilitySvc::getExpectedDB(const QVector<int> &value_30d, QPointF loc,int OS_OD)
{
    int val;
    if(loc.x()<=30&&loc.y()<=30)
    {
        int index=getIndex(loc,m_pointLoc_30d,OS_OD);
        val=value_30d[index];
    }
    else
    {
        int index=getIndex(loc,m_pointLoc_60d,OS_OD);
        val=m_value_60d[index];
    }
    return val;
}

QVector<int> UtilitySvc::getValue30d(int cursorSize, int cursorColor, int age)
{
    int age_correction;

    if(age<=35){age_correction=0;}
    else if(age<=45){age_correction=1;}
    else if(age<=55){age_correction=2;}
    else if(age<=65){age_correction=3;}
    else {age_correction=4;}

    QVector<int> value_30d;
    if(cursorSize==2)
    {
        if(cursorColor==0)
            value_30d=m_value_30d_cursorSizeIII_ageCorrection[age_correction];
        else
            value_30d=m_value_30d_cursorSize_cursorColor[cursorSize][cursorColor-1];
    }
    else
        value_30d=m_value_30d_cursorSize_cursorColor[cursorSize][cursorColor];

    return value_30d;
}

QSharedPointer<UtilitySvc> UtilitySvc::getSingleton()
{
    if(singleton.data()==nullptr)
    {
        singleton.reset(new UtilitySvc());
    }
    return singleton;
}

QSharedPointer<UtilitySvc> UtilitySvc::singleton=QSharedPointer<UtilitySvc>(nullptr);

}
