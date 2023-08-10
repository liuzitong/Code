import QtQuick 2.9
import QtQml   2.2
import QtQuick.Controls 2.2

// ============================================================================
// below code are only for designer syntax
// ============================================================================
Item {
    implicitWidth: 64; implicitHeight: 64;

    property var   xySeriesData : null;
    property var   arySeriesData : null;
    property float lineSize : 0;
    property color lineColor : "#000000"
    property rect  viewport : Qt.rect(0,0,-1,-1)

    function makePickInfoObjAt( pt, mode ) { }
    function mapValuePointToPxPos( pt ) { return Qt.point(0,0); }
}