#ifndef QXPACK_IC_SHAREDTHREADPOOL_PRIV_CXX
#define QXPACK_IC_SHAREDTHREADPOOL_PRIV_CXX

#include "qxpack_ic_sharedthreadpool_priv.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/sys/qxpack_ic_sysevtqthread_priv.hxx"
#include <QMutex>
#include <QDebug>

#define MAX_THREAD_NUM 10

namespace QxPack {

// /////////////////////////////////////////////////////////////
// global instance
// /////////////////////////////////////////////////////////////
static QMutex       g_locker;
static QAtomicInt   g_ref_cntr(0);
static QThreadPool *g_obj_ptr = Q_NULLPTR;

// ============================================================
// get the instance
// ============================================================
static QThreadPool *   getInstance( )
{
    // ----------------------------------------------------------
    // according to QT5, the QThreadPool all functions are thread-safe
    // ----------------------------------------------------------
    g_locker.lock();
    if ( g_obj_ptr != Q_NULLPTR ) {
        g_ref_cntr.fetchAndAddOrdered(1);
    } else {
        g_obj_ptr = new QThreadPool;
        IcSysEvtQThreadPriv *sys_evt = new IcSysEvtQThreadPriv;
        QObject::connect(
            g_obj_ptr, & QThreadPool::destroyed, [sys_evt](QObject*){ delete sys_evt; }
        );
        if ( g_obj_ptr->thread() != sys_evt->thread() ) {
            g_obj_ptr->moveToThread( sys_evt->thread() );
        }
        g_obj_ptr->setMaxThreadCount(MAX_THREAD_NUM);
        g_obj_ptr->setExpiryTimeout (5000);
        g_ref_cntr.storeRelease(1);
    }
    g_locker.unlock();
    return g_obj_ptr;
}

// ============================================================
// free the instance
// ============================================================
static bool    freeInstace( )
{
    QThreadPool *t = Q_NULLPTR;

    g_locker.lock();
    if ( g_obj_ptr != Q_NULLPTR ) {
        if ( g_ref_cntr.fetchAndSubOrdered(1) - 1 == 0 ) {
            t = g_obj_ptr; g_obj_ptr = Q_NULLPTR;
        }
    }
    g_locker.unlock();

    if ( t != Q_NULLPTR ) {
        t->deleteLater();
    }

    return ( t != Q_NULLPTR );
}

// /////////////////////////////////////////////////////////////
// shared pool
// /////////////////////////////////////////////////////////////
// =============================================================
// CTOR
// =============================================================
    IcSharedThreadPoolPriv :: IcSharedThreadPoolPriv ( )
{
    m_obj = getInstance();
}

// =============================================================
// DTOR
// =============================================================
    IcSharedThreadPoolPriv :: ~IcSharedThreadPoolPriv ( )
{
    if ( m_obj != Q_NULLPTR ) { freeInstace(); }
}

// =============================================================
// return the global thread
// =============================================================
QThreadPool *   IcSharedThreadPoolPriv :: threadPool() const
{   return T_ObjCast( QThreadPool *, m_obj ); }


}

#endif
