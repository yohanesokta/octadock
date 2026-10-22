import QtQuick

Item {
    id: root

    property bool isOpen: false
    property int targetIndex: -1
    property string targetName: ""
    property string targetExec: ""
    property string targetId: ""

    signal openApp(int index)
    signal removeApp(int index)
    signal openPreferences()
    signal quitApp()

    visible: opacity > 0.0
    opacity: isOpen ? 1.0 : 0.0

    Behavior on opacity {
        NumberAnimation { duration: 120; easing.type: Easing.OutQuad }
    }

    // Dismiss click catcher outside menu
    MouseArea {
        anchors.fill: parent
        enabled: root.isOpen
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: root.isOpen = false
    }

    Rectangle {
        id: menuBox
        width: 190
        height: contentCol.height + 14
        radius: 10
        color: Qt.rgba(0.14, 0.16, 0.20, 0.94)
        border.color: Qt.rgba(1.0, 1.0, 1.0, 0.22)
        border.width: 1

        // Shadow behind context menu
        Rectangle {
            anchors.fill: parent
            anchors.margins: -4
            radius: 14
            color: "#40000000"
            z: -1
        }

        Column {
            id: contentCol
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 7
            width: parent.width - 12
            spacing: 2

            // Menu Item: Open
            Rectangle {
                width: parent.width
                height: 26
                radius: 6
                color: openArea.containsMouse ? "#007aff" : "transparent"

                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.verticalCenter: parent.verticalCenter
                    text: root.targetName.length > 0 ? ("Open " + root.targetName) : "Open"
                    color: "#ffffff"
                    font.pixelSize: 13
                    font.weight: Font.Medium
                }

                MouseArea {
                    id: openArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        root.isOpen = false
                        root.openApp(root.targetIndex)
                    }
                }
            }

            // Separator
            Rectangle {
                width: parent.width - 12
                anchors.horizontalCenter: parent.horizontalCenter
                height: 1
                color: Qt.rgba(1.0, 1.0, 1.0, 0.12)
            }

            // Menu Item: Preferences
            Rectangle {
                width: parent.width
                height: 26
                radius: 6
                color: prefArea.containsMouse ? "#007aff" : "transparent"

                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Dock Preferences..."
                    color: "#ffffff"
                    font.pixelSize: 13
                }

                MouseArea {
                    id: prefArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        root.isOpen = false
                        root.openPreferences()
                    }
                }
            }

            // Menu Item: Remove from Dock
            Rectangle {
                width: parent.width
                height: 26
                radius: 6
                visible: root.targetIndex >= 0
                color: removeArea.containsMouse ? "#007aff" : "transparent"

                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Remove from Dock"
                    color: "#ffffff"
                    font.pixelSize: 13
                }

                MouseArea {
                    id: removeArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        root.isOpen = false
                        root.removeApp(root.targetIndex)
                    }
                }
            }

            // Separator
            Rectangle {
                width: parent.width - 12
                anchors.horizontalCenter: parent.horizontalCenter
                height: 1
                color: Qt.rgba(1.0, 1.0, 1.0, 0.12)
            }

            // Menu Item: Quit OctaDock
            Rectangle {
                width: parent.width
                height: 26
                radius: 6
                color: quitArea.containsMouse ? "#e53935" : "transparent"

                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Quit OctaDock"
                    color: "#ffffff"
                    font.pixelSize: 13
                }

                MouseArea {
                    id: quitArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        root.isOpen = false
                        root.quitApp()
                    }
                }
            }
        }
    }

    function showAt(x, y, index, name, id, exec) {
        targetIndex = index
        targetName = name
        targetId = id
        targetExec = exec

        var targetX = Math.max(10, Math.min(x - menuBox.width / 2, root.width - menuBox.width - 10))
        var targetY = y - menuBox.height - 12
        if (targetY < 10) {
            targetY = y + 20
        }

        menuBox.x = targetX
        menuBox.y = targetY
        isOpen = true
    }
}
