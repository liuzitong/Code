import QtQuick 2.0

Item{
    id:root;
    anchors.fill: parent;anchors.margins: parent.height*0.03;
    property alias range: displayCanvas.degreeRange;
    property alias displayCanvas: displayCanvas;
    property var currentProgram: null;
    property var currentCheckResult:null;
    property int clickedDotIndex: -1;
    property alias testOver:displayCanvas.testOver;

//    function testOver() {displayCanvas.testOver=true;displayCanvas.requestPaint();}

    onCurrentProgramChanged: displayCanvas.requestPaint();

    onCurrentCheckResultChanged: {
//        clickedDotIndex=-1;
        displayCanvas.requestPaint();
    }

    signal painted();
    antialiasing: true

    Canvas{
        id:displayCanvas;
        property int degreeRange: currentProgram.type!==2?currentProgram.params.commonParams.Range[1]:currentProgram.params.Range[1];
        anchors.fill: parent;
        property double diameter: height-heightMargin*2;
        property double widthMargin: (width-diameter)/2;
        property double heightMargin:height*0.015;
        property int fontSize: diameter*0.022;
        property bool testOver: true;
        smooth: false;
        MouseArea{
            anchors.fill: parent;
            onClicked:{
                if(currentCheckResult===null) return;
                var dotClicked=displayCanvas.pixCoordToDot({x:mouseX,y:mouseY});
                var dotList=currentProgram.data.dots;
                var dist=Math.pow(10,6);
                var index;
                for(var i=0;i<dotList.length;i++)
                {
                    var dot=dotList[i];
                    var temp=Math.pow(dot.x-dotClicked.x,2)+Math.pow(dot.y-dotClicked.y,2)
                    if(temp<dist)
                    {
                        dist=temp;
                        index=i;
                    }
                }
                clickedDotIndex=index;
                console.log(clickedDotIndex);
                displayCanvas.requestPaint();
            }
        }

        function drawDashCircle(x, y, radius, length)
        {
            var step=length/radius
            for (var b = 0, e = step ; e <=Math.PI*2; b += step*2, e += step*2)
            {
                 var ctx = getContext("2d")
                ctx.lineWidth = 0;
                ctx.strokeStyle = "black";
                ctx.fillStyle="white";
                ctx.beginPath()
                ctx.arc(x, y, radius, b, e);
                ctx.stroke();
                ctx.closePath();
            }
        }

        function drawDashLine(pointBegin,radius,angle,length)
        {
            var ctx = getContext("2d")
            ctx.strokeStyle="black";
            ctx.lineWidth=1;

            for(var loc=0;loc<radius;loc+=length*2)
            {
                ctx.beginPath();
                ctx.moveTo(pointBegin.x+loc*Math.cos(angle),pointBegin.y+loc*Math.sin(angle));
                if(loc+length>radius)
                {
                    ctx.lineTo(pointBegin.x+radius*Math.cos(angle),pointBegin.y+radius*Math.sin(angle));
                }
                else
                {
                    ctx.lineTo(pointBegin.x+(loc+length)*Math.cos(angle),pointBegin.y+(loc+length)*Math.sin(angle));
                }
                ctx.closePath();
                ctx.stroke();
            }
        }

        function drawText(content,x_pix,y_pix)
        {
            var ctx = getContext("2d");
            ctx.textAlign = "center";
            ctx.font = fontSize.toString()+"px sans-serif";
            ctx.fillStyle="white";
            ctx.fillRect(x_pix-fontSize*0.7,y_pix-fontSize*0.6,fontSize*1.4,fontSize*1.2)
            ctx.fillStyle="black";
            ctx.fillText(content, x_pix, y_pix+fontSize*1/3);
        }

        function pixCoordToDot(pix)
        {
            var pix_coordX=pix.x-width/2;
            var pix_coordY=-(pix.y-height/2);
            var dot_x=Math.round(pix_coordX/(diameter/2)*degreeRange);
            var dot_y=Math.round(pix_coordY/(diameter/2)*degreeRange);
            return {x:dot_x,y:dot_y};
        }

        function dotToPixCoord(dot)
        {
            var pix_coordX=(diameter/2)/degreeRange*dot.x;
            var pix_coordY=(diameter/2)/degreeRange*dot.y;
            var pix_x=pix_coordX+width/2;
            var pix_y=-pix_coordY+height/2;
            return {x:pix_x,y:pix_y};
        }

        function polarToOrth(dot)
        {
            var radius=dot.x;
            var angle=dot.y;
            return {x:radius*Math.cos(angle/180*Math.PI),y:radius*Math.sin(angle/180*Math.PI)}
        }

        function orthToPolar(dot)
        {
            var radius=Math.sqrt(Math.pow(dot.x,2)+Math.pow(dot.y,2));
            var rad=Math.asin(dot.y/radius);
            var angle=rad*(180/Math.PI);
            if(dot.x<0)
            {
                if(dot.y>=0){angle=90+(90-angle)}
                if(dot.y<0){angle=-90-(90+angle)}
            }
            if(angle<0) angle+=360;
            return {x:radius,y:angle}
        }

        function drawDot(dot)
        {
            var orthCoord;
            if(currentProgram.type===2) {orthCoord=polarToOrth(dot)}else{orthCoord=dot}

            var x_pix=(orthCoord.x/degreeRange)*(diameter*0.5)+width/2;
            var y_pix=(-orthCoord.y/degreeRange)*(diameter*0.5)+height/2;

            var dotRadius=diameter/180*1;
            var ctx = getContext("2d");
            ctx.lineWidth = 0;
            ctx.strokeStyle = "black";
            ctx.beginPath();
            ctx.arc(x_pix, y_pix, dotRadius, 0, Math.PI*2);
            ctx.stroke();
            ctx.closePath();
        }

        function drawDB(db,dot)
        {
            var x_pix=(dot.x/degreeRange)*(diameter*0.5)+width/2;
            var y_pix=(-dot.y/degreeRange)*(diameter*0.5)+height/2;
            drawText(db,x_pix,y_pix);
        }

        function drawAxisEndText(string,x_pix,y_pix,size)
        {
            var ctx = getContext("2d")
            drawText(string,x_pix,y_pix,size);
            ctx.strokeStyle="black";
            ctx.lineWidth=1;
            ctx.beginPath();
            ctx.moveTo(x_pix-size*0.7,y_pix-size*0.6);
            ctx.lineTo(x_pix-size*0.7,y_pix+size*0.6);
            ctx.closePath();
            ctx.stroke();
            ctx.moveTo(x_pix+size*0.7,y_pix-size*0.6);
            ctx.lineTo(x_pix+size*0.7,y_pix+size*0.6);
            ctx.closePath();
            ctx.stroke();
        }

        onPaint:
        {
            if(degreeRange==0) return;
            var ctx = getContext("2d")
            ctx.fillStyle = "#cbced0";
            ctx.fillRect(0, 0, width, height);
            var degreeStart=degreeRange%Math.ceil(degreeRange/3);
            var degreeStep;
            if(degreeStart==0){degreeStart=degreeStep=degreeRange/3;}
            else{degreeStep=(degreeRange-degreeStart)/2;}

            var i;
            for(i=3;i>=1;i--)                   //画三个圈圈
            {
                if(i!==3)
                     drawDashCircle(width/2,height/2,(degreeStart+degreeStep*(i-1))/degreeRange*diameter/2, 3)
                else{
                    ctx.lineWidth = 0;
                    ctx.strokeStyle = "black";
                    ctx.fillStyle="white";
                    ctx.beginPath();
                    ctx.arc(width/2,height/2,diameter/6*i,0,Math.PI*2);
                    ctx.closePath();
                    ctx.stroke();
                    ctx.fill();
                }
            }

            ctx.beginPath();                                    //画正交轴线
            ctx.moveTo(widthMargin,height/2);
            ctx.lineTo(widthMargin+diameter,height/2);
            ctx.closePath();
            ctx.stroke();
            ctx.beginPath();
            ctx.moveTo(width/2,heightMargin);
            ctx.lineTo(width/2,heightMargin+diameter);
            ctx.closePath();
            ctx.stroke();

            if(currentProgram.type===2)                         //移动投射画中心点放射线
            {
                for(i=1;i<12;i++)
                {
                    if(i%3==0) continue;
                    else
                    {
                        drawDashLine({x:width/2,y:height/2},diameter/2,Math.PI/6*i,3)
                    }
                }
            }

            for(i=-3;i<=3;i++)                                  //画轴线角度标记
            {
                if(i!==0)
                {
                    if(Math.abs(i)===3)
                    {
                        drawAxisEndText(degreeRange,widthMargin+(i+3)*diameter/6,height/2,fontSize);
                        drawAxisEndText(degreeRange,width/2,heightMargin+(i+3)*diameter/6,fontSize);
                    }
                    else
                    {
                        var degree=degreeStart+degreeStep*(Math.abs(i)-1);
                        var pix=degree/degreeRange*diameter/2;
                        if(i<-0) pix=-pix;
                        drawText(degree,width/2+pix,height/2,fontSize);
                        drawText(degree,width/2,height/2+pix,fontSize);
                    }
                }
            }

            if(root.currentCheckResult==null)                       //结果为空的时候按照程序画圆点
            {
                currentProgram.data.dots.forEach(function(item)
                {
                    drawDot(item);
                })
            }
            else
            {
                if(currentProgram.type!==2)                                     //静态是结果-1(表示没测到)画点,其它画值
                {
                    var dBList=currentCheckResult.resultData.checkData;
                    var dotList=currentProgram.data.dots;
                    for(i=0;i<dotList.length;i++)
                    {
                        if(dBList[i]===-1)
                            drawDot(dotList[i]);
                        else
                            drawDB(dBList[i],dotList[i]);
                    }
                }
                else                                                        //绘制动态检查结果
                {
                    dotList=currentCheckResult.resultData.checkData;
                    var dotRadius=diameter/180*1;

//                    ctx.beginPath();
//                    ctx.moveTo(100,100);
//                    ctx.lineTo(200,200);
//                    ctx.closePath();
//                    ctx.stroke();

//                    console.log(dotList[0].end.x);
                    for(i=0;i<dotList.length;i++)                               //画点
                    {
                        var dot=dotToPixCoord(polarToOrth(dotList[i].end));
                        ctx.lineWidth = 0;
                        ctx.strokeStyle = "black";
                        ctx.beginPath();
                        ctx.arc(dot.x, dot.y, dotRadius, 0, Math.PI*2);
                        ctx.stroke();
                        ctx.fill();
                        ctx.closePath();
                    }

                    for(i=1;i<dotList.length-1;i++)                                 //连线
                    {
                        var dot_Begin=dotToPixCoord(polarToOrth(dotList[i].end));
                        var dot_End=dotToPixCoord(polarToOrth(dotList[i+1].end));
                        console.log(dot_Begin.x+" "+dot_Begin.y);
                        console.log(dot_End.x+" "+dot_End.y);
                        ctx.beginPath();
                        ctx.moveTo(dot_Begin.x,dot_Begin.y);
                        ctx.lineTo(dot_End.x,dot_End.y);
                        ctx.closePath();
                        ctx.stroke();
                    }

                    if(testOver===true)                                                 //检查完之后连接
                    {
                        dot_Begin=dotToPixCoord(polarToOrth(dotList[dotList.length-1].end));
                        dot_End=dotToPixCoord(polarToOrth(dotList[0].end));
                        ctx.beginPath();
                        ctx.moveTo(dot_Begin.x,dot_Begin.y);
                        ctx.lineTo(dot_End.x,dot_End.y);
                        ctx.closePath();
                        ctx.stroke();
                    }
                }
            }
            if(currentProgram.type!==2&clickedDotIndex!=-1)                                     //选择点--实时图片用
            {
                var clickedDot=currentProgram.data.dots[clickedDotIndex];
                console.log(clickedDot);
                var pixLoc=dotToPixCoord(clickedDot);
                console.log(pixLoc.x+","+pixLoc.y);
                ctx.lineWidth = 1;
                ctx.strokeStyle = "blue";
                ctx.beginPath();
                ctx.arc(pixLoc.x, pixLoc.y, diameter/40, 0, Math.PI*2);
                ctx.closePath();
                ctx.stroke();
                root.painted();
            }
        }
    }
}

/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/
