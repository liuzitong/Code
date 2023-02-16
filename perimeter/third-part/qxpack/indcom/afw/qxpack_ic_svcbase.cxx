// ============================================================================
// author: night wing
// date stamp: 20190523 1649
// ============================================================================
#ifndef QXPACK_IC_SVCBASE_CXX
#define QXPACK_IC_SVCBASE_CXX

#include "qxpack_ic_svcbase.hxx"
#include <QThread>
#include <QMetaObject>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
// IcSvcBase API
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcSvcBase :: IcSvcBase ( QObject *pa ) : QObject( pa )
{
    m_obj = Q_NULLPTR;
}

// ============================================================================
// dtor
// ============================================================================
IcSvcBase :: ~IcSvcBase ( )
{ }

// ============================================================================
// call method of the service
// ============================================================================
bool  IcSvcBase :: call (
    const char *method_name,
    QGenericReturnArgument ret,
    QGenericArgument val0, QGenericArgument val1, QGenericArgument val2, QGenericArgument val3,
    QGenericArgument val4, QGenericArgument val5, QGenericArgument val6, QGenericArgument val7
)
{
    if ( method_name == Q_NULLPTR ) { return false; }

    // -------------------------------------------------
    // NOTE: the sub-class QMetaObject is used, not the IcSvcBase QMetaObject.
    // -------------------------------------------------
    return QMetaObject::invokeMethod (
        this, method_name,
        ( this->thread() != QThread::currentThread() ? Qt::BlockingQueuedConnection : Qt::DirectConnection ),
        ret, val0, val1, val2, val3, val4, val5, val6, val7
    );
}

// ============================================================================
// call method of the service without return argument
// ============================================================================
bool  IcSvcBase :: call (
    const char *method_name,
    QGenericArgument val0, QGenericArgument val1, QGenericArgument val2, QGenericArgument val3,
    QGenericArgument val4, QGenericArgument val5, QGenericArgument val6, QGenericArgument val7
)
{
    if ( method_name == Q_NULLPTR ) { return false; }
    return QMetaObject::invokeMethod(
        this, method_name,
        ( this->thread() != QThread::currentThread() ? Qt::BlockingQueuedConnection : Qt::DirectConnection ),
        val0, val1, val2, val3, val4, val5, val6, val7
    );

}


}

#endif
