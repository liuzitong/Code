// ============================================================================
// author: night wing
// date stamp: 20190523 1637
//  1) re-check
// ============================================================================
#ifndef QXPACK_IC_APPSETTINGSBASE_CXX
#define QXPACK_IC_APPSETTINGSBASE_CXX

#include "qxpack_ic_appsettingsbase.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QFile>
#include <QByteArray>
#include <QTextStream>
#include <QReadWriteLock>
#include <QAtomicPointer>
#include <QAtomicInt>
#include <QCoreApplication>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//                      private object
// <mem_cntr>: managed by IcAppSettingsBase
// ////////////////////////////////////////////////////////////////////////////
#define T_PrivPtr( o )  T_ObjCast( IcAppSettingsBasePriv*, o )
class QXPACK_IC_HIDDEN  IcAppSettingsBasePriv {
private:
    QReadWriteLock  m_rw_locker;
    QString         m_setting_path;
    QJsonObject     m_cfg_jo;
    int             m_ldr_cntr;
protected:
    void     tryLoad( );
public :
    explicit IcAppSettingsBasePriv ( const QString & setting_path );
    ~IcAppSettingsBasePriv( );
    inline void  readLock( )      { m_rw_locker.lockForRead(); }
    inline void  unlock( )        { m_rw_locker.unlock();      }
    inline QJsonObject&  joRef( ) { return m_cfg_jo; }
};

// ============================================================================
// ctor
// ============================================================================
IcAppSettingsBasePriv :: IcAppSettingsBasePriv ( const QString & setting_path )
                      : m_rw_locker( QReadWriteLock::Recursive )
{
    m_setting_path = setting_path;
    m_ldr_cntr = 0; this->tryLoad();
}

// ============================================================================
// dtor
// ============================================================================
IcAppSettingsBasePriv :: ~IcAppSettingsBasePriv ( )
{

}

// ============================================================================
// try load configuration
// ============================================================================
void    IcAppSettingsBasePriv :: tryLoad( )
{
    m_rw_locker.lockForRead();
    if ( m_ldr_cntr == 0 ) {
        QString app_set_path;
        if ( ! m_setting_path.isEmpty() ) {
            app_set_path = m_setting_path;
        } else {
            app_set_path = QCoreApplication::applicationDirPath() + "/resource/app_settings.json";
        }
        if ( QFile::exists( app_set_path )) {
            QFile fio( app_set_path );
            if ( fio.open( QFile::ReadOnly | QFile::Text )) {
                QByteArray ba;
                { QTextStream str( & fio ); ba = str.readAll().toUtf8(); }
                m_cfg_jo = QJsonDocument::fromJson( ba ).object();
                ++ m_ldr_cntr;
            }
        }
    }
    m_rw_locker.unlock();
}

// ////////////////////////////////////////////////////////////////////////////
//                             wrapper API
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcAppSettingsBase :: IcAppSettingsBase ( const QString &settings_path, QObject *pa )
                  : QObject( pa )
{
    m_obj = qxpack_ic_new( IcAppSettingsBasePriv, settings_path );
}

// ============================================================================
// dtor
// ============================================================================
IcAppSettingsBase :: ~IcAppSettingsBase ( )
{
    qxpack_ic_delete( m_obj, IcAppSettingsBasePriv );
    m_obj = Q_NULLPTR;
}

// ============================================================================
// return the version string
// ============================================================================
QString   IcAppSettingsBase :: version() const
{
    QString  ver;

    T_PrivPtr( m_obj )->readLock();
    QJsonObject &cfg = T_PrivPtr( m_obj )->joRef();
    ver = cfg.value("version").toString();
    T_PrivPtr( m_obj )->unlock();

    return ver;
}

// ============================================================================
// return the json object configuration path by name
// ============================================================================
QJsonObject  IcAppSettingsBase :: cfgObject( const QString &module_name )
{
    QJsonObject jo;

    T_PrivPtr( m_obj )->readLock();
    QJsonObject &cfg = T_PrivPtr( m_obj )->joRef();
    jo = cfg.value( module_name ).toObject( );
    T_PrivPtr( m_obj )->unlock();

    return jo;
}


}


#endif
