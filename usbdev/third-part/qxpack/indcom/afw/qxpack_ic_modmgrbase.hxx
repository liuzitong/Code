#ifndef QXPACK_IC_MODMGRBASE_HXX
#define QXPACK_IC_MODMGRBASE_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include "qxpack/indcom/afw/qxpack_ic_appctrlbase.hxx"
#include <QObject>

namespace QxPack {


// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief module manager ( per module )
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcModMgrBase : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY( IcModMgrBase )
public:
    //! ctor
    explicit IcModMgrBase ( QObject *pa = Q_NULLPTR );

    //! dtor
    virtual ~IcModMgrBase( ) Q_DECL_OVERRIDE;

    //! register types
    virtual void  registerTypes( IcAppCtrlBase* );

private:
    void *m_obj;
};

}

#endif
