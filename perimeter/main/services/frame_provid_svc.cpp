#include "frame_provid_svc.h"
#include <QImage>
#include <QDebug>
#include <deviceOperation/device_operation.h>
#include <memory>
namespace Perimeter{

QSharedPointer<FrameProvidSvc> FrameProvidSvc::m_singleton=nullptr;

FrameProvidSvc::FrameProvidSvc()
{
    connect(DevOps::DeviceOperation::getSingleton().data(),&DevOps::DeviceOperation::newFrameData,this,&FrameProvidSvc::onNewVideoContentReceived);
}

FrameProvidSvc::~FrameProvidSvc()
{

}

QSharedPointer<FrameProvidSvc> FrameProvidSvc::getSingleton()
{
    if(m_singleton==nullptr)
    {
        m_singleton.reset(new FrameProvidSvc());
    }
    return m_singleton;
}

QAbstractVideoSurface *FrameProvidSvc::videoSurface() const {
    return m_surface;
}

void FrameProvidSvc::setVideoSurface(QAbstractVideoSurface *surface)
{
    if (m_surface && m_surface != surface  && m_surface->isActive()) {
        m_surface->stop();
    }

    m_surface = surface;

    if (m_surface && m_format.isValid())
    {
        m_format = m_surface->nearestFormat(m_format);
        m_surface->start(m_format);
    }
}

void FrameProvidSvc::setFormat(int width, int heigth, QVideoFrame::PixelFormat format)
{
    QSize size(width, heigth);
    QVideoSurfaceFormat vsformat(size, format);
    m_format = vsformat;

    if (m_surface)
    {
        if (m_surface->isActive())
        {
            m_surface->stop();
        }
        m_format = m_surface->nearestFormat(m_format);
        m_surface->start(m_format);
    }
}



void FrameProvidSvc::onNewVideoContentReceived(/*QByteArray qa*/)
{
    auto rawData=DevOps::DeviceOperation::getSingleton()->m_frameRawData;
    qDebug()<<"frame provider received "+QString::number(rawData.size());
    if(rawData.size()==640*480)
    {
        QImage img((uchar*)rawData.data(),640,480,QImage::Format::Format_Grayscale8);
        auto img2=img.convertToFormat(QImage::Format::Format_ARGB32);
        QVideoFrame frame(img2);
        setFormat(frame.width(),frame.height(),frame.pixelFormat());
        if (m_surface)
            m_surface->present(frame);
    }
}

//void FrameProvidSvc::onNewVideoContentReceived(/*QByteArray qa*/)
//{

//    static quint8 byteR=0;
//    static quint8 byteG=0;
//    static quint8 byteB=0;

//    QImage img(640,480,QImage::Format::Format_ARGB32);
//    for(int y=0; y<img.height();y++)
//    {
//        for(int x=0;x<img.width();x++)
//        {
//            int color=0xff000000+((byteR+x)%256<<16)+((byteG+x)%256<<8)+(byteB+x)%256;
//            img.setPixel(x,y,color);
//        }
//    }

//    byteR+=1;
//    byteG+=2;
//    byteB+=3;

//    QVideoFrame frame2(img);
//    setFormat(frame2.width(),frame2.height(),frame2.pixelFormat());
//    if (m_surface)
//        m_surface->present(frame2);

////    auto devOp=DevOps::DeviceOperation::getSingleton();
////    auto qa=devOp->getRealTimeStimulationEyeImage();
////    QImage img((uchar*)qa.data(),640,480,QImage::Format::Format_Grayscale8);
////    if (m_surface)
////        m_surface->present(QVideoFrame(img.convertToFormat(QImage::Format::Format_ARGB32)));
//}
//}


//void FrameProvidSvc::onNewVideoContentReceived(/*QByteArray qa*/)
//{

//    static quint8 byteR=0;
//    static quint8 byteG=0;
//    static quint8 byteB=0;

//    QImage img(640,480,QImage::Format::Format_ARGB32);
//    for(int y=0; y<img.height();y++)
//    {
//        for(int x=0;x<img.width();x++)
//        {
//            int color=0xff000000+((byteR+x)%256<<16)+((byteG+x)%256<<8)+(byteB+x)%256;
//            img.setPixel(x,y,color);
//        }
//    }

//    byteR+=1;
//    byteG+=2;
//    byteB+=3;

//    QVideoFrame frame2(img);
//    setFormat(frame2.width(),frame2.height(),frame2.pixelFormat());
//    if (m_surface)
//        m_surface->present(frame2);

//}
}


