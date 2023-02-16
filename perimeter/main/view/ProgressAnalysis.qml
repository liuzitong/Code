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
    property var progressAnalysisListVm: null;
    property var progressAnalysisResult: null;
    property var staticAnalysisVm: null;
    property var currentPatient: null;

    property int report;



    signal refresh();
    signal changePage(var pageName,var params);

    onRefresh:
    {
//        console.log(progressAnalysisResult[2]);
        content.active=true;
        switch(report)
        {
        case 0:content.source="ProgressAnalysisBaseLine.qml";break;
        case 1:content.source="ProgressThreeFollowUps.qml";break;
        case 2:content.source="ProgressAnalysisSingle.qml";break;
        }
        content.item.progressAnalysisListVm=progressAnalysisListVm;
        content.item.progressAnalysisResult=progressAnalysisResult;
        if(report==2)
        {
            content.item.staticAnalysisVm=staticAnalysisVm;
            content.item.currentPatient=currentPatient;
        }
        console.log(Qt.formatDateTime(progressAnalysisListVm.getData(0,"dateTime"),"yyyy/MM/dd"));

        content.item.refresh();
    }


    Loader{
        id:content;
        active: false;
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
                    CusButton{text:lt+qsTr("Back");onClicked:{root.changePage("progressAnalysisLobby",null);content.active=false;}}}
                }
            Item{height: parent.height;width:parent.width*0.52;
                Item{anchors.fill: parent;anchors.margins:parent.height*0.15;
                    Flow{height: parent.height;spacing: height*0.8;width: parent.width;anchors.horizontalCenter: parent.horizontalCenter
                        CusButton{text:lt+qsTr("Recheck");onClicked:{root.changePage("check",null);}}
                        CusButton{text:lt+qsTr("New patient");onClicked:{;root.changePage("patientManagement","createNewPatient");}}
                    }
                }
            }

            Item{height: parent.height;width:parent.width*0.28;
                Item{id: item1;anchors.fill: parent;anchors.margins:parent.height*0.15;
                    Row
                    {
                        height: parent.height; layoutDirection: Qt.RightToLeft;spacing: height*0.8;width: parent.width
                        anchors.horizontalCenter: parent.horizontalCenter
                        CusButton{text:lt+qsTr("Print");onClicked:
                            {
                                var diagnosis=content.item.diagnosis
                                switch (report)
                                {
                                case 0:progressAnalysisListVm.getProgressBaseLineReport(diagnosis);return;
                                case 1:progressAnalysisListVm.getThreeFollowUpsReport(progressAnalysisListVm.selectedIndex,diagnosis);return;
                                case 2:progressAnalysisListVm.getSingleProgressReport(progressAnalysisListVm.selectedIndex,diagnosis);return;
                                }
                            }
                        }
//                        CusComboBoxButton{
//                            id:queryStrategy;
//                            height: parent.height;width: height*3.5;
//                            property var listModel:ListModel{ListElement{name:"进展基线";report:0}ListElement{name:"最后三次进展";report:1}ListElement{name:"单次进展分析";report:2}}
//                            comboBox.model: listModel
//                            popDirectionDown: false;complexType: true;
//                            button.text: "打印";
//                            button.onClicked:
//                            {
//                                var report=listModel.get(0).report;
//                                analysis(report);
//                            }
//                            comboBox.onActivated:
//                            {
//                                var report=listModel.get(index).report;
//                                analysis(report);
//                            }
//                            function analysis(report)
//                            {
//                                switch (report)
//                                {
//                                case 0:
//                                    progressAnalysisListVm.getProgressBaseLine(0,true);
//                                    return;
//                                case 1:
//                                    progressAnalysisListVm.getThreeFollowUps(progressAnalysisListVm.selectedIndex,0,true);
//                                    return;
//                                case 2:
//                                    progressAnalysisListVm.getSingleProgress(progressAnalysisListVm.selectedIndex,0,true);
//                                    return;
//                                }
//                            }
//                        }
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
