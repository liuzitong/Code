import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQml   2.2
import qxpack.indcom.ui_qml_base  1.0

ApplicationWindow {
    id: idBoot; width: 800; height: 600; visible: true;

    Component.onCompleted: {
        console.info("[] initalize IcQmlUiApi, version:" + IcUiQmlApi.apiVersion );
        idContentLoader.source = "qrc:/mainview.qml"
        //idGrabTmr.start();
    }
    Timer {
       id:idGrabTmr; interval: 3000;
       repeat: false;
       onTriggered: {
          idContentLoader.grabToImage( saveRsl );
       }
       function saveRsl( rsl ) {
          rsl.saveToFile("z:/rsl.png" );
          Qt.quit();
       }
    }

    Loader {
        id: idContentLoader; width: 800; height: 600; visible: true;
    }
}

