#ifndef WAFC_USERANDPERMINFO_HXX
#define WAFC_USERANDPERMINFO_HXX

#include "perm/common/perm_def.h"
#include "perm/model/perm_typedef.hxx"
#include <QObject>

namespace FcPerm {

// ////////////////////////////////////////////////////////////////////////////
//
// 用户信息 & 权限
//
// ////////////////////////////////////////////////////////////////////////////
class PERM_API UserAndPermInfo : public QObject
{
    Q_OBJECT
public:
    explicit UserAndPermInfo();
    virtual ~UserAndPermInfo( ) Q_DECL_OVERRIDE;

    quint64 UID       ( ) const;
    QString accToken  ( ) const;
    QString queryToken( ) const;
    quint16 role      ( ) const;
    ActualPermList actualPermList( ) const;
    AuthPermList   authPermList  ( ) const;
    AuthGroupList  authGroupList ( ) const;

    void setUID       ( const quint64 & );
    void setAccToken  ( const QString & );
    void setQueryToken( const QString & );
    void setActualPermList( const ActualPermList & );
    void setAuthPermList  ( const AuthPermList & );
    void setAuthGroupList ( const AuthGroupList & );

    void clear( );

    static UserAndPermInfo& globalInstance();
private:
    void *m_obj;
};

}

#endif // WAFC_USERANDPERMINFO_HXX
