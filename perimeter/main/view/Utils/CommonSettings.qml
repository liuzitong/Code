pragma Singleton
import QtQuick 2.0
import qxpack.indcom.ui_qml_base 1.0     // [HINT] this is the pre-registered module name.
Item {
    property string backGroundColor:"#dcdee0"
    property string backGroundBorderColor:"#bdc0c6"
    property string ribbonColor: "#333e44"
    property string darkButtonColor:"#e0e0e0";
    property int windowHeight;

    property int textHeight: windowHeight*0.0180;
    property int fontPointSize: IcUiQmlApi.appCtrl.settings.language==="Chinese"?textHeight*0.8:textHeight*0.7;
    property bool deletePermission: false;
    property string buttonDisabledBackGroundColor: "#c0c0c0";
    property string buttonBackGroundColor: "#eceef0";
    property string buttonDisabledTextColor: "gray";
    property string buttonTextColor: "black";
}

/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/
