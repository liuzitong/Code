import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

import perm.view 1.0             as   FcPerm
import qxpack.indcom.ui_qml_control 1.0
import qxpack.indcom.ui_qml_base 1.0 // [HINT] this is the pre-registered module name.

IcPageBase {   // this is the wrapped Popup element in ui_qml_control
    id: idUserBrowseView; pageName: "UserBrowse";
    implicitWidth: FcPerm.PermSkin.winRectWidth;  implicitHeight: FcPerm.PermSkin.winRectHeight;

    property int editItemWidth: width*0.4;

    Rectangle { anchors.fill: parent; color: "#F1F1F2"; }

    ColumnLayout { anchors.fill: parent; spacing: 4;

        Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 84; Layout.alignment: Qt.AlignHCenter | Qt.AlignTop; color: "#333e44"

            Label { anchors.centerIn: parent; text: qsTr("User Browse"); font.pixelSize: 24;  color: "#FAFAFA"; }
        }

        ColumnLayout { id: idContC; Layout.fillHeight: true; width: FcPerm.PermSkin.winRectWidth-20; spacing: 0; anchors.horizontalCenter: parent.horizontalCenter;

            Rectangle { Layout.preferredWidth: parent.width; Layout.preferredHeight: 38; Layout.alignment: Qt.AlignHCenter; color: "#6e6e6e";
                RowLayout { anchors.fill: parent; anchors.margins: 1; spacing: 1;

                    Rectangle { Layout.preferredWidth: parent.width*0.07; Layout.fillHeight: true; color: "#e0e0e0"; }

                    Rectangle { Layout.preferredWidth: parent.width*0.28; Layout.fillHeight: true; color: "#e0e0e0";
                        Label { anchors.fill: parent; text: qsTr("user name"); padding: 10; font.pixelSize: 16; clip: true; elide: Text.ElideRight;  color: "#202020"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter; }
                    }

                    Rectangle { Layout.preferredWidth: parent.width*0.4; Layout.fillHeight: true; color: "#e0e0e0";
                        Label { anchors.fill: parent; text: qsTr("memo"); padding: 10; font.pixelSize: 16; clip: true; elide: Text.ElideRight;  color: "#202020"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter; }
                    }

                    Rectangle { Layout.preferredWidth: parent.width*0.12; Layout.fillHeight: true; color: "#e0e0e0"; }

                    Rectangle { Layout.fillWidth: true; Layout.fillHeight: true; color: "#e0e0e0"; }
                }
                Rectangle { height: 1; anchors{ left: parent.left; right: parent.right; margins: 1; } color: "#6e6e6e"; }
            }

            ListView { id: idUserInfoList; Layout.preferredWidth: parent.width; Layout.fillHeight: true; Layout.alignment: Qt.AlignHCenter; focus: true; clip: true;
                delegate: Rectangle { width: idContC.width; height: 70; color: "#6E6E6E";

                    MouseArea { anchors.fill: parent;
                        onClicked: {
                            idUserInfoList.currentIndex = index;
                            idUserInfoList.focus = true;
                        }

                        onDoubleClicked: {
                            if ( !idPriv.user_mgr_vm.canAutoLoginUser ) { return; }

                            if ( idPriv.local_user_uid === model.uid )
                            {
                                IcUiQmlApi.postMetaCall( idPriv.user_mgr_vm, "cancelAutoLogin" );
                            }
                            else
                            {
                                IcUiQmlApi.postMetaCall( idPriv.user_mgr_vm, "setAutoLogin" );
                            }
                        }
                    }

                    RowLayout { anchors{ left: parent.left; leftMargin: 1; right: parent.right; rightMargin: 1; top: parent.top; bottom: parent.bottom; bottomMargin: 1; } spacing: 1;

                        Rectangle { Layout.preferredWidth: parent.width*0.07; Layout.fillHeight: true; color: idUserInfoList.currentIndex == index ? "#C7C7C7" : "#FFFFFF";

                            Label { anchors.fill: parent; text: index + 1; color: "#202020"; leftPadding: 10; rightPadding: 10; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter; }
                        }

                        Rectangle { Layout.preferredWidth: parent.width*0.28; Layout.fillHeight: true; color: idUserInfoList.currentIndex == index ? "#C7C7C7" : "#FFFFFF";

                            Label { anchors.fill: parent; text: model.name; color: "#202020"; leftPadding: 10; rightPadding: 10; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignLeft; }
                        }

                        Rectangle { Layout.preferredWidth: parent.width*0.4; Layout.fillHeight: true; color: idUserInfoList.currentIndex == index ? "#C7C7C7" : "#FFFFFF";

                            Label { anchors.fill: parent; text: model.memo; color: "#202020"; leftPadding: 10; rightPadding: 10; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignLeft; }
                        }

                        Rectangle { Layout.preferredWidth: parent.width*0.12; Layout.fillHeight: true; color: idUserInfoList.currentIndex == index ? "#C7C7C7" : "#FFFFFF";

                            Button { id: idUserGrpBtn; anchors.centerIn: parent; width: 130; height: 38; text: qsTr("User group");
                                background: Rectangle { implicitWidth: 130; implicitHeight: 38; border.color: "#6E6E6E"; border.width: 1; color: idUserGrpBtn.down ? "#bdc0c6" : "#E0E0E0"; radius: 5; }
                                contentItem: Label { text: idUserGrpBtn.text; color: idUserGrpBtn.down ? "#0064b6" : "#202020"; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter; }
                                onClicked: { idUserInfoList.currentIndex = index; idUserInfoList.focus = true; IcUiQmlApi.postMetaCall( idPriv.user_mgr_vm, "activeGroupList" ); }
                            }
                        }

                        Rectangle { Layout.fillWidth: true; Layout.fillHeight: true; color: idUserInfoList.currentIndex == index ? "#C7C7C7" : "#FFFFFF";

                            Button { id: idUserPermBtn; anchors.centerIn: parent; width: 130; height: 38; text: qsTr("User Permission");
                                background: Rectangle { implicitWidth: 130; implicitHeight: 38; border.color: "#6E6E6E"; border.width: 1; color: idUserPermBtn.down ? "#bdc0c6" : "#E0E0E0"; radius: 5; }
                                contentItem: Label { text: idUserPermBtn.text; color: idUserPermBtn.down ? "#0064b6" : "#202020"; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; wrapMode: Text.WordWrap; }
                                onClicked: { idUserInfoList.currentIndex = index; idUserInfoList.focus = true; IcUiQmlApi.postMetaCall( idPriv.user_mgr_vm, "activePermList" ); }
                            }
                        }
                    }

                    Image { width: 35; height: 35; source: "image://PermImProv/default/defaultLogin"; visible: idPriv.local_user_uid === model.uid; }
                }
            }
        }

        Label { Layout.leftMargin: 10; text: qsTr("Double-click the user to set automatic login."); color: "#202020"; font.pixelSize: 16;  }

        Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 60; Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom; color: "#333e44";

            RowLayout { anchors.fill: parent; anchors.leftMargin: 20; anchors.rightMargin: 20;

                RowLayout { Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter; spacing: 30;
                    Button { id: idBackBtn; Layout.preferredWidth: 100; Layout.preferredHeight: 38; Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter; text: qsTr("Back"); focusPolicy: Qt.NoFocus;
                        background: Rectangle { implicitWidth: 100; implicitHeight: 38; color: idBackBtn.down ? "#bdc0c6" : "#E0E0E0"; opacity: enabled ? 1 : 0.3; radius: 4; }
                        contentItem: Label { text: idBackBtn.text; color: idBackBtn.down ? "#0064b6" : "#202020"; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter; }
                    }

                    Button { id: idUserGrpBrowseBtn; Layout.preferredWidth: 160; Layout.preferredHeight: 38; Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter; text: qsTr("User group browse"); focusPolicy: Qt.NoFocus;
                        background: Rectangle { implicitWidth: 160; implicitHeight: 38; color: idUserGrpBrowseBtn.down ? "#bdc0c6" : "#E0E0E0"; opacity: enabled ? 1 : 0.3; radius: 4; }
                        contentItem: Label { text: idUserGrpBrowseBtn.text; color: idUserGrpBrowseBtn.down ? "#0064b6" : "#202020"; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; wrapMode: Text.WordWrap; }
                    }
                }

                RowLayout { Layout.alignment: Qt.AlignRight | Qt.AlignVCenter; spacing: 30;
                    Button { id: idCreateBtn; Layout.preferredWidth: 100; Layout.preferredHeight: 38; Layout.alignment: Qt.AlignRight | Qt.AlignVCenter; text: qsTr("Create"); focusPolicy: Qt.NoFocus; enabled: (idPriv.user_mgr_vm!==null)?idPriv.user_mgr_vm.canCreateUser:true;
                        background: Rectangle { implicitWidth: 100; implicitHeight: 38; color: idCreateBtn.down ? "#bdc0c6" : "#E0E0E0"; opacity: enabled ? 1 : 0.3; radius: 4; }
                        contentItem: Label { text: idCreateBtn.text; color: idCreateBtn.down ? "#0064b6" : "#202020"; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter; }
                    }

                    Button {
                        id: idModifyBtn; Layout.preferredWidth: 100; Layout.preferredHeight: 38; Layout.alignment: Qt.AlignRight | Qt.AlignVCenter; text: qsTr("Modify"); focusPolicy: Qt.NoFocus; enabled: (idPriv.user_mgr_vm!==null)?idPriv.user_mgr_vm.canModifyUser:true;
                        background: Rectangle { implicitWidth: 100; implicitHeight: 38; color: idModifyBtn.down ? "#bdc0c6" : "#E0E0E0"; opacity: enabled ? 1 : 0.3; radius: 4; }
                        contentItem: Label { text: idModifyBtn.text; color: idModifyBtn.down ? "#0064b6" : "#202020"; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter; }
                    }

                    Button { id: idDeleteBtn; Layout.preferredWidth: 100; Layout.preferredHeight: 38; Layout.alignment: Qt.AlignRight | Qt.AlignVCenter; text: qsTr("Delete"); focusPolicy: Qt.NoFocus; enabled: (idPriv.user_mgr_vm!==null)?idPriv.user_mgr_vm.canRemoveUser:true;
                        background: Rectangle { implicitWidth: 100; implicitHeight: 38; color: idDeleteBtn.down ? "#bdc0c6" : "#E0E0E0"; opacity: enabled ? 1 : 0.3; radius: 4; }
                        contentItem: Label { text: idDeleteBtn.text; color: idDeleteBtn.down ? "#0064b6" : "#202020"; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter; }
                    }

                    Button { id: idQuitBtn; Layout.preferredWidth: 100; Layout.preferredHeight: 38; Layout.alignment: Qt.AlignRight | Qt.AlignVCenter; text: qsTr("Quit"); focusPolicy: Qt.NoFocus;
                        background: Rectangle { implicitWidth: 100; implicitHeight: 38; color: idQuitBtn.down ? "#bdc0c6" : "#E0E0E0"; opacity: enabled ? 1 : 0.3; radius: 4; }
                        contentItem: Label { text: idQuitBtn.text; color: idQuitBtn.down ? "#0064b6" : "#202020"; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter; }
                    }
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
        property var user_mgr_vm: null;
        property var local_user_uid: null;

        // ==================================================================
        // initailize the page
        // ==================================================================
        function init( )
        {
            user_mgr_vm       = IcUiQmlApi.appCtrl.objMgr.attachObj( "FcPerm::UserMgrVm", true );
            initUI();
            doOnline(); idUserBrowseView.pageInit();
        }

        // ====================================================================
        // de-init the page
        // ====================================================================
        function deinit( )
        {
            doOffline( );
            idUserInfoList.model = null;
            IcUiQmlApi.appCtrl.objMgr.detachObj( "FcPerm::UserMgrVm", user_mgr_vm );
            idUserBrowseView.pageDeinit();
        }

        function doOffline( )
        {
            user_mgr_vm.showGroupList.disconnect(onShowGroupList);
            user_mgr_vm.showPermList.disconnect(onShowPermList);
            user_mgr_vm.showNewUserView.disconnect(turnNewUserView);
            user_mgr_vm.showModUserView.disconnect(turnModUserView);
            idBackBtn.clicked.disconnect( turnUserLoginView );
            idQuitBtn.clicked.disconnect( turnUserLoginView );
            idUserGrpBrowseBtn.clicked.disconnect( turnUserGrpBrowseView );
            idCreateBtn.clicked.disconnect( checkCreate );
            idModifyBtn.clicked.disconnect( checkModify );
            idDeleteBtn.clicked.disconnect( checkRemove );
        }

        function doOnline( )
        {
            user_mgr_vm.showGroupList.connect(onShowGroupList);
            user_mgr_vm.showPermList.connect(onShowPermList);
            user_mgr_vm.showNewUserView.connect(turnNewUserView);
            user_mgr_vm.showModUserView.connect(turnModUserView);
            idBackBtn.clicked.connect( turnUserLoginView );
            idQuitBtn.clicked.connect( turnUserLoginView );
            idUserGrpBrowseBtn.clicked.connect( turnUserGrpBrowseView );
            idCreateBtn.clicked.connect( checkCreate );
            idModifyBtn.clicked.connect( checkModify );
            idDeleteBtn.clicked.connect( checkRemove );
        }

        function checkCreate( )
        {
            IcUiQmlApi.postMetaCall(idPriv.user_mgr_vm, "checkCreate");
        }

        function checkModify( )
        {
            IcUiQmlApi.postMetaCall(idPriv.user_mgr_vm, "checkModify");
        }

        function checkRemove( )
        {
            IcUiQmlApi.postMetaCall(idPriv.user_mgr_vm, "checkRemove");
        }

        function turnUserLoginView( )
        {
            idUserBrowseView.request( "UserLoginView", { } );
        }

        function turnUserGrpBrowseView( )
        {
            idUserBrowseView.request( "UserGrpBrowseView", { } );
        }

        function turnNewUserView( )
        {
            idUserBrowseView.request( "NewUserView", { } );
        }

        function turnModUserView( )
        {
            idUserBrowseView.request( "ModUserView", { } );
        }

        function initUI()
        {
            idUserInfoList.model = user_mgr_vm.userListInfo;
            idPriv.user_mgr_vm.userListInfo.selectIndex = Qt.binding( function (){ return idUserInfoList.currentIndex; });
            idCreateBtn.enabled  = Qt.binding( function() { return user_mgr_vm.canCreateUser; });
            idModifyBtn.enabled  = Qt.binding( function() { return user_mgr_vm.canModifyUser && idUserInfoList.currentIndex >= 0; });
            idDeleteBtn.enabled  = Qt.binding( function() { return user_mgr_vm.canRemoveUser && idUserInfoList.currentIndex >= 0; });
            idPriv.local_user_uid = Qt.binding( function() { return idPriv.user_mgr_vm.localUser.uid; } );
        }

        function onShowGroupList( )
        {
            //[HINT] below code are demo. how to dyn. create component and object

            // a) before use the spec. custom control, create it's component
            var cmpt = Qt.createComponent("qrc:/perm/view/grouplist.qml");
            if ( cmpt.status !== Component.Ready ) { console.error("create confirmdiag failed!"); return; }

            // b) now create the object, set it parent to this control
            var cfm_diag = cmpt.createObject( idUserBrowseView );

            // c) setup information about this confirm object
            cfm_diag.contentWidth = idUserBrowseView.width; cfm_diag.contentHeight = idUserBrowseView.height;

            cfm_diag.destroyOnClose = true;

            // d) now enter the modal dialog event...
            cfm_diag.open();

            cfm_diag = null;
        }

        function onShowPermList( )
        {
            //[HINT] below code are demo. how to dyn. create component and object

            // a) before use the spec. custom control, create it's component
            var cmpt = Qt.createComponent("qrc:/perm/view/permlist.qml");
            if ( cmpt.status !== Component.Ready ) { console.error("create confirmdiag failed!"); return; }

            // b) now create the object, set it parent to this control
            var cfm_diag = cmpt.createObject( idUserBrowseView );

            // c) setup information about this confirm object
            cfm_diag.contentWidth = idUserBrowseView.width; cfm_diag.contentHeight = idUserBrowseView.height;

            cfm_diag.destroyOnClose = true;

            // d) now enter the modal dialog event...
            cfm_diag.open();

            cfm_diag = null;
        }
    }
}
