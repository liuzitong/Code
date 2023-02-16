import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import qxpack.indcom.ui_qml_base 1.0     // [HINT] this is the pre-registered module name.
import qxpack.indcom.ui_qml_control 1.0  // [HINT] ModalPopupDialog is in it
import perimeter.main.view.Controls 1.0
import perimeter.main.view.Utils 1.0

ModalPopupDialog /*Rectangle*/{   // this is the wrapped Popup element in ui_qml_contro
   id:idPopup
   property alias color: idContent.color;
   reqEnterEventLoop:false;
   anchors.fill: parent;
//   width: 1366;height: 640;

   property string lt:"";
   Component.onCompleted:{IcUiQmlApi.appCtrl.settings.langTriggerChanged.connect(function(){ltChanged();});}

   property int fontPointSize: CommonSettings.fontPointSize;
   property int rowHeight: CommonSettings.windowHeight*0.04;

   contentItem:
   Rectangle{
        id: idContent; color: "#60606060";implicitWidth: idPopup.width; implicitHeight: idPopup.height;
        Rectangle
        {
        // [HINT] Popup element need implicitWidth & implicitHeight to calc. the right position
            id: menu; width:idPopup.width*0.30; height: idPopup.height*0.5; color: "#f0f1f2";radius: 5;/*width:480; height:480;*/
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
                            spacing: rowHeight*1.0;
                            anchors.fill: parent;
                            Item{width: parent.width;height:rowHeight*0.1;}
                            Flow{
                                height: rowHeight; width: parent.width*0.7;anchors.horizontalCenter: parent.horizontalCenter;spacing: width*0.1
                                CusText{text:lt+qsTr("Select language")+":"; horizontalAlignment: Text.AlignRight;width:parent.width*0.3;font.pointSize: fontPointSize;}
                                CusComboBox{
                                    id:languageSelection;height: parent.height;width:parent.width*0.60;
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
                                CusText{text:lt+qsTr("Hospital name")+":"; horizontalAlignment: Text.AlignRight;width: parent.width*0.3;font.pointSize: fontPointSize;}
                                LineEdit{id:hospitalName;height: rowHeight;width:parent.width*0.60;text:IcUiQmlApi.appCtrl.settings.hospitalName}
                            }

                            Flow{
                                height: rowHeight;width: parent.width*0.7; anchors.horizontalCenter: parent.horizontalCenter;spacing: width*0.1
//                                CusText{text:"First name and last name separated";width: parent.width*0.3;font.pointSize: fontPointSize;}
//                                CusCheckBox{id:doubleName;checked:IcUiQmlApi.appCtrl.settings.doubleName;}
                            }
                        }
                        Item{
                            height: rowHeight; anchors.bottom: parent.bottom; anchors.bottomMargin: 0; anchors.horizontalCenter: parent.horizontalCenter; width: parent.width*0.6;
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
//                                    console.log(doubleName.checked);
//                                    IcUiQmlApi.appCtrl.settings.doubleName=doubleName.checked;
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

/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/
