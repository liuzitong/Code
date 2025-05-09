#ifndef DEVICE_SETTINGS_H
#define DEVICE_SETTINGS_H

#include <QPair>
#include <QList>
#include <QJsonArray>
#include <QMap>

#include <QSharedPointer>
#include <QJsonObject>


class DeviceSettings
{
public:
    DeviceSettings();
    ~DeviceSettings()=default;

    QString m_VID,m_PID;
    int m_pupilAutoAlignPixelTolerance;
    double m_castLightDADifferenceTolerance;
    int m_castLightStablizeWaitingTime;
    int m_castLightDA;
    double m_castLightDAChangeRate;
    int m_castLightDAChangeInterval;
    int m_castLightDAChangeMinStep;
    int m_castLightDALimit;
    int m_castLightTargetColor;
    int m_castLightTargetSize;
    QString m_castLightLastAdjustedDate;
    bool m_skipAdjustCastLight;
    int m_beepCount;
    int m_beepInterval;
    int m_beepDuration;
    int m_pupilAutoAlignStep;
    quint8 m_5MotorSpeed[5];
    quint8 m_motorChinSpeed[2];
    QString localConfigPath,localDataPath;
    QList<QPair<int,int>> m_colorToSlot,m_spotSizeToSlot;
    int m_waitingTime;
    int m_pixelDistFromPupilCenterToMiddleReflectionDot;
    double m_pupilDiameterPixelToMillimeterConstant;
    double m_pupilDeviationPixelToNumberConstant;
    int m_reflectDotDist;
    int m_reconTimes;
    bool m_settingsRead=false;
    int m_reconnectTime;
    int m_deviationCalibrationXMotorDeviation;       //正表副表都向左边。
    int m_deviationCalibrationYMotorDeviation;       //正表向上，副表向下。
    double m_deviationCalibrationStep;          //X方向上的扫描角度
    bool m_deviationCalibrationFail;
    int m_deviationCalibrationDA;        //低于次值一段时间就开始对位

    int m_deviationCalibrationWatingTime;
    static QSharedPointer<DeviceSettings> getSingleton();
    static QSharedPointer<DeviceSettings> m_singleton;

    void saveCastLightAdjustStatus();
    void saveDeviationCalibrationStatus();
    void saveReconTimes();
};

#endif // DEVICE_SETTINGS_H
