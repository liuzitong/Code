import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQml   2.2
import QtQuick.Layouts  1.3
import qxpack.indcom.ui_qml_base  1.0
import qxpack.indcom.ui_qml_control 1.0
import qxpack.indcom.ui_qml_charts 1.0

Item {
   id: idRoot; anchors.fill: parent;
   implicitWidth: 800; implicitHeight: 600;

   IcQuickLineSeriesItem {
       anchors.left: parent.left; anchors.right: parent.right;
       anchors.leftMargin: 10;   anchors.rightMargin: 10;
       anchors.top : parent.top;  anchors.bottom: parent.bottom;
       anchors.topMargin: 10;    anchors.bottomMargin: parent.height * 0.6;

       id: idWaveGraph;
       xySeriesData: gWaveLineData;
       lineColor: "steelblue"; lineSize: idSizeAdj.value;
       viewport: Qt.rect( gViewport.x, gViewport.y, gViewport.width, gViewport.height ); //Qt.rect( idVwX.value, 1.0, idVwWidth.value, 2.0 )
       Rectangle { anchors.fill: parent; color:"#00000000"; border.color: "blue"; border.width: 1; }
       onGeomChanged: idTracePt.adjustPos();
       onViewportChanged: idTracePt.adjustPos();
   }

   Rectangle { // this block used to trace focused clicked point
       id: idTracePt; width: 3; height: 3; color: "red";
       property var  pick_info : null; onPick_infoChanged: adjustPos();
       function adjustPos() {
           if ( pick_info === null ) { return; }
           var px_pt = idWaveGraph.mapValuePointToPxPos( pick_info.valuePoint );
           var x_pct = px_pt.x / idWaveGraph.width;
           var y_pct = px_pt.y / idWaveGraph.height;
           x = idWaveGraph.x + x_pct * idWaveGraph.width  - width / 2;
           y = idWaveGraph.y + y_pct * idWaveGraph.height - height / 2;
           visible = ( px_pt.x >= 0 && px_pt.x <= idWaveGraph.width ) &&
                     ( px_pt.y >= 0 && px_pt.y <= idWaveGraph.height );
       }
   }

   MouseArea {
       anchors.fill: idWaveGraph;
       onClicked: {
           idTracePt.pick_info = idWaveGraph.makePickInfoObjAt( Qt.point( mouse.x, mouse.y ), IcQuickLineSeriesItem.PickDataMode_LinearX  );
       }
   }


   ColumnLayout {
        anchors.fill: parent;
        Item {
            Layout.preferredHeight: 50; Layout.fillWidth: true;
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop;
        }
        Item {
            id: idGraph; clip: false;
            Layout.preferredWidth:  parent.width * 0.8;
            Layout.preferredHeight: parent.width * 0.8 * 0.5;
            Layout.alignment: Qt.AlignCenter;
        }
        Item {
          Layout.fillHeight: true; Layout.fillWidth: true;
          Layout.alignment: Qt.AlignCenter;
        }

        // button group
        Item {
          Layout.preferredHeight: 40; Layout.fillWidth: true;
          Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom;
          RowLayout { anchors.fill: parent;
              TextField {
                  Layout.preferredWidth: 80; Layout.fillHeight: true;
                  text : "steelblue"; onEditingFinished: { idWaveGraph.lineColor = text; }
              }
              Slider {
                  id: idSizeAdj;  Layout.preferredWidth: 200; Layout.fillHeight: true;
                  from: idWaveGraph.lineSizeRange.x; to: idWaveGraph.lineSizeRange.y; value: 1; stepSize: 0.2;
              }
              Item {
                  Text { text: idSizeAdj.value.toFixed(2);  }
                  Layout.preferredWidth: 20; Layout.fillHeight: true;
              }
              Slider {
                  id: idVwX; Layout.preferredWidth: 160; Layout.fillHeight:  true;
                  from: - gViewport.width; to: gViewport.width; value: 0; stepSize: 0.1 * gViewport.width;
                  onValueChanged: {
                      var vw_prt = idWaveGraph.viewport;
                      idWaveGraph.viewport =  Qt.rect( value, vw_prt.y, vw_prt.width, vw_prt.height );
                  }
              }
              Item {
                  Text { text: idVwX.value.toFixed(2);  }
                  Layout.preferredWidth: 50; Layout.fillHeight: true;
              }
              Slider {
                  id: idVwWidth; Layout.preferredWidth: 120; Layout.fillHeight:  true;
                  from: 0; to: gViewport.width; value: gViewport.width; stepSize: 0.1 * gViewport.width;
                  onValueChanged:  {
                      var vw_prt = idWaveGraph.viewport;
                      idWaveGraph.viewport =  Qt.rect( vw_prt.x, vw_prt.y, value, vw_prt.height );
                  }
              }
              Item {
                  Text { text: idVwWidth.value.toFixed(2);  }
                  Layout.preferredWidth: 50; Layout.fillHeight: true;
              }
              Item { Layout.fillWidth: true; Layout.fillHeight: true; }
          }
        }

        Item {
          Layout.preferredHeight: 40; Layout.fillWidth: true;
          Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom;
        }
    }


}
