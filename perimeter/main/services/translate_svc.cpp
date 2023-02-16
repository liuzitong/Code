#include "translate_svc.h"
#include <perimeter/third-part/qxpack/indcom/ui_qml_base/qxpack_ic_ui_qml_api.hxx>
#include <perimeter/main/appctrl/perimeter_appctrl.hxx>
#include <perimeter/main/viewModel/settings.h>

Perimeter::TranslateController *Perimeter::TranslateController::instance()
{
    static TranslateController controller/*(engine)*/;
    return &controller;
}

void Perimeter::TranslateController::loadLanguage(QLocale::Language lang) {
    switch (int(lang)) {
    case QLocale::Chinese:
        if (m_translator->load("./perimeter_zh_CN.qm")) {
//            emit message(tr("--- Language changed to Chinese"));
        }
        break;
    default:
        m_translator->load("");
//        emit message(tr("--- Language changed to English"));
        break;
    }

//    emit triggerChanged();
    //        retranslateUi();
}

Perimeter::TranslateController::TranslateController() {
    //        m_engine = engine;
    m_translator = new QTranslator(this);
    QCoreApplication::installTranslator(m_translator);
}

QString Perimeter::TranslateController::getTranlatedName(QString name)
{
    auto splitNames=name.split("@");
    if(splitNames.count()!=2)
    {
        return name;
    }
    else
    {
        return isRuntimeLangEng()?splitNames[0]:splitNames[1];
    }
}


bool Perimeter::TranslateController::isRuntimeLangEng()
{
    auto appCtrl=static_cast<AppCtrl*>(QxPack::IcUiQmlApi::appCtrl());
    auto setting=static_cast<Settings*>(appCtrl->getSettings());
    auto lang=setting->getLanguage();
    return setting->getIsRuntimeLangEng();
}
