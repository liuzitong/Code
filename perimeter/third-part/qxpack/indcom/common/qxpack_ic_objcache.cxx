#ifndef QXPACK_IC_OBJCACHETEMP_HPP
#define QXPACK_IC_OBJCACHETEMP_HPP

#include "qxpack_ic_objcache.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/common/qxpack_ic_tmrqueue.hxx"
#include "qxpack/indcom/common/qxpack_ic_queuetemp.hpp"

namespace QxPack {


// ////////////////////////////////////////////////////////////////////////////
//                      Object Cache Private
// ////////////////////////////////////////////////////////////////////////////
#define T_PrivPtr( o )  T_ObjCast( IcObjCachePriv*, o )
class QXPACK_IC_HIDDEN IcObjCachePriv {
private:
    typedef IcArrayQueuePsTemp<void*> ObjQue;

    ObjQue  *m_obj_que_ts;
    IcObjCache::OprIF  *m_opr_if;
    IcTmrQueue  *m_tmr_que;
    size_t  m_max_obj_num;
    int     m_tmr_gap;

protected:
    // ========================================================================
    // handle the timeout routine, here directly free the object
    // ========================================================================
    static void tmrQue_onTimeout (
        QxPack::qxic_uid_t,
        const IcVariant &ctxt, const IcVariant &,
        const IcVariant &,   const IcVariant &, bool is_valid
    ) {
        if ( ! is_valid ) { return; }

        IcObjCachePriv *t_this =
             reinterpret_cast<IcObjCachePriv*>( ctxt.toVoidStar() );
        if ( t_this == nullptr ) { return; }

        void* ptr;
        if ( ! t_this->m_obj_que_ts->dequeue( ptr )) { return; }
        t_this->m_opr_if->deleteObj( ptr );

        t_this->shedExpiryTmrCb();
    }

    // ========================================================================
    // shedule expiry timer callback
    // ========================================================================
    inline void  shedExpiryTmrCb( )
    {
        m_tmr_que->singleShot (
            m_tmr_gap, &tmrQue_onTimeout,
            IcVariant(this), IcVariant(), IcVariant(), IcVariant()
        );
    }

    // ========================================================================
    // remove all items in the queue
    // ========================================================================
    void  rmvAll()
    {
        void* ptr = nullptr;
        while ( m_obj_que_ts->dequeue( ptr ) ) {
            m_opr_if->deleteObj( ptr );
        }
    }

public :
    explicit IcObjCachePriv( size_t max_obj_num, int tmr_gap, IcObjCache::OprIF *opr_if );
    ~IcObjCachePriv( );
    inline IcArrayQueuePsTemp<void*>* objQue() { return m_obj_que_ts; }
    inline IcObjCache::OprIF* oprIf() { return m_opr_if; }
};

// ============================================================================
// ctor
// ============================================================================
IcObjCachePriv ::  IcObjCachePriv( size_t max_obj_num, int tmr_gap, IcObjCache::OprIF *opr_if )
{
    m_opr_if  = opr_if;
    m_max_obj_num = ( max_obj_num < 1 ? 1 : max_obj_num );
    m_obj_que_ts  = qxpack_ic_new( ObjQue, m_max_obj_num );
    m_tmr_gap = ( tmr_gap < 16 ? 16 : tmr_gap );
    m_tmr_que = qxpack_ic_new( IcTmrQueue, IcTmrQueue::disposeObjCbQueName() );

    shedExpiryTmrCb();
}

// ============================================================================
// dtor
// ============================================================================
IcObjCachePriv :: ~IcObjCachePriv ()
{
    qxpack_ic_delete( m_tmr_que, IcTmrQueue );
    rmvAll();
    qxpack_ic_delete( m_obj_que_ts, ObjQue );

    m_opr_if->deleteSelf();
}


// ////////////////////////////////////////////////////////////////////////////
//                 wrap API
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcObjCache :: IcObjCache( size_t max_obj_num, int tmr_gap, OprIF *opr_if )
{
    m_obj = qxpack_ic_new( IcObjCachePriv, max_obj_num, tmr_gap, opr_if );
}

// ============================================================================
// dtor
// ============================================================================
IcObjCache :: ~IcObjCache ( )
{
    qxpack_ic_delete( m_obj, IcObjCachePriv );
}

// ============================================================================
// return a useable object
// ============================================================================
void*         IcObjCache :: alloc()
{
    void*  ptr;
    if ( T_PrivPtr( m_obj )->objQue()->dequeue( ptr ) ) {
        T_PrivPtr( m_obj )->oprIf()->initObj( ptr );
    } else {
        ptr = T_PrivPtr( m_obj )->oprIf()->createObj();
    }

    return ptr;
}

// ============================================================================
// recycle the object
// ============================================================================
void          IcObjCache :: recycle( void* ptr )
{
    T_PrivPtr( m_obj )->oprIf()->deinitObj( ptr );

    T_PrivPtr( m_obj )->objQue()->forceEnqueue (
        ptr,
        []( void* &o, void *ctxt ) {
            IcObjCache *t_this = reinterpret_cast<IcObjCache*>( ctxt );
            T_PrivPtr( t_this->m_obj )->oprIf()->deleteObj( o );
        }, this
    );
}

// ============================================================================
// check if empty
// ============================================================================
bool          IcObjCache :: isEmpty()
{
    return T_PrivPtr( m_obj )->objQue()->isEmpty();
}






}

#endif
