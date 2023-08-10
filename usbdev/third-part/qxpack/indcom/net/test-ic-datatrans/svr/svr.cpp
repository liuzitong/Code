#include <QCoreApplication>
#include <QDebug>
#include <QObject>
#include <QtTest>
#include <QMetaObject>
#include <QVariant>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QProcess>
#include <QByteArray>

#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/net/qxpack_ic_datatrans_svr.hxx"
#include "qxpack/indcom/sys/qxpack_ic_appdcl_priv.hxx"

#define CLI_NAME "datatrans-cli"
#define CLI_EXE  "datatrans-cli.exe"
#define SVR_NAME "datatrans-svr"

#ifdef TEST_DATATRANS_TCP
#define RS_TYPE "rsTcp"
#else
#define RS_TYPE "rsLoc"
#endif

// ////////////////////////////////////////////////////////////////////////////
//
// local function
//
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// enum all process that try to find named process
// ============================================================================
static bool  gFindProcessByName ( const char *name )
{
    bool is_found = false;
#ifdef WIN32
    QProcess proc;
    proc.start("tasklist", QProcess::ReadOnly);
    bool ret = proc.waitForStarted();
    if ( ret ) { ret = proc.waitForFinished(); }
    if ( ret ) {
        QByteArray ba = proc.readAll();
        if ( ! ba.isEmpty()) {
            is_found = ( ba.indexOf(name) != -1 );
        }
    } else {
        qFatal("could not start the tasklist.");
    }
#else
    #error this platform is not supported.
#endif
    return is_found;
}

// ////////////////////////////////////////////////////////////////////////////
//
//  simple svr handler
//
// ////////////////////////////////////////////////////////////////////////////
class SvrHandler : public QObject {
    Q_OBJECT
private:
    QxPack::IcDataTransHandler *m_hde;  QTimer m_notice_tmr;
public :
    // ========================================================================
    // ctor
    // ========================================================================
    SvrHandler ( QxPack::IcDataTransHandler *hde, QObject *pa ) : QObject(pa)
    {
        m_hde = hde;
        QObject::connect( hde, SIGNAL(newRecvData()), this, SLOT(onNewRecvData()));
        QObject::connect( hde, & QxPack::IcDataTransHandler::postDataSent,
            [](){ qInfo(": a post data has been sent."); }
        );
        // --------------------------------------------------------------------
        // notice timer used to trigger post message to client
        // --------------------------------------------------------------------
        m_notice_tmr.setInterval(200);
        m_notice_tmr.setSingleShot(false);
        QObject::connect(
            &m_notice_tmr, &QTimer::timeout,
            [this]() {
                if ( this->m_hde != Q_NULLPTR ) {
                    static int cnt = 0;
                    this->m_hde->post (
                        QString("svr message: show me the money %1").arg(cnt++).toUtf8()
                    );
                }
            }
        );
        m_notice_tmr.start();
    }

    // ------------------------------------------------------------------------
    // dtor
    // ------------------------------------------------------------------------
    virtual ~SvrHandler ( ) Q_DECL_OVERRIDE
    {
        m_notice_tmr.stop();
        QObject::disconnect ( m_hde, Q_NULLPTR, this, Q_NULLPTR );
        m_hde->deleteLater();
    }

    // ------------------------------------------------------------------------
    // while recv. the request, invoke callback function
    // ------------------------------------------------------------------------
    Q_SLOT void  onNewRecvData( )
    {
        QByteArray ba;
        if ( m_hde->takeNextPendingRecvData( ba )) {
            qInfo("%s", ba.constData());
            if ( ba == "rpc.quitSvr") { QCoreApplication::quit(); }
        }

    }
};


// ////////////////////////////////////////////////////////////////////////////
// test object
// ////////////////////////////////////////////////////////////////////////////
class  Test : public QObject {
    Q_OBJECT
private:
Q_SLOT  void initTestCase ( ) {  }
Q_SLOT  void cleanupTestCase( );
Q_SLOT  void testSvr( );
public :
    Test ( ) { }
    ~Test( ) { }
};

// ============================================================================
// final cleanup
// ============================================================================
void    Test :: cleanupTestCase()
{
    qInfo() << "current MemCntr:" << QxPack::IcMemCntr::currNewCntr();
}

// ============================================================================
// test the sub
// ============================================================================
void    Test :: testSvr( )
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    //QxPack::IcMemCntr::enableMemTrace(true);
    {
        // --------------------------------------------------------------------
        // create a tcp server
        // --------------------------------------------------------------------
        QxPack::IcDataTransSvr  *svr = new QxPack::IcDataTransSvr( RS_TYPE, SVR_NAME, 9999 );
        QObject::connect (
            svr, & QxPack::IcDataTransSvr::newHandler,
            [svr]() {
                QxPack::IcDataTransHandler *hde =
                    qobject_cast<QxPack::IcDataTransHandler*>(svr->takeNextPendingHandler());
                if ( hde != Q_NULLPTR ) {
                    new SvrHandler( hde, svr );
                    //hde->deleteLater();
                }
            }
        );
        svr->startServer();
        if ( ! svr->isListening() ) {
            delete svr; QVERIFY2( false, "the tcp server is not working!");
        }

        // --------------------------------------------------------------------
        // detect the cli program
        // --------------------------------------------------------------------
        #if defined(WIN32)
        if ( ! gFindProcessByName( CLI_NAME )) {
           if ( QFile::exists( QCoreApplication::applicationDirPath() + "/" + CLI_EXE )) {
                qInfo("found " CLI_EXE );
                QProcess::startDetached( QCoreApplication::applicationDirPath() + "/" CLI_EXE );
            }
        }
        #endif

        QCoreApplication::instance()->exec(); // enter eventloop
        svr->deleteLater();
    }
    QxPack::IcAppDclPriv::barrier( 16 );

    //QxPack::IcMemCntr::saveTraceInfoToFile("z:\\t.txt");
    QVERIFY2 ( mem_cntr == QxPack::IcMemCntr::currNewCntr(),
        "current memory counter is not equal"
    );
}




QTEST_MAIN( Test )
#include "svr.moc"
