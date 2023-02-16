#ifndef TRANSLATE_SVC_H
#define TRANSLATE_SVC_H

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTranslator>

namespace Perimeter{
class TranslateController : public QObject
{
    Q_OBJECT
//    Q_ENUMS(Language)


public:
    static QString getTranlatedName(QString name);
    static bool isRuntimeLangEng();
//    enum class Language
//    {
//        English = 1,
//        Chinese
//    };

public:
    static TranslateController* instance(/*QQmlEngine *engine*/);
//    version too low can't work.at least 5.12.
//    void retranslateUi() {
//        m_engine->retranslate();
//    }

    Q_INVOKABLE void loadLanguage(QLocale::Language lang);

//signals:
//    void message(const QString &msg);

private:
    TranslateController(/*QQmlEngine *engine*/);

//    QQmlEngine *m_engine = nullptr;
    QTranslator *m_translator = nullptr;
};



}
#endif // TRANSLATE_SVC_H
