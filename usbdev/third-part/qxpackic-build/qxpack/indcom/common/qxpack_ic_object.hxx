#ifndef QXPACK_IC_OBJECT_HXX
#define QXPACK_IC_OBJECT_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include <cstdint>

namespace QxPack {

class IcEvent;

// ////////////////////////////////////////////////////////////////////////////
//
//                 The Object Used for Event System
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcObject {
public:
    //! default ctor
    explicit IcObject( void * = nullptr );

    //! dtor
    virtual ~IcObject( );

protected:
    //! the event filter functions
    virtual bool  event( IcEvent* );

private:
    void *m_obj;
    QXPACK_IC_DISABLE_COPY( IcObject )
};


}
#endif
