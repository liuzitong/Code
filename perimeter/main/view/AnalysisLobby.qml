import QtQuick 2.6
import QtQuick.Controls 2.0
import QtQuick.Window 2.3
import QtQml 2.2
import QtQuick.Controls.Styles 1.4
import perimeter.main.view.Controls 1.0
import qxpack.indcom.ui_qml_base 1.0
import perimeter.main.view.Utils 1.0
Column {
    id:root
    anchors.fill:parent;
    property var currentPatient: null;
    property string backGroundColor: CommonSettings.backGroundColor;
    property var currentCheckResult:null;
    property var analysisLobbyListVm: null;
    property var currentProgram: null;
    property var analysisVm: null;
    signal changePage(var pageName,var params);
    signal refresh()
    property int fontPointSize: CommonSettings.fontPointSize;

    onRefresh: {
        if(analysisLobbyListVm!==null) IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::AnalysisLobbyListVm",analysisLobbyListVm);
        analysisLobbyListVm=IcUiQmlApi.appCtrl.objMgr.
        attachObj("Perimeter::AnalysisLobbyListVm", false,[currentPatient.id,((content.height-10)/4*0.9-4)]);}

    ListView{
        id:content;width: parent.width;height: parent.height*14/15;/*snapMode: ListView.SnapOneItem;*/spacing: -2;clip:true;model:analysisLobbyListVm;
        delegate: checkRowDelegate
        signal cancelSelected();
//        signal setIndex();
        Component{id:checkRowDelegate
            Column{width: content.width;height: (content.height+6)/4;
                Rectangle{width: parent.width;height: 2;color: "white"}
                Rectangle{width: parent.width;height:(content.height-10)/4;color: backGroundColor;
                    Row{id: row;anchors.fill: parent;
                        Column{height: parent.height*0.8;anchors.verticalCenter: parent.verticalCenter;width: parent.width*0.07;
                            Item {width:parent.width;height: parent.height*0.18;}
                            CusText{width:parent.width;height: parent.height*0.27;text: year ;horizontalAlignment: Text.AlignHCenter;font.pointSize:fontPointSize;}
                            CusText{width:parent.width;height:parent.height*0.35;text:monthDay ;horizontalAlignment: Text.AlignHCenter;font.pointSize:fontPointSize*1.2;}
                        }
                        ListView{
                            height: parent.height*0.9;anchors.verticalCenter: parent.verticalCenter;clip: true;orientation: ListView.Horizontal;
                            spacing: height*0.15;width:parent.width*0.93;model:simpleCheckResult;
                            delegate: checkImgDelegate
                            Component{id:checkImgDelegate
                                Item{property bool selected: false;height: Math.floor(parent.height);width: Math.floor((parent.height-4)*0.8)+4;
                                    Image {
                                        id:image;height:parent.height-4;anchors.verticalCenter: parent.verticalCenter;anchors.horizontalCenter: parent.horizontalCenter;fillMode: Image.PreserveAspectFit
                                        source:"file:///" + applicationDirPath +picName;smooth: false;cache: false;
                                    }
                                    MouseArea
                                    {
                                        anchors.fill: parent;z:1;
                                        onClicked:{
                                            content.cancelSelected();parent.selected=true;
                                            console.log(checkResultId);
                                            if(currentCheckResult!==null)
                                            {
                                                if(currentCheckResult.type!==2)
                                                     IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::StaticCheckResultVm", currentCheckResult);
                                                else
                                                     IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::DynamicCheckResultVm", currentCheckResult);
                                            }

                                            if(type!==2)
                                                currentCheckResult=IcUiQmlApi.appCtrl.objMgr.attachObj("Perimeter::StaticCheckResultVm", false,[checkResultId]);
                                            else
                                                currentCheckResult=IcUiQmlApi.appCtrl.objMgr.attachObj("Perimeter::DynamicCheckResultVm", false,[checkResultId]);

                                            if(currentProgram!==null)
                                            {
                                                if(currentProgram.type!==2)
                                                    IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::StaticProgramVM", currentProgram);
                                                else
                                                    IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::DynamicProgramVM", currentProgram);
                                            }
                                            if(type!==2)
                                                currentProgram=IcUiQmlApi.appCtrl.objMgr.attachObj("Perimeter::StaticProgramVM", false,[currentCheckResult.program_id]);
                                            else
                                                currentProgram=IcUiQmlApi.appCtrl.objMgr.attachObj("Perimeter::DynamicProgramVM", false,[currentCheckResult.program_id]);
                                        }
                                    }
                                    Rectangle{anchors.fill: parent;color: "steelblue";opacity: parent.selected?1:0;z:-1;}
                                    Component.onCompleted:
                                    {
                                        content.cancelSelected.connect(function(){selected=false;})
                                    }
                                }
                            }
                        }
                    }
                }
                Rectangle{width: parent.width;height: 2;color: "white"}
            }
        }
    }
    Rectangle{id:bottomRibbon;width: parent.width;height: parent.height*1/15;color: "#333e44";
        Row{anchors.fill: parent;
            Item{height: parent.height;width:parent.width*0.20;
                Item{anchors.fill: parent;anchors.margins:parent.height*0.15;
                    CusButton{text:lt+qsTr("Back");
                        onClicked:{
                            root.changePage("patientManagement",null);
                            if(currentCheckResult!==null)
                            {
                                if(currentCheckResult.type!==2)
                                    IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::StaticCheckResultVm", currentCheckResult);
                                else
                                    IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::DynamicCheckResultVm", currentCheckResult);
                                currentCheckResult=null;
                            }
                        }}

                    }
                }

            Item{height: parent.height;width:parent.width*0.3;
                Item{anchors.fill: parent;anchors.margins:parent.height*0.15;
                    Flow{height: parent.height;spacing: height*0.8;anchors.horizontalCenter: parent.horizontalCenter;
//                        CusButton{text:"进展分析";onClicked:{changePage("progressAnalysis",null)}}
                        CusComboBoxButton{
                            height: parent.height;width:IcUiQmlApi.appCtrl.settings.isRuntimeLangEng?height*5:height*3.5;
                            property var listModel:[lt+qsTr("Left eye Analysis"),lt+qsTr("Right eye Analysis"),]
                            comboBox.model: listModel;popDirectionDown: false;complexType: false;
                            button.text: lt+qsTr("Progress Analysis");
                            button.onClicked:
                            {
                                changePage("progressAnalysisLobby",0)
                            }
                            comboBox.onActivated:
                            {
                                changePage("progressAnalysisLobby",index)
                            }
                        }
                        CusButton{
                            text:lt+qsTr("View island");
                            enabled: currentProgram.type===0&&currentProgram.category!==4;
                            onClicked:
                            {
                                var Vm=IcUiQmlApi.appCtrl.objMgr.attachObj("Perimeter::VisionFieldIslandVm", false,[currentCheckResult.id]);
                                IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::VisionFieldIslandVm",Vm);
                                changePage("visionFieldIsland",null);
                            }
                        }
                    }
                }
            }

            Item{height: parent.height;width:parent.width*0.20;
                Item{anchors.fill: parent;anchors.margins:parent.height*0.15;
                    CusButton{text:lt+qsTr("Delete"); enabled: currentCheckResult!==null;anchors.horizontalCenter: parent.horizontalCenter;
                        onClicked:analysisLobbyListVm.deleteCheckResult(currentCheckResult.id);
                    }
                }
            }

            Item{height: parent.height;width:parent.width*0.30;
                Flow{ layoutDirection: Qt.RightToLeft;anchors.fill: parent;anchors.margins:parent.height*0.15;spacing: height*0.8;
                    CusComboBoxButton{
                        id:queryStrategy;
                        height: parent.height;width: height*3.5;
                        enabled: currentCheckResult!==null;
                        property var listModel:ListModel {}
                        property var reportNames: [[lt+qsTr("Single"),lt+qsTr("Three in one"),lt+qsTr("Overview"),lt+qsTr("Three in one"),lt+qsTr("Threshold")],[lt+qsTr("Screening")],[lt+qsTr("Dynamic"),lt+qsTr("Dyanmic data")]]
                        comboBox.model: listModel;popDirectionDown: false;complexType: true;
                        button.text: lt+qsTr("Analysis");
                        button.onClicked:
                        {
                            var report=listModel.get(0).report;
                            analysis(report);
                        }
                        comboBox.onActivated:
                        {
                            var report=listModel.get(index).report;
                            analysis(report);
                        }
                        function analysis(report)
                        {
                            var diagramWidth;
                            switch (currentProgram.type)
                            {
                            case 0:
                                switch (report)
                                {
                                case 0:diagramWidth=root.height*14/15*0.92*0.97/3*1.25*0.8;break;
                                case 1:diagramWidth=root.height*14/15*0.92*0.47*0.8;break;
                                case 2:diagramWidth=root.height*14/15*0.92*0.40*0.8;break;
                                case 3:diagramWidth=root.height*14/15*0.92*0.5;break;                                                      //三合一
                                case 4:diagramWidth=root.height*14/15*0.92*0.8;break;                                                      //阈值图
                                }
                                break;
                            case 1:
                                diagramWidth=root.height*14/15*0.92*0.8;break;
                            case 2:
                                diagramWidth=root.height*14/15*0.92*0.8;break;
                            }


                            var analysisResult=null;
                            if(analysisVm!=null)
                            {
                                if(analysisVm.type!==2){ IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::StaticAnalysisVm",analysisVm);}
                                    else{ IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::DynamicAnalysisVm",analysisVm);}
                            }
                            if(currentProgram.type!==2)
                            {
                                analysisVm=IcUiQmlApi.appCtrl.objMgr.attachObj("Perimeter::StaticAnalysisVm", false,[currentCheckResult.id,diagramWidth,report]);
                                if(report===0||report===2)//三合一不用获取结果
                                {
                                    analysisResult=analysisVm.getResult();
                                }
                            }
                            else
                            {
                                analysisVm=IcUiQmlApi.appCtrl.objMgr.attachObj("Perimeter::DynamicAnalysisVm", false,[currentCheckResult.id,diagramWidth,report]);
                            }
                            changePage("analysis",{pageFrom:"analysisLobby",report:report,analysisVm:analysisVm,program:currentProgram,checkResult:currentCheckResult,analysisResult:analysisResult});
                        }

                        Component.onCompleted: {
                            root.currentProgramChanged.connect(function()
                            {
//                                console.log(currentProgram.type);
                                listModel.clear();
                                var report=currentProgram.report;
//                                console.log(report[0])
                                report.forEach(function(item){
//                                    console.log(item);
                                    listModel.append({name:reportNames[currentProgram.type][item],report:item});
                                })
                                comboBox.currentIndex=0;
                            })
                        }
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
