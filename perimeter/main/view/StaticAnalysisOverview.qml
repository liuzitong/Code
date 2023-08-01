import QtQuick 2.6
import QtQuick.Controls 2.0
import QtQuick.Window 2.3
import QtQml 2.2
import QtQuick.Controls.Styles 1.4
import perimeter.main.view.Controls 1.0
import perimeter.main.view.Utils 1.0
import qxpack.indcom.ui_qml_base 1.0

Item {
    property var currentPatient: null;
    property var currentProgram: null;
    property var currentCheckResult: null;
    property var analysisResult: null;
    property var analysisVm: null;
    property int report;
    signal refresh();

    onRefresh: {
        console.log(analysisVm.resultList);
//        console.log(analysisVm.resultList[0]);
//        console.log(analysisVm.resultList[0].checkDate);
    }
    ListView{
        id:listview;
        anchors.fill: parent
        property var listModel: null;
        model: analysisVm.resultList
//        model:ListModel{ListElement{checkDate:"ahah"}ListElement{checkDate:"ahah"}ListElement{checkDate:"ahah"}}
        delegate: overViewDelegate
        clip: true;
        snapMode: ListView.SnapOneItem
        spacing: -1;
        Component
        {
            id:overViewDelegate;
            Rectangle
            {
                id: rectangle
                height: (listview.height+2)/3;
                width: listview.width;
                anchors.horizontalCenter: parent.horizontalCenter;
                border.color: "gray";

                Flow{
                    height: parent.height;
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: parent.height*0.1;
                    Column{
                        height: parent.height;
                        width: parent.height;
                        property var centerDotCheck: [lt+qsTr("On"),lt+qsTr("Off")];
                        property var strategy: [lt+qsTr("Full threshold"),lt+qsTr("Fast threshold"),lt+qsTr("Smart interactive"),lt+qsTr("Fast interactive")]
                        property var ght: [lt+qsTr("Out of limits"),lt+qsTr("Low sensitivity"),lt+qsTr("Border of limits"),lt+qsTr("Within normal limits")];
                        property var proxyModel:
                        [
                            qsTr("program:")+model.program,
                            qsTr("check date:")+(Qt.formatDateTime(model.checkDate,"yyyy/MM/dd")),
                            qsTr("strategy:")+strategy[model.strategy],
                            qsTr("GHT:")+ght[model.GHT],
                            qsTr("eye:")+(model.OS_OD?"OS":"OD"),
                            qsTr("center dot check:")+(model.centerDotCheck?centerDotCheck[0]:centerDotCheck[1]),
                            qsTr("md:")+(model.md.toFixed(2)+(model.p_md<10?"(<"+model.p_md.toFixed(0)+"%)":"")),
                            qsTr("psd:")+(model.psd.toFixed(2)+(model.p_psd<10?"(<"+model.p_psd.toFixed(0)+"%)":"")),
                        ]
                        Repeater{
                            anchors.fill: parent;
                            model:parent.proxyModel;
                            CusText{width:parent.width;height:parent.height/8;text:modelData ; verticalAlignment: Text.AlignVCenter;horizontalAlignment: Text.AlignLeft}
                        }
                    }
                    Rectangle{
                        width: parent.height;
                        height: parent.height;
                        color: "red";
                    }
                    Rectangle{
                        width: parent.height;
                        height: parent.height;
                        color: "blue";
                    }
                    Rectangle{
                        width: parent.height;
                        height: parent.height;
                        color: "yellow";
                    }
                    Rectangle{
                        width: parent.height;
                        height: parent.height;
                        color: "cyan";
                    }
                }
            }
        }
    }
}

/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/
