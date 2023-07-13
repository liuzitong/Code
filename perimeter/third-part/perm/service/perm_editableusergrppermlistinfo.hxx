#ifndef PERM_EDITABLEUSERGRPPERMLISTINFO_HXX
#define PERM_EDITABLEUSERGRPPERMLISTINFO_HXX

#include "perm/common/perm_def.h"
#include "qxpack/indcom/afw/qxpack_ic_svcbase.hxx"
#include <QJsonArray>

namespace FcPerm {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief 新增编辑用户组权限列表展示
 */
// ////////////////////////////////////////////////////////////////////////////
class PERM_API EditableUserGrpPermListInfo : public QxPack::IcSvcBase
{
    Q_OBJECT
    Q_PROPERTY( QJsonArray permInfoList READ permInfoList CONSTANT )
public:
    Q_INVOKABLE explicit EditableUserGrpPermListInfo( );
    virtual ~EditableUserGrpPermListInfo( ) Q_DECL_OVERRIDE;

    QJsonArray permInfoList( ) const;

    void addPerm( const bool &, const QString & );

private:
    void *m_obj;
    Q_DISABLE_COPY( EditableUserGrpPermListInfo )
};

}

#endif // PERM_EDITABLEUSERGRPPERMLISTINFO_HXX
