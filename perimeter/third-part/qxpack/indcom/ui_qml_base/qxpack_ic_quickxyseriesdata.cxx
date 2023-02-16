#ifndef QXPACK_IC_QUICKXYSERIESDATA_CXX
#define QXPACK_IC_QUICKXYSERIESDATA_CXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include "qxpack/indcom/common/qxpack_ic_pimplprivtemp.hpp"
#include "qxpack_ic_quickxyseriesdata.hxx"
#include <QObject>
#include <QDebug>
#include <QReadWriteLock>
#include <QtMath>
#include <cmath>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
// private object
// ////////////////////////////////////////////////////////////////////////////
#define T_PrivPtr( o )  T_ObjCast( IcQuickXYSeriesDataPriv*, o )
class QXPACK_IC_HIDDEN  IcQuickXYSeriesDataPriv :
        public QObject,
        public IcPImplPrivTemp<IcQuickXYSeriesDataPriv>
{
    Q_OBJECT
private:
    QList<QPointF>  m_data; QReadWriteLock m_rw_lock;
public :
    explicit IcQuickXYSeriesDataPriv();
    IcQuickXYSeriesDataPriv ( const IcQuickXYSeriesDataPriv & ) : QObject( Q_NULLPTR )
    { qFatal("IcQuickXySeriesDataPriv::ctor(copy) did not support!"); }

    virtual ~IcQuickXYSeriesDataPriv() Q_DECL_OVERRIDE;
    inline QReadWriteLock &  rwLock() { return m_rw_lock; }
    inline QList<QPointF>&   data()   { return m_data; }

    Q_SIGNAL void  dataChanged();
};

// ============================================================================
// ctor
// ============================================================================
IcQuickXYSeriesDataPriv :: IcQuickXYSeriesDataPriv()
{
}

// ============================================================================
// dtor
// ============================================================================
IcQuickXYSeriesDataPriv :: ~IcQuickXYSeriesDataPriv()
{
}

// ============================================================================
// add the point on each point
// ============================================================================


// ////////////////////////////////////////////////////////////////////////////
// wrap API
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcQuickXYSeriesData :: IcQuickXYSeriesData( )
{  m_obj = nullptr; }

// ============================================================================
// dtor
// ============================================================================
IcQuickXYSeriesData :: ~IcQuickXYSeriesData ( )
{
    if ( m_obj != nullptr ) {
        IcQuickXYSeriesDataPriv::attach( &m_obj, nullptr );
    }
}

// ============================================================================
// ctor ( build object )
// ============================================================================
IcQuickXYSeriesData :: IcQuickXYSeriesData ( UseMode m )
{
    Q_UNUSED( m );
    m_obj = nullptr;
    IcQuickXYSeriesDataPriv::buildIfNull( &m_obj );

    QObject::connect (
        T_PrivPtr( m_obj ), SIGNAL(dataChanged()),
        this, SIGNAL(dataChanged()), Qt::QueuedConnection
    );
}

// ============================================================================
// check if this object is null
// ============================================================================
bool                    IcQuickXYSeriesData :: isNull() const { return m_obj == nullptr; }

// ============================================================================
// check if it equal to another object
// ============================================================================
bool                    IcQuickXYSeriesData :: isEqulTo( const IcQuickXYSeriesData &other ) const
{   return ( m_obj == other.m_obj ); }

// ============================================================================
// attach to an existed object
// ============================================================================
bool                    IcQuickXYSeriesData :: attach( const IcQuickXYSeriesData &other )
{
    // free old connections
    if ( m_obj != nullptr ) {
        QObject::disconnect( T_PrivPtr( m_obj ), Q_NULLPTR, this, Q_NULLPTR );
        IcQuickXYSeriesDataPriv::attach( & m_obj, nullptr );
        m_obj = nullptr;
    }

    // attach to other object
    IcQuickXYSeriesDataPriv::attach( &m_obj, const_cast<void**>( & other.m_obj ));
    if ( m_obj != nullptr ) {
        QObject::connect(
            T_PrivPtr( m_obj ), SIGNAL(dataChanged()),
            this, SIGNAL(dataChanged()), Qt::QueuedConnection
        );
    }

   // IcQuickXYSeriesDataPriv::attach( &m_obj, const_cast<void**>( & other.m_obj ));
    return true;
}

// ============================================================================
// clear all data
// ============================================================================
void                    IcQuickXYSeriesData :: clear()
{
    if ( m_obj != nullptr ) {
        T_PrivPtr( m_obj )->rwLock().lockForWrite();
        T_PrivPtr( m_obj )->data() = QList<QPointF>();
        T_PrivPtr( m_obj )->rwLock().unlock();
        //QMetaObject::invokeMethod( this, "dataChanged", Qt::QueuedConnection );
        emit T_PrivPtr( m_obj )->dataChanged();
    }
}

// ============================================================================
// return the data size of the series data
// ============================================================================
int                     IcQuickXYSeriesData :: size() const
{
    if ( m_obj != nullptr ) {
        T_PrivPtr( m_obj )->rwLock().lockForRead();
        int sz = T_PrivPtr( m_obj )->data().size();
        T_PrivPtr( m_obj )->rwLock().unlock();
        return sz;
    } else {
        return 0;
    }
}

// ============================================================================
// return the data copy ( implicit shared )
// ============================================================================
QList<QPointF>            IcQuickXYSeriesData :: data( ) const
{
    if ( m_obj != nullptr ) {
        T_PrivPtr( m_obj )->rwLock().lockForRead();
        QList<QPointF> lst = T_PrivPtr( m_obj )->data();
        T_PrivPtr( m_obj )->rwLock().unlock();
        return lst;
    } else {
        return QList<QPointF>();
    }
}

// ============================================================================
// pick the data at spec. x coord.
// ============================================================================
static QPair<QPointF,QPointF>  gFindBoundYdata( const QList<QPointF> &lst, const qreal &x, bool &is_found )
{
    QPointF  first, second;

    auto c_itr = lst.constBegin();
    while ( c_itr != lst.constEnd() &&  c_itr->x() <= x ) {
        ++ c_itr;
    }
    if ( c_itr != lst.constEnd()) {
        second = *( c_itr );
        first  = ( c_itr == lst.constBegin() ? second  : *( -- c_itr ) );
        is_found = true;
    } else {
        if ( lst.size() > 0 ) {           
            first = second = lst.last();
            is_found = true;
        }
    }

    return QPair<QPointF,QPointF>( first, second );
}

// ============================================================================
// find the bound of spec. x
// ============================================================================
QPair<QPointF,QPointF>    IcQuickXYSeriesData :: findBound( const qreal &x, bool &is_found )
{
    if ( m_obj != nullptr ) {
        auto *priv = T_PrivPtr( m_obj );
        priv->rwLock().lockForRead();
        auto pair = gFindBoundYdata( priv->data(), x, is_found );
        priv->rwLock().unlock();
        return pair;
    } else {
        is_found = false;
        return QPair<QPointF,QPointF>();
    }
}

// ============================================================================
// set the hole data
// ============================================================================
void                      IcQuickXYSeriesData :: setData( const QList<QPointF> &dt )
{
    if ( m_obj != nullptr ) {
        T_PrivPtr( m_obj )->rwLock().lockForWrite();
        T_PrivPtr( m_obj )->data() = dt;
        T_PrivPtr( m_obj )->rwLock().unlock();
        emit T_PrivPtr( m_obj )->dataChanged();
    }
}

// ============================================================================
// push back a new data
// ============================================================================
void                    IcQuickXYSeriesData :: rmvFrontAndPushBack( int num, const QPointF &dt )
{
    if ( m_obj != nullptr ) {
        T_PrivPtr( m_obj )->rwLock().lockForWrite();
        QList<QPointF> &lst = T_PrivPtr( m_obj )->data();
        int sz = lst.size();
        if ( num < 0  ) { num = 0;  }
        if ( num > sz ) { num = sz; }
        if ( num > 0  ) { lst.erase( lst.begin(), lst.begin() + num ); }
        lst.push_back( dt );
        T_PrivPtr( m_obj )->rwLock().unlock();
        emit T_PrivPtr( m_obj )->dataChanged();
    }
}

void                    IcQuickXYSeriesData :: rmvFrontAndPushBack( int num, const QList<QPointF> &dt_lst )
{
    if ( m_obj != nullptr ) {
        T_PrivPtr( m_obj )->rwLock().lockForWrite();
        QList<QPointF> &lst = T_PrivPtr( m_obj )->data();
        int sz = lst.size();
        if ( num < 0  ) { num = 0;  }
        if ( num > sz ) { num = sz; }
        if ( num > 0  ) { lst.erase( lst.begin(), lst.begin() + num ); }
        lst.append( dt_lst );
        T_PrivPtr( m_obj )->rwLock().unlock();
        emit T_PrivPtr( m_obj )->dataChanged();
    }
}

// ============================================================================
// remove the front data
// ============================================================================
void                    IcQuickXYSeriesData :: removeFront( int num )
{
    if ( num > 0  &&  m_obj != nullptr ) {
        T_PrivPtr( m_obj )->rwLock().lockForWrite();
        QList<QPointF> &lst = T_PrivPtr( m_obj )->data();
        int sz = lst.size();
        if ( num > sz ) { num = sz; }
        if ( num > 0  ) { lst.erase( lst.begin(), lst.begin() + num ); }
        T_PrivPtr( m_obj )->rwLock().unlock();
        emit T_PrivPtr( m_obj )->dataChanged();
    }
}




}


#include "qxpack_ic_quickxyseriesdata.moc"
#endif
