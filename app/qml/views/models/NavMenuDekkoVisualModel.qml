import QtQuick 2.4
import Ubuntu.Components 1.3

VisualItemModel {
    ListItem {
        height: versionLayout.implicitHeight
        ListItemLayout {
            id: versionLayout
            title.text: qsTr("Version")

            Icon {
                height: units.gu(3)
                width: height
                name: "info"
                SlotsLayout.position: SlotsLayout.Leading
            }
            ProgressionSlot{}
        }
    }
    ListItem {
        height: licenseLayout.implicitHeight
        ListItemLayout {
            id: licenseLayout
            title.text: qsTr("Licenses")

            Icon {
                height: units.gu(3)
                width: height
                name: "text-x-generic-symbolic"
                SlotsLayout.position: SlotsLayout.Leading
            }
            ProgressionSlot{}
        }
    }
    ListItem {
        height: contributorLayout.implicitHeight
        ListItemLayout {
            id: contributorLayout
            title.text: qsTr("Contributors")

            Icon {
                height: units.gu(3)
                width: height
                name: "contact"
                SlotsLayout.position: SlotsLayout.Leading
            }
            ProgressionSlot{}
        }
    }
}

