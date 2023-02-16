import QtQuick 2.0
import QtQuick.Controls 1.0
ScrollView
{
    anchors.fill: parent;
    property alias dots: canvas.dots;
    Canvas{
        id:canvas;
        antialiasing: false;
        smooth: false;
        height: 27;
        width: 322;

        property var dots:null;
        onDotsChanged: requestPaint();

        function convertY(p){ return 13-p;}

        function drawBackGround(){
            var ctx = getContext("2d")
            ctx.clearRect(0,0,322,27);
            ctx.strokeStyle="black";
            ctx.lineWidth=0;
            ctx.beginPath();
            ctx.moveTo(0,0);
            ctx.lineTo(0,27);
            ctx.stroke();
            ctx.moveTo(width-1,0);
            ctx.lineTo(width-1,27)
            ctx.stroke();
            ctx.moveTo(0,13);
            ctx.lineTo(width,13)
            ctx.stroke();
        }

        onPaint:
        {
            drawBackGround()
            if(dots!==null)
            {
                var ctx = getContext("2d")
                for(var i=0;i<dots.length;i++)
                {
                    var temp=dots[i];
                    temp=convertY(temp)
        //            console.log(temp);
                    ctx.strokeStyle="black";
                    ctx.lineWidth=0;
                    ctx.beginPath();
                    ctx.moveTo(i+1,13);
                    ctx.lineTo(i+1,temp);
                    ctx.closePath();
                    ctx.stroke();
                }
            }
        }
    }

}


