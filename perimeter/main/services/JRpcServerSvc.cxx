#ifndef RPCSERVERSVC_CXX
#define RPCSERVERSVC_CXX

#include "JRpcServerSvc.hxx"
#include "perimeter/base/common/perimeter_guns.hxx"


#include <QObject>
#include <QProcess>
#include <QApplication>
#include <QStringList>

//namespace CE {


class  JRpcServerSvcPriv : public QObject {
    Q_OBJECT
public:
    bool start(const QString &svr_name){
        QString jpc_path        = QString("%1/%2").arg(QApplication::applicationDirPath()).arg(RPC_PLAT_SVC_PRO_NAME);
        QStringList arguments  = QStringList() << QStringLiteral("--locSvrName") << svr_name;
        m_pro_jpc.start(jpc_path, arguments);
        return m_pro_jpc.waitForStarted();
    }
    void stop(){
        if(m_pro_jpc.state() == QProcess::Running){
//             m_pro_jpc.terminate();
            m_pro_jpc.kill();
//             m_pro_jpc.waitForFinished(1000);
        }
    }
private:
    QProcess        m_pro_jpc;

};

static JRpcServerSvcPriv *g_jrpc_sver   = nullptr;
JRpcServerSvc::JRpcServerSvc()
{

}

bool                        JRpcServerSvc::start(const QString &svr_name){
    if(nullptr == g_jrpc_sver) { g_jrpc_sver = new ( JRpcServerSvcPriv); }
    if(nullptr != g_jrpc_sver) { return g_jrpc_sver->start(svr_name); }
    return false;
}
void                        JRpcServerSvc::stop(){
    if(nullptr != g_jrpc_sver) {
        g_jrpc_sver->stop();
        delete g_jrpc_sver;
        g_jrpc_sver             = nullptr;
    }
}

//}

#include "JRpcServerSvc.moc"
#endif
