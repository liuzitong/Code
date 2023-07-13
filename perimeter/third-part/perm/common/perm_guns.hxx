#ifndef PERM_GUNS_HXX
#define PERM_GUNS_HXX

#include <QString>

namespace FcPerm {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief global unique name stirng
 */
// ////////////////////////////////////////////////////////////////////////////
// perm mode
#define GUNS_PermMgrVm         QStringLiteral("FcPerm::PermMgrVm"        )
#define GUNS_UserGrpMgrVm      QStringLiteral("FcPerm::UserGrpMgrVm"     )
#define GUNS_UserGrpPermListVm QStringLiteral("FcPerm::UserGrpPermListVm")
#define GUNS_UserInfoChgVm     QStringLiteral("FcPerm::UserInfoChgVm"    )
#define GUNS_UserLoginVm       QStringLiteral("FcPerm::UserLoginVm"      )
#define GUNS_UserMgrVm         QStringLiteral("FcPerm::UserMgrVm"        )
#define GUNS_UserPermListVm    QStringLiteral("FcPerm::UserPermListVm"   )
#define GUNS_NewUserVm         QStringLiteral("FcPerm::NewUserVm"        )
#define GUNS_UserGrpListVm     QStringLiteral("FcPerm::UserGrpListVm"    )
#define GUNS_NewUserGrpVm      QStringLiteral("FcPerm::NewUserGrpVm"     )
#define GUNS_ModUserVm         QStringLiteral("FcPerm::ModUserVm"        )
#define GUNS_ModUserGrpVm      QStringLiteral("FcPerm::ModUserGrpVm"     )

#define GUNS_DefaultMethodSvcCfmGroup QStringLiteral("FccPerm::DefaultMethodSvcCfmGroup")
#define GUNS_DefaultMethodSvcMsgGroup QStringLiteral("FccPerm::DefaultMethodSvcMsgGroup")
#define GUNS_LocalPwdMgrSvcMsgGroup   QStringLiteral("FccPerm::LocalPwdMgrSvcMsgGroup")

// add other unique name here...

}

#endif 
