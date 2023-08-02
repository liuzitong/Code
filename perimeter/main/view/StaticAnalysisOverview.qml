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
    property alias diagnosis: diagnosis.text;
    signal refresh();

//    onRefresh: {
//        console.log(analysisVm.resultList);
//        console.log(analysisVm.resultList[0]);
//        console.log(analysisVm.resultList[0].checkDate);
//    }

    ListView{id:listview;anchors.fill: parent;clip: true;snapMode: ListView.SnapOneItem;spacing: -1;
        delegate: overViewDelegate
        model: analysisVm.resultList
        property var listModel: null;
        Component{id:overViewDelegate;
            Rectangle{id: rectangle;height: (listview.height+2)/3;width: listview.width;anchors.horizontalCenter: parent.horizontalCenter;border.color: "gray";
                Flow{height: parent.height*0.85;spacing: parent.height*0.1;anchors.verticalCenter: parent.verticalCenter;
                    property var proxyModel:[model.grayPicPath,model.threshHoldPicPath,model.totalDeviationPicPath,model.patternDeviationPicPath]
                    Item {height: parent.height;width: parent.width*0.05;}
                    Column{
                        height: parent.height;width: parent.height;
                        property var centerDotCheck: [lt+qsTr("On"),lt+qsTr("Off")];
                        property var strategy: [lt+qsTr("Full threshold"),lt+qsTr("Fast threshold"),lt+qsTr("Smart interactive"),lt+qsTr("Fast interactive")]
                        property var ght: [lt+qsTr("Out of limits"),lt+qsTr("Low sensitivity"),lt+qsTr("Border of limits"),lt+qsTr("Within normal limits")];
                        property var proxyModel:
                        [
                            qsTr("program")+":"+model.program,
                            qsTr("check date")+":"+(Qt.formatDateTime(model.checkDate,"yyyy/MM/dd")),
                            qsTr("strategy")+":"+strategy[model.strategy],
                            qsTr("GHT")+":"+ght[model.GHT],
                            qsTr("eye kind")+":"+(model.OS_OD===0?"OS":"OD"),
                            qsTr("center dot check")+":"+(model.centerDotCheck?centerDotCheck[0]:centerDotCheck[1]),
                            qsTr("MD")+":"+(model.md.toFixed(2)+(model.p_md.toFixed(2)<10?"(<"+model.p_md.toFixed(2)+"%)":"")),
                            qsTr("PSD")+":"+(model.psd.toFixed(2)+(model.p_psd.toFixed(2)<10?"(<"+model.p_psd.toFixed(2)+"%)":"")),
                        ]
                        Repeater{
                            model:parent.proxyModel;
                            CusText{width:parent.width;height:parent.height/8;text:modelData ; verticalAlignment: Text.AlignVCenter;horizontalAlignment: Text.AlignLeft}
                        }
                    }


                    Column{width: parent.height*0.9;height: parent.height;
                        CusText{text:lt+qsTr("Threshold")+"(dB)";width: parent.width;height: parent.height*0.1;}
                        Image{
                           property string picSource: model.dBDiagramPicPath;
                           height: sourceSize.height;width: sourceSize.width;smooth: false;cache: false;
                           source:"file:///" + applicationDirPath + picSource;
                        }
                    }

                    Column{width: parent.height*0.9;height: parent.height;
                        CusText{text:lt+qsTr("Gray tone");width: parent.width;height: parent.height*0.1;}
                        Image{
                           property string picSource: model.grayPicPath;
                           height: sourceSize.height;width: sourceSize.width;smooth: false;cache: false;
                           source:"file:///" + applicationDirPath + picSource;
                        }
                    }

                    Column{width: parent.height*0.9;height: parent.height;
                        CusText{text:lt+qsTr("Total deviation");width: parent.width;height: parent.height*0.1;}
                        Image{
                           property string picSource: model.totalDeviationPicPath;
                           height: sourceSize.height;width: sourceSize.width;smooth: false;cache: false;
                           source:"file:///" + applicationDirPath + picSource;
                        }
                    }

                    Column{width: parent.height*0.9;height: parent.height;
                        CusText{text:lt+qsTr("Pattern deviation");width: parent.width;height: parent.height*0.1;}
                        Image{
                           property string picSource: model.patternDeviationPicPath;
                           height: sourceSize.height;width: sourceSize.width;smooth: false;cache: false;
                           source:"file:///" + applicationDirPath + picSource;
                        }
                    }

                }
            }
        }
    }
    Column{
        width:parent.width*0.15;height: parent.height*0.3;spacing: CommonSettings.textHeight*0.3;
        anchors.bottom: parent.bottom;
        anchors.right: parent.right;
        anchors.rightMargin: parent.width*0.05
        CusText{text:lt+qsTr("Diagnosis")+":"; horizontalAlignment: Text.AlignLeft;width:parent.width;height:CommonSettings.textHeight;}
        Rectangle{ width:parent.width;height: parent.height*0.7;radius: 5;border.color: "black";smooth: false;
            TextInput
            {
                id:diagnosis;anchors.fill: parent;anchors.margins: 3;
                width:parent.width*1.0;height: parent.height*0.70;
                selectionColor: "blue";selectByMouse: true;
                font.pointSize: CommonSettings.fontPointSize;font.family: "Consolas";
                wrapMode: Text.WrapAnywhere;renderType: Text.NativeRendering;
            }
        }
    }
}

/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/
