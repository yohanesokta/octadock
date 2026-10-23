import QtQuick

Item {
    id: root

    property int itemIndex: 0
    property string itemId: ""
    property string name: ""
    property string iconSource: ""
    property bool isRunning: false
    property real targetScale: 1.0
    property real currentScale: 1.0
    property bool isDirectlyHovered: false

    signal clicked()
    signal rightClicked(real mouseX, real mouseY)

    Behavior on currentScale {
        NumberAnimation {
            duration: 85
            easing.type: Easing.OutQuad
        }
    }

    property real baseIconSize: dockSettings ? dockSettings.iconSize : 58
    property real scaledIconSize: baseIconSize * currentScale
    property real cornerRadiusRatio: dockSettings ? dockSettings.iconCornerRadiusRatio : 0.225

    width: baseIconSize * currentScale
    height: baseIconSize

    property real bounceOffset: 0.0

    SequentialAnimation {
        id: bounceAnim
        running: false

        NumberAnimation {
            target: root
            property: "bounceOffset"
            to: -24
            duration: 150
            easing.type: Easing.OutQuad
        }
        NumberAnimation {
            target: root
            property: "bounceOffset"
            to: 0
            duration: 170
            easing.type: Easing.InQuad
        }
        NumberAnimation {
            target: root
            property: "bounceOffset"
            to: -12
            duration: 120
            easing.type: Easing.OutQuad
        }
        NumberAnimation {
            target: root
            property: "bounceOffset"
            to: 0
            duration: 140
            easing.type: Easing.InQuad
        }
        NumberAnimation {
            target: root
            property: "bounceOffset"
            to: -4
            duration: 80
            easing.type: Easing.OutQuad
        }
        NumberAnimation {
            target: root
            property: "bounceOffset"
            to: 0
            duration: 100
            easing.type: Easing.InQuad
        }
    }

    Connections {
        target: dockModel
        function onBounceTriggered(idx) {
            if (idx === root.itemIndex) {
                bounceAnim.restart()
            }
        }
    }

    // Floating Tooltip above icon
    DockTooltip {
        id: tooltip
        anchors.horizontalCenter: iconTile.horizontalCenter
        anchors.bottom: iconTile.top
        anchors.bottomMargin: 10
        text: root.name
        active: root.isDirectlyHovered && bounceOffset === 0
        z: 200
    }

    // Icon Tile with squircle drop shadow
    Item {
        id: iconTile
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        width: root.scaledIconSize
        height: root.scaledIconSize
        y: root.bounceOffset

        // Subtle tile drop shadow
        Rectangle {
            anchors.centerIn: parent
            anchors.verticalCenterOffset: 2
            width: parent.width * 0.96
            height: parent.height * 0.96
            radius: Math.round(width * root.cornerRadiusRatio)
            color: "#30000000"
            z: 0
        }

        // Native smooth antialiased SVG / System Theme icon
        Image {
            id: iconImg
            anchors.fill: parent
            source: "image://systemicon/" + (root.iconSource.length > 0 ? root.iconSource : root.itemId)
            sourceSize.width: 128
            sourceSize.height: 128
            fillMode: Image.PreserveAspectFit
            smooth: true
            mipmap: true
            antialiasing: true
            z: 1
        }
    }

    // Running indicator dot centered beneath the icon inside bottom padding
    Rectangle {
        id: runningDot
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.bottom
        anchors.topMargin: 3
        width: 4.5
        height: 4.5
        radius: 2.25
        color: "#182028"
        visible: (dockSettings ? dockSettings.showRunningIndicators : true) && root.isRunning
        opacity: visible ? 1.0 : 0.0

        Behavior on opacity {
            NumberAnimation { duration: 180 }
        }
    }
}
