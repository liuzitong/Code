import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

import perm.view 1.0             as   FcPerm
import qxpack.indcom.ui_qml_control 1.0
import qxpack.indcom.ui_qml_base 1.0 // [HINT] this is the pre-registered module name.

IcPageBase {   // this is the wrapped Popup element in ui_qml_control
    id: idNewUserView; pageName: "NewUser";
    implicitWidth: FcPerm.PermSkin.winRectWidth;  implicitHeight: FcPerm.PermSkin.winRectHeight;

    property int editItemWidth: width*0.4;

    Rectangle { anchors.fill: parent; color: "#F1F1F2"; }

    ColumnLayout { anchors.fill: parent; spacing: 4;

        Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 84; Layout.alignment: Qt.AlignHCenter | Qt.AlignTop; color: "#333e44";
            Text { anchors.centerIn: parent; text: qsTr("Create User"); font.pixelSize: 24; color: "#FAFAFA"; }
        }

        ColumnLayout { Layout.fillWidth: true; Layout.fillHeight: true; spacing: 40;

            Item { Layout.fillWidth: true; Layout.preferredHeight: 38; Layout.alignment: Qt.AlignHCenter;

                Label { anchors.left: parent.left; anchors.right: idUserNameEdit.left; anchors.rightMargin: 20; height: 38; text: qsTr("user name"); color: "#202020"; font.pixelSize: 16;  horizontalAlignment: Text.AlignRight; verticalAlignment: Text.AlignVCenter; }

                TextField { id: idUserNameEdit; objectName: "UserInputItem.TextField"; anchors.left: parent.left; anchors.leftMargin: editItemWidth; width: editItemWidth; height: 38; placeholderText: qsTr("Please enter user name"); font.pixelSize: 16;  selectByMouse: true; selectionColor: "lightblue";
                    background: Rectangle { implicitWidth: parent.width; implicitHeight: 38; border.color: "#6E6E6E"; border.width: 1; color: "#FAFAFA"; radius: 5; }
                }
            }

            Item { Layout.fillWidth: true; Layout.preferredHeight: 38; Layout.alignment: Qt.AlignHCenter;

                Label { anchors.left: parent.left; anchors.right: idPwdEdit.left; anchors.rightMargin: 20; height: 38; text: qsTr("password"); color: "#202020"; font.pixelSize: 16;  horizontalAlignment: Text.AlignRight; verticalAlignment: Text.AlignVCenter; }

                TextField { id: idPwdEdit; objectName: "UserInputItem.TextField"; anchors.left: parent.left; anchors.leftMargin: editItemWidth; width: editItemWidth; height: 38; placeholderText: qsTr("6-20 digits or letters"); font.pixelSize: 16;  selectByMouse: true; selectionColor: "lightblue";
                    validator: RegExpValidator { regExp: /[0-9a-zA-Z]{6,20}/ }
                    background: Rectangle { implicitWidth: parent.width; implicitHeight: 38; border.color: "#6E6E6E"; border.width: 1; color: "#FAFAFA"; radius: 5; }
                }
            }

            Item { Layout.fillWidth: true; Layout.preferredHeight: 38; Layout.alignment: Qt.AlignHCenter;

                Label { anchors.left: parent.left; anchors.right: idConfirmPwdEdit.left; anchors.rightMargin: 20; height: 38; text: qsTr("confirm password"); color: "#202020"; font.pixelSize: 16;  horizontalAlignment: Text.AlignRight; verticalAlignment: Text.AlignVCenter; }

                TextField { id: idConfirmPwdEdit; objectName: "UserInputItem.TextField"; anchors.left: parent.left; anchors.leftMargin: editItemWidth; width: editItemWidth; height: 38; placeholderText: qsTr("Please enter password again"); font.pixelSize: 16;  selectByMouse: true; selectionColor: "lightblue";
                    validator: RegExpValidator { regExp: /[0-9a-zA-Z]{6,20}/ }
                    background: Rectangle { implicitWidth: parent.width; implicitHeight: 38; border.color: "#6E6E6E"; border.width: 1; color: "#FAFAFA"; radius: 5; }
                }
            }

            Item { Layout.fillWidth: true; Layout.preferredHeight: 100; Layout.alignment: Qt.AlignHCenter;

                Label { anchors.left: parent.left; anchors.right: idMemoEdit.left; anchors.rightMargin: 20; height: 38; text: qsTr("memo"); color: "#202020"; font.pixelSize: 16;  horizontalAlignment: Text.AlignRight; verticalAlignment: Text.AlignVCenter; }

                TextArea { id: idMemoEdit; objectName: "UserInputItem.TextArea"; anchors.left: parent.left; anchors.leftMargin: editItemWidth; width: editItemWidth; height: 100; placeholderText: qsTr("Please enter memo information"); font.pixelSize: 16;  selectByMouse: true; selectionColor: "lightblue";
                    background: Rectangle { implicitWidth: parent.width; implicitHeight: 100; border.color: "#6E6E6E"; border.width: 1; color: "#FAFAFA"; radius: 5; }
                }
            }

            Item { Layout.fillWidth: true; Layout.preferredHeight: 38; Layout.alignment: Qt.AlignHCenter;

                Label { anchors.left: parent.left; anchors.right: idUserGrpBtn.left; anchors.rightMargin: 20; height: 38; text: qsTr("user group"); color: "#202020"; font.pixelSize: 16;  horizontalAlignment: Text.AlignRight; verticalAlignment: Text.AlignVCenter; }

                Button { id: idUserGrpBtn; anchors.left: parent.left; anchors.leftMargin: editItemWidth; width: 120; height: 38; text: qsTr("Edit"); focusPolicy: Qt.NoFocus;
                    background: Rectangle { implicitWidth: 120; implicitHeight: 38; color: idUserGrpBtn.down ? "#bdc0c6" : "#E0E0E0"; border.color: "#6E6E6E"; border.width: 1; radius: 5; }
                    contentItem: Label { text: idUserGrpBtn.text; color: idUserGrpBtn.down ? "#0064b6" : "#202020"; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter; }
                }
            }

            Item { Layout.fillWidth: true; Layout.preferredHeight: 38; Layout.alignment: Qt.AlignHCenter;

                Label { anchors.left: parent.left; anchors.right: idUserPermBtn.left; anchors.rightMargin: 20; height: 38; text: qsTr("user permission table"); color: "#202020"; font.pixelSize: 16;  horizontalAlignment: Text.AlignRight; verticalAlignment: Text.AlignVCenter; }

                Button { id: idUserPermBtn; anchors.left: parent.left; anchors.leftMargin: editItemWidth; width: 120; height: 38; text: qsTr("Edit"); focusPolicy: Qt.NoFocus;
                    background: Rectangle { implicitWidth: 120; implicitHeight: 38; color: idUserPermBtn.down ? "#bdc0c6" : "#E0E0E0"; border.color: "#6E6E6E"; border.width: 1; radius: 5; }
                    contentItem: Label { text: idUserPermBtn.text; color: idUserPermBtn.down ? "#0064b6" : "#202020"; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter; }
                }
            }
        }

//        Label { id: idNoticeText; width: 1278; anchors.left: parent.left; anchors.leftMargin: editItemWidth; anchors.bottom: parent.bottom; anchors.bottomMargin: 10; color: "red"; font.pixelSize: 16;  }


        Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 60; Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom; color: "#333e44";

            RowLayout { anchors.fill: parent; anchors.leftMargin: 20; anchors.rightMargin: 20; spacing: 40;

                Button { id: idBackBtn; Layout.preferredWidth: 100; Layout.preferredHeight: 38; Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter; text: qsTr("Back"); focusPolicy: Qt.NoFocus;
                    background: Rectangle { implicitWidth: 100; implicitHeight: 38; color: idBackBtn.down ? "#bdc0c6" : "#E0E0E0"; opacity: enabled ? 1 : 0.3; radius: 4; }
                    contentItem: Label { text: idBackBtn.text; color: idBackBtn.down ? "#0064b6" : "#202020"; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter; }
                }

                Item { Layout.fillWidth: true; Layout.fillHeight: true; }

                Button { id: idSaveBtn; Layout.preferredWidth: 100; Layout.preferredHeight: 38; Layout.alignment: Qt.AlignRight | Qt.AlignVCenter; text: qsTr("Save"); focusPolicy: Qt.NoFocus;
                    background: Rectangle { implicitWidth: 100; implicitHeight: 38; color: idSaveBtn.down ? "#bdc0c6" : "#E0E0E0"; opacity: enabled ? 1 : 0.3; radius: 4; }
                    contentItem: Label { text: idSaveBtn.text; color: idSaveBtn.down ? "#0064b6" : "#202020"; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter; }
                }

                Button { id: idQuitBtn; Layout.preferredWidth: 100; Layout.preferredHeight: 38; Layout.alignment: Qt.AlignRight | Qt.AlignVCenter; text: qsTr("Quit"); focusPolicy: Qt.NoFocus;
                    background: Rectangle { implicitWidth: 100; implicitHeight: 38; color: idQuitBtn.down ? "#bdc0c6" : "#E0E0E0"; opacity: enabled ? 1 : 0.3; radius: 4; }
                    contentItem: Label { text: idQuitBtn.text; color: idQuitBtn.down ? "#0064b6" : "#202020"; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter; }
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
        property var new_user_vm: null;

        // ==================================================================
        // initailize the page
        // ==================================================================
        function init( )
        {
            new_user_vm       = IcUiQmlApi.appCtrl.objMgr.attachObj( "FcPerm::NewUserVm", true );
            initUI(); doOnline();
            idNewUserView.pageInit();
        }

        // ====================================================================
        // de-init the page
        // ====================================================================
        function deinit( )
        {
            doOffline( );
            IcUiQmlApi.appCtrl.objMgr.detachObj( "FcPerm::NewUserVm", new_user_vm );
            idNewUserView.pageDeinit();
        }

        function doOffline( )
        {
            new_user_vm.showUserGrpListView.disconnect( onShowUserGroupListView );
            new_user_vm.showUserPermListView.disconnect( onShowUserPermListView );
            idBackBtn.clicked.disconnect( backView );
            idQuitBtn.clicked.disconnect( turnUserLoginView );
            idUserGrpBtn.clicked.disconnect( editUserGroup );
            idUserPermBtn.clicked.disconnect( editUserPerm );
            idSaveBtn.clicked.disconnect( save );
        }

        function doOnline( )
        {
            new_user_vm.showUserGrpListView.connect( onShowUserGroupListView );
            new_user_vm.showUserPermListView.connect( onShowUserPermListView );
            idBackBtn.clicked.connect( backView );
            idQuitBtn.clicked.connect( turnUserLoginView );
            idUserGrpBtn.clicked.connect( editUserGroup );
            idUserPermBtn.clicked.connect( editUserPerm );
            idSaveBtn.clicked.connect( save );
        }

        function backView( )
        {
            idNewUserView.request( "Back", { } );
        }

        function turnUserLoginView( )
        {
            idNewUserView.request( "UserLoginView", { } );
        }

        function editUserGroup( )
        {
            IcUiQmlApi.postMetaCall( idPriv.new_user_vm, "editUserGroup" );
        }

        function editUserPerm( )
        {
            IcUiQmlApi.postMetaCall( idPriv.new_user_vm, "editUserPerm" );
        }

        function initUI()
        {
            idUserNameEdit.text = Qt.binding( function() { return idPriv.new_user_vm.editUserInfo.userName; } );
            idPwdEdit.text      = Qt.binding( function() { return idPriv.new_user_vm.editUserInfo.userPwd; } );
            idConfirmPwdEdit.text = Qt.binding( function() { return idPriv.new_user_vm.editUserInfo.userPwd; } );
            idMemoEdit.text     = Qt.binding( function() { return idPriv.new_user_vm.editUserInfo.userMemo; } );

            idSaveBtn.enabled = Qt.binding( function() {
                return idUserNameEdit.text.length > 0 && idPwdEdit.text.length >=6 && idPwdEdit.text === idConfirmPwdEdit.text;
            });
        }

        // ====================================================================
        // save user
        // ====================================================================
        function save( )
        {
            var data = new Object();
            data.UID = "";
            data.name = idUserNameEdit.text;
            data.pwd = idPwdEdit.text;
            data.memo = idMemoEdit.text;
            new_user_vm.userInfo = data;
            IcUiQmlApi.postMetaCall( new_user_vm, "save" );
        }

        function onShowUserGroupListView( )
        {
            //[HINT] below code are demo. how to dyn. create component and object

            // a) before use the spec. custom control, create it's component
            var cmpt = Qt.createComponent("qrc:/perm/view/usergrplistview.qml");
            if ( cmpt.status !== Component.Ready ) { console.error("create confirmdiag failed!"); return; }

            // b) now create the object, set it parent to this control
            var cfm_diag = cmpt.createObject( idNewUserView );

            // c) setup information about this confirm object
            cfm_diag.contentWidth = idNewUserView.width; cfm_diag.contentHeight = idNewUserView.height;

            cfm_diag.destroyOnClose = true;

            // d) now enter the modal dialog event...
            cfm_diag.open();

            cfm_diag = null;
        }

        function onShowUserPermListView( )
        {
            //[HINT] below code are demo. how to dyn. create component and object

            // a) before use the spec. custom control, create it's component
            var cmpt = Qt.createComponent("qrc:/perm/view/userpermlistview.qml");
            if ( cmpt.status !== Component.Ready ) { console.error("create confirmdiag failed!"); return; }

            // b) now create the object, set it parent to this control
            var cfm_diag = cmpt.createObject( idNewUserView );

            // c) setup information about this confirm object
            cfm_diag.contentWidth = idNewUserView.width; cfm_diag.contentHeight = idNewUserView.height;

            cfm_diag.destroyOnClose = true;

            // d) now enter the modal dialog event...
            cfm_diag.open();

            cfm_diag = null;
        }
    }
}
