#include "device_operation.h"
#include <usbdev/main/usbdev_devctl.hxx>
namespace DevOps{

QSharedPointer<DeviceOperation> DeviceOperation::m_singleton=nullptr;

DeviceOperation::DeviceOperation()
{
}

void DeviceOperation::CreateInstance(QString PID, QString VID, QString localDataFilePath)
{
//    quint32 vid_pid=VID.toInt(nullptr,16)<<16|PID.toInt(nullptr,16);
//    m_devCtl=UsbDev::DevCtl::createInstance(vid_pid);
//    ui->label_VID->setText(VID);
//    ui->label_PID->setText(PID);
}

void DeviceOperation::hello()
{
//   m_singleton->hello();
//    m_singleton->privHello();
    qDebug()<<"hello from ops";
    UsbDev::DevCtl::hello();
}


void DeviceOperation::privHello()
{
    qDebug()<<word;
}
};
