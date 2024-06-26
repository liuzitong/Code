﻿import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import qxpack.indcom.ui_qml_base 1.0     // [HINT] this is the pre-registered module name.
import qxpack.indcom.ui_qml_control 1.0  // [HINT] ModalPopupDialog is in it

ModalPopupDialog {   // this is the wrapped Popup element in ui_qml_control
    id: idPopup;

    property alias contentWidth : idContent.implicitWidth;
    property alias contentHeight: idContent.implicitHeight;
    readonly property string rolePerms: "Perm.root;Perm.sys;Perm.admin;Perm.manager;Perm.chkPerm";

    // ////////////////////////////////////////////////////////////////////////
    // layout
    // ////////////////////////////////////////////////////////////////////////
    backgroundColor: "red"; backgroundVisible: false;

    contentItem: Rectangle {
        // [HINT] Popup element need implicitWidth & implicitHeight to calc. the right position
        id: idContent; implicitWidth: 586; implicitHeight: 510; color: "#60606060";

        MouseArea { anchors.fill: parent;

            onClicked: { idPopup.close(); }
        }

        Rectangle { anchors.centerIn: parent; implicitWidth: 480; implicitHeight: 500; color: "#FAFAFA"; border.color: "#6E6E6E"
            // ColumnLayout arrange text area and control area.
            ColumnLayout { anchors.fill: parent; anchors.margins: 20; spacing: 20;

                Label { Layout.alignment: Qt.AlignHCenter | Qt.AlignTop; text: qsTr("user group permission table"); font.pixelSize: 22;  color: "#202020"; }

                Rectangle { Layout.preferredWidth: 400; Layout.preferredHeight: 1; Layout.alignment: Qt.AlignHCenter; color: "#6E6E6E"; }

                ListView { id: idPermList; Layout.preferredWidth: 400; Layout.fillHeight: true; Layout.alignment: Qt.AlignHCenter; clip: true; spacing: 20; model: idListModel;
                    ScrollBar.vertical: ScrollBar{ orientation: Qt.Vertical; policy: idPermList.contentHeight > idPermList.height ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff;
                        contentItem: Rectangle { implicitWidth: 6; implicitHeight: 54; color: "#6E6E6E"; radius: 3; }
                    }
                    delegate: Item { width: 400; height: 36

                        CheckBox { id: idGrpChk; anchors.left: parent.left; anchors.leftMargin: 100; implicitHeight: 36; checked: isSelected; text: permName; ButtonGroup.group: rolePerms.indexOf(permName) >=0 ? idRoleGroup : null;
                            indicator: Image { width: 36; height: 36; source: idGrpChk.checked ? "qrc:/images/check_box_yes.svg" : "qrc:/images/check_box_no.svg"; }
                            contentItem: Label { leftPadding: 36; text: idGrpChk.text; color: "#202020"; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; }

                            onCheckedChanged: { idListModel.get(index).isSelected = checked; }
                        }
                    }
                }

                Item { Layout.fillWidth: true; Layout.preferredHeight: 38;

                    RowLayout { anchors.centerIn: parent; spacing: 50;

                        Button { id: idOkBtn; Layout.preferredWidth: 100; Layout.preferredHeight: 38; text: qsTr("OK"); focusPolicy: Qt.NoFocus;
                            background: Rectangle { implicitWidth: 128; implicitHeight: 80; color: idOkBtn.down ? "#bdc0c6" : "#E0E0E0";  radius: 4; }
                            contentItem: Label { text: idOkBtn.text; color: idOkBtn.down ? "#0064b6" : "#202020"; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter; }

                            onClicked: { idPopup.close(); }
                        }

                        Button { id: idCancelBtn; Layout.preferredWidth: 100; Layout.preferredHeight: 38; text: qsTr("Cancel"); focusPolicy: Qt.NoFocus;
                            background: Rectangle { implicitWidth: 128; implicitHeight: 80; color: idCancelBtn.down ? "#bdc0c6" : "#E0E0E0";  radius: 4; }
                            contentItem: Label { text: idCancelBtn.text; color: idCancelBtn.down ? "#0064b6" : "#202020"; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter; }

                            onClicked: { idPopup.close(); }
                        }
                    }
                }
            }
        }
    }

    ButtonGroup { id: idRoleGroup; }

    ListModel { id: idListModel; }

    // ////////////////////////////////////////////////////////////////////////
    // logic
    // ////////////////////////////////////////////////////////////////////////
    Component.onCompleted:   { idPriv.init();   }
    Component.onDestruction: { idPriv.deinit(); }

    QtObject {
        id: idPriv;
        property var user_grp_perm_list_vm: null;

        // ==================================================================
        // initailize the page
        // ==================================================================
        function init( )
        {
            user_grp_perm_list_vm = IcUiQmlApi.appCtrl.objMgr.attachObj( "FcPerm::UserGrpPermListVm", true );
            idOkBtn.clicked.connect( done );
            var permInfoList = user_grp_perm_list_vm.editableUserGrpPermListInfo.permInfoList;
            for ( var i = 0; i < permInfoList.length; i++ )
            {
                idListModel.append({"isSelected": permInfoList[i]["isSelected"], "permName": permInfoList[i]["permName"]});
            }
        }

        // ====================================================================
        // de-init the page
        // ====================================================================
        function deinit( )
        {
            idOkBtn.clicked.disconnect( done );
            IcUiQmlApi.appCtrl.objMgr.detachObj( "FcPerm::UserGrpPermListVm", user_grp_perm_list_vm );
        }

        function done( )
        {
            var permList = new Array;
            for ( var i = 0; i < idListModel.count; i++ )
            {
                if ( idListModel.get(i).isSelected )
                {
                    var elem = new Object;
                    elem.permName = idListModel.get(i).permName;
                    permList.push(elem);
                }
            }

            var data = new Object;
            data.permList = permList;

            user_grp_perm_list_vm.permInfoList = data;
        }
    }
}
