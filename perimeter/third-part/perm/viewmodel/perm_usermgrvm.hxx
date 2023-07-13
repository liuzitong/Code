#ifndef PERM_USERMGRVM_HXX
#define PERM_USERMGRVM_HXX

#include "perm/common/perm_def.h"
#include "qxpack/indcom/afw/qxpack_ic_viewmodelbase.hxx"

namespace FcPerm {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief 用户管理页面容器
 */
// ////////////////////////////////////////////////////////////////////////////
class PERM_API UserMgrVm : public QxPack::IcViewModelBase
{
    Q_OBJECT
    Q_PROPERTY( QObject* userListInfo         READ  userListInfoObj           CONSTANT )
    Q_PROPERTY( QObject* specUserGrpListInfo  READ  specUserGrpListInfoObj    CONSTANT )
    Q_PROPERTY( QObject* specUserPermListInfo READ  specUserPermListInfoObj   CONSTANT )
    Q_PROPERTY( QObject* localUser            READ  localUserObj              CONSTANT)
    Q_PROPERTY( bool     canAutoLoginUser READ  canAutoLoginUser   CONSTANT )
    Q_PROPERTY( bool     canCreateUser READ  canCreateUser   CONSTANT )
    Q_PROPERTY( bool     canModifyUser READ  canModifyUser   CONSTANT )
    Q_PROPERTY( bool     canRemoveUser READ  canRemoveUser   CONSTANT )
public:
    Q_INVOKABLE explicit UserMgrVm( const QVariantList & );
    Q_INVOKABLE virtual ~UserMgrVm( ) Q_DECL_OVERRIDE;

    QObject*  userListInfoObj ( ) const;
    bool      canAutoLoginUser( ) const;
    bool      canCreateUser( ) const;
    bool      canModifyUser( ) const;
    bool      canRemoveUser( ) const;
    QObject*  specUserGrpListInfoObj( ) const;
    QObject*  specUserPermListInfoObj( ) const;
    QObject*  localUserObj( ) const;

    Q_INVOKABLE void setAutoLogin( );
    Q_INVOKABLE void cancelAutoLogin( );
    Q_INVOKABLE void activeGroupList( );
    Q_INVOKABLE void activePermList( );
    Q_INVOKABLE void checkCreate( );
    Q_INVOKABLE void checkModify( );
    Q_INVOKABLE void checkRemove( );

    Q_SIGNAL    void showNewUserView( );
    Q_SIGNAL    void showModUserView( );
    Q_SIGNAL    void showGroupList( );
    Q_SIGNAL    void showPermList( );

private:
    void *m_obj;
    Q_DISABLE_COPY( UserMgrVm )
};

}

#endif // PERM_USERMGRVM_HXX
