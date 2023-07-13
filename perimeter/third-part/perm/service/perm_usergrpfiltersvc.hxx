#ifndef PERM_USERGRPFILTERSVC_HXX
#define PERM_USERGRPFILTERSVC_HXX

#include "perm/common/perm_def.h"
#include "qxpack/indcom/afw/qxpack_ic_svcbase.hxx"

namespace FcPerm {

class EditableUserGrpListInfo;
class EditableUserInfo;
// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief 用户组过滤
 */
// ////////////////////////////////////////////////////////////////////////////
class PERM_API UserGrpFilterSvc : public QxPack::IcSvcBase
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit UserGrpFilterSvc( );
    virtual ~UserGrpFilterSvc( ) Q_DECL_OVERRIDE;

    void filter( EditableUserGrpListInfo &, const EditableUserInfo & );

private:
    void *m_obj;
    Q_DISABLE_COPY( UserGrpFilterSvc )
};

}

#endif // PERM_USERGRPFILTERSVC_HXX
