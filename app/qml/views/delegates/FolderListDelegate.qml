import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Mail 1.0
import Dekko.Components 1.0
import "../components"

ListItemWithActions {

    property var folder


    function getIconForFolderType(f) {
        switch (f.type) {
        case Folder.StandardFolder:
            return Icons.InboxIcon
        case Folder.SpecialUseInboxFolder:
            return Icons.CombineInboxIcon
        case Folder.SpecialUseOutboxFolder:
            return Icons.InboxIcon
        case Folder.SpecialUseDraftsFolder:
            return Icons.DraftIcon
        case Folder.SpecialUseSentFolder:
            return Icons.SendIcon;
        case Folder.SpecialUseTrashFolder:
            return Icons.DeleteIcon
        case Folder.SpecialUseJunkFolder:
            return Icons.JunkIcon
        }
    }




    rightSideActions: [
        Action {
            id: favouriteFolder
            text: folder && folder.isFavourite ? qsTr("Un-favourite") : qsTr("Favourite")
            iconSource: folder && folder.isFavourite ? Paths.actionIconUrl(Paths.UnStarredIcon) :
                                                       Paths.actionIconUrl(Paths.StarredIcon)
            onTriggered: folder.isFavourite = !folder.isFavourite
        }

    ]
    height: layout.height
    anchors {
        left: parent.left
        right: parent.right
    }
    showDivider: false
    showActionHighlight: false

    ListItemLayout {
        id: layout
        height: units.gu(5)
        title.text: folder.name
        CachedImage {
            id: attachmentImg
            height: units.gu(3)
            width: height
            name: getIconForFolderType(folder)
            color: UbuntuColors.ash
            SlotsLayout.position: SlotsLayout.Leading
            CachedImage {
                id: favoriteIcon
                height: units.gu(1.75)
                width: height
                visible: folder.isFavourite
                anchors {
                    bottom: parent.top
                    bottomMargin: -units.gu(1.25)
                    leftMargin: -units.gu(1.25)
                    left: parent.right
                }
                color: "#f0e442"
                name: Icons.StarredIcon
            }
        }
        UbuntuShape {
            id: shape
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
        padding.leading: 1 * units.gu(folder.nestingDepth) + units.gu(2)
    }
}

