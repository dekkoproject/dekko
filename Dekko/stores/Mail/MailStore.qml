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
pragma Singleton
import QtQuick 2.4
import QuickFlux 1.0
import Dekko.Mail 1.0
import Dekko.Mail.API 1.0

/*!
*
* MailStore API
*
* MailStore maintains the state and models for all the messages of each account.
*
* @ingroup qml_stores
*/
BaseMessagingStore {
    id: mailStore

    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // NOTE: DO NOT CHANGE THESE WITHOUT UPDATING THE FILTER SWITCH IN MESSAGELISTVIEW !!!!
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    readonly property var defaultFilters: [qsTr("All"), qsTr("Unread"), qsTr("Starred"), qsTr("Replied"), qsTr("Forwarded"), qsTr("Attachments"), qsTr("Calendar")]
    readonly property var searchFilters: [qsTr("Local")/*, qsTr("Remote")*/] // TODO: Figure out why Remote is flaking. Stick with local search for now;

    property string formFactor
    property alias msgList: msgList
    property alias isInSelectionMode: msgList.isInSelectionMode
    property alias canLoadMore: msgList.canLoadMore
    property alias msgListKey: msgList.messageKey
    property alias msgListModel: msgList.model
    property alias currentSelectedIndex: msgList.currentSelectedIndex
    property int currentMessageId: -1

    property alias folderTitle: d.currentFolderName

    property MailStoreActions actions: MailStoreActions{}

    MessageList {
        id: msgList
        sortOrder: Qt.DescendingOrder
    }

    QtObject {
        id: d
        property string currentFolderName: "Inbox"
    }
}
