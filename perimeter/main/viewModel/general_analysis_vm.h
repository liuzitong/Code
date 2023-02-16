#ifndef GENERAL_ANALYSIS_VM_H
#define GENERAL_ANALYSIS_VM_H

#include <QObject>
#include <QVector>
#include <perimeter/main/model/checkResultModel.h>
#include <perimeter/main/model/patientmodel.h>
#include "perimeter/third-part/LimeReport/limereport/LimeReport"
namespace Perimeter {



class StaticAnalysisVm: public QObject
{
    Q_OBJECT
    Q_PROPERTY(int type READ getType)
    Q_PROPERTY(int selectedDotIndex READ getSelectedDotIndex NOTIFY selectedDotIndexChanged)
public:
    Q_INVOKABLE explicit StaticAnalysisVm(const QVariantList & );
    Q_INVOKABLE virtual ~StaticAnalysisVm();
    Q_INVOKABLE QPointF getClickDot(float MouseX,float MouseY,float width,float height);
    Q_INVOKABLE QPointF getPixFromPoint(QPointF point,float width,float height);
    Q_INVOKABLE int getSelectedDotIndex(){return m_selectedDotIndex;}Q_SIGNAL void selectedDotIndexChanged();
    Q_INVOKABLE void showReport(int report);
    Q_INVOKABLE QObject* getResult();

    int getType(){return m_type;}
private:

    QVector<int> m_values,m_fixationValues,m_dev,m_mDev,m_peDev,m_peMDev;
    QVector<QPointF> m_locs;
    float m_md,m_psd,m_VFI,m_p_md,m_p_psd;
    int m_type,m_GHT,m_innerRange,m_range,m_OS_OD,m_selectedDotIndex=-1;
    int m_dotSeen,m_dotWeakSeen,m_dotUnseen;
    QString m_previewFolder="./previewImage/";
    QString m_reportFolder="./reportImage/";

    StaticCheckResultModel m_checkResult;
    StaticProgramModel m_program;
    PatientModel m_patient;
};

class DynamicAnalysisVm: public QObject
{
    Q_OBJECT
    Q_PROPERTY(int type READ getType)
public:
    Q_INVOKABLE explicit DynamicAnalysisVm(const QVariantList & );
    Q_INVOKABLE virtual ~DynamicAnalysisVm();
    Q_INVOKABLE void showReport(int report);

    int getType(){return m_type;}
private:
    LimeReport::ReportEngine* m_reportEngine=nullptr;
    QString m_previewFolder="./previewImage/";
    QString m_reportFolder="./reportImage/";
    QVector<QPointF> m_values;
    QVector<QString> m_dotNames;
    QVector<int> m_fixationValues;
    int m_type,m_range;
    DynamicCheckResultModel m_checkResult;
    DynamicProgramModel m_program;
    PatientModel m_patient;
};


}
#endif // GENERAL_ANALYSIS_VM_H
