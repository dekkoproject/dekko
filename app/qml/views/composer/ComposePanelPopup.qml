import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Dekko.Components 1.0
import "../../actions/composer"
import "../components"

PopupBase {
    id: base

    readonly property int maxHeight: units.gu(70)
    readonly property int maxWidth: units.gu(100)

    Rectangle {
        anchors.fill: parent
        color: UbuntuColors.graphite
        opacity: 0.6
    }

    UbuntuShape {
        readonly property bool shouldCenter: height === base.maxHeight
        readonly property bool addTopMargin: height > units.gu(40)
        aspect: UbuntuShape.DropShadow
        width: Math.min((parent.width / 3)*2, base.maxWidth)
        height: Math.min((parent.height / 5)*4, base.maxHeight)
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: shouldCenter ? undefined : parent.top
            topMargin: shouldCenter ? undefined : (addTopMargin ? units.gu(6) : units.gu(2))
            verticalCenter: shouldCenter ? parent.verticalCenter : undefined
        }
        color: "#FFFFFF"
        clip: true

        StretchColumn {
            anchors.fill: parent
            Item {
                anchors {
                    top: parent.top
                    right: parent.right
                    left: parent.left
                }
                height: units.gu(5)
                implicitHeight: height

                StretchRow {
                    anchors.fill: parent
                    anchors.leftMargin: units.gu(1)
                    anchors.rightMargin: units.gu(1)
                    spacing: units.gu(0.5)
                    TitledHeaderButton {
                        height: parent.height
                        iconName: Icons.DeleteIcon
                        action: Action {
                            text: qsTr("Discard")
                            onTriggered: {
                                ComposerActions.discardMessage()
                                PopupUtils.close(base)
                            }
                        }
                    }
                    TitledHeaderButton {
                        height: parent.height
                        iconName: Icons.DraftIcon
                        action: Action {
                            text: qsTr("Save draft")
                            onTriggered: {
                                ComposerActions.saveDraft()
                                PopupUtils.close(base)
                            }
                        }
                    }

                    Stretcher {}

                    TitledHeaderButton {
                        height: parent.height
                        iconName: Icons.SendIcon
                        action: Action {
                            text: qsTr("Send")
                            onTriggered: {
                                ComposerActions.sendMessage()
                                PopupUtils.close(base)
                            }
                        }
                    }
                }

                Line {
                    anchors {
                        bottom: parent.bottom
                        right: parent.right
                        left: parent.left
                    }
                }
            }

            Stretcher {
                id: content
                MessageComposer {
                    anchors.fill: parent
                }
            }
            Item {
                id: statusBar
                height: units.gu(3)
                width: parent.width
                implicitWidth: width
                implicitHeight: height
            }
        }
    }
}

