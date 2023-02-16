#ifndef OBJECT_WRAPPER_H
#define OBJECT_WRAPPER_H
#include <QHeightMapSurfaceDataProxy>
#include <QVariantList>
#include <QDebug>

namespace Perimeter {
class QHeightMapSurfaceDataProxyWrapper:QtDataVisualization::QHeightMapSurfaceDataProxy
{
    Q_OBJECT
public:
//    Q_INVOKABLE explicit QHeightMapSurfaceDataProxyWrapper(const QVariantList &args):
//        QHeightMapSurfaceDataProxy(QImage(args[0].toString()),nullptr){
//        qDebug()<<args[0].toString();
//    };
    Q_INVOKABLE explicit QHeightMapSurfaceDataProxyWrapper(const QVariantList &args){};

    Q_INVOKABLE virtual ~QHeightMapSurfaceDataProxyWrapper() Q_DECL_OVERRIDE{qDebug()<<"decons";};

    Q_INVOKABLE void setHeightMap(QString filePath)
    {
//        auto base=static_cast<QHeightMapSurfaceDataProxy*>(this);
//        img.reset(new QImage(filePath));
//        base->setHeightMap(*img.data());
//        emit base->heightMapChanged(*img.data());

        auto base=static_cast<QHeightMapSurfaceDataProxy*>(this);
        base->setHeightMap(QImage(filePath));
        emit base->heightMapChanged(QImage(filePath));
    }
//private:
//    QSharedPointer<QImage> img;




};




}
#endif // OBJECT_WRAPPER_H
