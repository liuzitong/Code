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
    property var analysisResult: null;
    property var analysisVm: null;
    signal refresh;
    signal clearDot;
    property int range: currentProgram.params.commonParams.Range[1];
    property int fontPointSize: CommonSettings.fontPointSize;
    property int textHeight:CommonSettings.textHeight;
    onRefresh: clearDot();

    anchors.fill: parent;
    Column{anchors.fill: parent;/*spacing:height*0.035*/
        Row{ id: row;width: parent.width;height:parent.height*0.97/3;
            Item{width: parent.width*0.18;height: parent.height;}
            AnalysisDiagram{
                width: parent.height;height:parent.height*1.25;
                anchors.verticalCenter: parent.verticalCenter
                source:"/previewImage/dbDiagram.bmp";
                range:root.range;
                analysisVm: root.analysisVm;
                onClicked:root.clearDot();
                Component.onCompleted: {root.refresh.connect(refresh);root.clearDot.connect(clearDot);}
            }
            Item{width: parent.width*0.1;height: parent.height;}
            AnalysisDiagram{
                width: parent.height;height:parent.height*1.25;
                anchors.verticalCenter: parent.verticalCenter
                source:"/previewImage/gray.bmp";
                range:root.range;
                analysisVm: root.analysisVm;
                onClicked:root.clearDot();
                Component.onCompleted: {root.refresh.connect(refresh);root.clearDot.connect(clearDot);}
            }
//            Item{
//                width:parent.width*0.72-parent.height*2;height:parent.height;
//                CusText{ width:parent.width;height:parent.height*0.15;verticalAlignment: Text.AlignVCenter;
//                    text: "    ("+currentProgram.params.commonParams.Range[1]+")" ;
//                    horizontalAlignment: Text.AlignLeft; anchors.left: parent.left; anchors.leftMargin: 0;anchors.verticalCenter: parent.verticalCenter}
//            }
        }

        Row{
            width: parent.width;height:parent.height*0.97/3;spacing:width*0.05
            AnalysisDiagram{
                width: parent.height;height:parent.height*1.25;
                anchors.verticalCenter: parent.verticalCenter
                source:"/previewImage/TotalDeviation.bmp";
                range:root.range;
                analysisVm: root.analysisVm;
                onClicked:root.clearDot();
                Component.onCompleted: {root.refresh.connect(refresh);root.clearDot.connect(clearDot);}
            }

            AnalysisDiagram{
                width: parent.height;height:parent.height*1.25;
                anchors.verticalCenter: parent.verticalCenter
                source:"/previewImage/PatternDeviation.bmp";
                range:root.range;
                analysisVm: root.analysisVm;
                onClicked:root.clearDot();
                Component.onCompleted: {root.refresh.connect(refresh);root.clearDot.connect(clearDot);}
            }

            Column {
                height:parent.height;width:parent.width*0.9-parent.height*2;spacing:textHeight*0.5;
                Item{width: parent.width;height: textHeight;}
                CusText{width:parent.width;height:textHeight;text:"VFI:"+Math.round(analysisResult.VFI*100)+"%" ;horizontalAlignment: Text.AlignLeft;}
                CusText{property var ght: if(analysisResult.GHT===0){return lt+qsTr("Out of limits");} else if(analysisResult.GHT===1){return lt+qsTr("Low sensitivity");}else if(analysisResult.GHT===2){return lt+qsTr("Border of limits");}else if(analysisResult.GHT===3){return lt+qsTr("Within normal limits");}
                width:parent.width;height:textHeight;horizontalAlignment: Text.AlignLeft;text:lt+qsTr("GHT")+": "+ ght;}
                CusText{width:parent.width;height:textHeight;text:lt+qsTr("MD")+": "+analysisResult.md.toFixed(2)+(analysisResult.p_md>0?" (<"+analysisResult.p_md+"%)":"") ;horizontalAlignment: Text.AlignLeft;}
                CusText{width:parent.width;height:textHeight;text:lt+qsTr("PSD")+": "+analysisResult.psd.toFixed(2)+(analysisResult.p_psd>0?" (<"+analysisResult.p_psd+"%)" :"");horizontalAlignment: Text.AlignLeft;}
            }
        }
        Row{width: parent.width;height:parent.height*0.03;spacing:width*0.05;
            CusText{width:parent.parent.height*0.98/3;height:textHeight;text: lt+qsTr("Total deviation"); verticalAlignment: Text.AlignVCenter; }
            CusText{width:parent.parent.height*0.98/3;height:textHeight;text: lt+qsTr("Pattern deviation");  verticalAlignment: Text.AlignVCenter;}
        }
        Row{
            width: parent.width;height:parent.height*0.97/3;spacing:width*0.05
            AnalysisDiagram{
                width: parent.height;height:parent.height*1.25;
                anchors.verticalCenter: parent.verticalCenter
                source:"/previewImage/TotalPE.bmp";
                range:root.range;
                analysisVm: root.analysisVm;
                onClicked:root.clearDot();
                Component.onCompleted: {root.refresh.connect(refresh);root.clearDot.connect(clearDot);}
            }
            AnalysisDiagram{
                width: parent.height;height:parent.height*1.25;
                anchors.verticalCenter: parent.verticalCenter
                source:"/previewImage/PatternPE.bmp";
                range:root.range;
                analysisVm: root.analysisVm;
                onClicked:root.clearDot();
                Component.onCompleted: {root.refresh.connect(refresh);root.clearDot.connect(clearDot);}
            }
            Item{
                height: parent.height;width:parent.width*0.9-2*parent.height;
                Column{
                    height: parent.height*0.5; anchors.bottom: parent.bottom; width:parent.width;spacing:height*0.08;
                    Repeater{
                        anchors.fill: parent;
                        model: [{image:"qrc:/grays/PE1.bmp",pe:"<5%"},{image:"qrc:/grays/PE2.bmp",pe:"<2%"},{image:"qrc:/grays/PE3.bmp",pe:"<1%"},{image:"qrc:/grays/PE4.bmp",pe:"<0.5%"}]
                        Row{
                            height: textHeight;width: parent.width;spacing: height*0.5
                            Image {height: sourceSize.height*2; fillMode: Image.PreserveAspectFit; width:sourceSize.height*2;smooth: false;source: modelData.image}
                            CusText{height:textHeight;text:modelData.pe; horizontalAlignment: Text.AlignLeft;anchors.verticalCenter: parent.verticalCenter;}
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
