import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import perimeter.main.view.Controls 1.0
import qxpack.indcom.ui_qml_base 1.0     // [HINT] this is the pre-registered module name.
import qxpack.indcom.ui_qml_control 1.0  // [HINT] ModalPopupDialog is in it
import perimeter.main.view.Utils 1.0

ModalPopupDialog/* Rectangle*/{   // this is the wrapped Popup element in ui_qml_contro
    id:idPopup
    property alias color: idContent.color;
    reqEnterEventLoop:false;
//    width:1366; height: 640;
//    property alias contentWidth : idContent.implicitWidth;
//    property alias contentHeight: idContent.implicitHeight;
    property string backGroundColor:"#dcdee0"
    property string backGroundBorderColor:"#bdc0c6"
    property int type;
    signal ok();
    signal dataRefreshed();
    signal cancel();
    signal clearResult();

    property var currentProgram:null;
    property int fontPointSize: CommonSettings.fontPointSize;


   contentItem:
   Rectangle{
        id: idContent; color: "#60606060";implicitWidth: idPopup.width; implicitHeight: idPopup.height;
        Rectangle
        {
        // [HINT] Popup element need implicitWidth & implicitHeight to calc. the right position
            id: menu; width:isEng?idPopup.width*0.5:idPopup.width*0.4; height: idPopup.height*0.98;color: "#dcdee0";radius: 5;/*width:480; height:480;*/
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            border.color: "#7C7C7C";
            Column{
                id: column
                anchors.fill: parent;
                Canvas{
                    id:header;height: idPopup.height*0.05;width: parent.width;property alias radius: menu.radius;
                    onPaint: {var ctx = getContext("2d");ctx.beginPath(); // Start the path
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
                    CusText{text:lt+qsTr("Params setting"); horizontalAlignment: Text.AlignLeft;height:parent.height;anchors.left: parent.left;anchors.leftMargin:height*0.5;font.pointSize: fontPointSize;}
                }
                Column{
                    width: parent.width;height: parent.height-header.height;
                    Rectangle{
                        id: rectangle
                        height: parent.height*0.07;width:parent.width-2;color: "#D2D2D3";anchors.horizontalCenter: parent.horizontalCenter;
                        TabBar {id: bar;height: parent.height*0.8;width:isEng?parent.width*0.17:parent.width*0.12; anchors.bottom: parent.bottom; anchors.bottomMargin: 0;anchors.left: parent.left; anchors.leftMargin: 0.01*parent.width;spacing: 0;currentIndex: 0;
                            Repeater { model:[lt+qsTr("Common params")]
                                TabButton {width: bar.width;height: parent.height;
                                    Rectangle{anchors.fill: parent;color:parent.checked? "#E3E5E8":"#D2D2D3";
                                        Rectangle{width: parent.width;height: 3;color: "#0064B6";visible: parent.parent.checked? true:false; }
                                        CusText{text:modelData; anchors.fill: parent;color:parent.parent.checked?"#0064B6":"black";font.pointSize: height*0.3}
                                    }
                                }
                            }
                        }
                    }

                    StackLayout {
                        width: parent.width;height: parent.height*0.78;currentIndex: bar.currentIndex;
                        Rectangle{
                            anchors.fill: parent;
                            anchors.margins: parent.width*0.05;
                            radius: width*0.02;
                            id:eyeOptionsGroup;
                            width: parent.width*0.83;
                            height: parent.height*0.35;
                            anchors.horizontalCenter: parent.horizontalCenter;
                            border.color: backGroundBorderColor;
                            color: backGroundColor;
                            Row{
                                anchors.fill: parent;
                                anchors.margins: parent.width*0.04;
                                spacing:width*0.1;
                                property int rowHeight:height*0.09
                                Column{
                                    height: parent.height;width: parent.width*0.45;spacing:parent.rowHeight*0.8;
                                    Item{
                                        width: parent.width; height:parent.parent.rowHeight;
                                        CusText{text:lt+qsTr("Check range"); anchors.left: parent.left; anchors.leftMargin: 0;width: parent.width*0.4;horizontalAlignment: Text.AlignLeft;font.pointSize: fontPointSize;}
                                        LineEdit{width: parent.width*0.5; anchors.right: parent.right;readOnly: true;text:currentProgram===null?0:currentProgram.params.Range[1];}
                                    }
                                    Item{
                                        width: parent.width; height:parent.parent.rowHeight;
                                        CusText{text:lt+qsTr("Dots count"); anchors.left: parent.left; anchors.leftMargin: 0;width: parent.width*0.4;horizontalAlignment: Text.AlignLeft;font.pointSize: fontPointSize;}
                                        LineEdit{enabled:currentProgram.params.strategy===0;width: parent.width*0.5; anchors.right: parent.right;readOnly: true;text:currentProgram===null?0:currentProgram.data.dots.length;}
                                    }
                                    Item{
                                        width: parent.width; height:parent.parent.rowHeight;
                                        CusText{text:lt+qsTr("Strategy"); anchors.left: parent.left; anchors.leftMargin: 0;width: parent.width*0.4;horizontalAlignment: Text.AlignLeft;font.pointSize: fontPointSize;}
                                        LineEdit{
                                            property var strats:[lt+qsTr("Standard"),lt+qsTr("Blind area"),lt+qsTr("Dark area"),lt+qsTr("Straight line")];
                                            property int currentIndex:currentProgram===null?0:(currentProgram.type===2?currentProgram.params.strategy:0);
                                            width: parent.width*0.5; anchors.right: parent.right;readOnly: true;text:strats[currentIndex];
                                        }
                                    }
                                    Item{
                                        width: parent.width; height:parent.parent.rowHeight;
                                        CusText{text:lt+qsTr("Cursor color"); anchors.left: parent.left; anchors.leftMargin: 0;width: parent.width*0.4;horizontalAlignment: Text.AlignLeft;font.pointSize: fontPointSize;}
                                        CusComboBox{
                                            width: parent.width*0.5; anchors.right: parent.right;model:[lt+qsTr("White"),lt+qsTr("Red"),lt+qsTr("Blue")];
                                            currentIndex:currentProgram===null?0:currentProgram.params.cursorColor;
                                            Component.onCompleted: {idPopup.ok.connect(function(){currentProgram.params.cursorColor=currentIndex;})}
                                        }
                                    }
                                    Item{
                                        width: parent.width; height:parent.parent.rowHeight;
                                        CusText{text:lt+qsTr("Cursor size"); anchors.left: parent.left; anchors.leftMargin: 0;width: parent.width*0.4;horizontalAlignment: Text.AlignLeft;font.pointSize: fontPointSize;}
                                        CusComboBox{
                                            width: parent.width*0.5; anchors.right: parent.right;model:["I","II","III","IV","V"];
                                            currentIndex:currentProgram===null?0:currentProgram.params.cursorSize;
                                            Component.onCompleted: {idPopup.ok.connect(function(){currentProgram.params.cursorSize=currentIndex;})}
                                        }
                                    }
                                    Item{
                                        width: parent.width; height:parent.parent.rowHeight;
                                        CusText{text:lt+qsTr("Background color"); anchors.left: parent.left; anchors.leftMargin: 0;width: parent.width*0.4;horizontalAlignment: Text.AlignLeft;font.pointSize: fontPointSize;}
                                        CusComboBox{
                                            width: parent.width*0.5; anchors.right: parent.right;model:[lt+qsTr("White"),lt+qsTr("Yellow")];
                                            currentIndex:currentProgram===null?0:currentProgram.params.backGroundColor;
                                            Component.onCompleted: {idPopup.ok.connect(function(){currentProgram.params.backGroundColor=currentIndex;})}
                                        }
                                    }
                                }
                                Column{
                                   height: parent.height;width: parent.width*0.45;spacing:parent.rowHeight*0.8;
                                   Item{
                                       width: parent.width; height:parent.parent.rowHeight;
                                       CusText{text:lt+qsTr("Fixation target"); anchors.left: parent.left; anchors.leftMargin: 0;width: parent.width*0.4;horizontalAlignment: Text.AlignLeft;font.pointSize: fontPointSize;}
                                       LineEdit{width: parent.width*0.5; anchors.right: parent.right;readOnly: true;text:lt+qsTr("Center dot")}
                                   }
                                   Item{
                                       width: parent.width; height:parent.parent.rowHeight;
                                       CusText{text:lt+qsTr("Move speed"); anchors.left: parent.left; anchors.leftMargin: 0;width: parent.width*0.4;horizontalAlignment: Text.AlignLeft;font.pointSize: fontPointSize;}
                                       CusComboBox{
                                           width: parent.width*0.5; anchors.right: parent.right;model:["1°/"+qsTr("sec"),"2°/"+qsTr("sec"),"3°/"+qsTr("sec"),"4°/"+qsTr("sec"),"5°/"+qsTr("sec"),"6°/"+qsTr("sec"),"7°/"+qsTr("sec"),"8°/"+qsTr("sec"),"9°/"+qsTr("sec")];
                                           currentIndex:currentProgram===null?0:(currentProgram.params.speed);
                                           Component.onCompleted: {idPopup.ok.connect(function(){currentProgram.params.speed=currentIndex;})}
                                       }
                                   }
                                   Item{
                                       width: parent.width; height:parent.parent.rowHeight;
                                       CusText{text:lt+qsTr("Move brightness"); anchors.left: parent.left; anchors.leftMargin: 0;width: parent.width*0.4;horizontalAlignment: Text.AlignLeft;font.pointSize: fontPointSize;}
                                       NumberLineEdit{
                                           width: parent.width*0.5; anchors.right: parent.right;step:1;max:51;min:0;value:currentProgram.params.brightness;
                                           Component.onCompleted:
                                           {
                                               idPopup.ok.connect(function(){currentProgram.params.brightness=value;});
                                               idPopup.currentProgramChanged.connect(function(){
                                                   value=currentProgram.params.brightness;})
                                           }
                                       }
                                   }
                                   Item{
                                       width: parent.width; height:parent.parent.rowHeight;
                                       CusText{text:lt+qsTr("Move method"); anchors.left: parent.left; anchors.leftMargin: 0;width: parent.width*0.4;horizontalAlignment: Text.AlignLeft;font.pointSize: fontPointSize;}
                                       CusComboBox{
                                           enabled: currentProgram.params.strategy===1||currentProgram.params.strategy===2
                                           width: parent.width*0.5; anchors.right: parent.right;model:["4","6","8"];
                                           currentIndex:currentProgram===null?0:currentProgram.params.dynamicMethod;
                                           Component.onCompleted: {idPopup.ok.connect(function(){if(currentProgram.params.dynamicMethod!==currentIndex) idPopup.clearResult();currentProgram.params.dynamicMethod=currentIndex;})}
                                       }
                                   }
                                   Item{
                                       width: parent.width; height:parent.parent.rowHeight;
                                       CusText{text:lt+qsTr("Move distance"); anchors.left: parent.left; anchors.leftMargin: 0;width: parent.width*0.4;horizontalAlignment: Text.AlignLeft;font.pointSize: fontPointSize;}
                                       CusComboBox{
                                           enabled: currentProgram.params.strategy===1||currentProgram.params.strategy===2
                                           width: parent.width*0.5; anchors.right: parent.right;model:["5","10","15"];
                                           currentIndex:currentProgram===null?0:currentProgram.params.dynamicDistance;
                                           Component.onCompleted: {idPopup.ok.connect(function(){if(currentProgram.params.dynamicDistance!==currentIndex) idPopup.clearResult();currentProgram.params.dynamicDistance=currentIndex;})}
                                       }
                                   }
                                   Item{
                                       width: parent.width; height:parent.parent.rowHeight;
                                       CusText{text:lt+qsTr("Eye move alarm"); anchors.left: parent.left; anchors.leftMargin: 0;width: parent.width*0.4;horizontalAlignment: Text.AlignLeft;font.pointSize: fontPointSize;}
                                       CusComboBox{
                                           width: parent.width*0.5; anchors.right: parent.right;model:[lt+qsTr("Only alarm"),lt+qsTr("Alarm and pause")];
                                           currentIndex:currentProgram===null?0:currentProgram.params.fixationMonitor;
                                           Component.onCompleted: {idPopup.ok.connect(function(){currentProgram.params.fixationMonitor=currentIndex;})}
                                       }
                                   }
                                }

                            }

                        }
                        Rectangle{
                            anchors.fill: parent;
                            anchors.margins: parent.width*0.04;
                            radius: width*0.02;
                            width: parent.width*0.83;
                            height: parent.height*0.35;
                            anchors.horizontalCenter: parent.horizontalCenter;
                            border.color: "red";
                            color: "red";
                        }
                    }

                    Item {
                        width: parent.width;
                        height: parent.height*0.15
                        Flow{
                            anchors.top: parent.top
                            anchors.topMargin: parent.height*0.34;
                            anchors.right: parent.right
                            anchors.margins: parent.height*0.40;
                            spacing: height;
                            layoutDirection: Qt.RightToLeft
                            anchors.verticalCenter: parent.verticalCenter
                            CusButton{text:lt+qsTr("Cancel");onClicked: {idPopup.close();currentProgramChanged();}}
                            CusButton{text:lt+qsTr("OK");onClicked:{ok();idPopup.close();dataRefreshed();}}
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
