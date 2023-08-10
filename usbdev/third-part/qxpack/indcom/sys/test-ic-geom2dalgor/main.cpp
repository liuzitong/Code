#include <QCoreApplication>
#include <QDebug>
#include <QtTest>
#include <QByteArray>
#include <QTimer>

#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/sys/qxpack_ic_apptot_priv.hxx"
#include "qxpack/indcom/sys/qxpack_ic_appdcl_priv.hxx"
#include "qxpack/indcom/sys/qxpack_ic_geom2dalgor.hxx"

using namespace QxPack;

// ////////////////////////////////////////////////////////////////////////////
//
// test object
//
// ////////////////////////////////////////////////////////////////////////////
class  Test : public QObject {
    Q_OBJECT
private:
Q_SLOT  void  initTestCase() { }
Q_SLOT  void  cleanupTestCase();
Q_SLOT  void  test_mergeCDP();
Q_SLOT  void  test_makePolLnContour();
public :
    Test ( );
};

// ============================================================================
// ctor
// ============================================================================
    Test :: Test ( ) {  }

// ============================================================================
// final cleanup
// ============================================================================
void  Test :: cleanupTestCase()
{
    qInfo() << "current MemCntr:" << QxPack::IcMemCntr::currNewCntr();
}

// ============================================================================
// test merge CDP
// ============================================================================
void  Test :: test_mergeCDP()
{
    QList<QPointF> lst[] = {
        { QPointF(0,0), QPointF(0,0) },
        { QPointF(0,0), QPointF(1,2), QPointF(1,2)},
        { QPointF(0,0), QPointF(1,2), QPointF(1,2), QPointF(3,1)},
        { QPointF(1,2), QPointF(1,2), QPointF(1,2), QPointF(3,1)},
        { QPointF(0,0), QPointF(1,2), QPointF(1,2), QPointF(3,1), QPointF(3,1)}
    };
    QList<QPointF> rsl_lst_pt[] = {
        { QPointF(0,0) },
        { QPointF(0,0), QPointF(1,2)},
        { QPointF(0,0), QPointF(1,2), QPointF(3,1)},
        { QPointF(1,2), QPointF(3,1)},
        { QPointF(0,0), QPointF(1,2), QPointF(3,1)}
    };
    QList<int>  rsl_lst_rp[] = {
        { 2 }, { 1, 2 }, { 1, 2, 1 }, { 3, 1 }, { 1, 2, 2 }
    };

    int num = sizeof(lst)/sizeof(lst[0]);
    for ( int i = 0; i < num; i ++ ) {
        auto rsl = IcGeom2DAlgor::mergeCDP( lst[i] );
        auto &rsl_pt = rsl.first; auto &rsl_rp = rsl.second;
        QVERIFY2( rsl_pt == rsl_lst_pt[i], QString("result pt %1 list is not same.").arg(i).toUtf8().constData() );
        QVERIFY2( rsl_rp == rsl_lst_rp[i], QString("reslut rp %1 list is not same.").arg(i).toUtf8().constData() );
    }
}

// ============================================================================
// test make polygon line contour
// ============================================================================
void  Test :: test_makePolLnContour()
{
    QList<QPointF> lst[] = {
       // endp bold
       // { QPointF(0,0), QPointF(0,0), QPointF(1,0), QPointF(2,0) },
       // { QPointF(0,0), QPointF(0,0), QPointF(2,0), QPointF(2,0) },
       // { QPointF(0,0), QPointF(0,0), QPointF(0,0), QPointF(0,0) }
       { QPointF(0,0) },
       { QPointF(0,0), QPointF(1,0)}

       // endp not bold
//       { QPointF(0,0) },
//       { QPointF(0,0), QPointF(1,0)}

    };
    double lst_sc[] = { 2, 2 };
    bool   lst_bold[] = { false, false };

    int num = sizeof(lst)/sizeof(lst[0]);
    for ( int i = 0; i < num; i ++ ) {
        auto rsl = IcGeom2DAlgor::makePolLnContour( lst[i], lst_sc[i], lst_bold[i] );
        qInfo() << rsl;
    }
}

QTEST_MAIN( Test )
#include "main.moc"
