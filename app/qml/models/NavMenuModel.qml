import QtQuick 2.4
import Ubuntu.Components 1.3
import "../delegates"

VisualItemModel {

    id: navModel

    property bool panelIsParent: true
    signal openFolder(string accountName, var accountId)


    Item {
        width: navDrawer.width
        height: panelIsParent ? navDrawer.topListViewHandle.height : navDrawer.height

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
                            model: standardFolders.children
                            delegate: NavMenuStandardFolderDelegate {
                                id: folderDelegate
                                folder: qtObject
                                supportsDescendents: true
                                onClicked: {
                                    if (model.index === 0) {
                                        navDrawer.msgKeySelected(folder.displayName, folder.descendentsKey)
                                    } else {
                                        navDrawer.msgKeySelected(folder.displayName, folder.messageKey)
                                    }
                                }
                                onSubFolderClicked: navDrawer.msgKeySelected(name, key)
                                Component.onCompleted: {
                                    if (model.index === 0) {
                                        navDrawer.msgKeySelected(folder.displayName, folder.descendentsKey)
                                    }
                                }
                            }
                        }
                    }

                    ListItem {
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                        height: sLabel.height
                        color: "#ffffff"
                        Label {
                            id: sLabel
                            anchors {
                                left: parent.left
                                leftMargin: units.gu(2)
                                right: parent.right
                                rightMargin: units.gu(2)
                            }
                            text: qsTr("Smart folders")
                            height: units.gu(4)
                            fontSize: "medium"
                            font.weight: Font.DemiBold
                            verticalAlignment: Text.AlignVCenter

                        }
                        Icon {
                            height: units.gu(2)
                            width: height
                            anchors {
                                right: parent.right
                                verticalCenter: parent.verticalCenter
                                rightMargin: units.gu(2.5)
                            }
                            name: "add"
                        }
                    }
                    Column {
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                        Repeater {
                            id: smartFolderList
                            model: smartFolders.children
                            delegate: NavMenuStandardFolderDelegate {
                                id: smartFolderDelegate
                                folder: qtObject
                                smartFolder: true
                                onClicked: navDrawer.msgKeySelected(folder.displayName, folder.messageKey)
                            }
                        }
                    }

                    ListItem {
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                        height: sectionLabel.height
                        color: "#ffffff"
                        Label {
                            id: sectionLabel
                            anchors {
                                left: parent.left
                                leftMargin: units.gu(2)
                                right: parent.right
                                rightMargin: units.gu(2)
                            }
                            text: qsTr("Accounts")
                            height: units.gu(4)
                            fontSize: "medium"
                            font.weight: Font.DemiBold
                            verticalAlignment: Text.AlignVCenter

                        }
                    }
                    Repeater {
                        id: accountsList
                        model: accounts.model
                        delegate: ListItem {
                            height: dLayout.height
                            divider.visible: false
                            onClicked: openFolder(qtObject.name, qtObject.id)
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

    Item {
        width: navDrawer.width
        height: panelIsParent ? navDrawer.topListViewHandle.height : navDrawer.height

        ScrollView {
            anchors.fill: parent
            ListView {
                anchors.fill: parent
                model: NavMenuContactsModel{}
            }
        }
    }
    Item {
        width: navDrawer.width
        height: panelIsParent ? navDrawer.topListViewHandle.height : navDrawer.height

        ScrollView {
            anchors.fill: parent
            ListView {
                anchors.fill: parent
                model: NavMenuAccountSettingsModel{}
            }
        }
    }
    Item {
        width: navDrawer.width
        height: panelIsParent ? navDrawer.topListViewHandle.height : navDrawer.height

        ScrollView {
            anchors.fill: parent
            ListView {
                anchors.fill: parent
                model: NavMenuDekkoVisualModel{}
            }
        }
    }
}
