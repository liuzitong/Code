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
    QJsonObject m_rootObj;
    int m_pupilAutoAlignPixelTolerance;
    int m_castLightDADifference;
    int m_castLightTagetDA;
    int m_castLightDAChangeStep;
    int m_castLightTargetColor;
    int m_castLightTargetSize;
    int m_castLightDAChanged;
    QString m_castLightLastAdjustedDate;
    int m_beepCount;
    int m_beepInterval;
    int m_beepDuration;
    int m_pupilAutoAlignStep;
    quint8 m_5MotorSpeed[5];
    quint8 m_motorChinSpeed[2];
    QString localConfigPath,localDataPath;
    QList<QPair<int,int>> m_colorToSlot,m_spotSizeToSlot;
    int m_pupilGreyLimit;
    int m_pupilReflectionDotLimit;
    double m_pupilPixelDiameterMaxLimit;
    double m_pupilPixelDiameterMinLimit;
    double m_pupilDiameterPixelToMillimeterConstant;
    double m_pupilDiameterPixelToFixationDeviationConstant;
    static QSharedPointer<DeviceSettings> getSingleton();
    static QSharedPointer<DeviceSettings> m_singleton;

    void saveSettings();
};

#endif // DEVICE_SETTINGS_H
