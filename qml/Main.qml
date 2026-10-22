import QtQuick

Item {
    id: root

    width: dockContainer.width
    height: dockContainer.height

    onWidthChanged: {
        if (typeof dockWindow !== "undefined" && dockWindow) {
            dockWindow.updateDockGeometry(width, height)
        }
    }
    onHeightChanged: {
        if (typeof dockWindow !== "undefined" && dockWindow) {
            dockWindow.updateDockGeometry(width, height)
        }
    }

    Connections {
        target: appLauncher
        function onOpenSettingsRequested() {
            settingsDialog.isOpen = true
        }
    }

    // Auto-hide sliding offset
    property bool isHidden: (dockSettings ? dockSettings.autoHide : false) && !dockHoverArea.containsMouse && !settingsDialog.isOpen && !contextMenu.isOpen

    Timer {
        id: hideTimer
        interval: dockSettings ? dockSettings.autoHideDelay : 500
        repeat: false
        onTriggered: {
            if (dockSettings && dockSettings.autoHide && !dockHoverArea.containsMouse && !settingsDialog.isOpen && !contextMenu.isOpen) {
                root.isHidden = true
            }
        }
    }

    Item {
        id: slidingWrapper
        anchors.fill: parent
        y: root.isHidden ? (parent.height - 4) : 0

        Behavior on y {
            NumberAnimation {
                duration: 250
                easing.type: Easing.OutCubic
            }
        }

        DockContainer {
            id: dockContainer
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
        }

        MouseArea {
            id: dockHoverArea
            anchors.fill: parent
            hoverEnabled: true
            propagateComposedEvents: true
            onEntered: {
                hideTimer.stop()
                root.isHidden = false
            }
            onExited: {
                if (dockSettings && dockSettings.autoHide) {
                    hideTimer.restart()
                }
            }
        }
    }

    // Context Menu
    ContextMenu {
        id: contextMenu
        anchors.fill: parent
        z: 900

        onOpenApp: function(idx) {
            dockModel.launch(idx)
        }
        onRemoveApp: function(idx) {
            dockModel.removeItem(idx)
        }
        onOpenPreferences: {
            settingsDialog.isOpen = true
        }
        onQuitApp: {
            Qt.quit()
        }
    }

    // Preferences Dialog
    SettingsDialog {
        id: settingsDialog
        anchors.fill: parent
        z: 1000
    }
}
