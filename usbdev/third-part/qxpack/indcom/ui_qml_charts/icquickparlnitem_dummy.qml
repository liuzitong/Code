import QtQuick 2.9
import QtQml   2.2
import QtQuick.Controls 2.2

// ============================================================================
// below code are only for designer syntax
// ============================================================================
Item {
    implicitWidth: 64; implicitHeight: 64;

    property vector2d lineSizeRange : Qt.vector2d(0,10);
    property real     lineSize : 1;
    property color    lineColor: "#000000";
    property rect     viewport : Qt.rect(0,0,-1,-1)
    property real  min : 0.0;
    property real  max : 1.0;
    property int   tickCount : 10;  
    property bool  vertical : false;  
         
    property real headRsvdPx : 0;
    property real tailRsvdPx : 0;

    property bool showHeadLine : true;
    property bool showTailLine : true;
}