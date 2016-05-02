import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Mail 1.0
import Dekko.Accounts 1.0
import Dekko.Settings 1.0
import "./components" as Comps
import "./models"

Comps.DekkoPage {
    id: menuPage

    pageHeader.title: qsTr("Mail")
    pageHeader.composeButtonEnabled: tabBar.currentIndex === 0 && dekko.viewState.isLargeFF
//    pageHeader.showDivider: false
    extendHeader: true

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
    MessageFilterCollection {
        id: smartFolders
        filter: MessageFilterCollection.SmartFolders
    }
    NavigationSettings {
        id: navSettings
    }

    ListView {
        id: navDrawer
        signal msgKeySelected(string title, var key)
        onMsgKeySelected: {
            mailView.openFolder(title, key)
        }
        clip: true
        anchors {
            left: parent.left
            top: pageHeader.bottom
            right: parent.right
            bottom: tabBar.top
        }
        model: NavMenuModel{
            panelIsParent: false
            onOpenFolder: {
                mailView.openAccountFolder(accountName, accountId)
            }
        }
        interactive: false
        orientation: ListView.Horizontal
        snapMode: ListView.SnapOneItem
        currentIndex: tabBar.currentIndex
        highlightMoveDuration: 275
    }

    Comps.TabBar {
        id: tabBar
        width: parent.width
        anchors.bottom: parent.bottom
        iconNameModel: ["email", "contact-group", "settings", "like"]
        currentIndex: 0
        onCurrentIndexChanged: {
            switch(currentIndex) {
            case 0:
                menuPage.pageHeader.title = qsTr("Mail")
                break
            case 1:
                menuPage.pageHeader.title = qsTr("Contacts")
                break
            case 2:
                menuPage.pageHeader.title = qsTr("Settings")
                break
            case 3:
                menuPage.pageHeader.title = qsTr("About")
                break
            }
        }
    }

//    ListView {
//        id: navDrawer
//        anchors {
//            left: parent.left
//            top: pageHeader.bottom
//            right: parent.right
//            bottom: parent.bottom
//        }

//    }
}

