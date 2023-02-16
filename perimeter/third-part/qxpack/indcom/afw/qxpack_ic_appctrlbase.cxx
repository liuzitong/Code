// ============================================================================
// author: night wing
// date stamp: 20190523 1320
// ============================================================================
#ifndef QXPACK_IC_APPCTRLBASE_CXX
#define QXPACK_IC_APPCTRLBASE_CXX

#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack_ic_appctrlbase.hxx"
#include <QThread>
#include <QMutex>
#include <QMap>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
//                     global instance getter
//
// ////////////////////////////////////////////////////////////////////////////
static QMutex  g_global_inst_locker;
static QMap<QString,IcAppCtrlBase*>  g_global_inst_map;

// ============================================================================
// add global instance
// ============================================================================
static void  gAddGlobalInst( const QString &nm, IcAppCtrlBase *app )
{
    if ( app != Q_NULLPTR  && ! nm.isEmpty()) {
        g_global_inst_locker.lock();
        g_global_inst_map.insert( nm, app );
        g_global_inst_locker.unlock();
    }
}

// ============================================================================
// remove the global instance
// ============================================================================
static void  gRmvGlobalInst( const QString &nm )
{
    if ( ! nm.isEmpty() ) {
        g_global_inst_locker.lock();
        g_global_inst_map.take( nm );
        g_global_inst_locker.unlock();
    }
}

// ============================================================================
// query the global instance by name
// ============================================================================
IcAppCtrlBase*  IcAppCtrlBase :: instance( const QString &name )
{
    IcAppCtrlBase *app = Q_NULLPTR;
    if ( ! name.isEmpty() ) {
        g_global_inst_locker.lock();
        QMap<QString,IcAppCtrlBase*>::const_iterator c_itr = g_global_inst_map.constFind( name );
        if ( c_itr != g_global_inst_map.constEnd()) {
            app = c_itr.value();
        }
        g_global_inst_locker.unlock();
    }
    return app;
}


// ////////////////////////////////////////////////////////////////////////////
//
//                      private object
// <mem_cntr>: managed by IcAppCtrlBase
// ////////////////////////////////////////////////////////////////////////////
#define T_PrivPtr( o )  T_ObjCast( IcAppCtrlBasePriv*, o )
class QXPACK_IC_HIDDEN IcAppCtrlBasePriv {
private:
    QThread        m_bussi_thread;
    IcAppCtrlBase *m_parent;
    IcMsgBus      *m_msg_bus;
    IcObjMgr      *m_obj_mgr;
    IcConfirmBus  *m_cfm_bus; // nw: 2019/04/30 added
    IcAppSettingsBase *m_app_set;
    bool      m_is_init;

    // nw: 2019/04/30 added
    bool  m_is_own_msg_bus, m_is_own_obj_mgr, m_is_own_cfm_bus, m_is_own_app_set;

public :
    IcAppCtrlBasePriv ( IcAppCtrlBase *pa )
    {
        m_parent = pa;
        m_msg_bus = Q_NULLPTR; m_obj_mgr = Q_NULLPTR; m_cfm_bus = Q_NULLPTR; m_app_set = Q_NULLPTR;
        m_is_init = false;
       // m_bussi_thread.setStackSize( 2048 * 1024 ); // 2MB stack size, for android 9.0 it is not supported.
        m_bussi_thread.start();

        m_is_own_msg_bus = m_is_own_obj_mgr = m_is_own_cfm_bus = m_is_own_app_set = false;    
    }
    ~IcAppCtrlBasePriv( )
    {
        m_bussi_thread.quit();
        m_bussi_thread.wait();

        if ( m_is_own_cfm_bus  &&  m_cfm_bus != Q_NULLPTR ) { delete m_cfm_bus; }
        if ( m_is_own_obj_mgr  &&  m_obj_mgr != Q_NULLPTR ) { delete m_obj_mgr; }
        if ( m_is_own_msg_bus  &&  m_msg_bus != Q_NULLPTR ) { delete m_msg_bus; }
        if ( m_is_own_app_set  &&  m_app_set != Q_NULLPTR ) { delete m_app_set; }
    }
    inline QThread* bussiThread()      { return &m_bussi_thread; }
    inline bool     isInit( )    const { return m_is_init; }
    inline IcMsgBus* msgBus( )   const { return m_msg_bus; }
    inline IcObjMgr* objMgr( )   const { return m_obj_mgr; }
    inline IcConfirmBus* cfmBus( ) const { return m_cfm_bus; }  // nw: 2019/04/30 added
    inline IcAppSettingsBase* appSettings( ) const { return m_app_set; }
    
    inline void  setIsInit( bool sw    )   { m_is_init = sw; }
    inline void  setMsgBus( IcMsgBus *o )  { m_msg_bus = o; m_is_own_msg_bus = false; }
    inline void  setObjMgr( IcObjMgr *o )  { m_obj_mgr = o; m_is_own_obj_mgr = false; }
    inline void  setCfmBus( IcConfirmBus *o ) { m_cfm_bus = o; m_is_own_cfm_bus = false; } // nw: 2019/04/30 added
    inline void  setAppSet( IcAppSettingsBase *o ) { m_app_set = o; }

    inline void ensureCompt ( ) // nw: 2019/04/30 added
    {
        if ( m_msg_bus == Q_NULLPTR ) { m_msg_bus = new IcMsgBus( Q_NULLPTR ); m_is_own_msg_bus = true; }
        if ( m_obj_mgr == Q_NULLPTR ) { m_obj_mgr = new IcObjMgr( Q_NULLPTR ); m_is_own_obj_mgr = true; }
        if ( m_cfm_bus == Q_NULLPTR ) { m_cfm_bus = new IcConfirmBus( Q_NULLPTR ); m_is_own_cfm_bus = true; } 
        if ( m_app_set == Q_NULLPTR ) { m_app_set = new IcAppSettingsBase( QString() ); m_is_own_app_set = true; }
    }    

    // nw: 2019/04/30 added
    inline bool isOwnMsgBus( ) const { return m_is_own_msg_bus; } 
    inline bool isOwnObjMgr( ) const { return m_is_own_obj_mgr; }
    inline bool isOwnCfmBus( ) const { return m_is_own_cfm_bus; }
    inline bool isOwnAppSet( ) const { return m_is_own_app_set; }
};

// ////////////////////////////////////////////////////////////////////////////
//                         wrapper API
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
    IcAppCtrlBase :: IcAppCtrlBase ( QObject *pa ) : QObject( pa )
{
    m_obj = qxpack_ic_new( IcAppCtrlBasePriv, this );
}

// ============================================================================
// dtor
// ============================================================================
    IcAppCtrlBase :: ~IcAppCtrlBase ( )
{
    if ( m_obj != Q_NULLPTR ) {
        if ( T_PrivPtr( m_obj )->isInit() ) {
            this->doDeinit();
        }
        qxpack_ic_delete( m_obj, IcAppCtrlBasePriv );
        m_obj = Q_NULLPTR;
        gRmvGlobalInst( this->instanceName() );
    }
}

// ============================================================================
// do the initalize call seq.
// ============================================================================
void   IcAppCtrlBase :: doInit( )
{
    if ( ! T_PrivPtr( m_obj )->isInit() ) {

        if ( IcAppCtrlBase::instance( this->instanceName() ) == Q_NULLPTR ) {
            gAddGlobalInst( this->instanceName(), this );
        }

        this->preInit();
        T_PrivPtr( m_obj )->setAppSet( this->createSingleton_AppSettings() );
        T_PrivPtr( m_obj )->setMsgBus( this->createSingleton_MsgBus() );
        T_PrivPtr( m_obj )->setObjMgr( this->createSingleton_ObjMgr() );
        T_PrivPtr( m_obj )->setCfmBus( this->createSingleton_CfmBus() );
        T_PrivPtr( m_obj )->ensureCompt( ); // create compt if some compt not created
        this->registerTypes_ObjMgr( );
        this->postInit();
        T_PrivPtr( m_obj )->setIsInit( true );
    }
}

// ============================================================================
// do the deinitalize call seq.
// ============================================================================
void    IcAppCtrlBase :: doDeinit( )
{
    if ( T_PrivPtr( m_obj )->isInit() ) {
        this->preDeinit();
        if ( T_PrivPtr( m_obj )->isOwnCfmBus( )) { this->releaseSingleton_CfmBus( ); }
        if ( T_PrivPtr( m_obj )->isOwnObjMgr( )) { this->releaseSingleton_ObjMgr( ); }
        if ( T_PrivPtr( m_obj )->isOwnMsgBus( )) { this->releaseSingleton_MsgBus( ); }
        if ( T_PrivPtr( m_obj )->isOwnAppSet( )) { this->releaseSingleton_AppSettings( ); }
        this->postDeinit();
        T_PrivPtr( m_obj )->setIsInit( false );
    }
}

// ============================================================================
// return the running business thread
// ============================================================================
QThread*  IcAppCtrlBase :: businessThread() const
{  return T_PrivPtr( m_obj )->bussiThread(); }

// ============================================================================
// access the message bus
// ============================================================================
IcMsgBus *  IcAppCtrlBase :: msgBus( ) const
{ return T_PrivPtr( m_obj )->msgBus(); }

// ============================================================================
// access the object manager
// ============================================================================
IcObjMgr *  IcAppCtrlBase :: objMgr() const
{ return T_PrivPtr( m_obj )->objMgr(); }

// ============================================================================
// access the application settings
// ============================================================================
IcAppSettingsBase *  IcAppCtrlBase :: appSettings() const
{ return T_PrivPtr( m_obj )->appSettings(); }

// ============================================================================
// access the confirm bus
// ============================================================================
IcConfirmBus *  IcAppCtrlBase :: cfmBus( ) const
{ return T_PrivPtr( m_obj )->cfmBus( ); }

// ============================================================================
// instance name
// ============================================================================
QString   IcAppCtrlBase :: instanceName() const 
{ return QString(); }

// ============================================================================
// default implement of the init/deinit seq.
// ============================================================================
void      IcAppCtrlBase :: preInit()  { return; }
IcAppSettingsBase* IcAppCtrlBase :: createSingleton_AppSettings() { return Q_NULLPTR; }
IcMsgBus* IcAppCtrlBase :: createSingleton_MsgBus() { return Q_NULLPTR; }
IcObjMgr* IcAppCtrlBase :: createSingleton_ObjMgr() { return Q_NULLPTR; }
void      IcAppCtrlBase :: registerTypes_ObjMgr  () { }
void      IcAppCtrlBase :: postInit() { }

void      IcAppCtrlBase :: preDeinit()  { }
void      IcAppCtrlBase :: releaseSingleton_ObjMgr() {  }
void      IcAppCtrlBase :: releaseSingleton_MsgBus() {  }
void      IcAppCtrlBase :: releaseSingleton_AppSettings() { }
void      IcAppCtrlBase :: postDeinit() { }

// nw: 2019/04/30 added
IcConfirmBus*  IcAppCtrlBase :: createSingleton_CfmBus() { return Q_NULLPTR; }
void           IcAppCtrlBase :: releaseSingleton_CfmBus(){ }



}

#endif
