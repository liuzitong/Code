import QtQuick 2.6
import QtQuick.Controls 2.2

Item {
    id:root;
    height:20;width: 80
    property alias comboBox: comboBox;
    property alias button:button;
    property string borderColor:"#bdc0c6"
    property string borderColorPressed:"Brown"
    property string imageSrc: "qrc:/Pics/base-svg/btn_drop_down.svg";
    property string backgroundColor:enabled?"#dcdee0":"#a0a0a0"
    property bool complexType: false;
    property bool popDirectionDown: true;

//    signal activated();
//    signal currentIndexChanged();
    Row
    {
        anchors.fill: parent;
        CusButton
        {
            id:button;
            height: parent.height;
            width: parent.width-parent.height;
        }



        ComboBox {
            id: comboBox
            model: ["First","Second","Third"]
            font.family:"Microsoft YaHei"
            font.pointSize:height*0.30;
            height: parent.height;
            width: parent.height;

            contentItem: Item {}
//            onActivated:{console.log("haa");root.activated;}
//            onCurrentIndexChanged:root.currentIndexChanged;

            delegate: ItemDelegate {
                width: root.width
                height: comboBox.height
                contentItem: Text {
                    text: root.complexType?name:modelData;
                    font: comboBox.font
                    elide: Text.ElideRight
                    verticalAlignment: Text.AlignVCenter
                    renderType: Text.NativeRendering
                }
                highlighted: comboBox.highlightedIndex == index
            }

            indicator:
                Image {
                    id: indicatorImage
                    source: root.imageSrc
                    rotation: 180;
                    width: root.height*0.56
                    height: root.height*0.28
                    fillMode: Image.PreserveAspectFit
                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.horizontalCenter: parent.horizontalCenter;

                }

            background: Rectangle {
                implicitWidth: root.width
                implicitHeight: root.height
                border.color: root.pressed ? "blue" : root.borderColor
                border.width: root.visualFocus ? 2 : 1
                radius: height/6
                color:root.backgroundColor;
            }

            popup: Popup {
                x: comboBox.width-root.width;
                y: comboBox.height - 1
                width: root.width
                implicitHeight: listview.contentHeight+(listview.count-1)*padding+3;
                padding: 2;

                contentItem: ListView {
                    id: listview
                    clip: true
                    model: comboBox.popup.visible ? comboBox.delegateModel : null
                    currentIndex: comboBox.highlightedIndex
                    ScrollIndicator.vertical: ScrollIndicator { }
                }

                background: Rectangle {
                    border.color: "grey"
                    radius:comboBox.height/6
                }
            }
        }
    }


    /*##^##
    Designer {
        D{i:0;formeditorZoom:3}
    }
    ##^##*/

}
