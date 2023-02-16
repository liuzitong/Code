import QtQuick 2.6
import QtQuick.Controls 2.0
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
    property var currentProgram: null;
    property var currentPatient: null;
    property var currentCheckResult: null;
    property var progressAnalysisListVm: null;//<-
    property var progressAnalysisResult: null;//-<
    property var staticAnalysisVm: null;//<-
    property var staticAnalysisResult: null;
    property int fontPointSize: CommonSettings.fontPointSize;
    property int textHeight:CommonSettings.textHeight;
    property alias diagnosis: diagnosis.text;
    property string rx:"";
    signal clearDot;
    property int range: currentProgram.params.commonParams.Range[1];
    width: 1366;
    height: 660;
    onRefresh:
    {
        var checkResultId=progressAnalysisListVm.selectedResultId;
        if(currentCheckResult!==null)
        {
            if(currentCheckResult.type!==2)
                IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::StaticCheckResultVm", currentCheckResult);
            else
                IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::DynamicCheckResultVm", currentCheckResult);
        }
        currentCheckResult=IcUiQmlApi.appCtrl.objMgr.attachObj("Perimeter::StaticCheckResultVm", false,[checkResultId]);

        if(currentProgram!==null)
           IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::StaticProgramVM", currentProgram);
        currentProgram=IcUiQmlApi.appCtrl.objMgr.attachObj("Perimeter::StaticProgramVM", false,[currentCheckResult.program_id]);

        if(currentCheckResult.OS_OD===0)  //左
            rx=currentPatient.rx.rx1_l.toFixed(2)+" DS:"+currentPatient.rx.rx2_l.toFixed(2)+" DC:"+currentPatient.rx.rx3_l.toFixed(2);
        else
            rx=currentPatient.rx.rx1_r.toFixed(2)+" DS:"+currentPatient.rx.rx2_r.toFixed(2)+" DC:"+currentPatient.rx.rx3_r.toFixed(2);

        if(staticAnalysisResult!==null)
        {
            staticAnalysisResult.destroy();
        }
        staticAnalysisResult=staticAnalysisVm.getResult();
    }

    Row{
        anchors.fill: parent;
        Rectangle{width:parent.width*0.75;height: parent.height;color:"white";
            Row{anchors.fill: parent;anchors.leftMargin: parent.width*0.06;anchors.topMargin: parent.height*0.04;anchors.bottomMargin: parent.height*0.04;spacing: width*0.05;
                Column{ id: column;width: parent.width*0.25;height: parent.height;spacing:parent.height*0.02
                    CusText{text:currentProgram.name; font.bold: true; horizontalAlignment: Text.AlignLeft;height:parent.height*0.10;font.pointSize: fontPointSize*2;}
                    Column{id:ttt;width:parent.width;height: parent.height*0.25;spacing: textHeight*0.5;
                        Repeater{
                            property var params: currentCheckResult.params.commonParams;
                            property int timeSpan:currentCheckResult.resultData.testTimespan;
                            property var fixationMonitor: [lt+qsTr("No alarm"),lt+qsTr("Only alarm"),lt+qsTr("Alarm and pause")];
                            property var fixationTarget: [lt+qsTr("Center Dot"),lt+qsTr("Small diamond"),lt+qsTr("Big diamond"),lt+qsTr("Bottom dot")]
                            property var centerDotCheck: [lt+qsTr("On"),lt+qsTr("Off")];

                            model: [
                                {name:lt+qsTr("Eye move alarm mode"),param:fixationMonitor[params.fixationMonitor]},
                                {name:lt+qsTr("Fixation target"),param:fixationTarget[params.fixationTarget]},
                                {name:lt+qsTr("Fixation loss rate"),param:currentCheckResult.resultData.fixationLostCount+'/'+currentCheckResult.resultData.fixationLostTestCount},
                                {name:lt+qsTr("False positive rate"),param:Math.round(currentCheckResult.resultData.falsePositiveCount/currentCheckResult.resultData.falsePositiveTestCount*100)+"%"},
                                {name:lt+qsTr("False negative rate"),param:Math.round(currentCheckResult.resultData.falseNegativeCount/currentCheckResult.resultData.falseNegativeTestCount*100)+"%"},
                                {name:lt+qsTr("Check time"),param:Math.floor(timeSpan/60)+":"+timeSpan%60},
                                {name:lt+qsTr("Center Dot check"),param:params.centerDotCheck?centerDotCheck[0]:centerDotCheck[1]}]
                           CusText{text:modelData.name+":  "+modelData.param; horizontalAlignment: Text.AlignLeft;height:textHeight;width: parent.width;}
                        }
                    }
                    Column{width:parent.width;height: parent.height*0.12;spacing: textHeight*0.5;
                        Repeater{
                            property var params: currentCheckResult.params.commonParams;
                            property var cursorSize: ["I","II","III","IV","V"];
                            property var cursorColor: [lt+qsTr("White"),lt+qsTr("Red"),lt+qsTr("Blue")];
                            property var backGroundColor: ["31.5 ASB","315 ASB"];
                            property var strategy: [lt+qsTr("Full threshold"),lt+qsTr("Smart interactive"),lt+qsTr("Fast interactive"),lt+qsTr("One stage"),lt+qsTr("Two stages"),lt+qsTr("Quantify defects"),lt+qsTr("Single stimulus")]
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
                                width:parent.width*1.0;height: parent.height*0.70;
                                selectionColor: "blue";selectByMouse: true;
                                font.pointSize: fontPointSize;font.family: "Consolas";
                                wrapMode: Text.WrapAnywhere;renderType: Text.NativeRendering;
                            }
                        }
                    }


                    Item{width:parent.width;height: parent.height*0.08;
                        FixationDeviation{ dots:if(currentCheckResult!==null) currentCheckResult.resultData.fixationDeviation; }
                    }
                }
                Row {width: parent.width*0.70;height:parent.height
                    Column{width: parent.width*0.70;height:parent.height/*spacing:height*0.035*/
                        Row{ id: row;width: parent.width;height:parent.height*0.96/3;spacing:width*0.1
                            AnalysisDiagram{
                                width: parent.height;height:parent.height*1.25;
                                anchors.verticalCenter: parent.verticalCenter
                                source:"/previewImage/dbDiagram.bmp";
                                range:root.range;
                                analysisVm: root.staticAnalysisVm;
                                onClicked:root.clearDot();
                                Component.onCompleted: {root.refresh.connect(refresh);root.clearDot.connect(clearDot);}
                            }
                            AnalysisDiagram{
                                width: parent.height;height:parent.height*1.25;
                                anchors.verticalCenter: parent.verticalCenter
                                source:"/previewImage/gray.bmp";
                                range:root.range;
                                analysisVm: root.staticAnalysisVm;
                                onClicked:root.clearDot();
                                Component.onCompleted: {root.refresh.connect(refresh);root.clearDot.connect(clearDot);}
                            }
                        }
                        Item{width: parent.width;height:parent.height*0.01;}

                        Row{
                            width: parent.width;height:parent.height*0.96/3;spacing:width*0.1
                            AnalysisDiagram{
                                width: parent.height;height:parent.height*1.25;
                                anchors.verticalCenter: parent.verticalCenter
                                source:"/previewImage/TotalDeviation.bmp";
                                range:root.range;
                                analysisVm: root.staticAnalysisVm;
                                onClicked:root.clearDot();
                                Component.onCompleted: {root.refresh.connect(refresh);root.clearDot.connect(clearDot);}
                            }

                            AnalysisDiagram{
                                width: parent.height;height:parent.height*1.25;
                                anchors.verticalCenter: parent.verticalCenter
                                source:"/previewImage/PatternDeviation.bmp";
                                range:root.range;
                                analysisVm: root.staticAnalysisVm;
                                onClicked:root.clearDot();
                                Component.onCompleted: {root.refresh.connect(refresh);root.clearDot.connect(clearDot);}
                            }


                        }
                        Row{width: parent.width;height:parent.height*0.03;spacing:width*0.1;
                            CusText{width:parent.parent.height*0.98/3;height: parent.height;text: lt+qsTr("Total deviation"); verticalAlignment: Text.AlignVCenter; font.pointSize: height*0.50}
                            CusText{width:parent.parent.height*0.98/3;height: parent.height;text: lt+qsTr("Pattern deviation");  verticalAlignment: Text.AlignVCenter;font.pointSize: height*0.50}
                        }
                        Row{
                            width: parent.width;height:parent.height*0.96/3;spacing:width*0.1;
                            AnalysisDiagram{
                                width: parent.height;height:parent.height*1.25;
                                anchors.verticalCenter: parent.verticalCenter
                                source:"/previewImage/TotalPE.bmp";
                                range:root.range;
                                analysisVm: root.staticAnalysisVm;
                                onClicked:root.clearDot();
                                Component.onCompleted: {root.refresh.connect(refresh);root.clearDot.connect(clearDot);}
                            }
                            AnalysisDiagram{
                                width: parent.height;height:parent.height*1.25;
                                anchors.verticalCenter: parent.verticalCenter
                                source:"/previewImage/PatternPE.bmp";
                                range:root.range;
                                analysisVm: root.staticAnalysisVm;
                                onClicked:root.clearDot();
                                Component.onCompleted: {root.refresh.connect(refresh);root.clearDot.connect(clearDot);}
                            }

//                            }
                        }
                    }
                    Column{width: parent.width*0.3;height:parent.height;spacing: textHeight*0.5;
                        CusText{width:parent.width;height:textHeight;text:"VFI:"+Math.round(staticAnalysisResult.VFI*100)+"%" ;horizontalAlignment: Text.AlignLeft;}
                        CusText{
                            property var ght: if(staticAnalysisResult.GHT===0){return lt+qsTr("Out of limits");} else if(staticAnalysisResult.GHT===1){return lt+qsTr("Low sensitivity");}else if(staticAnalysisResult.GHT===2){return lt+qsTr("Border of limits");}else if(staticAnalysisResult.GHT===3){return lt+qsTr("Within normal limits");}
                            width:parent.width;height:textHeight;horizontalAlignment: Text.AlignLeft;text:lt+qsTr("GHT")+": "+ ght;}
                        CusText{width:parent.width;height:textHeight;text:lt+qsTr("MD")+": "+staticAnalysisResult.md.toFixed(2)+(staticAnalysisResult.p_md>0?" (<"+staticAnalysisResult.p_md+"%)":"") ;horizontalAlignment: Text.AlignLeft;}
                        CusText{width:parent.width;height:textHeight;text:lt+qsTr("PSD")+": "+staticAnalysisResult.psd.toFixed(2)+(staticAnalysisResult.p_psd>0?" (<"+staticAnalysisResult.p_psd+"%)" :"");horizontalAlignment: Text.AlignLeft;}
                        Rectangle{
                            width: parent.width;height: parent.height*0.66;border.color: "black";
                            Column{
                                width: parent.width*0.9;height: parent.height; anchors.horizontalCenter: parent.horizontalCenter;spacing: textHeight*0.5;
                                CusText{width: parent.width;height: textHeight;text:"GPA";}
                                Image {
                                    height: sourceSize.height; anchors.horizontalCenter: parent.horizontalCenter;width: sourceSize.width;
                                    smooth: false;
                                    cache: false;        //to refresh image
                                    Component.onCompleted: {root.refresh.connect(function(){source="";source="file:///" + applicationDirPath + "/previewImage/single_progressPic.bmp";})}
                                }
                                CusText{
                                    property var progress: [lt+qsTr("no progress"),lt+qsTr("possible progress"),lt+qsTr("very possible progress")];
                                    height: textHeight;text:progressAnalysisResult===null?"":progress[progressAnalysisResult] ;horizontalAlignment: Text.AlignLeft
                                }
                                CusText{height: textHeight;text:lt+qsTr("BaseLine dates")+":" ;horizontalAlignment: Text.AlignLeft}
                                Row{width: parent.width;height: textHeight;spacing: parent.width*0.1;
                                    CusText{height: parent.height;width:parent.width*0.4;text:progressAnalysisListVm===null?"":Qt.formatDateTime(progressAnalysisListVm.getData(0,"dateTime"),"yyyy/MM/dd"); horizontalAlignment: Text.AlignLeft}
                                    CusText{height: parent.height;width:parent.width*0.4;text:progressAnalysisListVm===null?"":Qt.formatDateTime(progressAnalysisListVm.getData(1,"dateTime"),"yyyy/MM/dd"); horizontalAlignment: Text.AlignLeft}
                                }
                                CusText{height: textHeight;text:lt+qsTr("Previous dates")+":" ;horizontalAlignment: Text.AlignLeft}
                                Row{width: parent.width;height: textHeight;spacing: parent.width*0.1;
                                    CusText{height: parent.height;width:parent.width*0.4;text:progressAnalysisListVm===null?"":Qt.formatDateTime(progressAnalysisListVm.getData(progressAnalysisListVm.selectedIndex-2,"dateTime"),"yyyy/MM/dd"); horizontalAlignment: Text.AlignLeft}
                                    CusText{height: parent.height;width:parent.width*0.4;text:progressAnalysisListVm===null?"":Qt.formatDateTime(progressAnalysisListVm.getData(progressAnalysisListVm.selectedIndex-1,"dateTime"),"yyyy/MM/dd"); horizontalAlignment: Text.AlignLeft}
                                }
                                Repeater{
                                    anchors.fill: parent;
                                    model: [{image:"qrc:/grays/GPA1.bmp",progress:"<5% ("+qsTr("Deterioarted")+")"},{image:"qrc:/grays/GPA2.bmp",progress:"<5% (2"+qsTr("Consecutiveness")+")"},{image:"qrc:/grays/GPA3.bmp",progress:"<5% (3"+qsTr("Consecutiveness")+")"},{image:"qrc:/grays/GPA4.bmp",progress:lt+qsTr("Out of range")}]
                                    Row{
                                        height: textHeight;width: parent.width;
        //                                height:parent.height*0.14;width: parent.width;spacing: height*0.5
                                        Image {height: sourceSize.height*2; fillMode: Image.PreserveAspectFit; width:sourceSize.height*2;smooth: false;source: modelData.image}
                                        CusText{height: textHeight;text:modelData.progress; horizontalAlignment: Text.AlignLeft;anchors.verticalCenter: parent.verticalCenter;}
                                    }
                                }
                            }
                        }


                        Column
                        {
                            width: parent.width;height: parent.height*0.25;spacing: textHeight*0.5;
                            Repeater{
                                anchors.fill: parent;
                                model: [{image:"qrc:/grays/PE1.bmp",pe:"<5%"},{image:"qrc:/grays/PE2.bmp",pe:"<2%"},{image:"qrc:/grays/PE3.bmp",pe:"<1%"},{image:"qrc:/grays/PE4.bmp",pe:"<0.5%"}]
                                Row{
                                    height: textHeight;width: parent.width;
    //                                height:parent.height*0.14;width: parent.width;spacing: height*0.5
                                    Image {height: sourceSize.height*2; fillMode: Image.PreserveAspectFit; width:sourceSize.height*2;smooth: false;source: modelData.image}
                                    CusText{height: textHeight;text:modelData.pe; horizontalAlignment: Text.AlignLeft;anchors.verticalCenter: parent.verticalCenter;}
                                }
                            }
                        }


                    }
                }


            }
        }


        Rectangle{width:parent.width*0.25;height: parent.height;color:"white";}
    }

}


