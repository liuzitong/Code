import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import qxpack.indcom.ui_qml_base 1.0     // [HINT] this is the pre-registered module name.
import qxpack.indcom.ui_qml_control 1.0  // [HINT] ModalPopupDialog is in it

ModalPopupDialog {   // this is the wrapped Popup element in ui_qml_control
    id: idPopup;

    property alias contentWidth : idContent.implicitWidth;
    property alias contentHeight: idContent.implicitHeight;

    // ////////////////////////////////////////////////////////////////////////
    // layout
    // ////////////////////////////////////////////////////////////////////////
    backgroundColor: "red"; backgroundVisible: false;

    contentItem: Rectangle {
        // [HINT] Popup element need implicitWidth & implicitHeight to calc. the right position
        id: idContent; implicitWidth: 586; implicitHeight: 510; color: "#40000000";

        MouseArea { anchors.fill: parent;
            onClicked: { idPopup.close(); }
        }

        Rectangle { anchors.centerIn: parent; implicitWidth: 620; implicitHeight: 590; color: "#FAFAFA"; border.color: "#6E6E6E"
            // ColumnLayout arrange text area and control area.
            ColumnLayout { anchors.fill: parent; anchors.margins: 20; spacing: 20;

                Label { Layout.alignment: Qt.AlignHCenter | Qt.AlignTop; text: qsTr("user permission table"); font.pixelSize: 22;  color: "#202020"; }

                Rectangle { Layout.preferredWidth: 550; Layout.preferredHeight: 390; Layout.alignment: Qt.AlignHCenter; border.color: "#6E6E6E"; border.width: 1; color: "#FAFAFA";

                    Rectangle { anchors.left: parent.left; anchors.leftMargin: 250; width: 1; height: parent.height; color: "#6E6E6E"; }

                    Rectangle { anchors.left: parent.left; anchors.leftMargin: 350; width: 1; height: parent.height; color: "#6E6E6E"; }

                    Rectangle { anchors.left: parent.left; anchors.leftMargin: 450; width: 1; height: parent.height; color: "#6E6E6E"; }

                    Rectangle { anchors.top: parent.top; anchors.topMargin: 50; width: 550; height: 1; color: "#6E6E6E"; }

                    RowLayout { width: 550; height: 50; spacing: 0;

                        Label { Layout.preferredWidth: 250; Layout.fillHeight: true; text: qsTr("permissions"); padding: 10; color: "#202020"; font.pixelSize: 16;  horizontalAlignment: Text.AlignLeft; verticalAlignment: Text.AlignVCenter; }

                        Label { Layout.preferredWidth: 100; Layout.fillHeight: true; text: qsTr("inherit"); padding: 10; color: "#202020"; font.pixelSize: 16;  horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter; }

                        Label { Layout.preferredWidth: 100; Layout.fillHeight: true; text: qsTr("enable"); padding: 10; color: "#202020"; font.pixelSize: 16;  horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter; }

                        Label { Layout.preferredWidth: 100; Layout.fillHeight: true; text: qsTr("disable"); padding: 10; color: "#202020"; font.pixelSize: 16;  horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter; }
                    }

                    ListView { id: idPermList; anchors.top: parent.top; anchors.topMargin: 50; anchors.bottom: parent.bottom; anchors.left: parent.left; anchors.right: parent.right; clip: true; model: idListModel;
                        ScrollBar.vertical: ScrollBar{ orientation: Qt.Vertical; policy: idPermList.contentHeight > idPermList.height ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff;
                            contentItem: Rectangle { implicitWidth: 6; implicitHeight: 54; color: "#6E6E6E"; radius: 3; }
                        }
                        delegate: RowLayout { width: 550; height: 50; spacing: 0;

                            ButtonGroup { id: idCheckBoxGroup; }

                            Label { Layout.preferredWidth: 250; Layout.fillHeight: true; text: perm; padding: 10; color: "#202020"; font.pixelSize: 16;  horizontalAlignment: Text.AlignLeft; verticalAlignment: Text.AlignVCenter; }

                            Item { Layout.preferredWidth: 100; Layout.fillHeight: true;
                                CheckBox { id: idInheritChk; anchors.centerIn: parent; width: 36; height: 36; enabled: isInherit; checked: (isInherit && !isPriv) || (isInherit && isPriv && isEnable); ButtonGroup.group: idCheckBoxGroup;
                                    indicator: Image { width: 36; height: 36; source: idInheritChk.checked ? "qrc:/images/check_box_yes.svg" : "qrc:/images/check_box_no.svg"; opacity: idInheritChk.enabled ? 1 : 0.4; }

                                    onClicked: { isPriv = false; }
                                }
                            }

                            Item { Layout.preferredWidth: 100; Layout.fillHeight: true;
                                CheckBox { id: idEnableChk; anchors.centerIn: parent; width: 36; height: 36; enabled: !isInherit; checked: (!isInherit && isPriv && isEnable); ButtonGroup.group: idCheckBoxGroup;
                                    indicator: Image { width: 36; height: 36; source: idEnableChk.checked ? "qrc:/images/check_box_yes.svg" : "qrc:/images/check_box_no.svg"; opacity: idEnableChk.enabled ? 1 : 0.4; }

                                    onCheckedChanged: { isEnable = true; isPriv = true; }
                                }
                            }

                            Item { Layout.preferredWidth: 100; Layout.fillHeight: true;
                                CheckBox { id: idDisableChk; anchors.centerIn: parent; width: 36; height: 36; checked: (isPriv && !isEnable) || (isInherit && !isEnable); ButtonGroup.group: idCheckBoxGroup;
                                    indicator: Image { width: 36; height: 36; source: idDisableChk.checked ? "qrc:/images/check_box_yes.svg" : "qrc:/images/check_box_no.svg"; }

                                    onClicked: { isEnable = false; isPriv = true; if ( isInherit ) { isPriv = true; } }
                                }
                            }
                        }
                    }
                }

                Item { Layout.fillWidth: true; Layout.preferredHeight: 55;

                    RowLayout { anchors.centerIn: parent; spacing: 50;

                        Button { id: idOkBtn; Layout.preferredWidth: 128; Layout.preferredHeight: 54; Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter; text: qsTr("OK"); focusPolicy: Qt.NoFocus;
                            background: Rectangle { implicitWidth: 128; implicitHeight: 80; color: idOkBtn.down ? "#bdc0c6" : "#E0E0E0"; }
                            contentItem: Label { text: idOkBtn.text; color: idOkBtn.down ? "#0064b6" : "#202020"; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter; }

                            onClicked: { idPopup.close(); }
                        }

                        Button { id: idCancelBtn; Layout.preferredWidth: 128; Layout.preferredHeight: 54; Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter; text: qsTr("Cancel"); focusPolicy: Qt.NoFocus;
                            background: Rectangle { implicitWidth: 128; implicitHeight: 80; color: idCancelBtn.down ? "#bdc0c6" : "#E0E0E0"; }
                            contentItem: Label { text: idCancelBtn.text; color: idCancelBtn.down ? "#0064b6" : "#202020"; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter; }

                            onClicked: { idPopup.close(); }
                        }
                    }
                }
            }
        }
    }

    ListModel { id: idListModel; }

    // ////////////////////////////////////////////////////////////////////////
    // logic
    // ////////////////////////////////////////////////////////////////////////
    Component.onCompleted:   { idPriv.init();   }
    Component.onDestruction: { idPriv.deinit(); }

    QtObject {
        id: idPriv;
        property var user_perm_list_vm: null;
        property var perm_list: new Object;

        // ==================================================================
        // initailize the page
        // ==================================================================
        function init( )
        {
            user_perm_list_vm       = IcUiQmlApi.appCtrl.objMgr.attachObj( "FcPerm::UserPermListVm", true );
            idOkBtn.clicked.connect( done );
            var permList = user_perm_list_vm.editableUserPermListInfo.permList;
            for ( var i = 0; i < permList.length; i++ )
            {
                idListModel.append({"isInherit": permList[i]["isInherit"], "isPriv": permList[i]["isPriv"], "isEnable": permList[i]["isEnable"], "perm": permList[i]["perm"]});
            }
        }

        // ====================================================================
        // de-init the page
        // ====================================================================
        function deinit( )
        {
            idOkBtn.clicked.disconnect( done );
            idListModel.clear();
            IcUiQmlApi.appCtrl.objMgr.detachObj( "FcPerm::UserPermListVm", user_perm_list_vm );
        }

        function done( )
        {
            var permList = new Array;
            for ( var i = 0; i < idListModel.count; i++ )
            {
                if ( idListModel.get(i).isPriv )
                {
                    var elem = new Object;
                    elem.perm = idListModel.get(i).perm;
                    elem.isEnable = idListModel.get(i).isEnable;
                    permList.push(elem);
                }
            }

            var data = new Object;
            data.permList = permList;

            user_perm_list_vm.permInfo = data;
        }
    }
}
