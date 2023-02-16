/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.1
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import QtDataVisualization 1.0
import perimeter.main.view.Controls 1.0
import qxpack.indcom.ui_qml_base 1.0
import perimeter.main.view.Utils 1.0
import "."

Column {
    id: root
    property real fontSize: 12
    property var heightMapSurfaceDataProxy: null;

    signal changePage(var pageName,var params);
    signal refresh;

    onRefresh: {
//        if(heightMapSurfaceDataProxy1==null) {
//            heightMapSurfaceDataProxy1=IcUiQmlApi.appCtrl.objMgr.attachObj("Perimeter::QHeightMapSurfaceDataProxyWrapper", false,["D:/perimeterProject/perimeter/bin/debug/visionFieldIsland/layer_1.png"]);
//        }
//        heightMapSurfaceDataProxy1.heightMapFile=":/layers/layer_1.png";
//        layerOneSeries.dataProxy=heightMapSurfaceDataProxy1;
        layerOneSeries.dataProxy.setHeightMap("D:/perimeterProject/perimeter/bin/debug/visionFieldIsland/visionField.png");
//        layerOneSeries.baseGradient=layerOneGradient;
    }

    Component.onCompleted:
    {
        heightMapSurfaceDataProxy=IcUiQmlApi.appCtrl.objMgr.attachObj("Perimeter::QHeightMapSurfaceDataProxyWrapper", false,["D:/perimeterProject/perimeter/bin/debug/visionFieldIsland/visionField.png"]);
        layerOneSeries.dataProxy=heightMapSurfaceDataProxy;
//        heightMapSurfaceDataProxy2=IcUiQmlApi.appCtrl.objMgr.attachObj("Perimeter::QHeightMapSurfaceDataProxyWrapper", false,["D:/perimeterProject/perimeter/bin/debug/visionFieldIsland/layer_1.png"]);
//        heightMapSurfaceDataProxy3=IcUiQmlApi.appCtrl.objMgr.attachObj("Perimeter::QHeightMapSurfaceDataProxyWrapper", false,["D:/perimeterProject/perimeter/bin/debug/visionFieldIsland/layer_1.png"]);
//        heightMapSurfaceDataProxy1.heightMapFile=":/layers/layer_1.png";
//        heightMapSurfaceDataProxy1=IcUiQmlApi.appCtrl.objMgr.attachObj("Perimeter::QHeightMapSurfaceDataProxyWrapper", false,["D:/perimeterProject/perimeter/bin/debug/visionFieldIsland/layer_1.png"]);
//        heightMapSurfaceDataProxy1.setHeightMap("D:/perimeterProject/perimeter/bin/debug/visionFieldIsland/layer_1.png");
//        heightMapSurfaceDataProxy2=IcUiQmlApi.appCtrl.objMgr.attachObj("Perimeter::QHeightMapSurfaceDataProxyWrapper", false,["D:/perimeterProject/perimeter/bin/debug/visionFieldIsland/layer_1.png"]);
//        heightMapSurfaceDataProxy2.setHeightMap("D:/perimeterProject/perimeter/bin/debug/visionFieldIsland/layer_2.png");
//        heightMapSurfaceDataProxy3=IcUiQmlApi.appCtrl.objMgr.attachObj("Perimeter::QHeightMapSurfaceDataProxyWrapper", false,["D:/perimeterProject/perimeter/bin/debug/visionFieldIsland/layer_1.png"]);
//        heightMapSurfaceDataProxy3.setHeightMap("D:/perimeterProject/perimeter/bin/debug/visionFieldIsland/layer_3.png");
    }

//    onRefresh: console.log(applicationDirPath);



    Item {
        id: surfaceView
        width: parent.width
        height: parent.height*14/15;
        //! [0]
        ColorGradient {
            id: layerOneGradient
            ColorGradientStop { position: 0.00; color: "#ff0000" }
            ColorGradientStop { position: 0.05; color: "#ff3200" }
            ColorGradientStop { position: 0.10; color: "#ff6400" }
            ColorGradientStop { position: 0.15; color: "#ff9600" }
            ColorGradientStop { position: 0.20; color: "#ffC800" }
            ColorGradientStop { position: 0.25; color: "#ffff00" }
            ColorGradientStop { position: 0.30; color: "#C8ff00" }
            ColorGradientStop { position: 0.35; color: "#96ff00" }
            ColorGradientStop { position: 0.40; color: "#64ff00" }
            ColorGradientStop { position: 0.45; color: "#32ff00" }
            ColorGradientStop { position: 0.50; color: "#00ff00" }
            ColorGradientStop { position: 0.55; color: "#00ff32" }
            ColorGradientStop { position: 0.60; color: "#00ff64" }
            ColorGradientStop { position: 0.65; color: "#00ff96" }
            ColorGradientStop { position: 0.70; color: "#00ffC8" }
            ColorGradientStop { position: 0.75; color: "#00ffff" }
            ColorGradientStop { position: 0.80; color: "#00C8ff" }
            ColorGradientStop { position: 0.85; color: "#0096ff" }
            ColorGradientStop { position: 0.90; color: "#0064ff" }
            ColorGradientStop { position: 0.95; color: "#0032ff" }
            ColorGradientStop { position: 1.00; color: "#0000ff" }


        }

        //! [0]

        Surface3D {
            id: surfaceLayers
            width: surfaceView.width
            height: surfaceView.height
            theme: Theme3D {
                type: Theme3D.ThemeEbony
                font.pointSize: 35
                colorStyle: Theme3D.ColorStyleRangeGradient
            }
            shadowQuality: AbstractGraph3D.ShadowQualityNone
            selectionMode: AbstractGraph3D.SelectionRow | AbstractGraph3D.SelectionSlice
//            scene.activeCamera.cameraPreset: Camera3D.CameraPresetIsometricLeft
//            axisY.min: 0
//            axisY.max: 40;
//            axisX.segmentCount: 5
//            axisX.subSegmentCount: 2
//            axisX.labelFormat: "%i"
//            axisZ.segmentCount: 5
//            axisZ.subSegmentCount: 2
//            axisZ.labelFormat: "%i"
//            axisY.segmentCount: 5
//            axisY.subSegmentCount: 2
//            axisY.labelFormat: "%i"

            //! [1]
            //! [2]



            Surface3DSeries {
                id: layerOneSeries
                baseGradient: layerOneGradient
                //! [2]
//                dataProxy:IcUiQmlApi.appCtrl.objMgr.attachObj("perimeter::QHeightMapSurfaceDataProxyWarpper", false,["D:/perimeterProject/perimeter/bin/debug/visionFieldIsland/layer_1.png"]);
//                dataProxy:heightMapSurfaceDataProxy1;
//                dataProxy:HeightMapSurfaceDataProxy {
//                    id:proxy1;
//                    heightMapFile: ":/layers/layer_1.png"
////                    heightMapFile: "file:///D:/perimeterProject/perimeter/bin/debug/visionFieldIsland/visionField.png";

//                }
                flatShadingEnabled: false
                drawMode: Surface3DSeries.DrawSurface

            }
        }
//        Surface3D {
//                 width: parent.width
//                 height: parent.height

//                 Surface3DSeries {
//                     baseGradient: layerOneGradient
//                     itemLabelFormat: "Pop density at (@xLabel N, @zLabel E): @yLabel"
//                     ItemModelSurfaceDataProxy {
//                         itemModel: dataModel
//                         // Mapping model roles to surface series rows, columns, and values.
//                         rowRole: "longitude"
//                         columnRole: "latitude"
//                         yPosRole: "pop_density"
//                     }
//                 }
//             }
//             ListModel {
//                 id: dataModel
//                 ListElement{ longitude: "20"; latitude: "10"; pop_density: "4.75"; }
//                 ListElement{ longitude: "21"; latitude: "10"; pop_density: "3.00"; }
//                 ListElement{ longitude: "22"; latitude: "10"; pop_density: "1.24"; }
//                 ListElement{ longitude: "23"; latitude: "10"; pop_density: "2.53"; }
//                 ListElement{ longitude: "20"; latitude: "11"; pop_density: "2.55"; }
//                 ListElement{ longitude: "21"; latitude: "11"; pop_density: "2.03"; }
//                 ListElement{ longitude: "22"; latitude: "11"; pop_density: "3.46"; }
//                 ListElement{ longitude: "23"; latitude: "11"; pop_density: "5.12"; }
//                 ListElement{ longitude: "20"; latitude: "12"; pop_density: "1.37"; }
//                 ListElement{ longitude: "21"; latitude: "12"; pop_density: "2.98"; }
//                 ListElement{ longitude: "22"; latitude: "12"; pop_density: "3.33"; }
//                 ListElement{ longitude: "23"; latitude: "12"; pop_density: "3.23"; }
//                 ListElement{ longitude: "20"; latitude: "13"; pop_density: "4.34"; }
//                 ListElement{ longitude: "21"; latitude: "13"; pop_density: "3.54"; }
//                 ListElement{ longitude: "22"; latitude: "13"; pop_density: "1.65"; }
//                 ListElement{ longitude: "23"; latitude: "13"; pop_density: "2.67"; }
//             }
    }
    Rectangle{id:bottomRibbon;width: parent.width;height: parent.height*1/15;color:CommonSettings.ribbonColor;
        Row{anchors.fill: parent;
            Item{height: parent.height;width:parent.width*0.50;
                Item{anchors.fill: parent;anchors.margins:parent.height*0.15;
                    CusButton{text:lt+qsTr("Back");onClicked:{root.changePage("analysisLobby",null);}}}
                }
//            Item{height: parent.height;width:parent.width*0.52;
//                Item{anchors.fill: parent;anchors.margins:parent.height*0.15;
//                    Flow{height: parent.height;spacing: height*0.8;width: parent.width;anchors.horizontalCenter: parent.horizontalCenter
//                        CusButton{text:"重测";onClicked:{root.changePage("check",null);}}
//                        CusButton{text:"新建患者";onClicked:{;root.changePage("patientManagement","createNewPatient");}}
//                    }
//                }
//            }

            Item{height: parent.height;width:parent.width*0.50;
                Item{id: item1;anchors.fill: parent;anchors.margins:parent.height*0.15;
                    Row
                    {
                        height: parent.height; layoutDirection: Qt.RightToLeft;spacing: height*0.8;width: parent.width
                        anchors.horizontalCenter: parent.horizontalCenter
                        CusButton{text:lt+qsTr("Reset");}
                        CusButton{text:lt+qsTr("Zoom out");}
                        CusButton{text:lt+qsTr("Zoom in");}
                    }
                }
            }
        }
    }

}

/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/
