#ifndef QXPACK_IC_RMTOBJSIGBLOCKER_HXX
#define QXPACK_IC_RMTOBJSIGBLOCKER_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include <QObject>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * this object used to do block object signals that in remote thread
 * NOTE: due to document of the QObject::blockSignals() is not say it is thread-safe
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcRmtObjSigBlocker {
public:
    static void  blockSignals( QObject *obj, bool sw, bool req_wait_evt = false );
protected:
    IcRmtObjSigBlocker( );
    virtual ~IcRmtObjSigBlocker( );
private:
    QXPACK_IC_DISABLE_COPY( IcRmtObjSigBlocker )
};


}

#endif
