#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>

namespace Perimeter
{
class Settings :public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString hospitalName READ getHospitalName WRITE setHospitalName NOTIFY hospitalNameChanged)
    Q_PROPERTY(QString language READ getLanguage WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(QString version READ getVersion WRITE setVersion NOTIFY versionChanged)
    Q_PROPERTY(QString deviceInfo READ getDeviceInfo WRITE setDeviceInfo NOTIFY deviceInfoChanged)
    Q_PROPERTY(bool isRuntimeLangEng READ getIsRuntimeLangEng WRITE setIsRuntimeLangEng NOTIFY isRuntimeLangEngChanged)
    Q_PROPERTY(QString programUnlockPwd READ programUnlockPwd WRITE setProgramUnlockPwd)
    Q_PROPERTY(int defaultProgramId READ defaultProgramId WRITE setDefaultProgramId)
    Q_PROPERTY(int defaultProgramType READ defaultProgramType WRITE setDefaultProgramType)
//    Q_PROPERTY(QString trigger READ getTrigger NOTIFY triggerChanged)
    Q_PROPERTY(QString langTrigger READ langTrigger /*WRITE setLangTrigger*/ NOTIFY langTriggerChanged)




public:
    Settings();
    Q_INVOKABLE void save();
    QString getHospitalName(){return m_hospitalName;}void setHospitalName(QString value){m_hospitalName=value;emit hospitalNameChanged(value);}Q_SIGNAL void hospitalNameChanged(QString value);
    QString getLanguage(){return m_language;}void setLanguage(QString value){m_language=value;emit languageChanged(value);} Q_SIGNAL void languageChanged(QString value);
    QString getVersion(){return m_version;}void setVersion(QString value){m_version=value;emit versionChanged(value);}Q_SIGNAL void versionChanged(QString value);
    QString getDeviceInfo(){return m_deviceInfo;}void setDeviceInfo(QString value){m_deviceInfo=value;emit deviceInfoChanged(value);}Q_SIGNAL void deviceInfoChanged(QString value);
    bool getIsRuntimeLangEng(){return m_isRuntimeLangEng;}void setIsRuntimeLangEng(bool value){m_isRuntimeLangEng=value;emit isRuntimeLangEngChanged(value);}Q_SIGNAL void isRuntimeLangEngChanged(bool value);
    QString programUnlockPwd() const{return m_programUnlockPwd;}void setProgramUnlockPwd(QString value){m_programUnlockPwd = value;}
    int defaultProgramId(){return m_defaultProgramId;}void setDefaultProgramId(int value){m_defaultProgramId=value;}
    int defaultProgramType(){return m_defaultProgramType;}void setDefaultProgramType(int value){m_defaultProgramType=value;}
    QString langTrigger(){return "";}Q_SIGNAL void langTriggerChanged();
    //    QString getTrigger(){return "";} Q_SIGNAL void triggerChanged();
//    void setLangTrigger(QString langTrigger)
//    {
//        if (m_langTrigger == langTrigger)
//            return;

//        m_langTrigger = langTrigger;
//        emit langTriggerChanged(m_langTrigger);
//    }
private:
    void changeLang();
private:
    QString m_hospitalName;
    QString m_language;
    QString m_version;
    QString m_deviceInfo;
    QString m_programUnlockPwd;
    bool m_isRuntimeLangEng;
    int m_defaultProgramId;
    int m_defaultProgramType;




};
}
#endif // SETTINGS_H
