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
    void processData(void* result);
    int caculateFixationDeviation(void* result);
    bool caculateIsTooFar(void *result);
    void clearData();


    QVector<QVector<int>> m_pupilData;
    double m_pupilDiameter=0;
    int m_pupilDeviation=0;
    double m_pupilDiameterPix;
    QVector<float> m_pupilDiameterArr;
    bool m_pupilResValid,m_reflectionResValid;
    // int m_pupilGreyLimit;
    // int m_pupilReflectionDotWhiteLimit;
    bool m_isTooFar=false;
    //    bool m_pupilDataGet;
};
}
#endif // DEVICE_PUPIL_PROCESSOR_H
