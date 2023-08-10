import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQml   2.2
import QtQuick.Layouts 1.3
import qxpack.indcom.ui_qml_base  1.0
import qxpack.indcom.ui_qml_control 1.0

Item {
   id: idRoot; anchors.fill: parent;
   implicitWidth: parent.width; implicitHeight: parent.height;

    RowLayout {
        anchors.fill: parent;
        Item {
            Layout.preferredWidth: 400;  Layout.preferredHeight: 400;
            Layout.minimumWidth: 400; Layout.minimumHeight: 400;
            Rectangle { anchors.fill: idImItem; color: "#808080"; }
            IcQuickImageItem {
                id: idImItem; imageData: gQuickImageSrc;
                width: 400; height: 400;
            }
            Text {
                anchors.bottom: idImItem.bottom; color: "red";
                text : "fillModeStr: %1".arg( idImItem.fillModeStr );
            }
        }
        Item {
            Layout.fillHeight: true; Layout.fillWidth: true;
            Rectangle { anchors.fill: idImItemStrech; color:"#808080"; }
            IcQuickImageItem {
                id: idImItemStrech; imageData: gQuickImageSrc;
                anchors.fill: parent; fillMode: Qt.IgnoreAspectRatio;
            }
            Text {
                anchors.bottom: idImItemStrech.bottom; color: "red";
                text : "fillModeStr: %1".arg( idImItemStrech.fillModeStr );
            }
        }
    }
    ShaderEffect {
        width: 100; height: 100;
           property variant src: idImItem;
              vertexShader: "
                  uniform highp mat4 qt_Matrix;
                  attribute highp vec4 qt_Vertex;
                  attribute highp vec2 qt_MultiTexCoord0;
                  varying highp vec2 coord;
                  void main() {
                      coord = qt_MultiTexCoord0;
                      gl_Position = qt_Matrix * qt_Vertex;
                  }"
              fragmentShader: "
                  varying highp vec2 coord;
                  uniform sampler2D src;
                  uniform lowp float qt_Opacity;
                  void main() {
                      lowp vec4 tex = texture2D(src, coord);
                      gl_FragColor = vec4(vec3(dot(tex.rgb,
                                          vec3(0.344, 0.5, 0.156))),
                                               tex.a) * qt_Opacity;
                  }"
    }
}
