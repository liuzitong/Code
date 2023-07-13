#ifndef PERM_SVRPROCESS_CXX
#define PERM_SVRPROCESS_CXX

#include "perm_svrprocess.hxx"
#include "perm/common/perm_memcntr.hxx"
#include <QProcess>

namespace FcPerm {

#define T_PrivPtr( o )  perm_objcast( SvrProcessPriv*, o )
class PERM_HIDDEN SvrProcessPriv : public QObject
{
    Q_OBJECT
private:
    SvrProcess  *m_parent;
    QProcess     m_process;
    bool  m_is_working;

public:
    SvrProcessPriv ( SvrProcess *pa );
    ~SvrProcessPriv( );

    inline bool isWorking( ) { return m_is_working; }

    void  start( const QString &program, const QStringList &arguments );

protected:
    Q_SLOT void  onStarted( );
    Q_SLOT void  onFinished( int, QProcess::ExitStatus );
};

SvrProcessPriv::SvrProcessPriv(SvrProcess *pa)
{
    m_parent = pa;
    m_is_working = false;

    QObject::connect( & m_process, SIGNAL(finished(int, QProcess::ExitStatus)),
                      this, SLOT(onFinished(int, QProcess::ExitStatus)) );
    QObject::connect( & m_process, SIGNAL(started()), this, SLOT(onStarted()));
//    QObject::connect( & m_process, &QProcess::readyReadStandardError, [this](){
//        std::fprintf(stderr, "rpcSrv stderr: %s\n", m_process.readAllStandardError().constData()); std::fflush(stderr);
//        qInfo() << "rpcSrv stderr" << m_process.readAllStandardError();
//    });
//    QObject::connect( & m_process, &QProcess::readyReadStandardOutput, [this](){
//        std::fprintf(stdout, "rpcSrv stdout: %s\n", m_process.readAllStandardOutput().constData()); std::fflush(stdout);
//        qInfo() << "rpcSrv stdout" << m_process.readAllStandardOutput();
//    });
}

SvrProcessPriv::~SvrProcessPriv()
{
    m_process.waitForFinished( );
}

void SvrProcessPriv::start(const QString &program, const QStringList &arguments)
{
    m_process.start( program, arguments );
    m_process.waitForStarted( );
}

void SvrProcessPriv::onStarted()
{
    m_is_working = true;
    QMetaObject::invokeMethod( m_parent, "started", Qt::QueuedConnection );
}

void SvrProcessPriv::onFinished(int, QProcess::ExitStatus)
{
    m_is_working = false;
    QMetaObject::invokeMethod( m_parent, "finished", Qt::QueuedConnection );
}

SvrProcess::SvrProcess( )
{
    m_obj = perm_new( SvrProcessPriv, this );
}

SvrProcess::~SvrProcess()
{
    perm_delete( m_obj, SvrProcessPriv );
}

bool SvrProcess::isWorking() const
{ return T_PrivPtr( m_obj )->isWorking(); }

void SvrProcess::start(const QString &program, const QStringList &arguments)
{ T_PrivPtr( m_obj )->start( program, arguments ); }

}

#include "perm_svrprocess.moc"
#endif // PERM_SVRPROCESS_CXX
