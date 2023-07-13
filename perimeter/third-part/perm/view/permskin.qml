pragma Singleton
import QtQuick 2.7

Item {

         property bool   isFullScreen    : false;
         property int    winRectWidth    : (isFullScreen? 1920 : 1280);
         property int    winRectHeight   : (isFullScreen? 1080 : 800);
readonly property double winWidthScale   : winRectWidth/1920.0
readonly property double winHeightScale  : winRectHeight/1080.0



}
