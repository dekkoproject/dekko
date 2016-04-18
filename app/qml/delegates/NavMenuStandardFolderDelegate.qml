import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Mail 1.0
import Dekko.Components 1.0
import "../components"

ListItem {
    id: li

    property var folder
    signal subFolderClicked(string name, var key)
    property bool supportsDescendents: false
    property bool smartFolder: false

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

    ListItemLayout {
        id: layout
        title.text: folder.displayName
        height: units.gu(6)

        CachedImage {
            id: attachmentImg
            height: units.gu(3)
            width: height
            name: getIconForFolderType(folder)
            color: UbuntuColors.ash
            SlotsLayout.position: SlotsLayout.Leading
        }

        UbuntuShape {
            id: shape
            // TODO: We should probably display total count for To-Do smart folder
            visible: folder.unreadCount > 0
            aspect: UbuntuShape.Flat
            color: UbuntuColors.porcelain
            height: units.gu(2.2)
            width: countLable.width < height ? height : countLable.width + units.gu(1)
            Label {
                id: countLable
                anchors.margins: units.gu(0.5)
                anchors.centerIn: parent
                fontSize: "small"
                text: folder.unreadCount
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
                color: UbuntuColors.lightGrey
            }
            Icon {
                height: units.gu(2)
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
        visible: folder.hasDescendents
        anchors.left: parent.left
        anchors.leftMargin: units.gu(2)
        anchors.top: layout.bottom
        anchors.right: parent.right
        Repeater {
            model: folder.descendents
            delegate: ListItem {
                id: delegate
                divider.visible: false
                height: dlayout.height
                onClicked: li.subFolderClicked(model.qtObject.displayName, model.qtObject.messageKey)
                ListItemLayout {
                    id: dlayout
                    height: units.gu(5)
                    title.text: qtObject.displayName

                    CachedImage {
                        height: units.gu(2.6)
                        width: height
                        name: Icons.InboxIcon
                        color: UbuntuColors.ash
                        SlotsLayout.position: SlotsLayout.Leading
                    }

                    UbuntuShape {
                        id: dshape
                        visible: qtObject.unreadCount > 0
                        aspect: UbuntuShape.Flat
                        color: UbuntuColors.porcelain
                        height: units.gu(2.2)
                        width: dcountLable.width < height ? height : dcountLable.width + units.gu(1)
                        Label {
                            id: dcountLable
                            anchors.margins: units.gu(0.5)
                            anchors.centerIn: parent
                            text: qtObject.unreadCount
                            fontSize: "small"
                        }
                    }
                }
            }
        }
    }
}

