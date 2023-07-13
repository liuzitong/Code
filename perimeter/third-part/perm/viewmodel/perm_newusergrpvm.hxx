#ifndef PERM_NEWUSERGRPVM_HXX
#define PERM_NEWUSERGRPVM_HXX

#include "perm/common/perm_def.h"
#include "qxpack/indcom/afw/qxpack_ic_viewmodelbase.hxx"

namespace FcPerm {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief 新增用户组页面容器
 */
// ////////////////////////////////////////////////////////////////////////////
class PERM_API NewUserGrpVm : public QxPack::IcViewModelBase
{
    Q_OBJECT
    Q_PROPERTY(QObject   * editUserGrpInfo READ editUserGrpInfoObj CONSTANT)
    Q_PROPERTY(QJsonObject grpInfo READ grpInfo WRITE setGrpInfo)
public:
    Q_INVOKABLE explicit NewUserGrpVm( const QVariantList & );
    Q_INVOKABLE virtual ~NewUserGrpVm( ) Q_DECL_OVERRIDE;

    QObject * editUserGrpInfoObj( ) const;

    QJsonObject grpInfo( ) const;
    void  setGrpInfo( const QJsonObject & );

    Q_INVOKABLE void editUserGrpPerm( );
    Q_INVOKABLE void save( );

    Q_SIGNAL void showUserGrpPermListView( );

private:
    void *m_obj;
    Q_DISABLE_COPY( NewUserGrpVm )
};

}

#endif // PERM_NEWUSERGRPVM_HXX
