import QtQuick
import Qt5Compat.GraphicalEffects
import PanoramaK

Rectangle {
    id: root
    color: "#414141"

    property int connectStatus: PanoramaK.ConnectionStatus.Disconnect
    property int afuStatus: PanoramaK.Status.Unknown
    property bool transmitMode: false
    readonly property color disconnectedColor: "#FF6F00"
    readonly property color connectingColor: "#FFEA00"
    readonly property color connectedColor: "#4CAF50"

    onAfuStatusChanged: {
        if (afuStatus === PanoramaK.Status.Ok) {
            textAfuStatus.text = qsTr("Качество АФУ: полностью исправно")
            textAfuStatus.color = "#4CAF50"
        }
        else if (afuStatus === PanoramaK.Status.ValidSwr) {
            textAfuStatus.text = qsTr("Качество АФУ: исправно, рекомендуется выполнить согласование.")
            textAfuStatus.color = "#8BC34A"
        }
        else if (afuStatus === PanoramaK.Status.HighSwr) {
            textAfuStatus.text = qsTr("Качество АФУ: необходимо выполнить согласование.")
            textAfuStatus.color = "#FFEB3B"
        }
        else if (afuStatus === PanoramaK.Status.VeryHighSwr) {
            textAfuStatus.text = qsTr("Качество АФУ: работа запрещена, требуется сервисное обслуживание!")
            textAfuStatus.color = "#FF9800"
        }
        else if (afuStatus === PanoramaK.Status.BadContact) {
            textAfuStatus.text = qsTr("Качество АФУ: работа запрещена, обнаружен плохой контакт!")
            textAfuStatus.color = "#FF5722"
        }
        else if (afuStatus === PanoramaK.Status.CableBreakage) {
            textAfuStatus.text = qsTr("Качество АФУ: работа запрещена, обрыв кабеля!")
            textAfuStatus.color = "#F44336"
        }
        else {
            textAfuStatus.text = ""
        }
    }

    Connections {
        target: controlObserver

        function onConnectionStatusChanged(status) {
            connectStatus = status
            if (connectStatus === PanoramaK.ConnectionStatus.Connecting)
                afuStatus = PanoramaK.Status.Unknown
        }

        function onStatusChanged(status) {
            afuStatus = status
        }

        function onTrxChanged(tx) {
            if (tx)
                textBadContact.text = ""

            root.transmitMode = tx
        }

        function onBadContactDetected() {
            textBadContact.text = qsTr("Обнаружен плохой контакт")
        }
    }

    Text {
        id: textConnectTitle
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 20
        font.pointSize: 22
        style: Text.Outline
        styleColor: "black"
        color: root.connectStatus === PanoramaK.ConnectionStatus.Disconnect ?
                       disconnectedColor : root.connectStatus === PanoramaK.ConnectionStatus.Connecting ?
                       connectingColor : connectedColor
        text: root.connectStatus === PanoramaK.ConnectionStatus.Disconnect ?
                  qsTr("Выполните подключение к трансиверу") : root.connectStatus === PanoramaK.ConnectionStatus.Connecting ?
                      qsTr("Выполняется подключение") : qsTr("Устройство подключено")
    }

    Text {
        id: textTransceiverMode
        anchors.left: parent.left
        anchors.top: textConnectTitle.bottom
        anchors.margins: 20
        font.pointSize: 22
        style: Text.Outline
        styleColor: "black"
        color: root.transmitMode ? "#FF3D00" : "#4CAF50"
        text: root.transmitMode ? qsTr("Режим передачи") : qsTr("Режим приёма")
        visible: root.connectStatus === PanoramaK.ConnectionStatus.Connected
    }

    Text {
        id: textAfuStatus
        anchors.left: parent.left
        anchors.top: textTransceiverMode.bottom
        anchors.margins: 20
        font.pointSize: 22
        style: Text.Outline
        styleColor: "black"
        visible: root.connectStatus === PanoramaK.ConnectionStatus.Connected
    }

    Text {
        id: textBadContact
        anchors.left: parent.left
        anchors.top: textAfuStatus.bottom
        anchors.margins: 20
        font.pointSize: 22
        style: Text.Outline
        styleColor: "black"
        color: "#F44336"
        visible: root.connectStatus === PanoramaK.ConnectionStatus.Connected
    }
}
