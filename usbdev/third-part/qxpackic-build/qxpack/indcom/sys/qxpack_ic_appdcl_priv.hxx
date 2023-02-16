#ifndef QXPACK_IC_APPDCL_PRIV_HXX
#define QXPACK_IC_APPDCL_PRIV_HXX

#include <QObject>
#include <QCoreApplication>
#include "qxpack/indcom/sys/qxpack_ic_eventloopbarrier.hxx"

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//! @brief the application event loop barrier
//! @details At the application end, call this to force done all deleteLater() \n
//!     that after QCoreApplication::exec().
// ////////////////////////////////////////////////////////////////////////////
class   IcAppDclPriv {
public  :  virtual ~IcAppDclPriv ( ) { }
private :  explicit IcAppDclPriv ( ) { }
public :
    /*!
     * @brief do eventloop
     * @param deep [in] how many times call QCoreAppliation::exec()
     */
    static void barrier( int deep = 6 )
    {
        for ( int x = 0; x < deep; x++ ) {
            QxPack::IcEventLoopBarrier::barrier (
                [](void*) { QCoreApplication::quit(); }, Q_NULLPTR
            );
            QCoreApplication::instance()->exec();
        }
    }
};

}

#endif
