import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Accounts 1.0
import Dekko.Mail 1.0
import "../components"
import "../delegates"
import "../models"

DekkoPage {
    id: msgListPage

    pageHeader.title: "Inbox"
    pageHeader.enableSearching: true
    pageHeader.backAction: drawerAction
    pageHeader.filterSections: ["All", "Unread", "Starred", "Tagged", "Deleted"]

    Action {
        id: drawerAction
        iconName: "navigation-menu"
        onTriggered: navDrawer.opened ? navDrawer.close() : navDrawer.open()
    }

    MessageList {
        id: msgList
        sortOrder: Qt.DescendingOrder
    }


    ListView {
        id: listView
        anchors {
            left: parent.left
            top: pageHeader.bottom
            right: parent.right
            bottom: parent.bottom
        }
        clip: true
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
        }

        footer: msgList.canLoadMore ? footerComponent : null

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
            listView.positionViewAtBeginning()
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

