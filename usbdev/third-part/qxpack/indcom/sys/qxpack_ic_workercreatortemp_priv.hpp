#ifndef QXPACK_IC_WORKERCREATORTEMP_PRIV_HPP
#define QXPACK_IC_WORKERCREATORTEMP_PRIV_HPP

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include <QObject>

namespace QxPack {

template <typename W >
class  IcWorkerCreatorTemp {
protected: W  *m_wkr;
public :
    IcWorkerCreatorTemp( ) { m_wkr = Q_NULLPTR; }
    virtual ~IcWorkerCreatorTemp ( ) { this->deleteWorker(); }
    inline W *  worker_di() { return m_wkr; }
    void  deleteWorker( )
    {
        if ( m_wkr != Q_NULLPTR ) {
            //m_wkr->disconnect( );
            //QObject::disconnect( m_wkr, Q_NULLPTR, Q_NULLPTR, Q_NULLPTR );
            m_wkr->deleteLater( );
        }
        m_wkr = Q_NULLPTR;
    }
};


}

#endif
