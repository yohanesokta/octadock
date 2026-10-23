import QtQuick

Item {
    id: root

    property real baseIconSize: dockSettings ? dockSettings.iconSize : 58
    property real iconSpacing: dockSettings ? dockSettings.iconSpacing : 18
    property real verticalPadding: dockSettings ? dockSettings.verticalPadding : 12
    property real horizontalPadding: dockSettings ? dockSettings.horizontalPadding : 22
    property real dockHeight: dockSettings ? dockSettings.dockHeight : 84
    property real cornerRadius: dockSettings ? dockSettings.cornerRadius : 24
    property real bgOpacity: dockSettings ? dockSettings.backgroundOpacity : 0.28
    property real borderOpacity: dockSettings ? dockSettings.borderOpacity : 0.32

    property real totalIconsWidth: iconsRow.width
    property bool mouseInside: dockMouseArea.containsMouse

    // Total width adapts to icons + horizontal padding
    width: Math.max(120, totalIconsWidth + horizontalPadding * 2)
    // Headroom for magnified icons expanding upwards
    height: dockHeight + 36

    // Floating Glass Capsule Container
    Rectangle {
        id: dockGlass
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width
        height: root.dockHeight
        radius: root.cornerRadius

        // Frosted glass cool blue-gray translucent tint matching reference
        color: Qt.rgba(0.86, 0.91, 0.97, root.bgOpacity)
        border.color: Qt.rgba(1.0, 1.0, 1.0, root.borderOpacity)
        border.width: 1.0

        // Soft restrained outer drop shadows (dual layer for smooth ambient falloff)
        Rectangle {
            anchors.fill: parent
            anchors.topMargin: 3
            anchors.bottomMargin: -5
            anchors.leftMargin: -4
            anchors.rightMargin: -4
            radius: root.cornerRadius + 4
            color: "#18000000"
            z: -2
        }

        Rectangle {
            anchors.fill: parent
            anchors.topMargin: 1
            anchors.bottomMargin: -2
            anchors.leftMargin: -1
            anchors.rightMargin: -1
            radius: root.cornerRadius + 2
            color: "#24000000"
            z: -1
        }

        // Inner top glass reflection highlight
        Rectangle {
            anchors.top: parent.top
            anchors.topMargin: 1
            anchors.left: parent.left
            anchors.leftMargin: root.cornerRadius
            anchors.right: parent.right
            anchors.rightMargin: root.cornerRadius
            height: 1
            color: Qt.rgba(1.0, 1.0, 1.0, 0.40)
        }

        // Icon Row: precisely centered vertically within the dock glass
        Row {
            id: iconsRow
            anchors.centerIn: parent
            spacing: root.iconSpacing
            z: 2

            Repeater {
                id: iconRepeater
                model: dockModel

                DockIcon {
                    id: iconItem
                    itemIndex: index
                    itemId: model.itemId
                    name: model.name
                    iconSource: model.iconSource
                    execCommand: model.execCommand
                    windowId: model.windowId
                    isPinned: model.isPinned
                    isRunning: model.isRunning

                    onClicked: {
                        dockModel.launch(index)
                    }

                    onRightClicked: function(mx, my) {
                        if (typeof dockWindow !== "undefined" && dockWindow) {
                            var pt = iconItem.mapToGlobal(iconItem.width / 2, 0)
                            dockWindow.showContextMenu(index, model.name, model.itemId, model.execCommand, Math.round(pt.x), Math.round(pt.y))
                        }
                    }
                }
            }
        }

        // Mouse tracking across dock for smooth magnification wave and background interaction
        MouseArea {
            id: dockMouseArea
            anchors.fill: parent
            anchors.topMargin: -36 // allow pointer hover slightly above dock
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            z: 1

            property real hoveredDockX: -1

            onPositionChanged: function(mouse) {
                hoveredDockX = mouse.x
                updateMagnification(mouse.x, mouse.y)
            }

            onEntered: {
                if (typeof dockWindow !== "undefined" && dockWindow) {
                    dockWindow.setRevealed(true)
                }
            }

            onExited: {
                hoveredDockX = -1
                resetMagnification()
            }

            onClicked: function(mouse) {
                if (mouse.button === Qt.RightButton) {
                    if (typeof dockWindow !== "undefined" && dockWindow) {
                        var pt = dockGlass.mapToGlobal(mouse.x, 0)
                        dockWindow.showContextMenu(-1, "", "", "", Math.round(pt.x), Math.round(pt.y))
                    }
                }
            }
        }
    }

    function updateMagnification(mouseX, mouseY) {
        if (!dockSettings || !dockSettings.enableMagnification) {
            resetMagnification()
            return
        }

        var maxFactor = dockSettings.magnificationFactor
        var radius = root.baseIconSize * dockSettings.magnificationRange

        for (var i = 0; i < iconRepeater.count; ++i) {
            var item = iconRepeater.itemAt(i)
            if (!item) continue

            // Compute icon center X in dockMouseArea coordinates
            var itemCenterPt = item.mapToItem(dockMouseArea, item.width / 2.0, 0)
            var dist = Math.abs(mouseX - itemCenterPt.x)

            if (dist < radius) {
                var normDist = dist / radius
                var wave = 0.5 * (1.0 + Math.cos(Math.PI * normDist))
                item.targetScale = 1.0 + (maxFactor - 1.0) * wave
            } else {
                item.targetScale = 1.0
            }

            var pt = item.mapFromItem(dockMouseArea, mouseX, mouseY)
            item.isDirectlyHovered = (pt.x >= 0 && pt.x <= item.width && pt.y >= -36 && pt.y <= item.height + 24)
        }
    }

    function resetMagnification() {
        for (var i = 0; i < iconRepeater.count; ++i) {
            var item = iconRepeater.itemAt(i)
            if (item) {
                item.targetScale = 1.0
                item.isDirectlyHovered = false
            }
        }
    }
}
