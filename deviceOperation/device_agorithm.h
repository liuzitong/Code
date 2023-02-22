#ifndef DEVICE_AGORITHM_H
#define DEVICE_AGORITHM_H
#include <QPoint>
#include "device_data.h"
namespace DevOps{
class DeviceAgorithm
{
public:
    DeviceAgorithm()=default;
    static int getFocusMotorPosByDist(int focalDist,int spotSlot);
    static CoordMotorPosFocalDistInfo getXYMotorPosAndFocalDistFromCoord(const QPointF loc);
    /**
     * @brief interPolation
     * @param value 四个点位的值
     * @param loc   求值的位置
     * @return      所在位置的值
     */
private:
    static int interpolation(int value[],QPointF loc);
};
}

#endif // DEVICE_AGORITHM_H
