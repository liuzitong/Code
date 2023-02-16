import qxpack.indcom.ui_qml_base 1.0

import QtQuick 2.0

//已废弃
Item{
    property var degLocs:[];
    property var values:[];
    property int type:null;                                 //0 画数字 1画灰度图  2画概率 3画点
    property int os_od: null;
    property int range: null;
    property double lengthPerDeg: picLength/(range*2);
    property int fontSize: width*0.055;
    property int picMargin: width*0.05;
    property int picLength: width-picMargin*2;
    property int singlePicLength: picLength/10;
    property var contentData:null;

    property alias canvas: contentCanvas;

    MouseArea
    {
        anchors.fill: parent;
        onClicked:
        {
            IcUiQmlApi.appCtrl.testClass.hello();
//            contentCanvas.requestPaint();
            var ar = contentCanvas.context.getImageData(0,0,contentCanvas.width,contentCanvas.height)
            var len = ar.data.length;
            var data = ar.data;



            contentData=ar;

            IcUiQmlApi.appCtrl.testClass.testObj(data);

//            console.log(len);
//             for(var i = 0;i< 64; i++)
//             {
////                  var x = i/4 % canvas.width;
////                  var y = i/4 / canvas.width;
//                 console.log(data[i]);
//             }
        }
    }

    Canvas {
        id:contentCanvas;
        anchors.fill: parent;
//        antialiasing: false
        smooth: false
        Component.onCompleted: {
            var i;
            if(type==1)
            {
                for(i=0;i<degLocs.length;i++)
                {
//                    var m_picturePath="qrc:/grays/Gray6.bmp";
                    var m_picturePath= "qrc:/grays/Gray"+values[i]+".bmp";
//                    console.log(m_picturePath);
                    loadImage(m_picturePath);
                }
            }
        }
//        onImageLoaded: requestPaint();

        function convertDegLocToPixLoc(point)
        {
            return {x:width/2+point.x*lengthPerDeg,y:height/2-point.y*lengthPerDeg};
        }

        function drawText(content,x_pix,y_pix)
        {
            var ctx = getContext("2d");
            ctx.textAlign = "center";
            ctx.font = fontSize.toString()+"px Consolas";
//             ctx.font = "14px sans-serif";
//            ctx.font = fontSize.toString()+"px Microsoft YaHei";
            ctx.fillStyle="black";
            ctx.fillText(content, x_pix-fontSize*1/6, y_pix+fontSize*1/3);
        }


        onPaint:
        {
//            antialiasing= false
//            smooth= false
            var ctx = getContext("2d")
            var i,j;

            switch (type)
            {
                case 0:
                    for(i=0;i<degLocs.length;i++)
                    {
                        var pixLoc = convertDegLocToPixLoc(degLocs[i])
//                        console.log(pixLoc.x+" "+pixLoc.y);
                        drawText(values[i],pixLoc.x,pixLoc.y)
                    }
                    break;
                case 1:
                    console.log("************************************");
                    console.log("************************************");
                    console.log("************************************");
//                    var m_picturePath="qrc:/grays/Gray6.bmp";
                    for(i=0;i<degLocs.length;i++)
                    {

                        pixLoc = convertDegLocToPixLoc(degLocs[i])
                        var m_picturePath= "qrc:/grays/Gray"+values[i]+".bmp";
                        var imageData = context.createImageData(m_picturePath);
                        var imge = ctx.createPattern(imageData, "repeat")
                        ctx.fillStyle = imge;
                        ctx.beginPath()
                        ctx.moveTo(pixLoc.x-singlePicLength/2,pixLoc.y-singlePicLength/2)
                        ctx.lineTo(pixLoc.x+singlePicLength/2,pixLoc.y-singlePicLength/2)
                        ctx.lineTo(pixLoc.x+singlePicLength/2,pixLoc.y+singlePicLength/2)
                        ctx.lineTo(pixLoc.x-singlePicLength/2,pixLoc.y+singlePicLength/2)
                        ctx.closePath()
                        ctx.fill()
                    }
                    break;
            }



            ctx.strokeStyle="black";
            ctx.lineWidth=0;
            ctx.beginPath();
            ctx.moveTo(picMargin,(height-1)/2);
            ctx.lineTo(picMargin+picLength-1,(height-1)/2)
            ctx.moveTo((width-1)/2,picMargin);
            ctx.lineTo((width-1)/2,picMargin+picLength-1);


            for( i=0;i<=6;i++)
            {
                if(i===3) continue;
                var temp=Math.round(picMargin+(picLength-1)/6*i);
                ctx.moveTo((width-1)/2-width/90,temp);
                ctx.lineTo((width-1)/2+width/90,temp);
                ctx.moveTo(temp,(height-1)/2-height/90);
                ctx.lineTo(temp,(height-1)/2+height/90);
            }
            ctx.stroke();

        }

    }
//    Canvas {
////        antialiasing: false
////        smooth: false
//        anchors.fill: parent;

//        onPaint: {
//            var ctx = getContext("2d")
//            ctx.strokeStyle="black";
//            ctx.lineWidth=0;
//            ctx.beginPath();
//            ctx.moveTo(picMargin,(height-1)/2);
//            ctx.lineTo(picMargin+picLength-1,(height-1)/2)
//            ctx.moveTo((width-1)/2,picMargin);
//            ctx.lineTo((width-1)/2,picMargin+picLength-1);

//            var i;
//            for( i=0;i<=6;i++)
//            {
//                if(i===3) continue;
//                 var temp=Math.round(picMargin+(picLength-1)/6*i);
//                 ctx.moveTo((width-1)/2-width/60,temp);
//                 ctx.lineTo((width-1)/2+width/60,temp);
//            }


//            for( i=0;i<=6;i++)
//            {
//                if(i===3) continue;
//                ctx.moveTo(picMargin+(picLength-1)/6*i,(height-1)/2-height/60);
//                ctx.lineTo(picMargin+(picLength-1)/6*i,(height-1)/2+height/60);
//            }
//            ctx.stroke();

//        }
//    }
}


/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/
