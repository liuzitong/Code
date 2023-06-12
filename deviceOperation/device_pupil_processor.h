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
    int findWhiteY(QByteArray data,int x,int y,int step);
    bool isWhiteYLine(QByteArray data,int x,int y);
    bool pupilFeature(QByteArray data,int x,int y,int x1,int y1);
    int findWhiteYLine(QByteArray data,int x,int y);
    QVector<int> findWhiteDot(QByteArray data,int x,int y,int x1,int y1);


    void processData(QByteArray data,int width,int height);
    QVector<QPointF> caculatePupil(const QByteArray ba,int width,int height);
    QVector<QPointF> caculateReflectingDot(const  QByteArray ba,int width,int height);
    float caculatePupilDiameter(QPointF topLeft,QPointF bottomRight);
    int caculateFixationDeviation(QVector<QPointF> pupil,QVector<QPointF> reflectionDot);
    void clearData();

    QVector<QVector<int>> m_pupilData;
    float m_pupilDiameter;
    int m_pupilDeviation;
    QPoint m_pupilCenterPoint;
    float m_pupilRadius;
    QVector<float> m_pupilDiameterArr;
    QVector<QPoint> m_reflectionDot;
    bool m_pupilResValid,m_reflectionResValid;
//    bool m_pupilDataGet;
};
}
#endif // DEVICE_PUPIL_PROCESSOR_H
