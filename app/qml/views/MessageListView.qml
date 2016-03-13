import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Accounts 1.0
import Dekko.Mail 1.0
import "../components"

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
        animate: true
        // Take up 90% height and 80% width
        maxHeight: (parent.height - header.height) / 10 * 9
        width: parent.width / 10 * 8
        // make sure this overlays the page contents
        z: 1
        anchors {
            left: parent.left
            top: pageHeader.bottom
        }
        inboxModel: standardFolders.children
        inboxDelegate: ListItem {
            height: iLayout.implicitHeight
            ListItemLayout {
                id: iLayout
                title.text: qtObject.displayName + " (" + qtObject.unreadCount + "/" + qtObject.totalCount + ")"
                Icon {
                    height: units.gu(2.5)
                    width: height
                    name: "inbox"
                    SlotsLayout.position: SlotsLayout.Leading
                }

            }
        }
        accountsModel: accounts.model
        accountsDelegate: ListItem {
            height: dLayout.implicitHeight
            ListItemLayout {
                id: dLayout
                title.text: qtObject.name
                Icon {
                    height: units.gu(2.5)
                    width: height
                    name: "contact"
                    SlotsLayout.position: SlotsLayout.Leading
                }
            }
        }
    }
}

