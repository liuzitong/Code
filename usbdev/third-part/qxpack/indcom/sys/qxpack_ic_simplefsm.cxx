// ////////////////////////////////////////////////////////////////////////////
// 20190710 1412 nightwing
//  * fixed the routine mechanism
// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
// 20190621 1204 nightwing
//  * added cancel mechanism
// ////////////////////////////////////////////////////////////////////////////
#ifndef QXPACK_IC_SIMPLEFSM_CXX
#define QXPACK_IC_SIMPLEFSM_CXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/common/qxpack_ic_pimplprivtemp.hpp"
#include "qxpack_ic_simplefsm.hxx"
#include "qxpack_ic_recurmutex_p.hxx"

#include <QMetaObject>
#include <QMetaMethod>
#include <QMap>
#include <QThread>
#include <QMap>
#include <QSharedPointer>
#include <QAtomicPointer>
#include <QMutex>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
//                 FSM item
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN IcSimpleFsm_Item {
public :
    QString  m_src_state, m_evt, m_dst_state;
public :
    explicit IcSimpleFsm_Item ( ) { }

    IcSimpleFsm_Item ( const IcSimpleFsm_Item &o )
    { m_src_state = o.m_src_state;  m_evt = o.m_evt; m_dst_state = o.m_dst_state; }

    IcSimpleFsm_Item &  operator=( const IcSimpleFsm_Item &o )
    { m_src_state = o.m_src_state;  m_evt = o.m_evt; m_dst_state = o.m_dst_state; return *this; }
};

inline bool operator<( const IcSimpleFsm_Item &e1, const IcSimpleFsm_Item &e2 )
{
    if ( e1.m_src_state < e2.m_src_state ) {
        return true;
    } else if ( e1.m_src_state == e2.m_src_state ) {
        return e1.m_evt < e2.m_evt;
    } else {
        return false;
    }
}


// ////////////////////////////////////////////////////////////////////////////
//
//                 FSM template
//
// ////////////////////////////////////////////////////////////////////////////
typedef QMap<IcSimpleFsm_Item,QMetaMethod>  IcSimpleFsm_SesMap;
typedef QMap<QString,QMetaMethod>           IcSimpleFsm_SelMap;
typedef QMap<QString,bool>                  IcSimpleFsm_StaMap;
class QXPACK_IC_HIDDEN  IcSimpleFsm_Temp {
private:
    IcSimpleFsm_SesMap  m_state_evt_state_map;
    IcSimpleFsm_SelMap  m_state_enter_map, m_state_leave_map, m_state_routine_map;
    IcSimpleFsm_StaMap  m_state_name_map;
    QString  m_obj_class_name;
public :
    explicit IcSimpleFsm_Temp ( ) { }
    inline QString &             objClassNameRef( )      { return m_obj_class_name; }
    inline IcSimpleFsm_SesMap &  stateEvetStateMapRef( ) { return m_state_evt_state_map; }
    inline IcSimpleFsm_SelMap &  stateEnterMapRef( )  { return m_state_enter_map; }
    inline IcSimpleFsm_SelMap &  stateLeaveMapRef( )  { return m_state_leave_map; }
    inline IcSimpleFsm_SelMap &  stateRoutineMapRef( ){ return m_state_routine_map; }
    inline IcSimpleFsm_StaMap &  stateNameMapRef( )   { return m_state_name_map; }
};


// ////////////////////////////////////////////////////////////////////////////
//
//                      shared template manager
//
// ////////////////////////////////////////////////////////////////////////////
typedef QPair<int,IcSimpleFsm_Temp*>  IcSimpleFsm_TempPair;
class QXPACK_IC_HIDDEN  IcSimpleFsm_TempMgr {
private:
    QMap<QString, IcSimpleFsm_TempPair>  m_temp_map; IcRecurMutex m_locker;
    int  m_all_incr_cnt;
protected:
    IcSimpleFsm_Temp*   createTemp( const QMetaObject &mo );
public :
    explicit IcSimpleFsm_TempMgr( )   { m_all_incr_cnt = 0; }
    virtual ~IcSimpleFsm_TempMgr( );
    void                incrAllTemp( );
    void                decrAllTemp( );
    IcSimpleFsm_Temp*   attatchTemp( const QMetaObject& );
    void                detachTemp ( IcSimpleFsm_Temp* );
};

// ============================================================================
// dtor
// ============================================================================
IcSimpleFsm_TempMgr :: ~IcSimpleFsm_TempMgr ( )
{
    QMap<QString,IcSimpleFsm_TempPair>::const_iterator c_itr = m_temp_map.constBegin();
    while ( c_itr != m_temp_map.constEnd()) {
        const IcSimpleFsm_TempPair &pair = c_itr.value(); ++ c_itr;
        if ( pair.second != Q_NULLPTR ) {
            qxpack_ic_delete( pair.second, IcSimpleFsm_Temp );
        }
    }
    m_temp_map = QMap<QString,IcSimpleFsm_TempPair>();
}

// ============================================================================
// create the template
// ============================================================================
IcSimpleFsm_Temp *   IcSimpleFsm_TempMgr :: createTemp( const QMetaObject &meta_obj )
{
    IcSimpleFsm_Temp  *temp = qxpack_ic_new( IcSimpleFsm_Temp );
    temp->objClassNameRef() = meta_obj.className();

    int mc = meta_obj.methodCount();
    for ( int x = meta_obj.methodOffset(); x < mc; x ++ ) {
        QMetaMethod mm = meta_obj.method( x );

        // --------------------------------------------------------------------
        // the method should be below:
        // a) void onFsm_StateName_Event_StateName ( const QVariantList& )
        // b) void onFsm_State_Enter (void)
        // c) void onFsm_State_Leave (void)
        // d) void onFsm_State_Routine ( const QVariantList & )
        // --------------------------------------------------------------------
        if ( mm.methodType() != QMetaMethod::Slot && mm.methodType() != QMetaMethod::Method )
        { continue; }
        QString mn = mm.name();
        if ( ! mn.startsWith("onFsm_")) { continue; }
        QString sec1 = mn.section('_',1,1 );
        QString sec2 = mn.section('_',2,2 );
        QString sec3 = mn.section('_',3,3 );
        if ( sec1.isEmpty() || sec2.isEmpty()) { continue; }

        // --------------------------------------------------------------------
        // check: void onFsm_State_Enter()
        // --------------------------------------------------------------------
        if ( sec3.isEmpty()  &&  sec2 == QStringLiteral("Enter") &&
             mm.returnType() == QMetaType::Void  &&  mm.parameterCount() == 0
        ) {
            temp->stateEnterMapRef().insert( sec1, mm );
            temp->stateNameMapRef().insert ( sec1, true );
            continue;
        }

        // --------------------------------------------------------------------
        // check: void onFsm_State_Leave()
        // --------------------------------------------------------------------
        if ( sec3.isEmpty()  &&  sec2 == QStringLiteral("Leave") &&
             mm.returnType() == QMetaType::Void  &&  mm.parameterCount() == 0
        ) {
            temp->stateLeaveMapRef().insert( sec1, mm  );
            temp->stateNameMapRef().insert ( sec1, true );
            continue;
        }

        // --------------------------------------------------------------------
        // check: void onFsm_State_Routine( const QVariantList&)
        // --------------------------------------------------------------------
        if ( sec3.isEmpty()  &&  sec2 == QStringLiteral("Routine") &&
             mm.returnType() == QMetaType::Void  &&  mm.parameterCount() == 1 &&
             mm.parameterType(0) == QMetaType::QVariantList
        ) {
            temp->stateRoutineMapRef().insert( sec1, mm );
            temp->stateNameMapRef().insert( sec1, true );
            continue;
        }

        // --------------------------------------------------------------------
        // check: event side
        // --------------------------------------------------------------------
        if ( ! sec3.isEmpty()  &&  mm.returnType() == QMetaType::Bool  &&
             mm.parameterCount() == 1  &&  mm.parameterType(0) == QMetaType::QVariantList
        ) {
            IcSimpleFsm_Item item;
            item.m_src_state = sec1;
            item.m_evt       = sec2;
            item.m_dst_state = sec3;
            temp->stateEvetStateMapRef().insert( item, mm );
            temp->stateNameMapRef().insert( sec1, true );
        }
    }

    return temp;
}

// ============================================================================
// attach template, if not exist, create one by object
// ============================================================================
IcSimpleFsm_Temp *   IcSimpleFsm_TempMgr :: attatchTemp ( const QMetaObject &mo )
{
    IcSimpleFsm_Temp *temp = Q_NULLPTR;
    m_locker.lock();

    QMap<QString,IcSimpleFsm_TempPair>::iterator itr = m_temp_map.find( mo.className());
    if ( itr != m_temp_map.end()) {
        // --------------------------------------------------------------------
        // find the item, incr. the reference counter
        // --------------------------------------------------------------------
        IcSimpleFsm_TempPair &pair = itr.value();
        pair.first ++;
        temp = pair.second;

    } else {
        // --------------------------------------------------------------------
        // not found. build one
        // --------------------------------------------------------------------
        IcSimpleFsm_TempPair pair;
        pair.first  = m_all_incr_cnt + 1; // IMPORTANT: the new created object ref. should be based on m_all_incr_cnt
        pair.second = this->createTemp( mo );
        m_temp_map.insert( mo.className(), pair );
        temp = pair.second;
    }

    m_locker.unlock();
    return temp;
}

// ============================================================================
// detach template
// ============================================================================
void   IcSimpleFsm_TempMgr :: detachTemp ( IcSimpleFsm_Temp *temp )
{
    if ( temp == Q_NULLPTR ) { return; }
    m_locker.lock();

    QMap<QString,IcSimpleFsm_TempPair>::iterator itr = m_temp_map.find( temp->objClassNameRef());
    if ( itr != m_temp_map.end()) {
        IcSimpleFsm_TempPair &pair = itr.value();
        if ( -- pair.first < 1 ) {   // no other object reference it
            qxpack_ic_delete( pair.second, IcSimpleFsm_Temp );
            m_temp_map.erase( itr ); // drop this item
        }
    } else {
        qFatal("can not find '%s' template", temp->objClassNameRef().toUtf8().constData());
    }

    m_locker.unlock();
}

// =============================================================================
// increase all template counter
// =============================================================================
void  IcSimpleFsm_TempMgr :: incrAllTemp()
{
    m_locker.lock();

    if ( m_temp_map.size() > 0 ) {
        QMap<QString,IcSimpleFsm_TempPair>::iterator itr = m_temp_map.begin();
        while ( itr != m_temp_map.end()) {
           IcSimpleFsm_TempPair &pair = itr.value(); ++ itr;
           ++ pair.first;
        }
        ++ m_all_incr_cnt;
    }
    m_locker.unlock();
}

// ============================================================================
// decrease all template counter
// ============================================================================
void  IcSimpleFsm_TempMgr :: decrAllTemp()
{
    m_locker.lock();

    if ( m_all_incr_cnt > 0  &&  m_temp_map.size() > 0 ) {
        QMap<QString,IcSimpleFsm_TempPair> tmp_map;
        QMap<QString,IcSimpleFsm_TempPair>::iterator itr = m_temp_map.begin();
        while ( itr != m_temp_map.end()) {
            IcSimpleFsm_TempPair &pair = itr.value();
            if ( -- pair.first < 1 ) { // free the counter zero template
                qxpack_ic_delete( pair.second, IcSimpleFsm_Temp );
            } else {                   // remember remain template
                tmp_map.insert( itr.key(), itr.value() );
            }
            ++ itr;
        }
        -- m_all_incr_cnt;
        m_temp_map = tmp_map;
    }

    m_locker.unlock();
}


// ////////////////////////////////////////////////////////////////////////////
//
//                 global template manager
//
// ////////////////////////////////////////////////////////////////////////////
static QMutex      g_locker;
static QAtomicInt  g_ref_cntr(0);
static QSharedPointer<IcSimpleFsm_TempMgr>  g_mgr_sp;

// ============================================================================
// get the template by meta object
// ============================================================================
static QSharedPointer<IcSimpleFsm_TempMgr>  gGetInstance( )
{
    g_locker.lock();
    if ( g_mgr_sp.data() == Q_NULLPTR ) {
       g_mgr_sp = QSharedPointer<IcSimpleFsm_TempMgr>(
           qxpack_ic_new( IcSimpleFsm_TempMgr ),
           []( IcSimpleFsm_TempMgr *obj ) {
               qxpack_ic_delete( obj, IcSimpleFsm_TempMgr );
           }
       );
       g_ref_cntr.storeRelease(1);
    } else {
       g_ref_cntr.fetchAndAddOrdered(1);
    }
    g_locker.unlock();

    return g_mgr_sp;
}

// ============================================================================
// free the instance
// ============================================================================
static void  gFreeInstance( )
{
    g_locker.lock();
    if ( g_mgr_sp.data() != Q_NULLPTR ) {
        if ( g_ref_cntr.fetchAndSubOrdered(1) - 1 == 0 ) {
            g_mgr_sp = QSharedPointer<IcSimpleFsm_TempMgr>();
        }
    }
    g_locker.unlock();
}


// ////////////////////////////////////////////////////////////////////////////
// private object
// ////////////////////////////////////////////////////////////////////////////
#define T_PrivPtr( o )  T_ObjCast( IcSimpleFsmPriv*, o )
class QXPACK_IC_HIDDEN  IcSimpleFsmPriv : public QObject {
    Q_OBJECT
private:
    QSharedPointer<IcSimpleFsm_TempMgr>  m_temp_mgr;
    IcSimpleFsm_Temp  *m_temp;  QObject *m_handle_obj;
    QString  m_curr_state, m_start_state, m_final_state, m_cancel_state;
    size_t   m_pending_go_cntr, m_pending_evt_cntr, m_pending_rt_cntr; // 20190710 added by nw
protected:
    Q_SLOT  void  handleObj_onDestroyed( QObject* ) { m_handle_obj = Q_NULLPTR; }
public :
    explicit IcSimpleFsmPriv( QObject*, QObject* );
    virtual ~IcSimpleFsmPriv( ) Q_DECL_OVERRIDE;

    inline QString &  currState( ) { return m_curr_state; }
    inline void  setStartState ( const QString &i ) { m_start_state = i; }
    inline void  setFinalState ( const QString &f ) { m_final_state = f; }
    inline void  setCancelState( const QString &c ) { m_cancel_state = c; }

    inline bool  isStateExisted( const QString &s )
    { return ( m_temp->stateNameMapRef().constFind( s ) != m_temp->stateNameMapRef().constEnd()); }
    inline bool  isStarted( ) const { return ! m_curr_state.isEmpty(); }
    inline bool  isFinished() const { return ! m_curr_state.isEmpty() &&  m_curr_state == m_final_state;  }
    inline bool  isCancelled()const { return ! m_curr_state.isEmpty() &&  m_curr_state == m_cancel_state; }

    // 20190710 added by nw
    inline size_t &  pendingGoCntrRef( ) { return m_pending_go_cntr;  }
    inline size_t &  pendingEvtCntrRef( ){ return m_pending_evt_cntr; }
    inline size_t &  pendingRtCntrRef( ) { return m_pending_rt_cntr;  }

    Q_INVOKABLE  void  handleStateGo     ( const QString &tgt );
    Q_INVOKABLE  void  handleStateRoutine( const QVariantList & par = QVariantList());
    Q_INVOKABLE  void  handleStateEvent  ( const QString &evt, const QVariantList & par = QVariantList());

    Q_INVOKABLE  void  start( )
    {
        QMetaObject::invokeMethod ( this, "started", Qt::QueuedConnection );
        QMetaObject::invokeMethod (
            this, "handleStateGo", Qt::QueuedConnection,
            Q_ARG( const QString &, m_start_state )
        );
    }

    Q_SIGNAL void started  ( );
    Q_SIGNAL void finished ( );
    Q_SIGNAL void cancelled( );
    Q_SIGNAL void unhandleEvent     ( const QString &evt_name, const QVariantList &par );
    Q_SIGNAL void untransitionEvent ( const QString &evt_name, const QVariantList &par );
    Q_SIGNAL void unexistedState    ( const QString &st );
};

// ============================================================================
// ctor
// ============================================================================
IcSimpleFsmPriv :: IcSimpleFsmPriv ( QObject *h, QObject *pa ) : QObject( pa )
{
    m_handle_obj = h;
    QObject::connect( h, SIGNAL(destroyed(QObject*)), this, SLOT(handleObj_onDestroyed(QObject*)));

    m_temp_mgr = gGetInstance();
    const QMetaObject *mo = h->metaObject();
    m_temp = m_temp_mgr->attatchTemp( *mo );

    m_pending_evt_cntr = m_pending_go_cntr = m_pending_rt_cntr = 0;
}

// ============================================================================
// dtor
// ============================================================================
IcSimpleFsmPriv :: ~IcSimpleFsmPriv( )
{
    if ( m_handle_obj != Q_NULLPTR ) {
        QObject::disconnect( m_handle_obj, Q_NULLPTR, this, Q_NULLPTR );
    }
    m_temp_mgr->detachTemp( m_temp );
    gFreeInstance();
}

// ============================================================================
// handle state go ( from a state to another state )
// ============================================================================
void  IcSimpleFsmPriv :: handleStateGo( const QString &tgt )
{
    if ( m_pending_go_cntr > 0 ) { -- m_pending_go_cntr; } // 20190710 added by nw
    if ( m_handle_obj == Q_NULLPTR ) { return; }

    if ( ! this->isStateExisted( tgt )) {
        qWarning("the target state is not existed.");
        emit this->unexistedState( tgt );
        return;
    }

    IcSimpleFsm_SelMap::const_iterator leave_itr = m_temp->stateLeaveMapRef().constFind( m_curr_state );
    IcSimpleFsm_SelMap::const_iterator enter_itr = m_temp->stateEnterMapRef().constFind( tgt );
    if ( leave_itr != m_temp->stateLeaveMapRef().constEnd()) {
        leave_itr.value().invoke( m_handle_obj );
    }
    m_curr_state = tgt;
    if ( enter_itr != m_temp->stateEnterMapRef().constEnd()) {
        enter_itr.value().invoke( m_handle_obj );
    }

    // check if is in last state
    if ( ! m_curr_state.isEmpty() ) {
        if ( m_curr_state == m_final_state ) {
            emit this->finished();
        }
        if ( m_curr_state == m_cancel_state ) {
            emit this->cancelled();
        }
    }
}

// ============================================================================
// handle state routine
// ============================================================================
void   IcSimpleFsmPriv :: handleStateRoutine( const QVariantList &par )
{
    // ------------------------------------------------------------------------
    // if user used a QTimer or async-thread, maybe a routine call posted before
    // the FSM event or go pending call. Here we need to drop these routine call.
    // ------------------------------------------------------------------------
    if ( m_pending_evt_cntr > 0 || m_pending_go_cntr > 0 ) {
        // 20190710 added by nw, we delay this call after the go or event
        bool c_ret = QMetaObject::invokeMethod(
            this, "handleStateRoutine", Qt::QueuedConnection,
            Q_ARG( const QVariantList &, par )
        );
        // if post failed, fix the counter.
        if ( ! c_ret ) { if ( m_pending_rt_cntr > 0 ) { -- m_pending_rt_cntr; }}
        return;
    }

    // 20190710 added by nw, normally do it...
    if ( m_pending_rt_cntr > 0 ) { -- m_pending_rt_cntr; }
    if ( m_handle_obj == Q_NULLPTR || m_curr_state.isEmpty() ) { return; }

    IcSimpleFsm_SelMap &rt_map = m_temp->stateRoutineMapRef();
    IcSimpleFsm_SelMap::const_iterator c_itr = rt_map.constFind( m_curr_state );
    if ( c_itr != rt_map.constEnd()) {
        c_itr.value().invoke (
            m_handle_obj, Q_ARG( const QVariantList &, par )
        );
    }
}

// ============================================================================
// handle state enter state
// ============================================================================
void  IcSimpleFsmPriv :: handleStateEvent( const QString &evt, const QVariantList &par )
{
    if ( m_pending_evt_cntr > 0 ) { -- m_pending_evt_cntr; }
    if ( m_handle_obj == Q_NULLPTR ) { return; }

    IcSimpleFsm_SesMap &ses_map = m_temp->stateEvetStateMapRef();

    IcSimpleFsm_Item   tgt_item;
    tgt_item.m_src_state = m_curr_state;
    tgt_item.m_evt       = evt;

    IcSimpleFsm_SesMap::const_iterator c_itr = ses_map.constFind( tgt_item );
    if ( c_itr != ses_map.constEnd()) {
        const QMetaMethod &mm = c_itr.value();
        bool c_ret = false;       
        mm.invoke( m_handle_obj, Q_RETURN_ARG( bool, c_ret ), Q_ARG( const QVariantList&, par ));
        if ( c_ret ) { // now handle the state go
            ++ m_pending_go_cntr;  // 20190710 by nw, here for fixed counter..
            this->handleStateGo ( c_itr.key().m_dst_state );
        } else {
            emit this->untransitionEvent ( evt, par );
        }
    } else {
        emit this->unhandleEvent ( evt, par );
    }
}


// ////////////////////////////////////////////////////////////////////////////
//
//                 wrapper API
//
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcSimpleFsm :: IcSimpleFsm ( QObject *pa ) : QObject( pa )
{
    m_obj = Q_NULLPTR;
}

// ============================================================================
// dtor
// ============================================================================
IcSimpleFsm :: ~IcSimpleFsm ( )
{
    if ( m_obj != Q_NULLPTR ) {
        T_PrivPtr( m_obj )->blockSignals( true );
        qxpack_ic_delete_qobj( T_PrivPtr( m_obj ));
    }
}

// ============================================================================
// check if it is finished
// ============================================================================
bool   IcSimpleFsm :: isFinished( ) const
{  return ( m_obj != Q_NULLPTR ? T_PrivPtr( m_obj )->isFinished() : false ); }

// ============================================================================
// check if it is started
// ============================================================================
bool   IcSimpleFsm :: isStarted( ) const
{ return ( m_obj != Q_NULLPTR ? T_PrivPtr( m_obj )->isStarted() : false ); }

// ============================================================================
// check if it is cancelled
// ============================================================================
bool   IcSimpleFsm :: isCancelled() const
{ return ( m_obj != Q_NULLPTR ? T_PrivPtr( m_obj )->isCancelled() : false ); }

// ============================================================================
// initialize by spec. object
// ============================================================================
bool  IcSimpleFsm :: init (QObject *h_obj, const QString &init_state, const QString &last_state )
{ return this->init( h_obj, init_state, last_state, QString() ); }

bool  IcSimpleFsm :: init (
    QObject *h_obj, const QString &init_state, const QString &final_state,
    const QString &cancel_state
) {
    // ------------------------------------------------------------------------
    // condition check
    // ------------------------------------------------------------------------
    if ( h_obj == Q_NULLPTR ) { return false; }
    if ( h_obj->thread() != this->thread()) {
        qFatal("the handle object should be same thread with IcSimpleFsm.");
    }

    // ------------------------------------------------------------------------
    // release old object
    // ------------------------------------------------------------------------
    if ( m_obj != Q_NULLPTR ) {
        T_PrivPtr( m_obj )->blockSignals(true);
        qxpack_ic_delete_qobj( T_PrivPtr( m_obj ) );
        m_obj = Q_NULLPTR;
    }

    // ------------------------------------------------------------------------
    // build new object
    // ------------------------------------------------------------------------
    m_obj = qxpack_ic_new_qobj( IcSimpleFsmPriv, h_obj, this );
    QObject::connect( T_PrivPtr( m_obj ), SIGNAL(started()),   this, SIGNAL(started()), Qt::QueuedConnection );
    QObject::connect( T_PrivPtr( m_obj ), SIGNAL(finished()),  this, SIGNAL(finished()), Qt::QueuedConnection );
    QObject::connect( T_PrivPtr( m_obj ), SIGNAL(cancelled()), this, SIGNAL(cancelled()), Qt::QueuedConnection );
    QObject::connect(
        T_PrivPtr( m_obj ), SIGNAL(unhandleEvent( const QString&, const QVariantList&)),
        this, SIGNAL(unhandleEvent( const QString &, const QVariantList &)), Qt::QueuedConnection
    );
    QObject::connect(
        T_PrivPtr( m_obj ), SIGNAL(unexistedState( const QString& )),
        this, SIGNAL(unexistedState( const QString &)), Qt::QueuedConnection
    );
    QObject::connect(
        T_PrivPtr( m_obj), SIGNAL(untransitionEvent( const QString&, const QVariantList&)),
        this, SIGNAL(untransitionedEvent( const QString&, const QVariantList&)), Qt::QueuedConnection
    );

    T_PrivPtr( m_obj )->setStartState( init_state  );
    T_PrivPtr( m_obj )->setFinalState( final_state );
    T_PrivPtr( m_obj )->setCancelState( cancel_state );

    return true;
}

// ============================================================================
// post start
// ============================================================================
bool   IcSimpleFsm :: postStart( )
{
    if ( m_obj != Q_NULLPTR ) {
        bool c_ret = QMetaObject::invokeMethod ( T_PrivPtr( m_obj ), "start", Qt::QueuedConnection );
        if ( c_ret ) {
            T_PrivPtr( m_obj )->pendingEvtCntrRef() = 0;
            T_PrivPtr( m_obj )->pendingGoCntrRef()  = 0;
            T_PrivPtr( m_obj )->pendingRtCntrRef()  = 0;
        }
        return true;
    } else {
        return false;
    }
}

// ============================================================================
// post a event to handle
// ============================================================================
bool   IcSimpleFsm :: postEvent( const QString &evt_name, const QVariantList &par)
{
    if ( ! evt_name.isEmpty() && m_obj != Q_NULLPTR ) {
        bool c_ret = QMetaObject::invokeMethod (
            T_PrivPtr( m_obj ), "handleStateEvent", Qt::QueuedConnection,
            Q_ARG( const QString &, evt_name ), Q_ARG( const QVariantList&, par )
        );
        // 20190710 added by nw
        if ( c_ret ) { T_PrivPtr( m_obj )->pendingEvtCntrRef() ++; }
        return c_ret;
    } else {
        return false;
    }
}

// ============================================================================
// directly go to another
// ============================================================================
bool   IcSimpleFsm :: postGo ( const QString &tgt_state )
{
    if ( ! tgt_state.isEmpty()  &&  m_obj != Q_NULLPTR ) {
        bool c_ret = QMetaObject::invokeMethod(
            T_PrivPtr( m_obj ), "handleStateGo", Qt::QueuedConnection,
            Q_ARG( const QString &, tgt_state )
        );
        // 20190710 added by new
        if ( c_ret ) { T_PrivPtr( m_obj )->pendingGoCntrRef() ++; }
        return c_ret;
    } else {
        return false;
    }
}

// ============================================================================
// do current state routine
// ============================================================================
bool   IcSimpleFsm :: postRoutine ( const QVariantList &par )
{
    if ( m_obj != Q_NULLPTR  &&  ! T_PrivPtr( m_obj )->currState().isEmpty() ) {
        // 20190710 fixed.. by nw.
        bool c_ret = false;
        if ( T_PrivPtr( m_obj )->pendingRtCntrRef() < 1 ) {
            c_ret = QMetaObject::invokeMethod(
                T_PrivPtr( m_obj ), "handleStateRoutine", Qt::QueuedConnection,
                Q_ARG( const QVariantList &, par )
            );
            if ( c_ret ) { T_PrivPtr( m_obj )->pendingRtCntrRef() ++; }
        }
        return c_ret;
    } else {
        return false;
    }
}




// ////////////////////////////////////////////////////////////////////////////
//
//                 template cache
//
// ////////////////////////////////////////////////////////////////////////////
#define T_CachePtr( o )   T_ObjCast( IcSimpleFsmTempCachePriv*, o )
class QXPACK_IC_HIDDEN  IcSimpleFsmTempCachePriv {
private:
    QSharedPointer<IcSimpleFsm_TempMgr>  m_temp_mgr_sp;
public :
    IcSimpleFsmTempCachePriv ( )
    {
        m_temp_mgr_sp = gGetInstance();
        m_temp_mgr_sp->incrAllTemp();
    }
    ~IcSimpleFsmTempCachePriv( )
    {
        m_temp_mgr_sp->decrAllTemp();
        gFreeInstance();
    }
    inline QSharedPointer<IcSimpleFsm_TempMgr> &  tempMgr( ) { return m_temp_mgr_sp; }
};

// ============================================================================
// ctor
// ============================================================================
IcSimpleFsmTempCache :: IcSimpleFsmTempCache ( QObject *pa ) : QObject( pa )
{
    m_obj = qxpack_ic_new( IcSimpleFsmTempCachePriv );
}

// ============================================================================
// dtor
// ============================================================================
IcSimpleFsmTempCache :: ~IcSimpleFsmTempCache ( )
{
    qxpack_ic_delete( m_obj, IcSimpleFsmTempCachePriv );
}

// ============================================================================
// register the type
// ============================================================================
void  IcSimpleFsmTempCache :: regTemplate( const QMetaObject &mo )
{
    // ------------------------------------------------------------------------
    // NOTE: attachTemp() will create one if not existed the template, and detachTemp()
    // will release the object. In CachePriv object we increased all template ref-counter,
    // so detachTemp() will not free the object.
    // ------------------------------------------------------------------------
    IcSimpleFsm_Temp *temp = T_CachePtr( m_obj )->tempMgr()->attatchTemp( mo );
    if ( temp != Q_NULLPTR ) {
        T_CachePtr( m_obj )->tempMgr()->detachTemp( temp );
    }
}


// ////////////////////////////////////////////////////////////////////////////
//
//                 cache object private object
//
// ////////////////////////////////////////////////////////////////////////////
static QMutex     g_cache_locker;
static QAtomicPointer<IcSimpleFsmTempCache>  g_cache_ptr;
static QAtomicInt g_cache_ref;

// ===========================================================================
// [ static ] get the cache instance
// ===========================================================================
IcSimpleFsmTempCache *   IcSimpleFsmTempCache :: getInstance()
{
    IcSimpleFsmTempCache * ptr = Q_NULLPTR;

    g_cache_locker.lock();
    if ( ( ptr = g_cache_ptr.loadAcquire()) == Q_NULLPTR ) {
        g_cache_ptr.storeRelease(( ptr = new IcSimpleFsmTempCache ));
        g_cache_ref.storeRelease(1);
    } else {
        g_cache_ref.fetchAndAddOrdered(1);
    }
    g_cache_locker.unlock();

    return ptr;
}

// ============================================================================
// [ static ] free the cache instance
// ============================================================================
void  IcSimpleFsmTempCache :: freeInstance()
{
    IcSimpleFsmTempCache *ptr = Q_NULLPTR;

    g_cache_locker.lock();
    if ( ( ptr = g_cache_ptr.loadAcquire()) != Q_NULLPTR ) {
        if ( g_cache_ref.fetchAndSubOrdered(1) - 1 < 1 ) {
            delete ptr;
            g_cache_ptr.storeRelease(Q_NULLPTR);
        }
    }
    g_cache_locker.unlock();
}


}
#include "qxpack_ic_simplefsm.moc"
#endif
