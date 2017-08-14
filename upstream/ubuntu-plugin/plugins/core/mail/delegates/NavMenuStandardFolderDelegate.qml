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
import Ubuntu.Components.Popups 1.3
import Dekko.Mail 1.0
import Dekko.Components 1.0
import Dekko.Mail.Stores.Views 1.0
import Dekko.Ubuntu.Components 1.0
import Dekko.Ubuntu.Constants 1.0

ListItem {
    id: li

    property var folder
    signal subFolderClicked(string name, var key)
    property bool supportsDescendents: false
    property bool smartFolder: false
    readonly property bool isCurrentIndex: layout ? layout.title.text === ViewStore.selectedNavFolder : false

    expansion.expanded: false
    expansion.height: layout.height + col.height
    height: layout.height
    divider.visible: false

    function getIconForFolderType(f) {
        if (smartFolder) {
            switch (f.type) {
            case SmartFolderSet.SmartTodayFolder:
                return Icons.CalendarTodayIcon;
            case SmartFolderSet.SmartTodoFolder:
                return Icons.ViewListSymbolic;
            case SmartFolderSet.SmartDoneFolder:
                return Icons.SelectIcon;
            case SmartFolderSet.SmartCustomFolder:
                return Icons.FilterIcon;
            default:
                return Icons.FilterIcon;
            }
        } else {
            switch (f.folderType) {
            case StandardFolderSet.StandardFolder:
                return Icons.FolderIcon
            case StandardFolderSet.SpecialUseInboxFolder:
                if (f.hasDescendents) {
                    return Icons.CombineInboxIcon
                } else {
                    return Icons.InboxIcon
                }
            case StandardFolderSet.SpecialUseOutboxFolder:
                return Icons.InboxIcon
            case StandardFolderSet.SpecialUseDraftsFolder:
                return Icons.DraftIcon
            case StandardFolderSet.SpecialUseSentFolder:
                return Icons.SendIcon;
            case StandardFolderSet.SpecialUseTrashFolder:
                return Icons.DeleteIcon
            case StandardFolderSet.SpecialUseJunkFolder:
                return Icons.JunkIcon
            }
        }
    }

    Rectangle {
        anchors.fill: layout ? layout : parent
        color: Qt.rgba(0, 0, 0, 0.05)
        visible: li.isCurrentIndex
    }
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        onClicked: {
            if (mouse.button === Qt.RightButton) {
                PopupUtils.open(Qt.resolvedUrl("../popovers/NavViewContextMenu.qml"), li, {folder: li.folder})
                mouse.accepted = true
            } else {
                mouse.accepted = false
            }
        }
    }

    ListItemLayout {
        id: layout
        title.text: folder ? folder.displayName : ""
        title.font.weight: Font.DemiBold
        title.color: UbuntuColors.silk
        height: units.gu(6)

        CachedImage {
            id: attachmentImg
            height: units.gu(2.6)
            width: height
            name: folder ? getIconForFolderType(folder) : ""
            color: UbuntuColors.silk
            SlotsLayout.position: SlotsLayout.Leading
        }

        UbuntuShape {
            id: shape
            // TODO: We should probably display total count for To-Do smart folder
            visible: folder && folder.unreadCount > 0
            aspect: UbuntuShape.Flat
            color: UbuntuColors.slate
            height: units.gu(2.2)
            width: countLable.width < height ? height : countLable.width + units.gu(1)
            Label {
                id: countLable
                anchors.margins: units.gu(0.5)
                anchors.centerIn: parent
                fontSize: "small"
                color: UbuntuColors.porcelain
                text: folder ? folder.unreadCount : ""
            }
        }

        Item {
            visible: model.index === 0 && supportsDescendents
            height: units.gu(3)
            width: units.gu(4)
            Rectangle {
                anchors.left: parent.left
                height: parent.height
                width: units.dp(1)
                color: UbuntuColors.slate
            }
            Icon {
                height: Style.defaultSpacing
                width: height
                anchors.right: parent.right
                anchors.rightMargin: units.gu(0.5)
                anchors.verticalCenter: parent.verticalCenter
                name: li.expansion.expanded ? "up" : "down"
            }
            SlotsLayout.position: SlotsLayout.Last
            AbstractButton {
                anchors.fill: parent
                onClicked: li.expansion.expanded = !li.expansion.expanded
            }
        }
    }
    Column {
        id: col
        visible: folder && folder.hasDescendents
        anchors.left: parent.left
        anchors.leftMargin: units.gu(1.5)
        anchors.top: layout.bottom
        anchors.right: parent.right
        Repeater {
            model: folder ? folder.descendents : 0
            delegate: ListItem {
                id: delegate
                divider.visible: false
                height: dlayout.height
                onClicked: li.subFolderClicked(dlayout.title.text, model.qtObject.messageKey)
                Rectangle {
                    anchors.fill: parent
                    color: Qt.rgba(0, 0, 0, 0.05)
                    visible: layout ? dlayout.title.text === ViewStore.selectedNavFolder : false
                }
                ListItemLayout {
                    id: dlayout
                    height: units.gu(5)
                    title.text: qsTr("Inbox (%1)").arg(qtObject.displayName)
                    title.color: UbuntuColors.silk

                    CachedImage {
                        height: units.gu(2.4)
                        width: height
                        name: Icons.InboxIcon
                        color: UbuntuColors.silk
                        SlotsLayout.position: SlotsLayout.Leading
                    }

                    UbuntuShape {
                        id: dshape
                        visible: qtObject.unreadCount > 0
                        aspect: UbuntuShape.Flat
                        color: UbuntuColors.slate
                        height: units.gu(2.2)
                        width: dcountLable.width < height ? height : dcountLable.width + units.gu(1)
                        Label {
                            id: dcountLable
                            anchors.margins: units.gu(0.5)
                            anchors.centerIn: parent
                            text: qtObject.unreadCount
                            fontSize: "small"
                            color: UbuntuColors.porcelain
                        }
                    }
                }
            }
        }
    }
}

