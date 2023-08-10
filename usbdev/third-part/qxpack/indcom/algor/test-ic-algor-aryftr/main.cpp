#include <QCoreApplication>

#include "../qxpack_ic_algor_aryftr.hpp"
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

// ============================================================================
// make random value array
// ============================================================================
template <typename T>
static auto  gMakeRandAry( const T &rg, const T &end ) -> QVector<T>
{
    QVector<T> rsl( 2 + qAbs(qrand() % 1024 ), 0 );
    auto lmt = rsl.size();
    for ( int i = 0; i < lmt; i ++ ) {
        rsl[i] = ( T( qAbs( qrand()) % int( end - rg )) + rg );
    }
    return rsl;
}


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
    Q_SLOT  void  AryFtrKrnlWorkBuff_ctor();
    Q_SLOT  void  AryFtrKrnlWorkBuff_init();
    Q_SLOT  void  boxFtrAry( );
    Q_SLOT  void  uptrndFixFtrAry( );
public :

};

// ============================================================================
// output the memory counter
// ============================================================================
auto     Tester :: cleanupTestCase( ) -> void
{

}

// ============================================================================
// test ary ftr krnl work buffer ctor
// ============================================================================
auto     Tester :: AryFtrKrnlWorkBuff_ctor() -> void
{
    struct Param { size_t krn_sz; size_t ary_sz; };
    Param  data[] = {
        { 0, 0 }, { 0, 1 }, { 0, 2 }, { 0, 3 },
        { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 }
    };
    size_t krn_sz[] = {
        1, 1, 1, 1,
        1, 3, 3, 5,  // kernel size should be odd, and just meet great than required krnl size.
    };
    for ( size_t i = 0; i < sizeof(data)/sizeof(data[0]); i ++ ) {
        IcAlgor::AryFtrKrnlWorkBuff<int> kw( uint16_t( data[i].krn_sz ), data[i].ary_sz );
        QVERIFY2( krn_sz[i] == kw.workBuffSize(),"ctor failed!");
    }
}


// ============================================================================
// test ary ftr krnl work buffer init
// ============================================================================
auto     Tester :: AryFtrKrnlWorkBuff_init() -> void
{
    QVector<int> data[] = {
       { 1, 2, 3 },
       { 1, 3, 5, 7 },
       { 2, 4, 6, 8 },
       { 1, 3, 5 }
    };
    uint16_t ksz[] = {
        1, 5, 9, 3
    };
    QVector<int> rsl_wk[] = {
       { 1,  1 },
       { 5,3,1,3,5,   5,3 },
       { 8,6,4,2,4,6,8,  6,4,2 },
       { 3,1,3,  3 }
    };

    for ( size_t i = 0; i < sizeof(data)/sizeof(data[0]); i ++ ) {
        IcAlgor::AryFtrKrnlWorkBuff<int> kw( ksz[i], size_t( data[i].size()));
        kw.init( data[i].constData(), size_t( data[i].size()) );
//        for ( size_t j = 0; j < kw.workBuffSize() + kw.mirrorTailBuffSize(); j ++ ) {
//            qInfo() << kw.workBuff()[j];
//        }
        auto is_wk_same  = std::memcmp( kw.workBuff(), rsl_wk[i].constData(), sizeof( int ) * kw.workBuffSize() );
        auto is_ext_same = std::memcmp( kw.mirrorTailBuff(), rsl_wk[i].constData() + kw.workBuffSize(), kw.mirrorTailBuffSize() * sizeof(int));
        QVERIFY2( is_wk_same == 0 && is_ext_same == 0, "init failed!" );
    }
}

//// ============================================================================
//// filter the array by box filter
//// ============================================================================
auto     Tester :: boxFtrAry( ) -> void
{
    QVector<int> data[] = {
       { 1, 3, 4, 5 }, {1,2}, { 1,2,3 },
       { 1, 3, 4, 5 }, /*
       { 1, 3, 5 }, { 1,3,5 }*/
    };
    QVector<int> data_ext[] = {
       { 4, 3, 1, 3, 4, 5, 4, 3 }, { 2, 1, 2, 1 }, { 2, 1, 2, 3, 2 },
       { 3, 1, 3, 4, 5, 4 },
    };
    uint16_t ksz[] = {
       5, 3, 3, 3
    };


    for ( size_t i = 0; i < sizeof(data)/sizeof(data[0]); i ++ ) {
        IcAlgor::AryFtrKrnlWorkBuff<int> kw( ksz[i], size_t( data[i].size()));

        QVector<int> rsl_t( data[i].size(), 0 );
        for ( int n = 0; n < rsl_t.size(); n ++ ) {
            auto rsl = IcAlgor::sumAry<int,double>( data_ext[i].constData()+n, kw.workBuffSize() );
            rsl_t[ n ] = int(rsl / kw.workBuffSize());
        }

       // qInfo() << rsl_t;
        IcAlgor::boxFtrAry( data[i].data(), size_t( data[i].size()), kw );
       QVERIFY2( kw.isSentOK(), "kw is broken!");
       // qInfo() << kw.isSentOK();
       // qInfo() << data[i];
       auto is_ary_same = std::memcmp( rsl_t.constData(), data[i].constData(), size_t( data[i].size()) * sizeof(int));
       QVERIFY2( is_ary_same == 0, "bad boxftr!" );
    }
}

// ============================================================================
// test uptrends fixing filter
// ============================================================================
auto     Tester :: uptrndFixFtrAry() -> void
{
    QVector<int> data[] = {
       { 1, 4, 2, 3 },
       { 1, 2, 3, 2, 3, 1, 5 }
    };
    QVector<int> rsl[] = {
       { 1, 4, 4, 4 },
       { 1, 2, 3, 3, 3, 3, 5 }
    };

    for ( size_t i = 0; i < sizeof(data)/sizeof(data[0]); i ++ ) {
        QVector<int> t_rsl( data[i].size(), 0 );
        std::memcpy( t_rsl.data(), data[i].constData(), sizeof(int) * size_t( data[i].size() ));
        IcAlgor::uptrndFixFtrAry( t_rsl.data(), size_t( t_rsl.size()) );
        QVERIFY2( t_rsl == rsl[i], "bad uptrends ftr!"  );
    }

}


QTEST_MAIN( Tester )

#include "main.moc"

