import QtQuick

Item {
    id: root

    property real baseIconSize: dockSettings ? dockSettings.iconSize : 58
    property real iconSpacing: dockSettings ? dockSettings.iconSpacing : 18
    property real verticalPadding: dockSettings ? dockSettings.verticalPadding : 12
    property real horizontalPadding: dockSettings ? dockSettings.horizontalPadding : 22
    property real dockHeight: dockSettings ? dockSettings.dockHeight : 84
    property real bottomMargin: dockSettings ? dockSettings.bottomMargin : 6
    property real cornerRadius: dockSettings ? dockSettings.cornerRadius : 24
    property real bgOpacity: dockSettings ? dockSettings.backgroundOpacity : 0.28
    property real borderOpacity: dockSettings ? dockSettings.borderOpacity : 0.32

    property real totalIconsWidth: iconsRow.width
    property bool mouseInside: dockMouseArea.containsMouse

    // Total width adapts to icons + horizontal padding
    width: Math.max(120, totalIconsWidth + horizontalPadding * 2)
    // Headroom for magnified icons expanding upwards + bottom margin
    height: dockHeight + 36 + bottomMargin

    // Floating Glass Capsule Container
    Rectangle {
        id: dockGlass
        anchors.bottom: parent.bottom
        anchors.bottomMargin: root.bottomMargin
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
            z: 1

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
                }
            }
        }

        // Unified mouse tracking overlay for smooth magnification wave, tooltips, and clicks
        MouseArea {
            id: dockMouseArea
            anchors.fill: parent
            anchors.topMargin: -36 // allow pointer hover slightly above dock
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            z: 10

            property real hoveredDockX: -1
            property int hoveredItemIndex: -1
            cursorShape: hoveredItemIndex >= 0 ? Qt.PointingHandCursor : Qt.ArrowCursor

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
                hoveredItemIndex = -1
                resetMagnification()
            }

            onClicked: function(mouse) {
                var hitIndex = -1
                for (var i = 0; i < iconRepeater.count; ++i) {
                    var item = iconRepeater.itemAt(i)
                    if (item) {
                        var pt = item.mapFromItem(dockMouseArea, mouse.x, mouse.y)
                        var margin = root.iconSpacing / 2.0
                        if (pt.x >= -margin && pt.x <= (item.width + margin)) {
                            hitIndex = i
                            break
                        }
                    }
                }

                if (hitIndex >= 0) {
                    var targetItem = iconRepeater.itemAt(hitIndex)
                    if (mouse.button === Qt.RightButton) {
                        if (typeof dockWindow !== "undefined" && dockWindow) {
                            var gPt = targetItem.mapToGlobal(targetItem.width / 2, 0)
                            dockWindow.showContextMenu(hitIndex, targetItem.name, targetItem.itemId, targetItem.execCommand, Math.round(gPt.x), Math.round(gPt.y))
                        }
                    } else if (mouse.button === Qt.LeftButton) {
                        dockModel.launch(hitIndex)
                    }
                } else if (mouse.button === Qt.RightButton) {
                    if (typeof dockWindow !== "undefined" && dockWindow) {
                        var gPt2 = dockGlass.mapToGlobal(mouse.x, 0)
                        dockWindow.showContextMenu(-1, "", "", "", Math.round(gPt2.x), Math.round(gPt2.y))
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

        var foundIndex = -1
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
            var isHovered = (pt.x >= 0 && pt.x <= item.width && pt.y >= -36 && pt.y <= item.height + 24)
            item.isDirectlyHovered = isHovered
            if (isHovered) {
                foundIndex = i
            }
        }
        dockMouseArea.hoveredItemIndex = foundIndex
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
