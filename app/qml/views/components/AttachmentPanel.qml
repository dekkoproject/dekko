import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Components 1.0

Rectangle {
    id: attachmentPanel

    property var attachments
    property int maxHeight
    readonly property int expandedHeight: internalHeight > maxHeight ? maxHeight : internalHeight
    readonly property int internalHeight: header.height + col.height
    readonly property int collapsedHeight: attachmentPanel.visible ? header.height : 0
    readonly property bool expanded: state === "expanded"
    color: "#ffffff"
    visible: attachments.model.count

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: mouse.accepted = true
        onWheel: wheel.accepted = true
    }
    Rectangle {
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        height: units.dp(1)
        color: UbuntuColors.lightGrey
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
            name: Icons.AttachmentIcon
            height: units.gu(2.5)
            width: height
            anchors {
                left: parent.left
                leftMargin: units.gu(2)
                verticalCenter: parent.verticalCenter
            }
        }

        Label {
            id: l
            anchors {
                left: ai.right
                leftMargin: units.gu(1)
                verticalCenter: parent.verticalCenter
            }
            text: qsTr("Attachments")
        }

        UbuntuShape {
            id: shape
            anchors {
                left: l.right
                leftMargin: units.gu(1)
                verticalCenter: parent.verticalCenter
            }

            aspect: UbuntuShape.Flat
            color: UbuntuColors.porcelain
            height: units.gu(2.2)
            width: countLable.width < height ? height : countLable.width + units.gu(1)
            Label {
                id: countLable
                anchors.margins: units.gu(0.5)
                anchors.centerIn: parent
                fontSize: "small"
                text: attachments.model.count
            }
        }

        Icon {
            id: icon
            name: "up"
            anchors {
                right: parent.right
                rightMargin: units.gu(2)
                verticalCenter: parent.verticalCenter
            }
            color: UbuntuColors.ash
            rotation: attachmentsPanel.expanded ? 180 : 0
            height: units.gu(2); width: height
            state: attachmentPanel.expanded ? "rotated" : "normal"
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
            contentHeight: col.height
            clip: true
            Column {
                id: col
                anchors {
                    left: parent.left
                    top: parent.top
                    right: parent.right
                }
                Repeater {
                    model: attachments ? attachments.model : 0
                    delegate: ListItem {
                        property Component openItem: Item{}
                        property var attachment: model.qtObject
                        height: aLayout.height

                        Connections {
                            target: model.qtObject
                            onReadyToOpen: {
                                Qt.openUrlExternally(url)
                            }
                        }
                        onClicked: {
                            // This really is a crude hack
                            // the attachments object can't directly
                            // access the custom qnam factory. So we pass
                            // it a QObject from the qml context which
                            // the attachment object can access the QQmlEngine from
                            // FIXME: Refactor Client* classes into seperate lib
                            model.qtObject.open(openItem.createObject())
                        }
                        ListItemLayout {
                            id: aLayout
                            height: units.gu(6)
                            title.text: attachment.displayName
                            subtitle.text: attachment.mimeType + ", " + attachment.size
                            Icon {
                                name: attachment.mimeTypeIcon
                                color: "#888888"
                                height: units.gu(4); width: height
                                SlotsLayout.position: SlotsLayout.Leading
                            }
                            ActivityIndicator {
                                visible: running
                                running: attachment.fetchInProgress
                            }

                            CachedImage {
                                height: units.gu(2)
                                width: height
                                name: Icons.ContextMenuIcon
                                AbstractButton {
                                    anchors.fill: parent
                                    onClicked: dekko.showNotice("Not implemented. Fix before release")
                                }
                            }
                        }
                    }
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
                target: attachmentPanel
                height: collapsedHeight
            }
        },
        State {
            name: "expanded"
            PropertyChanges {
                target: attachmentPanel
                height: expandedHeight
            }
        }
    ]
}

