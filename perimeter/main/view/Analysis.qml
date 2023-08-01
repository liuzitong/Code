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
    property var currentProgram: null;
    property var currentCheckResult: null;
    property var analysisResult: null;
    property var analysisVm: null;
    property int report;
    property string pageFrom: "";

//    property int textHeight: height*0.05;


    signal refresh();
    signal changePage(var pageName,var params);


    onRefresh: {
//        console.log("report type is "+report);
        var type=currentProgram!=null?currentProgram.type:0;
        if(type!==2)
        {
            if(!(type===0&&report==2))
            {
                content.source="StaticAnalysis.qml";
                content.item.analysisResult=analysisResult;
                content.item.analysisVm=analysisVm;
            }
            else
            {
                content.source="StaticAnalysisOverview.qml";
                content.item.analysisVm=analysisVm;
            }
        }
        else
        {
            content.source="DynamicAnalysis.qml";
        }



//        content.item.textHeight=textHeight;
        content.item.currentPatient=currentPatient;
        content.item.currentProgram=currentProgram;
        content.item.currentCheckResult=currentCheckResult;
        content.item.report=report;
        content.item.refresh();
    }


    Loader{
        id:content;
        width: parent.width;height: parent.height*14/15;
    }


//    onRefresh: {
//        sf.textHeight=textHeight;
//        sf.currentPatient=currentPatient;
//        sf.currentProgram=currentProgram;
//        sf.currentCheckResult=currentCheckResult;
//        sf.analysisResult=analysisResult;
//        sf.refresh();
//    }


//    SingleField{
//        id:sf;
//        width: parent.width;height: parent.height*14/15;
//    }

    Rectangle{id:bottomRibbon;width: parent.width;height: parent.height*1/15;color:CommonSettings.ribbonColor;
        Row{anchors.fill: parent;
            Item{height: parent.height;width:parent.width*0.20;
                Item{anchors.fill: parent;anchors.margins:parent.height*0.15;
                    CusButton{text:lt+qsTr("Back");onClicked:{
//                            if(pageFrom=="check")
//                                root.changePage(pageFrom,{currentProgram:currentProgram});
//                            else if(pageFrom=="analysisLobby")
                                root.changePage(pageFrom,{});
                        }}}
                }
            Item{height: parent.height;width:parent.width*0.52;
                Item{anchors.fill: parent;anchors.margins:parent.height*0.15;
                    Flow{height: parent.height;spacing: height*0.8;width: parent.width;anchors.horizontalCenter: parent.horizontalCenter
                        CusButton{text:lt+qsTr("Recheck");enabled:currentProgram!==null/*&&IcUiQmlApi.appCtrl.checkSvc.castLightAdjustStatus===3*/;
                            onClicked:{
                                currentProgram.params=currentCheckResult.params;
                                root.changePage("check",{currentProgram:currentProgram,type:"reCheck"});
                            }
                        }
                        CusButton{text:lt+qsTr("New patient");onClicked:{root.changePage("patientManagement",{});}}
                    }
                }
            }

            Item{height: parent.height;width:parent.width*0.28;
                Item{anchors.fill: parent;anchors.margins:parent.height*0.15;
                    Row
                    {
                        height: parent.height; layoutDirection: Qt.RightToLeft;spacing: height*0.8;width: parent.width
                        anchors.horizontalCenter: parent.horizontalCenter
                        CusButton{text:lt+qsTr("Print");onClicked:{console.log(currentCheckResult.diagnosis);analysisVm.showReport(report);}}
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
