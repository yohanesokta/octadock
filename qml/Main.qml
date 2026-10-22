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
            settingsDialog.showSettings()
        }
    }

    // Auto-hide sliding offset
    property bool isHidden: (dockSettings ? dockSettings.autoHide : false) && !dockContainer.mouseInside && !settingsDialog.isOpen

    Timer {
        id: hideTimer
        interval: dockSettings ? dockSettings.autoHideDelay : 500
        repeat: false
        onTriggered: {
            if (dockSettings && dockSettings.autoHide && !dockContainer.mouseInside && !settingsDialog.isOpen) {
                root.isHidden = true
            }
        }
    }

    Connections {
        target: dockContainer
        function onMouseInsideChanged() {
            if (dockContainer.mouseInside) {
                hideTimer.stop()
                root.isHidden = false
            } else if (dockSettings && dockSettings.autoHide) {
                hideTimer.restart()
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
        }
    }

    // Preferences Dialog (Floating top-level window)
    SettingsDialog {
        id: settingsDialog
    }
}
