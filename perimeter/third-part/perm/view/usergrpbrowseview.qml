import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

import perm.view 1.0             as   FcPerm
import qxpack.indcom.ui_qml_control 1.0
import qxpack.indcom.ui_qml_base 1.0 // [HINT] this is the pre-registered module name.

IcPageBase {   // this is the wrapped Popup element in ui_qml_control
    id: idUserGrpBrowseView; pageName: "UserGrpBrowse";
    implicitWidth: FcPerm.PermSkin.winRectWidth;  implicitHeight: FcPerm.PermSkin.winRectHeight;

    property int editItemWidth: width*0.4;

    Rectangle { anchors.fill: parent; color: "#F1F1F2"; }

    ColumnLayout { anchors.fill: parent; spacing: 4;

        Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 84; Layout.alignment: Qt.AlignHCenter | Qt.AlignTop; color: "#333e44";

            Label { anchors.centerIn: parent; text: qsTr("User Group Browse"); font.pixelSize: 24;  color: "#FAFAFA"; }
        }

        ColumnLayout { id: idContC; Layout.fillHeight: true; width: FcPerm.PermSkin.winRectWidth-20; spacing: 0; anchors.horizontalCenter: parent.horizontalCenter;

            Rectangle { Layout.preferredWidth: parent.width; Layout.preferredHeight: 38; Layout.alignment: Qt.AlignHCenter; color: "#6e6e6e";
                RowLayout { anchors.fill: parent; anchors.margins: 1; spacing: 1;

                    Rectangle { Layout.preferredWidth: parent.width*0.07; Layout.fillHeight: true; color: "#e0e0e0"; }

                    Rectangle { Layout.preferredWidth: parent.width*0.3; Layout.fillHeight: true; color: "#e0e0e0";
                        Label { anchors.fill: parent; text: qsTr("user group name"); padding: 10; font.pixelSize: 16; clip: true; elide: Text.ElideRight;  color: "#202020"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter; }
                    }

                    Rectangle { Layout.preferredWidth: parent.width*0.5; Layout.fillHeight: true; color: "#e0e0e0";
                        Label { anchors.fill: parent; text: qsTr("memo"); padding: 10; font.pixelSize: 16; clip: true; elide: Text.ElideRight;  color: "#202020"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter; }
                    }

                    Rectangle { Layout.fillWidth: true; Layout.fillHeight: true; color: "#e0e0e0"; }
                }
            }

            ListView { id: idUserGrpInfoList; Layout.preferredWidth: parent.width; Layout.fillHeight: true; Layout.alignment: Qt.AlignHCenter; focus: true; clip: true;
                delegate: Rectangle { width: idContC.width; height: 70; color: "#6E6E6E";

                    MouseArea { anchors.fill: parent;
                        onClicked: { idUserGrpInfoList.currentIndex = index; idUserGrpInfoList.focus = true; }
                    }

                    RowLayout { anchors{ left: parent.left; leftMargin: 1; right: parent.right; rightMargin: 1; top: parent.top; bottom: parent.bottom; bottomMargin: 1; }  spacing: 1;

                        Rectangle { Layout.preferredWidth: parent.width*0.07; Layout.fillHeight: true; color: idUserGrpInfoList.currentIndex == index ? "#C7C7C7" : "#FFFFFF";

                            Label { anchors.fill: parent; text: index + 1; color: "#202020"; leftPadding: 10; rightPadding: 10; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter; }
                        }

                        Rectangle { Layout.preferredWidth: parent.width*0.3; Layout.fillHeight: true; color: idUserGrpInfoList.currentIndex == index ? "#C7C7C7" : "#FFFFFF";

                            Label { anchors.fill: parent; text: model.name; color: "#202020"; leftPadding: 10; rightPadding: 10; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignLeft; }
                        }

                        Rectangle { Layout.preferredWidth: parent.width*0.5; Layout.fillHeight: true; color: idUserGrpInfoList.currentIndex == index ? "#C7C7C7" : "#FFFFFF";

                            Label { anchors.fill: parent; text: model.memo; color: "#202020"; leftPadding: 10; rightPadding: 10; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignLeft; }
                        }

                        Rectangle { Layout.fillWidth: true; Layout.fillHeight: true; color: idUserGrpInfoList.currentIndex == index ? "#C7C7C7" : "#FFFFFF";

                            Button { id: idUserPermBtn; anchors.centerIn: parent; width: 130; height: 38; text: qsTr("User Group Permissions");
                                background: Rectangle { implicitWidth: 130; implicitHeight: 38; border.color: "#6E6E6E"; border.width: 1; color: idUserPermBtn.down ? "#bdc0c6" : "#E0E0E0"; radius: 5; }
                                contentItem: Label { text: idUserPermBtn.text; color: idUserPermBtn.down ? "#0064b6" : "#202020"; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; wrapMode: Text.WordWrap; }
                                onClicked: { idUserGrpInfoList.currentIndex = index; idUserGrpInfoList.focus = true; IcUiQmlApi.postMetaCall( idPriv.user_grp_mgr_vm, "activeGrpPermList" ); }
                            }
                        }
                    }
                }
            }
        }

        Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 60; Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom; color: "#333e44";

            RowLayout { anchors.fill: parent; anchors.leftMargin: 20; anchors.rightMargin: 20; spacing: 40;

                Button { id: idBackBtn; Layout.preferredWidth: 100; Layout.preferredHeight: 38; Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter; text: qsTr("Back"); focusPolicy: Qt.NoFocus;
                    background: Rectangle { implicitWidth: 100; implicitHeight: 38; color: idBackBtn.down ? "#bdc0c6" : "#E0E0E0"; opacity: enabled ? 1 : 0.3;  radius: 4; }
                    contentItem: Label { text: idBackBtn.text; color: idBackBtn.down ? "#0064b6" : "#202020"; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter; }
                }

                Button { id: idUserBrowseBtn; Layout.preferredWidth: 160; Layout.preferredHeight: 38; Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter; text: qsTr("User browse"); focusPolicy: Qt.NoFocus;
                    background: Rectangle { implicitWidth: 160; implicitHeight: 38; color: idUserBrowseBtn.down ? "#bdc0c6" : "#E0E0E0"; opacity: enabled ? 1 : 0.3; radius: 4; }
                    contentItem: Label { text: idUserBrowseBtn.text; color: idUserBrowseBtn.down ? "#0064b6" : "#202020"; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; wrapMode: Text.WordWrap; }
                }

                Item { Layout.fillWidth: true; Layout.fillHeight: true; }

                Button { id: idCreateBtn; Layout.preferredWidth: 100; Layout.preferredHeight: 38; Layout.alignment: Qt.AlignRight | Qt.AlignVCenter; text: qsTr("Create"); focusPolicy: Qt.NoFocus;
                    background: Rectangle { implicitWidth: 100; implicitHeight: 38; color: idCreateBtn.down ? "#bdc0c6" : "#E0E0E0"; opacity: enabled ? 1 : 0.3;  radius: 4; }
                    contentItem: Label { text: idCreateBtn.text; color: idCreateBtn.down ? "#0064b6" : "#202020"; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter; }
                }

                Button { id: idModifyBtn; Layout.preferredWidth: 100; Layout.preferredHeight: 38; Layout.alignment: Qt.AlignRight | Qt.AlignVCenter; text: qsTr("Modify"); focusPolicy: Qt.NoFocus;
                    background: Rectangle { implicitWidth: 100; implicitHeight: 38; color: idModifyBtn.down ? "#bdc0c6" : "#E0E0E0"; opacity: enabled ? 1 : 0.3;  radius: 4; }
                    contentItem: Label { text: idModifyBtn.text; color: idModifyBtn.down ? "#0064b6" : "#202020"; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter; }
                }

                Button { id: idDeleteBtn; Layout.preferredWidth: 100; Layout.preferredHeight: 38; Layout.alignment: Qt.AlignRight | Qt.AlignVCenter; text: qsTr("Delete"); focusPolicy: Qt.NoFocus;
                    background: Rectangle { implicitWidth: 100; implicitHeight: 38; color: idDeleteBtn.down ? "#bdc0c6" : "#E0E0E0"; opacity: enabled ? 1 : 0.3;  radius: 4; }
                    contentItem: Label { text: idDeleteBtn.text; color: idDeleteBtn.down ? "#0064b6" : "#202020"; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter; }
                }

                Button { id: idQuitBtn; Layout.preferredWidth: 100; Layout.preferredHeight: 38; Layout.alignment: Qt.AlignRight | Qt.AlignVCenter; text: qsTr("Quit"); focusPolicy: Qt.NoFocus;
                    background: Rectangle { implicitWidth: 100; implicitHeight: 38; color: idQuitBtn.down ? "#bdc0c6" : "#E0E0E0"; opacity: enabled ? 1 : 0.3;  radius: 4; }
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
        property var user_grp_mgr_vm: null;

        // ==================================================================
        // initailize the page
        // ==================================================================
        function init( )
        {
            user_grp_mgr_vm       = IcUiQmlApi.appCtrl.objMgr.attachObj( "FcPerm::UserGrpMgrVm", true );
            initUI();
            doOnline(); idUserGrpBrowseView.pageInit();
        }

        // ====================================================================
        // de-init the page
        // ====================================================================
        function deinit( )
        {
            doOffline( );
            idUserGrpInfoList.model = null;
            IcUiQmlApi.appCtrl.objMgr.detachObj( "FcPerm::UserGrpMgrVm", user_grp_mgr_vm );
            idUserGrpBrowseView.pageDeinit();
        }

        function doOffline( )
        {
            user_grp_mgr_vm.showGrpPermList.disconnect(onShowGrpPermList);
            user_grp_mgr_vm.showNewUserGrpView.disconnect(turnNewUserGrpView);
            user_grp_mgr_vm.showModUserGrpView.disconnect(turnModUserGrpView);
            idBackBtn.clicked.disconnect( backView );
            idQuitBtn.clicked.disconnect( turnUserLoginView );
            idUserBrowseBtn.clicked.disconnect( turnUserBrowseView );
            idCreateBtn.clicked.disconnect( checkCreate );
            idModifyBtn.clicked.disconnect( checkModify);
            idDeleteBtn.clicked.disconnect( checkRemove );
        }

        function doOnline( )
        {
            user_grp_mgr_vm.showGrpPermList.connect(onShowGrpPermList);
            user_grp_mgr_vm.showNewUserGrpView.connect(turnNewUserGrpView);
            user_grp_mgr_vm.showModUserGrpView.connect(turnModUserGrpView);
            idBackBtn.clicked.connect( backView );
            idQuitBtn.clicked.connect( turnUserLoginView );
            idUserBrowseBtn.clicked.connect( turnUserBrowseView );
            idCreateBtn.clicked.connect( checkCreate );
            idModifyBtn.clicked.connect( checkModify );
            idDeleteBtn.clicked.connect( checkRemove );
        }

        function checkCreate( )
        {
            IcUiQmlApi.postMetaCall(idPriv.user_grp_mgr_vm, "checkCreate");
        }

        function checkModify( )
        {
            IcUiQmlApi.postMetaCall(idPriv.user_grp_mgr_vm, "checkModify");
        }

        function checkRemove( )
        {
            IcUiQmlApi.postMetaCall(idPriv.user_grp_mgr_vm, "checkRemove");
        }

        function backView( )
        {
            idUserGrpBrowseView.request( "Back", { } );
        }

        function turnUserLoginView( )
        {
            idUserGrpBrowseView.request( "UserLoginView", { } );
        }

        function turnUserBrowseView( )
        {
            idUserGrpBrowseView.request( "UserBrowseView", { } );
        }

        function turnNewUserGrpView( )
        {
            idUserGrpBrowseView.request( "NewUserGrpView", { } );
        }

        function turnModUserGrpView( )
        {
            idUserGrpBrowseView.request( "ModUserGrpView", { } );
        }

        function initUI()
        {
            idUserGrpInfoList.model = user_grp_mgr_vm.userGrpListInfo;
            idPriv.user_grp_mgr_vm.userGrpListInfo.selectIndex = Qt.binding( function (){ return idUserGrpInfoList.currentIndex; });
            idCreateBtn.enabled  = Qt.binding( function() { return user_grp_mgr_vm.canCreateGroup; });
            idModifyBtn.enabled  = Qt.binding( function() { return user_grp_mgr_vm.canModifyGroup && idUserGrpInfoList.currentIndex >= 0; });
            idDeleteBtn.enabled  = Qt.binding( function() { return user_grp_mgr_vm.canRemoveGroup && idUserGrpInfoList.currentIndex >= 0; });
        }

        function onShowGrpPermList( )
        {
            //[HINT] below code are demo. how to dyn. create component and object

            // a) before use the spec. custom control, create it's component
            var cmpt = Qt.createComponent("qrc:/perm/view/grppermlist.qml");
            if ( cmpt.status !== Component.Ready ) { console.error("create confirmdiag failed!"); return; }

            // b) now create the object, set it parent to this control
            var cfm_diag = cmpt.createObject( idUserGrpBrowseView );

            // c) setup information about this confirm object
            cfm_diag.contentWidth = idUserGrpBrowseView.width; cfm_diag.contentHeight = idUserGrpBrowseView.height;

            cfm_diag.destroyOnClose = true;

            // d) now enter the modal dialog event...
            cfm_diag.open();

            cfm_diag = null;
        }
    }
}
