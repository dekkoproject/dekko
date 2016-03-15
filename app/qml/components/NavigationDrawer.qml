/* Copyright (C) 2014-2016 Dan Chapman <dpniel@ubuntu.com>

   This file is part of Dekko email client for Ubuntu Devices/

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
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.0 as ListItem
import "../components" as Comps

Panel {
    id: accountsDrawer
//    property alias accountsModel: accountsList.model
//    property alias inboxModel: inboxList.model
//    property alias accountsDelegate: accountsList.delegate
//    property alias inboxDelegate: inboxList.delegate
    property alias panelModel: topListView.model
    property alias topListViewHandle: topListView
    property Action openSettingsAction
    property Action openAddressbookAction

    property int maxHeight

    visible: opened || animating
    align: Qt.AlignLeading
    state: "fixed"
    states: [
        State {
            name: "overlay"
            PropertyChanges {
                target: accountsDrawer
                height: col.height < maxHeight ? col.height : maxHeight
            }
        },
        // These two will come in to the equation when we hit tablet/desktop modes
        // As this panel will most likely be always on the left side of
        // the screen either collapsed or maxWidth. We don't actually use them yet
        State {
            name: "fixed-narrow"
            PropertyChanges {
                target: accountsDrawer
                width: units.gu(10)
                height: parent.height
            }
        },
        State {
            name: "fixed"
            PropertyChanges {
                target: accountsDrawer
                width: units.gu(30)
                height: parent.height
            }
        }
    ]
    Rectangle {
        anchors.fill: parent
        color: "#ffffff"
    }
    Rectangle {
        id: btmshadow
        anchors {
            left: parent.left
            right: parent.right
            top: parent.bottom
        }
        height: units.gu(0.8)
        gradient: Gradient {
            GradientStop { position: 0.0; color: Qt.rgba(0, 0, 0, 0.1) }
            GradientStop { position: 1.0; color: "transparent" }
        }
    }
    InverseMouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        onPressed: accountsDrawer.close()
    }
    // FIXME: where this meets the btmShadow there is a clear difference
    // in the way it looks where they join. How to wrap a gradient round a corner??
    // NOTE it's only really visible to the eye if you are looking for it
    HorizontalGradiant {
        id: sideShadow
        anchors {
            left: parent.right
            bottom: btmshadow.bottom
            bottomMargin: units.gu(0.3)
            top: parent.top
        }
        width: units.gu(0.8)
        gradient: Gradient {
            GradientStop { position: 0.0; color: Qt.rgba(0, 0, 0, 0.1) }
            GradientStop { position: 1.0; color: "transparent" }
        }
    }

    ListView {
        id: topListView
        clip: true
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
            bottom: tabBar.top
        }
        interactive: false
        orientation: ListView.Horizontal
        snapMode: ListView.SnapOneItem
        currentIndex: tabBar.currentIndex
        highlightMoveDuration: 100
    }

    Comps.TabBar {
        id: tabBar
        width: parent.width
        anchors.bottom: parent.bottom
        iconNameModel: ["email", "contact-group", "settings", "like"]
    }
}


//        Flickable {
//            id: flicker
//            anchors.fill: parent
//            clip: true
//            contentHeight: col.height + units.gu(5)
//            interactive: col.height > parent.height
//            Column {
//                id: col
//                anchors {left: parent.left; right: parent.right; top: parent.top}
//                // TODO: get combined inbox working here
//                //            ListItem.Standard { text: "Combined Inbox" }
//                Repeater {
//                    id: inboxList
//                }
//                Rectangle {
//                    anchors {
//                        left: parent.left
//                        right: parent.right
//                    }
//                    height: sectionLabel.height
//                    color: Qt.darker("#ffffff")
//                    Label {
//                        id: sectionLabel
//                        anchors {
//                            left: parent.left
//                            leftMargin: units.gu(2)
//                            right: parent.right
//                            rightMargin: units.gu(2)
//                        }
//                        text: qsTr("Accounts")
//                        color: Style.common.text
//                        height: units.gu(4)
//                        fontSize: "medium"
//                        font.weight: Font.DemiBold
//                        verticalAlignment: Text.AlignVCenter

//                    }
//                }
//                Repeater {
//                    id: accountsList
//                }
////                AccountsDrawerDelegate {
////                    text: qsTr("Add account")
////                    iconName: "add"
////                    onClicked: {
////                        accountsDrawer.close()
////                        rootPageStack.push(Qt.resolvedUrl("../SetupWizard/SetupWizard.qml"))
////                        dekko.setupWizardRunning = true;
////                    }
////                }
////                AccountsDrawerDelegate {
////                    text: qsTr("Email settings")
////                    iconName: "settings"
////                    onClicked: {
////                        accountsDrawer.close()
////                        openSettingsAction.trigger()
////                    }
////                }
////                AccountsDrawerDelegate {
////                    text: qsTr("Addressbook")
////                    iconName: "contact-group"
////                    onClicked: {
////                        accountsDrawer.close()
////                        openAddressbookAction.trigger()
////                    }
////                }
////                AccountsDrawerDelegate {
////                    visible: GlobalSettings.developer.developerModeEnabled
////                    text: "Dev settings"
////                    iconName: "security-alert"
////                    onClicked: {
////                        accountsDrawer.close()
////                        internalStack.push(Qt.resolvedUrl("../DeveloperMode/DeveloperSettings.qml"))
////                    }
////                }
//            }
//        }
