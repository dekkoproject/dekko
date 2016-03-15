import QtQuick 2.4
import Ubuntu.Components 1.3

VisualItemModel {
    ListItem {
        height: accountSettingsLayout.implicitHeight
        ListItemLayout {
            id: accountSettingsLayout
            title.text: qsTr("Manage accounts")

            Icon {
                height: units.gu(3)
                width: height
                name: "account"
                SlotsLayout.position: SlotsLayout.Leading
            }
            ProgressionSlot{}
        }
    }
    ListItem {
        height: displaySettingsLayout.implicitHeight
        ListItemLayout {
            id: displaySettingsLayout
            title.text: qsTr("Display settings")

            Icon {
                height: units.gu(3)
                width: height
                name: "video-display-symbolic"
                SlotsLayout.position: SlotsLayout.Leading
            }
            ProgressionSlot{}
        }
    }
    ListItem {
        height: privacySettingsLayout.implicitHeight
        ListItemLayout {
            id: privacySettingsLayout
            title.text: qsTr("Privacy settings")

            Icon {
                height: units.gu(3)
                width: height
                name: "private-browsing"
                SlotsLayout.position: SlotsLayout.Leading
            }
            ProgressionSlot{}
        }
    }
    ListItem {
        height: offlineSettingsLayout.implicitHeight
        ListItemLayout {
            id: offlineSettingsLayout
            title.text: qsTr("Offline settings")

            Icon {
                height: units.gu(3)
                width: height
                name: "network-wifi-symbolic"
                SlotsLayout.position: SlotsLayout.Leading
            }
            ProgressionSlot{}
        }
    }
}

