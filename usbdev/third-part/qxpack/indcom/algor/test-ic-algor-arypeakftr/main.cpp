#include <QCoreApplication>

#include "../qxpack_ic_algor_arypeakftr.hpp"
#include "../qxpack_ic_algor_peak.hpp"
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
    Q_SLOT  void  cleanupTestCase ();
    Q_SLOT  void  peakEvlopFtrAry_Lerp();
public :

};

// ============================================================================
// output the memory counter
// ============================================================================
auto     Tester :: cleanupTestCase( ) -> void
{

}



// ============================================================================
// test ary ftr krnl work buffer init
// ============================================================================
auto     Tester :: peakEvlopFtrAry_Lerp() -> void
{
    QVector<float> data[] = {
       { 0.1f, 0.1f, 0.1f, 0.6f, 0.5f, 0.7f, 0.5f, 0.3f, 0.5f, 0.2f },
    };

    for ( size_t i = 0; i < sizeof(data)/sizeof(data[0]); i ++ ) {

        auto pk_ary = reinterpret_cast<int8_t*>( std::malloc( size_t( data[i].size()) ));
        IcAlgor::makePeakAry( data[i].constData(), pk_ary, size_t( data[i].size() ));

        QVector<size_t> pkm; auto beg = size_t(0);
        while ( true ) {
            auto dsc = IcAlgor::findNextPeak( data[i].constData(), pk_ary, size_t( data[i].size()), beg );
            if ( ! dsc.is_valid ) { break; }
            pkm.append( dsc.middle );
        }
        std::free( pk_ary );

        IcAlgor::peakEvlopFtrAry_Lerp( data[i].data(), size_t( data[i].size()), pkm.constData(), size_t( pkm.size()), 3 );
        qInfo() << data[i];
    }
}



QTEST_MAIN( Tester )

#include "main.moc"

