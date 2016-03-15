import QtQuick 2.4
import Ubuntu.Components 1.3

VisualItemModel {
    ListItem {
        height: abookLayout.implicitHeight
        ListItemLayout {
            id: abookLayout
            title.text: qsTr("Addressbook")

            Icon {
                height: units.gu(3)
                width: height
                name: "address-book-app-symbolic"
                SlotsLayout.position: SlotsLayout.Leading
            }
            ProgressionSlot{}
        }
    }
    ListItem {
        height: recentLayout.implicitHeight
        ListItemLayout {
            id: recentLayout
            title.text: qsTr("Recent contacts")

            Icon {
                height: units.gu(3)
                width: height
                name: "contact"
                SlotsLayout.position: SlotsLayout.Leading
            }
            ProgressionSlot{}
        }
    }
    ListItem {
        height: importLayout.implicitHeight
        ListItemLayout {
            id: importLayout
            title.text: qsTr("Import contacts")

            Icon {
                height: units.gu(3)
                width: height
                name: "import"
                SlotsLayout.position: SlotsLayout.Leading
            }
            ProgressionSlot{}
        }
    }
}

