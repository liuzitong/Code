#ifndef PERM_USERGRPMGRVM_HXX
#define PERM_USERGRPMGRVM_HXX

#include "perm/common/perm_def.h"
#include "qxpack/indcom/afw/qxpack_ic_viewmodelbase.hxx"

namespace FcPerm {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief 用户组管理页面容器
 */
// ////////////////////////////////////////////////////////////////////////////
class PERM_API UserGrpMgrVm : public QxPack::IcViewModelBase
{
    Q_OBJECT
    Q_PROPERTY( QObject* userGrpListInfo          READ  userGrpListInfoObj          CONSTANT )
    Q_PROPERTY( QObject* specUserGrpPermListInfo  READ  specUserGrpPermListInfoObj  CONSTANT )
    Q_PROPERTY( bool     canCreateGroup    READ  canCreateGroup      CONSTANT )
    Q_PROPERTY( bool     canModifyGroup    READ  canModifyGroup      CONSTANT )
    Q_PROPERTY( bool     canRemoveGroup    READ  canRemoveGroup      CONSTANT )

public:
    Q_INVOKABLE explicit UserGrpMgrVm( const QVariantList & );
    Q_INVOKABLE virtual ~UserGrpMgrVm( ) Q_DECL_OVERRIDE;

    QObject* userGrpListInfoObj ( ) const;
    QObject* specUserGrpPermListInfoObj ( ) const;
    bool     canCreateGroup     ( ) const;
    bool     canModifyGroup     ( ) const;
    bool     canRemoveGroup     ( ) const;

    Q_INVOKABLE void activeGrpPermList( );
    Q_INVOKABLE void checkCreate( );
    Q_INVOKABLE void checkModify( );
    Q_INVOKABLE void checkRemove( );

    Q_SIGNAL    void showGrpPermList( );
    Q_SIGNAL    void showNewUserGrpView( );
    Q_SIGNAL    void showModUserGrpView( );

private:
    void *m_obj;
    Q_DISABLE_COPY( UserGrpMgrVm )
};

}

#endif // PERM_USERGRPMGRVM_HXX
