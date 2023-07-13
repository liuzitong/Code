#ifndef PERM_USERGRPLISTINFO_HXX
#define PERM_USERGRPLISTINFO_HXX

#include "perm/common/perm_def.h"
#include <QAbstractListModel>

namespace FcPerm {

class UserGroupInfoData;
typedef QList<UserGroupInfoData>  UserGroupInfoList;
// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief 用户列表信息
 */
// ////////////////////////////////////////////////////////////////////////////
class PERM_API UserGrpListInfo : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int selectIndex READ selectIndex WRITE setSelectIndex NOTIFY selectIndexChanged)
public:
    static UserGrpListInfo * getInstance( );
    static void release( );

    int  selectIndex( ) const;
    void setSelectIndex( const int & );
    Q_SIGNAL void selectIndexChanged( );

    UserGroupInfoData  currentUserGrpInfo( ) const;
    void addItem( const UserGroupInfoData & );
    void modItem( const UserGroupInfoData & );
    void rmvItem( );
    void refItem( const UserGroupInfoList & );

private:
    Q_INVOKABLE explicit UserGrpListInfo( );
    virtual ~UserGrpListInfo( ) Q_DECL_OVERRIDE;

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
    friend class UserGrpListInfoPriv;
    void *m_obj;
    Q_DISABLE_COPY( UserGrpListInfo )
};

}

#endif // PERM_USERGRPLISTINFO_HXX
