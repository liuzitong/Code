﻿import QtQuick 2.6
import QtQuick.Controls 2.0
import QtQuick.Controls 1.0
import QtQuick.Window 2.3
import QtQml 2.2
import QtQuick.Controls.Styles 1.4
import perimeter.main.view.Controls 1.0
import qxpack.indcom.ui_qml_base 1.0
import perimeter.main.view.Utils 1.0

Item
{
    id:root;
    signal refresh();
    signal realTimePicRefresh(var count);
    property var currentPatient: null;
    property var currentProgram: null;
    property var currentCheckResult: null;
    property var analysisResult: null;
    property var analysisVm: null;
    property int fontPointSize: CommonSettings.fontPointSize;
    property int textHeight:CommonSettings.textHeight;
    property var selectedDotIndex: analysisVm.selectedDotIndex;
    property var report:null;                           //0:常规,1:三合一,2:总览,3:筛选
    property string rx:"";
    width: 1366;
    height: 660;
    onSelectedDotIndexChanged:{if(currentCheckResult==null) return;var count=currentCheckResult.drawRealTimeEyePosPic(selectedDotIndex);realTimePicRefresh(count);}
//    onRealTimePicRefresh: {console.log(count);}


    onRefresh:
    {
        if(currentProgram.type===0)
        {
            switch(report)
            {
            case 0:content.source="StaticAnalysisReportSingle.qml";break;
            case 1:content.source="StaticAnalysisReportThreeInOne.qml";break;
            case 2:content.source="StaticAnalysisReportOverView.qml";break;
            case 3:content.source="StaticAnalysisReportTwoInOne.qml";break;
            case 4:content.source="StaticAnalysisReportDB.qml";break;
            }
        }
        else
        {
             content.source="StaticAnalysisReportScreening.qml"
        }


        content.item.analysisResult=analysisResult;
        content.item.analysisVm=analysisVm;
        content.item.refresh();

        if(currentCheckResult.OS_OD===0)  //左
            rx=currentPatient.rx.rx1_l.toFixed(2)+" DS:"+currentPatient.rx.rx2_l.toFixed(2)+" DC:"+currentPatient.rx.rx3_l.toFixed(2);
        else
            rx=currentPatient.rx.rx1_r.toFixed(2)+" DS:"+currentPatient.rx.rx2_r.toFixed(2)+" DC:"+currentPatient.rx.rx3_r.toFixed(2);
    }

    Row{
        anchors.fill: parent;
        Rectangle{width:parent.width*0.75;height: parent.height;color:"white";
            Row{anchors.fill: parent;anchors.leftMargin: parent.width*0.04;anchors.topMargin: parent.height*0.04;anchors.bottomMargin: parent.height*0.04;spacing: width*0.05;
                Column{ id: column;width: parent.width*0.25;height: parent.height;spacing:parent.height*0.02
                    CusText{text:lt+currentProgram.name; font.bold: true; horizontalAlignment: Text.AlignLeft;height:parent.height*0.10;font.pointSize: fontPointSize*2;}
                    Column{id:ttt;width:parent.width;height: parent.height*0.25;spacing: textHeight*0.5;
                        Repeater{
                            property var params: currentCheckResult.params.commonParams;
                            property int timeSpan:currentCheckResult.resultData.testTimespan;
                            property var fixationMonitor: [lt+qsTr("No remind"),lt+qsTr("Only remind"),lt+qsTr("Remind and pause")];
                            property var fixationTarget: [lt+qsTr("Center dot"),lt+qsTr("Small diamond"),lt+qsTr("Big diamond"),lt+qsTr("Bottom dot")]
                            property var centerDotCheck: [lt+qsTr("On"),lt+qsTr("Off")];

                            model: [
                                {name:lt+qsTr("Eye move remind mode"),param:fixationMonitor[params.fixationMonitor]},
                                {name:lt+qsTr("Fixation target"),param:fixationTarget[params.fixationTarget]},
                                {name:lt+qsTr("Fixation loss rate"),param:currentCheckResult.resultData.fixationLostCount+'/'+currentCheckResult.resultData.fixationLostTestCount},
                                {name:lt+qsTr("False positive rate"),param:Math.round(currentCheckResult.resultData.falsePositiveCount/currentCheckResult.resultData.falsePositiveTestCount*100)+"%"},
                                {name:lt+qsTr("False negative rate"),param:Math.round(currentCheckResult.resultData.falseNegativeCount/currentCheckResult.resultData.falseNegativeTestCount*100)+"%"},
                                {name:lt+qsTr("Check time"),param:Math.floor(timeSpan/60)+":"+timeSpan%60},
                                {name:lt+qsTr("Center dot check"),param:params.centerDotCheck?centerDotCheck[0]:centerDotCheck[1]}]
                           CusText{text:modelData.name+":  "+modelData.param; horizontalAlignment: Text.AlignLeft;height:textHeight;width: parent.width;}
                        }
    //                        CusText{text:"固视监测";  horizontalAlignment: Text.AlignLeft;height:textHeight;width: parent.width;}
                    }
                    Column{width:parent.width;height: parent.height*0.12;spacing: textHeight*0.5;
                        Repeater{
                            property var params: currentCheckResult.params.commonParams;
                            property var cursorSize: ["I","II","III","IV","V"];
                            property var cursorColor: [lt+qsTr("White"),lt+qsTr("Red"),lt+qsTr("Blue")];
                            property var backGroundColor: ["31.5 ASB","315 ASB"];
                            property var strategy: [lt+qsTr("Full threshold"),lt+qsTr("Fast threshold"),lt+qsTr("Smart interactive"),lt+qsTr("Fast interactive"),lt+qsTr("One stage"),lt+qsTr("Two stages"),lt+qsTr("Quantify defects"),lt+qsTr("Single stimulus")]
                            model: [
                                {name:lt+qsTr("Stimulus cursor"),param:cursorSize[params.cursorSize]+","+cursorColor[params.cursorColor]},
                                {name:lt+qsTr("Background light"),param:backGroundColor[params.backGroundColor]},
                                {name:lt+qsTr("Strategy"),param:strategy[params.strategy]}]
                            CusText{text:modelData.name+":  "+modelData.param; horizontalAlignment: Text.AlignLeft;height:textHeight;width: parent.width;}
                        }
                    }
                    Column{width:parent.width;height: parent.height*0.12;spacing: textHeight*0.5;
                        Repeater{
                            model: [
                                {name:lt+qsTr("Pupil diameter"),param:currentCheckResult.resultData.pupilDiameter.toFixed(2)+"mm"},
                                {name:lt+qsTr("Visual acuity"),param:!currentCheckResult.OS_OD?currentPatient.rx.visual_l.toFixed(2):currentPatient.rx.visual_r.toFixed(2)},
                                {name:lt+qsTr("Diopter"),param:"Rx:"+rx}]
                           CusText{text:modelData.name+":  "+modelData.param; horizontalAlignment: Text.AlignLeft;height:textHeight;width: parent.width;}
                        }
                    }

                    Column{width:parent.width;height: parent.height*0.20;spacing: textHeight*0.3;
                        CusText{text:lt+qsTr("Diagnosis")+":"; horizontalAlignment: Text.AlignLeft;width:parent.width;height:textHeight;}
                        Rectangle{ id: rectangle;width:parent.width;height: parent.height-1.3*textHeight;radius: 5;border.color: "black";smooth: false;
                            TextInput
                            {
                                id:diagnosis;anchors.fill: parent;anchors.margins: 3;
                                objectName: "UserInputItem.TextField";
                                property string vkbdLangCountry : root.lang;
                                width:parent.width*1.0;height: parent.height*0.70;
                                text:currentCheckResult==null?"":currentCheckResult.diagnosis;
                                selectionColor: "blue";selectByMouse: true;
                                font.pointSize: fontPointSize;font.family: "Consolas";
                                wrapMode: Text.WrapAnywhere;renderType: Text.NativeRendering;
                            }

                            CusButton{ id: cusButton;height: parent.height*0.28;width: height*2;
                                text:lt+qsTr("Save");anchors.right: parent.right;anchors.bottom: parent.bottom;
                                anchors.rightMargin: 5;anchors.bottomMargin: 5;
                                onClicked:
                                {
                                    currentCheckResult.diagnosis=diagnosis.text;
                                    currentCheckResult.update();
                                }
                            }
                        }
                    }


                    Item{width:parent.width;height: parent.height*0.08;
                        FixationDeviation{ dots:if(currentCheckResult!==null) currentCheckResult.resultData.fixationDeviation; }
                    }
                }

                Loader
                {
                    id:content;
                    width: parent.width*0.70;height:parent.height
                }
            }
        }
//        Item
//        {
//            id:testRec;
//            width:parent.width*0.25;height: parent.height;
//            ScrollView{
//                anchors.fill: parent;
//                horizontalScrollBarPolicy:Qt.ScrollBarAlwaysOff;
//                verticalScrollBarPolicy:Qt.ScrollBarAlwaysOff;
//                Column{
//                    width:testRec.width;height: testRec.height*2;
//                    Rectangle{width:parent.width;height: parent.height*0.5;color:"red";}
//                    Rectangle{width:parent.width;height: parent.height*0.5;color:"blue";}
//                }
//            }
//        }
         Rectangle{width:parent.width*0.25;height: parent.height;color:"white";
             GridView{
                 property ListModel listModel:ListModel{}
                 property string fileDir;
                 boundsBehavior: Flickable.StopAtBounds
                 clip: true
                 id:realTimeEyePosListView
                 cellWidth: parent.width/2;cellHeight:cellWidth;
                 anchors.fill: parent;
                 delegate: realTimeEyePos
                 model:listModel;

                 Component.onCompleted:
                 {
                     //靠analysisVm的选择点变化,来触发root.realTimePicRefresh,从而刷新
                     root.refresh.connect(function(){visible=false;parent.color="white"});
                     root.realTimePicRefresh.connect(
                     function(val){
                         console.log(val);
                         console.log(val[0]);
                         console.log(val[1]);

                         var count=val[0];
                         fileDir=val[1];
                         visible=true;parent.color="grey"
                         listModel.clear();
                         for(var i=0;i<count;i++)
                         {
                            listModel.append({fileDir:fileDir,index:i});
                         }
                     })
                 }

                 Component{
                     id:realTimeEyePos
                     Item{width: realTimeEyePosListView.width/2;height: width;
                         Image{
                            property string picSource: fileDir+index+".BMP";
                            anchors.fill: parent;
                            fillMode: Image.PreserveAspectCrop;smooth: false;cache: false;        //to refresh image
                            source: "file:///" + applicationDirPath + picSource;
                         }
                         CusText{width:CommonSettings.fontPointSize*2.5;height: CommonSettings.fontPointSize*1.6;anchors.top: parent.top; anchors.topMargin: parent.height*0.05; anchors.left: parent.left; anchors.leftMargin:parent.width*0.05;text:index+1; horizontalAlignment: Text.AlignLeft; verticalAlignment: Text.AlignTop; color: "yellow";}
                         CusText{width:CommonSettings.fontPointSize*2.5;height: CommonSettings.fontPointSize*1.6;anchors.bottom: parent.bottom; anchors.bottomMargin: parent.height*0.05; anchors.right: parent.right; anchors.rightMargin:parent.width*0.05;text:currentCheckResult.resultData.realTimeDB[selectedDotIndex][index]+"DB"; verticalAlignment: Text.AlignBottom; horizontalAlignment: Text.AlignRight;color: "yellow"; }
                     }
                 }
             }
         }
    }
}
