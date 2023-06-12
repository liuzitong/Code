#include "device_pupil_processor.h"
#include <iostream>
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

void DevicePupilProcessor::processData(QByteArray data, int width, int height)
{
    auto vcPupil=caculatePupil(data,width,height);
    m_pupilResValid=vcPupil.length()>0;
    if(m_pupilResValid)
    {
        m_pupilCenterPoint=vcPupil[0].toPoint();
        m_pupilRadius=caculatePupilDiameter(vcPupil[1],vcPupil[2])/2;
        if(m_pupilDiameter==0)
        {
            auto pupilDiameter=m_pupilRadius*DeviceSettings::getSingleton()->m_pupilDiameterPixelToMillimeterConstant*320/width;
            m_pupilDiameterArr.push_back(pupilDiameter);
            {
                float sum=0;
                for(auto&i:m_pupilDiameterArr) sum+=i;
                m_pupilDiameter=sum/m_pupilDiameterArr.size();
                m_pupilDiameterArr.clear();
                std::cout<<m_pupilDiameter<<std::endl;
            }
        }
        auto vcReflectionDot=caculateReflectingDot(data,width,height);
        m_reflectionResValid=vcReflectionDot.length()>0;
        if(m_reflectionResValid)
        {
            m_reflectionDot={vcReflectionDot[0].toPoint(),vcReflectionDot[1].toPoint(),vcReflectionDot[2].toPoint()};
            m_pupilDeviation=caculateFixationDeviation(vcPupil,vcReflectionDot);
        }
    }
}

QVector<QPointF> DevicePupilProcessor::caculatePupil(const QByteArray ba, int width, int height)
{
    int y_max=0;
    int y_min=INT_MAX;
    int x_max=0;
    int x_min=INT_MAX;
    QVector<int> x_vc;              //黑点
    QVector<int> y_vc;
    QVector<int> x_vc2;             //有效黑点
    QVector<int> y_vc2;
    auto pupilGreyLimit=DeviceSettings::getSingleton()->m_pupilGreyLimit;
    auto pupilPixelDiameterLimit=DeviceSettings::getSingleton()->m_pupilPixelDiameterLimit;
    int validCount=0;
    for(quint32 y=height*0.35;y<height*0.65;y++)
    {
        for(quint32 x=width*0.35;x<width*0.65;x++)
        {
//            qDebug()<<quint8(ba[x+width*y]);
            if(quint8(ba[x+width*y])<pupilGreyLimit)
            {
                x_vc.push_back(x);
                y_vc.push_back(y);
                validCount++;
            }
        }
    }

//    std::cout<<validCount<<std::endl;
    if(!(validCount>width*height*0.05*0.05&&validCount<=width*height*0.2*0.2))
    {
        return {};
    }

    float x_avg,y_avg,sum=0;
    for(int i=0;i<x_vc.length();i++)
    {
        sum+=x_vc[i];
    }
    x_avg=sum/x_vc.length();
    sum=0;
    for(int i=0;i<y_vc.length();i++)
    {
        sum+=y_vc[i];
    }
    y_avg=sum/y_vc.length();

    for(int i=0;i<x_vc.length();i++)        //刨开太远的
    {
        auto x=x_vc[i];
        if(qAbs(int(x-x_avg))<pupilPixelDiameterLimit)
        {
            if(x>x_max) x_max=x;
            if(x<x_min) x_min=x;
            x_vc2.push_back(x);
        }
//        qDebug()<<x;
    }
//    qDebug()<<x_max;
//    qDebug()<<x_min;
//    qDebug()<<x_vc2;

    for(int i=0;i<y_vc.length();i++)                //刨开太远的
    {
        auto y=y_vc[i];
        if(qAbs(int(y-y_avg))<pupilPixelDiameterLimit)
        {
            if(y>y_max) y_max=y;
            if(y<y_min) y_min=y;
            y_vc2.push_back(y);
        }
//        qDebug()<<y;
//        qDebug()<<y_max;
//        qDebug()<<y_min;
    }

//    qDebug()<<y_vc2;
    sum=0;
    for(int i=0;i<x_vc2.length();i++)
    {
        sum+=x_vc2[i];
    }
    if(x_vc2.length()==0)
    {
        return {};
    }
    float x_avg2=sum/x_vc2.length();
    sum=0;
    for(int i=0;i<y_vc2.length();i++)
    {
        sum+=y_vc2[i];
    }
    float y_avg2=sum/y_vc2.length();
    if(x_vc2.length()==0)
    {
        return {};
    }
//    qDebug()<<x_avg2;
//    qDebug()<<y_avg2;
    QPointF center={x_avg2-width*0.5,y_avg2-height*0.5};
    QPointF topLeft={x_min,y_min};
//    qDebug()<<topLeft;
    QPointF bottomRight={x_max,y_max};
//    qDebug()<<bottomRight;
    return QVector<QPointF>{center,topLeft,bottomRight};
}

QVector<QPointF> DevicePupilProcessor::caculateReflectingDot(const QByteArray ba, int width, int height)
{
    QVector<QPoint> brightPix,leftBrightPix,middleBrightPix,rightBrightPix;
    for(quint32 y=height*0.35;y<height*0.65;y++)
    {
        for(quint32 x=width*0.35;x<width*0.65;x++)
        {
//            qDebug()<<quint8(ba[x+width*y]);
            if(quint8(ba[x+width*y])>250)
            {
                brightPix.append({x,y});
//                x_vc.push_back(x);
//                y_vc.push_back(y);
//                validCount++;
            }
        }
    }

    if(brightPix.isEmpty()) return {};

    std::sort(brightPix.first(),brightPix.last(),[&](QPoint a,QPoint b){return a.x()<b.x();});

    leftBrightPix.append(brightPix[0]);
    for(int i=0;i<brightPix.length()-1;i++)
    {
        if(middleBrightPix.isEmpty())
        {
            if(pow((brightPix[i].x()-brightPix[i+1].x()),2)+pow((brightPix[i].y()-brightPix[i+1].y()),2)<2*2)
            {
                leftBrightPix.append(brightPix[i+1]);
            }
            else
                middleBrightPix.append(brightPix[i+1]);
        }
        else if(rightBrightPix.isEmpty())
        {
            if(pow((brightPix[i].x()-brightPix[i+1].x()),2)+pow((brightPix[i].y()-brightPix[i+1].y()),2)<2*2)
            {
                middleBrightPix.append(brightPix[i+1]);
            }
            else
                rightBrightPix.append(brightPix[i+1]);
        }
        else
        {
            rightBrightPix.append(brightPix[i+1]);
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
    return 0;
}

void DevicePupilProcessor::clearData()
{
    m_pupilDiameter=0;
    m_pupilDiameterArr.clear();
}

}


























