#ifndef PERM_USERGRPPERMFILTERSVC_HXX
#define PERM_USERGRPPERMFILTERSVC_HXX

#include "perm/common/perm_def.h"
#include "qxpack/indcom/afw/qxpack_ic_svcbase.hxx"

namespace FcPerm {

class EditableUserGrpPermListInfo;
class EditableUserGrpInfo;
// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief 用户组过滤
 */
// ////////////////////////////////////////////////////////////////////////////
class PERM_API UserGrpPermFilterSvc : public QxPack::IcSvcBase
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit UserGrpPermFilterSvc( );
    virtual ~UserGrpPermFilterSvc( ) Q_DECL_OVERRIDE;

    void filter( EditableUserGrpPermListInfo &, const EditableUserGrpInfo & );

private:
    void *m_obj;
    Q_DISABLE_COPY( UserGrpPermFilterSvc )
};

}

#endif // PERM_USERGRPPERMFILTERSVC_HXX
