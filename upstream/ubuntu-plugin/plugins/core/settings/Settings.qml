import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Mail.API 1.0
import PlugMan 1.0
import Dekko.Ubuntu.Components 1.0

DekkoPage {
    id: sp

    pageHeader {
        title: qsTr("Settings")
        backAction: Action {
            iconName: "back"
            onTriggered: ViewActions.closeSettings()
        }
    }

    ActionRegistry {
        id: registry
        location: "Dekko::Settings::PluginAction"
        defaultActions: [
//            Action {
//                iconName: "mail-unread"
//                text: qsTr("Mail")
//                onTriggered: ViewActions.pushToStageArea(
//                                 ViewKeys.settingsStack1,
//                                 Qt.resolvedUrl("./MailSettings.qml"),
//                                 {}
//                             )
//            },
//            Action {
//                iconName: "address-book-app-symbolic"
//                text: qsTr("Contacts")
////                onTriggered: ViewActions.pushStage("./AddressBookStage.qml", {})
//            },
//            Action {
//                iconName: "calendar"
//                text: qsTr("Calendar")
//                onTriggered: console.log("Action Clicked")
//            },
//            Action {
//                iconName: "stock_note"
//                text: qsTr("Notes")
//                onTriggered: console.log("Action Clicked")
//            }
//            Action {
//                iconName: "account"
//                text: qsTr("Accounts")
//                onTriggered: ViewActions.pushToStageArea(
//                                 ViewKeys.settingsStack1,
//                                 Qt.resolvedUrl("./ManageAccountsPage.qml"),
//                                 {}
//                             )
//            }
        ]
    }

    PageFlickable {
        margins: 0
        spacing: 0
        Repeater {
            id: settingsRepeater
            model: registry.actions
            delegate: ListItem {
                id: li
                property Action action: modelData
                height: layout.height + divider.height
                ListItemLayout {
                    id: layout
                    title.text: li.action.text
                    Icon {
                        height: units.gu(3)
                        width: height
                        name: li.action.iconName ? li.action.iconName : ""
                        source: li.action.iconSource ? li.action.iconSource : ""
                        SlotsLayout.position: SlotsLayout.Leading
                    }

                    ProgressionSlot {}
                }
                onClicked: li.action.trigger()
            }
        }
    }
}
