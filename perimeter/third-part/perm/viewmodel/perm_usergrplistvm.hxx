#ifndef PERM_USERGRPLISTVM_HXX
#define PERM_USERGRPLISTVM_HXX

#include "perm/common/perm_def.h"
#include "qxpack/indcom/afw/qxpack_ic_viewmodelbase.hxx"

namespace FcPerm {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief 用户组权限列表页面容器
 */
// ////////////////////////////////////////////////////////////////////////////
class PERM_API UserGrpListVm : public QxPack::IcViewModelBase
{
    Q_OBJECT
    Q_PROPERTY(QObject * editableUserGrpListInfo READ editableUserGrpListInfoObj CONSTANT)
    Q_PROPERTY(QJsonObject grpInfo READ grpInfo WRITE setGrpInfo)
public:
    Q_INVOKABLE explicit UserGrpListVm( const QVariantList & );
    Q_INVOKABLE virtual ~UserGrpListVm( ) Q_DECL_OVERRIDE;

    QObject * editableUserGrpListInfoObj( ) const;

    QJsonObject grpInfo( ) const;
    void  setGrpInfo( const QJsonObject & );

private:
    void *m_obj;
    Q_DISABLE_COPY( UserGrpListVm )
};

}

#endif // PERM_USERGRPLISTVM_HXX
