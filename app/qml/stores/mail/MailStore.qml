pragma Singleton
import QtQuick 2.4
import QuickFlux 1.0
import Dekko.Mail 1.0
import "../../actions"
import "../../actions/messaging"
import "../../actions/logging"

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
                Client.service.undoActions()
            }
        }
    }

    Filter {
        type: MessageKeys.setMessageListFilter
        onDispatched: {
            msgList.filter = message.filter
        }
    }

    Filter {
        type: MessageKeys.setMessageCheck
        onDispatched: {
            msgList.setChecked(message.msgIndex, message.checked)
        }
    }

    Filter {
        type: MessageKeys.startMultiSelection
        onDispatched: {
            Log.logInfo("MailStore::startMultiSelection", "Started")
            msgList.startSelection()
        }
    }

    Filter {
        type: MessageKeys.endMultiSelection
        onDispatched: {
            Log.logInfo("MailStore::endMultiSelection", "Ended")
            msgList.endSelection()
        }
    }

    Filter {
        type: MessageKeys.selectAllMessages
        onDispatched: msgList.selectAll()
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
        onDispatched: Client.deleteMessage(message.msgId)
    }
    Filter {
        type: MessageKeys.markMessageRead
        onDispatched: Client.markMessageRead(message.msgId, message.read)
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
