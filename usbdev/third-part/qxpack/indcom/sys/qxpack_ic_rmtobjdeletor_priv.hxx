#ifndef QXPACK_IC_RMTOBJDELETOR_PRIV_HXX
#define QXPACK_IC_RMTOBJDELETOR_PRIV_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include <QObject>
#include <QThread>

namespace QxPack {


// ////////////////////////////////////////////////////
/*!
 * this object used to delete object in a thread
 */
// ////////////////////////////////////////////////////
class QXPACK_IC_API IcRmtObjDeletor {
public:
    typedef void (* DeleteFunc )( void* ctxt, QObject *obj );
    static bool  deleteObjInThread( QThread *t, DeleteFunc f, void *ctxt, QObject *obj, bool req_evt_loop = false );
protected:
    IcRmtObjDeletor ( );
    virtual ~IcRmtObjDeletor( );
private:

};



}
#endif
