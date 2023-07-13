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
        id: idContent; implicitWidth: 586; implicitHeight: 510; color: "#60606060";

        MouseArea { anchors.fill: parent;
            onClicked: { idPopup.close(); }
        }

        Rectangle { anchors.centerIn: parent; implicitWidth: 480; implicitHeight: 700; color: "#FAFAFA"; border.color: "#6E6E6E"
            // ColumnLayout arrange text area and control area.
            ColumnLayout { anchors.fill: parent; anchors.margins: 20; spacing: 20;

                Label { Layout.alignment: Qt.AlignHCenter | Qt.AlignTop; text: qsTr("user group permission"); font.pixelSize: 22;  color: "#202020"; }

                Rectangle { Layout.preferredWidth: 400; Layout.preferredHeight: 1; Layout.alignment: Qt.AlignHCenter; color: "#6E6E6E"; }

                ListView { id: idGrpPermList; Layout.preferredWidth: 400; Layout.fillHeight: true; Layout.alignment: Qt.AlignHCenter; clip: true; spacing: 20;
                    ScrollBar.vertical: ScrollBar{ orientation: Qt.Vertical; policy: idGrpPermList.contentHeight > idGrpPermList.height ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff;
                        contentItem: Rectangle { implicitWidth: 6; implicitHeight: 54; color: "#6E6E6E"; radius: 3; }
                    }
                    delegate: Item { width: 400; height: 36

                        CheckBox { id: idGrpPermChk; anchors.left: parent.left; anchors.leftMargin: 100; implicitHeight: 30; enabled: false; checked: true; text: modelData;
                            indicator: Image { width: 36; height: 36; source: idGrpPermChk.checked ? "qrc:/images/check_box_yes.svg" : "qrc:/images/check_box_no.svg"; opacity: idGrpPermChk.enabled ? 1 : 0.4; }
                            contentItem: Label { leftPadding: 36; text: idGrpPermChk.text; color: "#202020"; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; }
                        }
                    }
                }

                Button { id: idOkBtn; Layout.preferredWidth: 120; Layout.preferredHeight: 50; Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter; text: qsTr("OK");
                    background: Rectangle { implicitWidth: 128; implicitHeight: 80; color: enabled ? (idOkBtn.down ? "#353637" : "#E0E0E0") : "#60E0E0E0"; radius: 4; }
                    contentItem: Label { text: idOkBtn.text; color: idOkBtn.down ? "#0064b6" : "#202020"; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter; }

                    onClicked: { idPopup.close(); }
                }
            }
        }
    }

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
            idGrpPermList.model = user_grp_mgr_vm.specUserGrpPermListInfo.permList;
        }

        // ====================================================================
        // de-init the page
        // ====================================================================
        function deinit( )
        {
            idGrpPermList.model = null;
            IcUiQmlApi.appCtrl.objMgr.detachObj( "FcPerm::UserGrpMgrVm", user_grp_mgr_vm );
        }
    }
}
