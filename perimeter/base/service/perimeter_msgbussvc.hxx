#ifndef PERIMETER_MSGBUSSVC_HXX
#define PERIMETER_MSGBUSSVC_HXX

#include "perimeter/base/common/perimeter_def.h"
#include "qxpack/indcom/afw/qxpack_ic_msgbus.hxx"

namespace Perimeter {

// ////////////////////////////////////////////////////////////////////////////
//
//  message bus manager
//
// ////////////////////////////////////////////////////////////////////////////
class PERIMETER_API MsgBusSvc : public QxPack::IcMsgBus {
    Q_OBJECT
private:
    explicit MsgBusSvc( QObject *pa = Q_NULLPTR );
    virtual ~MsgBusSvc( );
public :
    static MsgBusSvc*  getInstance ( );
    static void        freeInstance( );
};

}

#endif
