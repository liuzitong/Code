#ifndef QXPACK_IC_APPCTRLBASE_HXX
#define QXPACK_IC_APPCTRLBASE_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include "qxpack/indcom/afw/qxpack_ic_msgbus.hxx"
#include "qxpack/indcom/afw/qxpack_ic_objmgr.hxx"
#include "qxpack/indcom/afw/qxpack_ic_confirmbus.hxx"
#include "qxpack/indcom/afw/qxpack_ic_appsettingsbase.hxx"
#include <QObject>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief application controller base class
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcAppCtrlBase : public QObject {
    Q_OBJECT
public:
    //! ctor
    explicit IcAppCtrlBase( QObject *pa = Q_NULLPTR );

    //! dtor
    virtual ~IcAppCtrlBase( ) Q_DECL_OVERRIDE;

    //! execute the initalize process
    virtual void  doInit( );

    //! execute the deinitalize process
    virtual void  doDeinit( );

    //! thread of business thread
    virtual QThread*  businessThread( ) const;

    //! return the message bus object
    IcMsgBus *    msgBus( ) const;

    //! return the object manager object
    IcObjMgr *    objMgr( ) const;

    //! return application settings object
    IcAppSettingsBase *  appSettings( ) const;

    //! global instance map
    static IcAppCtrlBase*  instance( const QString &name );

    // nw: 2019/04/30 added
    IcConfirmBus *  cfmBus( ) const;

protected:
    //! query the instance name, this name must be unique.
    virtual QString  instanceName( ) const;

    // ========================================================================
    // initalize call order:
    // preInit() -- previously initalize routine
    // createSingleton_AppSettings() -- create global application settings
    // createSingleton_MsgBus() -- create global message bus object
    // createSingleton_ObjMgr() -- create global object manager
    // registerTypes_ObjMgr()   -- register global object types in object manager
    // postInit() -- finish initalize
    // ========================================================================
    virtual void       preInit( );
    virtual IcAppSettingsBase *  createSingleton_AppSettings( );
    virtual IcMsgBus*  createSingleton_MsgBus( );
    virtual IcObjMgr*  createSingleton_ObjMgr( );
    virtual void       registerTypes_ObjMgr  ( );
    virtual void       postInit( );

    // ========================================================================
    // deinitalize call order:
    // preDeinit() -- previously deinitalize routine
    // releaseSingleton_ObjMgr( )      -- release object manager
    // releaseSingleton_MsgBus()       -- release message bus
    // releaseSingleton_AppSettings( ) -- release applicatino settings
    // postDeinit( ) -- finish deinitalize
    // ========================================================================
    virtual void  preDeinit( );
    virtual void  releaseSingleton_ObjMgr( );
    virtual void  releaseSingleton_MsgBus( );
    virtual void  releaseSingleton_AppSettings( );
    virtual void  postDeinit( );

    // ========================================================================
    // nw: 2019/04/30 added:
    // new component about confirm bus..
    // ========================================================================
    virtual IcConfirmBus*  createSingleton_CfmBus( );
    virtual void           releaseSingleton_CfmBus( );

private:
    void *m_obj;
    Q_DISABLE_COPY( IcAppCtrlBase )
};


}

#endif
