#ifndef DEVICE_SVC_H
#define DEVICE_SVC_H
#include <QPoint>
#include <QSharedPointer>
#include <QVector>

class DeviceSvc
{
public:
    DeviceSvc();
    static QSharedPointer<DeviceSvc> getSingleton()
    {
        if(m_singleton.data()==nullptr)
        {
            m_singleton.reset(new DeviceSvc());
        }
        return m_singleton;
    }
    void stimulate(int db,QPointF loc,int msecs );
    bool waitForAnswer(int msecs);
    void dynamicStimulate(QPointF begin, QPointF end, int speedLevel);
    void setColorAndCursorSize(int color,int CursorSize);
    void setDB(int db);
    void moveCursor(QPointF loc);
    void openShutter(int msecs);
    bool waitForAnswer(QVector<int> motorIDs);
    void waitForMotors(QVector<int> motorIDs);
    QByteArray getRealTimeStimulationEyeImage();
private:
    static QSharedPointer<DeviceSvc> m_singleton;
};

#endif // DEVICE_SVC_H
