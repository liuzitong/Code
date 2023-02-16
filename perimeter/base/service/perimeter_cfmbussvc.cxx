#ifndef PERIMETER_CFMBUSSVC_CXX
#define PERIMETER_CFMBUSSVC_CXX

#include "perimeter_cfmbussvc.hxx"
#include "perimeter/base/common/perimeter_memcntr.hxx"
#include <QMutex>
#include <QAtomicPointer>
#include <QAtomicInt>

#include "qxpack/indcom/sys/qxpack_ic_rmtobjdeletor_priv.hxx"

namespace Perimeter {

// ////////////////////////////////////////////////////////////////////////////
// cfmbussvc wrapper
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
CfmBusSvc :: CfmBusSvc ( QObject *pa ) : QxPack::IcConfirmBus( pa ){ }

// ============================================================================
// dtor
// ============================================================================
CfmBusSvc :: ~CfmBusSvc ( ) { }


// ////////////////////////////////////////////////////////////////////////////
// static factory
// ////////////////////////////////////////////////////////////////////////////

static QAtomicPointer<CfmBusSvc>  g_ptr( Q_NULLPTR );
static QMutex       g_locker;
static QAtomicInt   g_ref_cntr(0);

// ============================================================================
// get new instance
// ============================================================================
CfmBusSvc *  CfmBusSvc::getInstance( )
{
    CfmBusSvc *ptr = Q_NULLPTR;

    g_locker.lock();
    ptr = g_ptr.loadAcquire();
    if ( ptr == Q_NULLPTR ) {
        g_ptr.store( ( ptr = new CfmBusSvc( Q_NULLPTR ) ));
        g_ref_cntr.store(1);
    } else {
        g_ref_cntr.fetchAndAddOrdered(1);
    }
    g_locker.unlock();
    return ptr;
}

// ============================================================================
// free the instance
// ============================================================================
void  CfmBusSvc::freeInstance()
{
    CfmBusSvc *ptr = Q_NULLPTR;

    g_locker.lock();
    ptr = g_ptr.loadAcquire();
    if ( ptr != Q_NULLPTR ) {
        if ( g_ref_cntr.fetchAndSubOrdered(1) - 1 == 0 ) {     
            QxPack::IcRmtObjDeletor::deleteObjInThread(
                ptr->thread(),
                [](void*, QObject* obj ) {
                    CfmBusSvc *svc = qobject_cast<CfmBusSvc*>( obj );
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
