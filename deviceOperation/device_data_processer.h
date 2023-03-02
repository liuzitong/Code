#ifndef DEVICE_AGORITHM_H
#define DEVICE_AGORITHM_H
#include <QPoint>
#include <QByteArray>
#include <QVector>
#include "device_data.h"
namespace DevOps{
class DeviceDataProcesser
{
public:
    DeviceDataProcesser()=default;
    static int getFocusMotorPosByDist(int focalDist,int spotSlot);
    static CoordMotorPosFocalDistInfo getXYMotorPosAndFocalDistFromCoord(const QPointF loc);
    static QPoint caculatePupilDeviation(const QByteArray img,int width,int height);
    static float caculatePupilDiameter(const QByteArray img,int width,int height);
    static QSharedPointer<DeviceDataProcesser> getSingleton();

    /**
     * @brief interPolation
     * @param value 四个点位的值
     * @param loc   求值的位置
     * @return      所在位置的值
     */
private:
    float m_pupilDiameter;
    QVector<float> m_pupilDiameters;
    static QSharedPointer<DeviceDataProcesser> m_singleton;
    static int interpolation(int value[],QPointF loc);
};
}

#endif // DEVICE_AGORITHM_H
