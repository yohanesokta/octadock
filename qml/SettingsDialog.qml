import QtQuick
import QtQuick.Window

Window {
    id: root
    property bool isOpen: false

    title: "OctaDock Preferences"
    width: 500
    height: 570
    minimumWidth: 460
    minimumHeight: 500
    maximumWidth: 600
    maximumHeight: 650
    color: "transparent"
    flags: Qt.Dialog | Qt.FramelessWindowHint

    visible: isOpen

    function showSettings() {
        x = Math.round((Screen.width - width) / 2)
        y = Math.round(Math.max(40, (Screen.height - height) / 2))
        isOpen = true
        show()
        raise()
        requestActivate()
    }

    function closeSettings() {
        isOpen = false
        hide()
    }

    Shortcut {
        sequence: "Escape"
        onActivated: closeSettings()
    }

    // Outer card container with ambient drop shadow
    Rectangle {
        id: dialogCard
        anchors.fill: parent
        anchors.margins: 10
        radius: 16
        color: Qt.rgba(0.14, 0.16, 0.20, 0.98)
        border.color: Qt.rgba(1.0, 1.0, 1.0, 0.22)
        border.width: 1

        // Soft ambient drop shadow
        Rectangle {
            anchors.fill: parent
            anchors.margins: -8
            radius: 22
            color: "#60000000"
            z: -1
        }

        // Draggable Header
        Item {
            id: header
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: 48

            // Drag window handler
            MouseArea {
                anchors.fill: parent
                property point clickPos: Qt.point(0, 0)
                onPressed: function(mouse) {
                    clickPos = Qt.point(mouse.x, mouse.y)
                }
                onPositionChanged: function(mouse) {
                    var delta = Qt.point(mouse.x - clickPos.x, mouse.y - clickPos.y)
                    root.x += delta.x
                    root.y += delta.y
                }
            }

            Text {
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.verticalCenter: parent.verticalCenter
                text: "OctaDock Preferences"
                color: "#ffffff"
                font.pixelSize: 15
                font.bold: true
            }

            // Close button (macOS style circular close)
            Rectangle {
                anchors.right: parent.right
                anchors.rightMargin: 16
                anchors.verticalCenter: parent.verticalCenter
                width: 24
                height: 24
                radius: 12
                color: closeHover.containsMouse ? "#ff5f56" : Qt.rgba(1, 1, 1, 0.15)
                z: 10

                Text {
                    anchors.centerIn: parent
                    text: "✕"
                    color: "#ffffff"
                    font.pixelSize: 11
                }

                MouseArea {
                    id: closeHover
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: root.closeSettings()
                }
            }

            Rectangle {
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                height: 1
                color: Qt.rgba(1, 1, 1, 0.12)
            }
        }

        // Scrollable Settings Content
        Flickable {
            id: flick
            anchors.top: header.bottom
            anchors.bottom: footer.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 14
            contentHeight: contentCol.height + 10
            clip: true
            boundsBehavior: Flickable.StopAtBounds

            Column {
                id: contentCol
                width: parent.width - 16
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 16

                // Row 1: Icon Size
                Row {
                    width: parent.width
                    spacing: 10
                    Text {
                        width: 145
                        text: "Icon Size: " + (dockSettings ? dockSettings.iconSize : 58) + "px"
                        color: "#d0d4dc"
                        font.pixelSize: 13
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Item {
                        width: parent.width - 155
                        height: 24
                        anchors.verticalCenter: parent.verticalCenter
                        Rectangle {
                            anchors.verticalCenter: parent.verticalCenter
                            width: parent.width
                            height: 6
                            radius: 3
                            color: Qt.rgba(1, 1, 1, 0.18)
                        }
                        Rectangle {
                            property real ratio: (dockSettings ? (dockSettings.iconSize - 36) / (128 - 36) : 0.24)
                            x: Math.max(0, Math.min(parent.width * ratio - 9, parent.width - 18))
                            anchors.verticalCenter: parent.verticalCenter
                            width: 18
                            height: 18
                            radius: 9
                            color: "#ffffff"
                        }
                        MouseArea {
                            anchors.fill: parent
                            onPositionChanged: {
                                if (pressed && dockSettings) {
                                    var val = Math.round(36 + (mouseX / width) * (128 - 36))
                                    dockSettings.iconSize = Math.max(36, Math.min(val, 128))
                                }
                            }
                            onClicked: {
                                if (dockSettings) {
                                    var val = Math.round(36 + (mouseX / width) * (128 - 36))
                                    dockSettings.iconSize = Math.max(36, Math.min(val, 128))
                                }
                            }
                        }
                    }
                }

                // Row 2: Magnification Factor
                Row {
                    width: parent.width
                    spacing: 10
                    Text {
                        width: 145
                        text: "Magnification: " + (dockSettings ? Math.round(dockSettings.magnificationFactor * 100) : 135) + "%"
                        color: "#d0d4dc"
                        font.pixelSize: 13
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Item {
                        width: parent.width - 155
                        height: 24
                        anchors.verticalCenter: parent.verticalCenter
                        Rectangle {
                            anchors.verticalCenter: parent.verticalCenter
                            width: parent.width
                            height: 6
                            radius: 3
                            color: Qt.rgba(1, 1, 1, 0.18)
                        }
                        Rectangle {
                            property real ratio: (dockSettings ? (dockSettings.magnificationFactor - 1.0) / (2.0 - 1.0) : 0.35)
                            x: Math.max(0, Math.min(parent.width * ratio - 9, parent.width - 18))
                            anchors.verticalCenter: parent.verticalCenter
                            width: 18
                            height: 18
                            radius: 9
                            color: "#ffffff"
                        }
                        MouseArea {
                            anchors.fill: parent
                            onPositionChanged: {
                                if (pressed && dockSettings) {
                                    var val = 1.0 + (mouseX / width) * (2.0 - 1.0)
                                    dockSettings.magnificationFactor = Math.max(1.0, Math.min(val, 2.0))
                                }
                            }
                            onClicked: {
                                if (dockSettings) {
                                    var val = 1.0 + (mouseX / width) * (2.0 - 1.0)
                                    dockSettings.magnificationFactor = Math.max(1.0, Math.min(val, 2.0))
                                }
                            }
                        }
                    }
                }

                // Row 3: Icon Spacing
                Row {
                    width: parent.width
                    spacing: 10
                    Text {
                        width: 145
                        text: "Icon Spacing: " + (dockSettings ? dockSettings.iconSpacing : 18) + "px"
                        color: "#d0d4dc"
                        font.pixelSize: 13
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Item {
                        width: parent.width - 155
                        height: 24
                        anchors.verticalCenter: parent.verticalCenter
                        Rectangle {
                            anchors.verticalCenter: parent.verticalCenter
                            width: parent.width
                            height: 6
                            radius: 3
                            color: Qt.rgba(1, 1, 1, 0.18)
                        }
                        Rectangle {
                            property real ratio: (dockSettings ? (dockSettings.iconSpacing - 4) / (32 - 4) : 0.5)
                            x: Math.max(0, Math.min(parent.width * ratio - 9, parent.width - 18))
                            anchors.verticalCenter: parent.verticalCenter
                            width: 18
                            height: 18
                            radius: 9
                            color: "#ffffff"
                        }
                        MouseArea {
                            anchors.fill: parent
                            onPositionChanged: {
                                if (pressed && dockSettings) {
                                    var val = Math.round(4 + (mouseX / width) * (32 - 4))
                                    dockSettings.iconSpacing = Math.max(4, Math.min(val, 32))
                                }
                            }
                            onClicked: {
                                if (dockSettings) {
                                    var val = Math.round(4 + (mouseX / width) * (32 - 4))
                                    dockSettings.iconSpacing = Math.max(4, Math.min(val, 32))
                                }
                            }
                        }
                    }
                }

                // Row 4: Background Opacity
                Row {
                    width: parent.width
                    spacing: 10
                    Text {
                        width: 145
                        text: "Glass Opacity: " + (dockSettings ? Math.round(dockSettings.backgroundOpacity * 100) : 28) + "%"
                        color: "#d0d4dc"
                        font.pixelSize: 13
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Item {
                        width: parent.width - 155
                        height: 24
                        anchors.verticalCenter: parent.verticalCenter
                        Rectangle {
                            anchors.verticalCenter: parent.verticalCenter
                            width: parent.width
                            height: 6
                            radius: 3
                            color: Qt.rgba(1, 1, 1, 0.18)
                        }
                        Rectangle {
                            property real ratio: (dockSettings ? (dockSettings.backgroundOpacity - 0.05) / (0.90 - 0.05) : 0.27)
                            x: Math.max(0, Math.min(parent.width * ratio - 9, parent.width - 18))
                            anchors.verticalCenter: parent.verticalCenter
                            width: 18
                            height: 18
                            radius: 9
                            color: "#ffffff"
                        }
                        MouseArea {
                            anchors.fill: parent
                            onPositionChanged: {
                                if (pressed && dockSettings) {
                                    var val = 0.05 + (mouseX / width) * (0.90 - 0.05)
                                    dockSettings.backgroundOpacity = Math.max(0.05, Math.min(val, 0.90))
                                }
                            }
                            onClicked: {
                                if (dockSettings) {
                                    var val = 0.05 + (mouseX / width) * (0.90 - 0.05)
                                    dockSettings.backgroundOpacity = Math.max(0.05, Math.min(val, 0.90))
                                }
                            }
                        }
                    }
                }

                // Row 5: Bottom Margin
                Row {
                    width: parent.width
                    spacing: 10
                    Text {
                        width: 145
                        text: "Bottom Margin: " + (dockSettings ? dockSettings.bottomMargin : 6) + "px"
                        color: "#d0d4dc"
                        font.pixelSize: 13
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Item {
                        width: parent.width - 155
                        height: 24
                        anchors.verticalCenter: parent.verticalCenter
                        Rectangle {
                            anchors.verticalCenter: parent.verticalCenter
                            width: parent.width
                            height: 6
                            radius: 3
                            color: Qt.rgba(1, 1, 1, 0.18)
                        }
                        Rectangle {
                            property real ratio: (dockSettings ? dockSettings.bottomMargin / 40.0 : 0.15)
                            x: Math.max(0, Math.min(parent.width * ratio - 9, parent.width - 18))
                            anchors.verticalCenter: parent.verticalCenter
                            width: 18
                            height: 18
                            radius: 9
                            color: "#ffffff"
                        }
                        MouseArea {
                            anchors.fill: parent
                            onPositionChanged: {
                                if (pressed && dockSettings) {
                                    var val = Math.round((mouseX / width) * 40.0)
                                    dockSettings.bottomMargin = Math.max(0, Math.min(val, 40))
                                }
                            }
                            onClicked: {
                                if (dockSettings) {
                                    var val = Math.round((mouseX / width) * 40.0)
                                    dockSettings.bottomMargin = Math.max(0, Math.min(val, 40))
                                }
                            }
                        }
                    }
                }

                // Row 6: Corner Radius
                Row {
                    width: parent.width
                    spacing: 10
                    Text {
                        width: 145
                        text: "Corner Radius: " + (dockSettings ? dockSettings.cornerRadius : 24) + "px"
                        color: "#d0d4dc"
                        font.pixelSize: 13
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Item {
                        width: parent.width - 155
                        height: 24
                        anchors.verticalCenter: parent.verticalCenter
                        Rectangle {
                            anchors.verticalCenter: parent.verticalCenter
                            width: parent.width
                            height: 6
                            radius: 3
                            color: Qt.rgba(1, 1, 1, 0.18)
                        }
                        Rectangle {
                            property real ratio: (dockSettings ? (dockSettings.cornerRadius - 10) / (40.0 - 10) : 0.46)
                            x: Math.max(0, Math.min(parent.width * ratio - 9, parent.width - 18))
                            anchors.verticalCenter: parent.verticalCenter
                            width: 18
                            height: 18
                            radius: 9
                            color: "#ffffff"
                        }
                        MouseArea {
                            anchors.fill: parent
                            onPositionChanged: {
                                if (pressed && dockSettings) {
                                    var val = Math.round(10 + (mouseX / width) * (40.0 - 10))
                                    dockSettings.cornerRadius = Math.max(10, Math.min(val, 40))
                                }
                            }
                            onClicked: {
                                if (dockSettings) {
                                    var val = Math.round(10 + (mouseX / width) * (40.0 - 10))
                                    dockSettings.cornerRadius = Math.max(10, Math.min(val, 40))
                                }
                            }
                        }
                    }
                }

                // Divider
                Rectangle {
                    width: parent.width
                    height: 1
                    color: Qt.rgba(1, 1, 1, 0.10)
                }

                // Toggle 1: Magnification
                Row {
                    width: parent.width
                    spacing: 10
                    Text {
                        width: parent.width - 60
                        text: "Magnification effect on hover"
                        color: "#ffffff"
                        font.pixelSize: 13
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Rectangle {
                        width: 44
                        height: 24
                        radius: 12
                        color: (dockSettings && dockSettings.enableMagnification) ? "#34c759" : Qt.rgba(1, 1, 1, 0.2)
                        Rectangle {
                            x: (dockSettings && dockSettings.enableMagnification) ? 22 : 2
                            anchors.verticalCenter: parent.verticalCenter
                            width: 20
                            height: 20
                            radius: 10
                            color: "#ffffff"
                            Behavior on x { NumberAnimation { duration: 120 } }
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: if (dockSettings) dockSettings.enableMagnification = !dockSettings.enableMagnification
                        }
                    }
                }

                // Toggle 2: Running Indicators
                Row {
                    width: parent.width
                    spacing: 10
                    Text {
                        width: parent.width - 60
                        text: "Show indicators for open applications"
                        color: "#ffffff"
                        font.pixelSize: 13
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Rectangle {
                        width: 44
                        height: 24
                        radius: 12
                        color: (dockSettings && dockSettings.showRunningIndicators) ? "#34c759" : Qt.rgba(1, 1, 1, 0.2)
                        Rectangle {
                            x: (dockSettings && dockSettings.showRunningIndicators) ? 22 : 2
                            anchors.verticalCenter: parent.verticalCenter
                            width: 20
                            height: 20
                            radius: 10
                            color: "#ffffff"
                            Behavior on x { NumberAnimation { duration: 120 } }
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: if (dockSettings) dockSettings.showRunningIndicators = !dockSettings.showRunningIndicators
                        }
                    }
                }

                // Toggle 3: Auto-hide
                Row {
                    width: parent.width
                    spacing: 10
                    Text {
                        width: parent.width - 60
                        text: "Automatically hide and show the Dock"
                        color: "#ffffff"
                        font.pixelSize: 13
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Rectangle {
                        width: 44
                        height: 24
                        radius: 12
                        color: (dockSettings && dockSettings.autoHide) ? "#34c759" : Qt.rgba(1, 1, 1, 0.2)
                        Rectangle {
                            x: (dockSettings && dockSettings.autoHide) ? 22 : 2
                            anchors.verticalCenter: parent.verticalCenter
                            width: 20
                            height: 20
                            radius: 10
                            color: "#ffffff"
                            Behavior on x { NumberAnimation { duration: 120 } }
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: if (dockSettings) dockSettings.autoHide = !dockSettings.autoHide
                        }
                    }
                }

                // Toggle 4: Always Stay on Top
                Row {
                    width: parent.width
                    spacing: 10
                    Text {
                        width: parent.width - 60
                        text: "Keep Dock on top of other windows"
                        color: "#ffffff"
                        font.pixelSize: 13
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Rectangle {
                        width: 44
                        height: 24
                        radius: 12
                        color: (dockSettings && dockSettings.alwaysOnTop) ? "#34c759" : Qt.rgba(1, 1, 1, 0.2)
                        Rectangle {
                            x: (dockSettings && dockSettings.alwaysOnTop) ? 22 : 2
                            anchors.verticalCenter: parent.verticalCenter
                            width: 20
                            height: 20
                            radius: 10
                            color: "#ffffff"
                            Behavior on x { NumberAnimation { duration: 120 } }
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: if (dockSettings) dockSettings.alwaysOnTop = !dockSettings.alwaysOnTop
                        }
                    }
                }
            }
        }

        // Fixed Footer Buttons
        Item {
            id: footer
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: 52

            Rectangle {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: 1
                color: Qt.rgba(1, 1, 1, 0.12)
            }

            Row {
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 20
                spacing: 12

                Rectangle {
                    width: 140
                    height: 32
                    radius: 8
                    color: rstHover.containsMouse ? Qt.rgba(1, 1, 1, 0.2) : Qt.rgba(1, 1, 1, 0.1)
                    border.color: Qt.rgba(1, 1, 1, 0.2)
                    border.width: 1

                    Text {
                        anchors.centerIn: parent
                        text: "Reset Defaults"
                        color: "#ffffff"
                        font.pixelSize: 13
                    }

                    MouseArea {
                        id: rstHover
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: if (dockSettings) dockSettings.resetToDefaults()
                    }
                }

                Rectangle {
                    width: 90
                    height: 32
                    radius: 8
                    color: doneHover.containsMouse ? "#0a84ff" : "#007aff"

                    Text {
                        anchors.centerIn: parent
                        text: "Done"
                        color: "#ffffff"
                        font.pixelSize: 13
                        font.weight: Font.DemiBold
                    }

                    MouseArea {
                        id: doneHover
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: root.closeSettings()
                    }
                }
            }
        }
    }
}
