import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import qxpack.indcom.ui_qml_base 1.0     // [HINT] this is the pre-registered module name.
import qxpack.indcom.ui_qml_control 1.0  // [HINT] ModalPopupDialog is in it

ModalPopupDialog {   // this is the wrapped Popup element in ui_qml_control
    id: idPopup;
    visible: false;
    reqEnterEventLoop: false;
    property alias contentWidth : idContent.implicitWidth;
    property alias contentHeight: idContent.implicitHeight;
    property var   inputObj: null;
    property string strData;

    onInputObjChanged: {
        if ( inputObj !== null )
        {
            strData = inputObj.text;
            if(strData===""){
                strData=idPriv.calendar_vm.currentDate;
            }

            idPriv.calendar_vm.yearListModel.currentValue = Number(strData.split("-")[0]);
            idPriv.calendar_vm.monthListModel.currentValue = Number(strData.split("-")[1]);
            idPriv.calendar_vm.dayListModel.currentValue = Number(strData.split("-")[2]);

            idYearList.positionViewAtIndex( Number(strData.split("-")[0] - Number(idYearList.currentItem.text)) + idYearList.currentIndex, PathView.Center );
            idMonthList.positionViewAtIndex( Number(strData.split("-")[1] - 1), PathView.Center );
            idDayList.positionViewAtIndex( Number(strData.split("-")[2] - 1), PathView.Center );

        }
    }

    // ////////////////////////////////////////////////////////////////////////
    // layout
    // ////////////////////////////////////////////////////////////////////////
//    backgroundColor: "red"; backgroundVisible: false;

   contentItem: Rectangle {
        // [HINT] Popup element need implicitWidth & implicitHeight to calc. the right position
        id: idContent; implicitWidth: idPopup.width; implicitHeight: idPopup.height; color: "#60606060";
        MouseArea { anchors.fill: parent;
            onClicked: {
                if ( inputObj !== null )
                { inputObj.text = idPriv.calendar_vm.dateStr; }
                idPopup.close();
            }
        }

        Rectangle { anchors.centerIn: parent; implicitWidth: 400; implicitHeight: 200; color: "#333e44"; border.color: "#6E6E6E";
            RowLayout { anchors.centerIn: parent;
                Item { id: idYear; Layout.preferredWidth: 100; Layout.preferredHeight: 180;
                    Rectangle { anchors.top: parent.top; anchors.topMargin: 60; width: parent.width; height: 1; color: "#FF8025"; }
                    Rectangle { anchors.bottom: parent.bottom; anchors.bottomMargin: 60; width: parent.width; height: 1; color: "#FF8025"; }
                    PathView { id: idYearList; clip: true; anchors.fill: parent; pathItemCount: 3; preferredHighlightBegin: 0.5; preferredHighlightEnd: 0.5; highlightRangeMode: PathView.StrictlyEnforceRange;
                        delegate: Label { width: 100; height: 60; text: modelData; font.pixelSize: PathView.pixelSize; color: PathView.color ? "#FAFAFA" : "#FF8025"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter; }
                        path: Path { startX: idYear.width / 2 ; startY: 0;
                            PathAttribute { name: "pixelSize"; value: 18 }
                            PathAttribute { name: "color"; value: 1 }
                            PathLine { relativeX: 0; y: idMonth.height / 2; }
                            PathAttribute { name: "pixelSize"; value: 36 }
                            PathAttribute { name: "color"; value: 0 }
                            PathLine { relativeX: 0; relativeY: idMonth.height / 2; }
                            PathAttribute { name: "pixelSize"; value: 18 }
                            PathAttribute { name: "color"; value: 1 }
                        }
                    }
                }

                Rectangle { Layout.preferredWidth: 25; Layout.preferredHeight: 1; Layout.alignment: Qt.AlignVCenter; color: "#FAFAFA" }
                Item { id: idMonth; Layout.preferredWidth: 100; Layout.preferredHeight: 180;
                    Rectangle { anchors.top: parent.top; anchors.topMargin: 60; width: parent.width; height: 1; color: "#FF8025"; }
                    Rectangle { anchors.bottom: parent.bottom; anchors.bottomMargin: 60; width: parent.width; height: 1; color: "#FF8025"; }
                    PathView { id: idMonthList; clip: true; anchors.fill: parent; pathItemCount: 3; preferredHighlightBegin: 0.5; preferredHighlightEnd: 0.5; highlightRangeMode: PathView.StrictlyEnforceRange;
                        delegate: Label { width: 100; height: 60; text: modelData; font.pixelSize: PathView.pixelSize; color: PathView.color ? "#FAFAFA" : "#FF8025"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter; }
                        path: Path { startX: idMonth.width / 2 ; startY: 0;
                            PathAttribute { name: "pixelSize"; value: 18 }
                            PathAttribute { name: "color"; value: 1 }
                            PathLine { relativeX: 0; y: idMonth.height / 2; }
                            PathAttribute { name: "pixelSize"; value: 36 }
                            PathAttribute { name: "color"; value: 0 }
                            PathLine { relativeX: 0; relativeY: idMonth.height / 2; }
                            PathAttribute { name: "pixelSize"; value: 18 }
                            PathAttribute { name: "color"; value: 1 }
                        }
                    }
                }

                Rectangle { Layout.preferredWidth: 25; Layout.preferredHeight: 1; Layout.alignment: Qt.AlignVCenter; color: "#FAFAFA" }
                Item { id: idDay; Layout.preferredWidth: 100; Layout.preferredHeight: 180;
                    Rectangle { anchors.top: parent.top; anchors.topMargin: 60; width: parent.width; height: 1; color: "#FF8025"; }
                    Rectangle { anchors.bottom: parent.bottom; anchors.bottomMargin: 60; width: parent.width; height: 1; color: "#FF8025"; }
                    PathView { id: idDayList; clip: true; anchors.fill: parent; pathItemCount: 3; preferredHighlightBegin: 0.5; preferredHighlightEnd: 0.5; highlightRangeMode: PathView.StrictlyEnforceRange;
                        delegate: Label { width: 100; height: 60; text: modelData; font.pixelSize: PathView.pixelSize; color: PathView.color ? "#FAFAFA" : "#FF8025"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter; }
                        path: Path { startX: idDay.width / 2 ; startY: 0;
                            PathAttribute { name: "pixelSize"; value: 18 }
                            PathAttribute { name: "color"; value: 1 }
                            PathLine { relativeX: 0; y: idMonth.height / 2; }
                            PathAttribute { name: "pixelSize"; value: 36 }
                            PathAttribute { name: "color"; value: 0 }
                            PathLine { relativeX: 0; relativeY: idMonth.height / 2; }
                            PathAttribute { name: "pixelSize"; value: 18 }
                            PathAttribute { name: "color"; value: 1 }
                        }
                    }
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
        property var calendar_vm: null;

        // ==================================================================
        // initailize the page
        // ==================================================================
        function init( )
        {
            calendar_vm       = IcUiQmlApi.appCtrl.objMgr.attachObj( "Perimeter::CalendarVm", true );
            idYearList.model  = calendar_vm.yearListModel;
            idMonthList.model = calendar_vm.monthListModel;
            idDayList.model   = calendar_vm.dayListModel;
            idYearList.movementEnded.connect( yearChanged );
            idMonthList.movementEnded.connect( monthChanged );
            idDayList.movementEnded.connect( dayChanged );
//            yearChanged( );monthChanged( );dayChanged( );   //好像没用也.
        }

        // ====================================================================
        // de-init the page
        // ====================================================================
        function deinit( )
        {
            idYearList.model  = null;
            idMonthList.model = null;
            idDayList.model   = null;
            idYearList.movementEnded.disconnect( yearChanged );
            idMonthList.movementEnded.disconnect( monthChanged );
            idDayList.movementEnded.disconnect( dayChanged );
            IcUiQmlApi.appCtrl.objMgr.detachObj( "Perimeter::CalendarVm", calendar_vm );
        }

        function yearChanged( )
        {
            idPriv.calendar_vm.yearListModel.currentValue = idYearList.currentItem.text;
        }

        function monthChanged( )
        {
            idPriv.calendar_vm.monthListModel.currentValue = idMonthList.currentItem.text;
        }

        function dayChanged( )
        {
            idPriv.calendar_vm.dayListModel.currentValue = idDayList.currentItem.text;
        }
    }
}

/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/
