#ifndef PERM_USERGROUPINFODATA_HXX
#define PERM_USERGROUPINFODATA_HXX

#include "perm/common/perm_def.h"
#include <QJsonArray>

namespace FcPerm {

// ////////////////////////////////////////////////////////////////////////////
//
// 用户组信息
//
// ////////////////////////////////////////////////////////////////////////////
class PERM_API UserGroupInfoData
{
public:
    UserGroupInfoData ( );
    UserGroupInfoData ( const QJsonArray & );
    UserGroupInfoData ( const QJsonObject & );
    UserGroupInfoData ( const UserGroupInfoData & );
    UserGroupInfoData &  operator = ( const UserGroupInfoData & );
    virtual ~UserGroupInfoData( );

    bool    isNull( ) const;

    quint64 GID ( ) const;
    QString name( ) const;
    QString memo( ) const;
    quint16 role( ) const;
    QStringList permList( ) const;

    void setGID ( const quint64 & );
    void setName( const QString & );
    void setMemo( const QString & );
    void setPermList( const QStringList & );

private:
    void *m_obj;
};

}

#endif // PERM_USERGROUPINFODATA_HXX
