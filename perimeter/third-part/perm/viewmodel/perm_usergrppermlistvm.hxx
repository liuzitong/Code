#ifndef PERM_USERGRPPERMLISTVM_HXX
#define PERM_USERGRPPERMLISTVM_HXX

#include "perm/common/perm_def.h"
#include "qxpack/indcom/afw/qxpack_ic_viewmodelbase.hxx"

namespace FcPerm {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief 用户组权限列表页面容器
 */
// ////////////////////////////////////////////////////////////////////////////
class PERM_API UserGrpPermListVm : public QxPack::IcViewModelBase
{
    Q_OBJECT
    Q_PROPERTY(QObject * editableUserGrpPermListInfo READ editableUserGrpPermListInfoObj CONSTANT)
    Q_PROPERTY(QJsonObject permInfoList READ permInfoList WRITE setPermInfoList)
public:
    Q_INVOKABLE explicit UserGrpPermListVm( const QVariantList & );
    Q_INVOKABLE virtual ~UserGrpPermListVm( ) Q_DECL_OVERRIDE;

    QObject * editableUserGrpPermListInfoObj( ) const;

    QJsonObject permInfoList( ) const;
    void setPermInfoList( const QJsonObject & );

private:
    void *m_obj;
    Q_DISABLE_COPY( UserGrpPermListVm )
};

}

#endif // PERM_USERGRPPERMLISTVM_HXX
