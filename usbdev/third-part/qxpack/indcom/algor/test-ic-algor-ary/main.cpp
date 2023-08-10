#include <QCoreApplication>

#include "../qxpack_ic_algor_ary.hpp"
#include "../qxpack_ic_algor_errdev.hpp"
#include <functional>

#include <QString>
#include <QDebug>
#include <QtTest>
#include <QThread>
#include <QList>
#include <cstdlib>
#include <cstring>
#include <QTime>
#include <QVector>

using namespace QxPack;

// ////////////////////////////////////////////////////////////////////////////
//
//   test the error deviation template
//
// ////////////////////////////////////////////////////////////////////////////
class  Tester : public QObject {
    Q_OBJECT
private:
    Q_SLOT  void  initTestCase( ) { }
    Q_SLOT  void  cleanupTestCase ( );
    Q_SLOT  void  revAryOrder( );
    Q_SLOT  void  mirrorExtendAry( );
    Q_SLOT  void  maxInAryRef();
    Q_SLOT  void  dotMulAry();
    Q_SLOT  void  mirrorExtendAryHalf();
    Q_SLOT  void  linearInterpolAry();
    Q_SLOT  void  linearInterpolAryToSmall();
    Q_SLOT  void  linearInterpolAry2();
    Q_SLOT  void  mirrorExtendAryHalfBAP();
    Q_SLOT  void  xorAry();
    Q_SLOT  void  sumAry();
    Q_SLOT  void  svrAry();
    Q_SLOT  void  rolAry();
    Q_SLOT  void  clampAry();
public :

};

// ============================================================================
// output the memory counter
// ============================================================================
auto     Tester :: cleanupTestCase( ) -> void
{

}

// ============================================================================
// reverse the array order
// ============================================================================
auto     Tester :: revAryOrder() -> void
{
    QVector<int> data_vec[] = {
        { 0, 1, 2, 4 }, { 0, 1 }, { 1 }, { 1,2,3},
        { 1, 2, 3, 4, 5, 6, 7, 8 },    
        { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },
        {
          1,   2, 3,  4, 5, 6, 7, 8, 9, 10,
          11, 12, 13, 14, 15, 16, 17, 18
        }
    };
    QVector<int> rsl_vec[] = {
        { 4, 2, 1, 0 }, { 1, 0 }, { 1 },{3,2,1},
        { 8, 7, 6, 5, 4, 3, 2, 1 },
        { 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 },
        {
          18, 17, 16, 15, 14, 13, 12, 11, 10, 9,
          8,  7,  6, 5, 4, 3, 2, 1
        }
    };

    for ( size_t i = 0; i < sizeof(data_vec)/sizeof(data_vec[0]); i ++ ) {
        QVector<int> rsl( data_vec[i].size(), 0 );
        IcAlgor::revAryOrder( data_vec[i].constData(), rsl.data(), size_t( rsl.size()));
        QVERIFY2( rsl_vec[i] == rsl, "rev. src to dst failed!" );
      //  qInfo() << data_vec[i];
      //  qInfo() << rsl;
        IcAlgor::revAryOrder( rsl.data(), size_t( rsl.size()) );
        QVERIFY2( data_vec[i] == rsl, "rev. self failed!");
     //   qInfo() << rsl;
    }
    
}

// ============================================================================
// mirror the extend array
// ============================================================================
auto     Tester :: mirrorExtendAry( ) -> void
{
    QVector<int> dat1 = { 0, 1, 2, 3 };
    QVector<int> mirror_dat1 = { 3, 2, 1, 0 };
    QVector<int> rsl( dat1.size() * 3 );

    IcAlgor::mirrorExtendAry( dat1.constData(), size_t( dat1.size()), rsl.data(), size_t( rsl.size()) );
    QVERIFY2(
        std::memcmp( rsl.constData(), mirror_dat1.constData(), size_t( dat1.size()) * sizeof(rsl[0]) ) == 0,
        "bad front seg rsl data !"
    );
    QVERIFY2(
        std::memcmp( rsl.constData() + dat1.size(), dat1.constData(), size_t( dat1.size() ) * sizeof(rsl[0]) )== 0,
        "bad middle seg. rsl data!"
    );
    QVERIFY2(
        std::memcmp( rsl.constData() + dat1.size() * 2, mirror_dat1.constData(), size_t( dat1.size() ) * sizeof(rsl[0])) == 0,
        "bad tail seg. rsl data!"
    );
}

// ============================================================================
// mirror the extend array half
// ============================================================================
auto     Tester :: mirrorExtendAryHalf() -> void
{
    QVector<int> data_vec[] = {
        { 0, 1, 2, 4 }, { 0, 1 }, { 1 }, { 1, 2, 3 }, { 1, 2, 3, 4, 5 }
    };
    QVector<int> rsl_vec[] = {
        { 1, 0, 0, 1, 2, 4, 4, 2 }, { 0, 0, 1, 1 }, { 1, 1, 1 }, { 1, 1, 2, 3, 3 }, { 2, 1, 1, 2, 3, 4, 5, 5, 4 }
    };

    for ( int i = 0; i < int( sizeof(data_vec)/sizeof(data_vec[0])); i ++ ) {
        auto &src = data_vec[i];
        auto &rsl = rsl_vec[i];
        QVector<int> tmp( rsl.size() );
        IcAlgor::mirrorExtendAryHalf( src.constData(), size_t( src.size()), tmp.data(), size_t( tmp.size()) );
        auto is_same = std::memcmp( tmp.constBegin(), rsl.constData(), size_t( rsl.size() ) * sizeof(rsl[0]));
        if ( is_same != 0 ) {
            qInfo() << "src:" << src;
            qInfo() << "tmp:" << tmp;
        }
        QVERIFY2(
            is_same == 0, "bad result!"
        );
    }
}

// ============================================================================
// mirror the extend array half
// ============================================================================
auto     Tester :: mirrorExtendAryHalfBAP() -> void
{
    QVector<int> data_vec[] = {
        { 0, 1, 2, 4 }, { 0, 1 }, { 1 }, { 1, 2, 3 }, { 1, 2, 3, 4, 5 }
    };
    QVector<int> rsl_vec[] = {
        { 2, 1, 0, 1, 2, 4, 2, 1 }, { 1, 0, 1, 0 }, { 1, 1, 1 }, { 2, 1, 2, 3, 2 },
        { 3, 2, 1, 2, 3, 4, 5, 4, 3 }
    };

    for ( int i = 0; i < int( sizeof(data_vec)/sizeof(data_vec[0])); i ++ ) {
        auto &src = data_vec[i];
        auto &rsl = rsl_vec[i];
        QVector<int> tmp( rsl.size() );
        IcAlgor::mirrorExtendAryHalfBAP( src.constData(), size_t( src.size()), tmp.data(), size_t( tmp.size()) );
        auto is_same = std::memcmp( tmp.constBegin(), rsl.constData(), size_t( rsl.size() ) * sizeof(rsl[0]));
        if ( is_same != 0 ) {
            qInfo() << "src:" << src;
            qInfo() << "tmp:" << tmp;
        }
        QVERIFY2(
            is_same == 0, "bad result!"
        );
    }
}

// ============================================================================
// max value in the array
// ============================================================================
auto     Tester :: maxInAryRef() -> void
{
    QVector<int> dat = { 0, 7, 99, -23, 6 };
    auto v = IcAlgor::maxInAryRef( dat.constData(), size_t( dat.size()) );
    QVERIFY2( v.first == 2,"");
}

// ============================================================================
// apply structure element on array
// ============================================================================
auto     Tester :: dotMulAry() -> void
{
    QVector<int>     ary1 = { 10, 10, 10, 10, 10 };
    QVector<double>  ary2 = { 0.2, 0.5, 0.7, 0.5, 0.2 };
    QVector<int>     rsl  = { 2, 5, 7, 5, 2 };
    class Clamp {
    public: inline auto  operator()( const double &r ) const ->int { return int( IcAlgor::clamp( r, 0.0, 10.0 )); }
    };

    IcAlgor::dotMulAry( ary1.data(), ary1.constData(), ary2.constData(), size_t( ary1.size() ), Clamp() );
    QVERIFY2( std::memcmp( ary1.constData(), rsl.constData(), size_t( ary1.size()) * sizeof(ary1[0])) == 0, "bad result" );
}

// ============================================================================
// linear interpolation
// ============================================================================
auto     Tester :: linearInterpolAry() -> void
{
    QVector<float>  ary0 = { 0, 1, 2 };
    QVector<float>  rsl0 = { 0, 0.4f, 0.8f, 1.2f, 1.6f, 2.0f };
    QVector<float>  ary1 = { 0, 1, 2 };
    QVector<float>  rsl1 = { 0, 0.5f, 1, 1.5f, 2 };
    QVector<float>  ary2 = { 0, 1, 2 };
    QVector<float>  rsl2 = { 0, 2 };
    QVector<float>  ary3 = { 7 };
    QVector<float>  rsl3 = { 7, 7 };
    QVector<float>  ary4 = { 0, 1 };
    QVector<float>  rsl4 = { 0, 0.5f, 1 };

    QVector<float>  t_rsl0( rsl0.size() );
    IcAlgor::linearInterpolAry( ary0.constData(), size_t(ary0.size()), t_rsl0.data(), size_t(t_rsl0.size()) );
    QVERIFY2( t_rsl0 == rsl0, "failed to linear interpolation!" );

    QVector<float>  t_rsl1( rsl1.size() );
    IcAlgor::linearInterpolAry( ary1.constData(), size_t(ary1.size()), t_rsl1.data(), size_t(t_rsl1.size()) );
    QVERIFY2( t_rsl1 == rsl1, "failed to linear interpolation!" );

    QVector<float>  t_rsl2( rsl2.size() );
    IcAlgor::linearInterpolAry( ary2.constData(), size_t(ary2.size()), t_rsl2.data(), size_t(t_rsl2.size()) );
    QVERIFY2( t_rsl2 == rsl2, "failed to linear interpolation!" );

    QVector<float>  t_rsl3( rsl3.size() );
    IcAlgor::linearInterpolAry( ary3.constData(), size_t(ary3.size()), t_rsl3.data(), size_t(t_rsl3.size()) );
    QVERIFY2( t_rsl3 == rsl3, "failed to linear interpolation!" );

    QVector<float>  t_rsl4( rsl4.size() );
    IcAlgor::linearInterpolAry( ary4.constData(), size_t(ary4.size()), t_rsl4.data(), size_t(t_rsl4.size()) );
    QVERIFY2( t_rsl4 == rsl4, "failed to linear interpolation!" );

    //auto t_ret0 = IcAlgor::isErrDevLess( rsl0.constData(), t_rsl0.constData(), 0.0001f, size_t( rsl0.size()));
    //QVERIFY2( t_ret0, "bad result!" );
}

// ============================================================================
// lerp array to small
// ============================================================================
auto     Tester :: linearInterpolAryToSmall() -> void
{
    QVector<float>  ary0 = { 0, 1, 2, 3, 4, 5 };
    QVector<float>  rsl0 = { 0, 0, 0, 0, 5 };

    QVector<float> t_rsl0( rsl0.size());
    IcAlgor::linearInterpolAry( ary0.constData(), size_t(ary0.size()), t_rsl0.data(), size_t( t_rsl0.size() ));
    qInfo() << t_rsl0;
}


// ============================================================================
// linear interpolation
// ============================================================================
auto     Tester :: linearInterpolAry2() -> void
{
    QVector<int>    ary0 = { 0, 1, 2 };
    QVector<float>  rsl0 = { 0, 0.4f, 0.8f, 1.2f, 1.6f, 2.0f };
    QVector<int>    ary1 = { 0, 1, 2 };
    QVector<float>  rsl1 = { 0, 0.5f, 1, 1.5f, 2 };
    QVector<int>    ary2 = { 0, 1, 2 };
    QVector<float>  rsl2 = { 0, 2 };
    QVector<int>    ary3 = { 7 };
    QVector<float>  rsl3 = { 7, 7 };

    QVector<float>  t_rsl0( rsl0.size() );
    IcAlgor::linearInterpolAry2( ary0.constData(), size_t(ary0.size()),  t_rsl0.data(), size_t(t_rsl0.size()) );
    QVERIFY2( t_rsl0 == rsl0, "failed to linear interpolation!" );

    QVector<float>  t_rsl1( rsl1.size() );
    IcAlgor::linearInterpolAry2( ary1.constData(), size_t(ary1.size()), t_rsl1.data(), size_t(t_rsl1.size()) );
    QVERIFY2( t_rsl1 == rsl1, "failed to linear interpolation!" );

    QVector<float>  t_rsl2( rsl2.size() );
    IcAlgor::linearInterpolAry2( ary2.constData(), size_t(ary2.size()), t_rsl2.data(), size_t(t_rsl2.size()) );
    QVERIFY2( t_rsl2 == rsl2, "failed to linear interpolation!" );

    QVector<float>  t_rsl3( rsl3.size() );
    IcAlgor::linearInterpolAry2( ary3.constData(), size_t(ary3.size()), t_rsl3.data(), size_t(t_rsl3.size()) );
    QVERIFY2( t_rsl3 == rsl3, "failed to linear interpolation!" );

    //auto t_ret0 = IcAlgor::isErrDevLess( rsl0.constData(), t_rsl0.constData(), 0.0001f, size_t( rsl0.size()));
    //QVERIFY2( t_ret0, "bad result!" );
}

// ============================================================================
// xor the array
// ============================================================================
auto     Tester :: xorAry() -> void
{
    uint8_t  dat1[]    = { 1,       2, 0x03, 0x04, 0x05, 0x5a };
    uint8_t  rsl1_ff[] = { 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xa5 };

    uint8_t  t_rsl1[ sizeof(dat1) ];
    std::memcpy( t_rsl1, dat1, sizeof(dat1));
    IcAlgor::xorAry( t_rsl1, uint8_t( 0xff ), sizeof(dat1) );
    auto ret1 = std::memcmp( rsl1_ff, t_rsl1, sizeof(rsl1_ff));
    QVERIFY2( ret1 == 0, "xorAry failed!" );
}

// ============================================================================
// eval sum the array
// ============================================================================
auto     Tester :: sumAry() -> void
{
    QVector<int> dat[] = {
        { 1, 2, 3, 4, 5 },
        { 1, 2, 3, 4, 5, 7, 8, 9, 10 }
    };
    for ( size_t i = 0; i < sizeof(dat)/sizeof(dat[0]); i ++ ) {
        auto sum = IcAlgor::sumAry( dat[i].constData(), size_t(dat[i].size()));

        auto rsl = int(0);
        for ( size_t k = 0; k < size_t( dat[i].size()); k ++ ) {
            rsl += dat[i].at(int(k));
        }
        QVERIFY2( sum == rsl, "bad sum!");
    }
}

// ============================================================================
// same value range of array
// ============================================================================
auto     Tester :: svrAry() -> void
{
    QVector<int> dat[] = {
        { 1, 1, 2, 3, 4 }, // left most, pos 0
        { 1, 1, 2, 3, 4 }, // left most, pos 1
        { 1, 2, 3, 4, 5 }, // only one, pos 2
        { 1, 2, 3, 4, 4 }, // right most, pos 3
        { 1, 2, 3, 4, 4 }, // right most, pos 4
        { 1 },  // just one, pos 0
        { 3, 3, 3, 3, 3, 3 }, // all, pos 1
        { 1, 2, 3, 4, 5, 5, 5, 9, 9 } // middel 4
    };
    int  pos[] = {
        0, 1, 2, 3, 4, 0, 1, 4
    };
    std::pair<size_t,size_t> rsl[] = {
        { 0, 1 }, { 0, 1 }, { 2, 2 },
        { 3, 4 }, { 3, 4 }, { 0, 0 },
        { 0, 5 }, { 4, 6 }
    };

    for ( size_t i = 0; i < sizeof(dat)/sizeof(dat[0]); i ++ ) {
        auto vr = IcAlgor::svrAry( dat[i].constData(), size_t(dat[i].size()), size_t( pos[i]));
        auto is_same = std::memcmp( &rsl[i], &vr, sizeof( std::pair<size_t,size_t>));
        QVERIFY2( is_same == 0, "bad range!" );
    }
}

// ============================================================================
// roll shift left array
// ============================================================================
auto     Tester :: rolAry() -> void
{
    QVector<int> dat[] = {
        { 1, 2, 3, 4, 5 },
        { 1, 2, 3, 4, 5 },
        { 1, 2, 3 },
    };
    size_t rol_n[] = {
        1, 4, 3
    };
    QVector<int> rsl[] = {
        { 2, 3, 4, 5, 1 },
        { 5, 1, 2, 3, 4 },
        { 1, 2, 3 }
    };

    for ( size_t i = 0; i < sizeof(dat)/sizeof(dat[0]); i ++ ) {
        QVector<int> t_rsl( dat[i].size(), 0 );
        std::memcpy( t_rsl.data(), dat[i].constData(), sizeof(int) * size_t( dat[i].size()) );
        IcAlgor::rolAry( t_rsl.data(), size_t( t_rsl.size()), rol_n[i]);
        QVERIFY2( t_rsl == rsl[i], "bad roll shift!" );
    }
}

// ============================================================================
// clamp hole array
// ============================================================================
auto     Tester :: clampAry() -> void
{
    QVector<float> dat[] = {
        { -33.3f, 0, 72.0f, 15.5f, 20.0f },
    };
    QPair<float,float> rg[] = {
        { 0, 20.0f }
    };
    QVector<float> rsl[] = {
        { 0.0f, 0, 20.0f, 15.5f, 20.0f }
    };

    for ( size_t i = 0; i < sizeof(dat)/sizeof(dat[0]); i ++ ) {
        QVector<float> t_rsl( dat[i].size(), 0 );

        std::memcpy( t_rsl.data(), dat[i].constData(), sizeof(float) * size_t( dat[i].size()) );
        IcAlgor::clampAry( t_rsl.data(), size_t( t_rsl.size()), rg[i].first, rg[i].second );
        auto is_same = ( t_rsl == rsl[i] );
        QVERIFY2(  is_same, "bad clamp!");
    }
}





QTEST_MAIN( Tester )

#include "main.moc"

