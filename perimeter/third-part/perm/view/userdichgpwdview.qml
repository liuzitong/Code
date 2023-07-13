import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import perm.view 1.0             as   FcPerm
import qxpack.indcom.ui_qml_control 1.0
import qxpack.indcom.ui_qml_base 1.0 // [HINT] this is the pre-registered module name.

IcPageBase {   // this is the wrapped Popup element in ui_qml_control
    id: idUserDiChgPwdView; pageName: "UserDiChgPwd";
    implicitWidth: FcPerm.PermSkin.winRectWidth;  implicitHeight: FcPerm.PermSkin.winRectHeight;

    property int editItemWidth: width*0.4;  property int rowH: 38;

    Rectangle { anchors.fill: parent; color: "#F1F1F2"; }

    ColumnLayout { anchors.fill: parent; spacing: 4;

        Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 82; Layout.alignment: Qt.AlignHCenter | Qt.AlignTop; color: "#333e44"

            Label { anchors.centerIn: parent; text: qsTr("Reset password"); font.pixelSize: 24;  color: "#FAFAFA"; }
        }

        Item { Layout.fillHeight: true; Layout.fillWidth: true;

            ColumnLayout { Layout.fillWidth: true; anchors.verticalCenter: parent.verticalCenter; spacing: 100*FcPerm.PermSkin.winHeightScale;

                Item { Layout.fillWidth: true; Layout.preferredHeight: rowH; Layout.alignment: Qt.AlignHCenter;

                    Label { anchors.right: idUserNameText.left; anchors.rightMargin: 20; anchors.verticalCenter: parent.verticalCenter; text: qsTr("user name"); color: "#202020"; font.pixelSize: 16;  }

                    TextField { id: idUserNameText; objectName: "UserInputItem.TextField"; anchors.left: parent.left; anchors.leftMargin: editItemWidth; width: editItemWidth; height: rowH; placeholderText: qsTr("Please enter user name"); font.pixelSize: 16;  selectByMouse: true; selectionColor: "lightblue";
                        background: Rectangle { implicitWidth: parent.width; implicitHeight: rowH; border.color: "#6E6E6E"; border.width: 1; color: "#FAFAFA"; radius: 5; }
                    }
                }

                Item { Layout.fillWidth: true; Layout.preferredHeight: rowH; Layout.alignment: Qt.AlignHCenter;

                    Label { anchors.right: idOldPwdText.left; anchors.rightMargin: 20; anchors.verticalCenter: parent.verticalCenter; text: qsTr("old password"); color: "#202020"; font.pixelSize: 16;  }

                    TextField { id: idOldPwdText; objectName: "UserInputItem.TextField"; anchors.left: parent.left; anchors.leftMargin: editItemWidth; width: editItemWidth; height: rowH; placeholderText: qsTr("Please enter old password"); font.pixelSize: 16;  selectByMouse: true; selectionColor: "lightblue";
                        background: Rectangle { implicitWidth: parent.width; implicitHeight: rowH; border.color: "#6E6E6E"; border.width: 1; color: "#FAFAFA"; radius: 5; }
                    }
                }

                Item { Layout.fillWidth: true; Layout.preferredHeight: rowH; Layout.alignment: Qt.AlignHCenter;

                    Label { anchors.right: idNewPwdText.left; anchors.rightMargin: 20; anchors.verticalCenter: parent.verticalCenter; text: qsTr("new password"); color: "#202020"; font.pixelSize: 16;  }

                    TextField { id: idNewPwdText; objectName: "UserInputItem.TextField"; anchors.left: parent.left; anchors.leftMargin: editItemWidth; width: editItemWidth; height: rowH; placeholderText: qsTr("6-20 digits or letters"); font.pixelSize: 16;  selectByMouse: true; selectionColor: "lightblue";
                        validator: RegExpValidator { regExp: /[0-9a-zA-Z]{6,20}/ }
                        background: Rectangle { implicitWidth: parent.width; implicitHeight: rowH; border.color: "#6E6E6E"; border.width: 1; color: "#FAFAFA"; radius: 5; }
                    }
                }

                Item { Layout.fillWidth: true; Layout.preferredHeight: rowH; Layout.alignment: Qt.AlignHCenter;

                    Label { anchors.right: idNewPwdAgainText.left; anchors.rightMargin: 20; anchors.verticalCenter: parent.verticalCenter; text: qsTr("confirm password"); color: "#202020"; font.pixelSize: 16;  }

                    TextField { id: idNewPwdAgainText; objectName: "UserInputItem.TextField"; anchors.left: parent.left; anchors.leftMargin: editItemWidth; width: editItemWidth; height: rowH; placeholderText: qsTr("Please enter password again"); font.pixelSize: 16;  selectByMouse: true; selectionColor: "lightblue";
                        validator: RegExpValidator { regExp: /[0-9a-zA-Z]{6,20}/ }
                        background: Rectangle { implicitWidth: parent.width; implicitHeight: rowH; border.color: "#6E6E6E"; border.width: 1; color: "#FAFAFA"; radius: 5; }
                    }
                }
            }
        }

        Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 60; Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom; color: "#333e44";

            RowLayout { anchors.fill: parent; anchors.leftMargin: 5; anchors.rightMargin: 5;

                Button { id: idBackBtn; Layout.preferredWidth: 100; Layout.preferredHeight: 38; Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter; text: qsTr("Back"); focusPolicy: Qt.NoFocus;
                    background: Rectangle { implicitWidth: 100; implicitHeight: 38; color: idBackBtn.down ? "#bdc0c6" : "#E0E0E0"; opacity: enabled ? 1 : 0.3; radius: 4; }
                    contentItem: Label { text: idBackBtn.text; color: idBackBtn.down ? "#0064b6" : "#202020"; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter; }
                }

                Item { Layout.fillWidth: true; Layout.fillHeight: true; }

                Button { id: idOkBtn; Layout.preferredWidth: 100; Layout.preferredHeight: 38; Layout.alignment: Qt.AlignRight | Qt.AlignVCenter; text: qsTr("OK"); focusPolicy: Qt.NoFocus;
                    background: Rectangle { implicitWidth: 100; implicitHeight: 38; color: idOkBtn.down ? "#bdc0c6" : "#E0E0E0"; opacity: enabled ? 1 : 0.3; radius: 4; }
                    contentItem: Label { text: idOkBtn.text; color: idOkBtn.down ? "#0064b6" : "#202020"; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter; }
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
        property var user_info_chg_vm: null;

        // ==================================================================
        // initailize the page
        // ==================================================================
        function init( )
        {
            user_info_chg_vm = IcUiQmlApi.appCtrl.objMgr.attachObj( "FcPerm::UserInfoChgVm", true );
            initUI(); doOnline();
            idUserDiChgPwdView.pageInit();
        }

        // ====================================================================
        // de-init the page
        // ====================================================================
        function deinit( )
        {
            doOffline( );
            IcUiQmlApi.appCtrl.objMgr.detachObj( "FcPerm::UserInfoChgVm", user_info_chg_vm );
            idUserDiChgPwdView.pageDeinit();
        }

        function doOffline( )
        {
            idBackBtn.clicked.disconnect( turnUserLoginView );
            idOkBtn.clicked.disconnect( changUserPwd );
        }

        function doOnline( )
        {
            idBackBtn.clicked.connect( turnUserLoginView );
            idOkBtn.clicked.connect( changUserPwd );
        }

        function turnUserLoginView( )
        {
            idUserDiChgPwdView.request("UserLoginView", { });
        }

        function initUI()
        {
            idUserNameText.text = Qt.binding( function() { return idPriv.user_info_chg_vm.editUserInfo.name; } );
            idOldPwdText.text   = Qt.binding( function() { return idPriv.user_info_chg_vm.editUserInfo.oldPwd; } );
            idNewPwdText.text   = Qt.binding( function() { return idPriv.user_info_chg_vm.editUserInfo.newPwd; } );
            idNewPwdAgainText.text = Qt.binding( function() { return idPriv.user_info_chg_vm.editUserInfo.newPwd; } );

            idOkBtn.enabled = Qt.binding( function() {
                return idNewPwdText.text.length >= 6 && idNewPwdText.text === idNewPwdAgainText.text;
            });
        }

        function changUserPwd( )
        {
            var data = new Object;
            data.name = idUserNameText.text;
            data.oldPwd = idOldPwdText.text;
            data.newPwd = idNewPwdText.text;
            user_info_chg_vm.userAndPwd = data;
            IcUiQmlApi.postMetaCall( user_info_chg_vm, "diChgUserPwd" );
        }
    }
}
