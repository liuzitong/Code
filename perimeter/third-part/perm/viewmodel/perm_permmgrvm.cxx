#ifndef PERM_PERMMGRVM_CXX
#define PERM_PERMMGRVM_CXX

#include "perm_permmgrvm.hxx"
#include "perm/common/perm_memcntr.hxx"

namespace FcPerm {

#define T_PrivPtr( o )  perm_objcast( PermMgrVmPriv*, o )
class PERM_HIDDEN PermMgrVmPriv
{
private:
    PermMgrVm   * m_parent;

public:
    explicit PermMgrVmPriv ( PermMgrVm *pa );
    ~PermMgrVmPriv( );
};

PermMgrVmPriv::PermMgrVmPriv(PermMgrVm *pa)
{
    m_parent = pa;
}

PermMgrVmPriv::~PermMgrVmPriv()
{
}

PermMgrVm::PermMgrVm(const QVariantList &)
{
    m_obj = perm_new( PermMgrVmPriv, this );
}

PermMgrVm::~PermMgrVm()
{
    perm_delete( m_obj, PermMgrVmPriv );
}

}

#endif // PERM_PERMMGRVM_CXX
