import QtQuick 2.0
import qxpack.indcom.ui_qml_base 1.0
Item{
    id:root;
    anchors.fill: parent;anchors.margins: parent.height*0.03;
    property alias range: displayCanvas.degreeRange;
    property alias displayCanvas: displayCanvas;
    property var currentProgram: null;
    property var currentCheckResult:null;
    property int clickedDotIndex: -1;
    property alias testOver:displayCanvas.testOver;
    property int os_od: 0;
    property var dynamicSelectedDots:[];
    property var dynamicSelectedDotLen;
    property var dynamicCheckDots;
    property bool dynamicSelectedDotsReady:false;
    property int type;
    property var boundaries:IcUiQmlApi.appCtrl.utilitySvc.boundaries;
    property int boundaryShowRange:IcUiQmlApi.appCtrl.utilitySvc.boundaryShowRange;
    property int dynamicCircleRadius: 0;
    property int lastStrategy;
    signal clearResult();



    function resetInputDot()
    {
        type=currentProgram.type;
        if(type==2)
        {
            var strategy=currentProgram.params.strategy;
            if(lastStrategy!==strategy) dynamicSelectedDots=[];
            if(strategy===0)
            {
                dynamicSelectedDots.push({x:0,y:0});
                dynamicSelectedDotLen=1;
                dynamicSelectedDotsReady=true;
            }
            if(strategy===1)
            {
/*                if(os_od==0)*/ dynamicSelectedDots.push({x:-15,y:0});
//                else dynamicSelectedDots.push(displayCanvas.orthToPolar({x:-15,y:0}));
                dynamicSelectedDotLen=1;
                dynamicSelectedDotsReady=true;
            }
            else if(strategy===2)
            {
                dynamicSelectedDotLen=1;
                dynamicSelectedDotsReady=(dynamicSelectedDotLen===dynamicSelectedDots.length);
            }
            else if(strategy===3)
            {
                dynamicSelectedDotLen=2;
                dynamicSelectedDotsReady=(dynamicSelectedDotLen===dynamicSelectedDots.length);
            }
            lastStrategy=strategy;
        }
    }

    onCurrentProgramChanged:
    {
        dynamicCircleRadius=0;
        if(currentProgram.type===2&&(currentProgram.params.strategy===1||currentProgram.params.strategy===2))
        {
            if(currentProgram.params.dynamicDistance===0) dynamicCircleRadius=5;
            if(currentProgram.params.dynamicDistance===1) dynamicCircleRadius=10;
            if(currentProgram.params.dynamicDistance===2) dynamicCircleRadius=15;
        }
        resetInputDot();
        displayCanvas.requestPaint();
    }
    onOs_odChanged:
    {
//        resetInputDot();
        displayCanvas.requestPaint();
    }

    onCurrentCheckResultChanged: { displayCanvas.requestPaint();}

    signal painted();
    antialiasing: true

    CusText{id:dotPosDisplay;text:""; horizontalAlignment: Text.AlignLeft;z:1; anchors.top: parent.top; anchors.topMargin: 0.07*parent.height; anchors.left: parent.left; anchors.leftMargin: 0.03*parent.width;width: parent.width*0.06;height: parent.height*0.05;}

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
            hoverEnabled:range!==0;
            acceptedButtons: Qt.LeftButton | Qt.RightButton;
            onPositionChanged:
            {
                var dot;
                dot = displayCanvas.pixCoordToDot({x:mouseX,y:mouseY})
                if(type!==2)
                {
                    dotPosDisplay.text="x:"+Math.round(dot.x)+" y:"+Math.round(dot.y);
                }
                else
                {
                    dot=displayCanvas.orthToPolar(dot)
                    dotPosDisplay.text=qsTr("radius")+":"+Math.round(dot.x)+" "+qsTr("angle")+":"+Math.round(dot.y);
                }
            }
            onClicked:{
                if(type!==2)
                {
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
                    if(currentCheckResult.params.centerDotCheck)
                    {
                        if(Math.pow(dotClicked.x,2)+Math.pow(dotClicked.y,2)<dist)
                        {
                            clickedDotIndex=dotList.length*2;                                   //中心点
                        }
                    }
                    else
                    {
                        clickedDotIndex=index;                                             //其它
                    }

                    displayCanvas.requestPaint();
                }
                else
                {
                    dot = displayCanvas.pixCoordToDot({x:mouseX,y:mouseY});
                    if(os_od==1)
                    {
                        dot.x=-dot.x;
                    }
                    if (mouse.button === Qt.RightButton)
                    {
                        if(dynamicSelectedDots.length===0) return;
                        var distance=1000*1000;
                        var nearestDot;
                        dynamicSelectedDots.forEach(function(item){
                            var newDist=Math.pow(dot.x-item.x,2)+Math.pow(dot.y-item.y,2);
                            if (newDist<distance) {nearestDot=item;distance=newDist;}
                        })

                        for(i=0;i<dynamicSelectedDots.length;i++)
                        {
                            if(dynamicSelectedDots[i].x===nearestDot.x&&dynamicSelectedDots[i].y===nearestDot.y)
                            {
                                dynamicSelectedDots.splice(i,1);
                                break;
                            }
                        }
                        dynamicSelectedDotsReady=(dynamicSelectedDotLen===dynamicSelectedDots.length);
                        clearResult();
                        displayCanvas.requestPaint();
                    }
                    else{
                        if(dynamicSelectedDots.length>=dynamicSelectedDotLen)
                            return;
                        var strategy=currentProgram.params.strategy;
                        if(strategy===2)                                    //暗区在两个坐标系中任意一个都可
                        {
                            if(Math.pow(dot.x-boundaries[0].x,2)+Math.pow(dot.y-boundaries[0].y,2)>Math.pow((boundaries[0].radius-dynamicCircleRadius),2)
                             &&Math.pow(dot.x-boundaries[1].x,2)+Math.pow(dot.y-boundaries[1].y,2)>Math.pow((boundaries[1].radius-dynamicCircleRadius),2))
                                return;
                        }
                        else if(strategy===3)                               //直线必须在同一坐标系内
                        {
                            if(dynamicSelectedDots.length===0)
                            {
                                if(Math.pow(dot.x-boundaries[0].x,2)+Math.pow(dot.y-boundaries[0].y,2)>Math.pow(boundaries[0].radius,2)
                                 &&Math.pow(dot.x-boundaries[1].x,2)+Math.pow(dot.y-boundaries[1].y,2)>Math.pow(boundaries[1].radius,2))
                                    return;
                            }
                            else
                            {
                                var selectedDot=dynamicSelectedDots[0];
                                var zone1,zone2;
                                if(Math.pow(selectedDot.x-boundaries[0].x,2)+Math.pow(selectedDot.y-boundaries[0].y,2)<Math.pow(boundaries[0].radius,2)) zone1=true;
                                if(Math.pow(selectedDot.x-boundaries[1].x,2)+Math.pow(selectedDot.y-boundaries[1].y,2)<Math.pow(boundaries[1].radius,2)) zone2=true;
                                if(!((zone1&&Math.pow(dot.x-boundaries[0].x,2)+Math.pow(dot.y-boundaries[0].y,2)<Math.pow(boundaries[0].radius,2))
                                 ||(zone2&&Math.pow(dot.x-boundaries[1].x,2)+Math.pow(dot.y-boundaries[1].y,2)<Math.pow(boundaries[1].radius,2))))
                                    return;
                            }
                        }
                        dynamicSelectedDots.push(dot);
                        dynamicSelectedDotsReady=(dynamicSelectedDotLen===dynamicSelectedDots.length);
                        displayCanvas.requestPaint();
                    }
                }
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
            var dot_x=pix_coordX/(diameter/2)*degreeRange;
            var dot_y=pix_coordY/(diameter/2)*degreeRange;
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
            if(radius===0) return {x:0,y:0}
            var rad=Math.asin(dot.y/radius);
            var angle=rad*(180/Math.PI);
            if(dot.x<0)
            {
                if(dot.y>=0){angle=90+(90-angle);}
                if(dot.y<0){angle=-90-(90+angle);}
            }
            if(angle<0) angle+=360;
            return {x:radius,y:angle}
        }

//        function drawDot(dot)
//        {
//            var orthCoord;
//            var pixDot=dotToPixCoord(dot);
//            var dotRadius=diameter/180*1;
//            var ctx = getContext("2d");
//            ctx.lineWidth = 0;
//            ctx.strokeStyle = "black";
//            ctx.beginPath();
//            ctx.arc(pixDot.x, pixDot.y, dotRadius, 0, Math.PI*2);
//            ctx.stroke();
//            ctx.closePath();
//            ctx.fillStyle = "white";
//            ctx.fill();
//        }

        function drawDot(dot,color)
        {
            var orthCoord;
            var pixDot=dotToPixCoord(dot);
            var dotRadius=diameter/180*1;
            var ctx = getContext("2d");
            ctx.lineWidth = 0;
            ctx.strokeStyle = "black";
            ctx.beginPath();
            ctx.arc(pixDot.x, pixDot.y, dotRadius, 0, Math.PI*2);
            ctx.stroke();
            ctx.closePath();
            ctx.fillStyle = color;
            ctx.fill();
        }


        function dynamicInputDots(dot)
        {
            var x_pix=(dot.x/degreeRange)*(diameter*0.5)+width/2;
            var y_pix=(-dot.y/degreeRange)*(diameter*0.5)+height/2;
            var dotRadius=diameter/180*1;
            var ctx = getContext("2d");
            ctx.lineWidth = 0;
            ctx.strokeStyle = "red";
            ctx.beginPath();
            ctx.arc(x_pix, y_pix, dotRadius, 0, Math.PI*2);
            ctx.stroke();
            ctx.closePath();
            ctx.fillStyle = "green";
            ctx.fill();

        }


        function drawDB(db,dot)
        {
            var pixDot=dotToPixCoord(dot);
            drawText(db,pixDot.x,pixDot.y);
        }

        function drawShortFlucDB(db,dot)
        {
            var pixDot=dotToPixCoord(dot);
            if(db<-0) db="<0";
            if(db>51) db=">51";
            drawText("("+db+")",pixDot.x,pixDot.y+fontPointSize*1.5);
        }

        function drawUnseen(dot)
        {
            var pixDot=dotToPixCoord(dot);
            var ctx = getContext("2d");
            var recHeight=diameter/180*2;
            var recWidth=diameter/180*1.5;
            ctx.fillRect(pixDot.x-recWidth/2, pixDot.y-recHeight/2,recWidth, recHeight);

//            var pixDot=dotToPixCoord(dot);
//            var dotRadius=diameter/180*2;
//            var ctx = getContext("2d");
//            ctx.lineWidth = 0;
//            ctx.strokeStyle = "black";
//            ctx.beginPath();
//            ctx.arc(pixDot.x, pixDot.y, dotRadius, 0, Math.PI*2);
//            ctx.stroke();
//            ctx.closePath();

        }

        function drawWeakSeen(dot)
        {
            var pixDot=dotToPixCoord(dot);
            var ctx = getContext("2d");
            var recHeight=diameter/180*1.72;
            var recWidth=diameter/180*1.72;
            ctx.lineWidth = 0;
            ctx.beginPath();
            ctx.moveTo(pixDot.x-recWidth/2, pixDot.y-recHeight/2);
            ctx.lineTo(pixDot.x+recWidth/2, pixDot.y+recHeight/2);
            ctx.moveTo(pixDot.x-recWidth/2, pixDot.y+recHeight/2);
            ctx.lineTo(pixDot.x+recWidth/2, pixDot.y-recHeight/2);
            ctx.stroke();
            ctx.closePath();
        }

        function drawSeen(dot)
        {
            var pixDot=dotToPixCoord(dot);
            var dotRadius=diameter/180*3;
            var ctx = getContext("2d");
            var recHeight=diameter/180*2;
            var recWidth=diameter/180*1.5;
            ctx.lineWidth = 0;
            ctx.strokeStyle = "black";
            ctx.beginPath();
            ctx.rect(pixDot.x-recWidth/2, pixDot.y-recHeight/2,recWidth, recHeight);
            ctx.stroke();
            ctx.closePath();
        }

        function drawShortFlucUnseen(dot)
        {
            var pixDot=dotToPixCoord(dot);
            drawText("(  )",pixDot.x,pixDot.y+fontPointSize*1.5);
            var ctx = getContext("2d");
            var recHeight=diameter/180*2;
            var recWidth=diameter/180*1.5;
            ctx.lineWidth = 0;
            ctx.beginPath();
            ctx.fillRect(pixDot.x-recWidth/2, pixDot.y-recHeight/2+fontPointSize*1.5,recWidth, recHeight);
            ctx.closePath();

        }

        function drawShortFlucWeakSeen(dot,y_offsetPix)
        {
            var pixDot=dotToPixCoord(dot);
            var ctx = getContext("2d");
            var recHeight=diameter/180*1.72;
            var recWidth=diameter/180*1.72;
            drawText("(  )",pixDot.x,pixDot.y+fontPointSize*1.5);
            ctx.lineWidth = 0;
            ctx.beginPath();
            ctx.moveTo(pixDot.x-recWidth/2, pixDot.y-recHeight/2+fontPointSize*1.5);
            ctx.lineTo(pixDot.x+recWidth/2, pixDot.y+recHeight/2+fontPointSize*1.5);
            ctx.moveTo(pixDot.x-recWidth/2, pixDot.y+recHeight/2+fontPointSize*1.5);
            ctx.lineTo(pixDot.x+recWidth/2, pixDot.y-recHeight/2+fontPointSize*1.5);
            ctx.stroke();
            ctx.closePath();
        }

        function drawShortFlucSeen(dot)
        {
            var pixDot=dotToPixCoord(dot);
            var ctx = getContext("2d");
            var recHeight=diameter/180*2;
            var recWidth=diameter/180*1.5;
            drawText("(  )",pixDot.x,pixDot.y+fontPointSize*1.5);
            ctx.lineWidth = 0;
            ctx.strokeStyle = "black";
            ctx.beginPath();
            ctx.rect(pixDot.x-recWidth/2, pixDot.y-recHeight/2+fontPointSize*1.5,recWidth, recHeight);
            ctx.stroke();
            ctx.closePath();
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

        function drawBoundaries()
        {
            var ctx = getContext("2d");
            ctx.lineWidth = 0;
            ctx.strokeStyle = "green";
            ctx.beginPath();
            var x_pix=(boundaries[0].x/degreeRange)*(diameter*0.5)+width/2;
            var y_pix=(- boundaries[0].y/degreeRange)*(diameter*0.5)+height/2;
            var radius_pix=((boundaries[0].radius-dynamicCircleRadius)/degreeRange)*(diameter*0.5);
            ctx.arc(x_pix, y_pix,radius_pix, 0, Math.PI*2);
            ctx.stroke();
            ctx.closePath();

            x_pix=(boundaries[1].x/degreeRange)*(diameter*0.5)+width/2;
            y_pix=(- boundaries[1].y/degreeRange)*(diameter*0.5)+height/2;
            radius_pix=((boundaries[1].radius-dynamicCircleRadius)/degreeRange)*(diameter*0.5);
            ctx.strokeStyle = "blue";
            ctx.beginPath();
            ctx.arc(x_pix, y_pix,radius_pix, 0, Math.PI*2);
            ctx.stroke();
            ctx.closePath();
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
                if(currentProgram.params.strategy===2||currentProgram.params.strategy===3)
                {
                    drawBoundaries()                                //画坐标系圈圈
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

            var dBList;
            var dotList=currentProgram.data.dots;
            var inputdotList=new Array(dynamicSelectedDots.length);
            for(i=0;i<dynamicSelectedDots.length;i++)
            {
                inputdotList[i]={x:dynamicSelectedDots[i].x,y:dynamicSelectedDots[i].y};
            }


            if(root.currentCheckResult==null)                       //结果为空的时候按照程序画圆点
            {
                dotList.forEach(function(item)
                {
                    if(os_od==1){
                        item.x=-item.x;
                    }
                    drawDot(item,"white");
                })
                if(type==2)
                {
                    inputdotList.forEach(function(item)
                    {
                        if(os_od==1){item.x=-item.x;}
                        dynamicInputDots(item);
                    })
                    if(currentProgram.params.strategy===1||currentProgram.params.strategy===2)   //画周围放射点
                    {
                        var method=currentProgram.params.dynamicMethod;
                        var lines;
                        if(method===0) lines=4;
                        if(method===1) lines=6;
                        if(method===2) lines=8;

                        for(var i=0;i<lines;i++)
                        {
                            var angle=Math.PI*2/lines*i;
                            var selectedDot=dynamicSelectedDots[0];
                            var x=Math.cos(angle)*dynamicCircleRadius+selectedDot.x;
                            if(os_od==1) x=-x;
                            var y=Math.sin(angle)*dynamicCircleRadius+selectedDot.y;
                            drawDot({x:x,y:y},"white");
                        }
                    }
                }
            }
            else
            {

                if(currentProgram.type!==2&clickedDotIndex!=-1)                                     //选择点--实时图片用
                {
                    var clickedDot
                    if(clickedDotIndex<dotList.length)
                    {
                         clickedDot=currentProgram.data.dots[clickedDotIndex];
                    }
                    else if(clickedDotIndex==2*dotList.length)
                    {
                        clickedDot={x:0,y:0};
                    }
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

                if(currentProgram.type===0)                                     //阈值,静态是结果-1(表示没测到)画点,其它画值
                {
                    dBList=currentCheckResult.resultData.checkData;
                    console.log(dBList);
                    for(i=0;i<dBList.length;i++)
                    {
                        if(i<dotList.length)                                            //一般结果
                        {
                            console.log(dBList[i]);
                            if(dBList[i]===-999)
                                drawDot(dotList[i],"white");
                            else if(dBList[i]<0)
                                 drawText("<0",dotToPixCoord(dotList[i]).x,dotToPixCoord(dotList[i]).y)
                            else if(dBList[i]>51)
                                drawText(">51",dotToPixCoord(dotList[i]).x,dotToPixCoord(dotList[i]).y)
                            else
                                drawDB(dBList[i],dotList[i]);
                        }
                        else if(dotList.length<=i&&i<2*dotList.length)                                                            //短周期
                        {
                            if(dBList[i]!==-999) drawShortFlucDB(dBList[i],dotList[i-dotList.length]);
                        }
                        else if(i===dotList.length*2)                           //中心点
                        {

                            if(dBList[i]===-999)
                                drawDot(dotList[i],"white");
                            else if(dBList[i]<0)
                                drawText("<0",dotToPixCoord(dotList[i]).x,dotToPixCoord(dotList[i]).y)
                            else if(dBList[i]>51)
                                drawText(">51",dotToPixCoord(dotList[i]).x,dotToPixCoord(dotList[i]).y)
                            else  if(dBList[i]!==-1)
                                drawDB(dBList[i],dotList[i]);
                        }

                    }
                }
                else if(currentProgram.type===1)                                // 筛选
                {
                    dBList=currentCheckResult.resultData.checkData;
                    for(i=0;i<dBList.length;i++)
                    {
                        if(i<dotList.length)
                        {
                            switch (dBList[i])
                            {
                            case -999:drawDot(dotList[i],"white");break;
                            case 0:drawUnseen(dotList[i]);break;
                            case 1:drawWeakSeen(dotList[i]);break;
                            case 2:drawSeen(dotList[i]);break;
                            default:drawDB(dBList[i],dotList[i]);break;
                            }
                        }
                        else if(dotList.length<=i&&i<2*dotList.length)          //短周期
                        {
                            var y_offset=-1.5*fontPointSize;
                            switch (dBList[i])
                            {
                            case -999:;break;
                            case 0:drawShortFlucUnseen(dotList[i-dotList.length]);break;
                            case 1:drawShortFlucWeakSeen(dotList[i-dotList.length]);break;
                            case 2:drawShortFlucSeen(dotList[i-dotList.length]);break;
                            default:drawShortFlucDB(dBList[i],dotList[i-dotList.length]);break;
                            }
                        }
                        else if(i===dotList.length*2)                           //中心点
                        {
                            switch (dBList[i])
                            {
                            case -999:break;
                            case 0:drawUnseen({x:0,y:0});break;
                            case 1:drawWeakSeen({x:0,y:0});break;
                            case 2:drawSeen({x:0,y:0});break;
                            default:drawDB(dBList[i],{x:0,y:0});break;
                            }
                        }
                    }

                }
                else                                                        //动态
                {
//                    var dotRadius=diameter/180*1;

//                    ctx.beginPath();
//                    ctx.moveTo(100,100);
//                    ctx.lineTo(200,200);
//                    ctx.closePath();
//                    ctx.stroke();

//                    console.log(dotList[0].end.x);
                    inputdotList.forEach(function(item)
                    {
                        dynamicInputDots(item);
                    })

//                    console.log(currentCheckResult.type);
//                    console.log(currentCheckResult.program_id);

//                    console.log(currentCheckResult.resultData.checkData)

//                    console.log(currentCheckResult.resultData.checkData[0].name);
//                    console.log(currentCheckResult.resultData.checkData[0].start.x);
                    dotList=currentCheckResult.resultData.checkData;
                    if(currentProgram.params.strategy!==3)
                    {
                        for(i=0;i<dotList.length;i++)                                 //连线
                        {
                            if(dotList[i%dotList.length].isChecked)
                                var dot_Begin=dotToPixCoord(dotList[i%dotList.length].end);
                            else
                                dot_Begin=dotToPixCoord(dotList[i%dotList.length].start);
                            if(dotList[(i+1)%dotList.length].isChecked)
                                var dot_End=dotToPixCoord(dotList[(i+1)%dotList.length].end);
                            else
                                dot_End=dotToPixCoord(dotList[(i+1)%dotList.length].start);
                            ctx.beginPath();
                            ctx.moveTo(dot_Begin.x,dot_Begin.y);
                            ctx.lineTo(dot_End.x,dot_End.y);
                            ctx.closePath();
                            ctx.strokeStyle = "green";
                            ctx.stroke();
                        }

                        for(i=0;i<dotList.length;i++)                               //画点
                        {
                            if(dotList[i].isChecked)
                            {
                                if(dotList[i].isSeen)
                                     drawDot(dotList[i].end,"yellow");
                            }
                            else
                                drawDot(dotList[i].start,"blue");
                        }

                    }
                    else
                    {
                        if(dotList[0].isSeen&&dotList[1].isSeen)
                        {
                            ctx.beginPath();
                            dot_Begin=dotToPixCoord((dotList[0].end));
                            dot_End=dotToPixCoord((dotList[1].end));
                            ctx.moveTo(dot_Begin.x,dot_Begin.y);
                            ctx.lineTo(dot_End.x,dot_End.y);
                            ctx.closePath();
                            ctx.strokeStyle = "green";
                            ctx.stroke();
                        }
                        if(dotList[0].isSeen)
                        {
                            drawDot(dotList[0].end,"yellow");
                        }
                        if(dotList[1].isSeen)
                        {
                            drawDot(dotList[1].end,"yellow");
                        }
                    }
                }
                delete inputdotList;
            }

        }

    }
}

/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/
