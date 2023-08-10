import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQml   2.2
import QtQuick.Layouts 1.3
import qxpack.indcom.ui_qml_base  1.0
import qxpack.indcom.ui_qml_control 1.0
import qxpack.indcom.ui_qml_charts  1.0
Item {
   id: idRoot; anchors.fill: parent;
   implicitWidth: parent.width; implicitHeight: parent.height;

    RowLayout {
        anchors.fill: parent;
//        IcQuickQtLogoItem {
//            width: 100; height: 100;
//        }
        IcQuickFboLineSeriesItem {
            width: 100; height: 100;
        }
    }
}
