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

    // Smooth spring-like interpolation towards target scale
    Behavior on currentScale {
        NumberAnimation {
            duration: 90
            easing.type: Easing.OutQuad
        }
    }

    width: (dockSettings ? dockSettings.iconSize : 64) * currentScale
    height: (dockSettings ? dockSettings.dockHeight : 92)

    property real baseIconSize: dockSettings ? dockSettings.iconSize : 64
    property real scaledIconSize: baseIconSize * currentScale
    property real baseBottomPadding: (height - baseIconSize) / 2.0

    property real bounceOffset: 0.0

    SequentialAnimation {
        id: bounceAnim
        running: false

        NumberAnimation {
            target: root
            property: "bounceOffset"
            to: -26
            duration: 160
            easing.type: Easing.OutQuad
        }
        NumberAnimation {
            target: root
            property: "bounceOffset"
            to: 0
            duration: 180
            easing.type: Easing.InQuad
        }
        NumberAnimation {
            target: root
            property: "bounceOffset"
            to: -14
            duration: 130
            easing.type: Easing.OutQuad
        }
        NumberAnimation {
            target: root
            property: "bounceOffset"
            to: 0
            duration: 150
            easing.type: Easing.InQuad
        }
        NumberAnimation {
            target: root
            property: "bounceOffset"
            to: -6
            duration: 90
            easing.type: Easing.OutQuad
        }
        NumberAnimation {
            target: root
            property: "bounceOffset"
            to: 0
            duration: 110
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

    // Tooltip above icon
    DockTooltip {
        id: tooltip
        anchors.horizontalCenter: iconContainer.horizontalCenter
        anchors.bottom: iconContainer.top
        anchors.bottomMargin: 14
        text: root.name
        active: root.isDirectlyHovered && bounceOffset === 0
        z: 100
    }

    // Icon Container
    Item {
        id: iconContainer
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: root.baseBottomPadding + (root.scaledIconSize - root.baseIconSize) * 0.15
        width: root.scaledIconSize
        height: root.scaledIconSize
        y: root.bounceOffset

        // Subtle drop shadow under icon tile
        Rectangle {
            anchors.centerIn: parent
            anchors.verticalCenterOffset: 3
            width: parent.width * 0.94
            height: parent.height * 0.94
            radius: width * 0.22
            color: "#35000000"
            z: 0
        }

        // Icon Graphic
        Image {
            id: iconImg
            anchors.fill: parent
            source: root.iconSource
            sourceSize.width: 128
            sourceSize.height: 128
            fillMode: Image.PreserveAspectFit
            smooth: true
            mipmap: true
            antialiasing: true
            z: 1
        }
    }

    // Running indicator dot centered beneath the icon
    Rectangle {
        id: runningDot
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Math.max(3, (root.baseBottomPadding - 5) / 2.0)
        width: 5
        height: 5
        radius: 2.5
        // Reference image shows a dark circular dot under active app
        color: "#182028"
        visible: (dockSettings ? dockSettings.showRunningIndicators : true) && root.isRunning
        opacity: visible ? 1.0 : 0.0

        Behavior on opacity {
            NumberAnimation { duration: 200 }
        }
    }
}
