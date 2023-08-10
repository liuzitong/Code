#ifndef QXPACK_IC_RMTOBJCREATOR_PRIV_HXX
#define QXPACK_IC_RMTOBJCREATOR_PRIV_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include <QObject>
#include <QThread>

namespace QxPack {

// ////////////////////////////////////////////////////
/*!
 * this object used to create object in a thread, and return it's pointer
 */
// ////////////////////////////////////////////////////
class QXPACK_IC_API  IcRmtObjCreator {
public:
    typedef QObject* (* CreateFunc )( void* ctxt );

    static QObject*  createObjInThread( QThread *t, CreateFunc f, void *ctxt, bool req_evt_loop = false );

protected:
    IcRmtObjCreator( );
    virtual ~IcRmtObjCreator( );
private:
    QXPACK_IC_DISABLE_COPY( IcRmtObjCreator )
};



}
#endif
