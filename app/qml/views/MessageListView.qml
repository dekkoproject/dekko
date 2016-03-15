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
        anchors {
            left: parent.left
            top: pageHeader.bottom
            right: parent.right
            bottom: parent.bottom
        }
        clip: true

        model: msgList.model
        delegate: ListItem {
            height: dlayout.implicitHeight
            ListItemLayout {
                id: dlayout
                title.text: model.from.name
                subtitle.text: model.subject
                summary.text: model.preview
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
        signal msgKeySelected(var key)
        onMsgKeySelected: msgList.messageKey = key
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

