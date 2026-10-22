import QtQuick

Item {
    id: root
    property string text: ""
    property bool active: false

    width: pill.width
    height: pill.height + 6

    opacity: active ? 1.0 : 0.0
    visible: opacity > 0.0

    Behavior on opacity {
        NumberAnimation { duration: 150; easing.type: Easing.OutQuad }
    }

    Rectangle {
        id: pill
        anchors.horizontalCenter: parent.horizontalCenter
        width: label.implicitWidth + 24
        height: 28
        radius: 8
        color: Qt.rgba(0.12, 0.14, 0.18, 0.88)
        border.color: Qt.rgba(1.0, 1.0, 1.0, 0.25)
        border.width: 1

        Text {
            id: label
            anchors.centerIn: parent
            text: root.text
            color: "#ffffff"
            font.pixelSize: 13
            font.weight: Font.Medium
            antialiasing: true
        }

        // Bottom arrow caret
        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.bottom
            anchors.topMargin: -3
            width: 7
            height: 7
            rotation: 45
            color: Qt.rgba(0.12, 0.14, 0.18, 0.88)
            border.color: Qt.rgba(1.0, 1.0, 1.0, 0.25)
            border.width: 1
            z: -1
        }
    }
}
