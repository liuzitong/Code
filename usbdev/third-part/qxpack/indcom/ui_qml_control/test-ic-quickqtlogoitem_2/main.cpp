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
        QQmlApplicationEngine *eg = new QQmlApplicationEngine( );

        //eg->rootContext()->setContextProperty("gQuickImageSrc", &im_src );
        eg->addImportPath("qrc:/");
        eg->load(QUrl(QStringLiteral("qrc:/main.qml")));
        ret = app.exec();

        // finish
        delete eg;
    }

   //QxPack::IcMemCntr::saveTraceInfoToFile("z:/mem_cntr.txt" );
   //QxPack::IcMemCntr::enableMemTrace(false);
    qInfo() << " the last mem cntr:" << QxPack::IcMemCntr::currNewCntr();

    return ret;
}

