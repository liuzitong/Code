#ifndef DEVICEOPERATION_H
#define DEVICEOPERATION_H

#include "device_operation_global.h"
#include <QSharedPointer>
#include <QString>
#include <QDebug>

namespace DevOps{
class DEVICEOPERATIONSHARED_EXPORT DeviceOperation
{

public:
    DeviceOperation();
    static void CreateInstance(QString PID,QString VID,QString localDataFilePath);
    static void hello();

private:
    void privHello();
    static QSharedPointer<DeviceOperation> m_singleton;
    QString word;
};
}

#endif // DEVICEOPERATION_H
