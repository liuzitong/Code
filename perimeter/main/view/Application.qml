import QtQuick 2.6
import QtQuick.Controls 2.0
import QtQuick.Window 2.3
import QtQml 2.2
import QtQuick.Controls.Styles 1.4
import perimeter.main.view.Controls 1.0
import qxpack.indcom.ui_qml_base 1.0
import perimeter.main.view.Controls 1.0
import perimeter.main.view.Utils 1.0
import perm.view 1.0


Window {
    id: window;visible: true;width: 1366;height: 768;title: lt+qsTr("Perimeter");
//    minimumWidth: 1366;minimumHeight: 768;
//    visibility:"Maximized"          //最大化
    flags: Qt.Window /*| Qt.FramelessWindowHint          */ //不能隐藏任务栏 不知道为什么

    DeviceStatus{anchors.fill:parent;visible:IcUiQmlApi.appCtrl.showDeviceStatusData;z:10;}
    Content{id:content;anchors.fill: parent;visible: false;onLogin:{visible=false;permMgrView.visible=true;}}
    PermMgrView{
        id:permMgrView; anchors.fill: parent;
        onReqExitAndGo: {visible=false;content.permission=arg;content.visible=true;}
        onReqExit: {Qt.quit();}
    }
    property bool isEng: IcUiQmlApi.appCtrl.settings.isRuntimeLangEng;
    property string lt:"";
    Component.onCompleted: {
        IcUiQmlApi.appCtrl.settings.langTriggerChanged.connect(function(){ltChanged();});
        idPriv.init();
        CommonSettings.windowHeight=height;
    }

    onHeightChanged: {CommonSettings.windowHeight=height;console.log(height);console.log(CommonSettings.textHeight);console.log(CommonSettings.fontPointSize);}

//    function changePage(pageName)
//    {
//        switch(pageName)
//        {
//            case "main":content.visible=true;login.visible=false;break;
//            case "login":content.visible=false;login.visible=true;break;
//        }
//    }


//    Loader {
//        id: pageLoader
//        anchors.fill: parent;
//        source: "qrc:/perimeter/main/view/Main.qml";
//        function changePage(pageName)
//        {
//                console.log(pageName);
//            switch(pageName)
//            {
//                case "main": source="qrc:/perimeter/main/view/main.qml";break;
//                case "login": source="qrc:/perimeter/main/view/login.qml";break;
//            }
//        }
//    }

    // ////////////////////////////////////////////////////////////////////////
    // logic
    // ////////////////////////////////////////////////////////////////////////
//    Component.onDestruction: { idPriv.deinit(); }
    QtObject{
        id: idPriv;
        property var patient_vm:null;
        property var patientList_vm:null;
        property var currentProgram:null;
        function init()
        {
            IcUiQmlApi.appCtrl.databaseSvc.initDataBase();
        }
    }
//


}
