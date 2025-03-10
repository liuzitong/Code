﻿#ifndef GENERAL_ANALYSIS_VM_H
#define GENERAL_ANALYSIS_VM_H

#include <QAbstractListModel>
#include <QObject>
#include <QVector>
#include <perimeter/main/model/checkResultModel.h>
#include <perimeter/main/model/patientmodel.h>

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
    Q_INVOKABLE void showReport(int report,bool uploadDicom=false);
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
    Q_INVOKABLE void showReport(int report,bool uploadDicom=false);

    int getType(){return m_type;}
private:
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

enum OverViewRoles
{
    program=Qt::UserRole + 1,
    checkDate,
    strategy,
    GHT,
    OS_OD,
    centerDotCheck,
    md,
    p_md,
    psd,
    p_psd,
    dBDiagramPicPath,
    grayPicPath,
    totalDeviationPicPath,
    patternDeviationPicPath
};

struct OverViewData
{
    QString program;
    QDateTime checkDate;
    int strategy;
    int GHT;
    int OS_OD;
    bool centerDotCheck;
    float md;
    float psd;
    float p_md;
    float p_psd;
    QString dBDiagramPicPath;
    QString grayPicPath;
    QString totalDeviationPicPath;
    QString patternDeviationPicPath;
};

class OverViewListVm:public QAbstractListModel
{
    // QAbstractItemModel interface
    Q_OBJECT
public:
    OverViewListVm(QList<int> ids,int diagramWidth);
    ~OverViewListVm();
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual QHash<int,QByteArray>  roleNames( ) const override;
    PatientModel m_patient;
    QList<OverViewData> m_data;
    QString m_previewFolder="./previewImage/";
    QString m_reportFolder="./reportImage/";
};

class StaticAnalysisOverViewVm:public QObject
{
    Q_OBJECT
    Q_PROPERTY(int type READ getType)
    Q_PROPERTY(OverViewListVm* resultList READ getResultList)
public:
    Q_INVOKABLE explicit StaticAnalysisOverViewVm(const QVariantList & );
    Q_INVOKABLE virtual ~StaticAnalysisOverViewVm();
    Q_INVOKABLE void showReport(int report,QString diagnosis,bool uploadDicom=false);

    int getType(){return 3;}
    OverViewListVm* getResultList(){return m_overViewList.data();}
private:

    QString m_reportFolder="./reportImage/";
//    QList<StaticCheckResultModel> m_checkResultList;
//    QList<StaticProgramModel> m_programList;
    PatientModel m_patient;
    QSharedPointer<OverViewListVm> m_overViewList;
};

}
#endif // GENERAL_ANALYSIS_VM_H
