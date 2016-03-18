import QtQuick 2.4
import Ubuntu.Components 1.3
import "../delegates"

VisualItemModel {

    id: navModel


    Item {
        width: navDrawer.width
        height: navDrawer.topListViewHandle.height

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
                    Repeater {
                        id: inboxList
                        model: standardFolders.children
                        delegate: NavMenuStandardFolderDelegate {
                            id: folderDelegate
                            folder: qtObject
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
            }
        }
    }

    Item {
        width: navDrawer.width
        height: navDrawer.topListViewHandle.height

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
        height: navDrawer.topListViewHandle.height

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
        height: navDrawer.topListViewHandle.height

        ScrollView {
            anchors.fill: parent
            ListView {
                anchors.fill: parent
                model: NavMenuDekkoVisualModel{}
            }
        }
    }
}
