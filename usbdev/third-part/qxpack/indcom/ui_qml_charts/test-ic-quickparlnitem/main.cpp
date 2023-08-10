#include <QDebug>
#include <QGuiApplication>
#include <QByteArray>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QThread>
#include <QSharedPointer>
#include <QList>
#include <QPointF>
#include <QTimer>

#include "qxpack/indcom/common/qxpack_ic_global.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/ui_qml_base/qxpack_ic_quickxyseriesdata.hxx"
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
        eg->addImportPath("qrc:/");
        eg->load(QUrl(QStringLiteral("qrc:/main.qml")));
        ret = app.exec();
        delete eg;
    }

   //QxPack::IcMemCntr::saveTraceInfoToFile("z:/mem_cntr.txt" );
   //QxPack::IcMemCntr::enableMemTrace(false);
    qInfo() << " the last mem cntr:" << QxPack::IcMemCntr::currNewCntr();

    return ret;
}


