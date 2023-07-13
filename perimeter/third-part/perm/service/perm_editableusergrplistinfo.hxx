#ifndef PERM_EDITABLEUSERGRPLISTINFO_HXX
#define PERM_EDITABLEUSERGRPLISTINFO_HXX

#include "perm/common/perm_def.h"
#include "qxpack/indcom/afw/qxpack_ic_svcbase.hxx"
#include <QJsonArray>

namespace FcPerm {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief 新增编辑用户用户列表展示
 */
// ////////////////////////////////////////////////////////////////////////////
class PERM_API EditableUserGrpListInfo : public QxPack::IcSvcBase
{
    Q_OBJECT
    Q_PROPERTY( QJsonArray grpList READ grpList CONSTANT )
public:
    Q_INVOKABLE explicit EditableUserGrpListInfo( );
    virtual ~EditableUserGrpListInfo( ) Q_DECL_OVERRIDE;

    QJsonArray grpList( ) const;

    void addGrp( const bool &, const QString &, const QString & );

private:
    void *m_obj;
    Q_DISABLE_COPY( EditableUserGrpListInfo )
};

}

#endif // PERM_EDITABLEUSERGRPLISTINFO_HXX
