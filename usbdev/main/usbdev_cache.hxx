#ifndef USBDEV_CACHE_H
#define USBDEV_CACHE_H

namespace UsbDev {
struct StaticCache
{
    int stimulateDotSerialNumber;
    int motorPosX;
    int motorPosY;
    int shutterTime;
    int answerPadPressedTime;
    int answerPadRealeasedTime;
};

struct MoveCache
{
    int stimulateDotSerialNumber;
    int motorPosX;
    int motorPosY;
};
}

#endif // USBDEV_CACHE_H
