import QtQuick 2.6
import QtQuick.Controls 2.2
import perimeter.main.view.Utils 1.0

ComboBox {
    id: control
    model: ["First","Second","Third"]
    property string borderColor:"#bdc0c6"
    property string borderColorPressed:"Brown"
    property string imageSrc: "qrc:/Pics/base-svg/btn_drop_down.svg";
    property string backgroundColor:enabled? CommonSettings.buttonBackGroundColor: CommonSettings.buttonDisabledBackGroundColor
    font.family:"Microsoft YaHei"
    font.pointSize:height*0.30;
    height: parent.height;
    property bool complexType: false;
    property bool popDirectionDown: true;
    property string text: "";



    //    currentIndex: 0



    delegate: ItemDelegate {
        width: control.width
        height: control.height
        contentItem:
//        Item{
            Text
            {
                text: complexType?name:modelData;
                color: CommonSettings.buttonTextColor;
                font: control.font
                elide: Text.ElideRight
                verticalAlignment: Text.AlignVCenter
                renderType: Text.NativeRendering
            }
//            Rectangle
//            {
//                anchors.fill: parent;
//                color: "red";
//            }
//        }
        highlighted: control.highlightedIndex == index
    }

    indicator:
        Image {
            id: indicatorImage
            width: control.height*0.56
            height: control.height*0.28
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            source: imageSrc
            anchors.rightMargin: 10
            rotation:popDirectionDown? 0:180;
        }

    contentItem: Text {
        leftPadding: control.height*0.5
        rightPadding: control.indicator.width + control.spacing
        text: control.text!==""?control.text:complexType?model.get(currentIndex).name:displayText;
        color: enabled?CommonSettings.buttonTextColor:CommonSettings.buttonDisabledTextColor;
        font: control.font
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        renderType: Text.NativeRendering
    }

    background: Rectangle {
        implicitWidth: control.width
        implicitHeight: control.height
        border.color: control.pressed ? "blue" : control.borderColor
        border.width: control.visualFocus ? 2 : 1
        radius: height/6
        color:backgroundColor;
    }

    popup: Popup {
        y: control.height - 1
        width: control.width
        implicitHeight: listview.contentHeight+(listview.count-1)*padding+3;
        padding: 2;

        contentItem: ListView {
            id: listview
            clip: true
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex
            ScrollIndicator.vertical: ScrollIndicator { }
        }

        background: Rectangle {
            border.color: "grey";
            radius:control.height/6;
        }

    }
}





/*##^##
Designer {
    D{i:0;formeditorZoom:3}
}
##^##*/
