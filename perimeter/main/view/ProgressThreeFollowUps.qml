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
    width: 1366;
    height: 660;
    signal refresh();
    onRefresh: {
        console.log(progressAnalysisListVm.getData(0,"dateTime"));
        console.log(progressAnalysisResult[2]);
    }
    Column
    {
        anchors.fill: parent;anchors.leftMargin: parent.width*0.04;anchors.rightMargin: parent.width*0.03;anchors.topMargin: parent.height*0.04;anchors.bottomMargin: parent.height*0.04;
        spacing: height*0.02
        Row{
            width: parent.width;height: parent.height*0.04;spacing: parent.width*0.82*0.02;
            Item{height: textHeight;width: parent.width*0.82*0.2;}
            Item{height: textHeight;width: parent.parent.height*0.96*0.32;CusText{anchors.fill: parent;text:lt+qsTr("Gray tone");}}
            Item{height: textHeight;width: parent.parent.height*0.96*0.32;CusText{anchors.fill: parent;text:lt+qsTr("Threshold")+"(DB)";}}
            Item{height: textHeight;width: parent.parent.height*0.96*0.32;CusText{anchors.fill: parent;text:lt+qsTr("Progress difference");}}
            Item{height: textHeight;width: parent.parent.height*0.96*0.32;CusText{anchors.fill: parent;text:lt+qsTr("Progress analysis");}}

        }
        Row{
            width: parent.width;height: parent.height*0.96;
            Column{
                width: parent.width*0.82;height: parent.height;spacing: parent.height*0.02;
                Repeater{
                    model: if(progressAnalysisListVm.selectedIndex>=4) return [{index_list:-2,index_progress:0},{index_list:-1,index_progress:1},{index_list:0,index_progress:2}];
                           else if(progressAnalysisListVm.selectedIndex===3) return [{index_list:-1,index_progress:0},{index_list:0,index_progress:1}];
                           else return [{index_list:0,index_progress:0}];
                    Row{
                        width: parent.width;height: parent.height*0.96/3;spacing: width*0.02;
                        Column{
                            width: parent.width*0.2;height: parent.height;spacing: textHeight*0.3;
                            Repeater{
                                property var centerDotCheck: [lt+qsTr("On"),lt+qsTr("Off")];
                                property var ght: [lt+qsTr("Out of limits"),lt+qsTr("Low sensitivity"),lt+qsTr("Border of limits"),lt+qsTr("Within normal limits")];
                                property var progress: [lt+qsTr("no progress"),lt+qsTr("possible progress"),lt+qsTr("very possible progress")];
                                property int index: progressAnalysisListVm.selectedIndex;
                                model: [
                                    {name:lt+qsTr("Date"),param:Qt.formatDateTime(progressAnalysisListVm.getData(index+modelData.index_list,"dateTime"),"yyyy/MM/dd")},
                                    {name:lt+qsTr("Program"),param:progressAnalysisListVm.getData(index+modelData.index_list,"program")},
                                    {name:lt+qsTr("GHT"),param:ght[progressAnalysisListVm.getData(index+modelData.index_list,"GHT")]},
                                    {name:lt+qsTr("Center dot"),param:centerDotCheck[progressAnalysisListVm.getData(index+modelData.index_list,"centerDotCheck")?1:0]},
                                    {name:lt+qsTr("MD"),param:progressAnalysisListVm.getData(index+modelData.index_list,"md").toFixed(2)+(progressAnalysisListVm.getData(index+modelData.index_list,"p_md")>0?(" (<"+progressAnalysisListVm.getData(index+modelData.index_list,"p_md")+"%)"):"")},
                                    {name:lt+qsTr("PSD"),param:progressAnalysisListVm.getData(index+modelData.index_list,"psd").toFixed(2)+(progressAnalysisListVm.getData(index+modelData.index_list,"p_psd")>0?(" (<"+progressAnalysisListVm.getData(index+modelData.index_list,"p_psd")+"%)"):"")},
                                    {name:lt+qsTr("Fixation loss rate"),param:progressAnalysisListVm.getData(index+modelData.index_list,"fixationLostCount")+"/"+progressAnalysisListVm.getData(index+modelData.index_list,"fixationLostTestCount")},
                                    {name:lt+qsTr("False negative rate"),param:Math.round((progressAnalysisListVm.getData(index+modelData.index_list,"falseNegativeRate")*100))+"%"},
                                    {name:lt+qsTr("False positive rate"),param:Math.round((progressAnalysisListVm.getData(index+modelData.index_list,"falsePositiveRate")*100))+"%"},
                                    {name:lt+qsTr("Progress"),param:progress[progressAnalysisResult[modelData.index_progress]]},
                                ]
                                CusText{text:modelData.name+":  "+modelData.param; horizontalAlignment: Text.AlignLeft;height:textHeight;width: parent.width;}
                            }
                        }
                        Repeater{
                            model:["/previewImage/threeFollowUps_grey"+modelData.index_progress+".bmp","/previewImage/threeFollowUps_PatternPE"+modelData.index_progress+".bmp","/previewImage/threeFollowUps_progressVal"+modelData.index_progress+".bmp","/previewImage/threeFollowUps_progressPic"+modelData.index_progress+".bmp"];
                            Image{
                               property string picSource: modelData;
                               height: sourceSize.height;width: sourceSize.width;
                               smooth: false;
                               cache: false;        //to refresh image
                               Component.onCompleted: {root.refresh.connect(function(){source="";source="file:///" + applicationDirPath + picSource;})}
                            }
                        }
                    }
                }
            }
            Column
            {
                width: parent.width*0.18;
                height: parent.height;spacing:textHeight;
                CusText{text:lt+qsTr("MD")+":"+((progressAnalysisListVm.getData(0,"md")+progressAnalysisListVm.getData(1,"md"))/2).toFixed(2); horizontalAlignment: Text.AlignLeft;height:textHeight;width: parent.width;}
                CusText{text:lt+qsTr("BaseLine dates")+":"+Qt.formatDateTime(progressAnalysisListVm.getData(0,"dateTime"),"yyyy/MM/dd")+"  "+Qt.formatDateTime(progressAnalysisListVm==null?"":progressAnalysisListVm.getData(1,"dateTime"),"yyyy/MM/dd");horizontalAlignment: Text.AlignLeft;height:textHeight;width: parent.width;}
//                CusText{text:Qt.formatDateTime(progressAnalysisListVm.getData(0,"dateTime"),"yyyy/MM/dd")+"  "+Qt.formatDateTime(progressAnalysisListVm==null?"":progressAnalysisListVm.getData(1,"dateTime"),"yyyy/MM/dd"); horizontalAlignment: Text.AlignLeft;height:textHeight;width: parent.width;}
                Row{
                    width: parent.width;height: parent.height*0.4
                    Column{
                        height: parent.height;  width:parent.width*0.3;spacing:height*0.08;
                        Repeater{
                            anchors.fill: parent;
                            model: [{image:"qrc:/grays/PE1.bmp",pe:"<5%"},{image:"qrc:/grays/PE2.bmp",pe:"<2%"},{image:"qrc:/grays/PE3.bmp",pe:"<1%"},{image:"qrc:/grays/PE4.bmp",pe:"<0.5%"}]
                            Row{
                                height:textHeight;width: parent.width;spacing: height*0.5
                                Image {height: sourceSize.height*2; fillMode: Image.PreserveAspectFit; width:sourceSize.height*2;smooth: false;source: modelData.image}
                                CusText{height: textHeight;text:modelData.pe; horizontalAlignment: Text.AlignLeft;anchors.verticalCenter: parent.verticalCenter;}
                            }
                        }
                    }
                    Column{
                        height: parent.height;  width:parent.width*0.7;spacing:textHeight*0.5;
                        Repeater{
                            anchors.fill: parent;
                            model: [{image:"qrc:/grays/GPA1.bmp",progress:"<5% ("+qsTr("Deterioarted")+")"},{image:"qrc:/grays/GPA2.bmp",progress:"<5% (2"+qsTr("Consecutiveness")+")"},{image:"qrc:/grays/GPA3.bmp",progress:"<5% (3"+qsTr("Consecutiveness")+")"},{image:"qrc:/grays/GPA4.bmp",progress:lt+qsTr("Out of range")}]
                            Row{
                                height:textHeight;width: parent.width;spacing: height*0.5
                                Image {height: sourceSize.height*2; fillMode: Image.PreserveAspectFit; width:sourceSize.height*2;smooth: false;source: modelData.image}
                                CusText{height: textHeight;text:modelData.progress; horizontalAlignment: Text.AlignLeft;anchors.verticalCenter: parent.verticalCenter}
                            }
                        }
                    }
                }

                Column{
                    width:parent.width;height: parent.height*0.3;spacing: textHeight*0.3;
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
}
