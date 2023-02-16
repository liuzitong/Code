#ifndef QXPACK_IC_QSGRENDERWORKER_CXX
#define QXPACK_IC_QSGRENDERWORKER_CXX

#include "qxpack_ic_qsgrenderworker.hxx"

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
// implement
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcQSGRenderWorker :: IcQSGRenderWorker( void* ) { }

// ============================================================================
// dtor
// ============================================================================
IcQSGRenderWorker :: ~IcQSGRenderWorker ( ) { }

// ============================================================================
// initialize
// ============================================================================
bool IcQSGRenderWorker :: initialize() { return true; }

// ============================================================================
// render work
// ============================================================================
void  IcQSGRenderWorker :: render() { }

// ============================================================================
// check if need call update()
// ============================================================================
bool  IcQSGRenderWorker :: isReqUpdateAfterRender() { return false; }

// ============================================================================
// This function will be called on the render thread while the GUI thread is blocked
// ============================================================================
void  IcQSGRenderWorker :: synchronize( QQuickFramebufferObject * )
{  }

}

#endif
