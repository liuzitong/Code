#ifndef QXPACK_IC_SYSEVTQTHREAD_PRIV_HXX
#define QXPACK_IC_SYSEVTQTHREAD_PRIV_HXX

#include <qxpack/indcom/common/qxpack_ic_def.h>
#include <QThread>
#include <QObject>

namespace QxPack {

// //////////////////////////////////////////////////
/*!
 * @brief sys module init checker
 */
// //////////////////////////////////////////////////
class  QXPACK_IC_HIDDEN  IcSysEvtQThreadPriv {

public:
    IcSysEvtQThreadPriv( );
    virtual ~IcSysEvtQThreadPriv( );
    QThread*   thread( ) const;

    //! check if thread alive
    static bool isAlive( );

private:
    void *m_obj;
    Q_DISABLE_COPY( IcSysEvtQThreadPriv )

};


}

#endif
