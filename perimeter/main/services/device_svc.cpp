#include "device_svc.h"

QSharedPointer<DeviceSvc> DeviceSvc::m_singleton=QSharedPointer<DeviceSvc>(nullptr);

DeviceSvc::DeviceSvc()
{

}

void DeviceSvc::stimulate(int db, QPointF loc,int msecs )
{
    setDB(db);
}

bool DeviceSvc::waitForAnswer(int msecs)
{
    return false;
}

bool DeviceSvc::waitForAnswer(QVector<int> motorIDs)
{
    return false;
}

void DeviceSvc::waitForMotors(QVector<int> motorIDs)
{

}


void DeviceSvc::dynamicStimulate(QPointF begin,QPointF end,int speedLevel)
{

}

void DeviceSvc::setColorAndCursorSize(int color, int CursorSize)
{

}

void DeviceSvc::setDB(int db)
{

}

void DeviceSvc::moveCursor(QPointF loc)
{

}

void DeviceSvc::openShutter(int msecs)
{

}

QByteArray DeviceSvc::getRealTimeStimulationEyeImage()
{
    return QByteArray();
}
