import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQml   2.2
import QtQuick.Layouts  1.3
import qxpack.indcom.ui_qml_base    1.0
import qxpack.indcom.ui_qml_control 1.0
import qxpack.indcom.ui_qml_charts  1.0

Item {
   id: idRoot; anchors.fill: parent;
   implicitWidth: 800; implicitHeight: 600;

   Item {
       id: idPlaceHolder; width: idRoot.width * 0.9; height: idRoot.height * 0.5;
   }
   IcQuickParLnItem {
       anchors.fill: idPlaceHolder;
       lineColor: "steelblue"; lineSize: idSizeAdj.value; anchors.centerIn: parent;
       viewport: Qt.rect( idVwX.value, idVwX.value, idVwWidth.value, idVwWidth.value )
       min: 0; max: 10; tickCount: 9;  headRsvdPx: 10; tailRsvdPx: 10;  showHeadLine: false; showTailLine: false;
       Rectangle { anchors.fill: parent; color:"#00000000"; border.color: "blue"; border.width: 1; }
   }
   IcQuickParLnItem {
       anchors.fill: idPlaceHolder;
       lineColor: "red"; lineSize: idSizeAdj.value; anchors.centerIn: parent;
       viewport: Qt.rect( idVwX.value, idVwX.value, idVwWidth.value, idVwWidth.value )
       min: 0; max: 10; tickCount: 9; vertical: true;
       Rectangle { anchors.fill: parent; color:"#00000000"; border.color: "blue"; border.width: 1; }
   }


   ColumnLayout {
        anchors.fill: parent;
        Item {
            Layout.preferredHeight: 50; Layout.fillWidth: true;
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop;
        }
        Item {
            Layout.preferredWidth:  parent.width * 0.8;
            Layout.preferredHeight: parent.width * 0.8 * 0.3;
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
                  text : "steelblue"; onEditingFinished: { idAxis.lineColor = text; }
              }
              Slider {
                  id: idSizeAdj;  Layout.preferredWidth: 120; Layout.fillHeight: true;
                  from: 0; to: 10; value: 1; stepSize: 0.2;
              }
              Item {
                  Text { text: idSizeAdj.value.toFixed(2);  }
                  Layout.preferredWidth: 50; Layout.fillHeight: true;
              }
              Slider {
                  id: idVwX; Layout.preferredWidth: 120; Layout.fillHeight:  true;
                  from: -10; to: 10; value: 0; stepSize: 0.1
              }
              Item {
                  Text { text: idVwX.value.toFixed(2);  }
                  Layout.preferredWidth: 50; Layout.fillHeight: true;
              }
              Slider {
                  id: idVwWidth; Layout.preferredWidth: 120; Layout.fillHeight:  true;
                  from: 0; to: 10; value: 50; stepSize: 0.1
              }
              Item {
                  Text { text: idVwWidth.value.toFixed(2);  }
                  Layout.preferredWidth: 50; Layout.fillHeight: true;
              }
              Switch {
                  id: idVwVert; Layout.preferredWidth: 80; Layout.preferredHeight: 50;
                  text: "isVert."; checked: false;
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
