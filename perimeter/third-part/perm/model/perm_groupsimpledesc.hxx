#ifndef PERM_GROUPSIMPLEDESC_HXX
#define PERM_GROUPSIMPLEDESC_HXX

#include "perm/common/perm_def.h"
#include <QObject>

namespace FcPerm {

// ////////////////////////////////////////////////////////////////////////////
//
// 用户组描述信息
//
// ////////////////////////////////////////////////////////////////////////////
class PERM_API GroupSimpleDesc
{
public:
    GroupSimpleDesc ( );
    GroupSimpleDesc ( const quint64 &, const QString & );
    GroupSimpleDesc ( const GroupSimpleDesc & );
    GroupSimpleDesc &  operator = ( const GroupSimpleDesc & );
    virtual ~GroupSimpleDesc( );

    quint64  gid   ( ) const;
    QString  name  ( ) const;

private:
    void *m_obj;
};

}

#endif // PERM_GROUPSIMPLEDESC_HXX
