import QtQuick 2.0
import QtQuick 2.6
import QtQuick.Controls 2.0
import QtQuick.Window 2.3
import QtQml 2.2
import QtQuick.Controls.Styles 1.4
import perimeter.main.view.Controls 1.0
import qxpack.indcom.ui_qml_base 1.0
import perimeter.main.view.Utils 1.0

Item {
    id:root;
    signal refresh;
    signal clearDot;
    anchors.fill: parent;
    property int range: currentProgram.params.commonParams.Range[1];
    property var analysisResult: null;
    property var analysisVm: null;
    onRefresh: clearDot();
    AnalysisDiagram{
//                id:defect;
        width: height;height:parent.height*0.5*1.25;
        anchors.left: parent.left
        anchors.top: parent.top
        source:"/previewImage/defectDepth.bmp";
        textTop:lt+qsTr("Defect depth")+"(db)";
        textBottom:"□=4db"+"Expectation";
        analysisVm: root.analysisVm;
        range:root.range;
        onClicked:root.clearDot();
        Component.onCompleted: {root.refresh.connect(refresh);root.clearDot.connect(clearDot);}
    }


    AnalysisDiagram{
        width: height;height:parent.height*0.5*1.25;
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        source:"/previewImage/dBDiagram.bmp";
        textTop:lt+qsTr("Threshold")+"(dB)";
        analysisVm: root.analysisVm;
        range:root.range;
        onClicked:root.clearDot();
        Component.onCompleted: {root.refresh.connect(refresh);root.clearDot.connect(clearDot);}
    }
}

/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/
