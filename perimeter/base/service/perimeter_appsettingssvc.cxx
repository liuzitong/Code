#ifndef PERIMETER_APPSETTINGSSVC_CXX
#define PERIMETER_APPSETTINGSSVC_CXX

#include "perimeter_appsettingssvc.hxx"
#include "perimeter/base/common/perimeter_memcntr.hxx"
#include <QMutex>
#include <QAtomicPointer>
#include <QAtomicInt>

#include "qxpack/indcom/sys/qxpack_ic_rmtobjdeletor_priv.hxx"

namespace Perimeter {

// ///////////////////////////////////////////////////////////
// msgbussvc wrapper
// ///////////////////////////////////////////////////////////
// ==========================================================
// ctor
// ==========================================================
AppSettingsSvc :: AppSettingsSvc ( const QString &settings_file_path, QObject *pa )
               : QxPack::IcAppSettingsBase( settings_file_path, pa )
{

}

// ==========================================================
// dtor
// ==========================================================
AppSettingsSvc :: ~AppSettingsSvc ( ) { }


// ///////////////////////////////////////////////////////////
// static factory
// ///////////////////////////////////////////////////////////

static QAtomicPointer<AppSettingsSvc>  g_ptr( Q_NULLPTR );
static QMutex       g_locker;
static QAtomicInt   g_ref_cntr(0);

// ==========================================================
// get new instance
// ==========================================================
AppSettingsSvc *  AppSettingsSvc::getInstance( )
{
    AppSettingsSvc *ptr = Q_NULLPTR;

    g_locker.lock();
    ptr = g_ptr.loadAcquire();
    if ( ptr == Q_NULLPTR ) {
        g_ptr.store( ( ptr = new AppSettingsSvc( QString() ) ) );
        g_ref_cntr.store(1);
    } else {
        g_ref_cntr.fetchAndAddOrdered(1);
    }
    g_locker.unlock();
    return ptr;
}

// =========================================================
// free the instance
// =========================================================
void  AppSettingsSvc::freeInstance()
{
    AppSettingsSvc *ptr = Q_NULLPTR;

    g_locker.lock();
    ptr = g_ptr.loadAcquire();
    if ( ptr != Q_NULLPTR ) {
        if ( g_ref_cntr.fetchAndSubOrdered(1) - 1 == 0 ) {
            QxPack::IcRmtObjDeletor::deleteObjInThread(
                ptr->thread(),
                [](void*, QObject* obj ) {
                    AppSettingsSvc *svc = qobject_cast<AppSettingsSvc*>( obj );
                    delete svc;
                }, Q_NULLPTR, ptr, false
            );
            g_ptr.store( Q_NULLPTR );
        }
    }
    g_locker.unlock();
}



}

#endif
