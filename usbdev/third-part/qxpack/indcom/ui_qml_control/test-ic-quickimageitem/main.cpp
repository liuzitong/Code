#include <QDebug>
#include <QGuiApplication>
#include <QByteArray>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QThread>
#include <QImage>
#include <QSharedPointer>

#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/common/qxpack_ic_queuetemp.hpp"
#include "qxpack/indcom/sys/qxpack_ic_rmtobjcreator_priv.hxx"
#include "qxpack/indcom/sys/qxpack_ic_imagedata.hxx"
#include "qxpack/indcom/ui_qml_base/qxpack_ic_quickimagedata.hxx"

typedef  QxPack::IcArrayQueueTemp<QxPack::IcImageData,5>  VideoSwapBuffer;

// ////////////////////////////////////////////////////////////////////////////
//                             grabber
// ////////////////////////////////////////////////////////////////////////////
class  Grabber : public QObject {
    Q_OBJECT
public:
    // ------------------------------------------------------------------------
    // ctor
    // ------------------------------------------------------------------------
    Grabber( QSharedPointer<VideoSwapBuffer> &s_vb )
    {
        m_im_idx = 0; m_vb_swp = s_vb;
        m_im[0].load( ":/bmp0.bmp"); m_im[0] = m_im[0].convertToFormat( QImage::Format_RGBA8888 );
        m_im[1].load( ":/bmp1.bmp"); m_im[1] = m_im[1].convertToFormat( QImage::Format_RGBA8888 );
        m_im[2].load( ":/bmp2.bmp"); m_im[2] = m_im[2].convertToFormat( QImage::Format_RGBA8888 );
        m_im[3].load( ":/bmp3.bmp"); m_im[3] = m_im[3].convertToFormat( QImage::Format_RGBA8888 );

//        m_im[0] = m_im[0].scaled(5000,4000);
//        m_im[1] = m_im[1].scaled(5000,4000);
//        m_im[2] = m_im[2].scaled(5000,4000);
//        m_im[3] = m_im[3].scaled(5000,4000);
        m_emit_once = false;
    }
    virtual ~Grabber ( ) { }

    // ------------------------------------------------------------------------
    // start the work
    // ------------------------------------------------------------------------
    Q_INVOKABLE void doWork( ) { startTimer(45); }

    Q_SIGNAL    void newImage( );

protected:
    // ------------------------------------------------------------------------
    // handle the timer event, to fetch the image...
    // ------------------------------------------------------------------------
    virtual void  timerEvent( QTimerEvent* ) Q_DECL_OVERRIDE {
        ++ m_im_idx;        
        QImage &im_ref = m_im[ m_im_idx % 4 ];

        // --------------------------------------------------------------------
        // make a image data and enqueue it as video frame
        // --------------------------------------------------------------------
        QImage t_im( im_ref.width(), im_ref.height(), im_ref.format() );
        QxPack::IcImageData im_data;
        // use memory copy to simulate the data transfer...
        std::memcpy( t_im.bits(), m_im[ m_im_idx % 4 ].constBits(), t_im.byteCount() );
        im_data.setImage( t_im );
        m_vb_swp->forceEnqueue( im_data );
        im_data = QxPack::IcImageData();

        // finished, tell parent
        if ( ! m_emit_once ) {
            emit this->newImage();
            m_emit_once = true;
        }
    }
private:
    uint m_im_idx; QImage m_im[4]; bool m_emit_once;
    QSharedPointer<VideoSwapBuffer> m_vb_swp;
};

// ////////////////////////////////////////////////////////////////////////////
// handler in ui thread
// ////////////////////////////////////////////////////////////////////////////
class  GrabberHandler : public QObject {
    Q_OBJECT
private:
    QSharedPointer<VideoSwapBuffer>   m_vb_swp;
    QxPack::IcQuickImageData          m_im_src;
public:
    // ------------------------------------------------------------------------
    // ctor
    // ------------------------------------------------------------------------
    GrabberHandler ( QSharedPointer<VideoSwapBuffer> &s_vb, const QxPack::IcQuickImageData &src )
    {
        m_vb_swp = s_vb;
        m_im_src.attach( src );
    }

    // ------------------------------------------------------------------------
    // handle: the new image slot
    // ------------------------------------------------------------------------
    Q_SLOT void onNewImage( )
    {
        QxPack::IcImageData im;
        if ( ! m_vb_swp->dequeue( im )) { return; }
        m_im_src.setImageData( im );  // update data image...
    }
};


// ////////////////////////////////////////////////////////////////////////////
//
//                             main entry
//
// ////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[])
{
    int ret;
    QGuiApplication app( argc, argv );
    qDebug() << "main thread id:" << QThread::currentThread();

   // QxPack::IcMemCntr::enableMemTrace( true );

    qInfo() << "the current mem cntr:" << QxPack::IcMemCntr::currNewCntr();
    {
        QSharedPointer<VideoSwapBuffer> share_vb(
            new VideoSwapBuffer,[](VideoSwapBuffer *obj){ delete obj; }
        );

        QxPack::IcQuickImageData  im_src( QxPack::IcQuickImageData::UseMode_Normal );

        QThread  grab_thread; grab_thread.start();

        // create grabber
        Grabber *grab = qobject_cast< Grabber *>(
            QxPack::IcRmtObjCreator::createObjInThread(
                & grab_thread,
                []( void *vb ) -> QObject* {
                    QSharedPointer<VideoSwapBuffer> *s_vb = reinterpret_cast<QSharedPointer<VideoSwapBuffer>*>( vb );
                    Grabber *gr = new Grabber( *s_vb );
                    QObject::connect( QThread::currentThread(), SIGNAL(finished()), gr, SLOT(deleteLater()) );
                    return gr;
                }, &share_vb
            )
        );

        // create grabber handler
        GrabberHandler  grab_handler( share_vb, im_src );
        QObject::connect( grab, SIGNAL(newImage()), & grab_handler, SLOT(onNewImage()));

        // start
        QMetaObject::invokeMethod( grab, "doWork", Qt::BlockingQueuedConnection);

        QQmlApplicationEngine *eg = new QQmlApplicationEngine( );

        eg->rootContext()->setContextProperty("gQuickImageSrc", &im_src );
        eg->addImportPath("qrc:/");
        eg->load(QUrl(QStringLiteral("qrc:/main.qml")));
        ret = app.exec();

        // finish
        delete eg;
        grab_thread.quit();
        grab_thread.wait();
    }

   //QxPack::IcMemCntr::saveTraceInfoToFile("z:/mem_cntr.txt" );
   //QxPack::IcMemCntr::enableMemTrace(false);
    qInfo() << " the last mem cntr:" << QxPack::IcMemCntr::currNewCntr();

    return ret;
}


#include "main.moc"
