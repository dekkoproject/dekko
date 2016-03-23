import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Accounts 1.0
import Dekko.Mail 1.0
import Dekko.Components 1.0
import "../components"
import "../delegates"
import "../models"

DekkoPage {
    id: msgListPage

    pageHeader.title: "Inbox"
    pageHeader.enableSearching: true
    pageHeader.backAction: drawerAction
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // NOTE: DO NOT CHANGE THESE WITHOUT UPDATING THE FILTER SWITCH BELOW!!!!
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    readonly property var defaultFilters: [qsTr("All"), qsTr("Unread"), qsTr("Starred"), qsTr("Replied"), qsTr("Forwarded"), qsTr("Attachments"), qsTr("Calendar")]
    pageHeader.filterSections: defaultFilters
    onSelectedIndexChanged: {
        switch (selectedIndex) {
        case 0: // All
            msgList.filter = MessageList.All;
            break;
        case 1: // Unread
            msgList.filter = MessageList.Unread;
            break;
        case 2: // Starred
            msgList.filter = MessageList.Important;
            break;
        case 3: // Replied
            msgList.filter = MessageList.Replied;
            break;
        case 4: // Forwarded
            msgList.filter = MessageList.Forwarded;
            break;
        case 5: // Attachments
            msgList.filter = MessageList.Attachments;
            break;
        case 6: // Calendar
            msgList.filter = MessageList.Calendar;
            break;
        }
    }

    pageHeader.multiSelectActionList: [selectAllAction, flagSelectedAction, markSelectedAction, deleteSelectedAction]
    onMultiSelectCanceled: msgList.endSelection()


    function reset() {
        listView.currentIndex = -1
        listView.positionViewAtBeginning()
        selectedIndex = 0
    }

    Action {
        id: selectAllAction
        iconSource: !msgList.canSelectAll ? Paths.actionIconUrl(Paths.NoneSelectedIcon) :
                                            Paths.actionIconUrl(Paths.SelectIcon)
        text: msgList.canSelectAll ? qsTr("Unselect all") : qsTr("Select all")
        onTriggered: {
            if (msgList.canSelectAll) {
                msgList.selectAll();
            } else {
                msgList.unselectAll();
            }
        }
    }

    Action {
        id: flagSelectedAction
        iconSource: msgList.canMarkSelectionImportant ? Paths.actionIconUrl(Paths.StarredIcon) :
                                                        Paths.actionIconUrl(Paths.UnStarredIcon)
        text: msgList.canMarkSelectionImportant ? qsTr("Star") : qsTr("Remove star")
        onTriggered: {
            msgList.markSelectedMessagesImportant()
        }
    }

    Action {
        id: deleteSelectedAction
        iconSource: Paths.actionIconUrl(Paths.DeleteIcon)
        text: qsTr("Delete")
        onTriggered: msgList.deleteSelectedMessages()
    }

    Action {
        id: markSelectedAction
        text: msgList.canMarkSelectionRead ? qsTr("Mark as un-read") : qsTr("Mark as read")
        iconSource: !msgList.canMarkSelectionRead ? Paths.actionIconUrl(Paths.MailUnreadIcon) :
                                                    Paths.actionIconUrl(Paths.MailReadIcon)
        onTriggered: msgList.markSelectedMessagesRead()
    }

    Action {
        id: drawerAction
        iconName: "navigation-menu"
        onTriggered: navDrawer.opened ? navDrawer.close() : navDrawer.open()
    }

    MessageList {
        id: msgList
        sortOrder: Qt.DescendingOrder
        currentSelectedIndex: listView.currentIndex
    }

    ListItem {
        id: undoNotification
        expansion.height: units.gu(6)
        expansion.expanded: Client.service.hasUndoableAction
        divider.visible: Client.service.hasUndoableAction
        height: 0
        anchors {
            left: parent.left
            top: pageHeader.bottom
            right: parent.right
        }

        Label {
            anchors {
                left: parent.left
                leftMargin: units.gu(2)
                verticalCenter: parent.verticalCenter
            }
            visible: Client.service.hasUndoableAction
            text: Client.service.undoableActionDescription
        }

        Button {
            height: units.gu(4)
            width: units.gu(8)
            anchors {
                right: parent.right
                rightMargin: units.gu(2)
                verticalCenter: parent.verticalCenter
            }
            color: UbuntuColors.green
            action: Action {
                text: "Undo"
                onTriggered: Client.service.undoActions()
            }
        }
    }


    ListView {
        id: listView
        property int selectionIndex: 0
        anchors {
            left: parent.left
            top: undoNotification.bottom
            right: parent.right
            bottom: parent.bottom
        }
        clip: true
        currentIndex: -1
        add: Transition {
            NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 300 }
            NumberAnimation { property: "scale"; easing.type: Easing.InOutSine; from: 0.4; to: 1.0; duration: 300 }
        }

        addDisplaced: Transition {
            NumberAnimation { properties: "y"; duration: 400; easing.type: Easing.InBack }
        }

        remove: Transition {
            NumberAnimation { property: "opacity"; from: 1.0; to: 0; duration: 100 }
        }

        removeDisplaced: Transition {
            SequentialAnimation {
                PauseAnimation { duration: 300 }
                NumberAnimation { properties: "x,y"; duration: 400; easing.type: Easing.OutBack }
            }
        }

        highlight: Rectangle {
            color: Qt.rgba(0, 0, 0, 0.05)
        }
        highlightFollowsCurrentItem: true
        highlightMoveDuration: 200

        model: msgList.model
        delegate: MessageListDelegate{
            id: msgListDelegate
            anchors {
                left: parent.left
                right: parent.right
            }
            // This prevents making excessive calls to data()
            // We just take the actual MinimalMessage object
            // in one go and reuse it throughout.
            msg: model.qtObject

            leftSideAction: Action {
                iconName: "delete"
                onTriggered: {
                    Client.deleteMessage(msgListDelegate.msg.messageId)
                }
            }

            rightSideActions: [flagAction, readAction, moveAction, contextAction]

            onItemClicked: {
                if (msgList.isInSelectionMode) {
                    if (msg.checked) {
                        msgList.setChecked(model.index, Qt.Unchecked)
                    } else {
                        msgList.setChecked(model.index, Qt.Checked)
                    }
                    return;
                }

                if (mouse.button === Qt.RightButton) {
                    rightClickActions.trigger()
                    return;
                }
                listView.currentIndex = model.index
            }
            onItemPressAndHold: {
                if (!msgList.isInSelectionMode) {
                    listView.selectionIndex = model.index
                    msgList.startSelection()
                    msgList.setChecked(model.index, Qt.Checked);
                    msgListPage.startMultiSelect()
                }
            }
        }

        footer: msgList.model.count && msgList.canLoadMore ? footerComponent : null

        Component {
            id: footerComponent
            ListItem {
                Label {
                    anchors.centerIn: parent
                    text: qsTr("Load more messages ...")
                }
                onClicked: msgList.loadMore()
            }
        }
    }

    Accounts {
        id: accounts
        // Here we only want the accounts that can receive messages
        // i.e pop/imap
        filter: Accounts.CanReceive
    }

    MessageFilterCollection {
        id: standardFolders
        filter: MessageFilterCollection.StandardFolders
    }

    NavigationDrawer {
        id: navDrawer
        signal msgKeySelected(string title, var key)
        onMsgKeySelected: {
            msgListPage.reset()
            pageHeader.title = title
            msgList.messageKey = key
            delayClose()
        }
        animate: true
        width: units.gu(35)
        state: "fixed"
        // make sure this overlays the page contents
        z: 1
        anchors {
            left: parent.left
            top: pageHeader.bottom
            bottom: parent.bottom
        }
        panelModel: NavMenuModel{}
    }
}

