#ifndef PERM_PERMMGRVM_HXX
#define PERM_PERMMGRVM_HXX

#include "perm/common/perm_def.h"
#include "qxpack/indcom/afw/qxpack_ic_viewmodelbase.hxx"

namespace FcPerm {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief 权限管理页面容器
 */
// ////////////////////////////////////////////////////////////////////////////
class PERM_API PermMgrVm : public QxPack::IcViewModelBase
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit PermMgrVm( const QVariantList & );
    Q_INVOKABLE virtual ~PermMgrVm( ) Q_DECL_OVERRIDE;

private:
    void *m_obj;
    Q_DISABLE_COPY( PermMgrVm )
};

}

#endif // PERM_PERMMGRVM_HXX
