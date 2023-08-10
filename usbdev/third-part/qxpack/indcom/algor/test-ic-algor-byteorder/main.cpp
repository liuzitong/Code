#include <QCoreApplication>

#include "../qxpack_ic_algor_ary.hpp"
#include "../qxpack_ic_algor_byteorder.hpp"

#include <QString>
#include <QDebug>
#include <QtTest>
#include <QThread>
#include <QList>
#include <cstdlib>
#include <cstring>
#include <QTime>
#include <QVector>

// ////////////////////////////////////////////////////////////////////////////
//
//   test the algorithm base template
//
// ////////////////////////////////////////////////////////////////////////////
class  Tester : public QObject {
    Q_OBJECT
private:
    Q_SLOT  void  initTestCase( ) { }
    Q_SLOT  void  cleanupTestCase ( );
    Q_SLOT  void  hostByteOrder();

    Q_SLOT  void  readData16_LE();
    Q_SLOT  void  readData32_LE();
    Q_SLOT  void  readData64_LE();
    Q_SLOT  void  readData16_BE();
    Q_SLOT  void  readData32_BE();
    Q_SLOT  void  readData64_BE();
    Q_SLOT  void  readData16_IsBE();
    Q_SLOT  void  readData32_IsBE();
    Q_SLOT  void  readData64_IsBE();

    Q_SLOT  void  writeData16_LE();
    Q_SLOT  void  writeData32_LE();
    Q_SLOT  void  writeData64_LE();
    Q_SLOT  void  writeData16_BE();
    Q_SLOT  void  writeData32_BE();
    Q_SLOT  void  writeData64_BE();
    Q_SLOT  void  writeData16_IsBE();
    Q_SLOT  void  writeData32_IsBE();
    Q_SLOT  void  writeData64_IsBE();
public :

};

// ============================================================================
// output the memory counter
// ============================================================================
auto     Tester :: cleanupTestCase( ) -> void
{
}

// ============================================================================
// test the host byte order
// ============================================================================
auto     Tester :: hostByteOrder() -> void
{

}

// ============================================================================
// read data  16bit
// ============================================================================
auto     Tester :: readData16_LE() -> void
{
    uint8_t buff1[] = { 0x12, 0x13, 0x14 };
    auto v1 = QxPack::IcAlgor::readData16_LE<uint16_t>( buff1 );
    QVERIFY2( v1 == 0x1312UL, "read data (LE) uint16_t failed!");

    int8_t  buff2[] = { 0x15, int8_t(0xf2), 0x22};
    auto v2 = QxPack::IcAlgor::readData16_LE<int16_t>( buff2 );
    QVERIFY2( v2 == int16_t(0xf215), "read data (LE) int16_t failed!" );
}

auto     Tester :: readData16_BE() -> void
{
    uint8_t buff1[] = { 0x12, 0x13, 0x14 };
    auto v1 = QxPack::IcAlgor::readData16_BE<uint16_t>( buff1 );
    QVERIFY2( v1 == 0x1213UL, "read data (BE) uint16_t failed!");

    int8_t  buff2[] = { 0x15, int8_t(0xf2), 0x22};
    auto v2 = QxPack::IcAlgor::readData16_BE<int16_t>( buff2 );
    QVERIFY2( v2 == int16_t(0x15f2), "read data (BE) int16_t failed!" );
}

auto     Tester :: readData16_IsBE() -> void
{
    uint8_t buff1[] = { 0x12, 0x13, 0x14 };
    auto v1 = QxPack::IcAlgor::readData16_IsBE<uint16_t>( buff1, false );
    QVERIFY2(  v1 == 0x1312UL, "read data (IsBE = false) uint16_t failed!");
    auto v11 = QxPack::IcAlgor::readData16_IsBE<uint16_t>( buff1, true );
    QVERIFY2( v11 == 0x1213UL, "read data (IsBE = true) uint16_t failed!");

    int8_t  buff2[] = { 0x15, int8_t(0xf2), 0x22};
    auto v2  = QxPack::IcAlgor::readData16_IsBE<int16_t>( buff2, false );
    QVERIFY2( v2 == int16_t(0xf215), "read data (IsBE = false) int16_t failed!" );
    auto v21 = QxPack::IcAlgor::readData16_IsBE<int16_t>( buff2, true );
    QVERIFY2( v21 == int16_t(0x15f2), "read data (IsBE = true) int16_t failed!" );
}


// ============================================================================
// read data  32bit
// ============================================================================
auto     Tester :: readData32_LE() -> void
{
    uint8_t buff1[] = { 0x12, 0x13, 0x14, 0x15 };
    auto v1 = QxPack::IcAlgor::readData32_LE<uint32_t>( buff1 );
    QVERIFY2( v1 == 0x15141312UL, "read data (LE) uint32_t failed!");

    uint8_t buff2[] = { 0x12, 0x13, 0x22, 0xf4 };
    auto v2 = QxPack::IcAlgor::readData32_LE<int32_t>( buff2 );
    QVERIFY2( v2 == int32_t( 0xf4221312 ), "read data (LE) int32_t failed!");

    float  v3_src = 1.3375f;
    if ( QxPack::IcAlgor::hostByteOrder() == QxPack::IcAlgor::ByteOrder_LittleEndian ) {
        auto v3 = QxPack::IcAlgor::readData32_LE<float>( &v3_src );
        QVERIFY2( qFuzzyCompare(v3_src, v3), "read data (LE) float failed!" );
    } else {
        auto v3_tmp = QxPack::IcAlgor::readData32_LE<float>( &v3_src );
        auto v3     = v3_tmp;
        QxPack::IcAlgor::revAryOrder( reinterpret_cast<const uint8_t*>(&v3_tmp), reinterpret_cast<uint8_t*>(&v3), sizeof(float) );
        QVERIFY2( qFuzzyCompare(v3_src, v3), "read data (LE) float faild!");
    }
}

auto     Tester :: readData32_BE() -> void
{
    uint8_t buff1[] = { 0x12, 0x13, 0x14, 0x15 };
    auto v1 = QxPack::IcAlgor::readData32_BE<uint32_t>( buff1 );
    QVERIFY2( v1 == 0x12131415UL, "read data (BE) uint32_t failed!");

    uint8_t buff2[] = { 0x12, 0x13, 0x22, 0xf4 };
    auto v2 = QxPack::IcAlgor::readData32_BE<int32_t>( buff2 );
    QVERIFY2( v2 == int32_t( 0x121322f4 ), "read data (BE) int32_t failed!");

    float  v3_src = 1.3375f;
    if ( QxPack::IcAlgor::hostByteOrder() == QxPack::IcAlgor::ByteOrder_BigEndian ) {
        auto v3 = QxPack::IcAlgor::readData32_BE<float>( &v3_src );
        QVERIFY2( qFuzzyCompare(v3_src, v3), "read data (LE) float failed!" );
    } else {
        auto v3_tmp = QxPack::IcAlgor::readData32_BE<float>( &v3_src );
        auto v3     = v3_tmp;
        QxPack::IcAlgor::revAryOrder( reinterpret_cast<const uint8_t*>(&v3_tmp), reinterpret_cast<uint8_t*>(&v3), sizeof(float) );
        QVERIFY2( qFuzzyCompare(v3_src, v3), "read data (LE) float faild!");
    }
}

auto     Tester :: readData32_IsBE() -> void
{
    uint8_t buff1[] = { 0x12, 0x13, 0x14, 0x15 };
    auto v1 = QxPack::IcAlgor::readData32_IsBE<uint32_t>( buff1, false );
    QVERIFY2( v1 == 0x15141312UL, "read data (IsBE = false) uint32_t failed!");
    auto v11 = QxPack::IcAlgor::readData32_IsBE<uint32_t>( buff1, true );
    QVERIFY2( v11 == 0x12131415, "read data (IsBE = true) uint32_t failed!" );

    uint8_t buff2[] = { 0x12, 0x13, 0x22, 0xf4 };
    auto v2 = QxPack::IcAlgor::readData32_IsBE<int32_t>( buff2, false );
    QVERIFY2( v2 == int32_t( 0xf4221312 ), "read data (IsBE = false) int32_t failed!");
    auto v21 = QxPack::IcAlgor::readData32_IsBE<int32_t>( buff2, true );
    QVERIFY2( v21 == 0x121322f4, "read data (IsBE = true) int32_t failed!" );

    float  v3_src = 1.3375f;
    if ( QxPack::IcAlgor::hostByteOrder() == QxPack::IcAlgor::ByteOrder_LittleEndian ) {
        auto v3 = QxPack::IcAlgor::readData32_IsBE<float>( &v3_src, false );
        QVERIFY2( qFuzzyCompare(v3_src, v3), "read data (IsBE = false) float failed!" );
        auto v31_t = QxPack::IcAlgor::readData32_IsBE<float>(&v3_src, true );
        auto v31 = v31_t;
        QxPack::IcAlgor::revAryOrder( reinterpret_cast<const uint8_t*>(&v31_t), reinterpret_cast<uint8_t*>(&v31), sizeof(float) );
        QVERIFY2( qFuzzyCompare(v3_src, v31), "read data (IsBE = true) float failed!" );
     } else {
        auto v3 = QxPack::IcAlgor::readData32_IsBE<float>( &v3_src, true );
        QVERIFY2( qFuzzyCompare(v3_src, v3), "read data (IsBE = true) float failed!" );
        auto v31_t = QxPack::IcAlgor::readData32_IsBE<float>( &v3_src, false );
        auto v31   = v31_t;
        QxPack::IcAlgor::revAryOrder( reinterpret_cast<const uint8_t*>(&v31_t), reinterpret_cast<uint8_t*>(&v31), sizeof(float) );
        QVERIFY2( qFuzzyCompare(v3_src, v3), "read data (IsBE = false) float failed!");
    }
}


// ============================================================================
// read data 64bit
// ============================================================================
auto     Tester :: readData64_LE() -> void
{
    uint8_t buff1[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
    auto v1 = QxPack::IcAlgor::readData64_LE<uint64_t>( buff1 );
    QVERIFY2( v1 == 0x0807060504030201ULL, "read data (LE) uint64_t failed!" );

    uint8_t buff2[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0xf8 };
    auto v2 = QxPack::IcAlgor::readData64_LE<int64_t>( buff2 );
    QVERIFY2( v2 == int64_t( 0xf807060504030201ULL ), "read data (LE) int64_t failed!");

    double  v3_src = 0.77551234999;
    if ( QxPack::IcAlgor::hostByteOrder() == QxPack::IcAlgor::ByteOrder_LittleEndian ) {
        auto v3 = QxPack::IcAlgor::readData64_LE<double>( &v3_src );
        QVERIFY2( qFuzzyCompare(v3_src, v3), "read data (LE) double failed!" );
    } else {
        auto v3_tmp = QxPack::IcAlgor::readData64_LE<double>( &v3_src );
        auto v3     = v3_tmp;
        QxPack::IcAlgor::revAryOrder( reinterpret_cast<const uint8_t*>(&v3_tmp), reinterpret_cast<uint8_t*>(&v3), sizeof(double) );
        QVERIFY2( qFuzzyCompare(v3_src, v3), "read data (LE) float faild!");
    }
}

auto     Tester :: readData64_BE() -> void
{
    uint8_t buff1[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
    auto v1 = QxPack::IcAlgor::readData64_BE<uint64_t>( buff1 );
    QVERIFY2( v1 == 0x0102030405060708ULL, "read data (BE) uint64_t failed!" );

    uint8_t buff2[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0xf8 };
    auto v2 = QxPack::IcAlgor::readData64_BE<int64_t>( buff2 );
    QVERIFY2( v2 == int64_t( 0x01020304050607f8ULL ), "read data (BE) int64_t failed!");

    double  v3_src = 1.77551234999;
    if ( QxPack::IcAlgor::hostByteOrder() == QxPack::IcAlgor::ByteOrder_BigEndian ) {
        auto v3 = QxPack::IcAlgor::readData64_BE<double>( &v3_src );
        QVERIFY2( qFuzzyCompare(v3_src, v3), "read data (LE) double failed!" );
    } else {
        auto v3_tmp = QxPack::IcAlgor::readData64_BE<double>( &v3_src );
        auto v3     = v3_tmp;
        QxPack::IcAlgor::revAryOrder( reinterpret_cast<const uint8_t*>(&v3_tmp), reinterpret_cast<uint8_t*>(&v3), sizeof(double) );
        QVERIFY2( qFuzzyCompare(v3_src, v3), "read data (LE) double faild!");
    }
}

auto     Tester :: readData64_IsBE() -> void
{
    uint8_t buff1[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
    auto v1 = QxPack::IcAlgor::readData64_IsBE<uint64_t>( buff1, false );
    QVERIFY2( v1 == 0x0807060504030201ULL, "read data (IsBE = false) uint64_t failed!");
    auto v11 = QxPack::IcAlgor::readData64_IsBE<uint64_t>( buff1, true );
    QVERIFY2( v11 == 0x0102030405060708ULL, "read data (IsBE = true) uint64_t failed!" );

    uint8_t buff2[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0xf8  };
    auto v2 = QxPack::IcAlgor::readData64_IsBE<int64_t>( buff2, false );
    QVERIFY2( v2 == int64_t( 0xf807060504030201LL ), "read data (IsBE = false) int64_t failed!");
    auto v21 = QxPack::IcAlgor::readData64_IsBE<int64_t>( buff2, true );
    QVERIFY2( v21 == 0x01020304050607f8LL, "read data (IsBE = true) int64_t failed!" );

    double  v3_src = 1.77551234999;
    if ( QxPack::IcAlgor::hostByteOrder() == QxPack::IcAlgor::ByteOrder_LittleEndian ) {
        auto v3 = QxPack::IcAlgor::readData64_IsBE<double>( &v3_src, false );
        QVERIFY2( qFuzzyCompare(v3_src, v3), "read data (IsBE = false) double failed!" );
        auto v31_t = QxPack::IcAlgor::readData64_IsBE<double>(&v3_src, true );
        auto v31 = v31_t;
        QxPack::IcAlgor::revAryOrder( reinterpret_cast<const uint8_t*>(&v31_t), reinterpret_cast<uint8_t*>(&v31), sizeof(double) );
        QVERIFY2( qFuzzyCompare(v3_src, v31), "read data (IsBE = true) double failed!" );
     } else {
        auto v3 = QxPack::IcAlgor::readData64_IsBE<double>( &v3_src, true );
        QVERIFY2( qFuzzyCompare(v3_src, v3), "read data (IsBE = true) double failed!" );
        auto v31_t = QxPack::IcAlgor::readData64_IsBE<double>( &v3_src, false );
        auto v31   = v31_t;
        QxPack::IcAlgor::revAryOrder( reinterpret_cast<const uint8_t*>(&v31_t), reinterpret_cast<uint8_t*>(&v31), sizeof(double) );
        QVERIFY2( qFuzzyCompare(v3_src, v3), "read data (IsBE = false) double failed!");
    }
}

// ============================================================================
// write data 16bit
// ============================================================================
auto     Tester :: writeData16_LE() -> void
{
    uint8_t rsl1[] = { 0xb8, 0x0b };
    uint8_t buff1[ sizeof(rsl1) ];
    QxPack::IcAlgor::writeData16_LE( buff1, uint16_t(3000) );
    QVERIFY2( std::memcmp( buff1, rsl1, sizeof(rsl1) ) == 0, "write (LE) 16bit failed!" );

    uint8_t rsl2[] = { 0xfe, 0xff };
    uint8_t buff2[ sizeof(rsl2)];
    QxPack::IcAlgor::writeData16_LE( buff2, int16_t(-2) );
    QVERIFY2( std::memcmp( buff2, rsl2, sizeof(rsl2) ) == 0, "write (LE) 16bit failed!" );
}

auto     Tester :: writeData16_BE() -> void
{
    uint8_t rsl1[] = { 0x08, 0x0b };
    uint8_t buff1[ sizeof(rsl1) ];
    QxPack::IcAlgor::writeData16_BE( buff1, uint16_t(0x080b) );
    QVERIFY2( std::memcmp( buff1, rsl1, sizeof(rsl1) ) == 0, "write (BE) 16bit failed!" );

    uint8_t rsl2[] = { 0xff, 0xfe };
    uint8_t buff2[ sizeof(rsl2)];
    QxPack::IcAlgor::writeData16_BE( buff2, int16_t(0xfffe) );
    QVERIFY2( std::memcmp( buff2, rsl2, sizeof(rsl2) ) == 0, "write (BE) 16bit failed!" );
}

auto     Tester :: writeData16_IsBE() -> void
{
    uint8_t rsl1[] = { 0x08, 0x0b };
    uint8_t buff1[ sizeof(rsl1) ];
    QxPack::IcAlgor::writeData16_IsBE( buff1, uint16_t(0x0b08), false );
    QVERIFY2( std::memcmp( buff1, rsl1, sizeof(rsl1) ) == 0, "write (IsBE = false) 16bit failed!" );
    QxPack::IcAlgor::writeData16_IsBE( buff1, uint16_t(0x080b), true );
    QVERIFY2( std::memcmp( buff1, rsl1, sizeof(rsl1) ) == 0, "write (IsBE = true) 16bit failed!" );

    uint8_t rsl2[] = { 0xff, 0xfe };
    uint8_t buff2[ sizeof(rsl2)];
    QxPack::IcAlgor::writeData16_IsBE( buff2, int16_t(0xfeff), false );
    QVERIFY2( std::memcmp( buff2, rsl2, sizeof(rsl2) ) == 0, "write (IsBE = false) 16bit failed!" );
    QxPack::IcAlgor::writeData16_IsBE( buff2, int16_t(0xfffe), true );
    QVERIFY2( std::memcmp( buff2, rsl2, sizeof(rsl2) ) == 0, "write (IsBE = true) 16bit failed!" );
}

// ============================================================================
// write data 32bit
// ============================================================================
auto     Tester :: writeData32_LE() -> void
{
    uint8_t rsl1[] = { 0xe0, 0x93, 0x04, 0x00 };
    uint8_t buff1[ sizeof(rsl1) ];
    QxPack::IcAlgor::writeData32_LE( buff1, uint32_t(300000) );
    QVERIFY2( std::memcmp( buff1, rsl1, sizeof(rsl1) ) == 0, "write (LE) 32bit failed!" );

    uint8_t rsl2[] = { 0xfe, 0xff, 0xff, 0xff };
    uint8_t buff2[ sizeof(rsl2)];
    QxPack::IcAlgor::writeData32_LE( buff2, int32_t(-2) );
    QVERIFY2( std::memcmp( buff2, rsl2, sizeof(rsl2) ) == 0, "write (LE) 32bit failed!" );

    float v1 = 0.113322f;
    if ( QxPack::IcAlgor::hostByteOrder() == QxPack::IcAlgor::ByteOrder_LittleEndian ) {
        float v2 = 0;
        QxPack::IcAlgor::writeData32_LE( &v2, v1 );
        QVERIFY2( qFuzzyCompare(v1,v2), "write (LE) float failed!");
    } else {
        float v2 = 0; float v3 = 0;
        QxPack::IcAlgor::writeData32_LE( &v3, v1 );
        QxPack::IcAlgor::revAryOrder( reinterpret_cast<const uint8_t*>( &v3 ), reinterpret_cast<uint8_t*>(&v2), sizeof(float));
        QVERIFY2( qFuzzyCompare(v1,v2), "write (LE) float failed!");
    }
}

auto     Tester :: writeData32_BE() -> void
{
    uint8_t rsl1[] = { 0xe0, 0x93, 0x04, 0x00 };
    uint8_t buff1[ sizeof(rsl1) ];
    QxPack::IcAlgor::writeData32_BE( buff1, uint32_t(0xe0930400) );
    QVERIFY2( std::memcmp( buff1, rsl1, sizeof(rsl1) ) == 0, "write (BE) 32bit failed!" );

    uint8_t rsl2[] = { 0xfe, 0xff, 0xff, 0xff };
    uint8_t buff2[ sizeof(rsl2)];
    QxPack::IcAlgor::writeData32_BE( buff2, int32_t(0xfeffffff) );
    QVERIFY2( std::memcmp( buff2, rsl2, sizeof(rsl2) ) == 0, "write (BE) 32bit failed!" );

    float v1 = 0.113322f;
    if ( QxPack::IcAlgor::hostByteOrder() == QxPack::IcAlgor::ByteOrder_LittleEndian ) {
        float v2 = 0, v3 = 0;
        QxPack::IcAlgor::writeData32_BE( &v2, v1 );
        QxPack::IcAlgor::revAryOrder( reinterpret_cast<const uint8_t*>(&v2), reinterpret_cast<uint8_t*>(&v3), sizeof(float));
        QVERIFY2( qFuzzyCompare(v1,v3), "write (BE) float failed!");
    } else {
        float v2 = 0;
        QxPack::IcAlgor::writeData32_BE( &v2, v1 );
        QVERIFY2( qFuzzyCompare(v1,v2), "write (BE) float failed!");
    }
}

auto     Tester :: writeData32_IsBE() -> void
{
    uint8_t rsl1[] = { 0xe0, 0x93, 0x04, 0x00 };
    uint8_t buff1[ sizeof(rsl1) ];
    QxPack::IcAlgor::writeData32_IsBE( buff1, uint32_t(0x000493e0), false );
    QVERIFY2( std::memcmp( buff1, rsl1, sizeof(rsl1) ) == 0, "write (IsBE = false) 32bit failed!" );
    QxPack::IcAlgor::writeData32_IsBE( buff1, uint32_t(0xe0930400), true );
    QVERIFY2( std::memcmp( buff1, rsl1, sizeof(rsl1) ) == 0, "write (IsBE = true) 32bit failed!" );

    uint8_t rsl2[] = { 0xfe, 0xff, 0xff, 0xff };
    uint8_t buff2[ sizeof(rsl2)];
    QxPack::IcAlgor::writeData32_IsBE( buff2, int32_t(0xfffffffe), false );
    QVERIFY2( std::memcmp( buff2, rsl2, sizeof(rsl2) ) == 0, "write (IsBE = false) 32bit failed!" );
    QxPack::IcAlgor::writeData32_IsBE( buff2, int32_t(0xfeffffff), true );
    QVERIFY2( std::memcmp( buff2, rsl2, sizeof(rsl2) ) == 0, "write (IsBE = true) 32bit failed!" );

    float v1 = 0.113322f;
    if ( QxPack::IcAlgor::hostByteOrder() == QxPack::IcAlgor::ByteOrder_LittleEndian ) {
        float v2 = 0;
        QxPack::IcAlgor::writeData32_IsBE( &v2, v1, false );
        QVERIFY2( qFuzzyCompare(v1,v2), "write (IsBE = false) float failed!" );
        float v3_t = 0, v3 = 0;
        QxPack::IcAlgor::writeData32_IsBE( &v3_t, v1, true );
        QxPack::IcAlgor::revAryOrder( reinterpret_cast<const uint8_t*>(&v3_t), reinterpret_cast<uint8_t*>(&v3), sizeof(float));
        QVERIFY2( qFuzzyCompare(v1,v3), "write (IsBE = true ) float failed!");
    } else {
        float v2 = 0;
        QxPack::IcAlgor::writeData32_IsBE( &v2, v1, true );
        QVERIFY2( qFuzzyCompare(v1,v2), "write (IsBE = true) float failed!" );
        float v3_t = 0, v3 = 0;
        QxPack::IcAlgor::writeData32_IsBE( &v3_t, v1, false );
        QxPack::IcAlgor::revAryOrder( reinterpret_cast<const uint8_t*>(&v3_t), reinterpret_cast<uint8_t*>(&v3), sizeof(float));
        QVERIFY2( qFuzzyCompare(v1,v3), "write (IsBE = false ) float failed!");
    }
}


// ============================================================================
// write data 64bit
// ============================================================================
auto     Tester :: writeData64_LE() -> void
{
    uint8_t rsl1[] = { 0xe0, 0x93, 0x04, 0x00, 0x10, 0x00, 0x00, 0x00 };
    uint8_t buff1[ sizeof(rsl1) ];
    QxPack::IcAlgor::writeData64_LE( buff1, uint64_t(68719776736ULL) );
    QVERIFY2( std::memcmp( buff1, rsl1, sizeof(rsl1) ) == 0, "write (LE) 64bit failed!" );

    uint8_t rsl2[] = { 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    uint8_t buff2[ sizeof(rsl2)];
    QxPack::IcAlgor::writeData64_LE( buff2, int64_t(-2) );
    QVERIFY2( std::memcmp( buff2, rsl2, sizeof(rsl2) ) == 0, "write (LE) 64bit failed!" );

    double v1 = 1.77551234999;
    if ( QxPack::IcAlgor::hostByteOrder() == QxPack::IcAlgor::ByteOrder_LittleEndian ) {
        double v2 = 0;
        QxPack::IcAlgor::writeData64_LE( &v2, v1 );
        QVERIFY2( qFuzzyCompare(v1,v2), "write (LE) double failed!");
    } else {
        double v2 = 0; double v3 = 0;
        QxPack::IcAlgor::writeData64_LE( &v3, v1 );
        QxPack::IcAlgor::revAryOrder( reinterpret_cast<const uint8_t*>( &v3 ), reinterpret_cast<uint8_t*>(&v2), sizeof(double));
        QVERIFY2( qFuzzyCompare(v1,v2), "write (LE) double failed!");
    }
}

auto     Tester :: writeData64_BE() -> void
{
    uint8_t rsl1[] = { 0xe0, 0x93, 0x04, 0x00, 0x10, 0x00, 0x00, 0x00 };
    uint8_t buff1[ sizeof(rsl1) ];
    QxPack::IcAlgor::writeData64_BE( buff1, uint64_t(0xe093040010000000ULL) );
    QVERIFY2( std::memcmp( buff1, rsl1, sizeof(rsl1) ) == 0, "write (BE) 64bit failed!" );

    uint8_t rsl2[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
    uint8_t buff2[ sizeof(rsl2)];
    QxPack::IcAlgor::writeData64_BE( buff2, int64_t(0x0102030405060708LL) );
    QVERIFY2( std::memcmp( buff2, rsl2, sizeof(rsl2) ) == 0, "write (BE) 64bit failed!" );

    double v1 = 1.77551234999;
    if ( QxPack::IcAlgor::hostByteOrder() == QxPack::IcAlgor::ByteOrder_LittleEndian ) {
        double v2 = 0, v3 = 0;
        QxPack::IcAlgor::writeData64_BE( &v2, v1 );
        QxPack::IcAlgor::revAryOrder( reinterpret_cast<const uint8_t*>( &v2 ), reinterpret_cast<uint8_t*>(&v3), sizeof(double));
        QVERIFY2( qFuzzyCompare(v1,v3), "write (BE) double failed!");
    } else {
        double v2 = 0;
        QxPack::IcAlgor::writeData64_BE( &v2, v1 );
        QVERIFY2( qFuzzyCompare(v1,v2), "write (BE) double failed!");
    }
}

auto     Tester :: writeData64_IsBE() -> void
{
    uint8_t rsl1[] = { 0xe0, 0x93, 0x04, 0x00, 0x10, 0x00, 0x00, 0x00 };
    uint8_t buff1[ sizeof(rsl1) ];
    QxPack::IcAlgor::writeData64_IsBE( buff1, uint64_t(0x00000010000493e0ULL), false );
    QVERIFY2( std::memcmp( buff1, rsl1, sizeof(rsl1) ) == 0, "write (IsBE = false) 64bit failed!" );
    QxPack::IcAlgor::writeData64_IsBE( buff1, uint64_t(0xe093040010000000ULL), true );
    QVERIFY2( std::memcmp( buff1, rsl1, sizeof(rsl1) ) == 0, "write (IsBE = true) 64bit failed!" );

    uint8_t rsl2[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
    uint8_t buff2[ sizeof(rsl2)];
    QxPack::IcAlgor::writeData64_IsBE( buff2, int64_t(0x0807060504030201LL), false );
    QVERIFY2( std::memcmp( buff2, rsl2, sizeof(rsl2) ) == 0, "write (IsBE = false) 64bit failed!" );
    QxPack::IcAlgor::writeData64_IsBE( buff2, int64_t(0x0102030405060708LL), true );
    QVERIFY2( std::memcmp( buff2, rsl2, sizeof(rsl2) ) == 0, "write (IsBE = true) 64bit failed!" );

    double v1 = 1.77551234999;
    if ( QxPack::IcAlgor::hostByteOrder() == QxPack::IcAlgor::ByteOrder_LittleEndian ) {
        double v2 = 0;
        QxPack::IcAlgor::writeData64_IsBE( &v2, v1, false );
        QVERIFY2( qFuzzyCompare(v1,v2), "write (IsBE = false) double failed!" );
        double v3_t = 0, v3 = 0;
        QxPack::IcAlgor::writeData64_IsBE( &v3_t, v1, true );
        QxPack::IcAlgor::revAryOrder( reinterpret_cast<const uint8_t*>(&v3_t), reinterpret_cast<uint8_t*>(&v3), sizeof(double));
        QVERIFY2( qFuzzyCompare(v1,v3), "write (IsBE = true ) double failed!");
    } else {
        double v2 = 0;
        QxPack::IcAlgor::writeData64_IsBE( &v2, v1, true );
        QVERIFY2( qFuzzyCompare(v1,v2), "write (IsBE = true) double failed!" );
        double v3_t = 0, v3 = 0;
        QxPack::IcAlgor::writeData64_IsBE( &v3_t, v1, false );
        QxPack::IcAlgor::revAryOrder( reinterpret_cast<const uint8_t*>(&v3_t), reinterpret_cast<uint8_t*>(&v3), sizeof(double));
        QVERIFY2( qFuzzyCompare(v1,v3), "write (IsBE = false ) double failed!");
    }
}


QTEST_MAIN( Tester )

#include "main.moc"

