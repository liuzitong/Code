import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQml   2.2
import qxpack.indcom.ui_qml_base  1.0

ApplicationWindow {
    id: idBoot; width: 800; height: 600; visible: true;

    Component.onCompleted: {
        console.info("[] initalize IcQmlUiApi, version:" + IcUiQmlApi.apiVersion );
        idContentLoader.source = "qrc:/mainview.qml"
    }
    Loader {
        id: idContentLoader; anchors.fill: parent;
    }
}

