pragma Singleton
import QtQuick 2.4
import QuickFlux 1.0

KeyTable {
    // Message API
    property string openMessage
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
}
