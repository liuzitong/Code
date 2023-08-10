import QtQuick 2.5
import QtQuick.Controls 2.0
import QtQml   2.2
import QtTest  1.0
import qxpack.indcom.ui_qml_base 1.0
import qxpack.indcom.ui_qml_control 1.0

TestCase {
    id: idRoot;

    // ========================================================================
    // put it here, it will force IcUiQmlApi create..
    // ========================================================================
    function test_apiVersionCall()
    {
        verify( IcUiQmlApi.apiVersion !== "", "could not read api version.");
    }

    Component {
        id: idWinCmpt;

        ApplicationWindow {
            id: idWin; width: 400; height: 400;
            Component {
                id: idPopupDialogCmpt;
                ModalPopupDialog {
                    id: idPopup; backgroundColor: "red"; backgroundVisible: false;
                    popupX: 10; popupY: 10;
                    contentItem: Rectangle {
                        implicitWidth: 100; implicitHeight: 100; color:"green";
                        onVisibleChanged: { if( visible ) { idTmr.start(); }}
                        Timer {
                            id: idTmr; interval: 3000;
                            onTriggered: { idPopup.close(); }
                        }
                    }
                }
            }
            Component.onCompleted: { IcUiQmlApi.postMetaCall( idWin, "showPopup"); }

            function showPopup( ) {
                var p = idPopupDialogCmpt.createObject( idWin );
                p.destroyOnClose = true;
                p.open();
                p = null;
                IcUiQmlApi.leaveEventLoop();
            }
        }
    }

    function test_showWin()
    {
        var obj = idWinCmpt.createObject( idRoot );
        obj.show();
        IcUiQmlApi.enterEventLoop();
        obj.destroy();
    }
}
