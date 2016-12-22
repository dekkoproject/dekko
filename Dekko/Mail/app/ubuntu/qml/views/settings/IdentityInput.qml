import QtQuick 2.4
import Ubuntu.Components 1.3
// FIXME: These are most likely going to be deprecated soon
import Ubuntu.Components.ListItems 1.0 as CanWeStillUseThis
import Dekko.Mail.API 1.0
import Dekko.Mail.Stores.Accounts 1.0
import QuickFlux 1.0
import "../components"

PageFlickable {
    id: input
    property bool newIdentity: false

    AppListener {
        Filter {
            type: SettingsKeys.saveCurrentGroup
            onDispatched: {
                // TODO: we just need to save to one of the Stores and not commit to the db
                //       at this point. as we may hit some issues with duplicate identities
                //       due to a new identity not having a valid id yet.
                //       The New(Edit)IdentityPage has a save action that will dispatch SettingsKeys.saveIdentity
                //       This is just about saving state on view/layout transitions.
            }
        }

        Filter {
            type: SettingsKeys.saveIdentity
            onDispatched: {
                // TODO: commit identity to db
            }
        }
    }

    Label {
        text: qsTr("Account")
        color: UbuntuColors.inkstone
    }

    CanWeStillUseThis.ItemSelector {
        model: AccountStore.enabledAccountsModel
        delegate: OptionSelectorDelegate {
            text: model.qtObject.name
        }
    }

    TitledTextField {
        id: name
        title: qsTr("Name")
        text:
    }

    TitledTextField {
        id: email
        title: qsTr("Email Address")
        text: account.outgoing.email
    }

    TitledTextField {
        id: replyTo
        title: qsTr("Reply-To")
        text: ""
    }

    Label {
        text: qsTr("Signature")
        color: UbuntuColors.inkstone
    }

    TextArea {
        anchors {
            left: parent.left
            right: parent.right
        }
    }

    states: [
        State {
            name: "new"
            when: input.newIdentity
            PropertyChanges {
                target: root
                title: qsTr("New identity")
            }
        },
        State {
            name: "edit"
            when: !input.newIdentity
            PropertyChanges {
                target: root
                title: qsTr("Edit identity")
            }
        }
    ]
}
