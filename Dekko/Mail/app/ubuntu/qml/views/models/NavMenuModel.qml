/* Copyright (C) 2016 Dan Chapman <dpniel@ubuntu.com>

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
import Ubuntu.Components 1.3
import Dekko.Mail 1.0
import Dekko.Mail.API 1.0
import Dekko.Mail.Settings 1.0
import Dekko.Mail.Stores.Views 1.0
import Dekko.Mail.Stores.Mail 1.0
import Dekko.Mail.Stores.Accounts 1.0
import "../delegates"
import "../components"

VisualItemModel {

    id: navModel

    property bool panelIsParent: true
    readonly property bool showGroups: MailboxStore.standardFoldersModel.count &&
                                       MailboxStore.smartFoldersModel.count &&
                                       AccountStore.receiveAccountsModel.count

    Item {
        width: navDrawer.width
        height: panelIsParent ? navDrawer.topListViewHandle.height : navDrawer.height
        visible: showGroups
        Loader {
            anchors.fill: parent
            asynchronous: true
            sourceComponent: Component {

                ScrollView {
                    anchors.fill: parent
                    Flickable {
                        id: flicker
                        anchors.fill: parent
                        clip: true
                        contentHeight: col.height + units.gu(5)
                        interactive: col.height > parent.height
                        Column {
                            id: col
                            anchors {left: parent.left; right: parent.right; top: parent.top}
                            Column {
                                anchors {
                                    left: parent.left
                                    right: parent.right
                                }
                                Repeater {
                                    id: inboxList
                                    model: MailboxStore.standardFoldersModel
                                    delegate: NavMenuStandardFolderDelegate {
                                        id: folderDelegate
                                        folder: qtObject
                                        supportsDescendents: true
                                        onClicked: {
                                            if (model.index === 0) {
                                                MessageActions.openFolder(folder.displayName, folder.descendentsKey)
                                            } else {
                                                MessageActions.openFolder(folder.displayName, folder.messageKey)
                                            }
                                        }
                                        onSubFolderClicked: MessageActions.openFolder(name, key)
                                        Component.onCompleted: {
                                            if (model.index === 0 && !panelIsParent) {
                                                ViewActions.delayCallWithArgs(MessageKeys.openFolder, {
                                                                                  folderName: folder.displayName,
                                                                                  folderKey: folder.descendentsKey
                                                                              })
                                            }
                                        }
                                    }
                                }
                            }
                            Item {
                                anchors {
                                    left: parent.left
                                    right: parent.right
                                }
                                height: smf.height
                                visible: PolicyManager.views.smartFoldersVisible
                                NavigationGroup {
                                    id: smf
                                    title: qsTr("Smart folders")
                                    model: MailboxStore.smartFoldersModel
                                    expansion.expanded: PolicyManager.views.smartFoldersExpanded
                                    onExpandClicked: PolicyManager.views.smartFoldersExpanded = !PolicyManager.views.smartFoldersExpanded
                                    delegate: NavMenuStandardFolderDelegate {
                                        id: smartFolderDelegate
                                        folder: qtObject
                                        smartFolder: true
                                        onClicked: MessageActions.openFolder(folder.displayName, folder.messageKey)
                                    }
                                }
                            }

                            Item {
                                anchors {
                                    left: parent.left
                                    right: parent.right
                                }
                                height: acg.height
                                visible: PolicyManager.views.accountsVisible
                                NavigationGroup {
                                    id: acg
                                    title: qsTr("Folders")
                                    model: AccountStore.receiveAccountsModel
                                    expansion.expanded: PolicyManager.views.accountsExpanded
                                    onExpandClicked: PolicyManager.views.accountsExpanded = !PolicyManager.views.accountsExpanded
                                    delegate: ListItem {
                                        height: dLayout.height
                                        divider.visible: false
                                        onClicked: MessageActions.openAccountFolder(qtObject.name, qtObject.id)
                                        Rectangle {
                                            anchors.fill: parent
                                            color: Qt.rgba(0, 0, 0, 0.05)
                                            visible: dLayout ? dLayout.title.text === ViewStore.selectedNavFolder : false
                                        }
                                        ListItemLayout {
                                            id: dLayout
                                            height: units.gu(6)
                                            title.text: qtObject.name
                                            Icon {
                                                height: units.gu(2.5)
                                                width: height
                                                name: "contact"
                                                SlotsLayout.position: SlotsLayout.Leading
                                            }
                                            ProgressionSlot{}
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Item {
        width: navDrawer.width
        height: panelIsParent ? navDrawer.topListViewHandle.height : navDrawer.height
        Loader {
            anchors.fill: parent
            asynchronous: true
            sourceComponent: Component {
                ScrollView {
                    anchors.fill: parent
                    ListView {
                        anchors.fill: parent
                        model: NavMenuContactsModel{}
                    }
                }
            }
        }
    }
    Item {
        width: navDrawer.width
        height: panelIsParent ? navDrawer.topListViewHandle.height : navDrawer.height
        Loader {
            anchors.fill: parent
            asynchronous: true
            sourceComponent: Component {
                ScrollView {
                    anchors.fill: parent
                    ListView {
                        anchors.fill: parent
                        model: NavMenuAccountSettingsModel{}
                    }
                }
            }
        }
    }
    Item {
        width: navDrawer.width
        height: panelIsParent ? navDrawer.topListViewHandle.height : navDrawer.height
        Loader {
            anchors.fill: parent
            asynchronous: true
            sourceComponent: Component {
                ScrollView {
                    anchors.fill: parent
                    ListView {
                        anchors.fill: parent
                        model: NavMenuDekkoVisualModel{}
                    }
                }
            }
        }
    }
}
