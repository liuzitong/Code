#ifndef PERM_PERM_MOD_CXX
#define PERM_PERM_MOD_CXX

#include "perm_mod.hxx"
#include "perm/common/perm_guns.hxx"
#include <QCoreApplication>

// the class list
// add the include files
#include "perm/viewmodel/perm_permmgrvm.hxx"
#include "perm/viewmodel/perm_usergrpmgrvm.hxx"
#include "perm/viewmodel/perm_usergrppermlistvm.hxx"
#include "perm/viewmodel/perm_userinfochgvm.hxx"
#include "perm/viewmodel/perm_userloginvm.hxx"
#include "perm/viewmodel/perm_usermgrvm.hxx"
#include "perm/viewmodel/perm_userpermlistvm.hxx"
#include "perm/viewmodel/perm_newuservm.hxx"
#include "perm/viewmodel/perm_usergrplistvm.hxx"
#include "perm/viewmodel/perm_newusergrpvm.hxx"
#include "perm/viewmodel/perm_moduservm.hxx"
#include "perm/viewmodel/perm_modusergrpvm.hxx"
#include "perm/local-part/resimprov/perm_improv.hxx"

namespace FcPerm {

// ////////////////////////////////////////////////////////////////////////////
//  wrap API
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
PermMod :: PermMod ( QObject *pa ) : QxPack::IcModMgrBase( pa )
{

}

// ============================================================================
// dtor
// ============================================================================
PermMod :: ~PermMod ( )
{
}

// ================================================
// register the image provider for use
// ================================================
static void  regPermImProv( QQmlEngine *eng )
{
    if(eng == Q_NULLPTR){ return; }
    // ------------------------------------------------------------------
    // NOTE: the engine will take ownership of the provider in QT.
    // ------------------------------------------------------------------
    eng->addImageProvider( QStringLiteral("PermImProv"), new PermImProv( ) );

}

// ============================================================================
// register types
// ============================================================================
void  PermMod :: registerTypesEx( QxPack::IcAppCtrlBase *app_base )
{
    // ------------------------------------------------------------------------
    // check param.
    // ------------------------------------------------------------------------
    if ( app_base == Q_NULLPTR ) { return; }
    QxPack::IcObjMgr *obj_mgr = app_base->objMgr();
    if ( obj_mgr  == Q_NULLPTR ) { return; }

    // ------------------------------------------------------------------------
    // register all public types of this module
    // ------------------------------------------------------------------------
    // example:
    // obj_mgr->registerType<ClassName>( "the instance name used in application", Q_NULLPTR );
    obj_mgr->registerType<FcPerm::PermMgrVm        >( GUNS_PermMgrVm        , Q_NULLPTR );
    obj_mgr->registerType<FcPerm::UserGrpMgrVm     >( GUNS_UserGrpMgrVm     , Q_NULLPTR );
    obj_mgr->registerType<FcPerm::UserGrpPermListVm>( GUNS_UserGrpPermListVm, Q_NULLPTR );
    obj_mgr->registerType<FcPerm::UserInfoChgVm    >( GUNS_UserInfoChgVm    , Q_NULLPTR );
    obj_mgr->registerType<FcPerm::UserLoginVm      >( GUNS_UserLoginVm      , Q_NULLPTR );
    obj_mgr->registerType<FcPerm::UserMgrVm        >( GUNS_UserMgrVm        , Q_NULLPTR );
    obj_mgr->registerType<FcPerm::UserPermListVm   >( GUNS_UserPermListVm   , Q_NULLPTR );
    obj_mgr->registerType<FcPerm::NewUserVm        >( GUNS_NewUserVm        , Q_NULLPTR );
    obj_mgr->registerType<FcPerm::UserGrpListVm    >( GUNS_UserGrpListVm    , Q_NULLPTR );
    obj_mgr->registerType<FcPerm::NewUserGrpVm     >( GUNS_NewUserGrpVm     , Q_NULLPTR );
    obj_mgr->registerType<FcPerm::ModUserVm        >( GUNS_ModUserVm        , Q_NULLPTR );
    obj_mgr->registerType<FcPerm::ModUserGrpVm     >( GUNS_ModUserGrpVm     , Q_NULLPTR );
}

void PermMod::regTypesToEngine(QQmlEngine *eng)
{
    regPermImProv(eng);
}


}

#endif
