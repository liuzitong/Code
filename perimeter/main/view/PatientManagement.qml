﻿import QtQuick 2.6
import QtQuick.Controls 2.0
import QtQuick.Window 2.3
import QtQml 2.2
import QtQuick.Controls.Styles 1.4
import perimeter.main.view.Controls 1.0
import perimeter.main.view.Utils 1.0
import qxpack.indcom.ui_qml_base 1.0

Item{
    id:root;
//    property string language:IcUiQmlApi.appCtrl.settings.language
    property bool doubleName:isEng;
    property string backGroundColor:"#dcdee0"
    property string backGroundBorderColor:"#bdc0c6"
    property var currentPatient:null;
    property var modifyPatient: null;
    property var lastProgram:null;
//    property var lastCheckResult: null;
    property int pageSize: 10;
    property int fontPointSize: CommonSettings.fontPointSize;
    property string pageFrom: "";
    property bool creatingNewPatient:false;
    signal queryStarted;
    signal changePage(var pageName,var params);
    width: 1440
    height: 700
    anchors.fill:parent;

//    onLanguageChanged: console.log("haha");
//    onDoubleNameChanged: console.log("dogdog");

    Timer{
        id: timer
        function setTimeout(cb, delayTime) {
            timer.interval = delayTime;
            timer.repeat = false;
            timer.triggered.connect(cb);
            timer.triggered.connect(function release () {
                timer.triggered.disconnect(cb); // This is important
                timer.triggered.disconnect(release); // This is important as well
            });
            timer.start();
        }
    }



    Component.onCompleted:
    {
//        patientInfoListView.patientListModelVm=IcUiQmlApi.appCtrl.objMgr.attachObj("Perimeter::PatientListModelVm", true);
        timer.setTimeout(function() {query.startQuery();}, 500);
//        currentPatient=IcUiQmlApi.appCtrl.objMgr.attachObj("Perimeter::PatientVm", false,[1]);
    }


    Item{id:content;width: parent.width;height: parent.height*14/15;anchors.top: parent.top;
        property int layoutGroupMargin: 0.03*height
        CusCalendar{id: calendar;width: parent.width;height: parent.height;}
        Row{
            anchors.fill: parent;anchors.margins: content.layoutGroupMargin;spacing:content.layoutGroupMargin
            Rectangle{width: (parent.width-content.layoutGroupMargin)*0.6;height: parent.height;
                border.color: backGroundBorderColor;color: backGroundColor;radius: commonRadius
                Item{anchors.fill: parent;anchors.margins: content.layoutGroupMargin
                    Column{id:patientInfo;anchors.fill: parent;spacing: rowHight
                        property int rowHight: height*0.06
                        Row{
                            id:dateSelection; width: parent.width;opacity: 1;height: patientInfo.rowHight;spacing: height*0.4;
                            CusText{text: lt+qsTr("Check date");horizontalAlignment: Text.AlignLeft;width: height*2.5;font.pointSize: fontPointSize;}
                            LineEdit{/*property string name: "dateFrom";*/id:dateFrom;readOnly:true;enabled: false;radius: height/6;width: height*3.1;}
                            CusButton{id:dateFromButton;text:lt+qsTr("Select");width:height*2;onClicked:{calendar.inputObj=dateFrom;calendar.open();}}
                            CusText{text:lt+qsTr("To");width: height*0.6;font.pointSize: fontPointSize;}
                            LineEdit{/*property string name: "dateTo";*/id:dateTo;readOnly:true;enabled: false;radius: height/6;width: height*3.1;}
                            CusButton{id:dateToButton;text:lt+qsTr("Select");width:height*2;
                                onClicked:{
                                    if(dateFrom.text!==""&&dateTo.text=="")
                                        dateTo.text=dateFrom.text;
                                    calendar.inputObj=dateTo;calendar.open();
                                }}
                        }
                        Row{
                            id:query; width: parent.width; height:patientInfo.rowHight;spacing: height*0.5
                            CusButton{id:queryAllPatients;text:lt+qsTr("Show all");onClicked: {patientInfoListView.patientListModelVm.getPatientListByTimeSpan("","");queryStarted();}}
                            CusComboBox{
                                id:queryStrategy;height: parent.height;width: parent.height*4.5;
                                borderColor: backGroundBorderColor;font.family:"Microsoft YaHei";
                                imageSrc: "qrc:/Pics/base-svg/btn_drop_down.svg";
//                                model: ListModel {ListElement { name: lt+qsTr("Query by time") } ListElement { name: lt+qsTr("Patient ID") } ListElement { name: lt+qsTr("Name") }ListElement { name: lt+qsTr("Sex") }ListElement { name: lt+qsTr("Birth date") }}
                                model:  [lt+qsTr("Query by time"),lt+qsTr("Patient ID"),lt+qsTr("Name"),lt+qsTr("Sex"),lt+qsTr("Birth date")]
                                onCurrentIndexChanged: {
                                    patientID.visible=false;chineseName.visible=false;englishNameGroup.visible=false;sex.visible=false;birthDateGroup.visible=false;dateSelection.opacity=0;
                                    switch(currentIndex)
                                    {
                                    case 0:dateSelection.opacity=1;break;
                                    case 1:patientID.visible=true;dateSelection.opacity=0;break;
                                    case 2:if(!doubleName) {chineseName.visible=true;} else{englishNameGroup.visible=true;} dateSelection.opacity=1;break;
                                    case 3:sex.visible=true;dateSelection.opacity=1;break;
                                    case 4:birthDateGroup.visible=true;dateSelection.opacity=0;break;
                                    }
                                }
                                Component.onCompleted: root.doubleNameChanged.connect(currentIndexChanged);
                            }

                            LineEdit{id:patientID;radius:height/6;width: height*4}
                            LineEdit{id:chineseName;radius:height/6;width: height*4;onEnterPressed:{query.startQuery();}}
                            Flow{
                                id:englishNameGroup;height: parent.height;spacing: height*0.3;
                                CusText{text:lt+qsTr("First name")+":";width:height*2.0;font.pointSize: fontPointSize;}
                                LineEdit{id:firstName;radius:height/6;width: height*4}
                                CusText{text:lt+qsTr("Last name")+":";width:height*2.0;font.pointSize: fontPointSize;}
                                LineEdit{id:lastName;radius:height/6;width: height*4}
                            }
                            CusComboBox{
                                id:sex;height: parent.height;width: parent.height*3;
                                borderColor: backGroundBorderColor;font.family:"Microsoft YaHei";
                                imageSrc: "qrc:/Pics/base-svg/btn_drop_down.svg";
                                model: [lt+qsTr("Male"),lt+qsTr("Female"),lt+qsTr("Others")]
//                                model: ListModel {ListElement { text: lt+qsTr("Male") } ListElement { text: lt+qsTr("Female") } ListElement { text: lt+qsTr("Others")} }
                            }
                            Flow{
                                id:birthDateGroup;visible: false;height: parent.height;spacing: height*0.4;
                                LineEdit{id:birthDate;height: parent.height;radius:height/6;width: height*4;visible: true;}
                                CusButton{text:lt+qsTr("Select");width:height*2;onClicked:{calendar.inputObj=birthDate;calendar.open();}}
                            }
                            CusButton{
                                height: parent.height;width: height;imageSrc:"qrc:/Pics/base-svg/btn_find.svg"
                                onClicked:query.startQuery();
                            }

                            function startQuery()
                            {
                                switch (queryStrategy.currentIndex)
                                {
                                case 0:patientInfoListView.patientListModelVm.getPatientListByTimeSpan(dateFrom.text,dateTo.text);break;
                                case 1:
                                    patientInfoListView.patientListModelVm.getPatientListByPatientId(patientID.text.replace(/^\s*|\s*$/g,""));break;
                                case 2:
                                    console.log(chineseName.text);
                                    if(!doubleName) patientInfoListView.patientListModelVm.getPatientListByName(chineseName.text.replace(/^\s*|\s*$/g,""),dateFrom.text,dateTo.text);
                                    else  patientInfoListView.patientListModelVm.getPatientListByName(firstName.text.replace(/^\s*|\s*$/g,"")+" "+lastName.text.replace(/^\s*|\s*$/g,""),dateFrom.text,dateTo.text);
                                    break;
                                case 3:patientInfoListView.patientListModelVm.getPatientListBySex(sex.currentIndex,dateFrom.text,dateTo.text);break;
                                case 4:patientInfoListView.patientListModelVm.getPatientListByBirthDate(birthDate.text);
                                }
                                queryStarted();
                            }
                        }
                        Item{
                            width: parent.width;height: parent.height-6*patientInfo.rowHight
                            Column{
                                id:patientInfoCol;
                                anchors.fill:parent;spacing: -1;
                                Row{
                                    id:header
                                    width: parent.width; height:patientInfo.rowHight;spacing: -1;z:1;
//                                    property bool isAllSelected: false;
//                                    CusButton{
//                                        width: parent.width*1/10;height: parent.height;buttonColor: "#D2D3D5"; borderColor: "#656566";radius:0;/*imageSrc:"qrc:/Pics/base-svg/btn_select_normal.svg"*/
//                                        imageSrc:header.isAllSelected?"qrc:/Pics/base-svg/btn_select_click.svg":"qrc:/Pics/base-svg/btn_select_normal.svg";
//                                        onClicked: {header.isAllSelected=!header.isAllSelected;}
//                                    }
                                    Rectangle{width: parent.width*1/10;height: parent.height;color: "#D2D3D5"; border.color: "#656566";}
                                    Rectangle{width: parent.width*2.5/10+1;height: parent.height;color: "#D2D3D5"; border.color: "#656566";CusText{anchors.fill: parent;text:lt+qsTr("Patient ID");font.pointSize: fontPointSize;}}
                                    Rectangle{width: parent.width*2.5/10+1;height: parent.height;color: "#D2D3D5"; border.color: "#656566";CusText{anchors.fill: parent;text:lt+qsTr("Name");font.pointSize: fontPointSize;}}
                                    Rectangle{width: parent.width*1/10+1;height: parent.height;color: "#D2D3D5"; border.color: "#656566";CusText{anchors.fill: parent;text:lt+qsTr("Sex");font.pointSize: fontPointSize;}}
                                    Rectangle{width: parent.width*2/10+1;height: parent.height;color: "#D2D3D5"; border.color: "#656566";CusText{anchors.fill: parent;text:lt+qsTr("Birth date");font.pointSize: fontPointSize;}}
                                    Rectangle{width: parent.width*1/10;height: parent.height;color: "#D2D3D5"; border.color: "#656566";CusText{anchors.fill: parent;text:lt+qsTr("Load");font.pointSize: fontPointSize;}}
                                }
                                ListView{
                                    id:patientInfoListView
//                                    property int seletedPatientLength:0;
//                                    property var seletedPatient:[];
                                    property var patientListModelVm:null;
                                    width: parent.width;height:patientInfoCol.height-patientInfo.rowHight+1;/* cacheBuffer: 0;*/spacing: -1;clip:true;snapMode: ListView.SnapPosition;interactive: false;
                                    delegate: patientInfoDelegate
                                    model:patientListModelVm;
                                    property var sex:[lt+qsTr("Male"),lt+qsTr("Female"),lt+qsTr("Others")];
                                    Component.onCompleted: {
                                        patientListModelVm=IcUiQmlApi.appCtrl.objMgr.attachObj("Perimeter::PatientListModelVm", true);
//                                        IcUiQmlApi.appCtrl.settings.langTriggerChanged.connect(patientListModelVmChanged);
//                                        patientListModelVm.getPatientListByTimeSpan(dateFrom.text,dateTo.text);
                                    }
                                    Component.onDestruction: {IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::PatientListModelVm",patientListModelVm);}
                                    Component{
                                        id:patientInfoDelegate
                                        Item{
                                            id:patientRowItem
                                            width: patientInfoListView.width;height: (patientInfoCol.height-patientInfo.rowHight)/pageSize+1;
                                            MouseArea{
                                                width: parent.width*0.8;anchors.left: parent.left;anchors.leftMargin: parent.width*0.1;height: parent.height;
                                                onClicked:
                                                {
                                                    var firstName ="";var lastName="";
                                                    if(currentPatient!==null) IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::PatientVm", currentPatient);
                                                    currentPatient=IcUiQmlApi.appCtrl.objMgr.attachObj("Perimeter::PatientVm", false,[model.Id]);
                                                    if(lastProgram!==null)
                                                    {
                                                        if(lastProgram.type!==2)
                                                        {IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::StaticProgramVM", lastProgram);}
                                                        else{IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::DynamicProgramVM", lastProgram);}
                                                    }
                                                    var lastCheckResult=currentPatient.getLastCheckResult();
                                                    if(lastCheckResult!==null)
                                                    {
                                                        if(lastCheckResult.type!==2)
                                                            lastProgram=IcUiQmlApi.appCtrl.objMgr.attachObj("Perimeter::StaticProgramVM", false,[lastCheckResult.program_id]);
                                                        else
                                                            lastProgram=IcUiQmlApi.appCtrl.objMgr.attachObj("Perimeter::DynamicProgramVM", false,[lastCheckResult.program_id]);

                                                        lastProgram.params=lastCheckResult.params;
                                                        if(lastCheckResult.type!==2)
                                                            IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::StaticCheckResultVm", lastCheckResult);
                                                        else
                                                            IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::DynamicCheckResultVm", lastCheckResult);
                                                        lastCheckResult=null;
                                                    }
                                                }
                                            }
                                            Row{
                                                id:patientInfoRow;anchors.fill: parent;spacing: -1;

//                                                property alias isAllSelected: header.isAllSelected;
//                                                onIsAllSelectedChanged: isSelected=isAllSelected;
                                                CusButton{
                                                    id:selectedPatient;
                                                    width: parent.width*1/10;height: parent.height;buttonColor: "white"; borderColor: backGroundBorderColor;radius:0;isAnime:false;imageSrc:"qrc:/Pics/base-svg/btn_select_normal.svg"
                                                    property bool isSelected:model.isSelected;
//                                                    property bool isAllSelected: header.isAllSelected;
//                                                    enabled: currentPatient==null||(currentPatient.id!=model.Id);
                                                    onClicked:isSelected=!isSelected;
                                                    onIsSelectedChanged:
                                                    {
                                                        if(isSelected)
                                                        {
                                                            imageSrc="qrc:/Pics/base-svg/btn_select_click.svg";/*patientInfoListView.seletedPatient.push(model.Id)*/model.isSelected=true;
                                                        }
                                                        else{
                                                             imageSrc="qrc:/Pics/base-svg/btn_select_normal.svg";/*patientInfoListView.seletedPatient.pop(model.Id);*/model.isSelected=false;
                                                        }
//                                                        patientInfoListView.seletedPatientLength=patientInfoListView.seletedPatient.length;
//                                                        console.log(patientInfoListView.seletedPatient);
                                                    }

//                                                    onIsAllSelectedChanged:
//                                                    {
//                                                        isSelected=isAllSelected;
//                                                    }

//                                                    onIsSelectedChanged:
//                                                    {
//                                                        if(!patientInfoRow.isSelected){patientInfoRow.isSelected=true;imageSrc="qrc:/Pics/base-svg/btn_select_click.svg";patientInfoListView.seletedPatient.push(model.Id);}
//                                                        else{patientInfoRow.isSelected=false;imageSrc="qrc:/Pics/base-svg/btn_select_normal.svg";patientInfoListView.seletedPatient.pop(model.Id);}
//                                                        patientInfoListView.seletedPatientLength=patientInfoListView.seletedPatient.length;
//                                                    }

                                                }
                                                Rectangle{width: parent.width*2.5/10+1;height: parent.height;color: currentPatient==null?"white":currentPatient.id==model.Id?"cyan":"white"; border.color: backGroundBorderColor;CusText{anchors.fill: parent;font.pointSize: fontPointSize;text:model.patientId/*+" "+model.Id*/}}
                                                Rectangle{width: parent.width*2.5/10+1;height: parent.height;color: currentPatient==null?"white":currentPatient.id==model.Id?"cyan":"white"; border.color: backGroundBorderColor;CusText{anchors.fill: parent;font.pointSize: fontPointSize;text:model.name/*+" "+model.Id+" "+model.lastUpdate*/}}
                                                Rectangle{width: parent.width*1/10+1;height: parent.height;color: currentPatient==null?"white":currentPatient.id==model.Id?"cyan":"white"; border.color: backGroundBorderColor;CusText{anchors.fill: parent;font.pointSize: fontPointSize;text:patientInfoListView.sex[model.sex]}}
                                                Rectangle{width: parent.width*2/10+1;height: parent.height;color: currentPatient==null?"white":currentPatient.id==model.Id?"cyan":"white"; border.color: backGroundBorderColor;CusText{anchors.fill: parent;font.pointSize: fontPointSize;text:model.birthDate}}
                                                Rectangle
                                                {
                                                    width: parent.width*1/10;height: parent.height;color: modifyPatient==null?"white":modifyPatient.id==model.Id?"cyan":"white";border.color: backGroundBorderColor;
                                                    Image {
                                                        source: "qrc:/Pics/base-svg/btn_analysis_enter.svg";
                                                        height: parent.height*0.6
                                                        anchors.horizontalCenter: parent.horizontalCenter
                                                        anchors.verticalCenter: parent.verticalCenter;
                                                        width: sourceSize.width*(height/sourceSize.height);
                                                    }
                                                    MouseArea{
                                                        anchors.fill: parent;
                                                        onClicked:
                                                        {
                                                            var firstName ="";var lastName="";
                                                            if(modifyPatient!==null) IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::PatientVm", modifyPatient);
                                                            modifyPatient=IcUiQmlApi.appCtrl.objMgr.attachObj("Perimeter::PatientVm", false,[model.Id]);
                                                            if(modifyPatient.name.indexOf(" ")>-1){ firstName =modifyPatient.name.split(" ")[0]; lastName=modifyPatient.name.split(" ")[1];};
                                                            newPatientId.text=modifyPatient.patientId;newChineseName.text=modifyPatient.name;
                                                            genderSelect.selectGender(modifyPatient.sex);newBirthDate.text=modifyPatient.birthDate;
                                                            newEnglishFirstName.text=firstName;newEnglishLastName.text=lastName
                                                            patientSaveButton.enabled=false;
                                                            rx1_r.text=modifyPatient.rx.rx1_r.toFixed(2);
                                                            rx2_r.text=modifyPatient.rx.rx2_r.toFixed(2);
                                                            rx3_r.text=modifyPatient.rx.rx3_r.toFixed(2);
                                                            visual_r.text=modifyPatient.rx.visual_r.toFixed(2);
                                                            rx1_l.text=modifyPatient.rx.rx1_l.toFixed(2);
                                                            rx2_l.text=modifyPatient.rx.rx2_l.toFixed(2);
                                                            rx3_l.text=modifyPatient.rx.rx3_l.toFixed(2);
                                                            visual_l.text=modifyPatient.rx.visual_l.toFixed(2);
                                                            if(CommonSettings.deletePermission) patientReviseButton.enabled=true;

                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        Item{
                            id:pageControl;width: parent.width;height:patientInfo.rowHight;
                            Flow{
                                id:pageIndex;
                                property int currentPage: 1;property int totalPage:1;property int totalRecordCount: 0;anchors.verticalCenter: parent.verticalCenter;anchors.left: parent.left;height:parent.height;
                                CusText{id:currentPageNumberText;text:lt+qsTr("The")+" "+pageIndex.currentPage+"/"+pageIndex.totalPage+" "+qsTr("Page")+","+qsTr("Total")+" "+pageIndex.totalRecordCount+" "+qsTr("Records"); horizontalAlignment: Text.AlignLeft;height: parent.height*0.8;font.pointSize: fontPointSize;}
                            }
                            Flow{
                                anchors.right: parent.right;height:parent.height*1.0;anchors.verticalCenter: parent.verticalCenter;spacing:0.5*height;
                                CusButton{
                                    id:startPage;
                                    imageHightScale: 1;width: height; borderWidth: 0;imageSrc: "qrc:/Pics/base-svg/page_1head_1normal.svg";hoverImageSrc:"qrc:/Pics/base-svg/page_1head_2hover.svg";pressImageSrc: "qrc:/Pics/base-svg/page_1head_3press.svg";
                                    onClicked: {
                                        pageIndex.currentPage=1;
                                        patientInfoListView.positionViewAtIndex(0,ListView.Beginning);
                                    }
                                }
                                CusButton{
                                    id:previousPage;
                                    imageHightScale: 1;width: height; borderWidth: 0;imageSrc: "qrc:/Pics/base-svg/page_2previous_1normal.svg";hoverImageSrc:"qrc:/Pics/base-svg/page_2previous_2hover.svg";pressImageSrc: "qrc:/Pics/base-svg/page_2previous_3press.svg";
                                    onClicked: {
                                        if(pageIndex.currentPage>1)
                                        {
                                            pageIndex.currentPage-=1;
                                            patientInfoListView.positionViewAtIndex((pageIndex.currentPage-1)*pageSize,ListView.Beginning);
                                        }
                                    }
                                }
                                LineEdit{id:currentPage;width: height*2;text:pageIndex.currentPage;radius: height*0.2;horizontalAlignment: Text.AlignHCenter}
                                CusButton{
                                    width: height; text:"Goto";fontSize: height*0.25;onEntered: {borderColor=hoverBorderColor;}onExited: {borderColor=commonBorderColor;}
                                    onClicked:
                                    {
                                        borderColor=pressBorderColor;
                                        pageIndex.currentPage=currentPage.text;
                                        patientInfoListView.positionViewAtIndex((pageIndex.currentPage-1)*pageSize,ListView.Beginning);
                                    }
                                }
                                CusButton{
                                    id:nextPage;
                                    imageHightScale: 1;width: height; borderWidth: 0;imageSrc: "qrc:/Pics/base-svg/page_4next_1normal.svg";hoverImageSrc:"qrc:/Pics/base-svg/page_4next_2hover.svg";pressImageSrc: "qrc:/Pics/base-svg/page_4next_3press.svg";
                                    onClicked: {
                                        if(pageIndex.currentPage<pageIndex.totalPage)
                                        {
                                            pageIndex.currentPage+=1;
                                            patientInfoListView.positionViewAtIndex((pageIndex.currentPage-1)*pageSize,ListView.Beginning);
                                        }
                                    }
                                }
                                CusButton{id:endPage;
                                    imageHightScale: 1;width: height; borderWidth: 0;imageSrc: "qrc:/Pics/base-svg/page_5end_1normal.svg";hoverImageSrc:"qrc:/Pics/base-svg/page_5end_2hover.svg";pressImageSrc: "qrc:/Pics/base-svg/page_5end_3press.svg";
                                    onClicked: {
                                        pageIndex.currentPage=pageIndex.totalPage;
                                        patientInfoListView.positionViewAtIndex((pageIndex.currentPage-1)*pageSize,ListView.Beginning);
                                    }
                                }
                            }
                            Component.onCompleted: {queryStarted.connect(refresh)}
                            function refresh(){
                                pageIndex.currentPage=1;
                                var rowCount=patientInfoListView.model.rowCount();
                                console.log("model count:"+rowCount);
                                pageIndex.totalPage=rowCount/pageSize;
                                if(rowCount%pageSize!==0) pageIndex.totalPage+=1;
                                pageIndex.totalRecordCount=rowCount;
                            }
                        }
                    }
                }
            }
            Column{
                width: (parent.width-content.layoutGroupMargin)*0.4; height: parent.height;spacing:content.layoutGroupMargin;
                Rectangle{
                    width: parent.width;height: (parent.height-content.layoutGroupMargin)*0.6;border.color: backGroundBorderColor;color: backGroundColor;radius: commonRadius;
                    Item{
                        anchors.fill: parent;anchors.margins: content.layoutGroupMargin;
                        Column{
                            id:newPatient;anchors.fill: parent;spacing: patientInfo.rowHight
                            Component.onCompleted: {
                                showControl();
                                onDoubleNameChanged.connect(showControl)
                            }

                            function showControl()
                            {
//                                console.log(doubleName);
                                if(!doubleName) {newChineseNameRow.visible=true;newEnglishFirstNameRow.visible=false;newEnglishLastNameRow.visible=false;}
                                else {newChineseNameRow.visible=false;newEnglishFirstNameRow.visible=true;newEnglishLastNameRow.visible=true;}
                            }

                            Row{
                                width: parent.width;height:patientInfo.rowHight;spacing: parent.width*0.02
                                CusText{text:"*"+lt+qsTr("Patient ID")+" "; horizontalAlignment: Text.AlignRight ;width:parent.width*0.20;font.pointSize: fontPointSize;}
                                LineEdit{id:newPatientId;width: parent.width*0.6;}
                                CusButton{height: parent.height;width: height;imageSrc:"qrc:/Pics/base-svg/btn_find.svg";onClicked:{patientInfoListView.patientListModelVm.getPatientListByPatientId(newPatientId.text);}}
                            }
                            Row{
                                id:newChineseNameRow;visible:false;width: parent.width;height:patientInfo.rowHight;spacing: parent.width*0.02
                                CusText{text:"*"+lt+qsTr("Name")+" "; horizontalAlignment: Text.AlignRight ;width:parent.width*0.20;font.pointSize: fontPointSize;}
                                LineEdit{id:newChineseName;width: parent.width*0.6}
                                CusButton{height: parent.height;width: height;imageSrc:"qrc:/Pics/base-svg/btn_find.svg";onClicked:{patientInfoListView.patientListModelVm.getPatientListByName(newChineseName.text,dateFrom.text,dateTo.text);}}
                            }

                            Row{
                                id:newEnglishFirstNameRow;visible:false;width: parent.width;height:patientInfo.rowHight;spacing: parent.width*0.02
                                CusText{text:"*"+lt+qsTr("First name")+" "; horizontalAlignment: Text.AlignRight ;width:parent.width*0.20;font.pointSize: fontPointSize;}
                                LineEdit{id:newEnglishFirstName;width: parent.width*0.6}
                                CusButton{height: parent.height;width: height;imageSrc:"qrc:/Pics/base-svg/btn_find.svg";onClicked:{patientInfoListView.patientListModelVm.getPatientListByName(newEnglishFirstName.text+" "+newEnglishLastNameRow.text,dateFrom.text,dateTo.text);}}
                            }

                            Row{
                                id:newEnglishLastNameRow;visible:false;width: parent.width;height:patientInfo.rowHight;spacing: parent.width*0.02
                                CusText{text:"*"+lt+qsTr("Last name"); horizontalAlignment: Text.AlignRight ;width:parent.width*0.20;font.pointSize: fontPointSize;}
                                LineEdit{id:newEnglishLastName;width: parent.width*0.6}
                            }

                            Row{
                                width: parent.width;height:patientInfo.rowHight;spacing: parent.width*0.02
                                CusText{text:"*"+lt+qsTr("Sex")+" "; horizontalAlignment: Text.AlignRight ;width:parent.width*0.20;font.pointSize: fontPointSize;}
                                Row{
                                    id:genderSelect;property int gender;
                                    height:parent.height;spacing:(width-6*height)/2;width:newPatient.width*0.6
                                    CusButton{
                                        id:manButton;property bool chosen:false;imageSrc: "qrc:/Pics/base-svg/btn_sex_man.svg";width: 2*height
                                        onClicked: {genderSelect.selectGender(0)}
                                    }
                                    CusButton{
                                        id:womanButton;property bool chosen:false;imageSrc: "qrc:/Pics/base-svg/btn_sex_woman.svg";width: 2*height
                                        onClicked:  {genderSelect.selectGender(1)}
                                    }
                                    CusButton{
                                        id:otherButton;property bool chosen:false;text:lt+qsTr("Others");width:height*2;
                                        onClicked: {genderSelect.selectGender(2)}
                                    }
                                    function selectGender(id)
                                    {
                                        manButton.borderColor=manButton.commonBorderColor;
                                        womanButton.borderColor=womanButton.commonBorderColor;
                                        otherButton.borderColor=otherButton.commonBorderColor;
                                        switch(id)
                                        {
                                            case 0:genderSelect.gender=0; manButton.borderColor=manButton.pressBorderColor;break;
                                            case 1:genderSelect.gender=1; womanButton.borderColor=womanButton.pressBorderColor;break;
                                            case 2:genderSelect.gender=2; otherButton.borderColor=otherButton.pressBorderColor;break;
                                        }
                                    }
                                    Component.onCompleted: {genderSelect.selectGender(0);}
                                }
                            }
                            Row{
                                id:newBirthDateRow
                                width: parent.width;height:patientInfo.rowHight;spacing: parent.width*0.02
                                CusText{text:"*"+lt+qsTr("Birth date")+" "; horizontalAlignment: Text.AlignRight ;width:parent.width*0.20;font.pointSize: fontPointSize;}
                                Row{
                                    height:parent.height;spacing:(width-6*height)/2;width:newPatient.width*0.6
                                    Item{height: parent.height;width: 2*height;
                                        LineEdit{id:newBirthDate;/*enabled:false;*/width: height*3.1;
                                            // textInput.validator: RegExpValidator { regExp: /^\d{4}-((0[1-9])|(1[012]))-((0[1-9]|[12]\d)|3[01])$/}
                                            textInput.validator:dateValidator;
                                            onTextChanged:{newPatientage.text=CusUtils.getAge(newBirthDate.text);}}}
                                    CusButton{
                                        text:lt+qsTr("Select");width:height*2;onClicked:{calendar.inputObj=newBirthDate;calendar.open();}}
                                    LineEdit{
                                        id:newPatientage;width: height*2;text:"";radius: height*0.2;horizontalAlignment: Text.AlignHCenter;enabled:false;backgroundColor:backGroundColor;
                                    }
                                }
                            }
                        }
                    }
                }
                Rectangle{
                    width: parent.width;height: (parent.height-content.layoutGroupMargin)*0.4;
                    border.color: backGroundBorderColor;color: backGroundColor;radius: commonRadius;
                   Item{
                       anchors.fill: parent;anchors.margins: content.layoutGroupMargin;
                       Column{
                           anchors.fill: parent;
                           Row{
                               width: parent.width;height:patientInfo.rowHight;spacing: parent.width*0.05
                               Item{height:parent.height;width:parent.width*0.15}
                               Row{
                                   height:parent.height;spacing:(width-8*height)/3;width:newPatient.width*0.7
                                   CusText{ text:lt+qsTr("Sphere");width: 2*height;font.pointSize: fontPointSize;}
                                   CusText{ text:lt+qsTr("Cylinder");width: 2*height;font.pointSize: fontPointSize;}
                                   CusText{ text:lt+qsTr("Axis");width: 2*height;font.pointSize: fontPointSize;}
                                   CusText{ text:lt+qsTr("Visual acuity");width: 2*height;font.pointSize: fontPointSize;}
                               }
                           }
                           Column{
                               id:eyeBallCalc;width: parent.width;height: parent.height-patientInfo.rowHight;spacing: patientInfo.rowHight*0.8;
                               Row{width: parent.width;height:patientInfo.rowHight;spacing: parent.width*0.05
                                   CusText{text:lt+qsTr("Right eye"); horizontalAlignment: Text.AlignRight;width:parent.width*0.15;font.pointSize: fontPointSize;}
                                   Row{
                                       height:parent.height;spacing:(width-8*height)/3;width:newPatient.width*0.7
                                       LineEdit{ id:rx1_r;width: 2*height;horizontalAlignment: Text.AlignHCenter;/*text:currentPatient==null?"":currentPatient.rx.rx1_r.toFixed(2);*/textInput.validator: DoubleValidator{notation: DoubleValidator.StandardNotation;decimals: 2}}
                                       LineEdit{ id:rx2_r;width: 2*height;horizontalAlignment: Text.AlignHCenter;/*text:currentPatient==null?"":currentPatient.rx.rx2_r.toFixed(2);*/textInput.validator: DoubleValidator{notation: DoubleValidator.StandardNotation;decimals: 2}}
                                       LineEdit{ id:rx3_r;width: 2*height;horizontalAlignment: Text.AlignHCenter;/*text:currentPatient==null?"":currentPatient.rx.rx3_r.toFixed(2);*/textInput.validator: DoubleValidator{notation: DoubleValidator.StandardNotation;decimals: 2}}
                                       LineEdit{ id:visual_r;width: 2*height;horizontalAlignment: Text.AlignHCenter;/*text:currentPatient==null?"":currentPatient.rx.visual_r.toFixed(2);*/textInput.validator: DoubleValidator{notation: DoubleValidator.StandardNotation;decimals: 2}}
//                                       Component.onCompleted:
//                                       {
//                                           currentPatientChanged.connect(function(){
//                                           if(currentPatient!==null&&!creatingNewPatient)
//                                           {
//                                               rx1_r.text=currentPatient.rx.rx1_r.toFixed(2);
//                                               rx2_r.text=currentPatient.rx.rx2_r.toFixed(2);
//                                               rx3_r.text=currentPatient.rx.rx3_r.toFixed(2);
//                                               visual_r.text=currentPatient.rx.visual_r.toFixed(2);
//                                           }
//                                           });
//                                       }
                                   }
                               }
                               Row{
                                   width: parent.width;height:patientInfo.rowHight;spacing: parent.width*0.05
                                   CusText{text:lt+qsTr("Left eye"); horizontalAlignment: Text.AlignRight;width:parent.width*0.15;font.pointSize: fontPointSize;}
                                   Row{
                                       height:parent.height;spacing:(width-8*height)/3;width:newPatient.width*0.7
                                       LineEdit{ id:rx1_l;width: 2*height;horizontalAlignment: Text.AlignHCenter;/*text:currentPatient==null?"":currentPatient.rx.rx1_l.toFixed(2);*/textInput.validator: DoubleValidator{notation: DoubleValidator.StandardNotation;decimals: 2}}
                                       LineEdit{ id:rx2_l;width: 2*height;horizontalAlignment: Text.AlignHCenter;/*text:currentPatient==null?"":currentPatient.rx.rx2_l.toFixed(2);*/textInput.validator: DoubleValidator{notation: DoubleValidator.StandardNotation;decimals: 2}}
                                       LineEdit{ id:rx3_l;width: 2*height;horizontalAlignment: Text.AlignHCenter;/*text:currentPatient==null?"":currentPatient.rx.rx3_l.toFixed(2);*/textInput.validator: DoubleValidator{notation: DoubleValidator.StandardNotation;decimals: 2}}
                                       LineEdit{ id:visual_l;width: 2*height;horizontalAlignment: Text.AlignHCenter;/*text:currentPatient==null?"":currentPatient.rx.visual_l.toFixed(2);*/textInput.validator: DoubleValidator{notation: DoubleValidator.StandardNotation;decimals: 2}}
//                                       Component.onCompleted:
//                                       {
//                                           currentPatientChanged.connect(function(){
//                                               if(currentPatient!==null&&!creatingNewPatient)
//                                               {
//                                                   rx1_l.text=currentPatient.rx.rx1_l.toFixed(2);
//                                                   rx2_l.text=currentPatient.rx.rx2_l.toFixed(2);
//                                                   rx3_l.text=currentPatient.rx.rx3_l.toFixed(2);
//                                                   visual_l.text=currentPatient.rx.visual_l.toFixed(2);
//                                               }
//                                           });
//                                       }
                                   }
                               }
                               Row{
                                   width: parent.width;height:patientInfo.rowHight;spacing: parent.width*0.05
                                   Item{height:parent.height;width:parent.width*0.15}
                                   Item{height:parent.height;width:newPatient.width*0.7
                                       CusButton{text:lt+qsTr("Calculate"); anchors.horizontalCenter: parent.horizontalCenter; width: 2*height;
                                           function getRx1(rx1,rx2,age)
                                           {
                                               var x,y;
                                               x=parseFloat(rx2);
                                               if(x<0.5||x>1.25) x=0;
                                               else x=x/2;

                                               if(age>=60) y=3.25;
                                               else if(age>=55) y=3;
                                               else if(age>=50) y=2.5;
                                               else if(age>=45) y=2;
                                               else if(age>=40) y=1.5;
                                               else if(age>=30) y=1;
                                               else y=0;

                                               return parseFloat(rx1)+x+y;
                                           }

                                           onClicked:
                                           {
                                               rx1_r.text=getRx1(rx1_r.text,rx2_r.text,newPatientage.text).toFixed(2);
                                               rx1_l.text=getRx1(rx1_l.text,rx2_l.text,newPatientage.text).toFixed(2);
                                           }
                                       }
                                   }
                               }
                           }
                        }
                    }
                }
            }
        }
    }
    Rectangle{id:bottomRibbon;width: parent.width;height: parent.height*1/15;color: "#333e44";anchors.bottom: parent.bottom
        Row{anchors.fill: parent;anchors.margins:parent.height*0.15;
            CusButton{
                text:lt+qsTr("Exit");
                onReleased: function() { Qt.callLater(tryClose); }
                function tryClose(){
                    IcUiQmlApi.appCtrl.checkSvc.leaveCheck();
                    Qt.quit();
                }
            }
            Item{height: parent.height;width: parent.width*0.08}
            Item
            {
                width: parent.width*0.32
                height: parent.height;
                Flow{
                    anchors.fill: parent;
                    layoutDirection: Qt.LeftToRight;spacing: height*0.8;anchors.horizontalCenter: parent.horizontalCenter
                    CusButton{
                        text:lt+qsTr("Recheck");
                        enabled:currentPatient!==null&&lastProgram!==null&&(IcUiQmlApi.appCtrl.checkSvc.debugMode||IcUiQmlApi.appCtrl.checkSvc.castLightAdjustStatus===3&&IcUiQmlApi.appCtrl.checkSvc.deviceStatus===2);
                        onClicked:root.changePage("check",{lastProgram:lastProgram,type:"reCheck"})
                    }
                    CusButton{
                        id:patientReviseButton;enabled: false;text:lt+qsTr("Modify");
                        onClicked:
                        {
                            var Name;
                            if(!doubleName) Name=newChineseName.text;else {Name=newEnglishFirstName.text+" "+newEnglishLastName.text;}
                            modifyPatient.patientId=newPatientId.text;
                            modifyPatient.name=Name;
                            modifyPatient.sex=genderSelect.gender;
                            modifyPatient.birthDate=newBirthDate.text;

                            if(parseFloat(rx1_l.text).toString()!=="NaN")
                                modifyPatient.rx.rx1_l=parseFloat(rx1_l.text);
                            if(parseFloat(rx2_l.text).toString()!=="NaN")
                                modifyPatient.rx.rx2_l=parseFloat(rx2_l.text);
                            if(parseFloat(rx3_l.text).toString()!=="NaN")
                                modifyPatient.rx.rx3_l=parseFloat(rx3_l.text);
                            if(parseFloat(visual_l.text).toString()!=="NaN")
                                modifyPatient.rx.visual_l=parseFloat(visual_l.text);
                            if(parseFloat(rx1_r.text).toString()!=="NaN")
                                modifyPatient.rx.rx1_r=parseFloat(rx1_r.text);
                            if(parseFloat(rx2_r.text).toString()!=="NaN")
                                modifyPatient.rx.rx2_r=parseFloat(rx2_r.text);
                            if(parseFloat(rx3_r.text).toString()!=="NaN")
                                modifyPatient.rx.rx3_r=parseFloat(rx3_r.text);
                            if(parseFloat(visual_r.text).toString()!=="NaN")
                                modifyPatient.rx.visual_r=parseFloat(visual_r.text);
                            modifyPatient.update();
                            if(modifyPatient.id===currentPatient.id)
                            {
                                IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::PatientVm", currentPatient);
                                currentPatient=IcUiQmlApi.appCtrl.objMgr.attachObj("Perimeter::PatientVm", false,[modifyPatient.id]);
                            }
                            query.startQuery();
                        }
                    }
                    CusButton{
                        text:lt+qsTr("Delete");
                        enabled: CommonSettings.deletePermission&&patientInfoListView.patientListModelVm.selectedCount>0;
                        onClicked: {
                            if(currentPatient!==null) IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::PatientVm", currentPatient);
                            if(modifyPatient!==null) IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::PatientVm", currentPatient);
                            currentPatient=null;
                            modifyPatient=null;
                            patientInfoListView.patientListModelVm.deletePatients();
                            newPatientButton.clicked()
                            query.startQuery();
                        }
                    }
                    CusButton{text:lt+qsTr("View reports");enabled:currentPatient!==null&&lastProgram!==null;onClicked: {root.changePage("analysisLobby",{});}}
                }

            }

            CusText
            {
                id:headsUp;color: "red";
                width: parent.width*0.31;height:parent.height;
            }

            Item{
                height:parent.height;width:parent.width*0.23;
                Flow{anchors.fill: parent; layoutDirection: Qt.RightToLeft;spacing: height*0.8;
                    CusButton{
                        text:lt+qsTr("Check");
    //                    enabled: true;
                        enabled:currentPatient!==null&&(IcUiQmlApi.appCtrl.checkSvc.debugMode||(IcUiQmlApi.appCtrl.checkSvc.deviceStatus===2&&IcUiQmlApi.appCtrl.checkSvc.castLightAdjustStatus===3));
                        onClicked:
                        {
                            root.changePage("check",{lastProgram:null,type:"check"});
    //                        console.log(currentPatient!==null);
    //                        console.log(IcUiQmlApi.appCtrl.checkSvc.deviceStatus);
    //                        console.log(IcUiQmlApi.appCtrl.checkSvc.debugMode);
    //                        console.log(IcUiQmlApi.appCtrl.checkSvc.castLightAdjustStatus);
                        }
                    }
                    CusButton{id:patientSaveButton;text:lt+qsTr("Save");/*enabled: false;*/
                        onClicked:{
                            creatingNewPatient=true;
                            headsUp.text="";
                            var name="";
                            if(!doubleName){
                                name=newChineseName.text;
                            }
                            else {
                                name = newEnglishFirstName.text+" "+newEnglishLastName.text;
                            }
                            console.log(newPatientId.text);
                            console.log(name);
                            console.log(newBirthDate.text);
                            if(newPatientId.text.trim()==="")
                                headsUp.text+=qsTr("Patient ID");
                            if(name.trim()==="")
                            {
                                if(headsUp.text!=="")
                                    headsUp.text+=","+qsTr("name");
                                else
                                    headsUp.text+=qsTr("Patient name");
                            }
                            if(newBirthDate.text.trim()==="")
                            {
                                if(headsUp.text!=="")
                                    headsUp.text+=","+qsTr("birth date");
                                else
                                    headsUp.text+=qsTr("Patient birth date");
                            }
                            console.log(headsUp.text.trim());
                            if(headsUp.text.trim()!=="")
                            {
                                headsUp.text+=" "+qsTr("can't be blank")+".";
                                creatingNewPatient=false;
                                return;
                            }

                            if(currentPatient!==null) IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::PatientVm", currentPatient);
                            currentPatient=IcUiQmlApi.appCtrl.objMgr.attachObj("Perimeter::PatientVm", false,[]);
                            currentPatient.patientId=newPatientId.text.trim();
                            currentPatient.name=name.trim();
                            currentPatient.sex=genderSelect.gender;
                            currentPatient.birthDate=newBirthDate.text.trim();
                            if(parseFloat(rx1_l.text).toString()!=="NaN")
                                currentPatient.rx.rx1_l=parseFloat(rx1_l.text);
                            if(parseFloat(rx2_l.text).toString()!=="NaN")
                                currentPatient.rx.rx2_l=parseFloat(rx2_l.text);
                            if(parseFloat(rx3_l.text).toString()!=="NaN")
                                currentPatient.rx.rx3_l=parseFloat(rx3_l.text);
                            if(parseFloat(visual_l.text).toString()!=="NaN")
                                currentPatient.rx.visual_l=parseFloat(visual_l.text);
                            if(parseFloat(rx1_r.text).toString()!=="NaN")
                                currentPatient.rx.rx1_r=parseFloat(rx1_r.text);
                            if(parseFloat(rx2_r.text).toString()!=="NaN")
                                currentPatient.rx.rx2_r=parseFloat(rx2_r.text);
                            if(parseFloat(rx3_r.text).toString()!=="NaN")
                                currentPatient.rx.rx3_r=parseFloat(rx3_r.text);
                            if(parseFloat(visual_r.text).toString()!=="NaN")
                                currentPatient.rx.visual_r=parseFloat(visual_r.text);
                            currentPatient.insert();
                            console.log(currentPatient.id);
                            root.currentPatientChanged();
                            query.startQuery();
                            creatingNewPatient=false;
                        }
                    }
                    CusButton{
                        id:newPatientButton;text:lt+qsTr("New");
                        onClicked:
                        {
                            if(currentPatient!==null) IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::PatientVm", currentPatient);
                            if(modifyPatient!==null) IcUiQmlApi.appCtrl.objMgr.detachObj("Perimeter::PatientVm", currentPatient);
                            currentPatient=null;
                            modifyPatient=null;
                            patientReviseButton.enabled=false;/*patientReviseButton.buttonColor="#787878"*/
                            patientSaveButton.enabled=true;/* patientSaveButton.buttonColor = "#dcdee0"*/
                            newPatientId.text="";
                            newChineseName.text="";
                            newEnglishFirstName.text="";
                            newEnglishLastName.text="";
                            genderSelect.selectGender(0);
                            newBirthDate.text="";
                            rx1_r.text="";
                            rx2_r.text="";
                            rx3_r.text="";
                            visual_r.text="";
                            rx1_l.text="";
                            rx2_l.text="";
                            rx3_l.text="";
                            visual_l.text="";
                        }
                    }
                }

            }

        }
    }
}
