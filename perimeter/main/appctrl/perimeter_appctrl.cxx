﻿#ifndef PERIMETER_APPCTRL_CXX
#define PERIMETER_APPCTRL_CXX

#include "perimeter_appctrl.hxx"
#include "perimeter/base/service/perimeter_appsettingssvc.hxx"
#include "perimeter/base/service/perimeter_msgbussvc.hxx"
#include "perimeter/base/service/perimeter_objmgrsvc.hxx"
#include "perimeter/base/common/perimeter_guns.hxx"
#include "perimeter/base/common/perimeter_memcntr.hxx"
#include "perimeter/main/appctrl/testclass.h"
#include <QCoreApplication>
#include "perimeter/main/viewModel/settings.h"
#include "perimeter/main/viewModel/checkResultVm.h"
#include "perimeter/main/perimeter_main.hxx"
#include "perimeter/main/services/check_svc.h"
#include "perimeter/main/services/frame_provid_svc.h"
#include "perimeter/main/viewModel/deviceStatusDataVm.h"
#include <deviceOperation/device_operation.h>
#include "perimeter/main/services/utility_svc.h"
#include "perimeter/third-part/perm/perm_mod.hxx"
#include <perimeter/main/services/keyboard_filter.h>
#if defined( WIN32 )
#include <windows.h>
#endif
#include <QWindow>
#include <QScreen>

namespace Perimeter {

// ////////////////////////////////////////////////////////////////////////////
//    private objects
// ////////////////////////////////////////////////////////////////////////////
#define T_PrivPtr( o )  perimeter_objcast( AppCtrlPriv*, o )
class PERIMETER_HIDDEN AppCtrlPriv {
private:
    AppCtrl   *m_parent;
    QxPack::IcMsgBus *m_msg_bus_svc;
    QxPack::IcObjMgr *m_obj_mgr_svc;
    QxPack::IcAppSettingsBase *m_app_set_svc;
    // add other sub modules here
    Main        m_main_mod;
    //Custom Code
//    QString     m_language="Chinese";
//    bool        m_doubleName=false;
    QObject*    m_databaseSvc;
    QObject*    m_testClass;
    QObject*    m_settings;
    QObject*    m_checkSvc;
    QObject*    m_deviceStatusData;
//    QObject*    m_currentPatient;
public :
    AppCtrlPriv ( AppCtrl *pa );
    ~AppCtrlPriv( );
    inline QxPack::IcMsgBus*  msgBusSvc( ) const { return m_msg_bus_svc; }
    inline QxPack::IcObjMgr*  objMgrSvc( ) const { return m_obj_mgr_svc; }
    inline QxPack::IcAppSettingsBase*  appSetSvc( ) const { return m_app_set_svc; }
    void  registerTypes( );

    //Custom Code
    QObject*    getDatabaseSvcObj() const           {return m_databaseSvc;}
    QObject*    getTestClass() const                {return m_testClass;}
    QObject*    getCheckSvc() const                 {return m_checkSvc;}
    QObject*    getSettings()                       {return m_settings;}
    QObject*    getUtilitySvc()                      {return FrameProvidSvc::getSingleton().data();}
    QObject*    getDeviceStatusData()                    {return m_deviceStatusData;}
//    QObject*    getDeviceOperation()                {return DevOps::DeviceOperation::getSingleton().data();/*return new QObject();*/}
//    QString     getLanguage()                       {return m_language;}
//    void        setLanguage(QString value)          {m_language=value;}
//    bool        getDoubleName()                     {return m_doubleName;}
//    void        setDoubleName(bool value)           {m_doubleName=value;}

//    QObject*    getCurrentPatient()                 {return m_currentPatient;}
    //    void        setCurrentPatient(QObject *value)   {m_currentPatient=value;}
public slots:
    void adjustWindowSize(QObject *win_obj);
};

// ============================================================================
// ctor
// ============================================================================
AppCtrlPriv :: AppCtrlPriv ( AppCtrl *pa )
{
    m_parent = pa;
    m_app_set_svc = AppSettingsSvc::getInstance();
    m_msg_bus_svc = MsgBusSvc::getInstance();
    m_obj_mgr_svc = ObjMgrSvc::getInstance();

    m_databaseSvc = perimeter_new(databaseSvc);
    m_testClass = perimeter_new(TestClass);
    m_checkSvc=perimeter_new(CheckSvc);
    m_settings=perimeter_new(Settings);
    m_deviceStatusData=perimeter_new(DeviceStatusDataVm);
}

void AppCtrl::adjustWindowSize(QObject *win_obj)
{
#if defined( WIN32 )
    QWindow *win = qobject_cast< QWindow *>( win_obj );
    if ( win == Q_NULLPTR ) { return; }
    QScreen* screen = win->screen();
    QSize  scr_size = screen->size();
#if 0
    RECT w_rect;
    ::GetWindowRect( ( HWND )( win->winId()), & w_rect );
    int  pos_x = ( scr_size.width()  - ( w_rect.right - w_rect.left )) / 2;
    int  pos_y = ( scr_size.height() - ( w_rect.bottom - w_rect.top )) / 2;

    win->setPosition( pos_x, pos_y );
#else
    QRect w_size = win->geometry();
    int  pos_x = ( scr_size.width()  - ( w_size.width()  - 2 )) / 2;
    int  pos_y = ( scr_size.height() - ( w_size.height() - 2 )) / 2;

    win->setGeometry( pos_x, pos_y, w_size.width() - 2, w_size.height() - 2 );

    HWND previousFocusWnd = Q_NULLPTR; HWND wHa = ( HWND )( win->winId());
    previousFocusWnd = GetForegroundWindow();
    if(previousFocusWnd != wHa){
        AttachThreadInput( GetWindowThreadProcessId( previousFocusWnd, Q_NULLPTR ), GetCurrentThreadId(), TRUE );
        AttachThreadInput( GetWindowThreadProcessId( previousFocusWnd, Q_NULLPTR ), GetWindowThreadProcessId( wHa, Q_NULLPTR ), TRUE );
        SendMessage( wHa, WM_SETFOCUS, 0, 0 );
        SetForegroundWindow( wHa );
        AttachThreadInput( GetWindowThreadProcessId( previousFocusWnd, Q_NULLPTR ), GetWindowThreadProcessId( wHa, Q_NULLPTR ), FALSE );
        AttachThreadInput( GetWindowThreadProcessId( previousFocusWnd, Q_NULLPTR ), GetCurrentThreadId(), FALSE );
    }

    //    // 2--Always front main window。这段代码打开会导致系统菜单栏显示在界面上方
    //    WId hwnd = win->winId();
    //    ::SetWindowPos( ( HWND ) hwnd,  HWND_NOTOPMOST,
    //                    pos_x, pos_y, 0, 0,
    //                    SWP_NOSIZE | SWP_NOSENDCHANGING | SWP_NOACTIVATE
    //                    );
    //    ::SetForegroundWindow( ( HWND ) hwnd );
#endif
#endif
}
// ============================================================================
// dtor
// ============================================================================
AppCtrlPriv :: ~AppCtrlPriv ( )
{
    // release base service
    ObjMgrSvc::freeInstance();
    MsgBusSvc::freeInstance();
    AppSettingsSvc::freeInstance();
    perimeter_delete(m_databaseSvc,databaseSvc);
    perimeter_delete(m_checkSvc,CheckSvc);
    perimeter_delete(m_testClass,TestClass);
    perimeter_delete(m_settings,Settings);
    perimeter_delete(m_deviceStatusData,DeviceStatusDataVm);
}

// ============================================================================
// register types in ObjMgr
// ============================================================================
void  AppCtrlPriv :: registerTypes()
{
    m_main_mod.registerTypes    ( m_parent );
}

// ////////////////////////////////////////////////////////////////////////////
//  wrapper API
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
AppCtrl :: AppCtrl ( QObject *pa ) : QxPack::IcAppCtrlBase( pa )
{
    m_obj = perimeter_new( AppCtrlPriv, this );
//    DevOps::DeviceOperation::getSingleton()
    connect(DevOps::DeviceOperation::getSingleton().data(),&DevOps::DeviceOperation::newStatusData,this,&AppCtrl::deviceStatusDataChanged);
    connect(KeyBoardFilter::getSingleton().data(),&KeyBoardFilter::showDeviceStatusChanged,this,&AppCtrl::showDeviceStatusDataChanged);
}

// ============================================================================
// dtor
// ============================================================================
AppCtrl :: ~AppCtrl ( )
{
    perimeter_delete( m_obj, AppCtrlPriv );
}

// ============================================================================
// return the objmgr
// ============================================================================
QObject*  AppCtrl :: objMgrObj() const
{ return objMgr(); }

// ============================================================================
// return the msgBus
// ============================================================================
QObject*  AppCtrl :: msgBusObj() const
{ return msgBus(); }

//Custom Code
QObject*    AppCtrl::databaseSvcObj() const             {return T_PrivPtr( m_obj )-> getDatabaseSvcObj();}

QObject *  AppCtrl::getTestClass() const
{
    return T_PrivPtr( m_obj )-> getTestClass();
}

QObject *AppCtrl::getCheckSvc() const
{
    return T_PrivPtr( m_obj )-> getCheckSvc();
}


QObject *AppCtrl::getFrameProvidSvc() const
{
    return FrameProvidSvc::getSingleton().data();
}

QObject *AppCtrl::getUtilitySvc() const
{
    return UtilitySvc::getSingleton().data();
}

QObject *AppCtrl::getDeviceStatusData() const
{
    return T_PrivPtr( m_obj )-> getDeviceStatusData();
}

bool AppCtrl::getShowDeviceStatusData() const
{
    return KeyBoardFilter::showDeviceStatusData;
}

//QObject *AppCtrl::getDeviceOperation() const
//{
//    return T_PrivPtr( m_obj )-> getDeviceOperation();
//}

QObject *AppCtrl::getSettings()
{
    return T_PrivPtr(m_obj)->getSettings();
}

//void AppCtrl::Quit()
//{
//    QCoreApplication::quit();
//}


//QObject*    AppCtrl::getCurrentPatient()                {
//    return T_PrivPtr( m_obj )->getCurrentPatient();
//}
//void        AppCtrl::setCurrentPatient(QObject *value)  {T_PrivPtr( m_obj )->setCurrentPatient(value);emit currentPatientChanged(value);}

// ============================================================================
// instance name
// ============================================================================
QString  AppCtrl :: instanceName() const
{
    return GUNS_AppCtrl;
}

// ============================================================================
//  previously initialization process
// ============================================================================
void    AppCtrl :: preInit( )
{ }

// ============================================================================
// create a global singleton message bus object
// ============================================================================
QxPack::IcMsgBus*  AppCtrl :: createSingleton_MsgBus( )
{
    return T_PrivPtr( m_obj )->msgBusSvc();
}

// ============================================================================
// create the object manager
// ============================================================================
QxPack::IcObjMgr*   AppCtrl :: createSingleton_ObjMgr( )
{
    return T_PrivPtr( m_obj )->objMgrSvc();
}

// ============================================================================
// create the appsettings manager
// ============================================================================
QxPack::IcAppSettingsBase*  AppCtrl :: createSingleton_AppSettings( )
{
    return T_PrivPtr( m_obj )->appSetSvc();
}

// ============================================================================
// register types in object manager
// ============================================================================
void       AppCtrl :: registerTypes_ObjMgr( )
{
    T_PrivPtr( m_obj )->registerTypes();
}

// ============================================================================
// post stage of initialization
// ============================================================================
void       AppCtrl :: postInit( )
{

}

// ============================================================================
// override the deinitialization process
// ============================================================================
void       AppCtrl :: preDeinit()
{

}

// ============================================================================
// release the refe. of object manager
// ============================================================================
void      AppCtrl :: releaseSingleton_ObjMgr ( )
{

}

// ============================================================================
// release the refe. of message bus
// ============================================================================
void      AppCtrl :: releaseSingleton_MsgBus( )
{

}

// ============================================================================
// release the refe. of application settings
// ============================================================================
void      AppCtrl :: releaseSingleton_AppSettings()
{

}

// ============================================================================
// post stage of deinitialization
// ============================================================================
void      AppCtrl :: postDeinit( )
{

}

}

#endif

