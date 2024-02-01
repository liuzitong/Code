#ifndef PERIMETER_UtilitySvc_H
#define PERIMETER_UtilitySvc_H
#include <QPoint>
#include <QPointF>
#include <QVector>
#include <QSharedPointer>
#include <QVariant>
#include <QSize>
#include "perimeter/third-part/LimeReport/limereport/LimeReport"
namespace Perimeter
{
class UtilitySvc:public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList boundaries READ getBoundaries)
    Q_PROPERTY(int boundaryShowRange READ getBoundaryShowRange)
public:
    UtilitySvc();

    QVariantList getBoundaries();
    int getBoundaryShowRange();

    static LimeReport::ReportEngine* reportEngine;

    static QPointF convertPolarToOrth(QPointF loc);

    static int getIndex(const QPointF& dot,const QVector<QPointF>& pointLoc);

    static int getIndex(const QPointF& dot,const QVector<QPoint>& pointLoc/*,int OS_OD*/);

//    static int getIndex(const QPointF& dot,const QVector<QPointF>& pointLoc,int OS_OD);

    static bool getIsMainTable(const QPointF & loc,bool isMainTable);

    static QPointF PolarToOrth(const QPointF& dot);

    static QPointF OrthToPolar(const QPointF& dot);

    static QString getDynamicDotEnglishName(int number);

    static void wait(int msecs);

    int getExpectedDB(const QVector<int>& value_30d,QPointF loc/*,int OS_OD*/);

    QVector<int> getValue30d(int cursorSize,int cursorColor,int age);

    static QSharedPointer<UtilitySvc> getSingleton();

    static QSharedPointer<UtilitySvc> singleton;

    QVector<QPoint> m_pointLoc_30d;
    QVector<QPoint> m_pointLoc_60d;

    QVector<QPoint> m_left_blindDot;
    QVector<QPoint> m_right_blindDot;
    int m_checkCountBeforeGetBlindDotCheck;
    int m_blindDotTestDB;
    int m_blindDotTestIncDB;
    int m_nearBlindDotCheckDB;
    int m_falseNegativeDecDB;
    bool m_checkFalseNegAndPos;
    double m_VFImultiplier;
    int m_deviationLimit;
    int m_pauseCheckDeviationCount;
    float m_checkZoneRatio;
    QSize m_realTimeEyePosPicSize;
    int m_centerPointCheckedWaitingTime;
    bool m_debugMode;
    bool m_showCheckingDot;
    int m_beginningCheckDBDec;
    int m_beginningCheckDBCount;
    int m_beginningCheckResultIgnoreCount;
    int m_beginningCheckResultIgnoreDB;

    QVector<QVector<QVector<int>>> m_value_30d_cursorSize_cursorColor;
    QVector<QVector<int>> m_value_30d_cursorSizeIII_ageCorrection;     //cursorSize!=2 ,it's CursorColor
    QVector<int> m_value_60d;
    QVariantList m_boundaries;
    int m_boundaryShowRange;

};
}
#endif // UtilitySvc_H
