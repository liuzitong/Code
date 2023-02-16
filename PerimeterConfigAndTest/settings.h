#ifndef LOCAL_DATA_H
#define LOCAL_DATA_H

#include <QPair>
#include <QList>
#include <QJsonArray>
#include <QMap>
#include <table_model.h>
#include <QSharedPointer>
#include <QJsonObject>
#include "models.h"


class Settings
{
public:
    Settings();
    bool m_updateIOInfo,m_updateRefreshInfo,m_updateRefreshIOInfo;
    QString m_VID,m_PID;
    QJsonObject m_rootObj;
    QList<QPair<QString,int>> m_colorToSlot,m_spotSizeToSlot;
    QString localConfig,localData;
    void Write();
};

#endif
