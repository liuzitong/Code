import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQml   2.2
import QtQuick.Layouts  1.3

import qxpack.indcom.ui_qml_base  1.0
import qxpack.indcom.ui_qml_control 1.0
import qxpack.indcom.ui_qml_charts 1.0

Rectangle {
    id: idRoot;
    anchors.fill: parent;

IcQuickLineSeriesItem {
    id: idWaveGraph; anchors.fill: parent;
    arySeriesData: gWaveLineData;
    lineColor: "steelblue"; lineSize: idSizeAdj.value;
    viewport: Qt.rect( gViewport.x, gViewport.y, gViewport.width, gViewport.height ); //Qt.rect( idVwX.value, 1.0, idVwWidth.value, 2.0 )
    Rectangle { anchors.fill: parent; color:"#00000000"; border.color: "#ff00ff00"; border.width: 1; }
//    onGeomChanged: idTracePt.adjustPos();
//    onViewportChanged: idTracePt.adjustPos();
}
}
