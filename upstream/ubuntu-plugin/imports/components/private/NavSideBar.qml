import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Mail 1.0
import Dekko.Mail.Accounts 1.0
import Dekko.Mail.API 1.0
import Dekko.Mail.Settings 1.0
import Dekko.Mail.Stores.Accounts 1.0
import Dekko.Mail.Stores.Mail 1.0
import Dekko.Mail.Stores.Views 1.0
import Dekko.Components 1.0
import MazDB 1.0
import PlugMan 1.0
import Dekko.Ubuntu.Components 1.0
import "../delegates"

StyledItem {
    width: parent.width
    height: parent.height

    property bool panelMode: false

    theme: ThemeSettings{
        name: "Ubuntu.Components.Themes.SuruDark"
    }

    MazDBSettings {
        category: "ui-property-cache"
        property alias smartFoldersExpanded: smf.expanded
        property alias accountFoldersExpanded: acg.expanded
    }

    Rectangle {
        anchors.fill: parent
        color: UbuntuColors.inkstone
    }

    ActionRegistry {
        id: headerRegistry
        location: "Dekko::Mail::NavHeadAction"
        defaultActions: [
            Action {
                iconName: "edit"
                onTriggered: ViewActions.openComposer()
                visible: dekko.isLargeFF
            },
            Action {
                iconName: "like"
                visible: dekko.isSmallFF
            },
            Action {
                iconName: "settings"
                visible: dekko.isSmallFF
                onTriggered: ViewActions.openSettings()
            }
        ]
    }

    StretchColumn {
        anchors {
            fill: parent
        }

        Item {
            width: parent.width
            height: units.gu(5)
            implicitHeight: height
            visible: !dekko.isMediumFF

            StretchRow {
                anchors {
                    leftMargin: units.gu(1)
                    fill: parent
                    rightMargin: units.gu(1)
                }

                HeaderButton {
                    height: units.gu(5.5)
                    width: units.gu(5)
                    implicitWidth: width
                    iconColor: UbuntuColors.silk
                    iconSize: units.gu(2.5)
                    highlightColor: UbuntuColors.slate
                    action: Action {
                        visible: dekko.isSmallFF
                        iconName: "navigation-menu"
                        onTriggered: ViewActions.toggleNavDrawer()
                    }
                }

                Stretcher {}

                Repeater {
                    model: headerRegistry.actions
                    delegate: HeaderButton {
                        height: units.gu(5.5)
                        width: units.gu(5)
                        implicitWidth: width
                        iconColor: UbuntuColors.silk
                        iconSize: units.gu(2.5)
                        highlightColor: UbuntuColors.slate
                        action: modelData
                    }
                }
            }
        }

        Stretcher {
            anchors {
                left: parent.left
                right: parent.right
            }

            ScrollView {
                anchors.fill: parent
                Flickable {
                    anchors.fill: parent
                    contentHeight: col.height

                    Column {
                        id: col
                        anchors {
                            left: parent.left
                            right: parent.right
                            top: parent.top
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
                                    if (model.index === 0 && !panelMode) {
                                        ViewActions.delayCallWithArgs(MessageKeys.openFolder, {
                                                                          folderName: folder.displayName,
                                                                          folderKey: folder.descendentsKey
                                                                      })
                                    }
                                }
                            }
                        }

                        NavigationGroup {
                            id: smf
                            title: qsTr("Smart folders")
                            model: MailboxStore.smartFoldersModel
//                            expansion.expanded: PolicyManager.views.smartFoldersExpanded
//                            onExpandClicked: PolicyManager.views.smartFoldersExpanded = !PolicyManager.views.smartFoldersExpanded
                            delegate: SmartFolderDelegate {
                                id: smartFolderDelegate
                                folder: qtObject
                                smartFolder: true
                                onClicked: MessageActions.openFolder(folder.displayName, folder.messageKey)
                            }
                        }

                        NavigationGroup {
                            id: acg
                            title: qsTr("Folders")
                            model: AccountStore.receiveAccountsModel
//                            expansion.expanded: PolicyManager.views.accountsExpanded
//                            onExpandClicked: PolicyManager.views.accountsExpanded = !PolicyManager.views.accountsExpanded
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
//                        Item {
//                            anchors {
//                                left: parent.left
//                                right: parent.right
//                            }
//                            height: lbl.height + units.gu(2)
//                            implicitHeight: height
//                            Label {
//                                id: lbl
//                                anchors {
//                                    left: parent.left
//                                    leftMargin: units.gu(1)
//                                    right: parent.right
//                                    verticalCenter: parent.verticalCenter
//                                }
//                                text: "Smart Folders"
//                                color: UbuntuColors.ash
//                            }
//                        }
//                        Line {
//                            anchors {
//                                left: parent.left
//                                leftMargin: units.gu(1)
//                                right: parent.right
//                                rightMargin: units.gu(1)
//                            }
//                            color: UbuntuColors.slate
//                        }


//                        ListItem {
//                            implicitHeight: i1.implicitHeight
//                            divider.visible: false
//                            highlightColor: UbuntuColors.ash
//                            ListItemLayout {
//                                title.text: "Today"
//                                title.font.weight: Font.DemiBold
//                                title.color: UbuntuColors.silk
//                                Icon {
//                                    name: "calendar-today"
//                                    color: UbuntuColors.silk
//                                    height: units.gu(2.2)
//                                    width: height

//                                    SlotsLayout.position: SlotsLayout.Leading
//                                }
//                            }
//                            onClicked: console.log("Clicked")
//                        }
//                        ListItem {
//                            implicitHeight: i1.implicitHeight
//                            divider.visible: false
//                            highlightColor: UbuntuColors.ash
//                            ListItemLayout {
//                                title.text: "Todo"
//                                title.font.weight: Font.DemiBold
//                                title.color: UbuntuColors.silk
//                                Icon {
//                                    name: "view-list-symbolic"
//                                    color: UbuntuColors.silk
//                                    height: units.gu(2.2)
//                                    width: height

//                                    SlotsLayout.position: SlotsLayout.Leading
//                                }
//                            }
//                            onClicked: console.log("Clicked")
//                        }
//                        ListItem {
//                            implicitHeight: i1.implicitHeight
//                            divider.visible: false
//                            highlightColor: UbuntuColors.ash
//                            ListItemLayout {
//                                title.text: "Done"
//                                title.font.weight: Font.DemiBold
//                                title.color: UbuntuColors.silk
//                                Icon {
//                                    name: "tick"
//                                    color: UbuntuColors.silk
//                                    height: units.gu(2.2)
//                                    width: height

//                                    SlotsLayout.position: SlotsLayout.Leading
//                                }
//                            }
//                            onClicked: console.log("Clicked")
//                        }

//                        ListItem {
//                            implicitHeight: i1.implicitHeight
//                            divider.visible: false
//                            highlightColor: UbuntuColors.ash
//                            ListItemLayout {
//                                title.text: "Mailing Lists"
//                                title.font.weight: Font.DemiBold
//                                title.color: UbuntuColors.silk
//                                Icon {
//                                    name: "contact-group"
//                                    color: UbuntuColors.silk
//                                    height: units.gu(2.2)
//                                    width: height

//                                    SlotsLayout.position: SlotsLayout.Leading
//                                }
//                            }
//                            onClicked: console.log("Clicked")
//                        }
//                        ListItem {
//                            implicitHeight: i1.implicitHeight
//                            divider.visible: false
//                            highlightColor: UbuntuColors.ash
//                            ListItemLayout {
//                                title.text: "Search: \"dekko-dev\""
//                                title.font.weight: Font.DemiBold
//                                title.color: UbuntuColors.silk
//                                Icon {
//                                    name: "search"
//                                    color: UbuntuColors.silk
//                                    height: units.gu(2.2)
//                                    width: height

//                                    SlotsLayout.position: SlotsLayout.Leading
//                                }
//                            }
//                            onClicked: console.log("Clicked")
//                        }

//                        Item {
//                            anchors {
//                                left: parent.left
//                                right: parent.right
//                            }
//                            height: lbl2.height + units.gu(2)
//                            implicitHeight: height
//                            Label {
//                                id: lbl2
//                                anchors {
//                                    left: parent.left
//                                    leftMargin: units.gu(1)
//                                    right: parent.right
//                                    verticalCenter: parent.verticalCenter
//                                }
//                                text: "Folders"
//                                color: UbuntuColors.ash
//                            }
//                        }

//                        Line {
//                            anchors {
//                                left: parent.left
//                                leftMargin: units.gu(1)
//                                right: parent.right
//                                rightMargin: units.gu(1)
//                            }
//                            color: UbuntuColors.slate
//                        }

//                        Item {
//                            height: tf.height + units.gu(2)
//                            implicitHeight: height
//                            width: parent.width
//                            TextField {
//                                id: tf
//                                anchors {
//                                    left: parent.left
//                                    leftMargin: units.gu(2)
//                                    right: parent.right
//                                    rightMargin: units.gu(2)
//                                    verticalCenter: parent.verticalCenter
//                                }
//                                placeholderText: "Search folders"
//                            }
//                        }

//                        ListItem {
//                            implicitHeight: i1.implicitHeight
//                            divider.visible: false
//                            highlightColor: UbuntuColors.ash
//                            ListItemLayout {
//                                title.text: "GMail"
//                                title.font.weight: Font.DemiBold
//                                title.color: UbuntuColors.silk
//                                Icon {
//                                    name: "contact"
//                                    color: UbuntuColors.silk
//                                    height: units.gu(2.2)
//                                    width: height

//                                    SlotsLayout.position: SlotsLayout.Leading
//                                }
//                            }
//                            onClicked: console.log("Clicked")
//                        }
//                        ListItem {
//                            implicitHeight: i1.implicitHeight
//                            divider.visible: false
//                            highlightColor: UbuntuColors.ash
//                            ListItemLayout {
//                                title.text: "Dekko"
//                                title.font.weight: Font.DemiBold
//                                title.color: UbuntuColors.silk
//                                Icon {
//                                    name: "contact"
//                                    color: UbuntuColors.silk
//                                    height: units.gu(2.2)
//                                    width: height

//                                    SlotsLayout.position: SlotsLayout.Leading
//                                }
//                            }
//                            onClicked: console.log("Clicked")
//                        }
//                        ListItem {
//                            implicitHeight: i1.implicitHeight
//                            divider.visible: false
//                            highlightColor: UbuntuColors.ash
//                            ListItemLayout {
//                                title.text: "Home"
//                                title.font.weight: Font.DemiBold
//                                title.color: UbuntuColors.silk
//                                Icon {
//                                    name: "contact"
//                                    color: UbuntuColors.silk
//                                    height: units.gu(2.2)
//                                    width: height

//                                    SlotsLayout.position: SlotsLayout.Leading
//                                }
//                            }
//                            onClicked: console.log("Clicked")
//                        }
                    }
                }
            }
        }
    }
}
