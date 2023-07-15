import QtQuick 2.7
import QtQuick.Controls 2.2

import perm.view 1.0             as   FcPerm
import qxpack.indcom.ui_qml_control 1.0
import qxpack.indcom.ui_qml_base 1.0 // [HINT] this is the pre-registered module name.

IcPageBase {   // this is the wrapped Popup element in ui_qml_control
    id: idPermMgrView; pageName: "PermMgr";
    property string viewName: "PermMgr";

    implicitWidth: FcPerm.PermSkin.winRectWidth;  implicitHeight: FcPerm.PermSkin.winRectHeight;

    function  doPageOnline ( ) { idPriv.doOnline(); }
    function  doPageOffline( ) { }

    signal reqExit(  );
    signal reqExitAndGo( var arg );

    StackView { id: idStackView; anchors.fill: parent;
        initialItem: UserLoginView { }
    }

    Connections { target: idStackView.currentItem

        onPageInit: { console.log("page init") }

        onPageDeinit: { console.log("page init") }

        onRequest: {
            if ( req_str === "UserDiChgPwdMgr" )
            { idPriv.turnUserDiChgPwdView(); }
            else if ( req_str === "UserMgrLoginView" )
            { idPriv.turnUserMgrLoginView(); }
            else if ( req_str === "UserBrowseView" )
            { idPriv.turnUserBrowseView(); }
            else if ( req_str === "UserGrpBrowseView" )
            { idPriv.turnUserGrpBrowseView(); }
            else if ( req_str === "NewUserView" )
            { idPriv.turnNewUserView(); }
            else if ( req_str === "ModUserView" )
            { idPriv.turnModUserView(); }
            else if ( req_str === "NewUserGrpView" )
            { idPriv.turnNewUserGrpView(); }
            else if ( req_str === "ModUserGrpView" )
            { idPriv.turnModUserGrpView(); }
            else if ( req_str === "Back" )
            { idStackView.pop(); }
            else if ( req_str === "UserLoginView" )
            { idStackView.pop(idStackView.find(function(item) { return item.pageName === "UserLogin"; })); }
            else if ( req_str === "NavMainView" )
            {idPermMgrView.reqExitAndGo( req_arg ); }
            else if(req_str === "CloseApp"){ idPermMgrView.reqExit(); }
        }
    }

    // ////////////////////////////////////////////////////////////////////////
    // logic
    // ////////////////////////////////////////////////////////////////////////
    Component.onCompleted:   { idPriv.init(); }
    Component.onDestruction: { idPriv.deinit(); }

    QtObject {
        id: idPriv;
        property var perm_mgr_view: null;

        // ==================================================================
        // initailize the page
        // ==================================================================
        function init( )
        {
            perm_mgr_view = IcUiQmlApi.appCtrl.objMgr.attachObj( "FcPerm::PermMgrVm", true );
            idPermMgrView.pageInit();
        }

        // ====================================================================
        // de-init the page
        // ====================================================================
        function deinit( )
        {
            IcUiQmlApi.appCtrl.objMgr.detachObj( "FcPerm::PermMgrVm", perm_mgr_view );
            idPermMgrView.pageDeinit();
        }

        function turnUserDiChgPwdView( )
        {
            var item = idStackView.find(function(item) { return item.pageName === "UserDiChgPwd"; });
            if ( item === null )
            { idStackView.push("qrc:/perm/view/userdichgpwdview.qml"); }
            else
            { idStackView.pop(item); }
        }

        function turnUserMgrLoginView( )
        {
            var item = idStackView.find(function(item) { return item.pageName === "UserMgrLogin"; });
            if ( item === null )
            { idStackView.push("qrc:/perm/view/usermgrloginview.qml"); }
            else
            { idStackView.pop(item); }
        }

        function turnUserBrowseView( )
        {
            var item = idStackView.find(function(item) { return item.pageName === "UserBrowse"; });
            if ( item === null )
            { idStackView.push("qrc:/perm/view/userbrowseview.qml"); }
            else
            { idStackView.pop(item); }
        }

        function turnUserGrpBrowseView( )
        {
            var item = idStackView.find(function(item) { return item.pageName === "UserGrpBrowse"; });
            if ( item === null )
            { idStackView.push("qrc:/perm/view/usergrpbrowseview.qml"); }
            else
            { idStackView.pop(item); }
        }

        function turnNewUserView( )
        {
            var item = idStackView.find(function(item) { return item.pageName === "NewUser"; });
            if ( item === null )
            { idStackView.push("qrc:/perm/view/newuserview.qml"); }
            else
            { idStackView.pop(item); }
        }

        function turnModUserView( )
        {
            var item = idStackView.find(function(item) { return item.pageName === "ModUser"; });
            if ( item === null )
            { idStackView.push("qrc:/perm/view/moduserview.qml"); }
            else
            { idStackView.pop(item); }
        }

        function turnNewUserGrpView( )
        {
            var item = idStackView.find(function(item) { return item.pageName === "NewUserGrp"; });
            if ( item === null )
            { idStackView.push("qrc:/perm/view/newusergrpview.qml"); }
            else
            { idStackView.pop(item); }
        }

        function turnModUserGrpView( )
        {
            var item = idStackView.find(function(item) { return item.pageName === "ModUserGrp"; });
            if ( item === null )
            { idStackView.push("qrc:/perm/view/modusergrpview.qml"); }
            else
            { idStackView.pop(item); }
        }

        function doOnline() {
            if(idStackView.currentItem.pageName === "UserLogin"){
                idStackView.currentItem.onReadyTo();

            }
        }
    }
}
