#ifndef QXPACK_IC_QUICKQTLOGO_HXX
#define QXPACK_IC_QUICKQTLOGO_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include "qxpack/indcom/ui_qml_base/qxpack_ic_quickfborenderbase.hxx"

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief the quick item
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcQuickQtLogoItem : public IcQuickFboRenderBase {
    Q_OBJECT
public:
    //! ctor
    IcQuickQtLogoItem ( QQuickItem *pa = Q_NULLPTR );

    //! dtor
    virtual ~IcQuickQtLogoItem( ) Q_DECL_OVERRIDE;

    static void  _reg();

private:
    void *m_obj;
    Q_DISABLE_COPY( IcQuickQtLogoItem )
};



}

#endif
