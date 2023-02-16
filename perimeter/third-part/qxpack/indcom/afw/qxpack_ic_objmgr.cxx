// ============================================================================
// author: night wing
// date stamp: 20190523 1624
//  1) use qxpack_ic_new_qobj instead new QObject..
//  2) blockSignals while dtor calling.
// ============================================================================
#ifndef QXPACK_IC_OBJMGR_CXX
#define QXPACK_IC_OBJMGR_CXX

#include "qxpack_ic_objmgr.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/sys/qxpack_ic_rmtobjcreator_priv.hxx"
#include "qxpack/indcom/sys/qxpack_ic_rmtobjdeletor_priv.hxx"
#include "qxpack/indcom/sys/qxpack_ic_recurmutex_p.hxx"

#include <QMap>
#include <QMutex>
#include <QAtomicInt>
#include <QDebug>
#include <QVariant>
#include <QCoreApplication>
#include <QMetaMethod>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
//                  item of register object
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN IcObjMgr_RegObjItem : public QObject {
    Q_OBJECT
private:
    QAtomicInt   m_ref_cntr;  QString  m_name;
    QObject     *m_obj;       QThread *m_thread_affi;
    const QMetaObject *m_mo;  QVariantList m_last_va;
    IcRecurMutex  m_locker;    bool     m_is_with_ref;
protected:
    Q_SLOT void onThreadFinished();
    Q_SLOT void onThreadDeleted(QObject*);
public :
    // ========================================================================
    // ctor
    // ========================================================================
    IcObjMgr_RegObjItem (
        const QString &nm, QThread *t, const QMetaObject *mo, bool is_with_ref
    )
    { 
        m_name = nm;         m_thread_affi = t;
        m_obj  = Q_NULLPTR;  m_ref_cntr.storeRelease(0);
        m_mo   = mo;         m_is_with_ref = is_with_ref;

        // --------------------------------------------------------------------
        // monitor the thread, here use DirectConnection( directly call )
        // --------------------------------------------------------------------
        if ( t != Q_NULLPTR  &&  t != QCoreApplication::instance()->thread() ) {
            QObject::connect( t, SIGNAL(finished()), this, SLOT(onThreadFinished()), Qt::DirectConnection );
            QObject::connect( t, SIGNAL(destroyed(QObject*)), this, SLOT(onThreadDeleted(QObject*)), Qt::DirectConnection);
        }
    }

    // ========================================================================
    // dtor
    // ========================================================================
    virtual ~IcObjMgr_RegObjItem( ) Q_DECL_OVERRIDE
    {
        if ( m_ref_cntr.loadAcquire() != 0 ) {
            qFatal("[ IcObjMgr_RegObjItem ] the reference counter is not zero!" );
        }
        if ( m_thread_affi != Q_NULLPTR && m_thread_affi != QCoreApplication::instance()->thread()) {
            QObject::disconnect( m_thread_affi, Q_NULLPTR, this, Q_NULLPTR );
        }
    }

    // ========================================================================
    // methods
    // ========================================================================
    QObject*  attach( bool req_global, const QVariantList &va, IcObjMgr::SetObjOwnershipFunc func );
    void      detach( QObject *obj    );
};

// ============================================================================
// handle the thread finished
// ============================================================================
void   IcObjMgr_RegObjItem :: onThreadFinished( )
{
    qFatal("[ IcObjMgr_RegObjItem ] thread finished before '%s' deleted!", m_name.toLatin1().constData() );
}

// ============================================================================
// handle the thread deleted
// ============================================================================
void   IcObjMgr_RegObjItem :: onThreadDeleted(QObject *)
{
    qFatal("[ IcObjMgr_RegObjItem ] thread deleted before '%s' deleted!", m_name.toLatin1().constData() );
}

// ============================================================================
// attach a module, this function can create global instance and
// common instance
// ============================================================================
QObject *   IcObjMgr_RegObjItem :: attach (
    bool req_global, const QVariantList &va, IcObjMgr::SetObjOwnershipFunc func
) {
    QObject *cr_obj = Q_NULLPTR;

    if ( req_global ) { m_locker.lock(); }

    if ( req_global && m_obj != Q_NULLPTR ) {
        // --------------------------------------------------------------------
        // just increase the counter, req. global instance
        // --------------------------------------------------------------------
        m_ref_cntr.fetchAndAddOrdered(1);
        cr_obj = m_obj;
    } else {
        // --------------------------------------------------------------------
        // create one ( IcRmtObjCreator will handle the thread problem )
        // --------------------------------------------------------------------
        m_last_va = va;
        cr_obj = IcRmtObjCreator::createObjInThread (
            m_thread_affi,
            []( void *obj_ctxt )->QObject* {
                QxPack::IcObjMgr_RegObjItem *item = static_cast< QxPack::IcObjMgr_RegObjItem*>( obj_ctxt );
                if ( item->m_is_with_ref ) { // ctor( QVariantList& )
                    return item->m_mo->newInstance( Q_ARG( QVariantList&, item->m_last_va ));
                } else {                     // ctor( const QVariantList& )
                    return item->m_mo->newInstance( Q_ARG( const QVariantList&, item->m_last_va ));
                }
            }, this, false
        );
        m_last_va = QVariantList();

        // --------------------------------------------------------------------
        // cond.:
        // 1) req_global( true ), m_obj( false )
        // 2) req_global( false)
        // --------------------------------------------------------------------
        if ( req_global ) {
            m_ref_cntr.storeRelease(1);
            m_obj = cr_obj;
        } else {
            ;
        }

        if ( func != Q_NULLPTR ) {
            func( cr_obj, 0 );
        }
    }

    if ( req_global ) { m_locker.unlock(); }

    return cr_obj;
}

// ============================================================================
// detach module
// ============================================================================
void        IcObjMgr_RegObjItem :: detach( QObject *obj )
{
    // ------------------------------------------------------------------------
    // check if passed  pointer is global or not
    // ------------------------------------------------------------------------
    bool is_global_obj = false;
    if ( obj == Q_NULLPTR ) {
        is_global_obj = true; obj = m_obj;
    } else {
        if ( reinterpret_cast<qintptr>(obj) == reinterpret_cast<qintptr>(m_obj)) {
            is_global_obj = true;
        }
    }
    if ( obj == Q_NULLPTR ) { return; }

    if ( is_global_obj ) { m_locker.lock(); }

    // ------------------------------------------------------------------------
    // free the object
    // ------------------------------------------------------------------------
    bool is_req_del = true;
    if ( is_global_obj ) {
        // for global instance, here to check the reference counter
        is_req_del = ( m_ref_cntr.fetchAndSubOrdered(1) - 1 == 0 );
    }
    if ( is_req_del ) {
        if ( m_thread_affi == QThread::currentThread()) {
            delete obj;
        } else {
            IcRmtObjDeletor::deleteObjInThread (
                m_thread_affi,
                []( void*, QObject *del_obj ){
                    del_obj->deleteLater();
                }, this, obj, false
            );
        }
        if ( is_global_obj ) {
            m_obj = Q_NULLPTR; // clear it
        }
    }

    if ( is_global_obj ) { m_locker.unlock(); }
}

// ////////////////////////////////////////////////////////////////////////////
//
//                      private object
//
// ////////////////////////////////////////////////////////////////////////////
typedef QMap<QString,IcObjMgr_RegObjItem*> IcObjMgr_ObjMap;
#define T_PrivPtr( o )  T_PtrCast( IcObjMgrPriv*, o )
class QXPACK_IC_HIDDEN IcObjMgrPriv {
private:
    IcObjMgr_ObjMap  m_obj_map;
    IcRecurMutex     m_locker;
    IcObjMgr::SetObjOwnershipFunc  m_soos_func;
protected:
    IcObjMgr_RegObjItem*  findItemByName( const QString & );
    void  clearObjMap();
public :
    IcObjMgrPriv ( IcObjMgr::SetObjOwnershipFunc func );
    ~IcObjMgrPriv( );
    bool    registerType (
        const QString &name, QThread *tgt_thread, const QMetaObject &mm, bool is_with_ref
    );
    QObject* attachObj( const QString &name, bool is_global, const QVariantList &va );
    bool     detachObj( const QString &name, QObject *obj );
};

// ============================================================================
// ctor
// ============================================================================
    IcObjMgrPriv :: IcObjMgrPriv ( IcObjMgr::SetObjOwnershipFunc func )                   
{
   m_soos_func = func;
}

// ============================================================================
// dtor
// ============================================================================
    IcObjMgrPriv :: ~IcObjMgrPriv ( )
{
    this->clearObjMap();
}

// ============================================================================
// find the item by name
// ============================================================================
IcObjMgr_RegObjItem *   IcObjMgrPriv :: findItemByName( const QString &nm )
{
    IcObjMgr_RegObjItem *item = Q_NULLPTR;

    m_locker.lock();
    IcObjMgr_ObjMap::const_iterator c_itr = m_obj_map.constFind( nm );
    if ( c_itr != m_obj_map.constEnd()) {
        item = c_itr.value();
    }
    m_locker.unlock();

    return item;
}

// ============================================================================
// clear all object in the object map
// ============================================================================
void  IcObjMgrPriv :: clearObjMap ( )
{
    m_locker.lock();
    IcObjMgr_ObjMap::const_iterator c_itr = m_obj_map.constBegin();
    while( c_itr != m_obj_map.constEnd()) {
        IcObjMgr_RegObjItem *item = ( c_itr ++ ).value();
        if ( item != Q_NULLPTR ) { delete item; }
    }
    m_obj_map = IcObjMgr_ObjMap(); // drop old context
    m_locker.unlock();
}

// ============================================================================
// register a service
// ============================================================================
bool   IcObjMgrPriv :: registerType (
    const QString &obj_name, QThread *tgt_thread, const QMetaObject &mo, bool is_with_ref
)
{
    bool is_reg = false;

    // ------------------------------------------------------------------------
    // param. check
    // ------------------------------------------------------------------------
    if ( obj_name.isEmpty() ) { return false; }

    // ------------------------------------------------------------------------
    // try register into object map
    // ------------------------------------------------------------------------
    m_locker.lock();
    IcObjMgr_RegObjItem *item = this->findItemByName( obj_name );
    if ( item == Q_NULLPTR ) {
        item = new IcObjMgr_RegObjItem( obj_name, tgt_thread, &mo, is_with_ref );
        m_obj_map.insert( obj_name, item );
        is_reg = true;
    }
    m_locker.unlock();

    return is_reg;
}

// ============================================================================
// attach specified module by name
// ============================================================================
QObject*  IcObjMgrPriv :: attachObj( const QString &name, bool is_global, const QVariantList &va )
{
    QObject *obj = Q_NULLPTR;

    // ------------------------------------------------------------------------
    // global instance or common instance need the registered
    // module name
    // ------------------------------------------------------------------------
    if ( ! name.isEmpty()) {
        m_locker.lock();
        IcObjMgr_RegObjItem *item = this->findItemByName( name );
        m_locker.unlock();
        if ( item != Q_NULLPTR ) { obj = item->attach( is_global, va, m_soos_func ); }
    }

    return obj;
}

// ============================================================================
// detach sepcified module by name, if deleted or detached return true
// ============================================================================
bool      IcObjMgrPriv :: detachObj( const QString &name, QObject *obj )
{
    bool is_detached = false;

    // ------------------------------------------------------------------------
    // global instance or common instance need registered module
    // ------------------------------------------------------------------------
    if ( ! name.isEmpty()) {
        m_locker.lock();
        IcObjMgr_RegObjItem *item = this->findItemByName( name );
        m_locker.unlock();
        if ( item != Q_NULLPTR ) { item->detach( obj ); }
        is_detached = ( item != Q_NULLPTR ); // if existed item, the object must be deleted
    }

    return is_detached;
}

// ////////////////////////////////////////////////////////////////////////////
//
//        wrapper API
//
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcObjMgr :: IcObjMgr ( SetObjOwnershipFunc func,  QObject *pa )
  : QObject( pa )
{
    m_obj = qxpack_ic_new( IcObjMgrPriv, func );
}

// ============================================================================
// dtor
// ============================================================================
IcObjMgr :: ~IcObjMgr( )
{
    if ( m_obj != Q_NULLPTR ) {
        qxpack_ic_delete( m_obj, IcObjMgrPriv );
        m_obj = Q_NULLPTR;
    }
}

// ============================================================================
// register module by name
// ============================================================================
bool  IcObjMgr :: registerType (
    const QString &name, QThread *tgt_thread, const QMetaObject &mo
)
{
    int ctor_cnt = mo.constructorCount();

    // require constructor
    if ( ctor_cnt < 1 ) {
        qCritical("registerType() method require use Q_INVOKABLE before constructor!");
        return false;
    }

    // check constructor method has a QVariantList
    bool is_reg = false;
    for ( int x = 0; x < ctor_cnt; x ++ ) {
        QMetaMethod mm = mo.constructor(x);

        if ( mm.methodType() != QMetaMethod::Constructor && mm.parameterCount() != 1 )
        { continue; }

        if ( mm.methodSignature().contains("(QVariantList)")) {
            is_reg = T_PrivPtr( m_obj )->registerType( name, tgt_thread, mo, false );
        } else if ( mm.methodSignature().contains("(QVariantList&)") ) {
            is_reg = T_PrivPtr( m_obj )->registerType( name, tgt_thread, mo, true  );
        } else {
            ;
        }
    }

    return is_reg;
}

// ============================================================================
// attach to module by name
// ============================================================================
QObject*  IcObjMgr :: attachObj( const QString &name, bool req_global, const QVariantList &va )
{
    return T_PrivPtr( m_obj )->attachObj( name, req_global, va );
}

// ============================================================================
// detach service by name
// ============================================================================
bool      IcObjMgr :: detachObj( const QString &name, QObject *obj )
{
    return T_PrivPtr( m_obj )->detachObj( name, obj );
}


}
#include "qxpack_ic_objmgr.moc"
#endif
