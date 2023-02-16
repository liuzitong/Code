#ifndef QXPACK_IC_VIEWMODELBASE_HXX
#define QXPACK_IC_VIEWMODELBASE_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include <QObject>
#include <QJsonObject>

namespace QxPack {

// ////////////////////////////////////////////////////
/*!
 * @brief viewmodel base class
 */
// ////////////////////////////////////////////////////
class QXPACK_IC_API  IcViewModelBase : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY( IcViewModelBase )
public:
    //! ctor
    explicit IcViewModelBase ( QObject *pa = Q_NULLPTR );

    //! dtor
    virtual ~IcViewModelBase( ) Q_DECL_OVERRIDE;

private:
    void *m_obj;
};


}

#endif
