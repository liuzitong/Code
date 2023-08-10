#ifndef QXPACK_IC_QUICKLBLLAYOUTSRC_P_CXX
#define QXPACK_IC_QUICKLBLLAYOUTSRC_P_CXX

#include "qxpack_ic_quicklbllayoutsrc_p.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
// private object
// ////////////////////////////////////////////////////////////////////////////
#define T_PrivPtr( o )  T_ObjCast( IcQuickLblLayoutSrcPriv*, o )
class QXPACK_IC_HIDDEN IcQuickLblLayoutSrcPriv {
private:
   qint64 m_start_idx, m_end_idx;
   qint64 m_major_tick_step, m_minor_hint_tick_step;
   qreal  m_vw_ofv, m_ln_par_min, m_vw_rg_sz, m_inv_vw_rg_sz;
   qreal  m_tick_step;
   bool   m_is_valid;
public :
   explicit IcQuickLblLayoutSrcPriv( );
   ~IcQuickLblLayoutSrcPriv(){ }
   auto  invalidate() -> void;
   inline auto  isValid()       -> bool&     { return m_is_valid;  }
   inline auto  startIdx()      -> qint64&   { return m_start_idx; }
   inline auto  endIdx()        -> qint64&   { return m_end_idx;   }
   inline auto  viewOfv()       -> qreal&    { return m_vw_ofv;    }
   inline auto  viewRangeSize() -> qreal&    { return m_vw_rg_sz;  }
   inline auto  invViewRangeSize() -> qreal& { return m_inv_vw_rg_sz; }
   inline auto  tickStep()      -> qreal&    { return m_tick_step; }
   inline auto  lnParMin()      -> qreal&    { return m_ln_par_min; }
   inline auto  majorTickStep() -> qint64&   { return m_major_tick_step; }
   inline auto  minorHintStep() -> qint64&   { return m_minor_hint_tick_step; }
   inline auto  isIdxInRange( const qint64 &idx ) const ->bool { return idx >= m_start_idx && idx <= m_end_idx; }
};

// ============================================================================
// ctor ( setup default value )
// ============================================================================
IcQuickLblLayoutSrcPriv :: IcQuickLblLayoutSrcPriv ( )
{
    invalidate();
}

// ============================================================================
// invalidate
// ============================================================================
auto     IcQuickLblLayoutSrcPriv :: invalidate() -> void
{
    m_start_idx = m_end_idx = -1;
    m_major_tick_step = 0; m_minor_hint_tick_step = 0;
    m_vw_ofv = 0; m_vw_rg_sz = 0; m_inv_vw_rg_sz = 0;
    m_tick_step = 0; m_ln_par_min = 0;
    m_is_valid = false;
}

// ////////////////////////////////////////////////////////////////////////////
// wrap API
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcQuickLblLayoutSrc :: IcQuickLblLayoutSrc ( QObject *pa ) : QObject( pa )
{
    m_obj = qxpack_ic_new( IcQuickLblLayoutSrcPriv );
}

// ============================================================================
// dtor
// ============================================================================
IcQuickLblLayoutSrc :: ~IcQuickLblLayoutSrc()
{
    qxpack_ic_delete( m_obj, IcQuickLblLayoutSrcPriv );
}

// ============================================================================
// initialize the data
// ============================================================================
auto     IcQuickLblLayoutSrc :: init (
    const qint64 &st_idx, const qint64 &end_idx,
    const qreal  &vw_ofv, const qreal  &vw_rg_sz,
    const qreal  &tk_step,    const qreal  &ln_par_min,
    const qint64 &mj_tk_step, const qint64& mn_hint_tk_step
) -> void {
    auto priv = T_PrivPtr( m_obj );

    // setup the data
    priv->startIdx() = st_idx;  priv->endIdx() = end_idx;
    priv->viewOfv()  = vw_ofv;  priv->viewRangeSize() = vw_rg_sz;
    priv->invViewRangeSize() = ( vw_rg_sz > 0 ? 1.0 / vw_rg_sz : 1 );
    priv->tickStep() = tk_step;
    priv->lnParMin() = ln_par_min;
    priv->majorTickStep() = mj_tk_step;
    priv->minorHintStep() = mn_hint_tk_step;
    priv->isValid() = ( mj_tk_step > 0 &&  end_idx >= st_idx );

    // shedule a signal
    QMetaObject::invokeMethod( this, "dataChanged", Qt::QueuedConnection );
}

// ============================================================================
// check if object is valid
// ============================================================================
auto     IcQuickLblLayoutSrc :: isValid() const -> bool
{   return T_PrivPtr( m_obj )->isValid(); }

// ============================================================================
// invalidate the object
// ============================================================================
auto     IcQuickLblLayoutSrc :: invalidate() -> void
{
    T_PrivPtr( m_obj )->invalidate();
    QMetaObject::invokeMethod( this, "dataChanged", Qt::QueuedConnection );
}

// ============================================================================
// return the count of current ticks
// ============================================================================
auto     IcQuickLblLayoutSrc :: count() const -> qint64
{
    auto priv = T_PrivPtr( m_obj );
    return ( priv->isValid() ? priv->endIdx() - priv->startIdx() + 1 : 0 );
}

// ============================================================================
// access start and end index
// ============================================================================
auto     IcQuickLblLayoutSrc :: startIdx() const -> qint64
{ return T_PrivPtr(m_obj)->startIdx(); }

auto     IcQuickLblLayoutSrc :: endIdx() const -> qint64
{ return T_PrivPtr(m_obj)->endIdx();   }

// ============================================================================
// return the percentage of the spec. tick index
// ============================================================================
auto     IcQuickLblLayoutSrc :: viewPctPosAt( const qint64 &tk_idx, TickType *tk_type ) const -> qreal
{
    auto priv = T_PrivPtr( m_obj );
    if ( priv->isValid() && priv->isIdxInRange( tk_idx )) {
        if ( tk_type != Q_NULLPTR ) {
            if ( tk_idx == 0 || ( tk_idx % priv->majorTickStep()) == 0 ) { *tk_type = TickType_Major; }
            else if ( priv->minorHintStep() != 0 && ( tk_idx % priv->minorHintStep()) == 0 ) { *tk_type = TickType_MinorHint; }
            else { *tk_type = TickType_Minor; }
        }
        qreal  pct_pos = ( priv->tickStep() * tk_idx + priv->lnParMin() - priv->viewOfv()) * priv->invViewRangeSize();
        return ! qFuzzyIsNull( pct_pos ) ? pct_pos : 0;
    } else {
        if ( tk_type != Q_NULLPTR ) { *tk_type = TickType_Major; }
        return -1;
    }
}

// ============================================================================
// return the value position of spec. tick index
// ============================================================================
auto     IcQuickLblLayoutSrc :: valPosAt( const qint64 &tk_idx ) -> qreal
{
    auto priv = T_PrivPtr( m_obj );
    if ( priv->isValid() && priv->isIdxInRange( tk_idx )) {
        return priv->tickStep() * tk_idx + priv->lnParMin();
    } else {
        return 0;
    }
}


}

#endif
