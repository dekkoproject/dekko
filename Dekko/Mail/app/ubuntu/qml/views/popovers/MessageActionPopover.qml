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
import Dekko.Components 1.0
import "../components"
import "../../actions/composer"
import "../../actions/messaging"
import "../../actions/popups"

Popover {
    id: actionPopover
    property var msg

    contentWidth: units.gu(25)

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
                }
            ]
        }
    }
}
