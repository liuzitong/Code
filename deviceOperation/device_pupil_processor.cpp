#include "device_pupil_processor.h"
#include <iostream>
#include <QDebug>
#include <QtMath>
// #include <opencv2/core.hpp>
// #include <opencv2/imgproc.hpp>
#include <QVector>
#include <QPoint>
#include <QImage>
#include <QDebug>
#include <pupilDetectApi.hxx>

namespace DevOps{

DevicePupilProcessor::DevicePupilProcessor(){}

void DevicePupilProcessor::processData(void* result)
{
    Result* res=static_cast<Result*>(result);
    m_pupilResValid=res->pupil.center.x>0;
    m_pupilDeviation=13;
    m_isTooFar=false;
    if(m_pupilResValid)
    {
        double diameterPix=sqrt(pow(res->pupil.long_axis,2)+pow(res->pupil.short_axis,2));
        auto pupilDiameter=diameterPix*DeviceSettings::getSingleton()->m_pupilDiameterPixelToMillimeterConstant;
        if(m_pupilDiameterArr.length()>=20) m_pupilDiameterArr.pop_front();
        m_pupilDiameterArr.push_back(pupilDiameter);
        {
            float sum=0;
            for(auto&i:m_pupilDiameterArr) sum+=i;
            m_pupilDiameter=sum/m_pupilDiameterArr.size();
        }

        int m_refectionResValidCount=0;

        for(auto& i:res->reflectDots)
        {
            if(i.x>0) m_refectionResValidCount++;
        }


        m_reflectionResValid=(m_refectionResValidCount==3);

        if(m_reflectionResValid)
        {
            m_pupilDeviation=caculateFixationDeviation(res);
            m_isTooFar=caculateIsTooFar(res);
        }
    }
}




int DevicePupilProcessor::caculateFixationDeviation(void* result)
{

    Result* res=static_cast<Result*>(result);
    QVector<Point> dots={res->reflectDots[0],res->reflectDots[1],res->reflectDots[2]};
    std::sort(dots.begin(),dots.end(),[&](Point p1,Point p2){return p1.x<p2.x;});

    auto middleDot=dots[1];
    double distX=res->pupil.center.x-middleDot.x;
    double distY=res->pupil.center.y-(middleDot.y-DeviceSettings::getSingleton()->m_pixelDistFromPupilCenterToMiddleReflectionDot);
    auto deviation=qRound(sqrt(distX*distX+distY*distY)*DeviceSettings::getSingleton()->m_pupilDeviationPixelToNumberConstant);
    return deviation;
}

bool DevicePupilProcessor::caculateIsTooFar(void *result)
{

    Result* res=static_cast<Result*>(result);
    QVector<Point> dots={res->reflectDots[0],res->reflectDots[1],res->reflectDots[2]};
    std::sort(dots.begin(),dots.end(),[&](Point p1,Point p2){return p1.x<p2.x;});
    int dist=dots[2].x-dots[0].x;
    return dist<DeviceSettings::getSingleton()->m_reflectDotDist;
}

void DevicePupilProcessor::clearData()
{
    m_pupilDiameter=0;
    m_pupilDiameterArr.clear();
}
}


























