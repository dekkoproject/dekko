import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Components 1.0
import "../components"

ListItem {
    height: ctxtLayout.height
    divider.visible: false
    color: UbuntuColors.porcelain
    property string description
    property alias actionIcon: leadingImage.name
    property alias actionIconColor: leadingImage.color
    property alias selectedIcon: trailingImage.name
    property alias selectedIconColor: trailingImage.color
    property alias selected: trailingImage.visible
    ListItemLayout {
        id: ctxtLayout
        height: units.gu(5)
        title.text: description
        CachedImage {
            id: leadingImage
            height: units.gu(3); width: height
            SlotsLayout.position: SlotsLayout.Leading
        }
        CachedImage {
            id: trailingImage
            height: units.gu(3); width: height
        }
    }
}

