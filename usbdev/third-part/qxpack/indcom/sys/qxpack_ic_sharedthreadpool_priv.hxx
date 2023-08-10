#ifndef QXPACK_IC_SHAREDTHREADPOOL_PRIV_HXX
#define QXPACK_IC_SHAREDTHREADPOOL_PRIV_HXX

#include <qxpack/indcom/common/qxpack_ic_def.h>
#include <QObject>
#include <QThreadPool>

namespace QxPack {

// /////////////////////////////////////////////////////
/*!
 * @brief this class provide shared QThreadPool
 */
// /////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN IcSharedThreadPoolPriv {
public:
    explicit IcSharedThreadPoolPriv ( );
    virtual ~IcSharedThreadPoolPriv( );
    QThreadPool *  threadPool( ) const;
private:
    void *m_obj;
    Q_DISABLE_COPY( IcSharedThreadPoolPriv )
};


}

#endif
