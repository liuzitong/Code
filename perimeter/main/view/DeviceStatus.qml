import QtQuick 2.6
import QtQuick.Controls 2.0
import QtQuick.Controls 1.0
import QtQuick.Window 2.3
import QtQml 2.2
import QtQuick.Controls.Styles 1.4
import qxpack.indcom.ui_qml_base 1.0
import perimeter.main.view.Utils 1.0
import perimeter.main.view.Controls 1.0
import QtQuick.Extras 1.4





Item
{
    property var statusData: IcUiQmlApi.appCtrl.deviceStatusData;
    id:root;width: 1366;height: 691;

    Rectangle {
        z:-1;
        anchors.fill: parent;
        color: "gray";opacity: 0.5;
    }

    Item
    {
        width: parent.width*0.93;height: parent.height-parent.width*0.07;
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        Row{
            anchors.fill: parent;
            Column{
                width: parent.width/4;height: parent.height;
                CusText{color:"yellow";horizontalAlignment: Text.AlignLeft; text:"serialNo:"+statusData.serialNo; height: parent.height*0.05}
                CusText{color:"yellow";horizontalAlignment: Text.AlignLeft;text:"answerpadStatus:"+statusData.answerpadStatus; height: parent.height*0.05}
                CusText{color:"yellow";horizontalAlignment: Text.AlignLeft;text:"cameraStatus:"+statusData.cameraStatus; height: parent.height*0.05}
                CusText{color:"yellow";horizontalAlignment: Text.AlignLeft;text:"eyeglassStatus:"+statusData.eyeglassStatus; height: parent.height*0.05}
                CusText{color:"yellow";horizontalAlignment: Text.AlignLeft;text:"xMotorBusy:"+statusData.xMotorBusy; height: parent.height*0.05}
                CusText{color:"yellow";horizontalAlignment: Text.AlignLeft;text:"yMotorBusy:"+statusData.yMotorBusy; height: parent.height*0.05}
                CusText{color:"yellow";horizontalAlignment: Text.AlignLeft;text:"focusMotorBusy:"+statusData.focusMotorBusy; height: parent.height*0.05}
                CusText{color:"yellow";horizontalAlignment: Text.AlignLeft;text:"colorMotorBusy:"+statusData.colorMotorBusy; height: parent.height*0.05}
                CusText{color:"yellow";horizontalAlignment: Text.AlignLeft;text:"lightSpotMotorBusy:"+statusData.lightSpotMotorBusy; height: parent.height*0.05}
                CusText{color:"yellow";horizontalAlignment: Text.AlignLeft;text:"shutterMotorBusy:"+statusData.shutterMotorBusy; height: parent.height*0.05}
                CusText{color:"yellow";horizontalAlignment: Text.AlignLeft;text:"xChinMotorBusy:"+statusData.xChinMotorBusy; height: parent.height*0.05}
                CusText{color:"yellow";horizontalAlignment: Text.AlignLeft;text:"yChinMotorBusy:"+statusData.yChinMotorBusy; height: parent.height*0.05}
                CusText{color:"yellow";horizontalAlignment: Text.AlignLeft;text:"xMotorCmdCntr:"+statusData.xMotorCmdCntr; height: parent.height*0.05}
                CusText{color:"yellow";horizontalAlignment: Text.AlignLeft;text:"yMotorCmdCntr:"+statusData.yMotorCmdCntr; height: parent.height*0.05}
                CusText{color:"yellow";horizontalAlignment: Text.AlignLeft;text:"focusMotorCmdCntr:"+statusData.focusMotorCmdCntr; height: parent.height*0.05}
                CusText{color:"yellow";horizontalAlignment: Text.AlignLeft;text:"colorMotorCmdCntr:"+statusData.colorMotorCmdCntr; height: parent.height*0.05}
                CusText{color:"yellow";horizontalAlignment: Text.AlignLeft;text:"lightSpotMotorCmdCntr:"+statusData.lightSpotMotorCmdCntr; height: parent.height*0.05}
                CusText{color:"yellow";horizontalAlignment: Text.AlignLeft;text:"shutterMotorCmdCntr:"+statusData.shutterMotorCmdCntr; height: parent.height*0.05}
                CusText{color:"yellow";horizontalAlignment: Text.AlignLeft;text:"xChinMotorCmdCntr:"+statusData.xChinMotorCmdCntr; height: parent.height*0.05}
                CusText{color:"yellow";horizontalAlignment: Text.AlignLeft;text:"yChinMotorCmdCntr:"+statusData.yChinMotorCmdCntr; height: parent.height*0.05}
            }
            Column{
                width: parent.width/4;height: parent.height;
                CusText{color:"yellow";horizontalAlignment: Text.AlignLeft;text:"moveStatus:"+statusData.moveStatus; height: parent.height*0.05}
                CusText{color:"yellow";horizontalAlignment: Text.AlignLeft;text:"xMotorCurrPos:"+statusData.xMotorCurrPos; height: parent.height*0.05}
                CusText{color:"yellow";horizontalAlignment: Text.AlignLeft;text:"yMotorCurrPos:"+statusData.yMotorCurrPos; height: parent.height*0.05}
                CusText{color:"yellow";horizontalAlignment: Text.AlignLeft;text:"focusMotorCurrPos:"+statusData.focusMotorCurrPos; height: parent.height*0.05}
                CusText{color:"yellow";horizontalAlignment: Text.AlignLeft;text:"colorMotorCurrPos:"+statusData.colorMotorCurrPos; height: parent.height*0.05}
                CusText{color:"yellow";horizontalAlignment: Text.AlignLeft;text:"lightSpotMotorCurrPos:"+statusData.lightSpotMotorCurrPos; height: parent.height*0.05}
                CusText{color:"yellow";horizontalAlignment: Text.AlignLeft;text:"shutterMotorCurrPos:"+statusData.shutterMotorCurrPos; height: parent.height*0.05}
                CusText{color:"yellow";horizontalAlignment: Text.AlignLeft;text:"xChinMotorCurrPos:"+statusData.xChinMotorCurrPos; height: parent.height*0.05}
                CusText{color:"yellow";horizontalAlignment: Text.AlignLeft;text:"yChinMotorCurrPos:"+statusData.yChinMotorCurrPos; height: parent.height*0.05}
                CusText{color:"yellow";horizontalAlignment: Text.AlignLeft;text:"envLightDA:"+statusData.envLightDA; height: parent.height*0.05}
                CusText{color:"yellow";horizontalAlignment: Text.AlignLeft;text:"castLightDA:"+statusData.castLightDA; height: parent.height*0.05}
            }
        }
    }


}

