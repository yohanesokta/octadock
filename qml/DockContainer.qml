import QtQuick

Item {
    id: root

    property real baseIconSize: dockSettings ? dockSettings.iconSize : 64
    property real iconSpacing: dockSettings ? dockSettings.iconSpacing : 14
    property real dockHeight: dockSettings ? dockSettings.dockHeight : 92
    property real cornerRadius: dockSettings ? dockSettings.cornerRadius : 28
    property real bgOpacity: dockSettings ? dockSettings.backgroundOpacity : 0.25

    // Dynamic width based on icons + horizontal padding
    property real totalIconsWidth: iconsRow.width
    property real horizontalPadding: 24

    width: Math.max(120, totalIconsWidth + horizontalPadding * 2)
    height: dockHeight + 30 // extra vertical headroom for magnified icons and shadows

    // Floating capsule container
    Rectangle {
        id: dockGlass
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        width: root.totalIconsWidth + root.horizontalPadding * 2
        height: root.dockHeight
        radius: root.cornerRadius

        // Soft frosted glass translucent tint matching reference
        color: Qt.rgba(0.84, 0.90, 0.96, root.bgOpacity)
        border.color: Qt.rgba(1.0, 1.0, 1.0, 0.42)
        border.width: 1.2

        // Soft ambient drop shadows beneath the dock
        Rectangle {
            anchors.fill: parent
            anchors.topMargin: 6
            anchors.bottomMargin: -10
            anchors.leftMargin: -8
            anchors.rightMargin: -8
            radius: root.cornerRadius + 8
            color: "#22000000"
            z: -2
        }

        Rectangle {
            anchors.fill: parent
            anchors.topMargin: 3
            anchors.bottomMargin: -5
            anchors.leftMargin: -3
            anchors.rightMargin: -3
            radius: root.cornerRadius + 4
            color: "#38000000"
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
            color: Qt.rgba(1.0, 1.0, 1.0, 0.45)
        }

        // Icon Row Container
        Row {
            id: iconsRow
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 4
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: root.iconSpacing

            Repeater {
                id: iconRepeater
                model: dockModel

                DockIcon {
                    id: iconItem
                    itemIndex: index
                    itemId: model.itemId
                    name: model.name
                    iconSource: model.iconSource
                    isRunning: model.isRunning

                    onClicked: {
                        dockModel.launch(index)
                    }

                    onRightClicked: function(mx, my) {
                        var pt = iconItem.mapToItem(root.parent, mx, my)
                        contextMenu.showAt(pt.x, pt.y, index, model.name, model.itemId, model.execCommand)
                    }
                }
            }
        }

        // Mouse tracking across dock for smooth magnification wave
        MouseArea {
            id: dockMouseArea
            anchors.fill: parent
            anchors.topMargin: -30 // allow hovering slightly above dock
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton | Qt.RightButton

            property real hoveredDockX: -1
            property bool isMouseOver: containsMouse

            onPositionChanged: function(mouse) {
                hoveredDockX = mouse.x
                updateMagnification(mouse.x, mouse.y)
            }

            onExited: {
                hoveredDockX = -1
                resetMagnification()
            }

            onClicked: function(mouse) {
                // Find clicked icon
                var iconFound = false
                for (var i = 0; i < iconRepeater.count; ++i) {
                    var item = iconRepeater.itemAt(i)
                    if (item) {
                        var pt = item.mapFromItem(dockGlass, mouse.x, mouse.y)
                        if (pt.x >= 0 && pt.x <= item.width && pt.y >= 0 && pt.y <= item.height) {
                            iconFound = true
                            if (mouse.button === Qt.RightButton) {
                                var globalPt = item.mapToItem(root.parent, pt.x, pt.y)
                                contextMenu.showAt(globalPt.x, globalPt.y, i, item.name, item.itemId, "")
                            } else {
                                dockModel.launch(i)
                            }
                            break
                        }
                    }
                }

                if (!iconFound && mouse.button === Qt.RightButton) {
                    var globalPt = dockGlass.mapToItem(root.parent, mouse.x, mouse.y)
                    contextMenu.showAt(globalPt.x, globalPt.y, -1, "", "", "")
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

            // Compute icon center X in dockGlass coordinates
            var itemCenterDockX = item.mapToItem(dockGlass, item.width / 2.0, 0).x
            var dist = Math.abs(mouseX - itemCenterDockX)

            if (dist < radius) {
                var normDist = dist / radius
                // Smooth cosine wave
                var wave = 0.5 * (1.0 + Math.cos(Math.PI * normDist))
                item.targetScale = 1.0 + (maxFactor - 1.0) * wave
            } else {
                item.targetScale = 1.0
            }

            // Directly hovered icon check
            var pt = item.mapFromItem(dockGlass, mouseX, mouseY)
            item.isDirectlyHovered = (pt.x >= 0 && pt.x <= item.width && pt.y >= 0 && pt.y <= item.height)
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
