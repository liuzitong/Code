#ifndef PERM_USERLISTINFO_HXX
#define PERM_USERLISTINFO_HXX

#include "perm/common/perm_def.h"
#include <QAbstractListModel>

namespace FcPerm {

class UserInfoData;
// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief 用户列表信息
 */
// ////////////////////////////////////////////////////////////////////////////
class PERM_API UserListInfo : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int selectIndex READ selectIndex WRITE setSelectIndex NOTIFY selectIndexChanged)
public:
    static UserListInfo * getInstance( );
    static void release( );

    int  selectIndex( ) const;
    void setSelectIndex( const int & );
    Q_SIGNAL void selectIndexChanged( );

    UserInfoData  currentUserInfo( ) const;
    void addItem( const UserInfoData & );
    void modItem( const UserInfoData & );
    void rmvItem( );
    void empItem( );

private:
    Q_INVOKABLE explicit UserListInfo( );
    virtual ~UserListInfo( ) Q_DECL_OVERRIDE;

    // ========================================================================
    // override functions
    // ========================================================================
    //! the total records number
    virtual int        rowCount( const QModelIndex &)  const Q_DECL_OVERRIDE;

    //! return the data object by index and role
    virtual QVariant   data    ( const QModelIndex &idx, int role ) const Q_DECL_OVERRIDE;

    //! return all role id ( and names ) supported by this model
    //! @note we used roles like "id","name","dob", so view can use name to access it.
    virtual QHash<int,QByteArray>  roleNames( ) const Q_DECL_OVERRIDE;

    //! directly set data object at spec. index
    virtual bool       setData ( const QModelIndex &, const QVariant &, int ) Q_DECL_OVERRIDE;

    //! return flags ( characteristics )
    virtual Qt::ItemFlags  flags ( const QModelIndex & ) const Q_DECL_OVERRIDE;

private:
    friend class UserListInfoPriv;
    void *m_obj;
    Q_DISABLE_COPY( UserListInfo )
};

}

#endif // PERM_USERLISTINFO_HXX
