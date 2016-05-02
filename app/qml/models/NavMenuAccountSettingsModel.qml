import QtQuick 2.4
import QtQuick.Window 2.2
import Ubuntu.Components 1.3
import "../setupwizard"

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
        onClicked: rootPageStack.push("qrc:/qml/setupwizard/SetupWizard.qml")
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
        onClicked: {
            var w = window.createObject(dekko)
            w.show()
        }
        Component {
            id: window
            Window {
                height: units.gu(70)
                width: units.gu(50)
                title: "New account"
                SetupWizard {
                    anchors.fill: parent
                }
            }
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

