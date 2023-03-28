#include "settings.h"
#include <QJsonArray>
#include <qfile.h>
#include <qjsondocument.h>
#include <QJsonObject>
#include <QDir>
#include <QDebug>
#include <perimeter/main/services/translate_svc.h>
namespace Perimeter
{
Settings::Settings()
{
    QFile jsonFile("./UserSettings.json");
    if(jsonFile.open(QIODevice::ReadOnly))
    {
        QJsonParseError jsonParserError;
        auto JsonDoc = QJsonDocument::fromJson(jsonFile.readAll(),&jsonParserError);
        auto jo=JsonDoc.object();
        m_hospitalName=jo["hospitalName"].toString();
        m_language=jo["language"].toString();
        m_version=jo["version"].toString();
        m_deviceInfo=jo["deviceInfo"].toString();
        m_defaultProgramId=jo["defaultProgramId"].toInt();
        m_defaultProgramType=jo["defaultProgramType"].toInt();
        changeLang();
        jsonFile.close();
    }

    jsonFile.setFileName("./FactorySettings.json");
    if(jsonFile.open(QIODevice::ReadOnly))
    {
        QJsonParseError jsonParserError;
        auto JsonDoc = QJsonDocument::fromJson(jsonFile.readAll(),&jsonParserError);
        auto jo=JsonDoc.object();
        {
            m_programUnlockPwd=jo["programUnlockPwd"].toString();
            auto boundary=jo["boundaryOne"].toObject();
            auto center=boundary["center"].toObject();
            auto radius=boundary["radius"].toInt();
            m_boundaries.append({center["x"].toInt(),center["y"].toInt(),radius});
        }
        {
            auto boundary=jo["boundaryTwo"].toObject();
            auto center=boundary["center"].toObject();
            auto radius=boundary["radius"].toInt();
            m_boundaries.append({center["x"].toInt(),center["y"].toInt(),radius});
            jsonFile.close();
        }
    }
}

void Settings::save()
{
    QFile jsonFile("./UserSettings.json");
    jsonFile.open(QIODevice::WriteOnly);
    QJsonObject jo{
        {"hospitalName",m_hospitalName},
        {"language",m_language},
        {"version",m_version},
        {"deviceInfo",m_deviceInfo},
        {"defaultProgramId",m_defaultProgramId},
        {"defaultProgramType",m_defaultProgramType}
    };
    changeLang();
    QJsonDocument jsonDoc;
    jsonDoc.setObject(jo);
    jsonFile.write(jsonDoc.toJson());
    jsonFile.close();
}

void Settings::changeLang()
{
    QLocale::Language lang;
    if(m_language=="Chinese"||(m_language=="Default"&&QLocale::system().language()==QLocale::Chinese))
    {
        lang=QLocale::Chinese;
        setIsRuntimeLangEng(false);
    }
    else
    {
        lang=QLocale::English;
        setIsRuntimeLangEng(true);
    }
    TranslateController::instance()->loadLanguage(lang);
    emit langTriggerChanged();
}
}
