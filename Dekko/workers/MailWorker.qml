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
import QuickFlux 1.0
import Dekko.Mail 1.0
import Dekko.Mail.API 1.0
import Dekko.Mail.Stores.Mail 1.0
import Dekko.Mail.Stores.Views 1.0

AppListener {

    waitFor: [
        MailStore.listenerId,
        ViewStore.listenerId
    ]

    Filter {
        type: MessageKeys.changeMessageListSortOrder
        onDispatched: {
            if (message.order !== Qt.DescendingOrder || message.order !== Qt.AscendingOrder) {
                Log.logError("MailWorker::changeMessageListOrder", "Unknown sort order... defaulting to descending order")
                MailStore.msgList.sortOrder = Qt.DescendingOrder
            } else {
                MailStore.msgList.sortOrder = message.order
            }
        }
    }

    AppScript {
        id: openFolderScript
        property var fkey
        property string fname
        runWhen: MessageKeys.openFolder
        script: {
            Log.logStatus("MailWorker::openFolder", "Opening %1 folder with key %2".arg(message.folderName).arg(message.folderKey))
            // So this is a performance critical point. Well for the UI anyway
            // We may have views stacked ontop of the base message list view
            // so before we go resetting models we first need to rewind the stack to the view we
            // want and *then* reset the model and apply the new key
            MessageActions.rewindMessageListStack()

            // Wait for the MailStage to inform us the stack has been rewound
            once(MessageKeys.stackRewound, function() {
                MessageActions.resetMessageList()
                MailStore.currentMessageId = -1
                // nice! we still have the message value captured from script execution
                // so we don't need to store the values in tmp properties
                MailStore.folderTitle = message.folderName
                ViewActions.setCurrentNavFolder(MailStore.folderTitle)
                MailStore.msgList.messageKey = message.folderKey
            })
        }
    }

    // Delete selected messages. Use when in selection mode
    Filter {
        type: MessageKeys.deleteSelectedMessages
        onDispatched: {
            if (!MailStore.isInSelectionMode) {
                Log.logError("MailWorker::deleteSelectedMessages", "Not in selection mode! Not going to delete anything...")
                return;
            }
            MailStore.msgList.deleteSelectedMessages()
        }
    }
    Filter {
        type: MessageKeys.markSelectedMessagesRead
        onDispatched: {
            if (!MailStore.isInSelectionMode) {
                Log.logError("MailWorker::markSeectedMessagesRead", "Not in selection mode! Not applying action...")
                return
            }
            MailStore.msgList.markSelectedMessagesRead()
        }
    }

    Filter {
        type: MessageKeys.markSelectedMessagesImportant
        onDispatched: {
            if (!MailStore.isInSelectionMode) {
                Log.logError("MailWorker::markSelectedMessagesImportant", "Not in selection mode! Not applying action...")
                return
            }
            MailStore.msgList.markSelectedMessagesImportant()
        }
    }

    Filter {
        type: MessageKeys.undoRecentActions
        onDispatched: {
            if (MailStore.hasUndoableActions) {
                Log.logInfo("MailWorker::undoRecentActions", "Undoing actions from the last 5 seconds")
                Client.undoActions()
            }
        }
    }

    Filter {
        type: MessageKeys.setMessageListFilter
        onDispatched: {
            Log.logInfo("MailWorker::setMessageListFilter", "Setting message list filter with index: " + message.filter)
            MailStore.msgList.filter = message.filter
        }
    }

    Filter {
        type: MessageKeys.setMessageCheck
        onDispatched: {
            Log.logInfo("MailWorker::setMessageCheck", "Setting %1 as checked: %2".arg(message.msgIndex).arg(message.checked))
            MailStore.msgList.setChecked(message.msgIndex, message.checked)
        }
    }

    Filter {
        type: MessageKeys.startMultiSelection
        onDispatched: {
            Log.logStatus("MailWorker::startMultiSelection", "Started")
            MailStore.msgList.startSelection()
        }
    }

    Filter {
        type: MessageKeys.endMultiSelection
        onDispatched: {
            Log.logStatus("MailWorker::endMultiSelection", "Ended")
            MailStore.msgList.endSelection()
        }
    }

    Filter {
        type: MessageKeys.selectAllMessages
        onDispatched: {
            Log.logInfo("MailWorker::selectAllMessages", "Selecting all messages")
            MailStore.msgList.selectAll()
        }
    }

    Filter {
        type: MessageKeys.unselectAllMessages
        onDispatched: MailStore.msgList.unselectAll()
    }

    Filter {
        type: MessageKeys.showMoreMessages
        onDispatched: MailStore.msgList.loadMore()
    }

    // Delete message by ID
    Filter {
        type: MessageKeys.deleteMessage
        onDispatched: {
            Log.logInfo("MailWorker::deleteMessage", "Deleting message %1".arg(message.msgId))
            if (ViewStore.formFactor === "small") {
                MailboxActions.resetSelectedMsgIndex()
            }
            Client.deleteMessage(message.msgId)
        }
    }
    Filter {
        type: MessageKeys.markMessageRead
        onDispatched: {
            Log.logInfo("MailWorker::markMessageRead", "Marking message %1 as %2".arg(message.msgId).arg((message.read ? "read":"unread")))
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

    Filter {
        type: MessageKeys.sendPendingMessages
        onDispatched: {
            Log.logStatus("MailWorker::sendPendingMessages", "Enqueuing any pending messages")
            Client.sendPendingMessages()
        }
    }
}
