import QtQuick 2.6
import QtQuick.Controls 2.0
import QtQuick.Window 2.3
import QtQml 2.2
import QtQuick.Controls.Styles 1.4
import perimeter.main.view.Controls 1.0
import qxpack.indcom.ui_qml_base 1.0
import QtMultimedia 5.8
import perimeter.main.view.Utils 1.0
import QtQuick.Extras 1.4

Item {id:root; width: 1366;height: 691
    signal changePage(var pageName,var params);
    function rePaintCanvas(){checkDisplay.displayCanvas.requestPaint();}
    property string backGroundColor:"#dcdee0"
    property string backGroundColorCheckPanel:"#cbced0"
    property string backGroundBorderColor:"#bdc0c6"
    property var currentProgram: null;
    property var currentPatient: null;
    property var currentCheckResult: null;
    property var analysisVm: null;
    signal refresh();
    property int fontPointSize: CommonSettings.fontPointSize;
    signal realTimePicRefresh(var count);
    property var frameProvidSvc: null;
    property var checkSvc: IcUiQmlApi.appCtrl.checkSvc;


    Component.onCompleted:{
        frameProvidSvc=IcUiQmlApi.appCtrl.frameProvidSvc;
        checkSvc.checkResultChanged.connect(currentCheckResultChanged);
        refresh();
    }


    onRefresh: {
        if (currentProgram==null)
        {
            console.log("changeProgram");
            var program_id=IcUiQmlApi.appCtrl.settings.defaultProgramId;
            var program_type=IcUiQmlApi.appCtrl.settings.defaultProgramType;
            if(program_type!==2)
                currentProgram=IcUiQmlApi.appCtrl.objMgr.attachObj("Perimeter::StaticProgramVM", false,[program_id]);
            else
                currentProgram=IcUiQmlApi.appCtrl.objMgr.attachObj("Perimeter::DynamicProgramVM", false,[program_id]);
        }
        currentProgramChanged();         //不知道为毛需要主动触发,还必须放外面
        program_type!==2?staticParamsSetting.currentProgram=currentProgram:dynamicParamsSetting.currentProgram=currentProgram;
        if(currentCheckResult!=null)
        {
            if(currentProgram.type!==2)
                IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::StaticCheckResultVm",currentCheckResult);
            else
                IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::DynamicCheckResultVm",currentCheckResult);
            currentCheckResult=null;
        }
        realTimeDBRec.visible=false;
        checkDisplay.clickedDotIndex=-1;
    }



    Column{anchors.fill: parent;
        Rectangle{width: parent.width; height: parent.height*14/15; id:content;
            ChooseProgram{id:chooseProgram;anchors.fill: parent;
                onOk:{
                    root.currentProgram=currentProgram;
                    currentProgram.type!==2?staticParamsSetting.currentProgram=currentProgram:dynamicParamsSetting.currentProgram=currentProgram;
                    if(currentCheckResult.type!==2)
                        IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::StaticCheckResultVm",currentCheckResult);
                    else
                        IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::DynamicCheckResultVm",currentCheckResult);
                    currentCheckResult=null;
                }
            }
            DynamicParamsSetting{id:dynamicParamsSetting;anchors.fill: parent;onDataRefreshed:root.currentProgramChanged();}
            StaticParamsSetting{id:staticParamsSetting;anchors.fill: parent;isCustomProg:true; onDataRefreshed:{root.currentProgramChanged();}}
            Item{anchors.fill: parent;anchors.margins: 2;
                Row{anchors.fill: parent;spacing: 2;
                    Rectangle{ width: parent.width*0.25-2;height: parent.height;color: backGroundColor;
                        Item{ anchors.fill: parent;anchors.leftMargin:parent.height*0.05;anchors.rightMargin: parent.height*0.05;anchors.topMargin: parent.height*0.02;anchors.bottomMargin: parent.height*0.02;
                            Column{anchors.fill: parent;spacing:height* 0.12
                                Rectangle{id: rectangle; width: parent.width;height: parent.height*0.28;anchors.horizontalCenter: parent.horizontalCenter;border.color: backGroundBorderColor;color: backGroundColor;radius: width*0.03;
                                    Item{anchors.fill: parent;anchors.margins: parent.height*0.1;
                                        Column{anchors.fill: parent;spacing: 0.175*height;
                                            Row{width:parent.width;height: parent.height*0.65/3;spacing: width*0.05;
                                                CusText{text:lt+qsTr("Program name"); horizontalAlignment: Text.AlignLeft;width: parent.width*0.45;}
                                                LineEdit {
                                                    text:currentProgram==null?"":currentProgram.name; width: parent.width*0.50;textInput.readOnly: true;
                                                    Component.onCompleted: {currentProgramChanged.connect(function(){text=currentProgram.name});}
                                                }
                                            }
                                            Row{width:parent.width;height: parent.height*0.65/3;spacing: width*0.05;
                                                CusText{text:lt+qsTr("Cursor"); horizontalAlignment: Text.AlignLeft;width: parent.width*0.45;font.pointSize: fontPointSize;}
                                                LineEdit{
                                                    property var cursorSize: ["I","II","III","IV","V"];
                                                    property var cursorColor: [lt+qsTr("White"),lt+qsTr("Red"),lt+qsTr("Blue")];
                                                    property var params:currentProgram.type!==2?currentProgram.params.commonParams:currentProgram.params;
                                                    text:currentProgram==null?"":cursorSize[params.cursorSize]+","+cursorColor[params.cursorColor];
                                                    width: parent.width*0.50;textInput.readOnly: true;
                                                }
                                            }
                                            Row{width:parent.width;height: parent.height*0.65/3;spacing: width*0.05;
                                                CusText{text:lt+qsTr("Strategy"); horizontalAlignment: Text.AlignLeft;width: parent.width*0.45;font.pointSize: fontPointSize;}
                                                LineEdit{
                                                    property var staticStrategy: [lt+qsTr("Full threshold"),lt+qsTr("Smart interactive"),lt+qsTr("Fast interative"),lt+qsTr("One stage"),lt+qsTr("Two stages"),lt+qsTr("Quantify defects"),lt+qsTr("Single stimulus")];
                                                    property var dynamicStrategy: [lt+qsTr("Standard"),lt+qsTr("Blind area"),lt+qsTr("Dark area"),lt+qsTr("Single stimulus")]
                                                    property var params:currentProgram.type!==2?currentProgram.params.commonParams:currentProgram.params;
                                                    width: parent.width*0.5;textInput.readOnly: true;
                                                    text:currentProgram.type!==2?staticStrategy[params.strategy]:dynamicStrategy[params.strategy];
//                                                    Component.onCompleted: {currentProgramChanged.connect(function(){
//                                                        text="";
//                                                        var params=(currentProgram.type!==2?currentProgram.params.commonParams:currentProgram.params);
//                                                        if(currentProgram.type!==2)
//                                                            switch (params.strategy){ case 0:text+=lt+qsTr("Full threshold");break;case 1:text+=lt+qsTr("Smart interactive");break;case 2:text+=lt+qsTr("Fast interative");break;case 3: text+=lt+qsTr("One stage");break;
//                                                                                      case 4:text+=lt+qsTr("Two stages");break;case 5:text+=lt+qsTr("Quantify defects");break;case 6:text+=lt+qsTr("Single stimulus");break;}
//                                                        else
//                                                            switch (params.strategy){ case 0:text+=lt+qsTr("Standard");break;case 1:text+=lt+qsTr("Blind area");break;case 2:text+=lt+qsTr("Dark area");break;case 3: text+=lt+qsTr("Straight line");break;}
//                                                    });}
                                                }
                                            }
                                        }
                                    }
                                }
                                Rectangle{ width: parent.width;height: parent.height*0.28; anchors.horizontalCenter: parent.horizontalCenter; border.color:backGroundBorderColor; color: currentProgram.type===null? backGroundColor:currentProgram.type!==2?backGroundColor:"grey";radius: width*0.03;
                                    Item{ anchors.fill: parent;anchors.margins: parent.height*0.1;
                                        Column{anchors.fill: parent;spacing: 0.175*height;
                                            Row{width:parent.width;height: parent.height*0.65/3;spacing: width*0.05;
                                                CusText{text:lt+qsTr("False positive rate"); horizontalAlignment: Text.AlignLeft;width: parent.width*0.45;font.pointSize: fontPointSize;}
                                                LineEdit{
                                                    property var checkedDots:currentCheckResult===null?0:currentCheckResult.type===2?0:currentCheckResult.resultData.falsePositiveCount;
                                                    property var totalDots: currentCheckResult===null?0:currentCheckResult.type===2?0:currentCheckResult.resultData.falsePositiveTestCount;
                                                    text:currentCheckResult===null?"":currentCheckResult.type!==2?checkedDots+"/"+totalDots:"";width: parent.width*0.5;textInput.readOnly: true;
                                                }
                                            }
                                            Row{width:parent.width;height: parent.height*0.65/3;spacing: width*0.05;
                                                CusText{text:lt+qsTr("False negative rate"); horizontalAlignment: Text.AlignLeft;width: parent.width*0.45;font.pointSize: fontPointSize;}
                                                LineEdit{
                                                    property var checkedDots: currentCheckResult===null?0:currentCheckResult.type===2?0:currentCheckResult.resultData.falseNegativeCount;
                                                    property var totalDots: currentCheckResult===null?0:currentCheckResult.type===2?0:currentCheckResult.resultData.falseNegativeTestCount;
                                                    text:currentCheckResult===null?"":currentCheckResult.type!==2?checkedDots+"/"+totalDots:"";width: parent.width*0.5;textInput.readOnly: true;
                                                }
                                            }
                                            Row{ width:parent.width;height: parent.height*0.65/3;spacing: width*0.05;
                                                CusText{text:lt+qsTr("Fixation loss rate"); horizontalAlignment: Text.AlignLeft;width: parent.width*0.45;font.pointSize: fontPointSize;}
                                                LineEdit{
                                                    property var checkedDots: currentCheckResult===null?0:currentCheckResult.type===2?0:currentCheckResult.resultData.fixationLostCount;
                                                    property var totalDots: currentCheckResult===null?0:currentCheckResult.type===2?0:currentCheckResult.resultData.fixationLostTestCount;
                                                    text:currentCheckResult===null?"":currentCheckResult.type!==2?checkedDots+"/"+totalDots:"";width: parent.width*0.5;textInput.readOnly: true;
                                                }
                                            }
                                        }
                                    }
                                }
                                Rectangle{width: parent.width;height: parent.height*0.20;anchors.horizontalCenter: parent.horizontalCenter;border.color: backGroundBorderColor; color: backGroundColor;radius: width*0.03;
                                    Item{anchors.fill: parent;anchors.margins: parent.height*0.13;
                                        Column{anchors.fill: parent;spacing: 1/3*height;
                                            Row{width:parent.width;height: parent.height*1/3;spacing: width*0.05;
                                                CusText{text:lt+qsTr("Check dot count"); horizontalAlignment: Text.AlignLeft;width: parent.width*0.45;font.pointSize: fontPointSize;}
                                                LineEdit{
//                                                    property int centerDot:  currentProgram.params.commonParams.centerDotCheck?1:0;
                                                    property var checkedDots: IcUiQmlApi.appCtrl.checkSvc.checkedCount;
                                                    property var totalDots: currentProgram===null?0:currentProgram.data.dots.length/*+centerDot*/;
                                                    text:checkedDots+"/"+totalDots;width: parent.width*0.5;textInput.readOnly: true;
                                                }
                                            }
                                            Row{width:parent.width;height: parent.height*1/3;spacing: width*0.05;
                                                CusText{text:lt+qsTr("Check timespan"); horizontalAlignment: Text.AlignLeft;width: parent.width*0.45;font.pointSize: fontPointSize;}
                                                LineEdit{
                                                    property int timeSpan: IcUiQmlApi.appCtrl.checkSvc.checkTime;
                                                    text:Math.floor(timeSpan/60)+":"+timeSpan%60;width: parent.width*0.5;textInput.readOnly: true;}
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    Rectangle{ width: parent.width*0.25-2;height:parent.height;color: backGroundColor;
                        Item{anchors.fill: parent;anchors.margins: parent.height*0.02;
                            Column{id: column;anchors.fill: parent;spacing:/*(height-videoArea.height-controlPanel.height-eyeOptionsGroup.height)/2*/height*0.03;
                                Item{id:videoArea; width: parent.width*0.83;height: width*3/4;anchors.horizontalCenter: parent.horizontalCenter;
                                    Rectangle
                                    {   anchors.fill: parent;color:"black";
                                        VideoOutput{
                                            source: frameProvidSvc //cameraDev
                                            anchors.fill: parent
                                            focus : visible
                                            id:vedio;

                                        }
                                    }
                                }
                                Item{id:controlPanel;width:controlPanel.height*4/3;height: parent.height*0.23;anchors.horizontalCenter: parent.horizontalCenter;
                                    property bool isAuto:checkSvc.autoAlignPupil;
                                    CusButton {id:autoButton;width: parent.width*0.35;height: parent.height*0.28;buttonColor: backGroundColor; text:controlPanel.isAuto?"Auto":"Manual";borderColor: "black";anchors.horizontalCenter: parent.horizontalCenter; anchors.verticalCenter: parent.verticalCenter;onClicked: IcUiQmlApi.appCtrl.deviceOperation.autoAlignPupil=!IcUiQmlApi.appCtrl.deviceOperation.autoAlignPupil;}
                                    CusButton {id:upButton;enabled:!controlPanel.isAuto;rec.visible: false;anchors.left: parent.Top;height: image.sourceSize.height*root.height/691;imageHightScale:1.0;width: image.sourceSize.width*root.width/1366;anchors.horizontalCenter: parent.horizontalCenter;imageSrc: "qrc:/Pics/capture-svg/arrow_1up.svg";onPressed:{imageHightScale=1.1;checkSvc.moveChinUp();}onReleased:{imageHightScale=1.0;checkSvc.stopMovingChin();}}
                                    CusButton {id:downButton;enabled:!controlPanel.isAuto;rec.visible: false;anchors.bottom: parent.bottom; height: image.sourceSize.height*root.height/691;imageHightScale:1.0;width: image.sourceSize.width*root.width/1366;anchors.horizontalCenter: parent.horizontalCenter;imageSrc: "qrc:/Pics/capture-svg/arrow_2down.svg";onPressed:{imageHightScale=1.1;checkSvc.moveChinDown();}onReleased:{imageHightScale=1.0;checkSvc.stopMovingChin();}}
                                    CusButton {id:leftButton;enabled:!controlPanel.isAuto;rec.visible: false; anchors.right: autoButton.left; anchors.verticalCenter: parent.verticalCenter; imageHightScale:1.0;height: image.sourceSize.height*root.height/691; anchors.rightMargin:(controlPanel.height-autoButton.height-upButton.height*2)/2;width: image.sourceSize.width*root.width/1366;imageSrc: "qrc:/Pics/capture-svg/arrow_3left.svg";onPressed:{imageHightScale=1.1;checkSvc.moveChinLeft();}onReleased:{imageHightScale=1.0;checkSvc.stopMovingChin();}}
                                    CusButton {id:rightButton;enabled:!controlPanel.isAuto;rec.visible: false;anchors.left: autoButton.right;anchors.verticalCenter: parent.verticalCenter;imageHightScale:1.0;height: image.sourceSize.height*root.height/691; anchors.leftMargin:(controlPanel.height-autoButton.height-upButton.height*2)/2;width: image.sourceSize.width*root.width/1366; imageSrc: "qrc:/Pics/capture-svg/arrow_4right.svg";onPressed:{imageHightScale=1.1;checkSvc.moveChinRight();}onReleased:{imageHightScale=1.0;checkSvc.stopMovingChin();}}
                                }
                                Rectangle{id:eyeOptionsGroup; width: parent.width*0.83;height: parent.height*0.25;anchors.horizontalCenter: parent.horizontalCenter; border.color: backGroundBorderColor;color: backGroundColor; radius: width*0.03;
                                    Item{ anchors.fill: parent;anchors.margins: parent.height*0.1;
                                        Column{anchors.fill: parent;spacing: 0.125*height;
                                            Row{width:parent.width;height: parent.height*0.75/3;spacing: width*0.05;
                                                CusCheckBox{}
                                                CusText{text:lt+qsTr("Pupil diameter"); horizontalAlignment: Text.AlignLeft;width: parent.width*0.5;font.pointSize: fontPointSize;}
                                                LineEdit{text:currentCheckResult.resultData.pupilDiameter.toFixed(2);width: parent.width*0.25;textInput.readOnly: true;}
                                            }
                                            Row{id: row;width:parent.width;height: parent.height*0.75/3;spacing: width*0.05;
                                                CusCheckBox{enabled: false;checked:currentProgram.type!==2?currentProgram.params.commonParams.fixationMonitor>0:currentProgram.params.fixationMonitor>0;}
                                                CusText{text:lt+qsTr("Eye move alarm"); horizontalAlignment: Text.AlignLeft;width: parent.width*0.50;font.pointSize: fontPointSize;}
                                                Image {source: "qrc:/Pics/capture-svg/btn_alarm.svg";height:parent.height*0.6; anchors.verticalCenter: parent.verticalCenter;width: height; }
                                            }
                                            Row{width:parent.width;height: parent.height*0.75/3;spacing: width*0.05;
                                                CusCheckBox{id:deviationCheckBox;checked:true;}
                                                CusText{text:lt+qsTr("Fixation deviation"); horizontalAlignment: Text.AlignLeft;width: parent.width*0.25;font.pointSize: fontPointSize;}
                                            }
                                        }
                                    }
                                }
                                Item{
                                    width: parent.width*0.83;height: parent.height*0.08;anchors.horizontalCenter: parent.horizontalCenter;
                                    FixationDeviation{ visible:deviationCheckBox.checked;anchors.horizontalCenter: parent.horizontalCenter;dots:if(currentCheckResult!==null) currentCheckResult.resultData.fixationDeviation;}
                                }
                            }
                        }
//                        Rectangle{ id: realTimeDBRec;visible:false;anchors.fill: parent;anchors.margins: parent.height*0.02;color:"grey";z:1;
//                            CusButton{anchors.right: parent.right;  anchors.top: parent.top;anchors.topMargin: 0; anchors.rightMargin: 0;z:1; imageHightScale: 1;height:image.sourceSize.height; width:image.sourceSize.width; rec.visible: false;imageSrc: "qrc:/Pics/base-svg/window_4close_1normal.svg";hoverImageSrc:"qrc:/Pics/base-svg/window_4close_2hover.svg";pressImageSrc: "qrc:/Pics/base-svg/window_4close_3press.svg";onClicked: parent.visible=false;}
//                            Grid{anchors.fill: parent;rows: 3;columns: 2;rowSpacing:(height-width/2*3)/2;columnSpacing: 0;
//                                Repeater{model:[0,1,2,3,4,5]
//                                    Image{
//                                       property string picSource: "/realTimeEyePosPic/"+modelData+".bmp";
//                                       height: width; fillMode: Image.PreserveAspectCrop;width: parent.width/2;smooth: false;cache: false;        //to refresh image
//                                       Component.onCompleted:
//                                       {
//                                           root.realTimePicRefresh.connect(function(count)
//                                           {
//                                               source="";source="file:///" + applicationDirPath + picSource;
//                                               visible=modelData<count;
//                                           })
//                                           root.refresh.connect(function(){visible=false;});
//                                       }
//                                    }
//                                }
//                            }
//                        }
                        Rectangle{id: realTimeDBRec;visible:false;anchors.fill: parent;anchors.margins: parent.height*0.02;color:"grey";z:1;
                            CusButton{opacity:0.5;anchors.right: parent.right;  anchors.top: parent.top;anchors.topMargin: 0; anchors.rightMargin: 0;z:1; imageHightScale: 1;height:image.sourceSize.height; width:image.sourceSize.width; rec.visible: false;imageSrc: "qrc:/Pics/base-svg/window_4close_1normal.svg";hoverImageSrc:"qrc:/Pics/base-svg/window_4close_2hover.svg";pressImageSrc: "qrc:/Pics/base-svg/window_4close_3press.svg";onClicked: parent.visible=false;}
                            Grid{anchors.fill: parent;rows: 3;columns: 2;rowSpacing:(height-width/2*3)/2;columnSpacing: 0;
                                Repeater{id:repeater;model:listModel;
                                    property ListModel listModel:ListModel{}
                                    Item{height: width; width: parent.width/2;
                                        Image{
                                           property string picSource: "/realTimeEyePosPic/"+index+".bmp";
                                           anchors.fill: parent;
                                           fillMode: Image.PreserveAspectCrop;smooth: false;cache: false;        //to refresh image
                                           source: "file:///" + applicationDirPath + picSource;
                                        }
                                        Rectangle
                                        {
                                            opacity: 0.8;radius: 2;color: "grey";width: 16;height: 12;anchors.top: parent.top; anchors.topMargin: parent.height*0.05; anchors.left: parent.left; anchors.leftMargin:parent.width*0.05;
                                            CusText{ anchors.fill: parent;text:index;color: "white"; }
                                        }
                                        Rectangle
                                        {
                                            opacity: 0.8;radius: 2;color: "grey";width: 34;height: 12;anchors.bottom: parent.bottom; anchors.bottomMargin: parent.height*0.05; anchors.right: parent.right; anchors.rightMargin:parent.width*0.05;
                                            CusText{ visible:currentProgram.type===0;anchors.fill: parent;text:currentCheckResult.resultData.realTimeDB[checkDisplay.clickedDotIndex][index]+"DB";color: "white"; }
                                        }
                                    }
                                }
                                Component.onCompleted:
                                {
                                    root.refresh.connect(function(){visible=false;parent.color="white"});
                                    root.realTimePicRefresh.connect(
                                    function(count){
                                        visible=true;parent.color="grey"
                                        repeater.listModel.clear();
                                        for(var i=0;i<count;i++)
                                        {
                                           repeater.listModel.append({index:i});
                                        }
                                    })
                                }
                            }
                        }

                    }
                    Rectangle{width: parent.width*0.5;height: parent.height;color:backGroundColorCheckPanel;
                        CusText{
                            id:os_od;font.pointSize: fontPointSize*2;
                            property int value: 0;
                            text:value===0?lt+qsTr("Left eye"):lt+qsTr("Right eye"); z: 1; anchors.top: parent.top; anchors.topMargin: 0.05*parent.height; anchors.left: parent.left; anchors.leftMargin: 0.05*parent.width;height: parent.height*0.05;}
                        CheckDisplay{id:checkDisplay; currentProgram:root.currentProgram;currentCheckResult:root.currentCheckResult;
                            onClickedDotIndexChanged: {
                                if(currentCheckResult==null) return;
                                realTimeDBRec.visible=true;
                                var count=currentCheckResult.drawRealTimeEyePosPic(clickedDotIndex);
                                realTimePicRefresh(count);
                            }
                        }
                        Row{
                            width: parent.width;height: 20; layoutDirection: Qt.LeftToRight;anchors.bottom: parent.bottom;
                            Button{text:"video on";width: parent.width/8;height: parent.height; onClicked: IcUiQmlApi.appCtrl.checkSvc.turnOnVideo();}
                            Button{text:"video off";width: parent.width/8;height: parent.height; onClicked: IcUiQmlApi.appCtrl.checkSvc.turnOffVideo();}
                            Button{text:"connect";width: parent.width/8;height: parent.height;onClicked:IcUiQmlApi.appCtrl.checkSvc.connectDev();}
                            Button{text:"disconnect";width: parent.width/8;height: parent.height;onClicked:IcUiQmlApi.appCtrl.checkSvc.disconnectDev();}
                        }

                    }
                }
            }

        }
        Rectangle{id:bottomRibbon;width: parent.width;height: parent.height*1/15;color: "#333e44";
            Row{anchors.fill: parent;
                Item{ height: parent.height;width:parent.width*0.25;
                    Item{anchors.fill: parent; anchors.margins:parent.height*0.15;
                        CusButton
                        {
                            enabled: IcUiQmlApi.appCtrl.checkSvc.checkState>=3;
                            text:lt+qsTr("Back");onClicked:
                            {
                                if(currentCheckResult!==null)
                                {
                                    if(currentCheckResult.type!==2)
                                        IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::StaticCheckResultVm",currentCheckResult);

                                    else
                                        IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::DynamicCheckResultVm",currentCheckResult);
                                    currentCheckResult=null;
                                }
                                root.changePage("patientManagement",null);
                            }
                        }
                    }
                }
                Item{height: parent.height;width:parent.width*0.25;
                    Item{anchors.fill: parent;anchors.margins:parent.height*0.15;
                        Flow{height: parent.height;spacing: height*0.8;anchors.horizontalCenter: parent.horizontalCenter;
                            CusButton{enabled:IcUiQmlApi.appCtrl.checkSvc.checkState>=3;text:lt+qsTr("Select program");width:IcUiQmlApi.appCtrl.settings.isRuntimeLangEng?height*4:height*2.5;onClicked: chooseProgram.open();}
                            CusButton{id:paramsSetting;text:lt+qsTr("Params setting");enabled:(currentProgram!==null&&IcUiQmlApi.appCtrl.checkSvc.checkState>=3);width:IcUiQmlApi.appCtrl.settings.isRuntimeLangEng?height*4:height*2.5;onClicked:if(currentProgram.type!==2){ staticParamsSetting.open();} else  { dynamicParamsSetting.open();}}
                        }
                    }
                }
                Item{ height: parent.height;width:parent.width*0.50;
                    Item{id: item1; anchors.fill: parent;anchors.margins:parent.height*0.15;
                        Flow{
                            id:checkControl
                            height: parent.height;spacing: height*0.8;anchors.horizontalCenter: parent.horizontalCenter;
                            CusButton{
//                                text:"开始测试";
                                enabled: IcUiQmlApi.appCtrl.checkSvc.devReady;
                                property int checkState: IcUiQmlApi.appCtrl.checkSvc.checkState;
                                text:{if(checkState>2) return lt+qsTr("Start");if(checkState===2) return lt+qsTr("Resume");if(checkState===0||checkState===1) return lt+qsTr("Pause")}
                                onClicked:{
//                                    if(currentCheckResult!=null)
//                                    {
//                                        if(currentCheckResult.type!==2)
//                                            IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::StaticCheckResultVm",currentCheckResult);

//                                        else
//                                            IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::DynamicCheckResultVm",currentCheckResult);
//                                        currentCheckResult=null;
//                                    }

//                                    if(currentProgram.type===0)
//                                    {
//                                        currentCheckResult=IcUiQmlApi.appCtrl.objMgr.attachObj("Perimeter::StaticCheckResultVm", false,[1]);
//                                        console.log(currentCheckResult.resultData.testTimespan);
//                                    }
//                                    else if(currentProgram.type===1)
//                                    {
//                                        currentCheckResult=IcUiQmlApi.appCtrl.objMgr.attachObj("Perimeter::StaticCheckResultVm", false,[200]);
//                                    }
//                                    else
//                                    {
//                                        currentCheckResult=IcUiQmlApi.appCtrl.objMgr.attachObj("Perimeter::DynamicCheckResultVm", false,[300]);
//                                    }

                                    if(checkState>2)
                                    {
                                        if(currentCheckResult!=null)
                                        {
                                            if(currentProgram.type!==2)
                                                IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::StaticCheckResultVm",currentCheckResult);
                                            else
                                                IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::DynamicCheckResultVm",currentCheckResult);
                                            currentCheckResult=null;
                                        }

                                        if(currentProgram.type!==2){
                                            currentCheckResult=IcUiQmlApi.appCtrl.objMgr.attachObj("Perimeter::StaticCheckResultVm", false,[]);
                                        }
                                        else
                                        {
                                            currentCheckResult=IcUiQmlApi.appCtrl.objMgr.attachObj("Perimeter::DynamicCheckResultVm", false,[]);
                                        }
                                        currentCheckResult.OS_OD=os_od.value;
                                        currentCheckResult.params=currentProgram.params;
                                        IcUiQmlApi.appCtrl.checkSvc.program=currentProgram;
                                        IcUiQmlApi.appCtrl.checkSvc.patient=currentPatient;
                                        IcUiQmlApi.appCtrl.checkSvc.checkResult=currentCheckResult;
                                        IcUiQmlApi.appCtrl.checkSvc.start();

                                    }
                                    else if(checkState===2)
                                    {
                                        IcUiQmlApi.appCtrl.checkSvc.resume();
                                    }
                                    else if(checkState===1||checkState===0)
                                    {
                                        IcUiQmlApi.appCtrl.checkSvc.pause();
                                    }

                                }
//                                Component.onCompleted: {
//                                    IcUiQmlApi.appCtrl.checkSvc.checkResultChanged.connect(function(){console.log("checkResultChanged.")});
//                                }
                            }
                            CusButton{
                                text:lt+qsTr("Stop");enabled: IcUiQmlApi.appCtrl.checkSvc.checkState<=2;
                                onClicked:
                                {
                                    IcUiQmlApi.appCtrl.checkSvc.stop();
                                    if(currentCheckResult!=null)
                                    {
                                        if(currentProgram.type!==2)
                                            IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::StaticCheckResultVm",currentCheckResult);
                                        else
                                            IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::DynamicCheckResultVm",currentCheckResult);
                                        currentCheckResult=null;
                                    }
                                }}
                            CusButton{
                                text:lt+qsTr("Switch eye");onClicked:os_od.value=(os_od.value+1)%2;
                                enabled:IcUiQmlApi.appCtrl.checkSvc.checkState>=3;
                            }
                        }

                        CusComboBoxButton{
                            height: parent.height; anchors.right: parent.right; anchors.rightMargin: 0;width: height*3.5;
//                            enabled: currentCheckResult!==null;
                            enabled: currentCheckResult!==null&&IcUiQmlApi.appCtrl.checkSvc.checkState===4;
                            property var listModel:ListModel {}
                            property var reportNames: [[lt+qsTr("Single"),lt+qsTr("Three in one"),lt+qsTr("Overview"),lt+qsTr("Three in one"),lt+qsTr("Threshold")],[lt+qsTr("Screening")],[lt+qsTr("Dynamic"),lt+qsTr("Dyanmic data")]]
                            comboBox.model: listModel;popDirectionDown: false;complexType: true;
                            button.text: lt+qsTr("Analysis");
                            button.onClicked:
                            {
                                console.log(currentCheckResult.id);
                                console.log(currentProgram.id);
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
                                changePage("analysis",{pageFrom:"check",report:report,analysisVm:analysisVm,program:currentProgram,checkResult:currentCheckResult,analysisResult:analysisResult});
                            }

                            Component.onCompleted: {
                                root.currentProgramChanged.connect(function()
                                {
                                    listModel.clear();
                                    var report=currentProgram.report;
                                    report.forEach(function(item){
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
}
