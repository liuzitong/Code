import QtQuick 2.9
import QtQml   2.2
import QtQuick.Controls 2.2

// ============================================================================
// below code are only for designer syntax
// ============================================================================
Item {
    implicitWidth: 64; implicitHeight: 64;

    property real  lineSize : 0;
    property color lineColor : "#000000"
    property rect  viewport : Qt.rect(0,0,-1,-1)

    property IcQuickValueAxisItem.TickDirection  tickDirection : IcQuickValueAxisItem.TickDirection_Both;
    property bool  vertical : false;       

    property real  min : 0.0;
    property real  max : 1.0;
    property real  blnMin : 0.0;
    property real  blnMax : 1.0;
    
    property int   majorTickCount : 2;
    property int   minorTickCount : 0;
    property int   minorHintStep  : 0;
    property real majorSize : 8;
    property real minorSize : 3;
    property real minorHintSize : 5;


    property bool  dispStartTickLabel : true; 
    property bool  dispMinorTickLabel : false;
    property bool  dispMinorHintLabel : false;
    property font  labelFont : Qt.font( { family:'Arial', weight: Font.Normal, pixelSize:10 } );
    property color labelColor: Qt.color("#000000");
    property int   labelPrecision : 2
    property int   labelDistOfv : 0

    property real headRsvdPx : 0;
    property real tailRsvdPx : 0;

    property bool labelVisible : true;
}