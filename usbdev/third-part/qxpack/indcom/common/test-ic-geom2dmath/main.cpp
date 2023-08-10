#include <QCoreApplication>
#include <qxpack/indcom/common/qxpack_ic_memcntr.hxx>
#include <QDebug>
#include <cstdlib>
#include <QObject>
#include <QtTest>
#include <string>
#include <cmath>
#include <QString>
#include <QElapsedTimer>
#include <QVector2D>
#include <QLineF>
#include <QPointF>

#include "qxpack/indcom/common/qxpack_ic_global.hxx"
#include "qxpack/indcom/common/qxpack_ic_geom2dmath.hxx"
#include "qxpack/indcom/common/qxpack_ic_geom2d.hxx"

using namespace QxPack;

// ////////////////////////////////////////////////////////////////////////////
//
// test object
//
// ////////////////////////////////////////////////////////////////////////////
class  Test : public QObject {
    Q_OBJECT
private:
Q_SLOT  void  initTestCase( ) {  }
Q_SLOT  void  cleanupTestCase();
Q_SLOT  void  testAntiClockedVertVector();
Q_SLOT  void  testFindCrossPoint();
public :
    Test ( ) { }
    ~Test( ) { }
};

// ============================================================================
// final cleanup
// ============================================================================
void  Test :: cleanupTestCase()
{
    qInfo() << "current MemCntr:" <<
            QxPack::IcMemCntr::currNewCntr();

}

// =============================================================================
// test the vertical of spec. vector
// =============================================================================
void   Test :: testAntiClockedVertVector()
{
    int mem_cntr = IcMemCntr::currNewCntr();
    {
        // zero point veritcal vector is self.
        IcVector2Dfp rsl = IcGeom2DMath::antiClockwiseVertVector( IcVector2Dfp(0,0), true );
        QVERIFY2( fuzzyCompare( rsl, IcVector2Dfp(0,0) ), "vector(0,0) vertical vector is not null!");
        rsl = IcGeom2DMath::antiClockwiseVertVector( IcVector2Dfp(0, 5), false );
        QVERIFY2( fuzzyIsNull( rsl.y() ) && rsl.x() < 0, "eval vec(0,5)'s vertical vec failed.");
        rsl = IcGeom2DMath::antiClockwiseVertVector( IcVector2Dfp(0,-5), false );
        QVERIFY2( fuzzyIsNull( rsl.y() ) && rsl.x() > 0, "eval vec(0,-5)'s vertical vec failed.");
        rsl = IcGeom2DMath::antiClockwiseVertVector( IcVector2Dfp(5,0), false );
        QVERIFY2( fuzzyIsNull( rsl.x() ) && rsl.y() > 0, "eval vec(5,0)'s vertical vec failed" );
        rsl = IcGeom2DMath::antiClockwiseVertVector( IcVector2Dfp(-5,0), false );
        QVERIFY2( fuzzyIsNull( rsl.x() ) && rsl.y() < 0, "eval vec(-5,0)'s vertical vec failed");
        rsl = IcGeom2DMath::antiClockwiseVertVector( IcVector2Dfp( 4, 5), false );
        QVERIFY2( rsl.x() < 0 && rsl.y() > 0, "eval vec(4,5)'s vertical vec failed" );
        rsl = IcGeom2DMath::antiClockwiseVertVector( IcVector2Dfp(-4, 5), false );
        QVERIFY2( rsl.x() < 0 && rsl.y() < 0, "eval vec(-4,5)'s vertical vec failed" );
        rsl = IcGeom2DMath::antiClockwiseVertVector( IcVector2Dfp(-4,-5), false );
        QVERIFY2( rsl.x() > 0 && rsl.y() < 0, "eval vec(-4,-5)'s vertical vec failed" );
        rsl = IcGeom2DMath::antiClockwiseVertVector( IcVector2Dfp( 4,-5), false );
        QVERIFY2( rsl.x() > 0 && rsl.y() > 0, "eval vec(4,-5)'s vertical vec failed" );

        rsl = IcGeom2DMath::antiClockwiseVertVector( IcVector2Dfp(0, 5), true );
        QVERIFY2( fuzzyIsNull( rsl.y()) && rsl.x() < 0, "eval vec(0,5)'s norm vertical vec failed.");
        rsl = IcGeom2DMath::antiClockwiseVertVector( IcVector2Dfp(0,-5), true );
        QVERIFY2( fuzzyIsNull( rsl.y()) && rsl.x() > 0, "eval vec(0,-5)'s norm vertical vec failed.");
        rsl = IcGeom2DMath::antiClockwiseVertVector( IcVector2Dfp(5,0), true );
        QVERIFY2( fuzzyIsNull( rsl.x()) && rsl.y() > 0, "eval vec(5,0)'s norm vertical vec failed" );
        rsl = IcGeom2DMath::antiClockwiseVertVector( IcVector2Dfp(-5,0), true );
        QVERIFY2( fuzzyIsNull( rsl.x()) && rsl.y() < 0, "eval vec(-5,0)'s norm vertical vec failed");
        rsl = IcGeom2DMath::antiClockwiseVertVector( IcVector2Dfp( 4, 5), true );
        QVERIFY2( rsl.x() < 0 && rsl.y() > 0, "eval vec(4,5)'s norm vertical vec failed" );
        rsl = IcGeom2DMath::antiClockwiseVertVector( IcVector2Dfp(-4, 5), true );
        QVERIFY2( rsl.x() < 0 && rsl.y() < 0, "eval vec(-4,5)'s norm vertical vec failed" );
        rsl = IcGeom2DMath::antiClockwiseVertVector( IcVector2Dfp(-4,-5), true );
        QVERIFY2( rsl.x() > 0 && rsl.y() < 0, "eval vec(-4,-5)'s norm vertical vec failed" );
        rsl = IcGeom2DMath::antiClockwiseVertVector( IcVector2Dfp( 4,-5), true );
        QVERIFY2( rsl.x() > 0 && rsl.y() > 0, "eval vec(4,-5)'s norm vertical vec failed" );
    }

    QVERIFY2( IcMemCntr::currNewCntr() == mem_cntr, "exist memory leak!" );
}

// =============================================================================
// test find the cross point
// =============================================================================
void   Test :: testFindCrossPoint()
{
    int mem_cntr = IcMemCntr::currNewCntr();
    {
//        IcLine2Dfp ln[][2] = {
//            { IcLine2Dfp( 0, 0, 1, 0 ), IcLine2Dfp( 1, 0, 2, 0 ) },
//            { IcLine2Dfp( 0, 0, 1, 0 ), IcLine2Dfp( 2, 0, 3, 0 ) },
//            { IcLine2Dfp(  )}
//        };
//        int cpt_ret[] = {
//            0,
//        };

//        int num = sizeof(pts)/sizeof(IcPoint2Dfp)/3;
//        for ( int i = 0; i < num; i ++ ) {
//            IcPoint2Dfp act_pt[3], cct_pt[3];
//            int ctp_ret = IcGeom2DMath::findCrossPoint(  );
//        }
    }

    QVERIFY2( IcMemCntr::currNewCntr() == mem_cntr, "exist memory leak!" );
}





QTEST_MAIN( Test )
#include "main.moc"
