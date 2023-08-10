#ifndef QXPACK_IC_EVENTLOOPBARRIER_HXX
#define QXPACK_IC_EVENTLOOPBARRIER_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include <QObject>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//! @brief event loop barrier
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcEventLoopBarrier : public QObject {
    Q_OBJECT
public:
    typedef void (* Callback )( void *ctxt );
    virtual ~IcEventLoopBarrier ( ) Q_DECL_OVERRIDE;

    //! static for do a event barrier
    static void barrier( Callback cb, void *ctxt );

protected:
    explicit IcEventLoopBarrier ( Callback cb, void *ctxt );
    Q_INVOKABLE void doCallback( );

private:
    Callback  m_cb_func;
    void *m_ctxt;
    void *m_obj;
    Q_DISABLE_COPY( IcEventLoopBarrier )
};


}

#endif
