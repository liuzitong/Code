#ifndef PERM_USERLISTINFO_CXX
#define PERM_USERLISTINFO_CXX

#include "perm_userlistinfo.hxx"
#include "perm/model/perm_userinfodata.hxx"
#include "perm/common/perm_memcntr.hxx"
#include "qxpack/indcom/sys/qxpack_ic_rmtobjdeletor_priv.hxx"
#include <QMutex>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>

namespace FcPerm {

#define T_PrivPtr( o )  perm_objcast( UserListInfoPriv*, o )
class PERM_HIDDEN UserListInfoPriv
{
private:
    UserListInfo        * m_parent;
    QList<UserInfoData>   m_dataList;
    int m_currentIndex;

protected:
    void insert( int row, const UserInfoData & info );
    void modify( int row, const UserInfoData & info );
    void remove( int row );
    void clear ( );

public:
    explicit UserListInfoPriv ( UserListInfo *pa );
    ~UserListInfoPriv( );
    inline int  rowCount( ) { return m_dataList.size(); }
    QVariant    data( int idx, int role );

    inline  UserInfoData currentUserInfo( );
    inline  int   & currentIndex( ) { return m_currentIndex; }
    inline  void    addItem( const UserInfoData & data ) { this->insert(this->rowCount(), data); }
    inline  void    modItem( const UserInfoData & data ) { this->modify(m_currentIndex, data); }
    inline  void    rmvItem( );
    inline  void    empItem( ) { this->clear(); }
};

UserListInfoPriv::UserListInfoPriv(UserListInfo *pa)
{
    m_parent = pa; m_currentIndex = -1;
}

UserListInfoPriv::~UserListInfoPriv()
{
}

// ============================================================================
// return the data of spec. index
// ============================================================================
QVariant  UserListInfoPriv :: data ( int idx, int role )
{
    QVariant var;
    if ( idx >= 0  && idx < m_dataList.size() ) {
        switch( role ) { //[HINT] DisplayRole is the default role name ("display")
        case Qt::DisplayRole : var = QVariant( m_dataList.at( idx ).UID () ); break;
        case Qt::UserRole + 1: var = QVariant( m_dataList.at( idx ).name() );  break;
        case Qt::UserRole + 2: var = QVariant( m_dataList.at( idx ).memo() ); break;
        default: break;
        }
    }
    return var;
}

UserInfoData UserListInfoPriv::currentUserInfo()
{
    if ( m_currentIndex >= 0 && m_dataList.size() > m_currentIndex )
        return m_dataList.at(m_currentIndex);
    return UserInfoData( );
}

void UserListInfoPriv::insert(int row, const UserInfoData &info)
{
    m_parent->beginInsertRows( QModelIndex(), row, row );
    m_dataList.insert( row, info );
    m_parent->endInsertRows();
}

void UserListInfoPriv::modify(int row, const UserInfoData &info)
{
    m_dataList.replace( row, info);
    emit m_parent->dataChanged( m_parent->index(row), m_parent->index(row) );
}

void UserListInfoPriv::remove(int row)
{
    m_parent->beginRemoveRows( QModelIndex(), row, row );
    m_dataList.removeAt(row);
    m_parent->endRemoveRows();
}

void UserListInfoPriv::clear()
{
    if ( m_dataList.size() <= 0 ) { return; }

    m_parent->beginRemoveRows( QModelIndex(), 0, this->rowCount()-1 );
    m_dataList.clear(); m_currentIndex = -1;
    m_parent->endRemoveRows();
}

void UserListInfoPriv::rmvItem()
{
    if ( m_currentIndex >= 0 && m_currentIndex < m_dataList.size() )
    { remove( m_currentIndex ); }
}

static QAtomicPointer<UserListInfo>  g_ptr( Q_NULLPTR );
static QMutex       g_locker;
static QAtomicInt   g_ref_cntr(0);

UserListInfo *UserListInfo::getInstance()
{
    UserListInfo *ptr = Q_NULLPTR;

    g_locker.lock();
    ptr = g_ptr.loadAcquire();
    if ( ptr == Q_NULLPTR ) {
        g_ptr.store( ( ptr = new UserListInfo( ) ) );
        g_ref_cntr.store(1);
    } else {
        g_ref_cntr.fetchAndAddOrdered(1);
    }
    g_locker.unlock();
    return ptr;
}

void UserListInfo::release()
{
    UserListInfo *ptr = Q_NULLPTR;

    g_locker.lock();
    ptr = g_ptr.loadAcquire();
    if ( ptr != Q_NULLPTR ) {
        if ( g_ref_cntr.fetchAndSubOrdered(1) - 1 == 0 ) {
            QxPack::IcRmtObjDeletor::deleteObjInThread(
                ptr->thread(),
                [](void*, QObject* obj ) {
                    UserListInfo *svc = qobject_cast<UserListInfo*>( obj );
                    delete svc;
                }, Q_NULLPTR, ptr, false
            );
            g_ptr.store( Q_NULLPTR );
        }
    }
    g_locker.unlock();
}

int UserListInfo::selectIndex() const
{ return T_PrivPtr( m_obj )->currentIndex(); }

void UserListInfo::setSelectIndex(const int &idx)
{ T_PrivPtr( m_obj )->currentIndex() = idx; }

UserInfoData UserListInfo::currentUserInfo() const
{ return T_PrivPtr( m_obj )->currentUserInfo(); }

void UserListInfo::addItem(const UserInfoData &data)
{ T_PrivPtr( m_obj )->addItem(data); }

void UserListInfo::modItem(const UserInfoData &data)
{ T_PrivPtr( m_obj )->modItem(data); }

void UserListInfo::rmvItem()
{ T_PrivPtr( m_obj )->rmvItem(); }

void UserListInfo::empItem()
{ T_PrivPtr( m_obj )->empItem(); }

UserListInfo::UserListInfo()
{
    m_obj = perm_new( UserListInfoPriv, this );
}

UserListInfo::~UserListInfo()
{
    perm_delete( m_obj, UserListInfoPriv );
}

// ============================================================================
// return the total count of records
// ============================================================================
int  UserListInfo :: rowCount(const QModelIndex &) const
{ return T_PrivPtr( m_obj )->rowCount(); }

// ============================================================================
// return the role flags
// ============================================================================
Qt::ItemFlags  UserListInfo :: flags ( const QModelIndex &idx ) const
{  return ( idx.isValid() ? QAbstractItemModel::flags( idx ) | Qt::ItemIsUserCheckable : Qt::NoItemFlags );}

// ============================================================================
// return the role names used by model
// ============================================================================
QHash<int,QByteArray>  UserListInfo :: roleNames() const
{
    QHash<int,QByteArray> roles;
    roles.insert( Qt::DisplayRole,  "uid" );
    roles.insert( Qt::UserRole + 1, "name" );
    roles.insert( Qt::UserRole + 2, "memo");
    return roles;
}

// ============================================================================
// return the data by index and role
// ============================================================================
QVariant   UserListInfo :: data ( const QModelIndex &idx, int role ) const
{  return T_PrivPtr( m_obj )->data( idx.row(), role );  }

// ============================================================================
// set the data, now allowed.
// ============================================================================
bool       UserListInfo :: setData ( const QModelIndex &, const QVariant &, int )
{ return false; }

}

#endif // PERM_USERLISTINFO_CXX
