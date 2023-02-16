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
    property int range: currentProgram.params.commonParams.Range[1];
    property var analysisResult: null;
    property var analysisVm: null;
    onRefresh: clearDot();
    anchors.fill: parent;
    Column{anchors.fill: parent;spacing:height*0.2
        Row{ id: row;width: parent.width;height:parent.height*0.8/2;
            AnalysisDiagram{
                width: parent.height;height:parent.height;
                textTop:lt+qsTr("Threshold")+"(dB)"
                anchors.verticalCenter: parent.verticalCenter
                source:"/previewImage/dbDiagram.bmp";
                range:root.range;
                onClicked:root.clearDot();
                analysisVm: root.analysisVm;
                Component.onCompleted: {root.refresh.connect(refresh);root.clearDot.connect(clearDot);}
            }
            AnalysisDiagram{
                width: parent.height;height:parent.height;
                textTop:lt+qsTr("Gray tone")
                anchors.verticalCenter: parent.verticalCenter
                source:"/previewImage/gray.bmp";
                range:root.range;
                onClicked:root.clearDot();
                analysisVm: root.analysisVm;
                Component.onCompleted: {root.refresh.connect(refresh);root.clearDot.connect(clearDot);}
            }

            Column {
                height:parent.height*0.8;width:parent.width-parent.height*2
                Item{width:parent.width;height:parent.height*0.2;}
                CusText{width:parent.width;height:parent.height*0.12;text:"VFI:"+Math.round(analysisResult.VFI*100)+"%" ;horizontalAlignment: Text.AlignLeft;font.pointSize: height*0.40}
                Item{width:parent.width;height:parent.height*0.1;}
                CusText{
                property var ght: if(analysisResult.GHT===0){return lt+qsTr("Out of limits");} else if(analysisResult.GHT===1){return lt+qsTr("Low sensitivity");}else if(analysisResult.GHT===2){return lt+qsTr("Border of limits");}else if(analysisResult.GHT===3){return lt+qsTr("Within normal limits");}
                width:parent.width;height:parent.height*0.12;horizontalAlignment: Text.AlignLeft;font.pointSize: height*0.40;text:lt+qsTr("GHT")+": "+ ght;}
                Item{width:parent.width;height:parent.height*0.1;}
                CusText{width:parent.width;height:parent.height*0.12;text:lt+qsTr("MD")+": "+analysisResult.md.toFixed(2)+(analysisResult.p_md>0?" (<"+analysisResult.p_md+"%)":"") ;horizontalAlignment: Text.AlignLeft;font.pointSize: height*0.40}
                CusText{width:parent.width;height:parent.height*0.12;text:lt+qsTr("PSD")+": "+analysisResult.psd.toFixed(2)+(analysisResult.p_psd>0?" (<"+analysisResult.p_psd+"%)" :"");horizontalAlignment: Text.AlignLeft;font.pointSize: height*0.40}
            }

        }



        Row{
            width: parent.width;height:parent.height*0.8/2;
            AnalysisDiagram{
                width: parent.height;height:parent.height;
                textTop:lt+qsTr("Total deviation")
                anchors.verticalCenter: parent.verticalCenter
                source:"/previewImage/TotalPE.bmp";
                range:root.range;
                onClicked:root.clearDot();
                analysisVm: root.analysisVm;
                Component.onCompleted: {root.refresh.connect(refresh);root.clearDot.connect(clearDot);}
            }
            AnalysisDiagram{
                width: parent.height;height:parent.height;
                textTop:lt+qsTr("Pattern deviation")
                anchors.verticalCenter: parent.verticalCenter
                source:"/previewImage/PatternPE.bmp";
                range:root.range;
                onClicked:root.clearDot();
                analysisVm: root.analysisVm;
                Component.onCompleted: {root.refresh.connect(refresh);root.clearDot.connect(clearDot);}
            }

        }
    }

}

/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/
