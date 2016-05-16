import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Mail 1.0
import Dekko.Accounts 1.0
import Dekko.Settings 1.0
import "./components" as Comps
import "./models"
import "../actions"
import "../actions/messaging"

Comps.DekkoPage {
    id: menuPage

    pageHeader.title: qsTr("Mail")
    pageHeader.composeButtonEnabled: tabBar.currentIndex === 0 && dekko.viewState.isLargeFF
    extendHeader: true

    NavigationSettings {
        id: navSettings
    }

    Comps.PageContent {

        ListView {
            id: navDrawer
            clip: true
            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
                bottom: tabBar.top
            }
            model: NavMenuModel{
                panelIsParent: false
            }
            interactive: false
            orientation: ListView.Horizontal
            snapMode: ListView.SnapOneItem
            currentIndex: tabBar.currentIndex
            highlightMoveDuration: 275 // seems about right :-D
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
    }
}

