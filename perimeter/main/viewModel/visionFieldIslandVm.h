#ifndef VISIONFIELDISLANDVM_H
#define VISIONFIELDISLANDVM_H

#include <QObject>
namespace Perimeter
{
class VisionFieldIslandVm:public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit VisionFieldIslandVm(const QVariantList & );
    Q_INVOKABLE virtual ~VisionFieldIslandVm();
};
}
#endif // VISIONFIELDISLANDVM_H
