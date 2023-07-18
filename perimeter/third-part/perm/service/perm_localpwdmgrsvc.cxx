#ifndef PERM_LOCALPWDMGRSVC_CXX
#define PERM_LOCALPWDMGRSVC_CXX

#include "perm_localpwdmgrsvc.hxx"
#include "perm_aesencryption.hxx"
#include "../../base/common/perimeter_guns.hxx"
#include "perm/common/perm_guns.hxx"
#include "perm/common/perm_memcntr.hxx"
#include "qxpack/indcom/sys/qxpack_ic_rmtobjdeletor_priv.hxx"
#include "qxpack/indcom/afw/qxpack_ic_appctrlbase.hxx"
#include "qxpack/indcom/afw/qxpack_ic_msgbus.hxx"
#include <QMutex>
#include <QDir>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>

#define  Text_LastDat   QStringLiteral("last.dat")

namespace FcPerm {

#define T_PrivPtr( o )  perm_objcast( LocalPwdMgrSvcPriv*, o )
class PERM_HIDDEN LocalPwdMgrSvcPriv : public QObject
{
    Q_OBJECT
private:
    LocalPwdMgrSvc   * m_parent;
    QString  m_file_path;
    quint64  m_uid;
    QString  m_name;
    QString  m_pwd;
    bool     m_remember_pwd, m_last_remember_pwd, m_auto_login;

protected:
    Q_SLOT void  IcMsgBus_onMsg ( const QxPack::IcMsgBusPkg &pkg );
    void setupLocalLogin ( const QJsonObject &jo );
    void removeLocalLogin( const QJsonObject &jo );
    void modifyLocalLogin( const QJsonObject &jo );

public:
    explicit LocalPwdMgrSvcPriv ( LocalPwdMgrSvc *pa );
    ~LocalPwdMgrSvcPriv( );

    inline quint64 uid ( ) { return m_uid; }
    inline QString name( ) { return m_name; }
    inline QString pwd ( ) { return m_pwd; }
    
    bool    rememberpwd(){ return m_remember_pwd; }
    bool    autoLogin()  { return m_auto_login;   }
    void    setRememberpwd( bool r);
    void    loginTo(QString name, QString pwd);

    bool  read( QJsonObject & );
    bool  write( );
    bool  remove( );
};

static QString mkMutiDir(const QString & path)
{
    QDir dir(path);
    if ( dir.exists(path)){
        return path;
    }
    QString parentDir = mkMutiDir(path.mid(0,path.lastIndexOf('/')));
    QString dirname = path.mid(path.lastIndexOf('/') + 1);
    QDir parentPath(parentDir);
    if ( !dirname.isEmpty() )
        parentPath.mkpath(dirname);
    return parentDir + "/" + dirname;
}

void LocalPwdMgrSvcPriv::IcMsgBus_onMsg(const QxPack::IcMsgBusPkg &pkg)
{
    QJsonObject jo_param = pkg.joParam();
    QString method = jo_param.value("method").toString();
    if ( method == "setupLocalLogin" ) {
        setupLocalLogin(jo_param);
    } else if ( method == "removeLocalLogin" ) {
        removeLocalLogin(jo_param);
    } else if ( method == "modifyLocalLogin" ) {
        modifyLocalLogin(jo_param);
    }
}

void LocalPwdMgrSvcPriv::setupLocalLogin(const QJsonObject &jo)
{
    m_uid = jo.contains("UID") ? QString(AESEncryption::decode(QByteArray::fromBase64(jo.value("UID").toString().toUtf8()))).toULongLong(Q_NULLPTR, 16) : 0;
    m_name = jo.contains("name") ? QString(AESEncryption::decode(QByteArray::fromBase64(jo.value("name").toString().toUtf8()))) : QString();
    m_pwd = jo.contains("pwd") ? QString(AESEncryption::decode(QByteArray::fromBase64(jo.value("pwd").toString().toUtf8()))) : QString();
    m_remember_pwd = jo.contains("remember_pwd") ? jo.value("remember_pwd").toBool() : false;
    m_auto_login   = jo.contains("auto_login") ? jo.value("auto_login").toBool() : true;
    emit m_parent->dataChanged();
    this->write( );
}

void LocalPwdMgrSvcPriv::removeLocalLogin(const QJsonObject &jo)
{
    quint64 uid = QString(QByteArray(AESEncryption::decode(QByteArray::fromBase64(jo.value("UID").toString().toUtf8())))).toULongLong(Q_NULLPTR, 16);
    if ( uid == m_uid ) {
        this->remove();
        m_uid = 0;
        m_name = QString();
        m_pwd = QString();
        m_remember_pwd = m_auto_login = false;
        emit m_parent->dataChanged();
    }
}

void LocalPwdMgrSvcPriv::modifyLocalLogin(const QJsonObject &jo)
{
    if ( jo.contains("UID") ) {
        quint64 uid = QString(QByteArray(AESEncryption::decode(QByteArray::fromBase64(jo.value("UID").toString().toUtf8())))).toULongLong(Q_NULLPTR, 16);
        if ( uid == m_uid ) {
            m_name = jo.contains("name") ? QString(AESEncryption::decode(QByteArray::fromBase64(jo.value("name").toString().toUtf8()))) : QString();
            m_pwd = jo.contains("pwd") ? QString(AESEncryption::decode(QByteArray::fromBase64(jo.value("pwd").toString().toUtf8()))) : QString();
            emit m_parent->dataChanged();
            this->write( );
        }
    } else {
        QString name = jo.contains("name") ? QString(AESEncryption::decode(QByteArray::fromBase64(jo.value("name").toString().toUtf8()))) : QString();
        if ( name == m_name ) {
            m_pwd = jo.contains("pwd") ? QString(AESEncryption::decode(QByteArray::fromBase64(jo.value("pwd").toString().toUtf8()))) : QString();
            emit m_parent->dataChanged();
            this->write( );
        }
    }
}

LocalPwdMgrSvcPriv::LocalPwdMgrSvcPriv(LocalPwdMgrSvc *pa)
{
    m_parent = pa;
    QDir dir(QCoreApplication::applicationDirPath());
//    dir.cdUp();
    m_file_path = QString("%1/params/syseye-nw/").arg(dir.path());
    m_remember_pwd = m_last_remember_pwd = m_auto_login = false;

    QJsonObject param;
    this->read( param );
    m_uid = param.contains("UID") ? param.value("UID").toString().toULongLong(Q_NULLPTR, 16) : 0;
    m_name = param.contains("name") ? param.value("name").toString() : QString();
    m_pwd = param.contains("pwd") ? param.value("pwd").toString() : QString();
    m_remember_pwd = m_last_remember_pwd = param.contains("remember_pwd") ? param.value("remember_pwd").toBool() : false;
    m_auto_login = param.contains("auto_login") ? param.value("auto_login").toBool() : false;

    QxPack::IcAppCtrlBase *app_ctrl = QxPack::IcAppCtrlBase::instance( GUNS_AppCtrl );
    QxPack::IcMsgBus *msg_bus = app_ctrl->msgBus();

    // Hint: here we listen the message group
    msg_bus->add( this, GUNS_LocalPwdMgrSvcMsgGroup );
}

LocalPwdMgrSvcPriv::~LocalPwdMgrSvcPriv()
{
    QxPack::IcAppCtrlBase *app_ctrl = QxPack::IcAppCtrlBase::instance( GUNS_AppCtrl );
    QxPack::IcMsgBus *msg_bus = app_ctrl->msgBus();

    // Hint: remember to remove from msg-group
    msg_bus->rmv( this, GUNS_LocalPwdMgrSvcMsgGroup );
}

void LocalPwdMgrSvcPriv::setRememberpwd(bool r)
{
    m_remember_pwd = r;
}

void LocalPwdMgrSvcPriv::loginTo(QString name, QString pwd)
{
    bool is_save = false;
    if(m_remember_pwd){
        if(m_name != name || pwd != m_pwd){
            m_name = name; m_pwd = pwd; is_save = true;
        }else{
            if(!m_last_remember_pwd){ m_last_remember_pwd = true; is_save = true; }
        }
    }else{
        if(!m_auto_login){
            if(!m_name.isEmpty() && !m_pwd.isEmpty()){
                m_name = QString(); m_pwd = QString();
                is_save = true;
            }
        }
    }
    if(is_save){ this->write(); }
}


bool LocalPwdMgrSvcPriv::read(QJsonObject &jo)
{
    QString path = m_file_path + Text_LastDat;
    if ( !QFile::exists(path) )
    {
        mkMutiDir( path.mid(0, path.lastIndexOf("/")) );
    }
    QFile file(path);
    if ( !file.open(QIODevice::ReadOnly) ) { return false; }
    QJsonDocument doc = QJsonDocument::fromJson(AESEncryption::decode(QByteArray::fromBase64(file.readAll())));
    if ( doc.isObject() )
    {
        jo = doc.object();
    }

    file.close();
    return true;
}

bool LocalPwdMgrSvcPriv::write()
{
    QString path = m_file_path + Text_LastDat;
    if ( !QFile::exists(path) )
    {
        mkMutiDir( path.mid(0, path.lastIndexOf("/")) );
    }
    QFile file(path);
    if ( !file.open(QIODevice::WriteOnly) ) { return false; }

    QJsonObject param = {{"UID", QString("%1").arg(m_uid, 16, 16, QChar('0'))}, {"name", m_name}, {"pwd", m_pwd}, {"remember_pwd", m_remember_pwd}, {"auto_login", m_auto_login}};

    file.write(AESEncryption::encode(QJsonDocument(param).toJson()).toBase64());

    file.close();
    return true;
}

bool LocalPwdMgrSvcPriv::remove()
{
    QString path = m_file_path + Text_LastDat;
    return QFile::remove(path);
}

static QAtomicPointer<LocalPwdMgrSvc>  g_ptr( Q_NULLPTR );
static QMutex       g_locker;
static QAtomicInt   g_ref_cntr(0);

LocalPwdMgrSvc * LocalPwdMgrSvc::getInstance()
{
    LocalPwdMgrSvc *ptr = Q_NULLPTR;

    g_locker.lock();
    ptr = g_ptr.loadAcquire();
    if ( ptr == Q_NULLPTR ) {
        g_ptr.store( ( ptr = new LocalPwdMgrSvc( ) ) );
        g_ref_cntr.store(1);
    } else {
        g_ref_cntr.fetchAndAddOrdered(1);
    }
    g_locker.unlock();
    return ptr;
}

void LocalPwdMgrSvc::release()
{
    LocalPwdMgrSvc *ptr = Q_NULLPTR;

    g_locker.lock();
    ptr = g_ptr.loadAcquire();
    if ( ptr != Q_NULLPTR ) {
        if ( g_ref_cntr.fetchAndSubOrdered(1) - 1 == 0 ) {
            QxPack::IcRmtObjDeletor::deleteObjInThread(
                ptr->thread(),
                [](void*, QObject* obj ) {
                    LocalPwdMgrSvc *svc = qobject_cast<LocalPwdMgrSvc*>( obj );
                    delete svc;
                }, Q_NULLPTR, ptr, false
            );
            g_ptr.store( Q_NULLPTR );
        }
    }
    g_locker.unlock();
}

quint64 LocalPwdMgrSvc::uid() const
{ return T_PrivPtr( m_obj )->uid(); }

QString LocalPwdMgrSvc::name() const
{ return T_PrivPtr( m_obj )->name(); }

QString LocalPwdMgrSvc::pwd() const
{ return T_PrivPtr( m_obj )->pwd(); }

bool LocalPwdMgrSvc::rememberpwd()
{ return T_PrivPtr( m_obj )->rememberpwd(); }

bool LocalPwdMgrSvc::autoLogin()
{ return T_PrivPtr( m_obj )->autoLogin(); }

void LocalPwdMgrSvc::setRememberpwd(bool r)
{ T_PrivPtr( m_obj )->setRememberpwd( r ); }

void LocalPwdMgrSvc::loginTo(QString name, QString pwd)
{ T_PrivPtr( m_obj )->loginTo( name, pwd); }

LocalPwdMgrSvc::LocalPwdMgrSvc()
{
    m_obj = perm_new( LocalPwdMgrSvcPriv, this );
}

LocalPwdMgrSvc::~LocalPwdMgrSvc()
{
    perm_delete( m_obj, LocalPwdMgrSvcPriv );
}

}

#include "perm_localpwdmgrsvc.moc"
#endif // PERM_LOCALPWDMGRSVC_CXX
