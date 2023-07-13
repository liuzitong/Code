#ifndef PERM_USERPERMFILTERSVC_HXX
#define PERM_USERPERMFILTERSVC_HXX

#include "perm/common/perm_def.h"
#include "qxpack/indcom/afw/qxpack_ic_svcbase.hxx"

namespace FcPerm {

class EditableUserPermListInfo;
class EditableUserInfo;
// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief 用户权限过滤
 */
// ////////////////////////////////////////////////////////////////////////////
class PERM_API UserPermFilterSvc : public QxPack::IcSvcBase
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit UserPermFilterSvc( );
    virtual ~UserPermFilterSvc( ) Q_DECL_OVERRIDE;

    void filter( EditableUserPermListInfo &, const EditableUserInfo & );

private:
    void *m_obj;
    Q_DISABLE_COPY( UserPermFilterSvc )
};

}

#endif // PERM_USERPERMFILTERSVC_HXX
