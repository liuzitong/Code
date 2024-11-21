import QtQuick 2.6
import QtQuick.Controls 2.0
import QtQuick.Window 2.3
import QtQml 2.2
import QtQuick.Controls.Styles 1.4
import qxpack.indcom.ui_qml_base 1.0
import perimeter.main.view.Controls 1.0
import perimeter.main.view.Utils 1.0
import qxpack.indcom.ui_qml_control 1.0

Rectangle {
    id: root;visible: true;width: 1366;height: 768;color: backGroundColor;
    property int commonRadius:height*0.03
    property string backGroundColor:"#dcdee0"
    property string backGroundBorderColor:"#bdc0c6"
    property string ribbonColor: "#333e44"
    property string selectionColor: "";
    property string language:IcUiQmlApi.appCtrl.settings.language
    property var currentPatient: patientPage.currentPatient;
    property var permission: null;
    signal login;
    property var castLightAdjustStatus:IcUiQmlApi.appCtrl.checkSvc.castLightAdjustStatus;

    function openOrCloseInfoPopup()
    {
        if(IcUiQmlApi.appCtrl.checkSvc.debugMode) return;
        if(castLightAdjustStatus===3)
        {
            idPopup2.close();


        }
        else if(visible)   //调整偏移位置 ||矫正的光强
        {
            idPopup2.open();
        }
    }

    onVisibleChanged:
    {
        openOrCloseInfoPopup()
    }

    onCastLightAdjustStatusChanged:
    {
        openOrCloseInfoPopup()

    }


    onPermissionChanged:
    {
        CommonSettings.deletePermission=false;
        for(var i=0;i<permission.length;i++)
        {
           if(permission[i]==="Perm.patient.delete")
           {
               console.log("give permission");
               CommonSettings.deletePermission=true;
           }
        }
    }

    MouseArea{anchors.fill: parent;hoverEnabled: true;
        onMouseXChanged: {if(checkPage.visible==true){IcUiQmlApi.appCtrl.checkSvc.castlightUp()}}
        onMouseYChanged: {if(checkPage.visible==true){IcUiQmlApi.appCtrl.checkSvc.castlightUp()}}
    }

    Settings{id:settings;anchors.fill: parent;}

    About{id:about;anchors.fill: parent;}

    ModalPopupDialog
    {
        id:idPopup2
        anchors.fill: parent;
        property string info: "";
        visible: false;
        reqEnterEventLoop:false;
        contentItem:
        Rectangle{
            color: "#40404040";implicitWidth: idPopup.width; implicitHeight: idPopup.height;
            Rectangle
            {
                color: "#d0ffffff";width:parent.width*0.7;height:parent.height*0.2;anchors.centerIn: parent;radius:15;
                CusText{anchors.centerIn: parent;text:qsTr("The device is calibrating,please wait a few minutes.");font.pointSize: CommonSettings.fontPointSize*2;color:"green";}
                CusButton{id:dateFromButton;anchors.right: parent.right;  anchors.top: parent.top;text:lt+qsTr("Close");height:parent.height*0.25;width:height*2;onClicked:{idPopup2.close();}}

            }

        }
    }


    ModalPopupDialog /*Rectangle*/{   // this is the wrapped Popup element in ui_qml_contro
        id:idPopup
        // reqEnterEventLoop:true;
        anchors.fill: parent;
        property bool result;
        contentItem:
        Rectangle{
            id: idContent; color: "#60606060";implicitWidth: idPopup.width; implicitHeight: idPopup.height;
            Rectangle
            {
            // [HINT] Popup element need implicitWidth & implicitHeight to calc. the right position
                id: menu; width:idPopup.width*0.22; height: idPopup.height*0.20;color: "#f0f1f2";radius: 5;/*width:480; height:480;*/
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                border.color: "#7C7C7C";
                Column{
                    anchors.fill: parent;
                    Canvas{
                        id:header;height: parent.height*.2;width: parent.width;
                        property alias radius: menu.radius;
                        onPaint: {
                            var ctx = getContext("2d");
                            ctx.beginPath(); // Start the path
                            ctx.moveTo(1,height);
                            ctx.lineTo(1, radius-1); // Set the path origin
                            ctx.arc(radius,radius,radius-1,Math.PI,Math.PI*1.5);
                            ctx.lineTo(width-radius, 1);// Set the path destination
                            ctx.arc(width-radius,radius,radius-1,Math.PI*1.5,0);
                            ctx.lineTo(width-1, height);
                            ctx.closePath();
                            context.fillStyle = "#D2D2D3"
                            ctx.fill();
                        }
                        CusText{text:lt+qsTr("  Warning"); horizontalAlignment: Text.AlignLeft;}
                    }
                    Item
                    {
                        width: parent.width;
                        height:parent.height*.8
                        Item{anchors.top: parent.top; anchors.bottom: parent.bottom;anchors.left: parent.left;anchors.right:parent.right;anchors.rightMargin: parent.height*0.1;anchors.leftMargin: parent.height*0.1;anchors.topMargin: parent.height*0.1;anchors.bottomMargin: parent.height*0.1;
                            Column {anchors.fill: parent;spacing: height*0.2;
                                Item{width:parent.width;height:parent.height*0.05;}
                                CusText{width:parent.width;height:parent.height*0.15;text:lt+qsTr("Patient is checking,are you sure?"); horizontalAlignment: Text.AlignLeft;}
                                Flow{width:parent.width;height:parent.height*0.3;spacing: width*0.1;layoutDirection: Qt.RightToLeft;
                                    CusButton{text:lt+qsTr(lt+qsTr("Cancel"));onClicked: {idPopup.result=false;idPopup.close();}}
                                    CusButton{text:lt+qsTr(lt+qsTr("OK"));onClicked:{idPopup.result=true;idPopup.close();}}
                                }
                            }
                        }
                    }


                }
            }
        }
    }


    Column {
        id: column;anchors.fill: parent
        Rectangle{
            id:topRibbon;width: parent.width;height: parent.height*0.1;color: "#333e44";
            CusButton{imageHightScale: 1;height:image.sourceSize.height;width:image.sourceSize.width; anchors.right: parent.right;  anchors.top: parent.top; rec.visible: false;imageSrc: "qrc:/Pics/base-svg/window_4close_1normal.svg";hoverImageSrc:"qrc:/Pics/base-svg/window_4close_2hover.svg";pressImageSrc: "qrc:/Pics/base-svg/window_4close_3press.svg";
                onReleased: function() { Qt.callLater(tryClose); }
                function tryClose(){
                    if(checkPage.atCheckingPage==true)
                    {
                        if(IcUiQmlApi.appCtrl.checkSvc.checkState<5)
                        {
                            idPopup.open();
                            if(idPopup.result==false)
                            {
                                // console.log("false");
                                return;
                            }
                            else
                            {
                                // console.log("true");
                                IcUiQmlApi.appCtrl.checkSvc.stop();
                            }
                        }
                    }
                    IcUiQmlApi.appCtrl.checkSvc.leaveCheck();
                    Qt.quit();
                }
            }
            Item{anchors.fill: parent;anchors.margins:parent.height*0.07;
                Column{anchors.fill:parent;spacing: height*0.1
                    Item{width: parent.width;height: parent.height*0.30;
                        Flow{height:parent.height;spacing: height*0.2
                            Image {width: height;height: parent.height;source: "qrc:/Pics/user-svg/0syseye_logo.svg";}
                            // CusText{text:lt+qsTr("Computer automatic perimeter system"); horizontalAlignment: Text.AlignLeft;color: "white";font.pointSize: height*0.5;width:isEng?height*15:height*10;}
                            Item{height:parent.height;width:parent.width*0.1;}
                            Flow{
                                id:patientInfo;visible: true;height: parent.height;spacing: height*0.5;
                                Flow{
                                    height: parent.height;
                                    CusText{text:lt+qsTr("Name")+":  "; horizontalAlignment: Text.AlignRight;color:"white";width: 2*height;font.pointSize: height*0.4;}
                                    CusText{id:name;text:currentPatient.name; horizontalAlignment: Text.AlignLeft;color:"white";font.pointSize: height*0.4;}
                                }
                                Flow{
                                    height: parent.height;
                                    CusText{text:lt+qsTr("Sex")+":  "; horizontalAlignment: Text.AlignRight;color:"white";width: 2*height;font.pointSize: height*0.4;}
                                    CusText{id:sex;text:{switch (currentPatient.sex){ case 0:return lt+qsTr("Male");case 1:return lt+qsTr("Female");case 2:return lt+qsTr("Others");};}
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
                            onClicked:login();
                        }
                        CusText
                        {
                            property string deviceStatus: IcUiQmlApi.appCtrl.checkSvc.deviceStatus===1?(qsTr("Device is reconnecting.")+lt):"";

                            property string castLightStatus: IcUiQmlApi.appCtrl.checkSvc.castLightAdjustStatus!==3?(qsTr("Device is adjusting light please wait.")+lt+"Current DA:"+IcUiQmlApi.appCtrl.deviceStatusData.castLightDA+" target DA:"+IcUiQmlApi.appCtrl.checkSvc.targetCastLightSensorDA+"."):"" ;

                            property string checkStatus:
                            {
                                if(checkPage.visible)
                                {
                                    if(IcUiQmlApi.appCtrl.checkSvc.checkState===0) return qsTr("Start");
                                    if(IcUiQmlApi.appCtrl.checkSvc.checkState===1) return qsTr("Checking");
                                    if(IcUiQmlApi.appCtrl.checkSvc.checkState===2) return qsTr("Pausing");
                                    if(IcUiQmlApi.appCtrl.checkSvc.checkState===3) return qsTr("Stoping");
                                    if(IcUiQmlApi.appCtrl.checkSvc.checkState===4) return qsTr("Finishing");
                                    if(IcUiQmlApi.appCtrl.checkSvc.checkState===5) return qsTr("Stopped");
                                    if(IcUiQmlApi.appCtrl.checkSvc.checkState===6) return qsTr("Finished");
                                }
                                else
                                {
                                    return "";
                                }
                            }
                            width: parent.width*0.2;
                            height: parent.height;
                            anchors.left: parent.left;
                            anchors.leftMargin: parent.width*0.07;
                            text:deviceStatus!=""?deviceStatus:castLightStatus!=""?castLightStatus:checkStatus;
                            horizontalAlignment: Text.AlignLeft
                            color:deviceStatus!=""?"Red":castLightStatus!=""?"Yellow":"White";
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
                                visible: false;
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
                                id:programCustomizeButton;
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
            PatientManagement{id:patientPage;anchors.fill:parent;visible: false; onChangePage: contentPage.changePage(pageName,"patientManagement",params);}
            Check{id:checkPage;anchors.fill: parent;visible: true;currentPatient: root.currentPatient;onChangePage: contentPage.changePage(pageName,"check",params);}
            ProgramCustomize{id:programPage;anchors.fill: parent;visible: false;onChangePage: contentPage.changePage(pageName,"programCustomize",params);}
            AnalysisLobby{id:analysisLobbypage;anchors.fill: parent;visible: false;currentPatient: root.currentPatient;onChangePage: contentPage.changePage(pageName,"analysisLobby",params);}
            Analysis{id:analysisPage;anchors.fill: parent;visible: false;currentPatient: root.currentPatient;onChangePage: contentPage.changePage(pageName,"analysis",params);}
            ProgressAnalysisLobby{id:progressAnalysisLobbyPage;anchors.fill: parent;visible: false;currentPatient: root.currentPatient;onChangePage: contentPage.changePage(pageName,"progressAnalysisLobby",params);}
            ProgressAnalysis{id:progressAnalysisPage;anchors.fill: parent;visible: false;currentPatient: root.currentPatient;onChangePage: contentPage.changePage(pageName,"progressAnalysis",params);}
            VisionFieldIsland{id:visionFieldIslandPage;anchors.fill: parent;visible: false;onChangePage: contentPage.changePage(pageName,"visionFieldIsland",params);}
//            Component.onCompleted: {
//                IcUiQmlApi.appCtrl.changePage.connect(changePage);
//                patientPage.changePage.connect(contentPage.changePage);
//                checkPage.changePage.connect(contentPage.changePage);
//                programPage.changePage.connect(contentPage.changePage);
//                analysisLobbypage.changePage.connect(contentPage.changePage);
//                singleAnalysisPage.changePage.conect(contentPage.changePage);
//            }

            function changePage(pageTo,pageFrom,params)
            {
                patientPage.visible=false;
                checkPage.visible=false;
                progressAnalysisLobbyPage.visible=false;
                progressAnalysisPage.visible=false;
                programPage.visible=false;
                analysisLobbypage.visible=false;
                analysisPage.visible=false;
                visionFieldIslandPage.visible=false;
                programCustomizeButton.enabled=false;
                console.log(params);

                console.log(pageTo);
                console.log(pageFrom);
                switch(pageTo)
                {
                    case "patientManagement":
                        programCustomizeButton.enabled=true;
                        patientPage.visible=true;
                        patientPage.pageFrom=pageFrom;
                        checkContentButton.image.source=checkContentButton.imageSrc;
                        patientContentButton.image.source=patientContentButton.pressImageSrc;
                        if(pageFrom==="analysis") patientPage.createNewPatient();
                        seperator1.opacity=1;
                        seperator2.opacity=1;
                        seperator3.opacity=0;
                        patientInfo.visible=false;
                        break;
                    case "analysisLobby":
                        analysisLobbypage.visible=true;
                        analysisLobbypage.pageFrom=pageFrom;
                        checkContentButton.image.source=checkContentButton.imageSrc;
                        patientContentButton.image.source=patientContentButton.pressImageSrc;
                        seperator1.opacity=1;
                        seperator2.opacity=1;
                        seperator3.opacity=0;
                        patientInfo.visible=true;
                        if(pageFrom==="patientManagement") analysisLobbypage.refresh();
                        break;
                    case "progressAnalysisLobby":
                        progressAnalysisLobbyPage.visible=true;
                        progressAnalysisLobbyPage.pageFrom=pageFrom;
                        if(pageFrom==="analysisLobby")
                        {
                            progressAnalysisLobbyPage.os_od=params;
                            progressAnalysisLobbyPage.refresh();
                        }
                        break;
                    case "progressAnalysis":
                        progressAnalysisPage.visible=true;
                        progressAnalysisPage.pageFrom=pageFrom;
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
                        checkPage.pageFrom=pageFrom;
                        checkPage.atCheckingPage=true;
//                        checkPage.rePaintCanvas();
                        if(pageFrom==="patientManagement")
                        {
                            if(params.type==="check")
                            {
                                checkPage.refresh();          //区别是从其它页面返回
                            }
                            else
                            {
                                checkPage.currentProgram=params.lastProgram;                //recheck
                            }

                        }
                        else if(pageFrom==="analysis")
                        {
                            if(params.type==="reCheck")
                                checkPage.currentProgram=params.currentProgram;
                        }
                        IcUiQmlApi.appCtrl.checkSvc.enterCheck();
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
                        break;
                    case "analysis":
                        analysisPage.pageFrom=pageFrom;
                        analysisPage.currentCheckResult=params.checkResult;
                        analysisPage.currentProgram=params.program;
                        analysisPage.analysisResult=params.analysisResult;
                        analysisPage.report=params.report;
                        console.log(params.report);
                        analysisPage.type=params.type;
                        console.log(params.type);
                        analysisPage.analysisVm=params.analysisVm;
                        analysisPage.refresh();
                        analysisPage.visible=true;
                        break;
                    case "visionFieldIsland":
                        visionFieldIslandPage.pageFrom=pageFrom;
                        visionFieldIslandPage.visible=true;
                        visionFieldIslandPage.refresh();
                        break;
                }
            }
        }
    }
}





