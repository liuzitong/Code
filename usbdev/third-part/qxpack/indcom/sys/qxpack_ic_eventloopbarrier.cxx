// ////////////////////////////////////////////////////////////////////////////
// @author nightwing
// @history
//        2019/04/27  checked.
// ////////////////////////////////////////////////////////////////////////////
#ifndef QXPACK_IC_EVENTLOOPBARRIER_CXX
#define QXPACK_IC_EVENTLOOPBARRIER_CXX

#include "qxpack_ic_eventloopbarrier.hxx"
#include <QObject>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
// Implement
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcEventLoopBarrier :: IcEventLoopBarrier ( Callback cb, void *ctxt )
                    : QObject( Q_NULLPTR )
{
    m_cb_func = cb; m_ctxt = ctxt; m_obj = Q_NULLPTR;
    QMetaObject::invokeMethod( this, "doCallback", Qt::QueuedConnection );
}

// ============================================================================
// dtor
// ============================================================================
IcEventLoopBarrier :: ~IcEventLoopBarrier ( )
{
}

// ============================================================================
// do callback
// ============================================================================
void  IcEventLoopBarrier :: doCallback()
{
    // do callback
    if ( m_cb_func != Q_NULLPTR ) {
        m_cb_func ( m_ctxt );
        this->deleteLater();
    }
}

// ============================================================================
// barrier [ static ]
// ============================================================================
void  IcEventLoopBarrier :: barrier( Callback cb, void *ctxt )
{
    IcEventLoopBarrier *b_obj = new IcEventLoopBarrier( cb, ctxt );
    Q_UNUSED( b_obj );
}

}
#endif
