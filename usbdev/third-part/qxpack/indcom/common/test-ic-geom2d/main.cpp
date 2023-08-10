#include <QCoreApplication>
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
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
#include <QString>

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
Q_SLOT  void  testPoint2D();
Q_SLOT  void  testVector2D();
Q_SLOT  void  testVector2D_isAtACCW();
Q_SLOT  void  testLine2D();
Q_SLOT  void  testLine2D_findCpt();
Q_SLOT  void  testLine2D_scaled();
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

// ============================================================================
//  test point 2d
// ============================================================================
void                    Test :: testPoint2D()
{
    // ctor test
    IcPoint2Dfp  pt(0,1);
    QVERIFY2( fuzzyIsNull( pt.x() ) && fuzzyCompare(pt.y(),1.0), "ctor failed!" );
    IcPoint2D<int> pt_i1(1,2), pt_i2(1,2);
    QVERIFY2( pt_i1 == pt_i2, "for int type, failed to override operator == " );
    IcPoint2D<double> pt_d1(1,2), pt_d2(1,2);
    QVERIFY2( pt_d1 == pt_d2, "for double type, failed to override operator == " );
}

// ============================================================================
//  test vector 2d
// ============================================================================
void                    Test :: testVector2D()
{
    IcVector2Dfp a(0.0,1.0);
    QVERIFY2( fuzzyIsNull( a.x() ) && fuzzyCompare(a.y(),1.0), "ctor directly failed");
    IcVector2Dfp b( IcPoint2Dfp(0.0,1.0), IcPoint2Dfp(2.0,3.0) );
    QVERIFY2( fuzzyCompare(b.x(),2.0) && fuzzyCompare(b.y(),2.0), "ctor by pts failed" );
    IcVector2Dfp r_add = IcVector2Dfp(1.0,2.0) + IcVector2Dfp(2.0,2.0);
    QVERIFY2( fuzzyCompare(r_add.x(),3.0) && fuzzyCompare(r_add.y(),4.0), "opr + failed" );
    IcVector2Dfp r_sub = IcVector2Dfp(3.0,2.0) - IcVector2Dfp(1.0,2.0);
    QVERIFY2( fuzzyCompare(r_sub.x(),2.0) && fuzzyCompare(r_sub.y(),0.0), "opr - failed" );
    double r_dot = IcVector2Dfp::dotProduct( IcVector2Dfp(1.0,3.0), IcVector2Dfp(2.0,4.0));
    QVERIFY2( fuzzyCompare(r_dot, 1.0*2.0+3.0*4.0 ), "dot product failed" );
    double r_crs = IcVector2Dfp::crossProduct( IcVector2Dfp(3.0,4.0), IcVector2Dfp(5.0,6.0));
    QVERIFY2( fuzzyCompare(r_crs, 3.0*6.0-4.0*5.0 ), "cross product failed" );
    IcVector2Dfp r_norm( 1, 1 ); r_norm.normalize();
    QVERIFY2( fuzzyCompare( r_norm.x() * r_norm.x() + r_norm.y() * r_norm.y(), 1.0 ), "normlize function failed." );
    IcVector2Dfp r_scale = IcVector2Dfp(1.0,2.0).scaled(3.0);
    QVERIFY2(
        fuzzyCompare(r_scale.x(), 1.0*3.0 ) && fuzzyCompare(r_scale.y(), 2.0*3.0 ),
        "scale failed"
    );

    {  // anti clocked vert vector test
        IcVector2Dfp vec[]    = {{1.2,0.0},{1.3,0.1},{0,1},{-0.1,3},{-0.1,0},{-0.1,-1.1},{0,-4},{0.2,-3}};
        int num = sizeof(vec)/sizeof(IcVector2Dfp);
        for ( int i = 0; i < num; i ++ ) {
            IcVector2Dfp vv = vec[i].ccwVertVector( true );
            double dot_v = IcVector2Dfp::dotProduct( vv, vec[i] );
            QVERIFY2( fuzzyIsNull( dot_v, 9 ), "eval anti-vector failed");
        }
    }
}

// ============================================================================
// test if vector is at ACCW
// ============================================================================
void                    Test :: testVector2D_isAtACCW()
{
    IcVector2Dfp pair[][2] = {
       // false cases
       { IcVector2Dfp(0,0), IcVector2Dfp(0,0) },  { IcVector2Dfp(0,0), IcVector2Dfp(1,0)},
       { IcVector2Dfp(1,0), IcVector2Dfp(0,0) },  { IcVector2Dfp(0,1), IcVector2Dfp(0,1)},
       { IcVector2Dfp(1,0), IcVector2Dfp(-1,0)},  { IcVector2Dfp(1,0), IcVector2Dfp(0,1)},
       { IcVector2Dfp(-1,-1), IcVector2Dfp(1,0)}, { IcVector2Dfp(-1,-1), IcVector2Dfp(1,1) },

       // true cases
       { IcVector2Dfp(0,1), IcVector2Dfp(1,0)}, { IcVector2Dfp(1,1), IcVector2Dfp(1,0)},
       { IcVector2Dfp(-1,1),IcVector2Dfp(1,0)}, { IcVector2Dfp(-1,0.0001), IcVector2Dfp(1,0)},
       { IcVector2Dfp(1,0), IcVector2Dfp(1,0) }
    };
    bool  rsl[] = {
       false,false,false,false,  false,false,false,false,
       true, true, true, true,   true
    };

    int num = sizeof( rsl ) / sizeof( bool );
    for ( int i = 0; i < num; i ++ ) {
       bool ret = pair[i][0].isAtCCWdirOf( pair[i][1] );
       QVERIFY2(
           ret == rsl[i],
           QString("vec(%1,%2) accw of vec(%3,%4), correct rsl is %5, but return %6")
                .arg( pair[i][0].x()).arg( pair[i][0].y() ).arg( pair[i][1].x()).arg( pair[i][1].y() )
                .arg( rsl[i] ? "true" : "false" ).arg( ret ? "true" : "false" )
                .toUtf8().constData()
       );
    }
}

// ============================================================================
// test the line 2D
// ============================================================================
void                    Test :: testLine2D()
{
    IcLine2Dfp a( 0, 0, 2, 4 );
    QVERIFY2( a.p1().isNull() && a.p2() == IcPoint2Dfp(2,4), "ctor failed" );

    { // for move the line
        IcLine2Dfp mv_ln[] = {
            IcLine2Dfp( 0,0, 3,0 )
        };
        double mv_vec_sz[] = { 3 };
        int num = sizeof(mv_ln)/sizeof(IcLine2Dfp);

        for ( int i = 0; i < num; i ++ ) {
            IcLine2Dfp r_mv = mv_ln[i].moved( mv_ln[i].vector().ccwVertVector( true ).scale( mv_vec_sz[i]) );
           // qInfo("moved ln(%lf,%lf,%lf,%lf)", r_mv.x1(), r_mv.y1(), r_mv.x2(), r_mv.y2());
        }
    }
}

// ============================================================================
// test the line 2D cross pt
// ============================================================================
void                    Test :: testLine2D_findCpt()
{
    IcLine2Dfp pair[][2] = {
        // false case
        { IcLine2Dfp(0,0,1,0), IcLine2Dfp(1,0,2,0) }, { IcLine2Dfp(0,0,1,0), IcLine2Dfp(2,0,3,0) },

        // true case
        { IcLine2Dfp(0,0,1,0), IcLine2Dfp(0.3,1,0.3,9)}, { IcLine2Dfp(0.3,1,0.3,9), IcLine2Dfp(0,0,1,0) },
        { IcLine2Dfp(-1,0,1,0),IcLine2Dfp(0,1,0,-1) },   { IcLine2Dfp(0,0,1,0), IcLine2Dfp(2,2,2,9) }
    };
    int  rsl[] = { -1, -1, 1, 2,3, 0 };
    int  num = sizeof(rsl) / sizeof(rsl[0]);
    
    for ( int i = 0; i < num; i ++ ) {
        IcPoint2Dfp cpt;
        int cpt_ret = IcLine2Dfp::findCrossPoint( cpt, pair[i][0], pair[i][1] ); 
        QVERIFY2( 
            cpt_ret == rsl[i], 
            QString("ln(%1,%2,%3,%4) and ln(%5,%6,%7,%8) has cross pt rsl(%9), but ret(%10)")
            .arg( pair[i][0].x1()).arg( pair[i][0].y1()).arg( pair[i][0].x2()).arg( pair[i][0].y2())
            .arg( pair[i][1].x1()).arg( pair[i][1].y1()).arg( pair[i][1].x2()).arg( pair[i][1].y2())
            .arg( rsl[i] ).arg( cpt_ret )
            .toUtf8().constData()
        );
//        qInfo("cpt(%lf,%lf)", cpt.x(), cpt.y());
    }
}

// ============================================================================
// test the line 2d scaled
// ============================================================================
void                    Test :: testLine2D_scaled()
{
    IcLine2Dfp sc_ln[] = {
        IcLine2Dfp(0,0,1,0),
        IcLine2Dfp(1,1,2,2),   IcLine2Dfp(0,1,2,3),
        IcLine2Dfp(-1,-2,9,9), IcLine2Dfp(9,8,1,2),
        IcLine2Dfp(9,9,-3,-5)
    };
    double sc_fact[] = { 2, 2, 3, 0.5, 1.2, 0.9 };
    int num = sizeof(sc_fact)/sizeof(double);

    for ( int i = 0; i < num; i ++ ) {
        IcLine2Dfp r_sc = sc_ln[i].scaled( sc_fact[i] );
        double r_sc_len = r_sc.length();
        double ln_len   = sc_ln[i].length();
//        qInfo("sc %lf,%lf,%lf,%lf ln_len:%lf  sc_len:%lf  sc_fact:%lf",
//             r_sc.p1().x(), r_sc.p1().y(), r_sc.p2().x(), r_sc.p2().y(),
//             ln_len, r_sc_len, r_sc_len/ln_len
//        );
        QVERIFY2( fuzzyCompare( r_sc_len / ln_len, abs( sc_fact[i] )), QString("scaled test failed, (%1) obj.").arg(i).toUtf8().constData() );
    }
}



QTEST_MAIN( Test )
#include "main.moc"
