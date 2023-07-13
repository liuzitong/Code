#ifndef PERM_MODUSERGRPVM_HXX
#define PERM_MODUSERGRPVM_HXX

#include "perm/common/perm_def.h"
#include "qxpack/indcom/afw/qxpack_ic_viewmodelbase.hxx"

namespace FcPerm {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief 修改用户组页面容器
 */
// ////////////////////////////////////////////////////////////////////////////
class PERM_API ModUserGrpVm : public QxPack::IcViewModelBase
{
    Q_OBJECT
    Q_PROPERTY(QObject   * editUserGrpInfo READ  editUserGrpInfoObj CONSTANT)
    Q_PROPERTY(QJsonObject grpInfo READ grpInfo WRITE setGrpInfo)
public:
    Q_INVOKABLE explicit ModUserGrpVm( const QVariantList & );
    Q_INVOKABLE virtual ~ModUserGrpVm( ) Q_DECL_OVERRIDE;

    QObject * editUserGrpInfoObj( ) const;

    QJsonObject grpInfo( ) const;
    void  setGrpInfo( const QJsonObject & );

    Q_INVOKABLE void editUserGrpPerm( );
    Q_INVOKABLE void save( );

    Q_SIGNAL void showUserGrpPermListView( );

private:
    void *m_obj;
    Q_DISABLE_COPY( ModUserGrpVm )
};

}

#endif // PERM_MODUSERGRPVM_HXX
