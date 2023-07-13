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
        id: idContent; implicitWidth: 586; implicitHeight: 710; color: "#60606060";

        MouseArea { anchors.fill: parent;

            onClicked: { idPopup.close(); }
        }

        Rectangle { anchors.centerIn: parent; implicitWidth: 480; implicitHeight: 700; color: "#FAFAFA"; border.color: "#6E6E6E"
            // ColumnLayout arrange text area and control area.
            ColumnLayout { anchors.fill: parent; anchors.margins: 20; spacing: 20;

                Label { Layout.alignment: Qt.AlignHCenter | Qt.AlignTop; text: qsTr("user permission"); font.pixelSize: 22;  color: "#202020"; }

                Rectangle { Layout.preferredWidth: 400; Layout.preferredHeight: 1; Layout.alignment: Qt.AlignHCenter; color: "#6E6E6E"; }

                ListView { id: idPermList; Layout.preferredWidth: 400; Layout.fillHeight: true; Layout.alignment: Qt.AlignHCenter; clip: true; spacing: 20;
                    ScrollBar.vertical: ScrollBar{ orientation: Qt.Vertical; policy: idPermList.contentHeight > idPermList.height ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff;
                        contentItem: Rectangle { implicitWidth: 6; implicitHeight: 54; color: "#6E6E6E"; radius: 3; }
                    }
                    delegate: Item { width: 400; height: 36

                        CheckBox { id: idPermChk; anchors.left: parent.left; anchors.leftMargin: 70; implicitHeight: 36; enabled: false; checked: true; text: modelData;
                            indicator: Image { width: 36; height: 36; source: idPermChk.checked ? "qrc:/images/check_box_yes.svg" : "qrc:/images/check_box_no.svg"; opacity: idPermChk.enabled ? 1 : 0.4; }
                            contentItem: Label { leftPadding: 36; text: idPermChk.text; color: "#202020"; font.pixelSize: 16;  verticalAlignment: Text.AlignVCenter; }
                        }
                    }
                }

                Button { id: idOkBtn; Layout.preferredWidth: 120; Layout.preferredHeight: 50; Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter; text: qsTr("OK"); focusPolicy: Qt.NoFocus;
                    background: Rectangle { implicitWidth: 128; implicitHeight: 80; color: enabled ? (idOkBtn.down ? "#bdc0c6" : "#E0E0E0") : "#60E0E0E0"; }
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
        property var user_mgr_vm: null;

        // ==================================================================
        // initailize the page
        // ==================================================================
        function init( )
        {
            user_mgr_vm       = IcUiQmlApi.appCtrl.objMgr.attachObj( "FcPerm::UserMgrVm", true );
            idPermList.model = user_mgr_vm.specUserPermListInfo.actualPermList;
        }

        // ====================================================================
        // de-init the page
        // ====================================================================
        function deinit( )
        {
            idPermList.model = null;
            IcUiQmlApi.appCtrl.objMgr.detachObj( "FcPerm::UserMgrVm", user_mgr_vm );
        }
    }
}
