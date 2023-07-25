#ifndef DEVICESTATUSVM_H
#define DEVICESTATUSVM_H
#include <QObject>
#include <deviceOperation/device_operation.h>
namespace Perimeter
{
class DeviceStatusDataVm:public QObject
{
    Q_OBJECT
    Q_PROPERTY(int serialNo READ getSerialNo);
    Q_PROPERTY(bool answerpadStatus READ getAnswerPadStatus);
    Q_PROPERTY(bool cameraStatus READ getCameraStatus);
    Q_PROPERTY(bool eyeglassStatus READ getEyeglassStatus);
    Q_PROPERTY(bool xMotorBusy READ getXMotorBusy);
    Q_PROPERTY(bool yMotorBusy READ getYMotorBusy);
    Q_PROPERTY(bool focusMotorBusy READ getFocusMotorBusy);
    Q_PROPERTY(bool colorMotorBusy READ getColorMotorBusy);
    Q_PROPERTY(bool lightSpotMotorBusy READ getLightSpotMotorBusy);
    Q_PROPERTY(bool shutterMotorBusy READ getShutterMotorBusy);
    Q_PROPERTY(bool xChinMotorBusy READ getXChinMotorBusy);
    Q_PROPERTY(bool yChinMotorBusy READ getYChinMotorBusy);
    Q_PROPERTY(int xMotorCmdCntr READ getXMotorCmdCntr);
    Q_PROPERTY(int yMotorCmdCntr READ getYMotorCmdCntr);
    Q_PROPERTY(int focusMotorCmdCntr READ getFocusMotorCmdCntr);
    Q_PROPERTY(int colorMotorCmdCntr READ getColorMotorCmdCntr);
    Q_PROPERTY(int lightSpotMotorCmdCntr READ getLightSpotMotorCmdCntr);
    Q_PROPERTY(int shutterMotorCmdCntr READ getShutterMotorCmdCntr);
    Q_PROPERTY(int xChinMotorCmdCntr READ getXChinMotorCmdCntr);
    Q_PROPERTY(int yChinMotorCmdCntr READ getYChinMotorCmdCntr);
    Q_PROPERTY(bool moveStatus READ getMoveStatus);
    Q_PROPERTY(int xMotorCurrPos READ getXMotorCurrPos);
    Q_PROPERTY(int yMotorCurrPos READ getYMotorCurrPos);
    Q_PROPERTY(int focusMotorCurrPos READ getFocusMotorCurrPos);
    Q_PROPERTY(int colorMotorCurrPos READ getColorMotorCurrPos);
    Q_PROPERTY(int lightSpotMotorCurrPos READ getLightSpotMotorCurrPos);
    Q_PROPERTY(int shutterMotorCurrPos READ getShutterMotorCurrPos);
    Q_PROPERTY(int xChinMotorCurrPos READ getXChinMotorCurrPos);
    Q_PROPERTY(int yChinMotorCurrPos READ getYChinMotorCurrPos);
    Q_PROPERTY(int envLightDA READ getEnvLightDA);
    Q_PROPERTY(int castLightDA READ getCastLightDA);

public:
    DeviceStatusDataVm();
    inline int getSerialNo(){return data.serialNo;}
    inline bool getAnswerPadStatus(){return data.answerpadStatus;}
    inline bool getCameraStatus(){return data.cameraStatus;}
    inline bool getEyeglassStatus(){return data.eyeglassStatus;}
    inline bool getXMotorBusy(){return data.xMotorBusy;}
    inline bool getYMotorBusy(){return data.yMotorBusy;}
    inline bool getFocusMotorBusy(){return data.focusMotorBusy;}
    inline bool getColorMotorBusy(){return data.colorMotorBusy;}
    inline bool getLightSpotMotorBusy(){return data.lightSpotMotorBusy;}
    inline bool getShutterMotorBusy(){return data.shutterMotorBusy;}
    inline bool getXChinMotorBusy(){return data.xChinMotorBusy;}
    inline bool getYChinMotorBusy(){return data.yChinMotorBusy;}
    inline int getXMotorCmdCntr(){return data.xMotorCmdCntr;}
    inline int getYMotorCmdCntr(){return data.yMotorCmdCntr;}
    inline int getFocusMotorCmdCntr(){return data.focusMotorCmdCntr;}
    inline int getColorMotorCmdCntr(){return data.colorMotorCmdCntr;}
    inline int getLightSpotMotorCmdCntr(){return data.lightSpotMotorCmdCntr;}
    inline int getShutterMotorCmdCntr(){return data.shutterMotorCmdCntr;}
    inline int getXChinMotorCmdCntr(){return data.xChinMotorCmdCntr;}
    inline int getYChinMotorCmdCntr(){return data.yChinMotorCmdCntr;}
    inline bool getMoveStatus(){return data.moveStatus;}
    inline int getXMotorCurrPos(){return data.xMotorCurrPos;}
    inline int getYMotorCurrPos(){return data.yMotorCurrPos;}
    inline int getFocusMotorCurrPos(){return data.focusMotorCurrPos;}
    inline int getColorMotorCurrPos(){return data.colorMotorCurrPos;}
    inline int getLightSpotMotorCurrPos(){return data.lightSpotMotorCurrPos;}
    inline int getShutterMotorCurrPos(){return data.shutterMotorCurrPos;}
    inline int getXChinMotorCurrPos(){return data.xChinMotorCurrPos;}
    inline int getYChinMotorCurrPos(){return data.yChinMotorCurrPos;}
    inline int getEnvLightDA(){return data.envLightDA;}
    inline int getCastLightDA(){return data.castLightDA;}

private:
    DevOps::StatusDataOut& data=DevOps::DeviceOperation::getSingleton()->m_statusDataOut;
};
}
#endif // DEVICESTATUSVM_H
