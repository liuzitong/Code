#include <QCoreApplication>
#include <QObject>
#include <QDebug>
#include "qxpack/indcom/net/qxpack_ic_datatrans_cli.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/sys/qxpack_ic_appdcl_priv.hxx"
#include "qxpack/indcom/sys/qxpack_ic_sysevtqthread_priv.hxx"
#include "qxpack/indcom/sys/qxpack_ic_rmtobjcreator_priv.hxx"
#include "qxpack/indcom/sys/qxpack_ic_rmtobjdeletor_priv.hxx"

#include <QTime>
#include <QList>
#include <QProcess>
#include <QThread>
#include <QByteArray>
#include <QString>

#define SVR_EXE  "datatrans-svr.exe"
#define SVR_NAME "datatrans-svr"

#ifdef TEST_DATATRANS_TCP
#define RS_TYPE  "rsTcp"
#define CONN_HOST "127.0.0.1"
#else
#define RS_TYPE "rsLoc"
#define CONN_HOST SVR_NAME

#endif

// ////////////////////////////////////////////////////////////////////////////
//
//                           static functions
//
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// make quit method call
// ============================================================================
static QByteArray  gMakeQuitCall( )
{
    return "rpc.quitSvr";
}

// ============================================================================
// make normal call
// ============================================================================
static QByteArray  gMakeNormalCall( )
{
    static int cntr = 0;
    return QString("cli message: %1 RMB.").arg( cntr ++ ).toUtf8();
}

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

// ============================================================================
// start the process
// ============================================================================
static bool gStartProcess( )
{
#ifdef WIN32
    QProcess proc;
    proc.startDetached( SVR_EXE );

    bool is_proc_existed = false;
    for ( int i = 0; i < 5; i ++ ) {
        QThread::msleep(1000);
        if ( gFindProcessByName( SVR_NAME )) {
            is_proc_existed = true; break;
        }
    }

    QThread::msleep(500);
    return is_proc_existed;
#else
    #error this platform is not supported.
#endif
}

// ////////////////////////////////////////////////////////////////////////////
//
//                               data generator
//
// ////////////////////////////////////////////////////////////////////////////
class DataGen {
private: QList<QByteArray> m_data_list;
public :
    explicit DataGen ( )
    {
        int expect_loop = qrand()% 20 + 10;
        if ( expect_loop < 1 ) { expect_loop = 1; }
        for ( int x = 0; x < expect_loop; x ++ ) {
            m_data_list.append( gMakeNormalCall( ));
        }
    }
    inline QList<QByteArray> dataList( ) { return m_data_list; }
};


// ////////////////////////////////////////////////////////////////////////////
//
//                       data trans client
//
// ////////////////////////////////////////////////////////////////////////////
class DataTransClient : public QObject {
    Q_OBJECT
private:
    QxPack::IcSysEvtQThreadPriv  m_sys_evt_thr;
    QList<QByteArray>       m_data_list;
    QxPack::IcDataTransCli *m_cli;

protected:
    Q_SLOT void cli_onHostConnected( ) { }
    Q_SLOT void cli_onNewRecvData  ( );
    Q_SLOT void cli_onFinished     ( ) { qInfo("connection finished"); QCoreApplication::quit(); }

           void postData ( );
           void tryFinishSvr( );
public :
    explicit DataTransClient( );
    virtual ~DataTransClient( ) Q_DECL_OVERRIDE;
};

// ============================================================================
// ctor
// ============================================================================
DataTransClient :: DataTransClient ( )
{
    m_data_list = DataGen().dataList();
    m_cli = new QxPack::IcDataTransCli( RS_TYPE, m_sys_evt_thr.thread() );

    QObject::connect ( m_cli, SIGNAL(hostConnected()), this, SLOT(cli_onHostConnected()));
    QObject::connect ( m_cli, SIGNAL(newRecvData()),   this, SLOT(cli_onNewRecvData()));
    QObject::connect ( m_cli, SIGNAL(finished()),      this, SLOT(cli_onFinished()));
    QObject::connect (
         m_cli, & QxPack::IcDataTransCli::postDataSent,
        []() {
            qInfo(": a post data has been sent");
        }
    );
    m_cli->connectToHost( CONN_HOST, 9999 );
}

// ============================================================================
// dtor
// ============================================================================
DataTransClient :: ~DataTransClient( )
{
    m_cli->blockSignals(true);
//    QObject::disconnect( &m_cli, Q_NULLPTR, this, Q_NULLPTR );
}

// ============================================================================
// post data
// ============================================================================
void  DataTransClient :: postData()
{
    if ( ! m_data_list.isEmpty() ) {
        QByteArray ba = m_data_list.takeFirst();
        m_cli->post( ba );
    }
}

// ============================================================================
// try finish server if all request has been solved
// ============================================================================
void  DataTransClient :: tryFinishSvr()
{
    if ( m_data_list.isEmpty() ) { // all request has been solved.
        QByteArray ba = gMakeQuitCall();
        m_cli->post( ba );
    }
}

// ============================================================================
// slot: handle the response from server
// ============================================================================
void  DataTransClient :: cli_onNewRecvData()
{
    QByteArray ba;
    if ( m_cli->takeNextPendingRecvData(ba)) {
       qInfo("%s",ba.constData());
       this->postData();
    }
    this->tryFinishSvr();
}


// ////////////////////////////////////////////////////////////////////////////
//
//                           main entry
//
// ////////////////////////////////////////////////////////////////////////////
int   main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    int ret = 0; QTime tm; tm.start();
    qsrand( uint( tm.msecsSinceStartOfDay()) );

    if ( ! gFindProcessByName( SVR_NAME )) {
        if ( ! gStartProcess()) {
            qInfo("cound not start server!");
            exit(-1);
        }
    }

    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    qInfo("before exec, mem cntr is %d", mem_cntr );
    {
        DataTransClient cli;
        ret = app.exec();
    }
    QxPack::IcAppDclPriv::barrier(16);
    int curr_cntr = QxPack::IcMemCntr::currNewCntr();
    if ( curr_cntr != mem_cntr) {
        qInfo("current mem counter is:%d, not equal to before cntr:%d", curr_cntr, mem_cntr);
    } else {
        qInfo("after exec, mem cntr is %d", curr_cntr );
    }

    return ret;
}

#include "cli.moc"
