pragma Singleton
import QtQuick 2.0
import qxpack.indcom.ui_qml_base 1.0

//Rectangle{
//    height:300;width:300;
//}
Item{
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
}
