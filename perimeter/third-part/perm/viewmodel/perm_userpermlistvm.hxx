#ifndef PERM_USERPERMLISTVM_HXX
#define PERM_USERPERMLISTVM_HXX

#include "perm/common/perm_def.h"
#include "qxpack/indcom/afw/qxpack_ic_viewmodelbase.hxx"

namespace FcPerm {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief 用户权限列表页面容器
 */
// ////////////////////////////////////////////////////////////////////////////
class PERM_API UserPermListVm : public QxPack::IcViewModelBase
{
    Q_OBJECT
    Q_PROPERTY(QObject * editableUserPermListInfo READ editableUserPermListInfoObj CONSTANT)
    Q_PROPERTY(QJsonObject permInfo READ permInfo WRITE setPermInfo)
public:
    Q_INVOKABLE explicit UserPermListVm( const QVariantList & );
    Q_INVOKABLE virtual ~UserPermListVm( ) Q_DECL_OVERRIDE;

    QObject * editableUserPermListInfoObj( ) const;

    QJsonObject permInfo( ) const;
    void  setPermInfo( const QJsonObject & );

private:
    void *m_obj;
    Q_DISABLE_COPY( UserPermListVm )
};

}

#endif // PERM_USERPERMLISTVM_HXX
