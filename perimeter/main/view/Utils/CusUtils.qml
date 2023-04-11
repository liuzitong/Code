pragma Singleton
import QtQuick 2.0
import qxpack.indcom.ui_qml_base 1.0

//Rectangle{
//    height:300;width:300;
//}
Item{
    property var boundaries:IcUiQmlApi.appCtrl.settings.boundaries;

    function rgb(r,g,b){var ret=(r<<16|g<<8|b); return ("#"+ret.toString(16).toUpperCase());}
    function getAge(birthDateStr){
        var birthDate=new Date(Date.parse(birthDateStr));
        var now=new Date(Date.now());
        var age=now.getFullYear()-birthDate.getFullYear();
        if(now.getMonth()<birthDate.getMonth()) age-=1;
        if((now.getMonth()==birthDate.getMonth())&&(now.getDay()<birthDate.getDay())) age-=1;
        return age;
    }

    function getTranslatedStr(previousStr)
    {
        var strs=previousStr.split('@');
        if(strs.length===2)
        {
            if( IcUiQmlApi.appCtrl.settings.isRuntimeLangEng)
                return strs[0];
            else return strs[1];
        }
        else
            return previousStr;
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
}
