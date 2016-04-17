import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Dekko.Components 1.0
import "../components"

Popover {
    id: actionPopover
    z: 100 // Prevent being rendered below the message header
    property var msg
    property var ctxtModel

    Column {
        id: containerLayout

        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }

        ContextGroup {
            // only show this group on links
            visible: ctxtModel && ctxtModel.linkUrl.toString()
            contextActions: [
                ContextAction {
                    description: qsTr("Open in browser")
                    actionIcon: Icons.BrowserIcon
                    onTriggered: {
                        Qt.openUrlExternally(ctxtModel.linkUrl.toString())
                        PopupUtils.close(actionPopover)
                    }
                },
                ContextAction {
                    description: qsTr("Copy link")
                    actionIcon: Icons.CopyIcon
                    onTriggered: {
                        Clipboard.push(["text/plain", ctxtModel.linkUrl.toString()])
                        PopupUtils.close(actionPopover)
                    }
                },
                ContextAction {
                    description: qsTr("Share link")
                    actionIcon: Icons.ShareIcon
                    onTriggered: {
                        dekko.showNotice("not implemented yet. Fix it before release!!!!")
                        PopupUtils.close(actionPopover)
                    }
                }

            ]
        }

        ContextGroup {
            contextActions: [
                ContextAction {
                    description: qsTr("Reply")
                    actionIcon: Icons.MailRepliedIcon
                    onTriggered: {
                        dekko.showNotice("not implemented yet. Fix it before release!!!!")
                        PopupUtils.close(actionPopover)
                    }
                },

                ContextAction {
                    description: qsTr("Reply all")
                    actionIcon: Icons.MailRepliedAllIcon
                    onTriggered: {
                        dekko.showNotice("not implemented yet. Fix it before release!!!!")
                        PopupUtils.close(actionPopover)
                    }
                },
                ContextAction {
                    description: qsTr("Forward")
                    actionIcon: Icons.MailForwardedIcon
                    onTriggered: {
                        dekko.showNotice("Not implemented yet. Fix it before release!!!!")
                        PopupUtils.close(actionPopover)
                    }
                }
            ]
        }
        ContextGroup {
            contextActions: [
                ContextAction {
                    description: qsTr("View source")
                    actionIcon: Icons.NoteIcon
                    onTriggered: {
                        dekko.showNotice("not implemented yet. Fix it before release!!!!")
                        PopupUtils.close(actionPopover)
                    }
                }
            ]
        }
    }
}

