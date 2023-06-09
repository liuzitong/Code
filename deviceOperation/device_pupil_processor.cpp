#include "device_pupil_processor.h"

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

    px=pupilFindRes[0];py=pupilFindRes[1];pdx=pupilFindRes[2];pdy=pupilFindRes[3];pwx=pupilFindRes[4];pwy=pupilFindRes[5];
    int x1=(px+py+pdx)/2;
    int y1=py+50;
    auto dataWhiteDot=DataToBlackAndWhite(data,250);
    QVector<int> res1;
    QVector<int> res2;
    for(int y=py+10;y<=y1;y++)
    {
        for(int x=px-30;x<270;)
        {
            res1=findWhiteDot(dataWhiteDot,{x,y},{x1,y1});
            if(!res1.isEmpty())
            {
                int x0=res1[0];
                int y0=res1[1];
                x=x0+1;
                res2=findWhiteDot(dataWhiteDot,{x0+20,y0-2},{x0+40,y0+2});
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
}
