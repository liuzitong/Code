// ============================================================================
// author: night wing
// date stamp: 20190522 1706
//  1) blockSignals while dtor calling.
// ============================================================================
#ifndef QXPACK_IC_SIMPLEACTCALLER_CXX
#define QXPACK_IC_SIMPLEACTCALLER_CXX

#include "qxpack_ic_simpleactcaller.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include <QMetaEnum>
#include <QThread>
#include <QMap>
#include <QtGlobal>
#include <QEventLoop>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
//         node object, used for describe the action network
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN IcSimpleActCaller_Node {
public :
    enum ActionType { ActionType_Switch = 0, ActionType_Trigger, ActionType_Rsvd };
private:
    QString      m_act;
    ActionType   m_type; IcSimpleActCaller::State  m_state;
    QMetaMethod  m_method;   QObject *m_method_obj;
    bool         m_is_freeze;
protected:
    bool   callMethod ( const QJsonObject &jo, const QVariantList &va, int, int );
public :
    IcSimpleActCaller_Node(
        const QString &act, const QMetaMethod &mm, QObject *mo, ActionType at
    );
    ~IcSimpleActCaller_Node( ) { }
    inline QString &   actNameRef( )       { return m_act;  }
    inline int         state   ( ) const   { return m_state;}
    inline ActionType  actionType( ) const { return m_type; }
    bool   doWork    ( const QJsonObject &jo, const QVariantList &va );
    bool   doFreeze  ( );
    bool   doUnFreeze( );
    bool   doCancel  ( );
};

// ============================================================================
// ctor
// ============================================================================
IcSimpleActCaller_Node :: IcSimpleActCaller_Node (
    const QString &act, const QMetaMethod &mm,  QObject *mo, ActionType at
) {
    m_act    = act;
    m_method = mm;   m_method_obj = mo;
    m_type   = at;   m_state = IcSimpleActCaller::State_Idle;
    m_is_freeze = false;
}

// ============================================================================
// call the method
// ============================================================================
bool  IcSimpleActCaller_Node :: callMethod (
    const QJsonObject &jo, const QVariantList &va, int c_st, int t_st
) {
    bool ret = false;
    if ( m_method.isValid() &&  m_method_obj != Q_NULLPTR ) {
        bool call_ret = m_method.invoke(
            m_method_obj, Qt::DirectConnection,
            Q_RETURN_ARG( bool, ret ), Q_ARG( const QJsonObject &, jo ),
            Q_ARG( const QVariantList &, va ), Q_ARG( int, c_st ), Q_ARG( int, t_st )
        );
        if ( ! call_ret ) { ret = false; }
    }
    return ret;
}

// ============================================================================
// do the work
// ============================================================================
bool   IcSimpleActCaller_Node :: doWork( const QJsonObject &jo, const QVariantList &va )
{
    bool ret = false;
    switch( m_type ) {
    case ActionType_Switch:
        if ( ! m_is_freeze  &&  m_state == IcSimpleActCaller::State_Idle ) {
            ret = this->callMethod ( jo, va, m_state, IcSimpleActCaller::State_Working );
            if ( ret ) { m_state = IcSimpleActCaller::State_Working; }
        }
        break;
    case ActionType_Trigger:
        if ( ! m_is_freeze  &&  m_state == IcSimpleActCaller::State_Idle ) {
            ret = this->callMethod ( jo, va, m_state, IcSimpleActCaller::State_Working );
        }
        break;
    default: break;
    }

    return ret;
}

// ============================================================================
// do the cancel
// ============================================================================
bool   IcSimpleActCaller_Node :: doCancel ( )
{
    bool ret = false;
    QJsonObject empty_jo; QVariantList empty_va;
    switch( m_type ) {
    case ActionType_Switch :
        if ( ! m_is_freeze  &&  m_state == IcSimpleActCaller::State_Working ) {
            ret = this->callMethod ( empty_jo, empty_va, m_state, IcSimpleActCaller::State_Idle );
            if ( ret ) { m_state = IcSimpleActCaller::State_Idle; }
        }
        break;
    case ActionType_Trigger :
        if ( ! m_is_freeze  &&  m_state == IcSimpleActCaller::State_Idle ) {
            ret = this->callMethod ( empty_jo, empty_va, m_state, IcSimpleActCaller::State_Idle );
            if ( ret ) { m_state = IcSimpleActCaller::State_Idle; }
        }
        break;
    default: break;
    }
    return ret;
}

// ============================================================================
// do the freeze the action
// ============================================================================
bool    IcSimpleActCaller_Node :: doFreeze ( )
{
    bool ret = false;
    QJsonObject empty_jo; QVariantList empty_va;

    switch ( m_type ) {
    case ActionType_Switch :
        if ( ! m_is_freeze ) {
            ret = this->callMethod ( empty_jo, empty_va, m_state, IcSimpleActCaller::State_Freeze );
            if ( ret ) { m_is_freeze = true; }
        }
        break;
    case ActionType_Trigger:
        if ( ! m_is_freeze ) {
            ret = this->callMethod ( empty_jo, empty_va, m_state, IcSimpleActCaller::State_Freeze );
            if ( ret ) { m_is_freeze = true; }
        }
        break;
    default: break;
    }
    return ret;
}

// ============================================================================
// unfree current state
// ============================================================================
bool     IcSimpleActCaller_Node :: doUnFreeze( )
{
    bool ret = false;
    QJsonObject empty_jo; QVariantList empty_va;

    switch ( m_type ) {
    case ActionType_Switch :
        if ( m_is_freeze ) {
            ret = this->callMethod ( empty_jo, empty_va, IcSimpleActCaller::State_Freeze, m_state );
            if ( ret ) { m_is_freeze = false; }
        }
        break;
    case ActionType_Trigger:
        if ( m_is_freeze ) {
            ret = this->callMethod ( empty_jo, empty_va, IcSimpleActCaller::State_Freeze, m_state );
            if ( ret ) { m_is_freeze = false; }
        }
        break;
    default: break;
    }
    return ret;
}

// ////////////////////////////////////////////////////////////////////////////
//
//              private object about simpleactcaller
//
// ////////////////////////////////////////////////////////////////////////////
typedef QMap<QString,IcSimpleActCaller_Node*>  IcSimpleActCaller_NodeMap;

#define T_PrivPtr( o )  T_ObjCast( IcSimpleActCallerPriv*, o )
class QXPACK_IC_HIDDEN  IcSimpleActCallerPriv : public QObject {
    Q_OBJECT
private:
    IcSimpleActCaller_NodeMap  m_node_map;
protected:
    IcSimpleActCaller_Node *  findNodeByActName( const QString &act );
public :
    IcSimpleActCallerPriv ( QObject* pa = Q_NULLPTR );
    virtual ~IcSimpleActCallerPriv( ) Q_DECL_OVERRIDE;
    Q_INVOKABLE bool  takeAndAddNode ( void* );
    Q_INVOKABLE void  clearNodeMap   ( );
    Q_INVOKABLE bool  doWork (
        const QString &act, const QJsonObject &jo, const QVariantList &va = QVariantList()
    );
    Q_INVOKABLE bool  doCancel  ( const QString &act );
    Q_INVOKABLE bool  doFreeze  ( const QString &act );
    Q_INVOKABLE bool  doUnfreeze( const QString &act );
    Q_INVOKABLE void  doBarrier ( ) { } // do-nothing
    Q_INVOKABLE void  doEventQuit ( QObject * );
    Q_SIGNAL    void  stateChanged( const QString &act, int curr_state, int target_state );
};

// ============================================================================
// ctor
// ============================================================================
IcSimpleActCallerPriv :: IcSimpleActCallerPriv ( QObject *pa ) : QObject(pa)
{
}

// ============================================================================
// dtor
// ============================================================================
IcSimpleActCallerPriv :: ~IcSimpleActCallerPriv ( )
{
    this->blockSignals( true ); // nw( 20190522 1708 ) added.
    this->clearNodeMap();
}

// ============================================================================
// do event quit
// ============================================================================
void  IcSimpleActCallerPriv :: doEventQuit( QObject *e )
{
    QEventLoop *evt = qobject_cast<QEventLoop*>( e );
    if ( evt != Q_NULLPTR ) {
        QMetaObject::invokeMethod( evt, "quit", Qt::QueuedConnection );
    }
}

// ============================================================================
// clear all node in the map
// ============================================================================
void  IcSimpleActCallerPriv :: clearNodeMap()
{
    IcSimpleActCaller_NodeMap::const_iterator c_itr = m_node_map.cbegin();
    while ( c_itr != m_node_map.cend()) {
        IcSimpleActCaller_Node *node = ( c_itr ++ ).value();
        if ( node != Q_NULLPTR ) {
            qxpack_ic_delete( node, IcSimpleActCaller_Node );
        }
    }
    m_node_map = IcSimpleActCaller_NodeMap();
}

// ============================================================================
// try find the node in the map
// ============================================================================
IcSimpleActCaller_Node *   IcSimpleActCallerPriv :: findNodeByActName( const QString &act )
{
    IcSimpleActCaller_NodeMap::const_iterator c_itr = m_node_map.constFind( act );
    if ( c_itr != m_node_map.cend()) {
        return c_itr.value();
    } else {
        return Q_NULLPTR;
    }
}

// ============================================================================
// add a node
// ============================================================================
bool  IcSimpleActCallerPriv :: takeAndAddNode( void *n )
{
    if ( n == Q_NULLPTR ) { return false; }

    IcSimpleActCaller_Node *node = static_cast< IcSimpleActCaller_Node*>(n);
    if ( this->findNodeByActName( node->actNameRef()) == Q_NULLPTR ) {
        m_node_map.insert( node->actNameRef(), node );
        return true;
    } else {
        return false;
    }
}

// ============================================================================
// find action and do work
// ============================================================================
bool  IcSimpleActCallerPriv :: doWork (
        const QString &act, const QJsonObject &jo, const QVariantList &va
    )
{
    bool ret = false;
    IcSimpleActCaller_Node *node = this->findNodeByActName( act );
    if ( node != Q_NULLPTR ) {
        int curr_st = node->state();
        if ( ( ret = node->doWork( jo, va )) &&  curr_st != node->state()) {
            emit this->stateChanged( act, curr_st, node->state() );
        }
    }
    return ret;
}

// ============================================================================
// find action and cancel
// ============================================================================
bool   IcSimpleActCallerPriv :: doCancel ( const QString &act )
{
    bool ret = false;
    IcSimpleActCaller_Node *node = this->findNodeByActName( act );
    if ( node != Q_NULLPTR ) {
        int curr_st = node->state();
        if ( ( ret = node->doCancel()) && curr_st != node->state()) {
            emit this->stateChanged( act, curr_st, node->state() );
        }
    }
    return ret;
}

// ============================================================================
// find action and freeze
// ============================================================================
bool    IcSimpleActCallerPriv :: doFreeze( const QString &act )
{
    bool ret = false;
    IcSimpleActCaller_Node *node = this->findNodeByActName( act );
    if ( node != Q_NULLPTR ) {
        int curr_st = node->state();
        if ( ( ret = node->doFreeze()) && curr_st != node->state()) {
            emit this->stateChanged( act, curr_st, node->state());
        }
    }
    return ret;
}

// ============================================================================
// find action and unfreeze
// ============================================================================
bool    IcSimpleActCallerPriv :: doUnfreeze( const QString &act )
{
    bool ret = false;
    IcSimpleActCaller_Node *node = this->findNodeByActName( act );
    if ( node != Q_NULLPTR ) {
        int curr_st = node->state();
        if ( ( ret = node->doUnFreeze()) && curr_st != node->state()) {
            emit this->stateChanged( act, curr_st, node->state());
        }
    }
    return ret;
}

// ////////////////////////////////////////////////////////////////////////////
// local functions
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// return a map start with a string ( Q_SLOT or Q_INVOKABLE )
// ============================================================================
static QMap<QByteArray,QMetaMethod>  gPickupMethodsStartWith(
    const QMetaObject *mo, const char *str
) {
    QMap<QByteArray,QMetaMethod> func_map;
    int cnt = mo->methodCount();
    for ( int x = 0; x < cnt; x ++ ) {
        QMetaMethod mm = mo->method( x );
        if ( mm.returnType() != QMetaType::Bool ) { continue; }
        if ( mm.methodType() != QMetaMethod::Slot &&
             mm.methodType() != QMetaMethod::Method )
        { continue; }
        QByteArray  ba = mm.name();
        if ( ba.startsWith(str) ) { func_map.insert( ba, mm ); continue; }
    }
    return func_map;
}

// ============================================================================
// add node by method map
// ============================================================================
static void  gAddNodeByFuncMap(
    QObject *mo, QMap<QByteArray,QMetaMethod> &me_map,
    IcSimpleActCallerPriv *obj, IcSimpleActCaller_Node::ActionType type
) {
    QMap<QByteArray,QMetaMethod>::const_iterator c_itr = me_map.constBegin();
    while ( c_itr != me_map.constEnd()) {
        QString sfx_name = QString( c_itr.key()).section( QChar('_'), 1, -1 );

        IcSimpleActCaller_Node *node = qxpack_ic_new( IcSimpleActCaller_Node,
            sfx_name, c_itr.value(), mo, type
        );
        if ( ! obj->takeAndAddNode( node )) { qxpack_ic_delete( node, IcSimpleActCaller_Node ); }

        ++ c_itr;
    }
}

// ////////////////////////////////////////////////////////////////////////////
// a event call jumper
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN IcSimpleActCaller_EvtJump : public QObject {
     Q_OBJECT
public:
     explicit IcSimpleActCaller_EvtJump( QObject *pa = Q_NULLPTR ) : QObject( pa ) { }
     virtual ~IcSimpleActCaller_EvtJump( ) Q_DECL_OVERRIDE { }
     Q_INVOKABLE void callEventQuit( QObject *tgt, QObject *evt )
     { QMetaObject::invokeMethod( tgt, "doEventQuit", Qt::QueuedConnection, Q_ARG( QObject*, evt )); }
};


// ////////////////////////////////////////////////////////////////////////////
// object wrapper
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcSimpleActCaller :: IcSimpleActCaller( QObject *pa ) : QObject( pa )
{
    m_obj = qxpack_ic_new( IcSimpleActCallerPriv, this );
    QObject::connect(
        T_PrivPtr( m_obj ), SIGNAL(stateChanged(const QString &,int,int)),
        this, SIGNAL(stateChanged(const QString &,int,int))
    );
}

// ============================================================================
// dtor
// ============================================================================
IcSimpleActCaller :: ~IcSimpleActCaller ( )
{
    QObject::disconnect( T_PrivPtr( m_obj ), Q_NULLPTR, this, Q_NULLPTR );
    qxpack_ic_delete( m_obj, IcSimpleActCallerPriv );
    m_obj = Q_NULLPTR;
}

// ============================================================================
// make a barrier to do
// ============================================================================
void   IcSimpleActCaller :: blockBarrier( bool req_evt_loop )
{
    if ( QThread::currentThread() != this->thread() ) {
        if ( req_evt_loop ) {
            // ----------------------------------------------------------------
            // the event will quit while process the queued call.
            // that means this barrier executed.
            // ----------------------------------------------------------------
            QEventLoop evt;
            IcSimpleActCaller_EvtJump  evt_jmp;
            evt_jmp.callEventQuit( T_PrivPtr( m_obj ), & evt );
            evt.exec( QEventLoop::ExcludeUserInputEvents );
        } else {
            // ----------------------------------------------------------------
            // here directly call doBarrier() without do eventloop
            // ----------------------------------------------------------------
            QMetaObject::invokeMethod (
                T_PrivPtr( m_obj ), "doBarrier", Qt::BlockingQueuedConnection
            );
        }
    } else {
        return; // nothing happen due to same thread!
    }
}

// ============================================================================
// build all nodes
// ============================================================================
bool   IcSimpleActCaller :: build( QObject *cb_obj, bool req_evt_loop )
{
    if ( cb_obj == Q_NULLPTR ) { return false; }

    // ------------------------------------------------------------------------
    // clear node
    // ------------------------------------------------------------------------
    this->blockBarrier( req_evt_loop );
    T_PrivPtr( m_obj )->clearNodeMap();

    // ------------------------------------------------------------------------
    // build function map
    // ------------------------------------------------------------------------
    QMap<QByteArray,QMetaMethod> trg_func = gPickupMethodsStartWith( cb_obj->metaObject(), "onTrg_");
    QMap<QByteArray,QMetaMethod> sw_func  = gPickupMethodsStartWith( cb_obj->metaObject(), "onSw_" );
    if ( trg_func.isEmpty() && sw_func.isEmpty() ) { return true; } // need not to do follown

    // ------------------------------------------------------------------------
    // add nodes
    // ------------------------------------------------------------------------
    gAddNodeByFuncMap( cb_obj, sw_func,  T_PrivPtr( m_obj ), IcSimpleActCaller_Node::ActionType_Switch );
    gAddNodeByFuncMap( cb_obj, trg_func, T_PrivPtr( m_obj ), IcSimpleActCaller_Node::ActionType_Trigger);

    return true;
}

// ============================================================================
// post a action call ( means working )
// ============================================================================
bool    IcSimpleActCaller :: post (
        const QString &act, const QJsonObject &jo, const QVariantList &va
) {
    // ------------------------------------------------------------------------
    // 1) caller's thread == obj's thread  ( queued )
    // 2) caller's thread != obj's thread  ( queued )
    // ------------------------------------------------------------------------
    return  QMetaObject::invokeMethod(
        T_PrivPtr( m_obj ), "doWork", Qt::QueuedConnection,
        Q_ARG( const QString&, act ), Q_ARG( const QJsonObject&, jo ),
        Q_ARG( const QVariantList&, va )
    );
}

// ============================================================================
// post a cancel to spec. action id
// ============================================================================
bool    IcSimpleActCaller :: postCancel ( const QString &act )
{
    return QMetaObject::invokeMethod(
         T_PrivPtr( m_obj ), "doCancel", Qt::QueuedConnection,
         Q_ARG( const QString &, act )
    );
}

// ============================================================================
// post a freeze to spec. action id
// ============================================================================
bool    IcSimpleActCaller :: postFreeze ( const QString &act )
{
    return QMetaObject::invokeMethod(
        T_PrivPtr( m_obj ), "doFreeze", Qt::QueuedConnection,
        Q_ARG( const QString &, act )
    );
}

// ============================================================================
// post a unfreeze to spec. action id
// ============================================================================
bool     IcSimpleActCaller :: postUnfreeze( const QString &act )
{
    return QMetaObject::invokeMethod(
        T_PrivPtr( m_obj ), "doUnfreeze", Qt::QueuedConnection,
        Q_ARG( const QString &, act )
    );
}

// ============================================================================
// send a action to do, and wait it finished
// ============================================================================
bool     IcSimpleActCaller :: send (
  const QString &act_name, const QJsonObject &jo, const QVariantList &ext_param
) {
    if ( QThread::currentThread() != this->thread()) {
        bool ret = this->post( act_name, jo, ext_param );
        this->blockBarrier( true );
        return ret;
    } else {
        return T_PrivPtr( m_obj )->doWork( act_name, jo, ext_param );
    }
}

// ============================================================================
// send the cancel action
// ============================================================================
bool     IcSimpleActCaller :: sendCancel ( const QString &act_name )
{
    if ( QThread::currentThread() != this->thread()) {
        bool ret = this->postCancel( act_name );
        this->blockBarrier( true );
        return ret;
    } else {
        return T_PrivPtr( m_obj )->doCancel( act_name );
    }
}

// ============================================================================
// send the freeze action
// ============================================================================
bool     IcSimpleActCaller :: sendFreeze ( const QString &act_name )
{
    if ( QThread::currentThread() != this->thread()) {
        bool ret = this->postFreeze( act_name );
        this->blockBarrier( true );
        return ret;
    } else {
        return T_PrivPtr( m_obj )->doFreeze( act_name );
    }
}

// ============================================================================
// send the unfreeze action
// ============================================================================
bool     IcSimpleActCaller :: sendUnfreeze( const QString &act_name )
{
    if ( QThread::currentThread() != this->thread()) {
        bool ret = this->postUnfreeze( act_name );
        this->blockBarrier( true );
        return ret;
    } else {
        return T_PrivPtr( m_obj )->doUnfreeze( act_name );
    }
}


}

#include "qxpack_ic_simpleactcaller.moc"
#endif
