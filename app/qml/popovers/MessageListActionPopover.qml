import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Dekko.Mail 1.0
import Dekko.Components 1.0
import "../components"

Popover {
    id: actionPopover
    property var msg

    Column {
        id: containerLayout

        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }

        ContextGroup {
            contextActions: [
                ContextAction {
                    description: msg && msg.isRead ? qsTr("Mark as unread") : qsTr("Mark as read")
                    actionIcon: msg && msg.isRead ? Icons.MailUnreadIcon : Icons.MailReadIcon
                    onTriggered: {
                        Client.markMessageRead(msg.messageId, !msg.isRead);
                        PopupUtils.close(actionPopover)
                    }
                },
                ContextAction {
                    description: msg && msg.isImportant ? qsTr("Mark as not important") : qsTr("Mark as important")
                    actionIcon: msg && msg.isImportant ? Icons.UnStarredIcon : Icons.StarredIcon
                    onTriggered: {
                        Client.markMessageImportant(msg.messageId, !msg.isImportant)
                        PopupUtils.close(actionPopover)
                    }
                },
                ContextAction {
                    description: qsTr("Mark as spam")
                    actionIcon: Icons.JunkIcon
                    onTriggered: {
                        dekko.showNotice("Mark as spam not implemented yet. Fix it before release!!!!")
                        PopupUtils.close(actionPopover)
                    }
                }
            ]
        }

        ContextGroup {
            contextActions: [
                ContextAction {
                    description: qsTr("To-do")
                    actionIcon: Icons.ViewListSymbolic
                    selectedIcon: Icons.TickIcon
                    selectedIconColor: UbuntuColors.green
                    selected: msg && msg.isTodo
                    onTriggered: {
                        // remove done flag if re-marking as todo
                        if (msg.isDone) {
                            Client.markMessageDone(msg.messageId, false)
                        }
                        Client.markMessageTodo(msg.messageId, !msg.isTodo)
                        PopupUtils.close(actionPopover)
                    }
                },
                ContextAction {
                    visible: msg && msg.isTodo
                    description: qsTr("Done")
                    actionIcon: Icons.SelectIcon
                    onTriggered: {
                        // markMessageDone will remove the Todo flag if it is set
                        Client.markMessageDone(msg.messageId, !msg.isDone)
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
            divider.visible: false // last group so we don't need divider
            contextActions: [
                ContextAction {
                    description: qsTr("Move")
                    actionIcon: Icons.InboxIcon
                    onTriggered: {
                        dekko.showNotice("not implemented yet. Fix it before release!!!!")
                        PopupUtils.close(actionPopover)
                    }
                },

                ContextAction {
                    description: qsTr("Restore to %1").arg(msg.previousFolderName)
                    actionIcon: Icons.UndoIcon
                    onTriggered: {
                        Client.restoreMessage(msg.messageId)
                        PopupUtils.close(actionPopover)
                    }
                },
                ContextAction {
                    description: qsTr("Delete")
                    actionIcon: Icons.DeleteIcon
                    onTriggered: {
                        Client.deleteMessage(msg.messageId)
                        PopupUtils.close(actionPopover)
                    }
                }
            ]
        }
    }
}
