/* Copyright (C) 2016 - 2017 Dan Chapman <dpniel@ubuntu.com>

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
import Dekko.Components 1.0
import Dekko.Mail.API 1.0

/*!
*
* MailStoreActions API
*
* MailStoreActions provided as a readonly property of the MailStore.
*
* @ingroup qml_stores
*/
QtObject {

    property Action selectAll: Action {
        iconSource: !msgList.canSelectAll ? Paths.actionIconUrl(Paths.NoneSelectedIcon) :
                                            Paths.actionIconUrl(Paths.SelectIcon)
        text: msgList.canSelectAll ? qsTr("Unselect all") : qsTr("Select all")
        onTriggered: {
            if (msgList.canSelectAll) {
                MessageActions.selectAllMessages()
            } else {
                MessageActions.unselectAllMessages()
            }
        }
    }

    property Action flagSelected: Action {
        iconSource: msgList.canMarkSelectionImportant ? Paths.actionIconUrl(Paths.StarredIcon) :
                                                        Paths.actionIconUrl(Paths.UnStarredIcon)
        text: msgList.canMarkSelectionImportant ? qsTr("Star") : qsTr("Remove star")
        onTriggered: {
            MessageActions.markSelectedMessagesImportant()
        }
    }

    property Action deleteSelected: Action {
        iconSource: Paths.actionIconUrl(Paths.DeleteIcon)
        text: qsTr("Delete")
        onTriggered: MessageActions.deleteSelectedMessages()
    }

    property Action markSelectedReadUnread: Action {
        text: msgList.canMarkSelectionRead ? qsTr("Mark as un-read") : qsTr("Mark as read")
        iconSource: !msgList.canMarkSelectionRead ? Paths.actionIconUrl(Paths.MailUnreadIcon) :
                                                    Paths.actionIconUrl(Paths.MailReadIcon)
        onTriggered: MessageActions.markSelectedMessagesRead()
    }
}
