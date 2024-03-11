#ifndef DEVICE_PUPIL_PROCESSOR_H
#define DEVICE_PUPIL_PROCESSOR_H
#include <QByteArray>
#include <QPoint>
#include "device_settings.h"

namespace DevOps{
class DevicePupilProcessor
{
public:
    DevicePupilProcessor();
    // void find_point(uchar* data, int width, int height);
    void processData(uchar* data,int width,int height);
    QVector<QPointF> caculatePupil(uchar* ba,int width,int height);
    QVector<QPointF> caculateReflectingDot(uchar* ba,int width,int height);
    float caculatePupilDiameter(QPointF topLeft,QPointF bottomRight);
    int caculateFixationDeviation(QPointF pupil,QVector<QPointF> reflectionDot);
    void clearData();

    QVector<QVector<int>> m_pupilData;
    double m_pupilDiameter=0;
    int m_pupilDeviation=0;
    QPointF m_pupilCenterPoint;
    double m_pupilDiameterPix;
    QVector<float> m_pupilDiameterArr;
    QVector<QPointF> m_reflectionDot;
    bool m_pupilResValid,m_reflectionResValid;
    int m_pupilGreyLimit;
    int m_pupilReflectionDotWhiteLimit;
    //    bool m_pupilDataGet;
};
}
#endif // DEVICE_PUPIL_PROCESSOR_H
