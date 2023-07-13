#ifndef PERM_USERGRPLISTINFO_CXX
#define PERM_USERGRPLISTINFO_CXX

#include "perm_usergrplistinfo.hxx"
#include "perm/model/perm_usergroupinfodata.hxx"
#include "perm/common/perm_memcntr.hxx"
#include "qxpack/indcom/sys/qxpack_ic_rmtobjdeletor_priv.hxx"
#include <QMutex>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>

namespace FcPerm {

#define T_PrivPtr( o )  perm_objcast( UserGrpListInfoPriv*, o )
class PERM_HIDDEN UserGrpListInfoPriv
{
private:
    UserGrpListInfo          * m_parent;
    QList<UserGroupInfoData>   m_dataList;
    int m_currentIndex;

protected:
    void insert( int row, const UserGroupInfoData & info );
    void modify( int row, const UserGroupInfoData & info );
    void remove( int row );
    void clear ( );

public:
    explicit UserGrpListInfoPriv ( UserGrpListInfo *pa );
    ~UserGrpListInfoPriv( );
    inline int  rowCount( ) { return m_dataList.size(); }
    QVariant    data( int idx, int role );

    inline  UserGroupInfoData currentUserGrpInfo( );
    inline  int   & currentIndex( ) { return m_currentIndex; }
    inline  void    addItem( const UserGroupInfoData & data ) { this->insert(this->rowCount(), data); }
    inline  void    modItem( const UserGroupInfoData & data ) { this->modify(m_currentIndex, data); }
    inline  void    rmvItem( );
    inline  void    refItem( const UserGroupInfoList & );
};

UserGrpListInfoPriv::UserGrpListInfoPriv(UserGrpListInfo *pa)
{
    m_parent = pa;
    m_currentIndex = -1;
}

UserGrpListInfoPriv::~UserGrpListInfoPriv()
{
}

// ============================================================================
// return the data of spec. index
// ============================================================================
QVariant  UserGrpListInfoPriv :: data ( int idx, int role )
{
    QVariant var;
    if ( idx >= 0  && idx < m_dataList.size() ) {
        switch( role ) { //[HINT] DisplayRole is the default role name ("display")
        case Qt::DisplayRole : var = QVariant( m_dataList.at( idx ).GID () ); break;
        case Qt::UserRole + 1: var = QVariant( m_dataList.at( idx ).name() );  break;
        case Qt::UserRole + 2: var = QVariant( m_dataList.at( idx ).memo() ); break;
        default: break;
        }
    }
    return var;
}

UserGroupInfoData UserGrpListInfoPriv::currentUserGrpInfo()
{
    if ( m_currentIndex >= 0 && m_dataList.size() > m_currentIndex )
        return m_dataList.at(m_currentIndex);
    return UserGroupInfoData( );
}

void UserGrpListInfoPriv::insert(int row, const UserGroupInfoData &info)
{
    m_parent->beginInsertRows( QModelIndex(), row, row );
    m_dataList.insert( row, info );
    m_parent->endInsertRows();
}

void UserGrpListInfoPriv::modify(int row, const UserGroupInfoData &info)
{
    m_dataList.replace( row, info );
    emit m_parent->dataChanged( m_parent->index(row), m_parent->index(row) );
}

void UserGrpListInfoPriv::remove(int row)
{
    m_parent->beginRemoveRows( QModelIndex(), row, row );
    m_dataList.removeAt(row);
    m_parent->endRemoveRows();
}

void UserGrpListInfoPriv::clear()
{
    if ( m_dataList.size() <= 0 ) { return; }

    m_parent->beginRemoveRows( QModelIndex(), 0, this->rowCount()-1 );
    m_dataList.clear();
    m_parent->endRemoveRows();
}

void UserGrpListInfoPriv::rmvItem()
{
    if ( m_currentIndex >= 0 && m_currentIndex < m_dataList.size() )
    { remove( m_currentIndex ); }
}

void UserGrpListInfoPriv::refItem(const UserGroupInfoList &vals)
{
    this->clear();

    if ( vals.size() > 0 )
    {
        m_parent->beginInsertRows( QModelIndex(), 0, vals.count() - 1 );
        m_dataList = vals;
        m_parent->endInsertRows( );
    }
}

static QAtomicPointer<UserGrpListInfo>  g_ptr( Q_NULLPTR );
static QMutex       g_locker;
static QAtomicInt   g_ref_cntr(0);

UserGrpListInfo *UserGrpListInfo::getInstance()
{
    UserGrpListInfo *ptr = Q_NULLPTR;

    g_locker.lock();
    ptr = g_ptr.loadAcquire();
    if ( ptr == Q_NULLPTR ) {
        g_ptr.store( ( ptr = new UserGrpListInfo( ) ) );
        g_ref_cntr.store(1);
    } else {
        g_ref_cntr.fetchAndAddOrdered(1);
    }
    g_locker.unlock();
    return ptr;
}

void UserGrpListInfo::release()
{
    UserGrpListInfo *ptr = Q_NULLPTR;

    g_locker.lock();
    ptr = g_ptr.loadAcquire();
    if ( ptr != Q_NULLPTR ) {
        if ( g_ref_cntr.fetchAndSubOrdered(1) - 1 == 0 ) {
            QxPack::IcRmtObjDeletor::deleteObjInThread(
                ptr->thread(),
                [](void*, QObject* obj ) {
                    UserGrpListInfo *svc = qobject_cast<UserGrpListInfo*>( obj );
                    delete svc;
                }, Q_NULLPTR, ptr, false
            );
            g_ptr.store( Q_NULLPTR );
        }
    }
    g_locker.unlock();
}

int UserGrpListInfo::selectIndex() const
{ return T_PrivPtr( m_obj )->currentIndex(); }

void UserGrpListInfo::setSelectIndex(const int &idx)
{ T_PrivPtr( m_obj )->currentIndex() = idx; }

UserGroupInfoData UserGrpListInfo::currentUserGrpInfo() const
{ return T_PrivPtr( m_obj )->currentUserGrpInfo(); }

void UserGrpListInfo::addItem(const UserGroupInfoData &data)
{ T_PrivPtr( m_obj )->addItem(data); }

void UserGrpListInfo::modItem(const UserGroupInfoData &data)
{ T_PrivPtr( m_obj )->modItem(data); }

void UserGrpListInfo::rmvItem()
{ T_PrivPtr( m_obj )->rmvItem(); }

void UserGrpListInfo::refItem(const UserGroupInfoList &vals)
{ T_PrivPtr( m_obj )->refItem(vals); }

UserGrpListInfo::UserGrpListInfo()
{
    m_obj = perm_new( UserGrpListInfoPriv, this );
}

UserGrpListInfo::~UserGrpListInfo()
{
    perm_delete( m_obj, UserGrpListInfoPriv );
}

// ============================================================================
// return the total count of records
// ============================================================================
int  UserGrpListInfo :: rowCount(const QModelIndex &) const
{ return T_PrivPtr( m_obj )->rowCount(); }

// ============================================================================
// return the role flags
// ============================================================================
Qt::ItemFlags  UserGrpListInfo :: flags ( const QModelIndex &idx ) const
{  return ( idx.isValid() ? QAbstractItemModel::flags( idx ) | Qt::ItemIsUserCheckable : Qt::NoItemFlags );}

// ============================================================================
// return the role names used by model
// ============================================================================
QHash<int,QByteArray>  UserGrpListInfo :: roleNames() const
{
    QHash<int,QByteArray> roles;
    roles.insert( Qt::DisplayRole,  "gid" );
    roles.insert( Qt::UserRole + 1, "name" );
    roles.insert( Qt::UserRole + 2, "memo");
    return roles;
}

// ============================================================================
// return the data by index and role
// ============================================================================
QVariant   UserGrpListInfo :: data ( const QModelIndex &idx, int role ) const
{  return T_PrivPtr( m_obj )->data( idx.row(), role );  }

// ============================================================================
// set the data, now allowed.
// ============================================================================
bool       UserGrpListInfo :: setData ( const QModelIndex &, const QVariant &, int )
{ return false; }

}

#endif // PERM_USERGRPLISTINFO_CXX
