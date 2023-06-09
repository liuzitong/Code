#ifndef DEVICE_PUPIL_PROCESSOR_H
#define DEVICE_PUPIL_PROCESSOR_H
#include <QByteArray>
#include <QPoint>
#include <device_settings.h>
namespace DevOps{
class DevicePupilProcessor
{
public:
    DevicePupilProcessor();
    void processData(QByteArray data);
    QVector<int> findPupil(QByteArray data);
    QByteArray DataToBlackAndWhite(QByteArray data,int value);
    double getEyeMove(double x,double y,double dx,double dy,double wx,double wy);
    QVector<int> findPupilAtXY(QByteArray data,int x,int y);
    QVector<int> findWhiteDot(QByteArray data,QPoint topLeft,QPoint bottomRight);
    void clearData();

    QVector<QVector<int>> m_pupilData;
    float m_pupilDiameter;
    int m_pupilDeviation;
//    bool m_pupilDataGet;
};
}
#endif // DEVICE_PUPIL_PROCESSOR_H
