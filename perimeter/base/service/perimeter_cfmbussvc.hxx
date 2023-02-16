#ifndef PERIMETER_CFMBUSSVC_HXX
#define PERIMETER_CFMBUSSVC_HXX

#include "perimeter/base/common/perimeter_def.h"
#include "qxpack/indcom/afw/qxpack_ic_confirmbus.hxx"

namespace Perimeter {

// ////////////////////////////////////////////////////////////////////////////
//
//  confirm bus manager
//
// ////////////////////////////////////////////////////////////////////////////
class PERIMETER_API CfmBusSvc : public QxPack::IcConfirmBus {
    Q_OBJECT
private:
    explicit CfmBusSvc( QObject *pa = Q_NULLPTR );
    virtual ~CfmBusSvc( );
public :
    static CfmBusSvc*  getInstance ( );
    static void        freeInstance( );
};

}

#endif
