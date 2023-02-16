import QtQuick 2.6
import QtQuick.Controls 2.0
import QtQuick.Window 2.3
import QtQml 2.2
import QtQuick.Controls.Styles 1.4
import qxpack.indcom.ui_qml_base 1.0
import perimeter.main.view.Controls 1.0
import perimeter.main.view.Utils 1.0

Rectangle {
    id: root;visible: true;width: 1366;height: 768;color: backGroundColor;
    property int commonRadius:height*0.03
    property string backGroundColor:"#dcdee0"
    property string backGroundBorderColor:"#bdc0c6"
    property string ribbonColor: "#333e44"
    property string selectionColor: "";
    property string language:IcUiQmlApi.appCtrl.settings.language
    property var currentPatient: patientPage.currentPatient;


    signal changePage(var name);

    Settings{id:settings;anchors.fill: parent;}

    About{id:about;anchors.fill: parent;}

    Column {
        id: column;anchors.fill: parent
        Rectangle{
            id:topRibbon;width: parent.width;height: parent.height*0.1;color: "#333e44";
            CusButton{imageHightScale: 1;height:image.sourceSize.height;width:image.sourceSize.width; anchors.right: parent.right;  anchors.top: parent.top; rec.visible: false;imageSrc: "qrc:/Pics/base-svg/window_4close_1normal.svg";hoverImageSrc:"qrc:/Pics/base-svg/window_4close_2hover.svg";pressImageSrc: "qrc:/Pics/base-svg/window_4close_3press.svg";onClicked: Qt.quit()}
            Item{anchors.fill: parent;anchors.margins:parent.height*0.07;
                Column{anchors.fill:parent;spacing: height*0.1
                    Item{width: parent.width;height: parent.height*0.30;
                        Flow{height:parent.height;spacing: height*0.2
                            Image {width: height;height: parent.height;source: "qrc:/Pics/user-svg/0syseye_logo.svg";}
                            CusText{text:lt+qsTr("Computer automatic perimeter system"); horizontalAlignment: Text.AlignLeft;color: "white";font.pointSize: height*0.5;width:isEng?height*15:height*10;}

                            Flow{
                                id:patientInfo;visible: false;height: parent.height;spacing: height*0.5;
                                Flow{
                                    height: parent.height;
                                    CusText{text:lt+qsTr("Name")+":  "; horizontalAlignment: Text.AlignRight;color:"white";width: 2*height;font.pointSize: height*0.4;}
                                    CusText{id:name;text:currentPatient.name; horizontalAlignment: Text.AlignLeft;color:"white";font.pointSize: height*0.4;}
                                }
                                Flow{
                                    height: parent.height;
                                    CusText{text:lt+qsTr("Sex")+":  "; horizontalAlignment: Text.AlignRight;color:"white";width: 2*height;font.pointSize: height*0.4;}
                                    CusText{id:sex;text:{switch (currentPatient.sex){ case 0:return qsTr("Male");case 1:return qsTr("Female");case 2:return qsTr("Others");};}
                                            horizontalAlignment: Text.AlignLeft;color:"white";font.pointSize: height*0.4;width: height*1;}
                                }
                                Flow{
                                    height: parent.height;
                                    CusText{text:lt+qsTr("Age")+":  "; horizontalAlignment: Text.AlignRight;color:"white";width: 2*height;font.pointSize: height*0.4;}
                                    CusText{id:age;text:CusUtils.getAge(currentPatient.birthDate); horizontalAlignment: Text.AlignLeft;color:"white";font.pointSize: height*0.4;width: height*1;}
                                }


                                Flow{
                                    height: parent.height;
                                    CusText{text:"ID:  "; horizontalAlignment: Text.AlignRight;color:"white";width: height;font.pointSize: height*0.4;}
                                    CusText{id:id;text:currentPatient.patientId; horizontalAlignment: Text.AlignLeft;color:"white";font.pointSize: height*0.4;width: height*1.5;}
                                }
                            }
                        }
                    }
                    Item{
                        width: parent.width;height: parent.height*0.60;
                        CusButton{
                            type:"click";isAnime: false;underImageText.text: lt+qsTr("Login");underImageText.color: "white"; fontSize: height/4;rec.visible: false;width:image.sourceSize.width;imageSrc: "qrc:/Pics/base-svg/menu_login.svg";pressImageSrc: "qrc:/Pics/base-svg/menu_login_select.svg";
                            onClicked: {changePage("login");}
                        }
                        Flow{
                            id:contentSwitcher
//                            property string contentType: "patientManagement";
                            height: parent.height;
                            anchors.horizontalCenter: parent.horizontalCenter
                            Rectangle{ id:seperator1;height: parent.height; gradient: Gradient {GradientStop {position: 0;color: "#333e44"}GradientStop { position: 0.5;color: "#7e8588"}GradientStop {position: 1;color: "#333e44"; }} width: 2;}
                            CusButton{id:patientContentButton;isAnime: false;image.source:pressImageSrc;underImageText.text: lt+qsTr("Patient");underImageText.color: "white"; fontSize: height/4;rec.visible: false;width:image.sourceSize.width*2;imageSrc: "qrc:/Pics/base-svg/menu_patient.svg";pressImageSrc: "qrc:/Pics/base-svg/menu_patient_select.svg";enabled: false;/*onClicked: {switchContent("patientManagement")}*/}
                            Rectangle{ id:seperator2;height: parent.height; gradient: Gradient {GradientStop {position: 0;color: "#333e44"}GradientStop { position: 0.5;color: "#7e8588"}GradientStop {position: 1;color: "#333e44"; }} width: 2;}
                            CusButton{id:checkContentButton;isAnime: false;underImageText.text: lt+qsTr("Check");underImageText.color: "white"; rec.visible: false;width:image.sourceSize.width*2; fontSize: height/4;imageSrc: "qrc:/Pics/base-svg/menu_capture.svg";pressImageSrc: "qrc:/Pics/base-svg/menu_capture_select.svg";enabled: false;/*onClicked: {switchContent("check")}*/}
                            Rectangle{ id:seperator3;opacity: 0;height: parent.height; gradient: Gradient {GradientStop {position: 0;color: "#333e44"}GradientStop { position: 0.5;color: "#7e8588"}GradientStop {position: 1;color: "#333e44"; }} width: 2;}
                        }
                        Flow{
                            height: parent.height;anchors.right: parent.right;spacing: height*1

                            CusButton{
                                width: parent.height*2;text: "DoStuff";
//                                onClicked: {IcUiQmlApi.appCtrl.AnalysisSvc.showReport();}
                                onClicked: {IcUiQmlApi.appCtrl.testClass.test();
                               /* console.log(IcUiQmlApi.appCtrl.settings.language);*/}
                            }
                            CusButton{
                                type:"click";isAnime: false;underImageText.text: lt+qsTr("Settings");underImageText.color: "white"; fontSize: height/4;rec.visible: false;
                                width:image.sourceSize.width;imageSrc: "qrc:/Pics/base-svg/menu_set.svg";pressImageSrc: "qrc:/Pics/base-svg/menu_set_select.svg";
                                onClicked:
                                {
                                    settings.open();
                                    /*if(language==="Chinese") {IcUiQmlApi.appCtrl.language="English" }else{ IcUiQmlApi.appCtrl.language="Chinese";}*/
                                }
                            }
                            CusButton{
                                type:"click";isAnime: false;underImageText.text: lt+qsTr("Customize");underImageText.color: "white"; fontSize: height/4;rec.visible: false;width:image.sourceSize.width;imageSrc: "qrc:/Pics/base-svg/menu_customize.svg";pressImageSrc: "qrc:/Pics/base-svg/menu_customize_select.svg";
                                onClicked: contentPage.changePage("programCustomize",null);
                            }
                            CusButton{type:"click";isAnime: false;underImageText.text: lt+qsTr("About");underImageText.color: "white"; fontSize: height/4;rec.visible: false;width:image.sourceSize.width;imageSrc: "qrc:/Pics/base-svg/menu_about.svg";pressImageSrc: "qrc:/Pics/base-svg/menu_about_select.svg";onClicked: {about.open();}}
                        }
                    }
                }
            }
        }

        Item{
            id:contentPage;width:parent.width;height: parent.height*0.90;
            PatientManagement{id:patientPage;anchors.fill:parent; onChangePage: contentPage.changePage(pageName,params);}
            Check{id:checkPage;anchors.fill: parent;visible: false;currentPatient: root.currentPatient;onChangePage: contentPage.changePage(pageName,params);}
            ProgramCustomize{id:programPage;anchors.fill: parent;visible: false;onChangePage: contentPage.changePage(pageName,params);}
            AnalysisLobby{id:analysisLobbypage;anchors.fill: parent;visible: false;currentPatient: root.currentPatient;onChangePage: contentPage.changePage(pageName,params);}
            Analysis{id:analysisPage;anchors.fill: parent;visible: false;currentPatient: root.currentPatient;onChangePage: contentPage.changePage(pageName,params);}
            ProgressAnalysisLobby{id:progressAnalysisLobbyPage;anchors.fill: parent;visible: false;currentPatient: root.currentPatient;onChangePage: contentPage.changePage(pageName,params);}
            ProgressAnalysis{id:progressAnalysisPage;anchors.fill: parent;visible: false;currentPatient: root.currentPatient;onChangePage: contentPage.changePage(pageName,params);}
            VisionFieldIsland{id:visionFieldIslandPage;anchors.fill: parent;visible: false;onChangePage: contentPage.changePage(pageName,params);}
//            Component.onCompleted: {
//                IcUiQmlApi.appCtrl.changePage.connect(changePage);
//                patientPage.changePage.connect(contentPage.changePage);
//                checkPage.changePage.connect(contentPage.changePage);
//                programPage.changePage.connect(contentPage.changePage);
//                analysisLobbypage.changePage.connect(contentPage.changePage);
//                singleAnalysisPage.changePage.conect(contentPage.changePage);
//            }

            function changePage(pageName,params)
            {
                patientPage.visible=false;
                checkPage.visible=false;
                progressAnalysisLobbyPage.visible=false;
                progressAnalysisPage.visible=false;
                programPage.visible=false;
                analysisLobbypage.visible=false;
                analysisPage.visible=false;
                visionFieldIslandPage.visible=false;
                switch(pageName)
                {
                    case "patientManagement":
                        if(params==="createNewPatient") patientPage.createNewPatient();
                        patientPage.visible=true;
                        checkContentButton.image.source=checkContentButton.imageSrc;
                        patientContentButton.image.source=patientContentButton.pressImageSrc;
                        seperator1.opacity=1;
                        seperator2.opacity=1;
                        seperator3.opacity=0;
                        patientInfo.visible=false;
                        break;
                    case "analysisLobby":
                        analysisLobbypage.visible=true;
                        checkContentButton.image.source=checkContentButton.imageSrc;
                        patientContentButton.image.source=patientContentButton.pressImageSrc;
                        seperator1.opacity=1;
                        seperator2.opacity=1;
                        seperator3.opacity=0;
                        patientInfo.visible=true;
                        if(params==="patientManagement") analysisLobbypage.refresh();
                        break;
                    case "progressAnalysisLobby":
                        progressAnalysisLobbyPage.visible=true;
                        if(params!==null){
                            progressAnalysisLobbyPage.os_od=params;
                            progressAnalysisLobbyPage.refresh();
                        }
                        break;
                    case "progressAnalysis":
                        progressAnalysisPage.visible=true;
                        progressAnalysisPage.progressAnalysisListVm=params.progressAnalysisListVm;
                        progressAnalysisPage.report=params.report;
                        progressAnalysisPage.progressAnalysisResult=params.result;
                        if(params.report===2)
                        {
                            progressAnalysisPage.staticAnalysisVm=params.staticAnalysisVm;
                        }
                        progressAnalysisPage.refresh();
                        break;
                    case "check":
                        checkPage.visible=true;
//                        checkPage.rePaintCanvas();
                        if(params.pageFrom==="patientManagement") checkPage.refresh();          //区别是从其它页面返回
                        else {checkPage.currentProgram=params.currentProgram;}
                        patientContentButton.image.source=patientContentButton.imageSrc;
                        checkContentButton.image.source=checkContentButton.pressImageSrc;
                        seperator1.opacity=0;
                        seperator2.opacity=1;
                        seperator3.opacity=1;
                        patientInfo.visible=true;
                        break;
                    case "programCustomize":
                        programPage.visible=true;
                        seperator1.opacity=0;
                        seperator2.opacity=0;
                        seperator3.opacity=0;
                        patientContentButton.image.source=patientContentButton.imageSrc;
                        checkContentButton.image.source=checkContentButton.imageSrc;
//                        patientInfo.visible=false;
                        break;
                    case "analysis":
                        analysisPage.currentCheckResult=params.checkResult;
                        analysisPage.currentProgram=params.program;
                        if(analysisPage.analysisResult!==null){analysisPage.analysisResult.destroy();}
                        analysisPage.analysisResult=params.analysisResult;
                        analysisPage.visible=true;
                        analysisPage.lastPage=params.pageFrom;
                        analysisPage.report=params.report;
                        analysisPage.analysisVm=params.analysisVm;
                        analysisPage.refresh();
                        break;
                    case "visionFieldIsland":
                        visionFieldIslandPage.visible=true;
                        visionFieldIslandPage.refresh();
                        break;
                }
            }
        }
    }
}





