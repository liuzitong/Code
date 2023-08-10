#include <QCoreApplication>

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
#include <QByteArray>
#include <QCryptographicHash>

#include "../datapack/datapack.hxx"

// ////////////////////////////////////////////////////////////////////////////
//
//   test the error deviation template
//
// ////////////////////////////////////////////////////////////////////////////
class  Tester : public QObject {
    Q_OBJECT
private:
    Q_SLOT  void  initTestCase( );
    Q_SLOT  void  cleanupTestCase ( );
    Q_SLOT  void  dataPack();
protected:
    static int  m_mem_cntr;
    static void  memCntr( int req_cntr, const char *file, const char *func, int ln, const char *type );
public :

};
int Tester::m_mem_cntr = 0;

// ============================================================================
// init.
// ============================================================================
auto     Tester :: initTestCase() -> void
{
    DataPack::setMemCntrTrace( & Tester::memCntr );
}

// ============================================================================
// output the memory counter
// ============================================================================
auto     Tester :: cleanupTestCase( ) -> void
{
    qInfo("the last memory counter value:%d", Tester::m_mem_cntr );
}

// ============================================================================
// memory counter
// ============================================================================
auto     Tester :: memCntr( int req_cntr, const char *file, const char *func, int ln, const char *type ) -> void
{
    qDebug("mem req %s:%s   %d", file, func, req_cntr );
    m_mem_cntr += req_cntr;
}

// ============================================================================
// data packing
// ============================================================================
auto     Tester :: dataPack() -> void
{
     QByteArray ba1( 512, 10 );
     QByteArray ba2( 512, 20 );



     auto h1 = QCryptographicHash::hash( ba1, QCryptographicHash::Md5 );
     auto h2 = QCryptographicHash::hash( ba2, QCryptographicHash::Md5 );

     qInfo("the DATA_FILE := %s", DATA_FILE );
     DataPack::FileIo  file_io( DATA_FILE, DataPack::FileIo::Mode_ReadWrite );
     qInfo("file io is worked ? := %s ", ( file_io.isVaild() ? "YES" : "NO" ));
     if ( ! file_io.isVaild() ) { qInfo("file can not created failed!"); return; }
     {
        DataPack::Writer wrt( &file_io, 16 );
        auto idx = uint32_t(0);
        auto ret = wrt.writeRecord( ba1.constData(), size_t( ba1.size()), &idx );
        if ( ! ret ) { qFatal(" write failed ba1!"); } else { qInfo(" write block at %d index!", idx ); }
        qInfo("file size:%ld", file_io.size());
        ret = wrt.writeRecord( ba2.constData(), size_t( ba2.size()), &idx );
        if ( ! ret ) { qFatal(" write failed ba2!"); } else { qInfo(" write block at %d index!", idx ); }
        qInfo("file size:%ld", file_io.size());
     }
     {
        DataPack::Reader rdr( &file_io );
        auto hdr = rdr.header();
        qInfo(" the current rec.: [%d/%d]", hdr.usedRecNum(), hdr.totalRecNum());

        auto rba1 = rdr.readRecord( 0 );
        auto r_ba1 = QByteArray::fromRawData( reinterpret_cast<const char*>( ba1.constData()), int(ba1.size()));
        qInfo(" read data size := %d", r_ba1.size());
        auto r_h1 = QCryptographicHash::hash( r_ba1, QCryptographicHash::Md5 );

        auto rba2 = rdr.readRecord( 1 );
        auto r_ba2 = QByteArray::fromRawData( reinterpret_cast<const char*>( ba2.constData()), int(ba2.size()));
        qInfo(" read data size := %d", r_ba2.size());

        auto r_h2 = QCryptographicHash::hash( r_ba2, QCryptographicHash::Md5 );

        qInfo() << std::memcmp( r_h1.constData(), h1.constData(), h1.size() );
        qInfo() << std::memcmp( r_h2.constData(), h2.constData(), h2.size() );
     }
}



QTEST_MAIN( Tester )

#include "main.moc"

