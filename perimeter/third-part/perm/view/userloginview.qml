import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import perm.view 1.0             as   FcPerm
import qxpack.indcom.ui_qml_control 1.0
import qxpack.indcom.ui_qml_base 1.0 // [HINT] this is the pre-registered module name.

IcPageBase {   // this is the wrapped Popup element in ui_qml_control
    id: idUserLoginView; pageName: "UserLogin";
    implicitWidth: FcPerm.PermSkin.winRectWidth;  implicitHeight: FcPerm.PermSkin.winRectHeight;

    property int editItemWidth: width*0.4;

    function onReadyTo(){ idPriv.onReadyTo(); }

    Rectangle { anchors.fill: parent; color: "#333E44"; }

    Item { width: 124; height: 124;
        Image { anchors.centerIn: parent; width: 64; height: 64; source: "image://PermImProv/default/sysEyeLogo128"; }
    }

    Item { width: 96; height: 96; anchors.right: parent.right;
        Button { id: idCloseBtn; anchors.centerIn: parent; width: 36; height: 36; focusPolicy: Qt.NoFocus; opacity: pressed? 0.7 : 1.0;
            background: Image { source: "image://PermImProv/default/close"; }
        }
    }

    ColumnLayout { anchors.centerIn: parent; spacing: 20*FcPerm.PermSkin.winHeightScale;

        property int tfWidth: 478;  property int tfHeght: 38;
        Label { Layout.preferredWidth: parent.tfWidth; Layout.preferredHeight: 80; text: qsTr("User Login")+lt; color: "#FAFAFA"; font.pixelSize: 48;  horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter; }

        Item{ height: 40*FcPerm.PermSkin.winHeightScale; }
        Rectangle { Layout.preferredWidth: parent.tfWidth; Layout.preferredHeight: parent.tfHeght; border.color: "#6E6E6E"; border.width: 1; color: "#FAFAFA"; radius: 4;

            RowLayout { anchors.fill: parent; anchors.leftMargin: 8; anchors.rightMargin: 8; spacing: 10;

                Image { Layout.preferredWidth: 22; Layout.preferredHeight: 22; source: "image://PermImProv/default/username"; }

                Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: "#6E6E6E"; }

                TextField { id: idUserNameInput; objectName: "UserInputItem.TextField"; Layout.fillWidth: true; Layout.fillHeight: true; placeholderText: qsTr("Please enter user name")+lt; font.pixelSize: 16;  padding: 0; selectByMouse: true; selectionColor: "lightblue";
                    background: Item { }
                }

                Button { id: idKeyboardBtn; Layout.preferredWidth: 38; Layout.preferredHeight: 38; checkable: true; focusPolicy: Qt.NoFocus;
                    background: Image { width: 22; height: 22; anchors.centerIn: parent; source: idKeyboardBtn.checked ? "image://PermImProv/default/keyboard" : "image://PermImProv/default/keyboardClose"; }
                    onToggled: { idPriv.user_login_vm.enableOfVkb = checked; }
                }
            }
        }

        Item{ height: 40*FcPerm.PermSkin.winHeightScale; }
        Rectangle { Layout.preferredWidth: parent.tfWidth; Layout.preferredHeight: parent.tfHeght; border.color: "#6E6E6E"; border.width: 1; color: "#FAFAFA"; radius: 4;

            RowLayout { anchors.fill: parent; anchors.leftMargin: 8; anchors.rightMargin: 8; spacing: 10;

                Image { Layout.preferredWidth: 22; Layout.preferredHeight: 22; source: "image://PermImProv/default/password"; }

                Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: "#6E6E6E"; }

                TextField { id: idUserPwdInput; objectName: "UserInputItem.TextField"; Layout.fillWidth: true; Layout.fillHeight: true; placeholderText: qsTr("Please enter password")+lt; font.pixelSize: 16;  padding: 0; echoMode: idPwdBtn.checked ? TextInput.Normal : TextInput.Password; selectByMouse: true; selectionColor: "lightblue";
                    background: Item { }
                }

                Button {
                    id: idPwdBtn; Layout.preferredWidth: 38; Layout.preferredHeight: 38; checkable: true; checked: false; focusPolicy: Qt.NoFocus;
                    background: Image { width: 22; height: 22; anchors.centerIn: parent; source: idPwdBtn.checked ? "image://PermImProv/default/pwdShow" : "image://PermImProv/default/pwdHide"; }
                }
            }
        }
        Item{ height: 10*FcPerm.PermSkin.winHeightScale; }
        Item{ height: 1; }
        Item { Layout.preferredWidth: parent.tfWidth; Layout.preferredHeight: parent.tfHeght;
            CheckBox{ id: idRememberPwd; text: qsTr("Remember the password")+lt; contentItem:Label{ leftPadding: 35; width: contentWidth; text:idRememberPwd.text; font.pixelSize: 16; color: "#FAFAFA"; anchors.verticalCenter: parent.verticalCenter;}
                onCheckStateChanged: { idPriv.rememberPwdTo(checked); }
            }
            Button { id: idResetPwdBtn; anchors.right: parent.right; anchors.verticalCenter: parent.verticalCenter; text: qsTr("Reset password")+lt; focusPolicy: Qt.NoFocus;
                background: Item { }
                contentItem: Label { text: idResetPwdBtn.text; color: idResetPwdBtn.pressed? "#0064b6" : "#FAFAFA";  font.underline: true; font.pixelSize: 16; verticalAlignment: Text.AlignVCenter; }
            }
        }
        Item{ height: 1; }
        Button { id: idLoginBtn; Layout.preferredWidth: parent.tfWidth; Layout.preferredHeight: parent.tfHeght; text: qsTr("Login")+lt; focusPolicy: Qt.NoFocus;
            background: Rectangle { border.color: "#6E6E6E"; border.width: 1; color: idLoginBtn.pressed? "#bdc0c6" : "#E0E0E0"; radius: 4; }
            contentItem: Label { text: idLoginBtn.text; color: idLoginBtn.pressed? "#0064b6" : "#202020"; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter; }
        }

        Item{ height: 10*FcPerm.PermSkin.winHeightScale; }
        Item { Layout.preferredWidth: parent.tfWidth; Layout.preferredHeight: parent.tfHeght;

            Button { id: idUserMgrBtn; anchors.right: parent.right; anchors.verticalCenter: parent.verticalCenter; text: qsTr("User management")+lt; focusPolicy: Qt.NoFocus;
                background: Item { }
                contentItem: Label { text: idUserMgrBtn.text; color: idUserMgrBtn.pressed? "#0064b6" : "#FAFAFA";  font.underline: true; font.pixelSize: 16; verticalAlignment: Text.AlignVCenter; }
            }
        }
    }

    Timer { id: idTimer; interval: 1000; running: false; repeat: true
        onTriggered : {
            idLoginBtn.text = qsTr("Login") + lt + "(" + idPriv.autoLoginTime + ")"+lt;
            if ( idPriv.autoLoginTime <= 0 )
            { idPriv.login(); idPriv.stopTimer(); return; }

            idPriv.autoLoginTime -= 1;
        }
    }

    function doOffline ( ) { idPriv.doOffline( ); }
    function doOnline  ( ) { idPriv.doOnline( ); }

    // ////////////////////////////////////////////////////////////////////////
    // logic
    // ////////////////////////////////////////////////////////////////////////
    Component.onCompleted:   { idPriv.init();   }
    Component.onDestruction: { idPriv.deinit(); }

    QtObject {
        id: idPriv;
        property var  user_login_vm: null;
        property int  autoLoginTime: 3;
        property bool autoLoginSwt: false;

        // ==================================================================
        // initailize the page
        // ==================================================================
        function init( )
        {
            user_login_vm = IcUiQmlApi.appCtrl.objMgr.attachObj( "FcPerm::UserLoginVm", true );
            initUI();
            doOnline(); idUserLoginView.pageInit();
        }

        // ====================================================================
        // de-init the page
        // ====================================================================
        function deinit( )
        {
            doOffline( );
            IcUiQmlApi.appCtrl.objMgr.detachObj( "FcPerm::UserLoginVm", user_login_vm );
            idUserLoginView.pageDeinit();
        }

        function doOffline( )
        {
            user_login_vm.showNavMainView.disconnect( turnNavMainView);
            idResetPwdBtn.clicked.disconnect( turnUserDiChgPwdMgr );
            idUserMgrBtn.clicked.disconnect( turnUserMgrLoginView );
            idCloseBtn.clicked.disconnect( turnCloseApp );
            idLoginBtn.clicked.disconnect( login );
            idUserNameInput.textEdited.disconnect( stopTimer );
            idUserPwdInput.textEdited.disconnect( stopTimer );
        }

        function doOnline( )
        {
            user_login_vm.showNavMainView.connect( turnNavMainView);
            idResetPwdBtn.clicked.connect( turnUserDiChgPwdMgr );
            idUserMgrBtn.clicked.connect( turnUserMgrLoginView );
            idCloseBtn.clicked.connect( turnCloseApp );
            idLoginBtn.clicked.connect( login );
            idUserNameInput.textEdited.connect( stopTimer );
            idUserPwdInput.textEdited.connect( stopTimer );
            idPriv.onReadyTo();
        }

        function turnNavMainView( )
        {
            idUserLoginView.request( "NavMainView", user_login_vm.permission);
//            if(user_login_vm !== null)
//            {
//                user_login_vm.quitLogin();
//            }
        }

        function turnUserDiChgPwdMgr( )
        {
            stopTimer( );
            idUserLoginView.request( "UserDiChgPwdMgr", {} );
        }

        function turnUserMgrLoginView( )
        {
            stopTimer( );
            idUserLoginView.request( "UserMgrLoginView", {} );
        }

        function turnCloseApp( )
        {
            idUserLoginView.request( "CloseApp", {} );
            if(user_login_vm !== null){
                user_login_vm.quitLogin();
            }
        }

        function initUI()
        {
            idKeyboardBtn.checked = Qt.binding( function() { return idPriv.user_login_vm.enableOfVkb; } );
            idUserNameInput.text = Qt.binding( function() { return idPriv.user_login_vm.localUser.name; } );
            idRememberPwd.checked = Qt.binding( function() { return idPriv.user_login_vm.localUser.rememberpwd; } );
            if(idRememberPwd.checked)
                idUserPwdInput.text = Qt.binding( function() { return idPriv.user_login_vm.localUser.pwd; } );
            autoLoginSwt = Qt.binding( function() { return idPriv.user_login_vm.localUser.autoLogin; } );
            if ( autoLoginSwt && idUserNameInput.text.length > 0 && idUserPwdInput.text.length > 0 )
            {

                idTimer.start();
            }
        }

        function stopTimer( )
        {
            idTimer.stop(); idLoginBtn.text = qsTr("Login")+lt;
        }

        // ====================================================================
        // login request
        // ====================================================================
        function login( )
        {
            var data = new Object;
            data.name = idUserNameInput.text;
            data.pwd = idUserPwdInput.text;
            data.type = 0;

            user_login_vm.userLoginData = data;
            IcUiQmlApi.postMetaCall( user_login_vm, "login" );
//            console.log("gogogog");
//            loginSucceed();
        }

        function onReadyTo(){
            if(user_login_vm !== null){
                user_login_vm.online(true);

            }
        }
	
        function rememberPwdTo( s ){
            if(idPriv.user_login_vm!==null){
                idPriv.user_login_vm.localUser.rememberpwd = s;
            }
        }
    }
}
