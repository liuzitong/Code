// ============================================================================
// author: night wing
// date stamp: 20190523 1613
//  1) use qxpack_ic_new_qobj instead new QObject..
//  2) blockSignals while dtor calling.
// ============================================================================
#ifndef QXPACK_IC_MSGBUS_CXX
#define QXPACK_IC_MSGBUS_CXX

#include "qxpack_ic_msgbus.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/common/qxpack_ic_pimplprivtemp.hpp"
#include "qxpack/indcom/sys/qxpack_ic_workercreatortemp_priv.hpp"
#include "qxpack/indcom/sys/qxpack_ic_sysevtqthread_priv.hxx"
#include "qxpack/indcom/sys/qxpack_ic_recurmutex_p.hxx"

#include <QDebug>
#include <QtGlobal>
#include <QMap>
#include <QMutex>
#include <QMetaObject>
#include <QMetaMethod>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
// All objects in one group can publish message, and the message will
// be received by other objects execept sender
//
// ////////////////////////////////////////////////////////////////////////////
typedef QMap<QObject*,void*>  IcMsgBus_GroupMemberPair_Map;
class QXPACK_IC_HIDDEN IcMsgBus_Group : public QObject {
    Q_OBJECT
private:
    IcRecurMutex m_locker; IcMsgBus_GroupMemberPair_Map m_member_map;
protected:
    Q_SLOT void member_onDestoryed( QObject *o ) { this->rmv( o ); }
public :
    IcMsgBus_Group ( QObject *pa = Q_NULLPTR );
    virtual ~IcMsgBus_Group( ) Q_DECL_OVERRIDE;
    bool  add ( QObject* );
    bool  rmv ( QObject* );
    inline bool isEmpty( ) { m_locker.lock(); bool ret = m_member_map.isEmpty(); m_locker.unlock(); return ret; }
    Q_SIGNAL void message ( const QxPack::IcMsgBusPkg &pkg );
};

// ============================================================================
// ctor
// ============================================================================
IcMsgBus_Group :: IcMsgBus_Group ( QObject *pa )
               : QObject( pa )
{ }

// ============================================================================
// dtor
// ============================================================================
IcMsgBus_Group :: ~IcMsgBus_Group ( )
{
    m_locker.lock();

    IcMsgBus_GroupMemberPair_Map::const_iterator c_itr = m_member_map.constBegin();
    while ( c_itr != m_member_map.constEnd()) {
        QObject *obj = c_itr.key();
        if ( obj != Q_NULLPTR ) {
            QObject::disconnect( this, Q_NULLPTR, obj, Q_NULLPTR );
            QObject::disconnect( obj, Q_NULLPTR, this, Q_NULLPTR );
        }
        ++ c_itr;
    }

    m_locker.unlock();
}

// ============================================================================
// add a new member into group
// ============================================================================
bool  IcMsgBus_Group :: add ( QObject *m )
{
    if ( m == Q_NULLPTR ) { return false; }
    IcMsgBusPkg();        // ensure register the meta info into Qt5 meta system
    m_locker.lock();

    IcMsgBus_GroupMemberPair_Map::const_iterator c_itr = m_member_map.constFind( m );
    if ( c_itr == m_member_map.constEnd()) { // only add it if not in map
        QObject::connect(     // m::destoryed() -> this::onDestoryed()
            m,        SIGNAL(destroyed(QObject*)),
            this,     SLOT(member_onDestoryed(QObject*)),
            Qt::DirectConnection
        );
        QObject::connect(     // sig_item::message() -> m::IcMsgBus_onMsg()
            this,     SIGNAL(message(const QxPack::IcMsgBusPkg&)),
            m,        SLOT(IcMsgBus_onMsg(const QxPack::IcMsgBusPkg&)),
            Qt::QueuedConnection
        );
        m_member_map.insert( m, Q_NULLPTR );
    }

    m_locker.unlock();

    return true;
}

// ============================================================================
// remove a member from group
// ============================================================================
bool   IcMsgBus_Group :: rmv ( QObject *m )
{
    bool ret = false;
    if ( m == Q_NULLPTR ) { return ret; }

    m_locker.lock();
    IcMsgBus_GroupMemberPair_Map::const_iterator c_itr = m_member_map.constFind( m );
    if ( c_itr != m_member_map.constEnd()) {
        QObject::disconnect( m, Q_NULLPTR, this, Q_NULLPTR );
        QObject::disconnect( this, Q_NULLPTR, m, Q_NULLPTR );
        ret = ( m_member_map.remove( m ) != 0 );
    }
    m_locker.unlock();

    return ret;
}

// ////////////////////////////////////////////////////////////////////////////
//
//                  worker object
//
// ////////////////////////////////////////////////////////////////////////////
#define T_PrivPtr( o )  T_ObjCast( IcMsgBusWorker*, o )
class QXPACK_IC_HIDDEN  IcMsgBusWorker :
    public QObject, public IcPImplPrivTemp< IcMsgBusWorker >{
    Q_OBJECT
private:
    QMap<QString,IcMsgBus_Group*>  m_groups_map;
public :
    explicit IcMsgBusWorker( QObject *pa = Q_NULLPTR );
    IcMsgBusWorker( const IcMsgBusWorker & );
    virtual ~IcMsgBusWorker( ) Q_DECL_OVERRIDE;
    Q_INVOKABLE bool  add( QObject *, const QString & );
    Q_INVOKABLE bool  rmv( QObject *, const QString & );
    Q_INVOKABLE void  invokeMsg( const QxPack::IcMsgBusPkg &pkg );
};

// ============================================================================
// ctor
// ============================================================================
IcMsgBusWorker :: IcMsgBusWorker( QObject *pa ) : QObject( pa )
{

}

// ============================================================================
// ctor ( copy )
// ============================================================================
IcMsgBusWorker :: IcMsgBusWorker ( const IcMsgBusWorker& ) : QObject( Q_NULLPTR )
{
    qFatal("IcMsgBusWorker: this is not supported!");
}

// ============================================================================
// dtor
// ============================================================================
IcMsgBusWorker :: ~IcMsgBusWorker( )
{
    QMap<QString,IcMsgBus_Group*>::const_iterator c_itr = m_groups_map.constBegin();
    while ( c_itr != m_groups_map.constEnd()) {
        IcMsgBus_Group *group = c_itr.value();
        if ( group != Q_NULLPTR ) {
            group->blockSignals(true);
            qxpack_ic_delete_qobj( group );
        }
        ++ c_itr;
    }
}

// ============================================================================
// add the object into a specified group
// ============================================================================
bool   IcMsgBusWorker :: add( QObject *m, const QString &grp_name )
{
    bool is_added = false;
    if ( m == Q_NULLPTR || grp_name.isEmpty() ) { return is_added; }

    // add into group
    IcMsgBus_Group *group = Q_NULLPTR;
    QMap<QString,IcMsgBus_Group*>::const_iterator c_itr = m_groups_map.constFind( grp_name );
    if ( c_itr == m_groups_map.constEnd() ) {
        group = qxpack_ic_new_qobj( IcMsgBus_Group, Q_NULLPTR );
        is_added = group->add( m );
        if ( is_added ) {
            m_groups_map.insert( grp_name, group );
        } else {
            qxpack_ic_delete_qobj( group );
        }
    } else {
        group = c_itr.value();
        is_added = group->add( m );
    }

    return is_added;
}

// ============================================================================
// remove the object from specified group
// ============================================================================
bool  IcMsgBusWorker :: rmv ( QObject *m, const QString &grp_name )
{
    bool is_rmv = false;
//    if ( m != Q_NULLPTR || grp_name.isEmpty() ) { return is_rmv; }
    if ( m == Q_NULLPTR || grp_name.isEmpty() ) { return is_rmv; } // 2020/11/12 fixed.


    IcMsgBus_Group *group = Q_NULLPTR;
    QMap<QString,IcMsgBus_Group*>::iterator itr = m_groups_map.find( grp_name );
    if ( itr != m_groups_map.end()) {
        group  = itr.value();
        is_rmv = group->rmv( m );
        if ( group->isEmpty()) {
            m_groups_map.erase( itr );
            group->deleteLater();
        }
    }

    return is_rmv;
}

// ============================================================================
// inovke the message
// ============================================================================
void   IcMsgBusWorker :: invokeMsg( const QxPack::IcMsgBusPkg &pkg )
{
    if ( pkg.groupName().isEmpty() ) { return; }

    // try find the group by name
    IcMsgBus_Group *group = Q_NULLPTR;
    QMap<QString,IcMsgBus_Group*>::const_iterator grp_itr = m_groups_map.constFind( pkg.groupName() );
    if ( grp_itr == m_groups_map.constEnd() ) { return; }

    // emit th signals, except the object
    group = grp_itr.value();
    if ( group != Q_NULLPTR ) { emit group->message( pkg ); }
}


// ////////////////////////////////////////////////////////////////////////////
//
//                   keep the life-cycle of worker
//
// ////////////////////////////////////////////////////////////////////////////
#define T_Creator( o )   T_ObjCast( IcMsgBusWorkerCreator*, o )
class QXPACK_IC_HIDDEN  IcMsgBusWorkerCreator :
             public QObject, public IcWorkerCreatorTemp< IcMsgBusWorker >
{
    Q_OBJECT
private: IcSysEvtQThreadPriv m_sys_evt_thread;
public :
    IcMsgBusWorkerCreator ( ) : QObject( Q_NULLPTR )
    {
        this->moveToThread( m_sys_evt_thread.thread());
    }

    virtual ~IcMsgBusWorkerCreator( ) Q_DECL_OVERRIDE
    {
    }

    Q_INVOKABLE void createWorker ( )
    {
        this->deleteWorker();
        m_wkr = qxpack_ic_new_qobj( IcMsgBusWorker );
    }

    // ========================================================================
    //  user should use this to create a new creator that live in sys thread
    // ========================================================================
    static IcMsgBusWorkerCreator*  newCreator( )
    {
        IcMsgBusWorkerCreator *wkr = qxpack_ic_new_qobj( IcMsgBusWorkerCreator );
        if ( wkr->thread() != QThread::currentThread()) {
            QMetaObject::invokeMethod( wkr, "createWorker", Qt::BlockingQueuedConnection );
        } else {
            wkr->createWorker( );
        }
        return wkr;
    }

    // ========================================================================
    // delete the creator directly
    // ========================================================================
    static void  deleteCreator( IcMsgBusWorkerCreator *wkr_cr )
    {
        wkr_cr->deleteWorker();
        wkr_cr->deleteLater();
    }
};




// ////////////////////////////////////////////////////////////////////////////
//
//                        wrapper API of IcMsgBus
// <mem_cntr>: need to to count it due to has priv object
//
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcMsgBus :: IcMsgBus ( QObject *pa ) : QObject( pa )
{
    IcMsgBusWorkerCreator *wkr_creator = IcMsgBusWorkerCreator::newCreator();
    m_obj = wkr_creator;
}

// ============================================================================
// dtor
// ============================================================================
IcMsgBus :: ~IcMsgBus ( )
{
    IcMsgBusWorkerCreator::deleteCreator( T_Creator( m_obj ));
    m_obj = Q_NULLPTR;
}

// ============================================================================
// add new object item into group
// ============================================================================
bool   IcMsgBus :: add( QObject *obj, const QString &grp_name )
{
    // check object
    if ( obj == Q_NULLPTR || grp_name.isEmpty() ) { return false; }

    const QMetaObject *meta_obj = obj->metaObject();
    int mc = meta_obj->methodCount();
    bool is_found_slot = false;
    for ( int x = meta_obj->methodOffset(); x < mc; x ++ ) {
        QMetaMethod mm = meta_obj->method(x);
        if ( mm.methodType() == QMetaMethod::Slot ) {
            QString mn = QString::fromLatin1(mm.methodSignature());
            if ( mn == QStringLiteral("IcMsgBus_onMsg(QxPack::IcMsgBusPkg)"))
            {
                is_found_slot = true; break;
            }
        }
    }

    // if found the method, then this object can add into message group
    bool is_added = false;
    if ( is_found_slot ) {
        if ( QThread::currentThread() != T_Creator( m_obj )->worker_di()->thread() ) {
            QMetaObject::invokeMethod (
                T_Creator( m_obj )->worker_di(),
                "add", Qt::BlockingQueuedConnection,
                Q_RETURN_ARG( bool, is_added ),
                Q_ARG( QObject*, obj ), Q_ARG( const QString&, grp_name )
            );
        } else {
            is_added = T_Creator( m_obj )->worker_di()->add( obj, grp_name );
        }
    }

    return is_added;
}

// ============================================================================
// remove an existed object from group
// ============================================================================
bool   IcMsgBus :: rmv ( QObject *obj, const QString &grp_name )
{
    // check objects
    if ( obj == Q_NULLPTR || grp_name.isEmpty() ) { return false; }

    // remove from message group
    bool is_rmv = false;
    if ( QThread::currentThread() != T_Creator( m_obj )->worker_di()->thread()) {
        QMetaObject::invokeMethod(
            T_Creator( m_obj )->worker_di(),
            "rmv", Qt::BlockingQueuedConnection,
            Q_RETURN_ARG( bool, is_rmv ),
            Q_ARG( QObject*, obj ), Q_ARG( const QString &, grp_name )
        );
    } else {
        is_rmv = T_Creator( m_obj )->worker_di()->rmv( obj, grp_name );
    }

    return is_rmv;
}

// ============================================================================
// post the message, all member in the group can be recieved
// ============================================================================
void   IcMsgBus :: post ( const IcMsgBusPkg &pkg )
{
    if ( pkg.groupName().isEmpty()) { return; }

    QMetaObject::invokeMethod(
        T_Creator( m_obj )->worker_di(),
        "invokeMsg", Qt::QueuedConnection,
        Q_ARG( const QxPack::IcMsgBusPkg&, pkg )
    );
}

// ============================================================================
// post the message ( override )
// ============================================================================
void   IcMsgBus :: post(
    const QString &grp, const QString &msg, const QJsonObject &jo, const QVariantList &va
) {
    if ( grp.isEmpty() ) { return; }

    Q_UNUSED( va );
    IcMsgBusPkg pkg;
    pkg.setGroupName( grp );
    pkg.setMessage  ( msg );
    pkg.setJoParam  ( jo  );
    this->post( pkg );
}




// ////////////////////////////////////////////////////////////////////////////
//
//                     Message Package
//
// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
//  <mem_cntr>: count it due to used PIMPL
// ////////////////////////////////////////////////////////////////////////////
#define T_MsgPkgPriv( o )  T_ObjCast( IcMsgBusPkgPriv*, o)
class QXPACK_IC_HIDDEN IcMsgBusPkgPriv : public IcPImplPrivTemp< IcMsgBusPkgPriv > {
private:
    QString     m_group, m_msg;
    QJsonObject m_jo_param;
public :
    IcMsgBusPkgPriv ( ) {  }
    IcMsgBusPkgPriv ( const IcMsgBusPkgPriv &other )
    {
        m_group    = other.m_group; m_msg = other.m_msg;
        m_jo_param = other.m_jo_param;
    }
    virtual ~IcMsgBusPkgPriv( ) Q_DECL_OVERRIDE;

    inline void   setGroupName( const QString &nm ) { m_group = nm; }
    inline void   setMessage  ( const QString &msg) { m_msg   = msg;}
    inline void   setJoParam  ( const QJsonObject &jo ) { m_jo_param = jo; }
    inline QString      groupName( ) const { return m_group;    }
    inline QString      message  ( ) const { return m_msg;      }
    inline QJsonObject  joParam( )   const { return m_jo_param; }
};
IcMsgBusPkgPriv :: ~IcMsgBusPkgPriv ( ) { }


// ////////////////////////////////////////////////////////////////////////////
//               message package wrapper
// <mem_cntr>: need not to count it due to used priv PIMPL
// ////////////////////////////////////////////////////////////////////////////
static bool g_is_msg_bus_pkg_reg = false;
static void gIcMsgBusPkg_regInQt5( )
{
    if ( ! g_is_msg_bus_pkg_reg ) {
        qRegisterMetaType<QxPack::IcMsgBusPkg>("QxPack::IcMsgBusPkg");
        qRegisterMetaType<QxPack::IcMsgBusPkg>("QxPack::IcMsgBusPkg&");
        qRegisterMetaType<QxPack::IcMsgBusPkg>("const QxPack::IcMsgBusPkg&");
        g_is_msg_bus_pkg_reg = true;
    }
}

IcMsgBusPkg :: IcMsgBusPkg ( )
{
    m_obj = nullptr; gIcMsgBusPkg_regInQt5();
}

IcMsgBusPkg :: IcMsgBusPkg ( const IcMsgBusPkg &other )
{
    m_obj = nullptr; gIcMsgBusPkg_regInQt5();
    IcMsgBusPkgPriv::attach( &m_obj, const_cast<void**>( &other.m_obj ));
}

IcMsgBusPkg &  IcMsgBusPkg :: operator = ( const IcMsgBusPkg &other )
{
    IcMsgBusPkgPriv::attach( &m_obj, const_cast<void**>( &other.m_obj ));
    return *this;
}

    IcMsgBusPkg :: ~IcMsgBusPkg ( )
{
    if ( m_obj != nullptr ) {
        IcMsgBusPkgPriv::attach( & m_obj, nullptr );
        m_obj = nullptr;
    }
}

void    IcMsgBusPkg :: setGroupName( const QString &nm )
{
    IcMsgBusPkgPriv::instanceCow( & m_obj )->setGroupName( nm );
}

void    IcMsgBusPkg :: setMessage  ( const QString &msg )
{
    IcMsgBusPkgPriv::instanceCow( & m_obj )->setMessage( msg );
}

void    IcMsgBusPkg :: setJoParam  ( const QJsonObject &jo )
{
    IcMsgBusPkgPriv::instanceCow( & m_obj )->setJoParam( jo );
}

QString  IcMsgBusPkg :: groupName( ) const
{
    return ( m_obj != Q_NULLPTR ? T_MsgPkgPriv( m_obj )->groupName() : QString() );
}

QString  IcMsgBusPkg :: message( )  const
{
    return ( m_obj != Q_NULLPTR ? T_MsgPkgPriv( m_obj )->message() : QString() );
}

QJsonObject   IcMsgBusPkg :: joParam( ) const
{
    return ( m_obj != Q_NULLPTR ? T_MsgPkgPriv( m_obj )->joParam() : QJsonObject() );
}


// ////////////////////////////////////////////////////////////////////////////
//               confirm message package qobject wrapper
// ////////////////////////////////////////////////////////////////////////////
IcMsgBusPkgQObj :: IcMsgBusPkgQObj ( QObject *pa ) : QObject( pa )
{ m_ext = Q_NULLPTR; }

IcMsgBusPkgQObj :: ~IcMsgBusPkgQObj ( )
{ }

void IcMsgBusPkgQObj :: setContent( IcMsgBusPkg &pkg )
{
    m_pkg = pkg;
    QMetaObject::invokeMethod( this, "groupNameChanged",   Qt::QueuedConnection );
    QMetaObject::invokeMethod( this, "messageChanged",     Qt::QueuedConnection );
    QMetaObject::invokeMethod( this, "joParamChanged",     Qt::QueuedConnection );
}

IcMsgBusPkg  IcMsgBusPkgQObj :: content() const
{   return m_pkg; }

QString   IcMsgBusPkgQObj :: groupName( ) const
{ return m_pkg.groupName(); }

QString   IcMsgBusPkgQObj :: message( ) const
{ return m_pkg.message(); }

QJsonObject  IcMsgBusPkgQObj :: joParam( ) const
{ return m_pkg.joParam(); }

void    IcMsgBusPkgQObj :: setGroupName( const QString &nm )
{
    m_pkg.setGroupName( nm );
    QMetaObject::invokeMethod( this, "groupNameChanged", Qt::QueuedConnection );
}

void    IcMsgBusPkgQObj :: setMessage( const QString &m )
{
    m_pkg.setMessage( m );
    QMetaObject::invokeMethod( this, "messageChanged", Qt::QueuedConnection );
}

void    IcMsgBusPkgQObj :: setJoParam (const QJsonObject &jo )
{
    m_pkg.setJoParam( jo );
    QMetaObject::invokeMethod( this, "setJoParam", Qt::QueuedConnection );
}


}

#include "qxpack_ic_msgbus.moc"
#endif
