﻿#ifndef PERIMETER_APPCTRL_HXX
#define PERIMETER_APPCTRL_HXX

#include "perimeter/base/common/perimeter_def.h"
#include "qxpack/indcom/afw/qxpack_ic_appctrlbase.hxx"
#include <QObject>
#include <QString>
#include "perimeter/main/database/databaseSvc.h"
#include <perimeter/main/services/translate_svc.h>


namespace Perimeter {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief  the application controller
 */
// ////////////////////////////////////////////////////////////////////////////
class PERIMETER_API  AppCtrl : public QxPack::IcAppCtrlBase {
    Q_OBJECT
    Q_PROPERTY( QObject*  objMgr READ  objMgrObj CONSTANT )
    Q_PROPERTY( QObject*  msgBus READ  msgBusObj CONSTANT )
    Q_PROPERTY( QObject*  databaseSvc READ  databaseSvcObj CONSTANT )
//    Q_PROPERTY( QObject*  currentPatient READ  getCurrentPatient WRITE setCurrentPatient NOTIFY currentPatientChanged)
    Q_PROPERTY( QObject*  testClass READ  getTestClass CONSTANT )
    Q_PROPERTY( QObject* checkSvc  READ getCheckSvc CONSTANT)
//    Q_PROPERTY( QObject* deviceOperation  READ getDeviceOperation CONSTANT)
    Q_PROPERTY( QObject* settings READ getSettings)
    Q_PROPERTY( QObject* utilitySvc READ getUtilitySvc)
    Q_PROPERTY( QObject* frameProvidSvc READ getFrameProvidSvc CONSTANT)
    Q_PROPERTY( QObject* deviceStatusData READ getDeviceStatusData NOTIFY deviceStatusDataChanged)
    Q_PROPERTY( bool showDeviceStatusData READ getShowDeviceStatusData NOTIFY showDeviceStatusDataChanged)

public:
    //! ctor
    explicit AppCtrl( QObject *pa = Q_NULLPTR );

    //! dtor
    virtual ~AppCtrl() Q_DECL_OVERRIDE;

    //! objMgrObj
    QObject*  objMgrObj( ) const;

    //! msgBusObj
    QObject*  msgBusObj( ) const;


    //Custom Code
    QObject*    databaseSvcObj() const;
    QObject*    getTestClass() const;
    QObject*    getAnalysisSvc() const;
    QObject*    getCheckSvc() const;
    QObject*    getSettings() ;
    QObject*    getFrameProvidSvc() const;
    QObject*    getUtilitySvc() const;
    QObject*    getDeviceStatusData() const;Q_SIGNAL void deviceStatusDataChanged();
    bool        getShowDeviceStatusData() const;Q_SIGNAL void showDeviceStatusDataChanged();
//    QObject*    getDeviceOperation() const;


//    Q_INVOKABLE void Quit();
//    QObject*    getCurrentPatient();    void setCurrentPatient(QObject* value);     Q_SIGNAL void  currentPatientChanged(QObject* value);
    //    Q_SIGNAL void  changePage(QString value);
public slots:
    void adjustWindowSize(QObject *win_obj);
protected:
    // override the name
    virtual QString   instanceName() const Q_DECL_OVERRIDE;

    // override the the initialization process
    virtual void      preInit( ) Q_DECL_OVERRIDE;
    virtual QxPack::IcAppSettingsBase* createSingleton_AppSettings( ) Q_DECL_OVERRIDE;
    virtual QxPack::IcMsgBus*  createSingleton_MsgBus( ) Q_DECL_OVERRIDE;
    virtual QxPack::IcObjMgr*  createSingleton_ObjMgr( ) Q_DECL_OVERRIDE;
    virtual void      registerTypes_ObjMgr( ) Q_DECL_OVERRIDE;
    virtual void      postInit( ) Q_DECL_OVERRIDE;

    // override the deinitialization process
    virtual void      preDeinit() Q_DECL_OVERRIDE;
    virtual void      releaseSingleton_ObjMgr( ) Q_DECL_OVERRIDE;
    virtual void      releaseSingleton_MsgBus( ) Q_DECL_OVERRIDE;
    virtual void      releaseSingleton_AppSettings( ) Q_DECL_OVERRIDE;
    virtual void      postDeinit() Q_DECL_OVERRIDE;

private:
    void *m_obj;
    Q_DISABLE_COPY( AppCtrl )
};

}

#endif 

