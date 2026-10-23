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

    // Auto-hide state synchronized with DockWindow edge trigger
    property bool isHidden: (dockSettings ? dockSettings.autoHide : false) && 
                            (typeof dockWindow !== "undefined" && dockWindow ? !dockWindow.isRevealed : false) && 
                            !settingsDialog.isOpen

    Connections {
        target: typeof dockWindow !== "undefined" ? dockWindow : null
        function onRevealedChanged() {
            if (typeof dockWindow !== "undefined" && dockWindow) {
                root.isHidden = (dockSettings ? dockSettings.autoHide : false) && !dockWindow.isRevealed && !settingsDialog.isOpen
            }
        }
    }

    onIsHiddenChanged: {
        if (typeof platformIntegration !== "undefined" && platformIntegration && typeof dockWindow !== "undefined" && dockWindow) {
            platformIntegration.setWindowBlur(dockWindow, !root.isHidden)
        }
    }

    Item {
        id: slidingWrapper
        anchors.fill: parent
        y: root.isHidden ? (dockContainer.dockHeight + 24) : 0

        Behavior on y {
            NumberAnimation {
                duration: 260
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
