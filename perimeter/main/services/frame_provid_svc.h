#ifndef FRAMEPROVIDSVC_H
#define FRAMEPROVIDSVC_H

#include <QObject>
#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>
#include <QImage>
/*!
 * \brief FrameProvidSvc 作为qml VideoOutput.source
 */
namespace Perimeter{
class FrameProvidSvc : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAbstractVideoSurface *videoSurface READ videoSurface WRITE setVideoSurface)


public:
    FrameProvidSvc();
    ~FrameProvidSvc();

    static QSharedPointer<FrameProvidSvc> getSingleton();

    QAbstractVideoSurface *videoSurface() const;

    /*!
     * \brief 可设置外部自定义QAbstractVideoSurface
     * \param surface
     */
    void setVideoSurface(QAbstractVideoSurface *surface);

    /*!
     * \brief 设置视频格式
     * \param width     视频宽
     * \param heigth    视频高
     * \param format    enum QVideoFrame::PixelFormat
     */
    void setFormat(int width, int height, QVideoFrame::PixelFormat format);

    Q_INVOKABLE void setVideoSize(int width,int height);

public slots:
    /*!
     * \brief 接收外部数据源，视频帧
     * \param frame
     */
    void onNewVideoContentReceived(/*QByteArray qa*/);
private:
    void drawCrossHair(QImage& img);
    QAbstractVideoSurface *m_surface = NULL;
    QVideoSurfaceFormat m_format;
    int m_width;
    int m_height;
    static QSharedPointer<FrameProvidSvc> m_singleton;
};
}
#endif // FRAMEPRODER_H
