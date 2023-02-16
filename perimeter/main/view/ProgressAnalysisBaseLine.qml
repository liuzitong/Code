import QtQuick 2.6
import QtQuick.Controls 1.0
import QtQuick.Controls 2.0
import QtQuick.Window 2.3
import QtQml 2.2
import QtQuick.Controls.Styles 1.4
import perimeter.main.view.Controls 1.0
import qxpack.indcom.ui_qml_base 1.0
import perimeter.main.view.Utils 1.0

Rectangle {
    id:root;
    anchors.fill: parent;
    color: "white";
    property var progressAnalysisListVm: null;
    property var progressAnalysisResult: null;
    property int fontPointSize: CommonSettings.fontPointSize;
    property int textHeight:CommonSettings.textHeight;

    property alias diagnosis: diagnosis.text;
//    width: 1440;height:768;
    signal refresh();
    onRefresh: {
//        console.log(progressAnalysisListVm.getData(0,"dateTime"));
//        console.log(progressAnalysisResult.slopeType);
    }

    Column
    {
        anchors.fill: parent;anchors.leftMargin: parent.width*0.04;anchors.rightMargin: parent.width*0.04;anchors.topMargin: parent.height*0.04;anchors.bottomMargin: parent.height*0.04;
        Row{
            width: parent.width;height: parent.height*0.04;spacing: parent.width*0.03;
            Item{height: parent.height;width: parent.parent.height*0.32;}
            Item{height: parent.height;width: parent.parent.height*0.32;CusText{anchors.fill: parent;text:lt+qsTr("Gray tone");font.pointSize: fontPointSize;}}
            Item{height: parent.height;width: parent.parent.height*0.32;CusText{anchors.fill: parent;text:lt+qsTr("Threshold")+"(DB)";font.pointSize: fontPointSize;}}
            Item{height: parent.height;width: parent.parent.height*0.32;CusText{anchors.fill: parent;text:lt+qsTr("Total deviation");font.pointSize: fontPointSize;}}
            Item{height: parent.height;width: parent.parent.height*0.32;CusText{anchors.fill: parent;text:lt+qsTr("Pattern deviation");font.pointSize: fontPointSize;}}
        }

        Row{
            width: parent.width;height: parent.height*0.32;spacing: parent.width*0.03;
            Column{
                width: height;height: parent.height;spacing: textHeight*0.5;
                Repeater{
                    property var centerDotCheck: [lt+qsTr("On"),lt+qsTr("Off")];
                    property var ght: [lt+qsTr("Out of limits"),lt+qsTr("Low sensitivity"),lt+qsTr("Border of limits"),lt+qsTr("Within normal limits")];
                    model: [
                        {name:lt+qsTr("Date"),param:Qt.formatDateTime(progressAnalysisListVm.getData(0,"dateTime"),"yyyy/MM/dd")},
                        {name:lt+qsTr("Program"),param:progressAnalysisListVm.getData(0,"program")},
                        {name:lt+qsTr("GHT"),param:progressAnalysisListVm==null?"":ght[progressAnalysisListVm.getData(0,"GHT")]},
                        {name:lt+qsTr("Center dot"),param:centerDotCheck[progressAnalysisListVm.getData(0,"centerDotCheck")?1:0]},
                        {name:lt+qsTr("MD"),param:progressAnalysisListVm.getData(0,"md").toFixed(2)+(progressAnalysisListVm.getData(0,"p_md")>0?(" (<"+progressAnalysisListVm.getData(0,"p_md")+"%)"):"")},
                        {name:lt+qsTr("PSD"),param:progressAnalysisListVm.getData(0,"psd").toFixed(2)+(progressAnalysisListVm.getData(0,"p_psd")>0?(" (<"+progressAnalysisListVm.getData(0,"p_psd")+"%)"):"")},
                        {name:lt+qsTr("Fixation loss rate"),param:progressAnalysisListVm.getData(0,"fixationLostCount")+"/"+progressAnalysisListVm.getData(0,"fixationLostTestCount")},
                        {name:lt+qsTr("False negative rate"),param:Math.round((progressAnalysisListVm.getData(0,"falseNegativeRate")*100))+"%"},
                        {name:lt+qsTr("False positive rate"),param:Math.round((progressAnalysisListVm.getData(0,"falsePositiveRate")*100))+"%"},
                    ]
                   CusText{text:modelData.name+":  "+modelData.param; horizontalAlignment: Text.AlignLeft;height:textHeight;width: parent.width;}
                }

            }
            Repeater{
                model:["/previewImage/baseLine_gray0.bmp","/previewImage/baseLine_dBDiagram0.bmp","/previewImage/baseLine_TotalPE0.bmp","/previewImage/baseLine_PatternPE0.bmp"];
                Image{
                   property string picSource: modelData;
                   height: sourceSize.height;width: sourceSize.width;
                   smooth: false;
                   cache: false;        //to refresh image
                   Component.onCompleted: {root.refresh.connect(function(){source="";source="file:///" + applicationDirPath + picSource;})}
                }
            }
            Item{
                height: parent.height;width:parent.height*0.5;
                Column{
                    height: parent.height*0.5; anchors.bottom: parent.bottom; width:parent.width;spacing:height*0.08;
                    Repeater{
                        anchors.fill: parent;
                        model: [{image:"qrc:/grays/PE1.bmp",pe:"<5%"},{image:"qrc:/grays/PE2.bmp",pe:"<2%"},{image:"qrc:/grays/PE3.bmp",pe:"<1%"},{image:"qrc:/grays/PE4.bmp",pe:"<0.5%"}]
                        Row{
                            height: parent.height*0.14;width: parent.width;spacing: height*0.5
                            Image {height: sourceSize.height*2; fillMode: Image.PreserveAspectFit; width:sourceSize.height*2;smooth: false;source: modelData.image}
                            CusText{height: parent.height;text:modelData.pe; horizontalAlignment: Text.AlignLeft;anchors.verticalCenter: parent.verticalCenter;}
                        }
                    }
                }
            }
        }
        Item{width: parent.width;height: parent.height*0.01;}
        Row{
            width: parent.width;height: parent.height*0.32;spacing: parent.width*0.03;
            Column{
                width: height;height: parent.height;spacing: textHeight*0.5;
                Repeater{
                    property var centerDotCheck: [lt+qsTr("On"),lt+qsTr("Off")];
                    property var ght: [lt+qsTr("Out of limits"),lt+qsTr("Low sensitivity"),lt+qsTr("Border of limits"),lt+qsTr("Within normal limits")];
                    model: [
                        {name:lt+qsTr("Date"),param:Qt.formatDateTime(progressAnalysisListVm.getData(1,"dateTime"),"yyyy/MM/dd")},
                        {name:lt+qsTr("Program"),param:progressAnalysisListVm.getData(1,"program")},
                        {name:lt+qsTr("GHT"),param:ght[progressAnalysisListVm.getData(1,"GHT")]},
                        {name:lt+qsTr("Center dot"),param:centerDotCheck[progressAnalysisListVm.getData(1,"centerDotCheck")?1:0]},
                        {name:lt+qsTr("MD"),param:progressAnalysisListVm.getData(1,"md").toFixed(2)+(progressAnalysisListVm.getData(1,"p_md")>0?(" (<"+progressAnalysisListVm.getData(0,"p_md")+"%)"):"")},
                        {name:lt+qsTr("PSD"),param:progressAnalysisListVm.getData(1,"psd").toFixed(2)+(progressAnalysisListVm.getData(1,"p_psd")>0?(" (<"+progressAnalysisListVm.getData(0,"p_psd")+"%)"):"")},
                        {name:lt+qsTr("Fixation loss rate"),param:progressAnalysisListVm.getData(1,"fixationLostCount")+"/"+progressAnalysisListVm.getData(1,"fixationLostTestCount")},
                        {name:lt+qsTr("False negative rate"),param:Math.round((progressAnalysisListVm.getData(1,"falseNegativeRate")*100))+"%"},
                        {name:lt+qsTr("False positive rate"),param:Math.round((progressAnalysisListVm.getData(1,"falsePositiveRate")*100))+"%"},
                    ]
                   CusText{text:modelData.name+":  "+modelData.param; horizontalAlignment: Text.AlignLeft;height:textHeight;width: parent.width;}
                }

            }
            Repeater{
                model:["/previewImage/baseLine_gray1.bmp","/previewImage/baseLine_dBDiagram1.bmp","/previewImage/baseLine_TotalPE1.bmp","/previewImage/baseLine_PatternPE1.bmp"];
                Image{
                   property string picSource: modelData;
                   height: sourceSize.height;width: sourceSize.width;
                   smooth: false;
                   cache: false;        //to refresh image
                   Component.onCompleted: {root.refresh.connect(function(){source="";source="file:///" + applicationDirPath + picSource;})}
                }
            }
        }
        Item{width: parent.width;height: parent.height*0.01;}
        Row{
            width: parent.width;height: parent.height*0.30;spacing: parent.width*0.03;
            Column{
                width: height;height: parent.parent.height*0.32;spacing: textHeight*0.5;
                Repeater{
                    property var slopeType: [lt+qsTr("Slope insignificant"),lt+qsTr("Slope significant")];
                    model: [
                        {name:lt+qsTr("ProgressSpeed"),param:progressAnalysisResult.progressSpeedBase.toFixed(2)+"±"+progressAnalysisResult.progressSpeedDeviation.toFixed(2)+" DB/YEAR"},
                        {name:lt+qsTr("Baseline average MD"),param:((progressAnalysisListVm.getData(0,"md")+progressAnalysisListVm.getData(1,"md"))/2).toFixed(2)},
                        {name:lt+qsTr("Slope status"),param:slopeType[progressAnalysisResult.slopeType]},
                    ]
                   CusText{text:modelData.name+":  "+modelData.param; horizontalAlignment: Text.AlignLeft;height:textHeight;width: parent.width;}
                }
            }
            ScrollView
            {
                width: parent.width*0.5;height: parent.height;
                Image{
                    property string picSource: "/previewImage/baseLine.bmp"
                    height: sourceSize.height;width: sourceSize.width;
                    smooth: false; cache: false;
                    Component.onCompleted: {root.refresh.connect(function(){source="";source="file:///" + applicationDirPath + picSource;})}
                }
            }

            Column{
                width:parent.width*0.2;height: parent.height;spacing: textHeight*0.3;
                CusText{text:lt+qsTr("Diagnosis")+":"; horizontalAlignment: Text.AlignLeft;width:parent.width;height:textHeight;}
                Rectangle{ width:parent.width;height: parent.height*0.7;radius: 5;border.color: "black";smooth: false;
                    TextInput
                    {
                        id:diagnosis;anchors.fill: parent;anchors.margins: 3;
                        width:parent.width*1.0;height: parent.height*0.70;
                        selectionColor: "blue";selectByMouse: true;
                        font.pointSize: fontPointSize;font.family: "Consolas";
                        wrapMode: Text.WrapAnywhere;renderType: Text.NativeRendering;
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
