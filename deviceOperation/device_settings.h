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
    static QSharedPointer<DeviceSettings> getSingleton();
    static QSharedPointer<DeviceSettings> m_singleton;

    void saveCastLightAdjustStatus();
    void saveReconTimes();
};

#endif // DEVICE_SETTINGS_H
