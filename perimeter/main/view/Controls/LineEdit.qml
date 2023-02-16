import QtQuick 2.0
import QtQuick.Controls 2.0
import perimeter.main.view.Utils 1.0

Item{
    id:root
    property int radius:height/6;
    property alias text:textInput.text;
    property alias verticalAlignment: textInput.verticalAlignment;
    property alias horizontalAlignment: textInput.horizontalAlignment;
    property alias readOnly: textInput.readOnly;
    property alias backgroundColor: recbackground.color;
    property alias textInput: textInput;
    width: parent.width;
    height:parent.height;
    clip: true
    signal enterPressed();
    Rectangle {
        anchors.fill: parent;
        id:recbackground;border.width: 1;radius: root.radius;color:readOnly? "#e4e6e8":"white" ;border.color: "#bdc0c6"
        TextInput{
            id:textInput;
            anchors.fill: parent;
            anchors.leftMargin:horizontalAlignment==Text.AlignHCenter?0:10;
            text:"";
            renderType: Text.NativeRendering
            verticalAlignment: Text.AlignVCenter
            font.pointSize: CommonSettings.fontPointSize
            font.family:"Microsoft YaHei"
            selectByMouse: true
            selectionColor: "blue"
            Keys.onPressed: { switch (event.key){/*EnterKey*/ case 16777220:root.enterPressed();break;};
            }
        }
    }
}
