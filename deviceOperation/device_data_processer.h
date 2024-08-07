﻿#ifndef DEVICE_AGORITHM_H
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
    static int getFocusMotorPosByDist(int focalDist, int spotSlot, int focalMotorPosCorrection);
    static CoordMotorPosFocalDistInfo getXYMotorPosAndFocalDistFromCoord(const QPointF loc,bool isMainDotInfoTable);
    static quint16 calcCrc(quint8 *p_data, int data_len);

private:
    /**
     * @brief interPolation
     * @param value 四个点位的值
     * @param loc   求值的位置
     * @return      所在位置的值
     */
    static int interpolation(int value[],QPointF loc);
//    static bool isMainDotInfoTable;
};
}

#endif // DEVICE_AGORITHM_H
