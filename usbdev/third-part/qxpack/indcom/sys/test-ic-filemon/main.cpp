#include <QCoreApplication>
#include <QDebug>
#include <cstdlib>
#include <QObject>
#include <QtTest>
#include <QThread>
#include <QMap>
#include <QImage>

#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/sys/qxpack_ic_filemon.hxx"
#include "qxpack/indcom/sys/qxpack_ic_ncstring.hxx"
#include "qxpack/indcom/sys/qxpack_ic_apptot_priv.hxx"
#include "qxpack/indcom/sys/qxpack_ic_sysevtqthread_priv.hxx"
#include "qxpack/indcom/sys/qxpack_ic_appdcl_priv.hxx"


// ////////////////////////////////////////////////////////////////////////////
// static functions
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// generate the temp file name only
// ============================================================================
static QMap<QxPack::IcNcString,int>  gGenTmpName( )
{
   QMap<QxPack::IcNcString,int> file_map;
   file_map.insert( "1.jpg", true );
   file_map.insert( "2.jpg", true );
   file_map.insert( "3.jpg", true );
   return file_map;
}

// ============================================================================
// generate the temp file at specified path
// ============================================================================
static void gGenWriteImage( const QString &dir, const QMap<QxPack::IcNcString,int> &file_map )
{
   QImage test_im( 400, 400, QImage::Format_RGBA8888 ); test_im.fill( 0x00ff00 );
   QMap<QxPack::IcNcString,int>::const_iterator c_itr = file_map.cbegin();
   while ( c_itr != file_map.cend()) {
       test_im.save( dir + "/" + c_itr.key().strRef(), "jpeg");
       c_itr ++;
   }
}

// ============================================================================
// delete all temp file that in map
// ============================================================================
static void   gDelTempImage( const QString &dir, QMap<QxPack::IcNcString,int> &file_map )
{
   QMap<QxPack::IcNcString,int>::iterator itr = file_map.begin();
   while ( itr != file_map.end()) {
       QFile::remove( dir + "/" + itr.key().strRef() );
       itr ++;
   }
}

// ////////////////////////////////////////////////////////////////////////////
//
// test object
//
// ////////////////////////////////////////////////////////////////////////////
class  Test : public QObject {
    Q_OBJECT
private:
Q_SLOT  void initTestCase( ) { }
Q_SLOT  void cleanupTestCase( );
Q_SLOT  void testFirstScan( );
Q_SLOT  void testDynScan  ( );
public :
    Test ( ) { }
    ~Test( ) { }
};

// ============================================================================
// final cleanup
// ============================================================================
void  Test :: cleanupTestCase()
{
    qInfo() << "current MemCntr:" << QxPack::IcMemCntr::currNewCntr();
}

// ============================================================================
// test first scan the file.
// ============================================================================
void   Test :: testFirstScan()
{
   int mem_cntr = QxPack::IcMemCntr::currNewCntr();
   //QxPack::IcMemCntr::enableMemTrace( true );

   {
       QMap<QxPack::IcNcString,int> file_map = gGenTmpName();
       gGenWriteImage( QCoreApplication::applicationDirPath(), file_map );
       static int testFirstScan_file_cntr = file_map.count();

       // create a new file mon object
       QxPack::IcFileMon  *file_mon = new QxPack::IcFileMon (
           QStringList() << "*.jpg"
       );

       // listen the file first found.
       QObject::connect (
           file_mon, & QxPack::IcFileMon::fileFirstFound,
           [ file_mon ]( const QStringList &fn_list ) {
               static int add_cntr = 0;

               QStringList::const_iterator c_itr = fn_list.constBegin();
               while ( c_itr != fn_list.constEnd()) {
                   qInfo() << "added:" << ( *c_itr ++ );
               }
               add_cntr += fn_list.size();

               if ( add_cntr >= testFirstScan_file_cntr ) {
                   file_mon->deleteLater();
               }
           }
       );

       // start the monitor folder
       file_mon->setWorkPath( QCoreApplication::applicationDirPath() );

       // create wait timer, and enter in main loop
       QxPack::IcAppTotPriv app_tot( 10000, 64,
           []( void *, QxPack::IcAppTotPriv &tot, bool is_limit ) {
               if ( is_limit ) {
                   qInfo() << "TimeOut! quit MainEventLoop!";
                   QCoreApplication::quit();
               } else {
                   if ( ! QxPack::IcSysEvtQThreadPriv::isAlive()) {
                       tot.stopTimer();
                       QCoreApplication::quit();
                   }
               }
           }, nullptr
       );
       QCoreApplication::instance()->exec();

       // remove all temp image
       gDelTempImage( QCoreApplication::applicationDirPath(), file_map );
   }
   QxPack::IcAppDclPriv::barrier();

   //QxPack::IcMemCntr::saveTraceInfoToFile("z:/mem_cntr.txt" );
//   QxPack::IcMemCntr::enableMemTrace(false);
//   qInfo() << "current counter is:" << QxPack::IcMemCntr::currNewCntr();
   QVERIFY2( mem_cntr == QxPack::IcMemCntr::currNewCntr(), "current memory counter is not equal" );
}

// ====================================================
// test dync. scan
// ====================================================
#if 1
void   Test :: testDynScan()
{
   int mem_cntr = QxPack::IcMemCntr::currNewCntr();
   QxPack::IcMemCntr::enableMemTrace( false );

   {
       QMap<QxPack::IcNcString,int> file_map = gGenTmpName();
       static int testDynScan_file_cntr = file_map.count();
       static QMap<QxPack::IcNcString,int> *testDyncScan_file_map = & file_map;

       QxPack::IcFileMon *file_mon = new QxPack::IcFileMon(
           QStringList() << "*.jpg"
       );

       QObject::connect(
           file_mon, & QxPack::IcFileMon::fileAdded,
           [file_mon]( const QStringList &fn_list ) {
              static int add_cntr = 0;

              QStringList::const_iterator c_itr = fn_list.constBegin();
              while ( c_itr != fn_list.constEnd()) {
                   qInfo() << "added:" << ( *c_itr ++ );
               }
               add_cntr += fn_list.size();

               if ( add_cntr >= testDynScan_file_cntr ) {
                   file_mon->deleteLater();
               }

           }
       );

       file_mon->setWorkPath( QCoreApplication::applicationDirPath() ); // start monitor

       // create the write timer after 1000, the write the file
       QTimer write_tmr;
       QObject::connect(
           & write_tmr, & QTimer::timeout,
           [](){ gGenWriteImage( QCoreApplication::applicationDirPath(), *testDyncScan_file_map ); }
       );
       write_tmr.start(1000);

       // create wait timer, and enter in main loop
       QxPack::IcAppTotPriv app_tot( 10000, 64,
           []( void *, QxPack::IcAppTotPriv &tot, bool is_limit ) {
               if ( is_limit ) {
                   qInfo() << "TimeOut! quit MainEventLoop!";
                   QCoreApplication::quit();
               } else {
                   if ( ! QxPack::IcSysEvtQThreadPriv::isAlive()) {
                       tot.stopTimer();
                       QCoreApplication::quit();
                   }
               }
           }, nullptr
       );
       QCoreApplication::instance()->exec();

       // del temp image
       gDelTempImage( QCoreApplication::applicationDirPath(), file_map );
   }
   QxPack::IcAppDclPriv::barrier();

//   QxPack::IcMemCntr::saveTraceInfoToFile("mem_cntr.txt" );
//   QxPack::IcMemCntr::enableMemTrace(false);
   QVERIFY2( mem_cntr == QxPack::IcMemCntr::currNewCntr(), "current memory counter is not equal" );
}
#endif


QTEST_MAIN( Test )
#include "main.moc"
