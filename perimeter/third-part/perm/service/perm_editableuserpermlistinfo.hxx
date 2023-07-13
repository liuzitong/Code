#ifndef PERM_EDITABLEUSERPERMLISTINFO_HXX
#define PERM_EDITABLEUSERPERMLISTINFO_HXX

#include "perm/common/perm_def.h"
#include "qxpack/indcom/afw/qxpack_ic_svcbase.hxx"
#include <QJsonArray>

namespace FcPerm {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief 新增编辑用户用户组权限列表展示
 */
// ////////////////////////////////////////////////////////////////////////////
class PERM_API EditableUserPermListInfo : public QxPack::IcSvcBase
{
    Q_OBJECT
    Q_PROPERTY( QJsonArray permList READ permList CONSTANT )
public:
    Q_INVOKABLE explicit EditableUserPermListInfo( );
    virtual ~EditableUserPermListInfo( ) Q_DECL_OVERRIDE;

    QJsonArray permList( ) const;

    void addPerm( const bool &, const bool &, const bool &, const QString & );

private:
    void *m_obj;
    Q_DISABLE_COPY( EditableUserPermListInfo )
};

}

#endif // PERM_EDITABLEUSERPERMLISTINFO_HXX
