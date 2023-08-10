// ============================================================================
// author: night wing
// date stamp: 20191117 1330
//  1) re-design
// ============================================================================
// ============================================================================
// author: night wing
// date stamp: 20190523 1603
//  1) use qxpack_ic_new_qobj instead new QObject..
//  2) blockSignals while dtor calling.
// ============================================================================
#ifndef QXPACK_IC_CONFIRMBUS_CXX
#define QXPACK_IC_CONFIRMBUS_CXX

#include "qxpack_ic_confirmbus.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/common/qxpack_ic_pimplprivtemp.hpp"
#include "qxpack/indcom/sys/qxpack_ic_rmtobjcreator_priv.hxx"
#include "qxpack/indcom/sys/qxpack_ic_rmtobjdeletor_priv.hxx"
#include <QDebug>
#include <QtGlobal>
#include <QMap>
#include <QMutex>
#include <QMetaObject>
#include <QMetaMethod>
#include <QList>
#include <QVector>
#include <QPair>
#include <QSemaphore>
#include <QCoreApplication>

#include <cstdlib>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
// item object
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN  IcConfirmBusItem {
private:
    QObject *m_tgt; int m_ref_cntr;
public :
    explicit IcConfirmBusItem ( QObject *obj ) : m_tgt( obj ), m_ref_cntr(1) { }
    ~IcConfirmBusItem ( ) { }
    inline int       incrRef() { return ++ m_ref_cntr; }
    inline int       decrRef() { return -- m_ref_cntr; }
    inline int       refCntr() { return m_ref_cntr; }
    inline QObject*  tgtObj()  { return m_tgt; }
};

// ////////////////////////////////////////////////////////////////////////////
//
// group
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN  IcConfirmBusGroup : public QObject {
    Q_OBJECT
private:
    QString m_name; QList<void*>  m_item_list;

protected:
    Q_SLOT void  item_onDestroyed( QObject *obj )
    { qFatal("obj (%s) is destroyed before remove/using it!", obj->objectName().toUtf8().constData()); }

public :
    IcConfirmBusGroup( const QString &nm ) : QObject(Q_NULLPTR)
    { m_name = nm; }

    virtual ~IcConfirmBusGroup( ) Q_DECL_OVERRIDE;

    inline bool  isEmpty() { return m_item_list.isEmpty(); }
           bool  add ( QObject* );
           bool  rmv ( QObject* );
           bool  isItemExist( void* ); // check if item is existed or not
    const QList<void*>&  itemList() { return m_item_list; }
};

// ============================================================================
// dtor
// ============================================================================
IcConfirmBusGroup :: ~IcConfirmBusGroup ( )
{
    // ------------------------------------------------------------------------
    // make sure target object disconnected.
    // ------------------------------------------------------------------------
    QList<void*>::const_iterator citr = m_item_list.constBegin();
    while ( citr != m_item_list.constEnd()) {
        IcConfirmBusItem *m = reinterpret_cast<IcConfirmBusItem*>( *( citr ++ ) ); // HINT: n++, return the old n! *(n++) means get the old n value.
        QObject::disconnect( m->tgtObj(), Q_NULLPTR, this, Q_NULLPTR );
        qxpack_ic_delete( m, IcConfirmBusItem );
    }
    m_item_list = QList<void*>();
}

// ============================================================================
// add object in list if not exised.
// ============================================================================
bool                    IcConfirmBusGroup :: add ( QObject *obj )
{
    if ( obj == Q_NULLPTR ) { return false; }

    // ------------------------------------------------------------------------
    // add the object if not added
    // ------------------------------------------------------------------------
    bool is_found = false;
    QList<void*>::iterator itr = m_item_list.begin();
    while ( itr != m_item_list.end()) {
        IcConfirmBusItem *m = reinterpret_cast<IcConfirmBusItem*>( *( itr ++ ));
        if ( m->tgtObj() == obj ) { m->incrRef(); is_found = true; break; }
    }
    if ( ! is_found ) {
        IcConfirmBusItem *m = qxpack_ic_new( IcConfirmBusItem, obj );
        QObject::connect( obj, SIGNAL(destroyed(QObject*)), this, SLOT(item_onDestroyed(QObject*)));
        m_item_list.push_front( m ); // make it first
    }
    return true;
}

// ============================================================================
// remove object from map
// ============================================================================
bool                    IcConfirmBusGroup :: rmv ( QObject *obj )
{
    if ( obj == Q_NULLPTR ) { return false; }

    // ------------------------------------------------------------------------
    // remove the object if existed
    // ------------------------------------------------------------------------
    bool is_found = false;
    QList<void*>::iterator itr = m_item_list.begin();
    while ( itr != m_item_list.end()) {
        IcConfirmBusItem *m = reinterpret_cast<IcConfirmBusItem*>( *itr );
        if ( m->tgtObj() != obj ) { ++ itr; continue; }

        if ( m->decrRef() == 0 ) {
            QObject::disconnect( m->tgtObj(), Q_NULLPTR, this, Q_NULLPTR );
            qxpack_ic_delete( m, IcConfirmBusItem );
            m_item_list.erase( itr );
        }
        is_found = true;
        break;
    }

    return is_found;
}

// ============================================================================
// check if the item is existed or not
// ============================================================================
bool                    IcConfirmBusGroup :: isItemExist( void *m )
{
    bool is_found = false;
    QList<void*>::const_iterator citr = m_item_list.constBegin();
    while ( citr != m_item_list.constEnd()){
        if ( m == *( citr ++ )) { is_found = true; break; }
    }
    return is_found;
}



// ////////////////////////////////////////////////////////////////////////////
//
// private object of IcConfirmBus
//
// ////////////////////////////////////////////////////////////////////////////
#define T_BusPriv( o )   T_ObjCast( IcConfirmBusPriv*, o )
class QXPACK_IC_HIDDEN IcConfirmBusPriv : public QObject {
    Q_OBJECT
private:
    QMap<QString,void*>   m_grp_map;
protected:
    Q_INVOKABLE IcConfirmBusGroup*  findGroup_pr( const QString &grp_name )
    {
        QMap<QString,void*>::const_iterator citr = m_grp_map.constFind(grp_name);
        return ( citr != m_grp_map.constEnd() ? reinterpret_cast<IcConfirmBusGroup*>( citr.value()) : Q_NULLPTR );
    }

public :
    explicit IcConfirmBusPriv ( ) : QObject(Q_NULLPTR) { }
    virtual ~IcConfirmBusPriv ( ) Q_DECL_OVERRIDE;
    Q_INVOKABLE  bool  add ( QObject*, const QString& );
    Q_INVOKABLE  bool  rmv ( QObject*, const QString& );
                 void  reqCfm_UI_UI( IcConfirmBusPkg& );

    Q_INVOKABLE  bool  callItem_BG_UI_bc ( const QString &grp_name, void *item, QxPack::IcConfirmBusPkg& );
    Q_INVOKABLE  void  attachListByGroupName_BG_UI_bc( const QString &grp_name, void *ptr )
    {
        QList<void*> *sd_lst_ptr = reinterpret_cast<QList<void*>*>( ptr );
        IcConfirmBusGroup *grp = findGroup_pr( grp_name );
        if ( grp != Q_NULLPTR ) { *sd_lst_ptr = grp->itemList(); }
    }
};

// ============================================================================
// dtor
// ============================================================================
IcConfirmBusPriv :: ~IcConfirmBusPriv ( )
{
    // ------------------------------------------------------------------------
    // delete all group
    // ------------------------------------------------------------------------
    QMap<QString,void*>::const_iterator citr = m_grp_map.constBegin();
    while ( citr != m_grp_map.constEnd()) {
        IcConfirmBusGroup *grp =  reinterpret_cast<IcConfirmBusGroup*>(( citr ++ ).value());
        if ( grp != Q_NULLPTR ) { grp->deleteLater(); }
    }
    m_grp_map = QMap<QString,void*>();
}

// ============================================================================
// add object into group
// ============================================================================
bool                    IcConfirmBusPriv :: add ( QObject *obj, const QString &grp_name )
{
    // ------------------------------------------------------------------------
    // find spec. name group if existed
    // ------------------------------------------------------------------------
    IcConfirmBusGroup *grp = Q_NULLPTR;
    QMap<QString,void*>::const_iterator citr = m_grp_map.constFind( grp_name );
    if ( citr == m_grp_map.constEnd()) {   // not found, we create a group
        grp = new IcConfirmBusGroup( grp_name );
        m_grp_map.insert( grp_name, grp );
    } else {
        grp = reinterpret_cast<IcConfirmBusGroup*>( citr.value() );
    }

    return grp->add( obj ); // add the object
}

// ============================================================================
// remove object from group
// ============================================================================
bool                    IcConfirmBusPriv :: rmv ( QObject *obj, const QString &grp_name )
{
    // ------------------------------------------------------------------------
    // find the item that can be remove
    // ------------------------------------------------------------------------
    IcConfirmBusGroup *grp = Q_NULLPTR;
    QMap<QString,void*>::iterator itr = m_grp_map.find( grp_name );
    if ( itr != m_grp_map.end()) {
        grp = reinterpret_cast<IcConfirmBusGroup*>(itr.value());
        if ( grp->rmv( obj )) { // NOTE: the object must not be used when remove it.
            if ( grp->isEmpty() ) {
                grp->deleteLater();
                m_grp_map.erase( itr );
            }
        }
        return true;
    } else {
        return false;
    }
}

// ============================================================================
// require confirm UI call UI
// ============================================================================
void                    IcConfirmBusPriv :: reqCfm_UI_UI( IcConfirmBusPkg &pkg )
{
    IcConfirmBusGroup *curr_grp = this->findGroup_pr( pkg.groupName() );
    if ( curr_grp == Q_NULLPTR ) { return; }

    // ------------------------------------------------------------------------
    // make a shadow list so we can keep the correct order in future
    // ------------------------------------------------------------------------
    QList<void*> sd_lst = curr_grp->itemList();
    QList<void*>::const_iterator sd_citr = sd_lst.constBegin();
    if ( sd_citr == sd_lst.constEnd()) { return; }
    IcConfirmBusItem *item = reinterpret_cast<IcConfirmBusItem*>( *( sd_citr++ ) );

    do {
        // --------------------------------------------------------------------
        // call item slot
        // --------------------------------------------------------------------
        bool ret = QMetaObject::invokeMethod(
            item->tgtObj(), "IcConfirmBus_onReqConfirm", Qt::DirectConnection,
            Q_ARG( QxPack::IcConfirmBusPkg &, pkg )
        );
        if (! ret ) { qFatal("call obj(%s) IcConfirmBus_onReqConfirm() failed!", item->tgtObj()->objectName().toUtf8().constData()); }
        if ( pkg.isGotResult()) { break; } // we got a result, need not to continue

        // --------------------------------------------------------------------
        // handle: if the cfm object require do confirm later
        // --------------------------------------------------------------------
        if ( pkg.isGrabbed()) {
            QEventLoop evt; pkg.setEvtLoop( &evt ); evt.exec(); pkg.setEvtLoop(Q_NULLPTR);
            break;
        }

        // --------------------------------------------------------------------
        // fetch next item
        // --------------------------------------------------------------------
        IcConfirmBusGroup *curr_grp = this->findGroup_pr( pkg.groupName() );
        if ( curr_grp == Q_NULLPTR ) { break; } // not existed..
        item = Q_NULLPTR;
        while ( sd_citr != sd_lst.constEnd()) {
            IcConfirmBusItem *m = reinterpret_cast<IcConfirmBusItem*>( *( sd_citr ++ ));
            if ( curr_grp->isItemExist( m ) ) { item = m; break; }
        }

    } while ( item != Q_NULLPTR );
}

// ============================================================================
// call item from background thread
// ============================================================================
bool                    IcConfirmBusPriv :: callItem_BG_UI_bc (
    const QString &grp_name, void *item, IcConfirmBusPkg &pkg
) {
    // ------------------------------------------------------------------------
    // cond. check
    // ------------------------------------------------------------------------
    IcConfirmBusGroup *grp = findGroup_pr( grp_name );
    if ( grp == Q_NULLPTR  ||  ! grp->isItemExist( item )) { return false; }

    // ------------------------------------------------------------------------
    // call meta method of the item
    // ------------------------------------------------------------------------
    IcConfirmBusItem *m = reinterpret_cast<IcConfirmBusItem*>( item );
    bool ret = QMetaObject::invokeMethod (
        m->tgtObj(), "IcConfirmBus_onReqConfirm", Qt::DirectConnection,
        Q_ARG( QxPack::IcConfirmBusPkg &, pkg )
    );
    if (! ret ) {
        qFatal("call obj(%s) IcConfirmBus_onReqConfirm() failed!", m->tgtObj()->objectName().toUtf8().constData());
    }

    // ------------------------------------------------------------------------
    // check if grabbed, we need to ensure a semaphore
    // ------------------------------------------------------------------------
    if ( ! pkg.isGotResult()  &&  pkg.isGrabbed()) {
        pkg.ensureWaitSem();
    }

    return ret;
}



// ////////////////////////////////////////////////////////////////////////////
//
// IcConfirmBus
//
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcConfirmBus :: IcConfirmBus ( QObject *pa ) : QObject( pa )
{
   m_obj = IcRmtObjCreator::createObjInThread(
       QCoreApplication::instance()->thread(), [](void*)->QObject* {
           return new IcConfirmBusPriv;
       }, Q_NULLPTR, false
   );
}

// ============================================================================
// dtor
// ============================================================================
IcConfirmBus :: ~IcConfirmBus ( )
{
    if ( QThread::currentThread() == QCoreApplication::instance()->thread()) {
        delete T_BusPriv(m_obj);
    } else {
        IcRmtObjDeletor::deleteObjInThread(
            QCoreApplication::instance()->thread(), [](void*,QObject*obj) {
                if ( obj != Q_NULLPTR ) { delete obj; }
            }, Q_NULLPTR, T_BusPriv(m_obj), false
        );
    }
}

// ============================================================================
// add object into map group
// ============================================================================
bool                    IcConfirmBus :: add( QObject *obj, const QString &grp_name )
{
    if ( obj == Q_NULLPTR || grp_name.isEmpty() ) { return false; }
    if ( obj->thread() != QCoreApplication::instance()->thread() ) {
        qFatal("obj(%s) is not belong to UI thread.", obj->objectName().toUtf8().constData());
        return false;
    }

    // ------------------------------------------------------------------------
    // find method on the target qobject, if not found, means it is not a valid object
    // ------------------------------------------------------------------------
    const QMetaObject *meta_obj = obj->metaObject();
    int mc = meta_obj->methodCount();
    bool is_found_slot = false;
    for ( int x = meta_obj->methodOffset(); x < mc; x ++ ) {
        QMetaMethod mm = meta_obj->method(x);
        if ( mm.methodType() == QMetaMethod::Slot ) {
            QString mn = QString::fromLatin1(mm.methodSignature());
            if ( mn == QStringLiteral("IcConfirmBus_onReqConfirm(QxPack::IcConfirmBusPkg&)")) {
                is_found_slot = true; break;
            }
        }
    }
    if ( ! is_found_slot ) {
        qWarning("the obj(%s) has no IcConfirmBusPkg slot!", obj->objectName().toUtf8().constData());
        return false;
    }

    // ------------------------------------------------------------------------
    // add object into confirmbus, if already existed, will increase the refr. cntr
    // ------------------------------------------------------------------------
    bool is_added = true;
    QMetaObject::invokeMethod (
        T_BusPriv(m_obj), "add",
        ( QThread::currentThread() != T_BusPriv(m_obj)->thread() ?
          Qt::BlockingQueuedConnection : Qt::DirectConnection ),
        Q_RETURN_ARG( bool, is_added ), Q_ARG( QObject*, obj ), Q_ARG( const QString&, grp_name )
    );
    return is_added;
}

// ============================================================================
// remove object from message group
// ============================================================================
bool                    IcConfirmBus :: rmv( QObject *obj, const QString &grp_name )
{
    if ( obj == Q_NULLPTR || grp_name.isEmpty() ) { return false; }
    if ( obj->thread() != QCoreApplication::instance()->thread() ) {
        qFatal("obj(%s) is not belong to UI thread.", obj->objectName().toUtf8().constData());
        return false;
    }

    // ------------------------------------------------------------------------
    // remove object from confirmbus
    // ------------------------------------------------------------------------
    bool is_rmv = false;
    QMetaObject::invokeMethod (
        T_BusPriv(m_obj), "rmv",
        ( QThread::currentThread() != T_BusPriv(m_obj)->thread() ?
          Qt::BlockingQueuedConnection : Qt::DirectConnection ),
        Q_RETURN_ARG( bool, is_rmv ), Q_ARG( QObject*, obj ), Q_ARG( const QString&, grp_name )
    );
    return is_rmv;
}

// ============================================================================
// require confirm from UI
// ============================================================================
void                    IcConfirmBus :: reqConfirm( QxPack::IcConfirmBusPkg &pkg )
{
    IcConfirmBusPriv *priv = T_BusPriv(m_obj);

    if ( QThread::currentThread() == QCoreApplication::instance()->thread()) {
        priv->reqCfm_UI_UI( pkg );

    } else {
        bool c_ret = false;

        // --------------------------------------------------------------------
        // pick the shadow list of current group
        // --------------------------------------------------------------------
        QList<void*> sd_lst; void *list_ptr = & sd_lst;
        c_ret = QMetaObject::invokeMethod (
            priv, "attachListByGroupName_BG_UI_bc", Qt::BlockingQueuedConnection,
            Q_ARG( const QString, pkg.groupName() ), Q_ARG(void*, list_ptr )
        );
        if ( ! c_ret ) { qFatal("attach shadow list failed!"); }

        // --------------------------------------------------------------------
        // enum all handler in the list, test one by one
        // --------------------------------------------------------------------
        QList<void*>::const_iterator citr = sd_lst.constBegin();
        while ( citr != sd_lst.constEnd() ) {

            // ----------------------------------------------------------------
            // try call current item as handler
            // ----------------------------------------------------------------
            IcConfirmBusItem *m = reinterpret_cast<IcConfirmBusItem*>( *( citr ++ ) );
            bool ret = false;
            c_ret = QMetaObject::invokeMethod(
                priv, "callItem_BG_UI_bc", Qt::BlockingQueuedConnection,
                Q_RETURN_ARG( bool, ret ), Q_ARG( const QString&, pkg.groupName() ),
                Q_ARG( void*, m ), Q_ARG( QxPack::IcConfirmBusPkg &, pkg )
            );
            if ( ! c_ret ) { qFatal(" callItem_BG_UI_bc() failed!");  }
            if ( ! ret ) {  continue; } // UI tell us current item lost

            // ----------------------------------------------------------------
            // package is got result from UI, need not to continue
            // ----------------------------------------------------------------
            if ( pkg.isGotResult()) { break; }

            // ----------------------------------------------------------------
            // the UI thread grabbed this package, we need to wait
            // ----------------------------------------------------------------
            if ( pkg.isGrabbed()) {
                QSemaphore *sem = pkg.ensureWaitSem();
                if ( sem == Q_NULLPTR ) { qFatal("package is grabbed, but has no semaphore!"); }
                sem->acquire(1);
                break;
            }
        }
    }
}






// ////////////////////////////////////////////////////////////////////////////
//
//                   Confirm  Message Package
//  this object is a shared data, not the Copy On Write.
//
// ////////////////////////////////////////////////////////////////////////////
typedef void (* PkgRslCb )( void*, const QJsonObject & );
#define T_MsgPkgPriv( o )  T_ObjCast( IcConfirmBusPkgPriv*, o)
class QXPACK_IC_HIDDEN IcConfirmBusPkgPriv : public IcPImplPrivTemp< IcConfirmBusPkgPriv > {
private:
    QMutex      m_locker;   QString   m_group, m_msg;
    QJsonObject m_jo_param, m_jo_rsl; QAtomicPointer<void>  m_evt_ptr, m_sem_ptr;
    QVector<QPair<PkgRslCb,void*>>  m_rsl_cb_list;
    bool        m_is_got_rsl, m_grab_flag;
public :
    IcConfirmBusPkgPriv ( ) : m_evt_ptr(Q_NULLPTR), m_sem_ptr(Q_NULLPTR),
                              m_is_got_rsl(false), m_grab_flag(false)
    { }

    IcConfirmBusPkgPriv ( const IcConfirmBusPkgPriv & )
    {  qFatal("this function should not be called!"); }

    virtual ~IcConfirmBusPkgPriv( );

    inline QString &      groupNameRef() { return m_group; }
    inline QString &      messageRef()   { return m_msg;   }
    inline QJsonObject &  joParamRef()   { return m_jo_param; }
    inline QJsonObject &  joResultRef()  { return m_jo_rsl; }
    inline bool &         isGotRslRef()  { return m_is_got_rsl; }
    inline QMutex &       lockerRef()    { return m_locker; }
    inline bool &         grabFlagRef()  { return m_grab_flag; }
    inline QAtomicPointer<void>&  evtPtrRef() { return m_evt_ptr; }
    inline QAtomicPointer<void>&  semPtrRef() { return m_sem_ptr; }
    inline QVector<QPair<PkgRslCb,void*>>&  rslCbListRef() { return m_rsl_cb_list; }
};

IcConfirmBusPkgPriv :: ~IcConfirmBusPkgPriv ( )
{
    if ( m_sem_ptr.loadAcquire() != Q_NULLPTR )
    { qxpack_ic_delete( m_sem_ptr.loadAcquire(), QSemaphore ); }
}

// ============================================================================
// register package type into QT5 meta system
// ============================================================================
static bool g_is_cfm_msg_bus_pkg_reg = false;
static void gIcConfirmBusPkg_regInQt5( )
{
    if ( ! g_is_cfm_msg_bus_pkg_reg ) {
        qRegisterMetaType<QxPack::IcConfirmBusPkg>("QxPack::IcConfirmBusPkg");
        qRegisterMetaType<QxPack::IcConfirmBusPkg>("QxPack::IcConfirmBusPkg&");
       // qRegisterMetaType<QxPack::IcConfirmBusPkg>("const QxPack::IcConfirmBusPkg&");
        g_is_cfm_msg_bus_pkg_reg = true;
    }
}

// ============================================================================
// ctor
// ============================================================================
IcConfirmBusPkg :: IcConfirmBusPkg ( )
{
    m_obj = nullptr; gIcConfirmBusPkg_regInQt5();
}

IcConfirmBusPkg :: IcConfirmBusPkg ( const IcConfirmBusPkg &other )
{
    m_obj = nullptr; gIcConfirmBusPkg_regInQt5();
    IcConfirmBusPkgPriv::attach( &m_obj, const_cast<void**>( &other.m_obj ));
}

IcConfirmBusPkg &  IcConfirmBusPkg :: operator = ( const IcConfirmBusPkg &other )
{
    IcConfirmBusPkgPriv::attach( &m_obj, const_cast<void**>( &other.m_obj ));
    return *this;
}

IcConfirmBusPkg :: ~IcConfirmBusPkg ( )
{
    if ( m_obj != nullptr ) {
        IcConfirmBusPkgPriv::attach( & m_obj, nullptr );
    }
}

// ============================================================================
// group name access
// ============================================================================
void                    IcConfirmBusPkg :: setGroupName( const QString &nm )
{
    IcConfirmBusPkgPriv::buildIfNull(&m_obj)->lockerRef().lock();
    T_MsgPkgPriv(m_obj)->groupNameRef() = nm;
    T_MsgPkgPriv(m_obj)->lockerRef().unlock();
}

QString  IcConfirmBusPkg :: groupName( ) const
{ return ( m_obj != Q_NULLPTR ? T_MsgPkgPriv( m_obj )->groupNameRef() : QString() ); }

// ============================================================================
//  message access
// ============================================================================
void                    IcConfirmBusPkg :: setMessage  ( const QString &msg )
{
    IcConfirmBusPkgPriv::buildIfNull(&m_obj)->lockerRef().lock();
    T_MsgPkgPriv(m_obj)->messageRef() = msg;
    T_MsgPkgPriv(m_obj)->lockerRef().unlock();
}

QString  IcConfirmBusPkg :: message( )  const
{ return ( m_obj != Q_NULLPTR ? T_MsgPkgPriv( m_obj )->messageRef() : QString() );   }

// ============================================================================
// jo parameter access
// ============================================================================
void                    IcConfirmBusPkg :: setJoParam  ( const QJsonObject &jo )
{
    IcConfirmBusPkgPriv::buildIfNull(&m_obj)->lockerRef().lock();
    T_MsgPkgPriv(m_obj)->joParamRef() = jo;
    T_MsgPkgPriv(m_obj)->lockerRef().unlock();
}

QJsonObject  IcConfirmBusPkg :: joParam( ) const
{ return ( m_obj != Q_NULLPTR ? T_MsgPkgPriv( m_obj )->joParamRef() : QJsonObject() ); }

// ============================================================================
// result access
// ============================================================================
void                    IcConfirmBusPkg :: setResult   ( const QJsonObject &jo )
{
    IcConfirmBusPkgPriv::buildIfNull(&m_obj)->lockerRef().lock();
    IcConfirmBusPkgPriv *priv = T_MsgPkgPriv(m_obj);
    priv->joResultRef() = jo;
    priv->isGotRslRef() = true;
    priv->grabFlagRef() = false;

    // exit eventloop
    QEventLoop *evt_ptr = reinterpret_cast<QEventLoop*>(priv->evtPtrRef().loadAcquire());
    if ( evt_ptr != Q_NULLPTR ) {
        evt_ptr->quit();
        priv->evtPtrRef().storeRelease(Q_NULLPTR);
    }

    // semaphore release
    QSemaphore *sem_ptr = reinterpret_cast<QSemaphore*>(priv->semPtrRef().loadAcquire());
    if ( sem_ptr != Q_NULLPTR ) {
        sem_ptr->release(1);
    }

    // result callback handler
    QVector<QPair<PkgRslCb,void*>>::const_iterator citr = priv->rslCbListRef().constBegin();
    while ( citr != priv->rslCbListRef().constEnd()) {
        const QPair<PkgRslCb,void*> &item = *( citr ++ );
        if ( item.first != Q_NULLPTR ) { item.first( item.second, jo ); }
    }
    priv->rslCbListRef() = QVector<QPair<PkgRslCb,void*>>(); // clear it

    priv->lockerRef().unlock();
}

QJsonObject             IcConfirmBusPkg :: result ( ) const
{ return ( m_obj != Q_NULLPTR ? T_MsgPkgPriv( m_obj )->joResultRef() : QJsonObject() );  }

bool                    IcConfirmBusPkg :: isGotResult( ) const
{ return ( m_obj != Q_NULLPTR ? T_MsgPkgPriv( m_obj )->isGotRslRef() : false ); }

bool                    IcConfirmBusPkg :: isGrabbed() const
{ return ( m_obj != Q_NULLPTR ? T_MsgPkgPriv(m_obj)->grabFlagRef() : false ); }

bool                    IcConfirmBusPkg :: isEmpty() const
{ return ( m_obj == Q_NULLPTR ); }

// ============================================================================
// mark a flag to means the confirm message is got by target thread
// ============================================================================
bool                    IcConfirmBusPkg :: grab()
{
    bool ret = false;
    IcConfirmBusPkgPriv::buildIfNull(&m_obj)->lockerRef().lock();
    if ( ! T_MsgPkgPriv(m_obj)->grabFlagRef()) {
        ret = true; T_MsgPkgPriv(m_obj)->grabFlagRef() = true;
    }
    T_MsgPkgPriv(m_obj)->lockerRef().unlock();
    return ret;
}

// ============================================================================
// [ protected ] set the eventloop pointer into it
// ============================================================================
void                    IcConfirmBusPkg :: setEvtLoop( QEventLoop *evt )
{
    IcConfirmBusPkgPriv::buildIfNull(&m_obj)->lockerRef().lock();
    T_MsgPkgPriv(m_obj)->evtPtrRef().storeRelease(evt);
    T_MsgPkgPriv(m_obj)->lockerRef().unlock();
}

// ============================================================================
// [ protected ] ensure the semaphore
// ============================================================================
QSemaphore*             IcConfirmBusPkg :: ensureWaitSem( )
{
    IcConfirmBusPkgPriv::buildIfNull(&m_obj)->lockerRef().lock();
    IcConfirmBusPkgPriv *priv = T_MsgPkgPriv(m_obj);
    if ( priv->semPtrRef() == Q_NULLPTR ) {
        priv->semPtrRef().storeRelease(qxpack_ic_new( QSemaphore, 0 ));
    }
    QSemaphore *sem = reinterpret_cast<QSemaphore*>( priv->semPtrRef().loadAcquire() );
    priv->lockerRef().unlock();

    return sem;
}

// ============================================================================
// [ protected ]
// ============================================================================
void                    IcConfirmBusPkg :: addCbIfSetRsl(
    void (* cb )(void*, const QJsonObject & ), void *ctxt
) {
    IcConfirmBusPkgPriv::buildIfNull(&m_obj)->lockerRef().lock();
    IcConfirmBusPkgPriv *priv = T_MsgPkgPriv(m_obj);

    // check if callback already existed or not
    bool is_found = false;
    QVector<QPair<PkgRslCb,void*>>::const_iterator citr = priv->rslCbListRef().constBegin();
    while ( citr != priv->rslCbListRef().constEnd()) {
        const QPair<PkgRslCb,void*> &item = *( citr ++ );
        if ( item.first == cb  &&  item.second == ctxt ) { is_found = true; break; }
    }

    // append it, the callback will be called while setResult()
    if ( ! is_found ) {
        if ( priv->rslCbListRef().isEmpty()) { priv->rslCbListRef().reserve( 4 ); }
        priv->rslCbListRef().push_back( QPair<PkgRslCb,void*>( cb, ctxt ) );
    }

    priv->lockerRef().unlock();
}

// ============================================================================
// [ protected ] remove the callback from list
// ============================================================================
void                    IcConfirmBusPkg :: rmvCbIfSetRsl (
    void (* cb )(void*, const QJsonObject & ), void *ctxt
) {
    IcConfirmBusPkgPriv::buildIfNull(&m_obj)->lockerRef().lock();
    IcConfirmBusPkgPriv *priv = T_MsgPkgPriv(m_obj);

    QVector<QPair<PkgRslCb,void*>>::iterator citr = priv->rslCbListRef().begin();
    while ( citr != priv->rslCbListRef().end()) {
        const QPair<PkgRslCb,void*> &item = *citr;
        if ( item.first == cb  &&  item.second == ctxt ) { priv->rslCbListRef().erase(citr); break; }
        ++ citr;
    }

    priv->lockerRef().unlock();
}


// ////////////////////////////////////////////////////////////////////////////
//
//               confirm message package qobject wrapper
//
// ////////////////////////////////////////////////////////////////////////////
IcConfirmBusPkgQObj :: IcConfirmBusPkgQObj ( QObject *pa ) : QObject( pa )
{ m_ext = Q_NULLPTR; }

IcConfirmBusPkgQObj :: IcConfirmBusPkgQObj ( const IcConfirmBusPkg &pkg ) : QObject(Q_NULLPTR)
{
    m_ext = Q_NULLPTR;
    m_pkg = pkg;
    if ( ! m_pkg.isEmpty()) { m_pkg.addCbIfSetRsl( & IcConfirmBusPkgQObj::handleRslCb, this ); }
}

IcConfirmBusPkgQObj :: ~IcConfirmBusPkgQObj ( )
{ }

void                    IcConfirmBusPkgQObj :: setContent( const IcConfirmBusPkg &pkg )
{
    if ( ! m_pkg.isEmpty()) { m_pkg.rmvCbIfSetRsl( & IcConfirmBusPkgQObj::handleRslCb, this ); }
    m_pkg = pkg;
    if ( ! m_pkg.isEmpty()) { m_pkg.addCbIfSetRsl( & IcConfirmBusPkgQObj::handleRslCb, this ); }
}

IcConfirmBusPkg         IcConfirmBusPkgQObj :: content()   const { return m_pkg; }
QString                 IcConfirmBusPkgQObj :: groupName() const { return m_pkg.groupName(); }
QString                 IcConfirmBusPkgQObj :: message()   const { return m_pkg.message(); }
QJsonObject             IcConfirmBusPkgQObj :: joParam()   const { return m_pkg.joParam(); }
QJsonObject             IcConfirmBusPkgQObj :: result()    const { return m_pkg.result(); }
bool                    IcConfirmBusPkgQObj :: isGotResult() const { return m_pkg.isGotResult(); }
bool                    IcConfirmBusPkgQObj :: grab()  { return m_pkg.grab(); }

void    IcConfirmBusPkgQObj :: setGroupName( const QString &nm )
{
    m_pkg.setGroupName( nm );
    QMetaObject::invokeMethod( this, "groupNameChanged", Qt::QueuedConnection );
}

void    IcConfirmBusPkgQObj :: setMessage( const QString &m )
{
    m_pkg.setMessage( m );
    QMetaObject::invokeMethod( this, "messageChanged", Qt::QueuedConnection );
}

void    IcConfirmBusPkgQObj :: setJoParam (const QJsonObject &jo )
{
    m_pkg.setJoParam( jo );
    QMetaObject::invokeMethod( this, "setJoParam", Qt::QueuedConnection );
}

void    IcConfirmBusPkgQObj :: setResult ( const QJsonObject &r )
{
    m_pkg.setResult( r );
}

// ============================================================================
// [ protected ] create a new instance about package
// ============================================================================
IcConfirmBusPkgQObj*    IcConfirmBusPkgQObj :: newInst( const IcConfirmBusPkg &pkg )
{
    return new IcConfirmBusPkgQObj( pkg );
}

// ============================================================================
// [ protected ] handle the result callback
// ============================================================================
void    IcConfirmBusPkgQObj :: handleRslCb( void *ctxt, const QJsonObject & )
{
    IcConfirmBusPkgQObj *t_this = reinterpret_cast<IcConfirmBusPkgQObj*>(ctxt);
    t_this->deleteLater();
}


}

#include "qxpack_ic_confirmbus.moc"
#endif
