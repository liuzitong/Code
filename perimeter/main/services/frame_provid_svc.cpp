#include "frame_provid_svc.h"
#include <QImage>
#include <QDebug>
namespace Perimeter{

QSharedPointer<FrameProvidSvc> FrameProvidSvc::m_singleton=nullptr;

FrameProvidSvc::FrameProvidSvc()
{
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




void FrameProvidSvc::onNewVideoContentReceived(QByteArray qa)
{
    static quint8 grey=0;
    grey+=1;
    grey%=255;
    QByteArray qa2(640*480,grey);
    QImage img1((uchar*)qa2.data(),640,480,QImage::Format::Format_Grayscale8);
    auto img2=img1.convertToFormat(QImage::Format::Format_ARGB32);
    QVideoFrame frame2(img2);

    if (m_surface)
        m_surface->present(frame2);
}
}


