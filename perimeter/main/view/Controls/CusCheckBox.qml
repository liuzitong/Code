import QtQuick 2.0
import QtQml 2.2
Item{
    id:root
    height: parent.height;
    width: height;
    property bool checked: false;
    property string imageSrc:"qrc:/Pics/base-svg/btn_checkbox_1normal.svg";
    property string hoverImageSrc: "qrc:/Pics/base-svg/btn_checkbox_2hover.svg";
    property string pressImageSrc: "qrc:/Pics/base-svg/btn_checkbox_3press.svg";
    property string backGroundColor:enabled? "#f0f2f3":"#989898";
    signal clicked;

    Image {
        id: image
        source: checked?"qrc:/Pics/base-svg/btn_checkbox_3press.svg":"qrc:/Pics/base-svg/btn_checkbox_1normal.svg";
        anchors.fill: parent;
        Rectangle{id:rec;anchors.fill: parent;color:backGroundColor;z:-1;radius:parent.height/5;}
    }

    MouseArea{
        hoverEnabled: true;
        anchors.fill: parent
        onClicked:{if(checked) {image.source=imageSrc}else{image.source=pressImageSrc;} checked=!checked;root.clicked();}
        onEntered:{if(!checked){image.source=hoverImageSrc;} rec.color="white";}
        onExited:{if(!checked)/* image.source=pressImageSrc:*/image.source=imageSrc;rec.color=backGroundColor;}
    }
}


/*##^##
Designer {
    D{i:0;formeditorZoom:0.33}
}
##^##*/
//import QtQuick 2.0
//import QtQml 2.2
//Item{
//    id:root
//    property alias buttonColor: rec.color;
//    property string hoverBorderColor: "black"
//    property string pressBorderColor: "#006486"
//    property string commonBorderColor: "#bdc0c6"
//    property string borderColor:commonBorderColor
//    property bool hoverEnabled: true
//    property string text
//    property int fontSize:height/3
//    property int radius:height/6
//    property int borderWidth:1
//    property string hoverColor: "white"
//    property string imageSrc:""
//    property string hoverImageSrc:""
//    property string pressImageSrc: ""
//    property double imageHightScale: 0.6
//    property bool isAnime: true
//    property int animationDuration: 150
//    property alias image: image;
//    property alias rec: rec;
//    property alias underImageText: imageText;
//    property string type;
//    height:parent.height;/*300*/
//    width:parent.height*2.5;/*100*/
//    signal clicked()
//    signal entered()
//    signal exited()
//    onEnabledChanged: {
//        enabled?buttonColor="#dcdee0":buttonColor="#787878";
//    }

//    MouseArea{
//        property string tempColor;
//        anchors.fill: parent
//        hoverEnabled: root.hoverEnabled
//        onClicked:{ root.clicked();}
//        onPressed:{if(pressImageSrc!="") image.source=pressImageSrc;}
//        onEntered:{
//            if(root.hoverEnabled) {
//                tempColor=rec.color;
//                rec.color=root.hoverColor;if(hoverImageSrc!="") image.source=hoverImageSrc;root.entered();}}
//        onExited:{if(root.hoverEnabled) {rec.color=tempColor;if(hoverImageSrc!="") image.source=imageSrc;root.exited();}}
//        onReleased: {if(root.type=="click"){image.source=imageSrc;}}
//    }
//    Item{
//        anchors.horizontalCenter: parent.horizontalCenter
//        width: parent.height;
//        height: image.height+imageText.height
//        anchors.verticalCenter: parent.verticalCenter
//        Column{
//            id: column
//            anchors.fill: parent;
//            Image {
//                id: image
//                source: imageSrc
//                height: root.height*root.imageHightScale
//                anchors.horizontalCenter: parent.horizontalCenter
//                width: sourceSize.width*(height/sourceSize.height);

//            }
//            Text {
//                id:imageText
//                width: parent.width;
//                anchors.horizontalCenter: parent.horizontalCenter
//                font.pointSize: root.fontSize
//                fontSizeMode: Text.Fit
//                verticalAlignment: Text.AlignVCenter
//                horizontalAlignment: Text.AlignHCenter
//                font.family: "Microsoft YaHei"
//            }
//            Component.onCompleted: {if(imageText.text=="") imageText.height=0;}
//        }
//    }




//    Item{
//        id: row
//        anchors.fill: parent
//        Text {
//            text: root.text
//            font.pointSize: root.fontSize
//            fontSizeMode: Text.Fit
//            verticalAlignment: Text.AlignVCenter
//            horizontalAlignment: Text.AlignHCenter
//            anchors.fill: parent
//            font.family: "Microsoft YaHei"
//        }
//    }

//    Rectangle{
//        id:rec
//        width:parent.width
//        height:parent.height
//        color: enabled?"#dcdee0":"#787878"
//        radius: root.radius
//        border.color: root.borderColor
//        border.width: root.borderWidth
//        z:-1

//        PropertyAnimation
//        {
//            id:anime
//            target: rec
//            properties: "opacity";
//            to: 0.5
//            duration: animationDuration
//            onStopped: {
//                rec.opacity=1.0;
//                root.clicked();
//            }
//        }

//    }

//}


/*##^##
Designer {
    D{i:0;formeditorZoom:0.33}
}
##^##*/
