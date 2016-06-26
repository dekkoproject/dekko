import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Components 1.0
import "../../constants"

Rectangle {
    id: expandingPanel
    default property alias content: content.data
    property alias iconName: ai.name
    property alias text: l.text
    property alias countText: countLable.text
    property int maxHeight
    readonly property int expandedHeight: internalHeight > maxHeight ? maxHeight : internalHeight
    readonly property int internalHeight: header.height + content.height
    readonly property int collapsedHeight: expandingPanel.visible ? header.height : 0
    readonly property bool expanded: state === "expanded"
    color: "#ffffff"

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: mouse.accepted = true
        onWheel: wheel.accepted = true
    }

    Line {
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
    }

    ListItem {
        id: header
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        height: units.gu(6)
        onClicked: d.isExpanded = !d.isExpanded

        CachedImage {
            id: ai
            height: Style.defaultIconSize
            width: height
            anchors {
                left: parent.left
                leftMargin: Style.defaultSpacing
                verticalCenter: parent.verticalCenter
            }
        }

        Label {
            id: l
            anchors {
                left: ai.right
                leftMargin: Style.smallSpacing
                verticalCenter: parent.verticalCenter
            }
            text: qsTr("Attachments")
        }

        UbuntuShape {
            id: shape
            anchors {
                left: l.right
                leftMargin: Style.smallSpacing
                verticalCenter: parent.verticalCenter
            }
            visible: countLable.text

            aspect: UbuntuShape.Flat
            color: UbuntuColors.porcelain
            height: units.gu(2.2)
            width: countLable.width < height ? height : countLable.width + Style.smallSpacing
            Label {
                id: countLable
                anchors.margins: units.gu(0.5)
                anchors.centerIn: parent
                fontSize: "small"
            }
        }

        Icon {
            id: icon
            name: "up"
            anchors {
                right: parent.right
                rightMargin: Style.defaultSpacing
                verticalCenter: parent.verticalCenter
            }
            color: UbuntuColors.ash
            rotation: expandingPanel.expanded ? 180 : 0
            height: Style.defaultSpacing; width: height
            state: expandingPanel.expanded ? "rotated" : "normal"
            states: [
                State {
                    name: "rotated"
                    PropertyChanges { target: icon; rotation: 180 }
                },
                State {
                    name: "normal"
                    PropertyChanges { target: icon; rotation: 0 }
                }
            ]
            transitions: Transition {
                RotationAnimation {
                    duration: UbuntuAnimation.FastDuration
                    direction: icon.state === "normal"  ? RotationAnimation.Clockwise : RotationAnimation.Counterclockwise
                }
            }
        }
    }
    ScrollView {
        anchors {
            left: parent.left
            top: header.bottom
            right: parent.right
            bottom: parent.bottom
        }
        Flickable {
            anchors.fill: parent
            contentHeight: content.height
            clip: true
            Column {
                id: content
                anchors {
                    left: parent.left
                    top: parent.top
                    right: parent.right
                }
            }
        }
    }
    QtObject {
        id: d
        property bool isExpanded: false
    }

    Behavior on height {
        UbuntuNumberAnimation{}
    }

    state: d.isExpanded ? "expanded" : "collapsed"
    states: [
        State {
            name: "collapsed"
            PropertyChanges {
                target: expandingPanel
                height: collapsedHeight
            }
        },
        State {
            name: "expanded"
            PropertyChanges {
                target: expandingPanel
                height: expandedHeight
            }
        }
    ]
}
