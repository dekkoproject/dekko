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

/*!
* @ingroup qml_actions
*/
KeyTable {
    // Message API
    property string openMessage
    property string setCurrentMessage
    property string openFolder
    property string openAccountFolder
    property string changeMessageListSortOrder
    property string resetMessageList
    property string deleteMessage
    property string deleteSelectedMessages
    property string undoRecentActions
    property string markMessageRead
    property string markSelectedMessagesRead
    property string markMessageImportant
    property string markSelectedMessagesImportant
    property string markMessageAsTodo
    property string markMessageAsDone
    property string restoreMessage
    property string selectAllMessages
    property string unselectAllMessages
    property string setMessageCheck
    property string showMoreMessages
    property string setMessageListFilter
    property string rewindMessageListStack
    property string startMultiSelection
    property string endMultiSelection
    property string resetFolderLists
    property string stackRewound
    property string sendPendingMessages
}
