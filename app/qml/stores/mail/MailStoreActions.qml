import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Components 1.0
import "../../actions/messaging"

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
