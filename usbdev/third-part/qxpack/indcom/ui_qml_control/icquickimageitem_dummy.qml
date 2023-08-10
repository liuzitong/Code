import QtQuick 2.9
import QtQml   2.2
import QtQuick.Controls 2.2

// ============================================================================
// below code are only for designer syntax
// ============================================================================
Item {
    implicitWidth: 64; implicitHeight: 64;

    property var   imageData : null;
    property bool  flipHorizontal : false;
    property bool  flipVertical   : false;
    property size  textureSize    : Qt.size(0,0);

    function mapImagePointToItemPoint( image_point ) { return Qt.point(0,0); }
    function mapItemPointToImagePoint( item_point  ) { return Qt.point(0,0); }

}