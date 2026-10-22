import QtQuick

Item {
    id: root
    property bool isOpen: false

    visible: opacity > 0.0
    opacity: isOpen ? 1.0 : 0.0

    Behavior on opacity {
        NumberAnimation { duration: 160; easing.type: Easing.OutQuad }
    }

    // Modal background blocker
    MouseArea {
        anchors.fill: parent
        enabled: root.isOpen
        onClicked: {} // prevent clicks through
    }

    Rectangle {
        id: dialog
        anchors.centerIn: parent
        width: 480
        height: 520
        radius: 16
        color: Qt.rgba(0.16, 0.18, 0.22, 0.96)
        border.color: Qt.rgba(1.0, 1.0, 1.0, 0.22)
        border.width: 1

        // Shadow behind dialog
        Rectangle {
            anchors.fill: parent
            anchors.margins: -10
            radius: 22
            color: "#60000000"
            z: -1
        }

        // Header
        Item {
            id: header
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: 48

            Text {
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.verticalCenter: parent.verticalCenter
                text: "OctaDock Preferences"
                color: "#ffffff"
                font.pixelSize: 16
                font.bold: true
            }

            Rectangle {
                anchors.right: parent.right
                anchors.rightMargin: 16
                anchors.verticalCenter: parent.verticalCenter
                width: 26
                height: 26
                radius: 13
                color: closeHover.containsMouse ? "#ff5f56" : Qt.rgba(1, 1, 1, 0.15)

                Text {
                    anchors.centerIn: parent
                    text: "✕"
                    color: "#ffffff"
                    font.pixelSize: 12
                }

                MouseArea {
                    id: closeHover
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: root.isOpen = false
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

        // Content
        Column {
            anchors.top: header.bottom
            anchors.topMargin: 16
            anchors.left: parent.left
            anchors.leftMargin: 24
            anchors.right: parent.right
            anchors.rightMargin: 24
            spacing: 16

            // Row 1: Icon Size
            Row {
                width: parent.width
                spacing: 10
                Text {
                    width: 140
                    text: "Icon Size: " + (dockSettings ? dockSettings.iconSize : 64) + "px"
                    color: "#d0d4dc"
                    font.pixelSize: 13
                    anchors.verticalCenter: parent.verticalCenter
                }
                Item {
                    width: 240
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
                        property real ratio: (dockSettings ? (dockSettings.iconSize - 36) / (128 - 36) : 0.3)
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

            // Row 2: Magnification
            Row {
                width: parent.width
                spacing: 10
                Text {
                    width: 140
                    text: "Magnification: " + (dockSettings ? Math.round(dockSettings.magnificationFactor * 100) : 136) + "%"
                    color: "#d0d4dc"
                    font.pixelSize: 13
                    anchors.verticalCenter: parent.verticalCenter
                }
                Item {
                    width: 240
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
                        property real ratio: (dockSettings ? (dockSettings.magnificationFactor - 1.0) / (2.0 - 1.0) : 0.36)
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

            // Row 3: Spacing
            Row {
                width: parent.width
                spacing: 10
                Text {
                    width: 140
                    text: "Icon Spacing: " + (dockSettings ? dockSettings.iconSpacing : 14) + "px"
                    color: "#d0d4dc"
                    font.pixelSize: 13
                    anchors.verticalCenter: parent.verticalCenter
                }
                Item {
                    width: 240
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
                        property real ratio: (dockSettings ? (dockSettings.iconSpacing - 4) / (32 - 4) : 0.35)
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
                    width: 140
                    text: "Glass Opacity: " + (dockSettings ? Math.round(dockSettings.backgroundOpacity * 100) : 25) + "%"
                    color: "#d0d4dc"
                    font.pixelSize: 13
                    anchors.verticalCenter: parent.verticalCenter
                }
                Item {
                    width: 240
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
                        property real ratio: (dockSettings ? (dockSettings.backgroundOpacity - 0.05) / (0.90 - 0.05) : 0.25)
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
                    width: 140
                    text: "Bottom Margin: " + (dockSettings ? dockSettings.bottomMargin : 11) + "px"
                    color: "#d0d4dc"
                    font.pixelSize: 13
                    anchors.verticalCenter: parent.verticalCenter
                }
                Item {
                    width: 240
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
                        property real ratio: (dockSettings ? dockSettings.bottomMargin / 40.0 : 0.27)
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

            // Toggles
            // Magnification Toggle
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

            // Running Indicators Toggle
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

            // Auto-hide Toggle
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
        }

        // Footer buttons
        Row {
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 16
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
                    onClicked: root.isOpen = false
                }
            }
        }
    }
}
