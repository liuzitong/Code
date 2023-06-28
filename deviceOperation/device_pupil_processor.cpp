#include "device_pupil_processor.h"
#include <iostream>
#include <QDebug>
#include <QtMath>
namespace DevOps{
DevicePupilProcessor::DevicePupilProcessor()
{

}

void DevicePupilProcessor::processData(QByteArray data)
{
    int px,py,pdx,pdy,pwx,pwy;
    double vx,vy,vdx,vdy,vwx,vwy;
    auto res=findPupil(data);
    QVector<double> avg;
    if(!res.isEmpty())
    {
        px=res[0];py=res[1];pdx=res[2];pdy=res[3];pwx=res[4];pwy=res[5];
        m_pupilData.push_back(res);
        m_pupilDeviation=qAbs(getEyeMove(px,py,pdx,pdy,pwx,pwy));
    }
    if(!res.isEmpty()&&pdx>=10&&pdx<=71)
    {
        if(m_pupilData.length()>10) m_pupilData.pop_front();
        for(int i=0;i<6;i++)
        {
            int sum=0;
            for(int j=0;j<m_pupilData.length();j++)
            {
                sum+=m_pupilData[j][i];
            }
            avg.push_back(double(sum)/m_pupilData.length());
        }
        vx=avg[0];vy=avg[1];vdx=avg[2];vdy=avg[3];vwx=avg[4];vwy=avg[5];
        m_pupilDiameter=vdx*DeviceSettings::getSingleton()->m_pupilDiameterPixelToMillimeterConstant;
    }
}

QVector<int> DevicePupilProcessor::findPupil(QByteArray data)
{
    int px,py,pdx,pdy,pwx,pwy;
    auto pupilGreyLimit=DeviceSettings::getSingleton()->m_pupilGreyLimit;
    auto dataPupil=DataToBlackAndWhite(data,pupilGreyLimit);
    QVector<int> pupilFindRes;
    for(int w=50;w<270;w++)
    {
        for(int h=50;h<190;h++)
        {
            auto val=dataPupil.at(h*320+w);
            if(val==0)
            {
                auto res=findPupilAtXY(dataPupil,w,h);
                if(!res.isEmpty())
                {
                    pupilFindRes=res;
                    break;
                }
            }
        }
        if(!pupilFindRes.isEmpty()) break;
    }
    if(pupilFindRes.isEmpty()){return {};}

    px=pupilFindRes[0];py=pupilFindRes[1];pdx=pupilFindRes[2];pdy=pupilFindRes[3];
    int x1=(px+py+pdx)/2;
    int y1=py+50;
    auto dataWhiteDot=DataToBlackAndWhite(data,250);
    QVector<int> res1;
    QVector<int> res2;
    for(int y=py+10;y<=y1;y++)
    {
        for(int x=px-30;x<270;)
        {
            res1=findWhiteDot(dataWhiteDot,x,y,x1,y1);
            if(!res1.isEmpty())
            {
                int x0=res1[0];
                int y0=res1[1];
                x=x0+1;
                res2=findWhiteDot(dataWhiteDot,x0+20,y0-2,x0+40,y0+2);
                if(!res2.isEmpty())
                {

                    x1=res2[0];
                    y1=res2[1];
                    pwx=(x0+x1)/2;
                    pwy=(y0+y1)/2;
                    return {px,py,pdx,pdy,pwx,pwy};
                }
            }
            else
            {
                return {};
            }
        }
    }
    return {};
}

QByteArray DevicePupilProcessor::DataToBlackAndWhite(QByteArray data, int value)
{
    uchar* data_ptr=(uchar*)data.data();
    for(int i=0;i<data.length();i++)
    {
        if(data_ptr[i]>value)
            data_ptr[i]=255;
        else
            data_ptr[i]=0;
    }
    return data;
}

double DevicePupilProcessor::getEyeMove(double x, double y, double dx, double dy, double wx, double wy)
{
    return 0.0;
}

QVector<int> DevicePupilProcessor::findPupilAtXY(QByteArray data, int x, int y)
{
    auto y1=findWhiteY(data,x,y,1);
    if(y1-y>15||y==-1) return {};
    if(!isWhiteYLine(data,x-1,(y+y1)/2-10)) return {};
    auto x1=findWhiteYLine(data,x,(y+y1)/10);
    if(x1==-1) return{};
    int n;
    for(int i=0;i<10;i++)
        for(int j=0;j<10;j++)
            if((uchar)data[(y+j)*320+x+i]==0)
                n++;
    if(n<70) return {};
    if(!pupilFeature(data,x,y,x1,y1)) return {};

    int px=x;
    int pdx=x1-x;
    int py=(y+y1-pdx)/2;
    int pdy=pdx;
    return {px,py,pdx,pdy};

}

int DevicePupilProcessor::findWhiteY(QByteArray data, int x, int y,int step)
{
    for(int i=0;i<40;i++)
    {
        if((uchar)data[y*320+x]==255)
        {
            return y;
        }
        else y+=step;
    }
    return -1;
}

int DevicePupilProcessor::findWhiteYLine(QByteArray data,int x,int y)
{
    for(int i=0;i<40;i++)
    {
        if(isWhiteYLine(data,x+i,y))
        {
            if(i<10) return -1;
            if(i>=10) return x+i;
        }
    }
    return -1;
}

QVector<int> DevicePupilProcessor::findWhiteDot(QByteArray data, int x, int y, int x1, int y1)
{
    return {};
}


bool DevicePupilProcessor::isWhiteYLine(QByteArray data, int x, int y)
{
    int n=0;
    for(int i=0;i<20;i++)
    {
        if((uchar)data[(y+i)*320+x]==0) n++;
    }
    if(n<=2) return true;
    else return false;
}

bool DevicePupilProcessor::pupilFeature(QByteArray data, int x, int y, int x1, int y1)
{
    return false;
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
            m_pupilDeviation=caculateFixationDeviation(vcPupil,vcReflectionDot);
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
    for(int y=height*0.3;y<height*0.7;y++)
    {
        int x_min=INT_MAX;
        int x_max=0;
        for(int x=width*0.2;x<width*0.8;x++)
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
    for(int y=m_pupilCenterPoint.y()-height*0.08;y<m_pupilCenterPoint.y()+height*0.10;y++)
    {
        for(int x=m_pupilCenterPoint.x()-width*0.08;x<m_pupilCenterPoint.x()+width*0.08;x++)
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

int DevicePupilProcessor::caculateFixationDeviation(QVector<QPointF> pupil, QVector<QPointF> reflectionDot)
{
    auto middleDot=reflectionDot[1];
    double distX=pupil[0].x()-middleDot.x();
    double distY=pupil[0].y()-(middleDot.y()-DeviceSettings::getSingleton()->m_pixelDistFromPupilCenterToMiddleReflectionDot);
    auto deviation=qRound(sqrt(distX*distX+distY*distY)*DeviceSettings::getSingleton()->m_pupilDeviationPixelToNumberConstant);
    return deviation;
}

void DevicePupilProcessor::clearData()
{
    m_pupilDiameter=0;
    m_pupilDiameterArr.clear();
}

}


























