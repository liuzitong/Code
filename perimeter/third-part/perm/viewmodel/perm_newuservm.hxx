#ifndef PERM_NEWUSERVM_HXX
#define PERM_NEWUSERVM_HXX

#include "perm/common/perm_def.h"
#include "qxpack/indcom/afw/qxpack_ic_viewmodelbase.hxx"

namespace FcPerm {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief 新增用户页面容器
 */
// ////////////////////////////////////////////////////////////////////////////
class PERM_API NewUserVm : public QxPack::IcViewModelBase
{
    Q_OBJECT
    Q_PROPERTY(QObject   * editUserInfo READ  editUserInfoObj CONSTANT )
    Q_PROPERTY(QJsonObject userInfo READ userInfo WRITE setUserInfo)
public:
    Q_INVOKABLE explicit NewUserVm( const QVariantList & );
    Q_INVOKABLE virtual ~NewUserVm( ) Q_DECL_OVERRIDE;

    QObject * editUserInfoObj( ) const;

    QJsonObject userInfo( ) const;
    void  setUserInfo( const QJsonObject & );

    Q_INVOKABLE void editUserGroup( );
    Q_INVOKABLE void editUserPerm( );
    Q_INVOKABLE void save( );

    Q_SIGNAL void showUserGrpListView( );
    Q_SIGNAL void showUserPermListView( );

private:
    void *m_obj;
    Q_DISABLE_COPY( NewUserVm )
};

}

#endif // PERM_NEWUSERVM_HXX
