import QtQuick 2.4
import Ubuntu.Components 1.3
// FIXME: These are most likely going to be deprecated soon
import Ubuntu.Components.ListItems 1.0 as CanWeStillUseThis
import Dekko.Mail.API 1.0
import Dekko.Mail.Stores.Accounts 1.0
import Dekko.Mail.Stores.Settings 1.0
import QuickFlux 1.0
import "../components"

PageFlickable {
    id: input
    property bool newIdentity: false

    AppListener {
        Filter {
            type: SettingsKeys.saveCurrentGroup
            onDispatched: {
                if (SettingsStore.saveGroupDisabled) {
                    return
                }
                SettingsStore.tempIsNew = input.newIdentity
                SettingsStore.tempIdentity = input.toMap()
            }
        }

        Filter {
            type: SettingsKeys.saveIdentity
            onDispatched: {
                if (newIdentity) {
                    AccountActions.addIdentity(input.toMap())
                } else {
                    AccountActions.updateIdentity(input.toMap())
                }
            }
        }
    }

    function toMap() {
        var map = {}
        if (!newIdentity) {
            map["id"] = SettingsStore.tempIdentity.id
        }
        map["parentId"] = accountSelector.selectedAccount
        map["name"] = name.text
        map["email"] = email.text
        map["replyTo"] = replyTo.text
        map["signature"] = signature.text
        return map
    }

    Label {
        text: qsTr("Account")
        color: UbuntuColors.inkstone
    }

    CanWeStillUseThis.ItemSelector {
        id: accountSelector
        property int setIdx: 0
        property int selectedAccount
        property Timer setIndexTimer: Timer {
            interval: 100
            repeat: false
            onTriggered: accountSelector.selectedIndex = accountSelector.setIdx
        }
        model: AccountStore.enabledAccountsModel
        delegate: OptionSelectorDelegate {
            id: delegate
            property bool isSelected: accountSelector.selectedIndex === model.index
            property var account: model.qtObject
            text: account.name
            Binding {
                target: accountSelector
                property: "selectedAccount"
                value: delegate.account.id
                when: delegate.isSelected
            }
            Component.onCompleted: {
                if (!input.newIdentity || SettingsStore.tempIsNew) {
                    if (SettingsStore.tempIdentity.parentId === account.id) {
                        accountSelector.setIdx = model.index
                        accountSelector.setIndexTimer.start()
                    }
                }
            }
        }
    }

    TitledTextField {
        id: name
        title: qsTr("Name")
    }

    TitledTextField {
        id: email
        title: qsTr("Email Address")
    }

    TitledTextField {
        id: replyTo
        title: qsTr("Reply-To")
    }

    Label {
        text: qsTr("Signature")
        color: UbuntuColors.inkstone
    }

    TextArea {
        id: signature
        anchors {
            left: parent.left
            right: parent.right
        }
    }

    states: [
        State {
            name: "new"
            when: input.newIdentity && !SettingsStore.tempIsNew
            PropertyChanges {
                target: root
                title: qsTr("New identity")
            }
        },
        State {
            name: "new-continued"
            when: input.newIdentity && SettingsStore.tempIsNew
            PropertyChanges {
                target: root
                title: qsTr("New identity")
            }
            PropertyChanges {
                target: name
                text: SettingsStore.tempIdentity.name
            }
            PropertyChanges {
                target: email
                text: SettingsStore.tempIdentity.email
            }
            PropertyChanges {
                target: replyTo
                text: SettingsStore.tempIdentity.replyTo
            }
            PropertyChanges {
                target: signature
                text: SettingsStore.tempIdentity.signature
            }
        },
        State {
            name: "edit"
            when: !input.newIdentity
            PropertyChanges {
                target: root
                title: qsTr("Edit identity")
            }
            PropertyChanges {
                target: name
                text: SettingsStore.tempIdentity.name
            }
            PropertyChanges {
                target: email
                text: SettingsStore.tempIdentity.email
            }
            PropertyChanges {
                target: replyTo
                text: SettingsStore.tempIdentity.replyTo
            }
            PropertyChanges {
                target: signature
                text: SettingsStore.tempIdentity.signature
            }
        }
    ]
}
