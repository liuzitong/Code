import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import qxpack.indcom.ui_qml_base 1.0     // [HINT] this is the pre-registered module name.
import qxpack.indcom.ui_qml_control 1.0  // [HINT] ModalPopupDialog is in it
import perimeter.main.view.Controls 1.0
import perimeter.main.view.Utils 1.0
import QtQuick.Dialogs 1.0

ModalPopupDialog /*Rectangle*/{   // this is the wrapped Popup element in ui_qml_contro
    id:idPopup
    property alias color: idContent.color;
    reqEnterEventLoop:false;
    anchors.fill: parent;
  // width: 1366;height: 640;
    property string lt:"";
    property int fontPointSize: CommonSettings.fontPointSize;
    property int rowHeight: CommonSettings.windowHeight*0.045;

    Component.onCompleted:{
        IcUiQmlApi.appCtrl.settings.langTriggerChanged.connect(function(){ltChanged();});
        doctorUIDChanged.connect(function(){
                signatureImg.source="";
                signatureImg.source="file:///" + applicationDirPath + "/signature/"+doctorUID+".png";
            }
        );
    }




    contentItem:
    Rectangle{
        id: idContent; color: "#60606060";implicitWidth: idPopup.width; implicitHeight: idPopup.height;
        Rectangle
        {
        // [HINT] Popup element need implicitWidth & implicitHeight to calc. the right position
            id: menu; width:idPopup.width*0.35; height: idPopup.height*0.65; color: "#f0f1f2";radius: 5;/*width:480; height:480;*/
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            border.color: "#7C7C7C";
            Column{
                anchors.fill: parent;
                Canvas{
                    id:header;height: rowHeight;width: parent.width;
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
                        ctx.fillStyle = "#D2D2D3"
                        ctx.fill();
                    }
                    CusText{text:lt+qsTr("Settings"); horizontalAlignment: Text.AlignLeft;width: height*1.4;font.pointSize: fontPointSize;anchors.left:parent.left;anchors.leftMargin: height*0.5;}
                }
                Item{
                    width: parent.width;height: parent.height-header.height;
                    Item{
                        id: item1
                        anchors.fill: parent;anchors.margins:height/10
                        Column{
                            id: column
//                            property int rowHeight: height/12;
                            spacing: rowHeight*0.7;
                            anchors.fill: parent;
                            // Item{width: parent.width;height:rowHeight*0.05;}
                            Flow{
                                height: rowHeight; width: parent.width*0.7;anchors.horizontalCenter: parent.horizontalCenter;spacing: width*0.1
                                CusText{text:lt+qsTr("Select language")+":"; horizontalAlignment: Text.AlignRight;width:parent.width*0.44;font.pointSize: fontPointSize;}
                                CusComboBox{
                                    id:languageSelection;height: parent.height;width:parent.width*0.44;
                                    borderColor: backGroundBorderColor;font.family:"Microsoft YaHei";
                                    imageSrc: "qrc:/Pics/base-svg/btn_drop_down.svg";
                                    model: [qsTr("Default"), "中文" ,"English"]
                                    currentIndex:
                                    {
                                        if(IcUiQmlApi.appCtrl.settings.language==="Default") return 0;
                                        else if(IcUiQmlApi.appCtrl.settings.language==="Chinese") return 1;
                                        else return 2;
                                    }
                                }
                            }
                            Flow{
                                height: rowHeight;width: parent.width*0.7; anchors.horizontalCenter: parent.horizontalCenter;spacing: width*0.1
                                CusText{text:lt+qsTr("Hospital name")+":"; horizontalAlignment: Text.AlignRight;width: parent.width*0.44;font.pointSize: fontPointSize;}
                                LineEdit{id:hospitalName;height: rowHeight;width:parent.width*0.44;text:IcUiQmlApi.appCtrl.settings.hospitalName}
                            }

                            Flow{
                                height: rowHeight;width: parent.width*0.7; anchors.horizontalCenter: parent.horizontalCenter;spacing: width*0.1
                                CusText{text:lt+qsTr("Use VirtualKeyboard")+":"; horizontalAlignment: Text.AlignRight;width: parent.width*0.44;font.pointSize: fontPointSize;}
                                Switch {
                                    id: idEnableVkbd; width:parent.width*0.44; height:rowHeight;checked:IcUiQmlApi.appCtrl.settings.virtualKeyBoard;
                                    Component.onCompleted: {
                                        gVkbd.enabled=IcUiQmlApi.appCtrl.settings.virtualKeyBoard;
                                    }
                                }
                            }

                            Flow{
                                height: rowHeight;width: parent.width*0.7; anchors.horizontalCenter: parent.horizontalCenter;spacing: width*0.1
                                CusText{text:lt+qsTr("Use digital signature")+":"; horizontalAlignment: Text.AlignRight;width: parent.width*0.44;font.pointSize: fontPointSize;}
                                Switch {
                                    id:useDigitalSignature;
                                    width:parent.width*0.44; height:rowHeight;checked:IcUiQmlApi.appCtrl.settings.useDigitalSignature;
                                }
                            }

                            FileDialog {
                                id:fileDialog
                                nameFilters: [ "Image files (*.png *.jpg *.jepg *.bmp)"];
                                onAccepted: {
                                    // console.log(fileUrl);
                                    var path=String(fileUrl);
                                    path=path.split('///')[1];
                                    // console.log(doctorUID);
                                    IcUiQmlApi.appCtrl.signatureHelper.upLoadSignature(path,doctorUID);
                                    signatureImg.source="";
                                    signatureImg.source="file:///" + applicationDirPath + "/signature/"+doctorUID+".png";
                                }
                            }




                            Row{
                                height: rowHeight;width: parent.width*0.7; anchors.horizontalCenter: parent.horizontalCenter;spacing: width*0.1
                                CusButton{text:lt+qsTr("UpLoad signature")+":"; /*horizontalAlignment: Text.AlignRight;*/width: parent.width*0.44;/*font.pointSize: fontPointSize;*/onClicked: fileDialog.open();}
                                Item
                                {
                                    width:parent.width*0.44; height:rowHeight;
                                    Image
                                    {
                                        id:signatureImg;
                                        // property string picSource: "/signature/"+doctorUID+".png";

                                        height: parent.height;
                                        anchors.horizontalCenter: parent.horizontalCenter
                                        fillMode: Image.PreserveAspectFit
                                        source: "file:///" + applicationDirPath + "/signature/"+doctorUID+".png";
                                        layer.smooth: true
                                        antialiasing: true;
                                        cache:false;

                                    }
                                }
                            }
                        }
                        Item{
                            height: rowHeight; anchors.bottom: parent.bottom; anchors.bottomMargin: 0; anchors.horizontalCenter: parent.horizontalCenter; width: parent.width*0.5;
                            CusButton
                            {
                                buttonColor: CommonSettings.darkButtonColor;
                                text:lt+qsTr("OK");
                                anchors.left: parent.left;
                                anchors.leftMargin: 0;
                                onClicked:
                                {
                                    if(languageSelection.currentIndex==0) IcUiQmlApi.appCtrl.settings.language="Default";
                                    if(languageSelection.currentIndex==1) IcUiQmlApi.appCtrl.settings.language="Chinese";
                                    if(languageSelection.currentIndex==2) IcUiQmlApi.appCtrl.settings.language="English";
                                    IcUiQmlApi.appCtrl.settings.virtualKeyBoard=idEnableVkbd.checked;
                                    IcUiQmlApi.appCtrl.settings.useDigitalSignature=useDigitalSignature.checked;
                                    gVkbd.enabled=idEnableVkbd.checked;
                                    IcUiQmlApi.appCtrl.settings.hospitalName=hospitalName.text;
                                    IcUiQmlApi.appCtrl.settings.save();
                                    idPopup.close();
                                }
                            }
                            CusButton{buttonColor: CommonSettings.darkButtonColor;text:lt+qsTr("Cancel"); anchors.right: parent.right; anchors.rightMargin: 0;onClicked: {idPopup.close();}}
                        }
                    }
                }
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
