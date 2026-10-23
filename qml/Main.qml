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
    readonly property bool autoHideActive: dockSettings ? dockSettings.autoHide : false
    readonly property bool windowRevealed: typeof dockWindow !== "undefined" && dockWindow ? dockWindow.isRevealed : true
    readonly property bool isHidden: autoHideActive && !windowRevealed && !settingsDialog.isOpen

    onIsHiddenChanged: {
        if (typeof platformIntegration !== "undefined" && platformIntegration && typeof dockWindow !== "undefined" && dockWindow) {
            platformIntegration.setWindowBlur(dockWindow, !root.isHidden)
        }
    }

    Item {
        id: slidingWrapper
        anchors.fill: parent
        y: root.isHidden ? (dockContainer.dockHeight + 30) : 0

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
