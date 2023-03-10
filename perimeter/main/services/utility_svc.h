#ifndef PERIMETER_UtilitySvc_H
#define PERIMETER_UtilitySvc_H
#include <QPoint>
#include <QPointF>
#include <QVector>
#include <QSharedPointer>
namespace Perimeter
{
class UtilitySvc
{
public:
    UtilitySvc();


    static QPointF convertPolarToOrth(QPointF loc);

    static int getIndex(const QPointF& dot,const QVector<QPointF>& pointLoc);

    static int getIndex(const QPointF& dot,const QVector<QPoint>& pointLoc,int OS_OD);

    static int getIndex(const QPointF& dot,const QVector<QPointF>& pointLoc,int OS_OD);

    static void wait(int msecs);

    int getExpectedDB(const QVector<int>& value_30d,QPointF loc,int OS_OD);

    QVector<int> getValue30d(int cursorSize,int cursorColor,int age);

    static QSharedPointer<UtilitySvc> getSingleton();

    static QSharedPointer<UtilitySvc> singleton;

    QVector<QPoint> m_pointLoc_30d;
    QVector<QPoint> m_pointLoc_60d;

    QVector<QPoint> m_left_blindDot;
    QVector<QPoint> m_right_blindDot;
    int m_checkCountBeforeGetBlindDotCheck;
    int m_blindDotTestDB;
    int m_falsePositiveDecDB;
    double m_VFImultiplier;

    QVector<QVector<QVector<int>>> m_value_30d_cursorSize_cursorColor;
    QVector<QVector<int>> m_value_30d_cursorSizeIII_ageCorrection;     //cursorSize!=2 ,it's CursorColor
    QVector<int> m_value_60d;

};
}
#endif // UtilitySvc_H
