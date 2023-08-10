#ifndef QXPACK_IC_QUICKARYSERIESDATA_CXX
#define QXPACK_IC_QUICKARYSERIESDATA_CXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include "qxpack/indcom/common/qxpack_ic_pimplprivtemp.hpp"
#include "qxpack_ic_quickaryseriesdata.hxx"
#include <QObject>
#include <QDebug>
#include <QReadWriteLock>
#include <QtMath>
#include <cmath>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
// private object
// ////////////////////////////////////////////////////////////////////////////
#define T_PrivPtr( o )  T_ObjCast( IcQuickArySeriesDataPriv*, o )
class QXPACK_IC_HIDDEN  IcQuickArySeriesDataPriv :
        public QObject,
        public IcPImplPrivTemp<IcQuickArySeriesDataPriv>
{
    Q_OBJECT
private:
    IcQuickArySeriesData::SeriesData  m_data; QReadWriteLock m_rw_lock;
public :
    explicit IcQuickArySeriesDataPriv();
    IcQuickArySeriesDataPriv ( const IcQuickArySeriesDataPriv & ) : QObject( Q_NULLPTR )
    { qFatal("IcQuickArySeriesDataPriv::ctor(copy) did not support!"); }

    virtual ~IcQuickArySeriesDataPriv() Q_DECL_OVERRIDE;
    inline auto  rwLock() -> QReadWriteLock& { return m_rw_lock; }
    inline auto  data()   -> IcQuickArySeriesData::SeriesData &  { return m_data; }

    Q_SIGNAL void  dataChanged();
};

// ============================================================================
// ctor
// ============================================================================
IcQuickArySeriesDataPriv :: IcQuickArySeriesDataPriv()
{
}

// ============================================================================
// dtor
// ============================================================================
IcQuickArySeriesDataPriv :: ~IcQuickArySeriesDataPriv()
{
}


// ////////////////////////////////////////////////////////////////////////////
// wrap API
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcQuickArySeriesData :: IcQuickArySeriesData( )
{  m_obj = nullptr; }

// ============================================================================
// dtor
// ============================================================================
IcQuickArySeriesData :: ~IcQuickArySeriesData ( )
{
    if ( m_obj != nullptr ) {
        IcQuickArySeriesDataPriv::attach( &m_obj, nullptr );
    }
}

// ============================================================================
// ctor ( build object )
// ============================================================================
IcQuickArySeriesData :: IcQuickArySeriesData ( UseMode m )
{
    Q_UNUSED( m );
    m_obj = nullptr; auto priv = IcQuickArySeriesDataPriv::buildIfNull( &m_obj );

    QObject::connect (
        priv, SIGNAL(dataChanged()), this, SIGNAL(dataChanged()), Qt::QueuedConnection
    );
}

// ============================================================================
// check if this object is null
// ============================================================================
auto     IcQuickArySeriesData :: isNull() const -> bool
{ return m_obj == nullptr; }

// ============================================================================
// check if it equal to another object
// ============================================================================
auto     IcQuickArySeriesData :: isEqulTo( const IcQuickArySeriesData &other ) const -> bool
{   return ( m_obj == other.m_obj ); }

// ============================================================================
// attach to an existed object
// ============================================================================
auto     IcQuickArySeriesData :: attach( const IcQuickArySeriesData &other ) -> bool
{
    // free old connections
    if ( m_obj != nullptr ) {
        QObject::disconnect( T_PrivPtr( m_obj ), Q_NULLPTR, this, Q_NULLPTR );
        IcQuickArySeriesDataPriv::attach( & m_obj, nullptr );
        m_obj = nullptr;
    }

    // attach to other object
    IcQuickArySeriesDataPriv::attach( &m_obj, const_cast<void**>( & other.m_obj ));
    if ( m_obj != nullptr ) {
        QObject::connect(
            T_PrivPtr( m_obj ), SIGNAL(dataChanged()),  this, SIGNAL(dataChanged()), Qt::QueuedConnection
        );
    }

    return true;
}

// ============================================================================
// clear all data
// ============================================================================
auto     IcQuickArySeriesData :: clear() -> void
{
    if ( m_obj != nullptr ) {
        auto priv = T_PrivPtr(m_obj);
        priv->rwLock().lockForWrite();
        priv->data() = IcQuickArySeriesData::SeriesData();
        priv->rwLock().unlock();
        emit priv->dataChanged();  // fix bugs
        //QMetaObject::invokeMethod( this, "dataChanged", Qt::QueuedConnection );
    }
}

// ============================================================================
// return the data size of the series data
// ============================================================================
auto     IcQuickArySeriesData :: size() const -> int
{
    if ( m_obj != nullptr ) {
        auto priv = T_PrivPtr(m_obj);
        priv->rwLock().lockForRead();
        int sz = priv->data().vec.size();
        priv->rwLock().unlock();
        return sz;
    } else {
        return 0;
    }
}

// ============================================================================
// return the data copy ( implicit shared )
// ============================================================================
auto     IcQuickArySeriesData :: data( ) const -> SeriesData
{
    if ( m_obj != nullptr ) {
        auto priv = T_PrivPtr(m_obj);
        priv->rwLock().lockForRead();
        auto dt = priv->data();
        priv->rwLock().unlock();
        return dt;
    } else {
        return SeriesData();
    }
}

// ============================================================================
// pick the data at spec. x coord.
// ============================================================================
static QPair<QPointF,QPointF>  gFindBoundYdata( const IcQuickArySeriesData::SeriesData &sd, const qreal &x, bool &is_found )
{
    QPointF  first, second;

    // is out of bound ?
    auto hi_bound = qreal( sd.x_ofv + sd.step * sd.vec.size() );
    if ( sd.vec.empty() || x < qreal(sd.x_ofv) || x >= hi_bound ) { is_found = false; return QPair<QPointF,QPointF>(); }

    // find x coord that less or equal than x
    auto seg     = x - qreal( sd.x_ofv );
    auto seg_dn    = seg / qreal( sd.step );
    auto seg_int   = seg_dn;
    std::modf( seg_dn, &seg_int );

    auto x_idx1   = int( int64_t( seg_int )); // x_idx1 pos data is less or equal than x.
    auto x_idx2   = ( x_idx1 < sd.vec.size() - 1 ? x_idx1 + 1 : x_idx1 ); //  20210628, nw fixed: old is x_idx1 < sd.vec.size()
    auto x_coord1 = sd.x_ofv + sd.step * x_idx1;
    auto x_coord2 = sd.x_ofv + sd.step * x_idx2;

    is_found = true;
    return QPair<QPointF,QPointF>( QPointF( qreal( x_coord1 ), qreal( sd.vec.at(x_idx1))), QPointF( qreal( x_coord2 ), qreal(sd.vec.at(x_idx2))) );
}

// ============================================================================
// set the hole data
// ============================================================================
auto     IcQuickArySeriesData :: setData( const SeriesData &dt ) -> void
{
    if ( m_obj != nullptr ) {
        auto priv = T_PrivPtr(m_obj);
        priv->rwLock().lockForWrite();
        priv->data() = dt;
        priv->rwLock().unlock();
        emit priv->dataChanged();
        //QMetaObject::invokeMethod( this, "dataChanged", Qt::QueuedConnection );
    }
}

// ============================================================================
// find spec. coord. point
// ============================================================================
auto     IcQuickArySeriesData ::  findBound( const qreal &x, bool &is_found ) -> QPair<QPointF,QPointF>
{
    if ( m_obj != nullptr ) {
        auto priv = T_PrivPtr(m_obj);
        return gFindBoundYdata( priv->data(), x, is_found );
    } else {
        return QPair<QPointF,QPointF>(QPointF(),QPointF());
    }
}

}


#include "qxpack_ic_quickaryseriesdata.moc"
#endif
