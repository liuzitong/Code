import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

import perm.view 1.0             as   FcPerm
import qxpack.indcom.ui_qml_control 1.0
import qxpack.indcom.ui_qml_base 1.0 // [HINT] this is the pre-registered module name.

IcPageBase {   // this is the wrapped Popup element in ui_qml_control
    id: idUserMgrLoginView; pageName: "UserMgrLogin";
    implicitWidth: FcPerm.PermSkin.winRectWidth;  implicitHeight: FcPerm.PermSkin.winRectHeight;

    property int editItemWidth: width*0.4;

    Rectangle { anchors.fill: parent; color: "#F1F1F2"; }

    ColumnLayout { anchors.fill: parent;

        Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 84; Layout.alignment: Qt.AlignHCenter | Qt.AlignTop; color: "#333e44";

            Label { anchors.centerIn: parent; text: qsTr("User Management"); font.pixelSize: 24;  color: "#FAFAFA"; }
        }

        Item { Layout.fillWidth: true; Layout.fillHeight: true;

            ColumnLayout { anchors.centerIn: parent; spacing: 80*FcPerm.PermSkin.winHeightScale;

                Rectangle { Layout.preferredWidth: 478; Layout.preferredHeight: 38; border.color: "#6E6E6E"; border.width: 1; color: "#FAFAFA"; radius: 4;

                    RowLayout { anchors.fill: parent; anchors.leftMargin: 8; anchors.rightMargin: 8; spacing: 10;

                        Image { Layout.preferredWidth: 22; Layout.preferredHeight: 22; source: "image://PermImProv/default/username"; }

                        Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: "#6E6E6E"; }

                        TextField { id: idUserNameInput; objectName: "UserInputItem.TextField"; Layout.fillWidth: true; Layout.fillHeight: true; placeholderText: qsTr("Please enter user name"); font.pixelSize: 16;  padding: 0; selectByMouse: true; selectionColor: "lightblue";
                            background: Item { }
                        }

                        Button { id: idKeyboardBtn; Layout.preferredWidth: 22; Layout.preferredHeight: 22; checkable: true; checked: false; focusPolicy: Qt.NoFocus;
                            background: Image { width: 22; height: 22; source: idKeyboardBtn.checked ? "image://PermImProv/default/keyboard" : "image://PermImProv/default/keyboardClose"; }
                            onToggled: { idPriv.user_login_vm.enableOfVkb = checked; }
                        }
                    }
                }

                Rectangle { Layout.preferredWidth: 478; Layout.preferredHeight: 38; border.color: "#6E6E6E"; border.width: 1; color: "#FAFAFA"; radius: 4;

                    RowLayout { anchors.fill: parent; anchors.leftMargin: 8; anchors.rightMargin: 8; spacing: 10;

                        Image { Layout.preferredWidth: 22; Layout.preferredHeight: 22; source: "image://PermImProv/default/password"; }

                        Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: "#6E6E6E"; }

                        TextField { id: idUserPwdInput; objectName: "UserInputItem.TextField"; Layout.fillWidth: true; Layout.fillHeight: true; placeholderText: qsTr("Please enter password"); font.pixelSize: 16;  padding: 0; echoMode: idPwdBtn.checked ? TextInput.Normal : TextInput.Password; selectByMouse: true; selectionColor: "lightblue";
                            background: Item { }
                        }

                        Button { id: idPwdBtn; Layout.preferredWidth: 22; Layout.preferredHeight: 22; checkable: true; checked: false; focusPolicy: Qt.NoFocus;
                            background: Image { width: 22; height: 22; source: idPwdBtn.checked ? "image://PermImProv/default/pwdShow" : "image://PermImProv/default/pwdHide"; }
                        }
                    }
                }

                Label { Layout.preferredWidth: 534 - 65; Layout.alignment: Qt.AlignRight; text: qsTr("Please enter a username and password width administrative rights."); font.pixelSize: 16;  color: "#202020"; wrapMode: Text.WordWrap; }
            }
        }

        Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 60; Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom; color: "#333e44";

            RowLayout { anchors.fill: parent; anchors.leftMargin: 5; anchors.rightMargin: 5;

                Button { id: idQuitBtn; Layout.preferredWidth: 100; Layout.preferredHeight: 38; Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter; text: qsTr("Quit"); focusPolicy: Qt.NoFocus;
                    background: Rectangle { implicitWidth: 100; implicitHeight: 38; color: idQuitBtn.down ? "#bdc0c6" : "#E0E0E0"; opacity: enabled ? 1 : 0.3; radius: 4; }
                    contentItem: Label { text: idQuitBtn.text; color: idQuitBtn.down ? "#0064b6" : "#202020"; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter; }
                }

                Item { Layout.fillWidth: true; Layout.fillHeight: true; }

                Button { id: idNextBtn; Layout.preferredWidth: 100; Layout.preferredHeight: 38; Layout.alignment: Qt.AlignRight | Qt.AlignVCenter; text: qsTr("Next"); focusPolicy: Qt.NoFocus;
                    background: Rectangle { implicitWidth: 100; implicitHeight: 38; color: idNextBtn.down ? "#bdc0c6" : "#E0E0E0"; opacity: enabled ? 1 : 0.3; radius: 4; }
                    contentItem: Label { text: idNextBtn.text; color: idNextBtn.down ? "#0064b6" : "#202020"; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter; }
                }
            }
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
        property var user_login_vm: null;

        // ==================================================================
        // initailize the page
        // ==================================================================
        function init( )
        {
            user_login_vm = IcUiQmlApi.appCtrl.objMgr.attachObj( "FcPerm::UserLoginVm", true );
            initUI();
            doOnline(); idUserMgrLoginView.pageInit();
        }

        // ====================================================================
        // de-init the page
        // ====================================================================
        function deinit( )
        {
            doOffline( ); logout();
            IcUiQmlApi.appCtrl.objMgr.detachObj( "FcPerm::UserLoginVm", user_login_vm );
            idUserMgrLoginView.pageDeinit();
        }

        function doOffline( )
        {
            idNextBtn.clicked.disconnect( login );
            user_login_vm.showUserBrowseView.disconnect( turnUserBrowseView );
            idQuitBtn.clicked.disconnect( turnUserLoginView );
        }

        function doOnline( )
        {
            idNextBtn.clicked.connect( login );
            console.log(user_login_vm.userLoginData);
             console.log(user_login_vm.enableOfVkb);
            user_login_vm.showUserBrowseView.connect( turnUserBrowseView );
            console.log(user_login_vm.userLoginData);
             console.log(user_login_vm.enableOfVkb);
            idQuitBtn.clicked.connect( turnUserLoginView );
        }

        function initUI()
        {
            idKeyboardBtn.checked = Qt.binding( function() { return idPriv.user_login_vm.enableOfVkb; } );
        }

        function turnUserBrowseView( )
        {
            idUserMgrLoginView.request( "UserBrowseView", { } );
        }

        function turnUserLoginView( )
        {
            idUserMgrLoginView.request( "UserLoginView", { } );
        }

        // ====================================================================
        // login
        // ====================================================================
        function login( )
        {
            var data = new Object;
            data.name = idUserNameInput.text;
            data.pwd = idUserPwdInput.text;
            data.type = 1;
            user_login_vm.userLoginData = data;
            IcUiQmlApi.postMetaCall( user_login_vm, "login" );
        }

        // ====================================================================
        // logout request
        // ====================================================================
        function logout( )
        {
            IcUiQmlApi.postMetaCall( user_login_vm, "logout" );
        }
    }
}
