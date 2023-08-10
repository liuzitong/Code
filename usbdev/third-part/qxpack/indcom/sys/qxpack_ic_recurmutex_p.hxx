#ifndef QXPACK_IC_RECURMUTEX_P_HXX
#define QXPACK_IC_RECURMUTEX_P_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include <QMutex>

#ifdef QXIC_QT6
#include <QRecursiveMutex>
#endif

namespace QxPack {

#ifdef QXIC_QT6
typedef QRecursiveMutex  IcRecurMutex;
#else
class QXPACK_IC_HIDDEN  IcRecurMutex : public QMutex {
public :
    IcRecurMutex( ) : QMutex( QMutex::Recursive ) {  }
};
#endif

}
#endif
