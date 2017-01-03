/* Copyright (C) 2016 Dan Chapman <dpniel@ubuntu.com>

   This file is part of Dekko email client for Ubuntu devices

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Dekko.Mail 1.0
import Dekko.Mail.API 1.0
import Dekko.Components 1.0
import "../components"

Popover {
    id: actionPopover
    property var msg

    contentWidth: units.gu(30)

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
                        MessageActions.markMessageRead(msg.messageId, !msg.isRead)
                        PopupUtils.close(actionPopover)
                    }
                },
                ContextAction {
                    description: msg && msg.isImportant ? qsTr("Mark as not important") : qsTr("Mark as important")
                    actionIcon: msg && msg.isImportant ? Icons.UnStarredIcon : Icons.StarredIcon
                    onTriggered: {
                        MessageActions.markMessageImportant(msg.messageId, !msg.isImportant)
                        PopupUtils.close(actionPopover)
                    }
                },
                ContextAction {
                    description: qsTr("Mark as spam")
                    actionIcon: Icons.JunkIcon
                    onTriggered: {
                        PopupActions.showNotice("Mark as spam not implemented yet. Fix it before release!!!!")
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
                            MessageActions.markMessageAsDone(msg.messageId, false)
                        }
                        MessageActions.markMessageAsTodo(msg.messageId, !msg.isTodo)
                        PopupUtils.close(actionPopover)
                    }
                },
                ContextAction {
                    visible: msg && msg.isTodo
                    description: qsTr("Done")
                    actionIcon: Icons.SelectIcon
                    onTriggered: {
                        // markMessageDone will remove the Todo flag if it is set
                        MessageActions.markMessageAsDone(msg.messageId, !msg.isDone)
                        PopupUtils.close(actionPopover)
                    }
                }
            ]
        }

        ContextGroup {
            contextActions: [
                ContextAction {
                    description: qsTr("Reply")
                    actionIcon: Icons.MailRepliedAllIcon
                    onTriggered: {
                        ComposerActions.respondToMessage(msg.isListPost ? SubmissionManager.ReplyList : SubmissionManager.Reply, msg.messageId)
                        PopupUtils.close(actionPopover)
                    }
                },
                ContextAction {
                    description: qsTr("Reply all")
                    actionIcon: Icons.MailRepliedIcon
                    onTriggered: {
                        ComposerActions.respondToMessage(SubmissionManager.ReplyAll, msg.messageId)
                        PopupUtils.close(actionPopover)
                    }
                },
                ContextAction {
                    description: qsTr("Forward")
                    actionIcon: Icons.MailForwardedIcon
                    onTriggered: {
                        ComposerActions.forwardMessage(SubmissionManager.ForwardInline, msg.messageId)
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
                        MailboxActions.moveMessage(msg.parentAccountId, msg.messageId)
                        PopupUtils.close(actionPopover)
                    }
                },

                ContextAction {
                    description: msg ? qsTr("Restore to %1").arg(msg.previousFolderName) : ""
                    actionIcon: Icons.UndoIcon
                    visible: msg && msg.canBeRestored
                    onTriggered: {
                        MessageActions.restoreMessage(msg.messageId)
                        PopupUtils.close(actionPopover)
                    }
                },
                ContextAction {
                    description: qsTr("Delete")
                    actionIcon: Icons.DeleteIcon
                    onTriggered: {
                        MessageActions.deleteMessage(msg.messageId)
                        PopupUtils.close(actionPopover)
                    }
                }
            ]
        }
    }
}
