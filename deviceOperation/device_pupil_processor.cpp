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

namespace DevOps{
// void DevicePupilProcessor::find_point(uchar* data, int width, int height)
// {
//     QImage image(data,width,height,QImage::Format_Grayscale8);
//     QVector<QPoint>  point_vec;    //0:瞳孔中心点  1,2,3:三个反光点
//     int  center_radius;
//     m_pupilResValid=false;

//     QVector<QPoint> result;
//     cv::Mat gray_image = cv::Mat(image.height(), image.width(), CV_8UC1, image.bits(), image.bytesPerLine());
//     std::vector<cv::Vec3f> circles;
//     cv::HoughCircles(gray_image, circles, cv::HOUGH_GRADIENT, 1, 100, 60, 30, 15, 50);

//     QPoint pupil_center;
//     int pupil_radius = 0;

//     for (auto circle : circles) {
//         pupil_center = QPoint(cvRound(circle[0]), cvRound(circle[1]));
//         pupil_radius = cvRound(circle[2]);
//         break;
//     }

//     center_radius = pupil_radius;

//     result.append(pupil_center);

//     if (pupil_radius > pupil_center.x() || pupil_radius > pupil_center.y()) {
//         return;
//     }

//     cv::Rect pupil_roi_rect(pupil_center.x() - pupil_radius, pupil_center.y() - pupil_radius, pupil_radius * 2, pupil_radius * 2);

//     cv::Mat pupil_roi = gray_image(pupil_roi_rect);
//     double pupil_max_val = 0;
//     double image_mean_val = cv::mean(gray_image).val[0];

//     cv::minMaxLoc(pupil_roi, nullptr, &pupil_max_val);

//     double pupil_threshold = (pupil_max_val + image_mean_val) / 2;

//     cv::Mat threshold_image;

//     cv::threshold(gray_image, threshold_image, pupil_threshold, 255, cv::THRESH_BINARY_INV);

//     if (threshold_image.empty()) {
//         return;
//     }

//     std::vector<cv::Mat> contours;
//     cv::findContours(threshold_image, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

//     contours = std::vector<cv::Mat>(contours.begin() + 1, contours.end());

//     if (contours.empty()) {
//         return;
//     }

//     for (auto cnt : contours) {
//         auto rect = cv::boundingRect(cnt);
//         QPoint point(rect.x + int(rect.width / 2), rect.y + int(rect.height / 2));
//         result.append(point);
//     }

//     point_vec = std::move(result);


//     m_pupilDeviation=13;
//     m_pupilResValid=(point_vec.count()>0);
//     if(m_pupilResValid)
//     {
//         m_pupilCenterPoint=point_vec[0];
//         m_pupilDiameterPix=center_radius*2;

//         auto pupilDiameter=m_pupilDiameterPix*DeviceSettings::getSingleton()->m_pupilDiameterPixelToMillimeterConstant*320/width;
//         if(m_pupilDiameterArr.length()>=20) m_pupilDiameterArr.pop_front();
//         m_pupilDiameterArr.push_back(pupilDiameter);
//         {
//             float sum=0;
//             for(auto&i:m_pupilDiameterArr) sum+=i;
//             m_pupilDiameter=sum/m_pupilDiameterArr.size();
// //            std::cout<<m_pupilDiameter<<std::endl;
//         }

// //        auto vcReflectionDot=caculateReflectingDot(data,width,height);
//         m_reflectionResValid=(point_vec.length()==4);


//         qDebug()<<"******************************";
//         qDebug()<<m_reflectionResValid;
//         qDebug()<<"******************************";
//         if(m_reflectionResValid)
//         {
// //            std::cout<<"find reflectionDot."<<std::endl;
//             m_reflectionDot={point_vec[1],point_vec[2],point_vec[3]};
//             m_pupilDeviation=caculateFixationDeviation(m_pupilCenterPoint,m_reflectionDot);
//         }
//     }
// }

DevicePupilProcessor::DevicePupilProcessor()
{

}

void DevicePupilProcessor::processData(uchar* data, int width, int height)
{
    auto vcPupil=caculatePupil(data,width,height);
    m_pupilResValid=vcPupil.length()>0;
    m_pupilDeviation=13;
    if(m_pupilResValid)
    {
        m_pupilCenterPoint=vcPupil[0].toPoint();
        m_pupilDiameterPix=caculatePupilDiameter(vcPupil[1],vcPupil[2]);

        auto pupilDiameter=m_pupilDiameterPix*DeviceSettings::getSingleton()->m_pupilDiameterPixelToMillimeterConstant*320/width;
        if(m_pupilDiameterArr.length()>=20) m_pupilDiameterArr.pop_front();
        m_pupilDiameterArr.push_back(pupilDiameter);
        {
            float sum=0;
            for(auto&i:m_pupilDiameterArr) sum+=i;
            m_pupilDiameter=sum/m_pupilDiameterArr.size();
//            std::cout<<m_pupilDiameter<<std::endl;
        }

        auto vcReflectionDot=caculateReflectingDot(data,width,height);
        m_reflectionResValid=vcReflectionDot.length()>0;
        qDebug()<<"******************************";
        qDebug()<<m_reflectionResValid;
        qDebug()<<"******************************";
        if(m_reflectionResValid)
        {
//            std::cout<<"find reflectionDot."<<std::endl;
            m_reflectionDot={vcReflectionDot[0].toPoint(),vcReflectionDot[1].toPoint(),vcReflectionDot[2].toPoint()};
            m_pupilDeviation=caculateFixationDeviation(m_pupilCenterPoint,vcReflectionDot);
        }
    }
}

QVector<QPointF> DevicePupilProcessor::caculatePupil(uchar* data, int width, int height)
{
    auto pupilPixelDiameterMaxLimit=DeviceSettings::getSingleton()->m_pupilPixelDiameterMaxLimit;
    auto pupilPixelDiameterMinLimit=DeviceSettings::getSingleton()->m_pupilPixelDiameterMinLimit;
    auto pupilGreyLimit=m_pupilGreyLimit;
    auto reflectionDotLimit=m_pupilReflectionDotWhiteLimit;
    float y_max=0;
    float y_min=FLT_MAX;
    float x_max=0;
    float x_min=FLT_MAX;
    QVector<QPoint> vc;              //黑点
    QVector<QPoint> vc2;             //有效黑点
    for(int y=height*0.35;y<height*0.65;y++)
    {
        int x_min=INT_MAX;
        int x_max=0;
        for(int x=width*0.35;x<width*0.65;x++)
        {
            int gapCount=0;
            QVector<QPoint> vc_line;             //每一行
            if(quint8(data[x+width*y])<pupilGreyLimit)  //黑点或者反光点
            {
//                if(x>x_max) x_max=x;
//                if(x<x_min) x_min=x;
//                if((vc_line.length()>0&&x-vc_line.last().x()>2)&&(data[qRound(float(x+vc_line.last().x())/2)+width*y]<reflectionDotLimit)) gapCount++;
//                if(x_max-x_min<width*pupilPixelDiameterMaxLimit&&gapCount<=2)                //不能太大和有太多空隙,排除眉毛
//                {
                    vc_line.push_back({x,y});
//                }
//                else
//                {
//                    vc_line.clear();
//                }
            }
             vc.append(vc_line);
        }
    }


    if(vc.length()==0)
    {
        return {};
    }

    int x_sum=0,y_sum=0;
    for(int i=0;i<vc.length();i++)
    {
        x_sum+=vc[i].x();
        y_sum+=vc[i].y();
    }
    float x_avg=x_sum/vc.length();
    float y_avg=y_sum/vc.length();

    double pupilDiameterEstimated=sqrt(vc.length()/M_PI*4);
    if(pupilDiameterEstimated<width*pupilPixelDiameterMinLimit||pupilDiameterEstimated>width*pupilPixelDiameterMaxLimit)            //太大太小的不是瞳孔
        return {};

    for(int i=0;i<vc.length();i++)        //刨开太远的
    {
        auto pix=vc[i];
        if(qAbs(int(pix.x()-x_avg))<pupilDiameterEstimated*0.55&&qAbs(int(pix.y()-y_avg))<pupilDiameterEstimated*0.55)
        {
            if(pix.x()>x_max) x_max=pix.x();
            if(pix.x()<x_min) x_min=pix.x();
            if(pix.y()>y_max) y_max=pix.y();
            if(pix.y()<y_min) y_min=pix.y();
            vc2.push_back(pix);
        }
    }

    if(vc2.length()==0)
    {
        return {};
    }

    x_sum=0,y_sum=0;
    for(int i=0;i<vc2.length();i++)
    {
        x_sum+=vc2[i].x();
        y_sum+=vc2[i].y();
    }

    float x_avg2=x_sum/vc2.length();
    float y_avg2=y_sum/vc2.length();

    QPointF center={x_avg2,y_avg2};
    QPointF topLeft={x_min,y_min};
    QPointF bottomRight={x_max,y_max};
//   if((x_max-x_min<width*pupilPixelDiameterMaxLimit&&y_max-y_min<width*pupilPixelDiameterMaxLimit)&&(x_max-x_min>width*pupilPixelDiameterMinLimit&&y_max-y_min>width*pupilPixelDiameterMinLimit))            //太大太小的不是瞳孔
//    {
        return QVector<QPointF>{center,topLeft,bottomRight};
//    }
//    else
//    {
//        return {};
//    }
}

QVector<QPointF> DevicePupilProcessor::caculateReflectingDot(uchar* ba, int width, int height)
{
    QVector<QPoint> brightPix,leftBrightPix,middleBrightPix,rightBrightPix;
    for(int y=m_pupilCenterPoint.y()-height*0.12;y<m_pupilCenterPoint.y()+height*0.08;y++)
    {
        for(int x=m_pupilCenterPoint.x()-width*0.06;x<m_pupilCenterPoint.x()+width*0.06;x++)
        {
//            qDebug()<<quint8(ba[x+width*y]);
            if(quint8(ba[x+width*y])>m_pupilReflectionDotWhiteLimit)
            {
                brightPix.append({x,y});
//                x_vc.push_back(x);
//                y_vc.push_back(y);
//                validCount++;
            }
        }
    }
//    qDebug()<<brightPix;

    if(brightPix.isEmpty()) return {};

    std::sort(brightPix.begin(),brightPix.end(),[](QPoint a,QPoint b){return a.x()<b.x();});

    leftBrightPix.append(brightPix[0]);
    for(int i=0;i<brightPix.length()-1;i++)
    {
        int dist=pow((brightPix[i].x()-brightPix[i+1].x()),2)+pow((brightPix[i].y()-brightPix[i+1].y()),2);
        if(middleBrightPix.isEmpty())
        {
            if(dist<5*5)
            {
                leftBrightPix.append(brightPix[i+1]);
            }
            else if(dist>15*15)
            {
                if(leftBrightPix.length()>4)
                    middleBrightPix.append(brightPix[i+1]);
                else
                {
                    leftBrightPix.clear();
                    leftBrightPix.append(brightPix[i+1]);
                }
            }
        }
        else if(rightBrightPix.isEmpty())
        {
            if(dist<3*3)
            {
                middleBrightPix.append(brightPix[i+1]);
            }
            else if(dist>15*15)
            {
                if(middleBrightPix.length()>4)
                    rightBrightPix.append(brightPix[i+1]);
                else
                {
                    middleBrightPix.clear();
                    middleBrightPix.append(brightPix[i+1]);
                }
            }
        }
        else
        {
            if(dist<3*3)
            {
                rightBrightPix.append(brightPix[i+1]);
            }
            else if(dist>15*15)
            {
                rightBrightPix.clear();
                rightBrightPix.append(brightPix[i+1]);
            }

        }
    }
    if(!leftBrightPix.isEmpty()&&!middleBrightPix.isEmpty()&&!rightBrightPix.isEmpty())
    {
       QPointF left,middle,right;
       int sumX=0,sumY=0;
       for(auto&i:leftBrightPix)
       {
           sumX+=i.x();
           sumY+=i.y();
       }
       left.rx()=sumX/leftBrightPix.length();
       left.ry()=sumY/leftBrightPix.length();

       sumX=0,sumY=0;
       for(auto&i:middleBrightPix)
       {
           sumX+=i.x();
           sumY+=i.y();
       }
       middle.rx()=sumX/middleBrightPix.length();
       middle.ry()=sumY/middleBrightPix.length();

       sumX=0,sumY=0;
       for(auto&i:rightBrightPix)
       {
           sumX+=i.x();
           sumY+=i.y();
       }
       right.rx()=sumX/rightBrightPix.length();
       right.ry()=sumY/rightBrightPix.length();
       return {left,middle,right};
    }
    else
    {
        return {};
    }
}


float DevicePupilProcessor::caculatePupilDiameter(QPointF topLeft,QPointF bottomRight)
{
//    qDebug()<<"***********************************";
//    qDebug()<<topLeft;
//    qDebug()<<bottomRight;
    auto width=bottomRight.x()-topLeft.x();
    auto height=bottomRight.y()-topLeft.y();
    auto pixelDiameter=sqrt(width*height);
//    auto diameter=pixelDiameter*DeviceSettings::getSingleton()->m_pupilDiameterPixelToMillimeterConstant;
    return pixelDiameter;
}

int DevicePupilProcessor::caculateFixationDeviation(QPointF pupil, QVector<QPointF> reflectionDot)
{
    auto middleDot=reflectionDot[1];
    double distX=pupil.x()-middleDot.x();
    double distY=pupil.y()-(middleDot.y()-DeviceSettings::getSingleton()->m_pixelDistFromPupilCenterToMiddleReflectionDot);
    auto deviation=qRound(sqrt(distX*distX+distY*distY)*DeviceSettings::getSingleton()->m_pupilDeviationPixelToNumberConstant);
    return deviation;
}

void DevicePupilProcessor::clearData()
{
    m_pupilDiameter=0;
    m_pupilDiameterArr.clear();
}

}


























