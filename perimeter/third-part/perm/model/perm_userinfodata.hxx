#ifndef PERM_USERINFODATA_HXX
#define PERM_USERINFODATA_HXX

#include "perm/common/perm_def.h"
#include <QJsonArray>
#include <QMap>

namespace FcPerm {

// ////////////////////////////////////////////////////////////////////////////
//
// 用户信息
//
// ////////////////////////////////////////////////////////////////////////////
class PERM_API UserInfoData
{
public:
    UserInfoData ( );
    UserInfoData ( const QJsonArray & );
    UserInfoData ( const QJsonObject & );
    UserInfoData ( const UserInfoData & );
    UserInfoData &  operator = ( const UserInfoData & );
    virtual ~UserInfoData( );

    bool isNull ( ) const;

    quint64 UID ( ) const;
    QString name( ) const;
    QString pwd ( ) const;
    QString memo( ) const;
    quint16 role( ) const;
    QMap<QString, bool> privPermList( ) const;
    QStringList groupPermList ( ) const;
    QStringList actualPermList( ) const;
    QMap<quint64, QString> groupList( ) const;

    void setUID ( const quint64 & );
    void setName( const QString & );
    void setPwd ( const QString & );
    void setMemo( const QString & );
    void setPrivPermList  ( const QMap<QString, bool> & );
    void setGroupPermList ( const QStringList & );
    void setActualPermList( const QStringList & );
    void setGroupList( const QMap<quint64, QString> & );

private:
    void *m_obj;
};

}

#endif // PERM_USERINFODATA_HXX
