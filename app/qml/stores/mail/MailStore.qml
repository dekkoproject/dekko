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
pragma Singleton
import QtQuick 2.4
import QuickFlux 1.0
import Dekko.Mail 1.0
import "../../actions"
import "../../actions/messaging"
import "../../actions/logging"
import "../../actions/views"

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
    property alias isInSelectionMode: msgList.isInSelectionMode
    property alias canLoadMore: msgList.canLoadMore
    property alias msgListKey: msgList.messageKey
    property alias msgListModel: msgList.model
    property alias currentSelectedIndex: msgList.currentSelectedIndex

    property alias folderTitle: d.currentFolderName

    property MailStoreActions actions: MailStoreActions{}

    MessageList {
        id: msgList
        sortOrder: Qt.DescendingOrder
        currentSelectedIndex: /*listView.currentIndex*/ -1
    }

    Filter {
        type: MessageKeys.changeMessageListSortOrder
        onDispatched: {
            if (message.order !== Qt.DescendingOrder || message.order !== Qt.AscendingOrder) {
                Log.logError("MailStore::changeMessageListOrder", "Unknown sort order... defaulting to descending order")
                msgList.sortOrder = Qt.DescendingOrder
            } else {
                msgList.sortOrder = message.order
            }
        }
    }

    Filter {
        type: MessageKeys.openFolder
        onDispatched: {
            Log.logStatus("MailStore::openFolder", "Opening %1 folder with key %2"
                                   .arg(message.folderName).arg(message.folderKey))
            MessageActions.resetMessageList()
            d.currentFolderName = message.folderName
            msgList.messageKey = message.folderKey
            MessageActions.rewindMessageListStack()
            ViewActions.setCurrentNavFolder(d.currentFolderName)
        }
    }

    // Delete selected messages. Use when in selection mode
    Filter {
        type: MessageKeys.deleteSelectedMessages
        onDispatched: {
            if (!msgList.isInSelectionMode) {
                Log.logError("MailStore::deleteSelectedMessages", "Not in selection mode! Not going to delete anything...")
                return;
            }
            msgList.deleteSelectedMessages()
        }
    }

    Filter {
        type: MessageKeys.markSelectedMessagesRead
        onDispatched: {
            if (!msgList.isInSelectionMode) {
                Log.logError("MailStore::markSeectedMessagesRead", "Not in selection mode! Not applying action...")
                return
            }
            msgList.markSelectedMessagesRead()
        }
    }


    Filter {
        type: MessageKeys.markSelectedMessagesImportant
        onDispatched: {
            if (!msgList.isInSelectionMode) {
                Log.logError("MailStore::markSelectedMessagesImportant", "Not in selection mode! Not applying action...")
                return
            }
            msgList.markSelectedMessagesImportant()
        }
    }

    Filter {
        type: MessageKeys.undoRecentActions
        onDispatched: {
            if (hasUndoableActions) {
                Log.logInfo("MailStore::undoRecentActions", "Undoing actions from the last 5 seconds")
                Client.service.undoActions()
            }
        }
    }

    Filter {
        type: MessageKeys.setMessageListFilter
        onDispatched: {
            Log.logInfo("MailStore::setMessageListFilter", "Setting message list filter with index: " + message.filter)
            msgList.filter = message.filter
        }
    }

    Filter {
        type: MessageKeys.setMessageCheck
        onDispatched: {
            Log.logInfo("MailStore::setMessageCheck", "Setting %1 as checked: %2".arg(message.msgIndex).arg(message.checked))
            msgList.setChecked(message.msgIndex, message.checked)
        }
    }

    Filter {
        type: MessageKeys.startMultiSelection
        onDispatched: {
            Log.logStatus("MailStore::startMultiSelection", "Started")
            msgList.startSelection()
        }
    }

    Filter {
        type: MessageKeys.endMultiSelection
        onDispatched: {
            Log.logStatus("MailStore::endMultiSelection", "Ended")
            msgList.endSelection()
        }
    }

    Filter {
        type: MessageKeys.selectAllMessages
        onDispatched: {
            Log.logInfo("MailStore::selectAllMessages", "Selecting all messages")
            msgList.selectAll()
        }
    }

    Filter {
        type: MessageKeys.unselectAllMessages
        onDispatched: msgList.unselectAll()
    }

    Filter {
        type: MessageKeys.showMoreMessages
        onDispatched: msgList.loadMore()
    }

    // Delete message by ID
    Filter {
        type: MessageKeys.deleteMessage
        onDispatched: {
            Log.logInfo("MailStore::deleteMessage", "Deleting message %1".arg(message.msgId))
            Client.deleteMessage(message.msgId)
        }
    }
    Filter {
        type: MessageKeys.markMessageRead
        onDispatched: {
            Log.logInfo("MailStore::markMessageRead", "Marking message %1 as %2".arg(message.msgId).arg((message.read ? "read":"unread")))
            Client.markMessageRead(message.msgId, message.read)
        }
    }
    Filter {
        type: MessageKeys.markMessageImportant
        onDispatched: Client.markMessageImportant(message.msgId, message.important)
    }

    Filter {
        type: MessageKeys.markMessageAsTodo
        onDispatched: Client.markMessageTodo(message.msgId, message.todo)
    }

    Filter {
        type: MessageKeys.markMessageAsDone
        onDispatched: Client.markMessageDone(message.msgId, message.done)
    }

    Filter {
        type: MessageKeys.restoreMessage
        onDispatched: Client.restoreMessage(message.msgId)
    }

    QtObject {
        id: d
        property string currentFolderName: "Inbox"
    }
}
