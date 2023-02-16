import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import perimeter.main.view.Controls 1.0
import qxpack.indcom.ui_qml_base 1.0     // [HINT] this is the pre-registered module name.
import qxpack.indcom.ui_qml_control 1.0  // [HINT] ModalPopupDialog is in it
import perimeter.main.view.Utils 1.0


ModalPopupDialog /*Rectangle*/{   // this is the wrapped Popup element in ui_qml_contro
    id:idPopup
    property alias color: idContent.color;
    reqEnterEventLoop:false;
    width: 1366;height: 680;
//    property alias contentWidth : idContent.implicitWidth;
//    property alias contentHeight: idContent.implicitHeight;
//    property int textHeight: height*0.02;
    property int fontPointSize: CommonSettings.fontPointSize;
    signal confirm(var outterRadius,var innerRadius,var gap);
    signal cancel;

    property var range: currentProgram.params.commonParams.Range;


   contentItem:
   Rectangle{
        id: idContent; color: "#60606060";implicitWidth: idPopup.width; implicitHeight: idPopup.height;
        Rectangle
        {
        // [HINT] Popup element need implicitWidth & implicitHeight to calc. the right position
            id: menu; width:idPopup.width*0.30; height: idPopup.height*0.6;color: "#f0f1f2";radius: 5;/*width:480; height:480;*/
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            border.color: "#7C7C7C";
            Column{anchors.fill: parent;
                Canvas{id:header;height: parent.height/10;width: parent.width;property alias radius: menu.radius;
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
                    CusText{text:lt+qsTr("Create program according to circle"); horizontalAlignment: Text.AlignLeft;font.pointSize:fontPointSize;height:parent.height;anchors.left: parent.left; anchors.leftMargin:height*0.5;}
                }
                Item{width: parent.width;height: parent.height-header.height;
                    Item{width: parent.width*0.8;height: parent.height*0.8;anchors.horizontalCenter: parent.horizontalCenter;anchors.verticalCenter: parent.verticalCenter;
                        Column{id:column;property int rowHeight: height/8;anchors.fill: parent;spacing: rowHeight;
                            Row{width: parent.width*0.6;height: parent.rowHeight;anchors.horizontalCenter: parent.horizontalCenter;spacing: width*0.1
                                CusText{text:lt+qsTr("Outer circle");width: parent.width*0.3;font.pointSize: CommonSettings.fontPointSize;}
                                NumberLineEdit{id:outerRadius;width: parent.width*0.6;step:1;max:range[1];min:range[0];}
                            }
                            Row{width: parent.width*0.6;height: parent.rowHeight;anchors.horizontalCenter: parent.horizontalCenter;spacing: width*0.1
                                CusText{text:lt+qsTr("Inner circle");width: parent.width*0.3;font.pointSize: CommonSettings.fontPointSize;}
                                NumberLineEdit{id:innerRadius;width: parent.width*0.6;step:1;max:range[1];min:range[0];}
                            }
                            Row{width: parent.width*0.6;height: parent.rowHeight;anchors.horizontalCenter: parent.horizontalCenter;spacing: width*0.1;
                                CusText{text:lt+qsTr("Spacing");width: parent.width*0.3;font.pointSize: CommonSettings.fontPointSize;}
                                NumberLineEdit{id:gap;width: parent.width*0.6;step:1;max:range[1];value:1;min:1;}
                            }
                        }
                        Row{
                            height: column.rowHeight; anchors.bottom: parent.bottom; width: parent.width;spacing: width*0.05;
                            CusText{width: parent.width*0.40;text:"* "+qsTr("Unit")+":°,"+qsTr("Degree");horizontalAlignment: Text.AlignLeft;font.pointSize:CommonSettings.fontPointSize;}
                            CusButton{width: parent.width*0.25;buttonColor: "#e0e0e0";text:lt+qsTr("OK");
                                onClicked: {confirm(outerRadius.value,innerRadius.value,gap.value);idPopup.close();}
                            }
                            CusButton{width: parent.width*0.25;buttonColor: "#e0e0e0";text:lt+qsTr("Cancel");
                                onClicked: {cancel();idPopup.close();}
                            }
                        }
                    }
                }
            }
        }
    }
}
