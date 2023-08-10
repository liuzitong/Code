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

   IcQuickValueAxisItem {
       anchors.fill: parent;
       anchors.leftMargin: 10; anchors.rightMargin: 10;
       anchors.topMargin:  10; anchors.bottomMargin: parent.height * 0.4;
       labelVisible: false;
       id: idAxis; width: idRoot.width * 0.9; height: idRoot.height * 0.3;
       lineColor: "steelblue"; lineSize: idSizeAdj.value; anchors.centerIn: parent;
       viewport: Qt.rect( min,min,max-min,max-min )
       min: 56.792; max: 891.485; blnMin: min; blnMax: max;
       majorTickCount: 6 /*max-min*/; minorTickCount: 9; minorHintStep: minorTickCount / 2 + 1;
       labelFont: Qt.font( { family:'Arial', weight: Font.Normal, pixelSize:20 } );
       majorSize:  20; minorSize: majorSize * 0.3; minorHintSize: majorSize * 0.5;
       labelDistOfv: majorSize * 0.5;
       tickDirection: IcQuickValueAxisItem.TickDirection_Both;
       dispStartTickLabel: true;
       vertical: idVwVert.checked;
       headRsvdPx: 10; tailRsvdPx: 5;
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
                  from: idAxis.lineSizeRange.x; to: idAxis.lineSizeRange.y; value: 1; stepSize: 0.2;
              }
              Item {
                  Text { text: idSizeAdj.value.toFixed(2);  }
                  Layout.preferredWidth: 50; Layout.fillHeight: true;
              }
              Slider {
                  id: idVwX; Layout.preferredWidth: 120; Layout.fillHeight:  true;
                  property rect vw_prt : idAxis.viewport;
                  from: -vw_prt.width - 10; to: vw_prt.width + 10; value: 0; stepSize: 0.1 * vw_prt.width
                  onValueChanged:  {
                      var vw = idAxis.viewport;
                      idAxis.viewport = Qt.rect( value, vw.y, vw.width, vw.height );
                  }
              }
              Item {
                  Text { text: idVwX.value.toFixed(2);  }
                  Layout.preferredWidth: 50; Layout.fillHeight: true;
              }
              Slider {
                  id: idVwWidth; Layout.preferredWidth: 120; Layout.fillHeight:  true;
                  from: 0; to: 10; value: 0; stepSize: 0.1
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
