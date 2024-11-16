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
    QFile jsonFile("./settings.json");
    if(jsonFile.open(QIODevice::ReadOnly))
    {
        QJsonParseError jsonParserError;
        auto JsonDoc = QJsonDocument::fromJson(jsonFile.readAll(),&jsonParserError);
        auto jo=JsonDoc.object();
        m_hospitalName=jo["hospitalName"].toString();
        m_language=jo["language"].toString();
        m_publishVersion=jo["publishVersion"].toString();
        m_fullVersion=jo["fullVersion"].toString();
        m_version=jo["version"].toString();
        m_deviceInfo=jo["deviceInfo"].toString();
        m_defaultProgramId=jo["defaultProgramId"].toInt();
        m_defaultProgramType=jo["defaultProgramType"].toInt();
        m_programUnlockPwd=jo["programUnlockPwd"].toString();
        m_virtualKeyBoard=jo["virtualKeyBoard"].toBool();
        m_showUserAuthorities=jo["showUserAuthorities"].toBool();
        changeLang();
        jsonFile.close();
    }
}

void Settings::save()
{
    QFile jsonFile("./settings.json");
    jsonFile.open(QIODevice::WriteOnly);
    QJsonObject jo{
        {"hospitalName",m_hospitalName},
        {"language",m_language},
        {"publishVersion",m_publishVersion},
        {"fullVersion",m_fullVersion},
        {"version",m_version},
        {"deviceInfo",m_deviceInfo},
        {"defaultProgramId",m_defaultProgramId},
        {"defaultProgramType",m_defaultProgramType},
        {"programUnlockPwd",m_programUnlockPwd},
        {"virtualKeyBoard",m_virtualKeyBoard},
        {"showUserAuthorities",m_showUserAuthorities}
    };
    QJsonDocument jsonDoc;
    jsonDoc.setObject(jo);
    jsonFile.write(jsonDoc.toJson());
    jsonFile.close();
    changeLang();
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
