import QtQuick 2.15

Image {
    id: root
    width: 300
    height: width/1.28
    source: "qrc:/images/swr_meter_scale.jpg"

    property real angle: 0

    Image {
        id: arraw
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        height: 0.9*parent.height
        width: height/8.8
        source: "qrc:/images/arrow.png"
        rotation: 45
    }
}
