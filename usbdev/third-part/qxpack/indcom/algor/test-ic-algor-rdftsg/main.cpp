#include <QCoreApplication>

#include "../qxpack_ic_algor_rdftsg.hpp"
#include "../qxpack_ic_algor_errdev.hpp"

#include <QString>
#include <QDebug>
#include <QtTest>
#include <QThread>
#include <QList>
#include <cstdlib>
#include <cstring>
#include <QTime>
#include <QVector>
#include <QtGlobal>
#include <QElapsedTimer>

// this rdft is in 'fftsg.c'
extern "C" void rdft(int, int, double *, int *, double* );

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
    Q_SLOT  void  rdftsg_large();
    Q_SLOT  void  rdftsg_fw();
    Q_SLOT  void  rdftsg_bw();
    Q_SLOT  void  rdftsg_fb();
    Q_SLOT  void  RdftSgSolver_toFFT_HalfRawOrder();
    Q_SLOT  void  RdftSgSolver_toFFT_HalfCorrectedOrder();
    Q_SLOT  void  RdftSgSolver_toData_HalfRawOrder();
    Q_SLOT  void  RdftSgSolver_toData_HalfCorrectedOrder();
public :

};

// ============================================================================
// output the memory counter
// ============================================================================
auto     Tester :: cleanupTestCase( ) -> void
{
}

// ============================================================================
// large FFT
// ============================================================================
auto     Tester :: rdftsg_large() -> void
{
    qsrand( uint( QTime::currentTime().elapsed()) );

    auto e_num = 4096*1024;
    auto  ipw = IcAlgor::rdftsgAllocIPW( e_num );

    QVector<double> rdftsg_vec( e_num );
    for ( int i = 0; i < e_num; i ++ ) {
        auto rd = double( qrand());
        rdftsg_vec[i] = rd;
    }

    IcAlgor::rdftsg( ipw, rdftsg_vec.data(), 1 );

    IcAlgor::rdftsgFreeIPW( ipw );
}


// ============================================================================
// test RDFT forward
// ============================================================================
auto     Tester :: rdftsg_fw() -> void
{
    int e_num_table[] = { 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768 };

    qsrand( uint( QTime::currentTime().elapsed()) );

    int  e_num_idx = 0;
    for ( int loop = 0; loop < 1024; loop ++ ) {
        auto e_num =  e_num_table[ e_num_idx ];
        e_num_idx = size_t( e_num_idx + 1 ) % ( sizeof( e_num_table ) / sizeof( e_num_table[0]));

        QVector<double> fftsg_vec ( e_num );
        QVector<double> rdftsg_vec( e_num );

        auto  fftsg_ip = reinterpret_cast<int*>   ( std::malloc( size_t( e_num + 2 ) * sizeof(int) ));
        auto  fftsg_w  = reinterpret_cast<double*>( std::malloc( size_t( e_num * 5 / 4 ) * sizeof(double) ));
        fftsg_ip[0] = 0; // mark it need initialize
        auto  ipw = IcAlgor::rdftsgAllocIPW( e_num );

        for ( int i = 0; i < e_num; i ++ ) {
            auto rd = double( qrand());
            fftsg_vec[i]  = rd;  rdftsg_vec[i] = rd;
        }

        rdft( int(e_num), 1,  fftsg_vec.data(), fftsg_ip, fftsg_w );
        IcAlgor::rdftsg( ipw, rdftsg_vec.data(), 1 );
        auto ed = QxPack::IcAlgor::isErrDevLessEqu( fftsg_vec.constData(), rdftsg_vec.constData(), 0.0000001, size_t(e_num));
        if ( ! ed ) {
            qInfo( "test %d number data failed.", e_num );
        }
        QVERIFY2( ed, "rdftsg not same as FFTSG.rdft!" );

        IcAlgor::rdftsgFreeIPW( ipw );
        std::free( fftsg_ip );
        std::free( fftsg_w  );
    }
}

// ============================================================================
// test RDFT backward
// ============================================================================
auto     Tester :: rdftsg_bw() -> void
{
    int e_num_table[] = { 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768 };

    qsrand( uint( QTime::currentTime().elapsed()) );

    int  e_num_idx = 0;
    for ( int loop = 0; loop < 1024; loop ++ ) {
        auto e_num =  e_num_table[ e_num_idx ];
        e_num_idx = size_t( e_num_idx + 1 ) % ( sizeof( e_num_table ) / sizeof( e_num_table[0]));

        QVector<double> fftsg_vec ( e_num );
        QVector<double> rdftsg_vec( e_num );

        auto  fftsg_ip = reinterpret_cast<int*>   ( std::malloc( size_t( e_num + 2 ) * sizeof(int) ));
        auto  fftsg_w  = reinterpret_cast<double*>( std::malloc( size_t( e_num * 5 / 4 ) * sizeof(double) ));
        fftsg_ip[0] = 0; // mark it need initialize
        auto  ipw = IcAlgor::rdftsgAllocIPW( e_num );

        for ( int i = 0; i < e_num; i ++ ) {
            auto rd = double( qrand());
            fftsg_vec[i]  = rd;  rdftsg_vec[i] = rd;
        }

        rdft( int(e_num), -1,  fftsg_vec.data(), fftsg_ip, fftsg_w );
        IcAlgor::rdftsg( ipw, rdftsg_vec.data(), -1, false );
        auto ed = QxPack::IcAlgor::isErrDevLessEqu( fftsg_vec.constData(), rdftsg_vec.constData(), 0.0000001, size_t(e_num));
        if ( ! ed ) {
            qInfo( "test %d number data failed.", e_num );
        }
        QVERIFY2( ed, "rdftsg not same as FFTSG.rdft!" );

        IcAlgor::rdftsgFreeIPW( ipw );
        std::free( fftsg_ip );
        std::free( fftsg_w  );
    }
}

// ============================================================================
// test RDFT fwd_bwd
// ============================================================================
auto     Tester :: rdftsg_fb() -> void
{
    int e_num_table[] = { 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768 };

    qsrand( uint( QTime::currentTime().elapsed()) );

    int  e_num_idx = 0;
    for ( int loop = 0; loop < 1024; loop ++ ) {
        auto e_num =  e_num_table[ e_num_idx ];
        e_num_idx = size_t( e_num_idx + 1 ) % ( sizeof( e_num_table ) / sizeof( e_num_table[0]));

        QVector<double> org_vec ( e_num );
        QVector<double> wk_vec  ( e_num );

        auto  ipw = IcAlgor::rdftsgAllocIPW( e_num );
        for ( int i = 0; i < e_num; i ++ ) {
            auto rd = double( qrand());
            org_vec[i]  = rd;  wk_vec[i] = rd;
        }

        IcAlgor::rdftsg( ipw, wk_vec.data(), 1 );
        IcAlgor::rdftsg( ipw, wk_vec.data(), -1, true );
        auto ed = QxPack::IcAlgor::isErrDevLessEqu( org_vec.constData(), wk_vec.constData(), 0.0000001, size_t(e_num));
        if ( ! ed ) {
            qInfo( "test %d number data failed.", e_num );
        }
        QVERIFY2( ed, "rdftsg convert fft to data failed!" );

        IcAlgor::rdftsgFreeIPW( ipw );
    }
}

// ============================================================================
// test RdftSgSolver method toFFT with parameter half raw order
// ============================================================================
auto     Tester :: RdftSgSolver_toFFT_HalfRawOrder() -> void
{
    int e_num_table[] = { 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768 };

    qsrand( uint( QTime::currentTime().elapsed()) );

    int  e_num_idx = 0;
    for ( int loop = 0; loop < 1024; loop ++ ) {
        auto e_num =  e_num_table[ e_num_idx ];
        e_num_idx = size_t( e_num_idx + 1 ) % ( sizeof( e_num_table ) / sizeof( e_num_table[0]));

        QVector<double>     fftsg_vec ( e_num );
        std::vector<double> rdftsg_vec; rdftsg_vec.resize( size_t( e_num ));

        auto  fftsg_ip = reinterpret_cast<int*>   ( std::malloc( size_t( e_num + 2 ) * sizeof(int) ));
        auto  fftsg_w  = reinterpret_cast<double*>( std::malloc( size_t( e_num * 5 / 4 ) * sizeof(double) ));
        fftsg_ip[0] = 0; // mark it need initialize

        for ( int i = 0; i < e_num; i ++ ) {
            auto rd = double( qrand());
            fftsg_vec[i]  = rd;  rdftsg_vec[size_t(i)] = rd;
        }

        rdft( int(e_num), 1,  fftsg_vec.data(), fftsg_ip, fftsg_w );
        IcAlgor::RdftSgSolver<double> solver;
        auto solver_rsl = solver.toFFT( rdftsg_vec );

        auto ed = QxPack::IcAlgor::isErrDevLessEqu( fftsg_vec.constData(), solver_rsl.data(), 0.0000001, size_t(e_num));
        if ( ! ed ) {
            qInfo( "test %d number data failed.", e_num );
        }
        QVERIFY2( ed, "RdftSgSolver result not same as FFTSG.rdft!" );

        std::free( fftsg_ip );
        std::free( fftsg_w  );
    }
}

// ============================================================================
// test RdftSgSolver method toFFT with parameter half corrected order
// ============================================================================
auto     Tester :: RdftSgSolver_toFFT_HalfCorrectedOrder() -> void
{
    int e_num_table[] = { 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768 };

    qsrand( uint( QTime::currentTime().elapsed()) );

    int  e_num_idx = 0;
    for ( int loop = 0; loop < 1024; loop ++ ) {
        auto e_num =  e_num_table[ e_num_idx ];
        e_num_idx = size_t( e_num_idx + 1 ) % ( sizeof( e_num_table ) / sizeof( e_num_table[0]));

        QVector<double>     fftsg_vec ( e_num );
        std::vector<double> rdftsg_vec; rdftsg_vec.resize( size_t( e_num ));

        auto  fftsg_ip = reinterpret_cast<int*>   ( std::malloc( size_t( e_num + 2 ) * sizeof(int) ));
        auto  fftsg_w  = reinterpret_cast<double*>( std::malloc( size_t( e_num * 5 / 4 ) * sizeof(double) ));
        fftsg_ip[0] = 0; // mark it need initialize

        for ( int i = 0; i < e_num; i ++ ) {
            auto rd = double( qrand());
            fftsg_vec[i]  = rd;  rdftsg_vec[size_t(i)] = rd;
        }

        rdft( int(e_num), 1,  fftsg_vec.data(), fftsg_ip, fftsg_w );
        IcAlgor::RdftSgSolver<double> solver;
        auto solver_rsl = solver.toFFT( rdftsg_vec, IcAlgor::RdftSgSolver<double>::FFtType_HalfCorrectedOrder );
        QVERIFY2( solver_rsl.size() >= size_t(e_num + 2), "bad size!");

        auto ed_mid = QxPack::IcAlgor::isErrDevLessEqu( fftsg_vec.constData() + 2, solver_rsl.data() + 2, 0.0000001, size_t(e_num - 2));
        auto ed_fi = qFuzzyCompare( fftsg_vec.constData()[0], solver_rsl.data()[0] );
        auto ed_la = qFuzzyCompare( fftsg_vec.constData()[1], solver_rsl.data()[ e_num ] );

        if ( !( ed_mid  && ed_fi && ed_la )) {
            qInfo( "test %d number data failed.", e_num );
        }
        QVERIFY2( ed_mid  && ed_fi  && ed_la, "RdftSgSolver result not same as FFTSG.rdft!" );

        std::free( fftsg_ip );
        std::free( fftsg_w  );
    }
}

// ============================================================================
// test RdftSgSolver method toData with parameter half  raw order
// ============================================================================
auto     Tester :: RdftSgSolver_toData_HalfRawOrder() -> void
{
    int e_num_table[] = { 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768 };

    qsrand( uint( QTime::currentTime().elapsed()) );

    int  e_num_idx = 0;
    for ( int loop = 0; loop < 1024; loop ++ ) {
        auto e_num =  e_num_table[ e_num_idx ];
        e_num_idx = size_t( e_num_idx + 1 ) % ( sizeof( e_num_table ) / sizeof( e_num_table[0]));

        std::vector<double> org_vec;
        org_vec.resize( size_t(e_num) );

        for ( int i = 0; i < e_num; i ++ ) {
            auto rd = double( qrand());
            org_vec[size_t(i)]  = rd;
        }

        IcAlgor::RdftSgSolver<double> solver;
        auto fft_rsl = solver.toFFT ( org_vec, IcAlgor::RdftSgSolver<double>::FFtType_HalfRawOrder );
        auto dat_rsl = solver.toData( fft_rsl, IcAlgor::RdftSgSolver<double>::FFtType_HalfRawOrder );
        QVERIFY2( dat_rsl.size() == org_vec.size(), "the data converted back size is not corrected!");
        auto ed = QxPack::IcAlgor::isErrDevLessEqu( org_vec.data(), dat_rsl.data(), 0.0000001, org_vec.size());
        if ( ! ed ) {
            qInfo( "test %d number data failed.", e_num );
        }
        QVERIFY2( ed, "RdftSgSolver convert back data failed!" );
    }
}

// ============================================================================
//  test RdftSgSolver method toData with parameter half  raw order
// ============================================================================
auto     Tester :: RdftSgSolver_toData_HalfCorrectedOrder() -> void
{
    int e_num_table[] = { 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768 };

    qsrand( uint( QTime::currentTime().elapsed()) );

    int  e_num_idx = 0;
    for ( int loop = 0; loop < 1024; loop ++ ) {
        auto e_num =  e_num_table[ e_num_idx ];
        e_num_idx = size_t( e_num_idx + 1 ) % ( sizeof( e_num_table ) / sizeof( e_num_table[0]));

        std::vector<double> org_vec;
        org_vec.resize( size_t(e_num) );

        for ( int i = 0; i < e_num; i ++ ) {
            auto rd = double( qrand());
            org_vec[size_t(i)]  = rd;
        }

        IcAlgor::RdftSgSolver<double> solver;
        auto fft_rsl = solver.toFFT ( org_vec, IcAlgor::RdftSgSolver<double>::FFtType_HalfCorrectedOrder );
        auto dat_rsl = solver.toData( fft_rsl, IcAlgor::RdftSgSolver<double>::FFtType_HalfCorrectedOrder );
        QVERIFY2( dat_rsl.size() == org_vec.size(), "the data converted back size is not corrected!");
        auto ed = QxPack::IcAlgor::isErrDevLessEqu( org_vec.data(), dat_rsl.data(), 0.0000001, org_vec.size());
        if ( ! ed ) {
            qInfo( "test %d number data failed.", e_num );
        }
        QVERIFY2( ed, "RdftSgSolver convert back data failed!" );
    }
}


QTEST_MAIN( Tester )

#include "main.moc"

