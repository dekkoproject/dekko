import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Accounts 1.0
import Dekko.Components 1.0
import QuickFlux 1.0
import "../../components"
import "../../../stores/accounts"
import "../../../actions/logging"
import "../../../actions/wizard"

DekkoPage {
    id: newAccountUI

    objectName: "newAccountUI"

    pageHeader.title: qsTr("New account")
    pageHeader.backAction: Action {
        iconName: "back"
        onTriggered: {
            WizardActions.wizardStepBack()
        }
    }

    PresetProviderModel {
        id: providers
    }

    ScrollView {
        anchors {
            left: parent.left
            top: pageHeader.bottom
            right: parent.right
            bottom: parent.bottom
        }
        ListView {
            anchors.fill: parent
            model: providers.model
            delegate: ListItem {
                property var provider: model.qtObject
                objectName: "provider_" + provider.description
                height: layout.height
                divider.visible: false
                ListItemLayout {
                    id: layout
                    title.text: provider.description
                    Icon {
                        height: units.gu(3)
                        width: height
                        source: provider.icon
                        SlotsLayout.position: SlotsLayout.Leading
                    }
                    ProgressionSlot{}
                }
                onClicked: {
                    switch(provider.type) {
                    case Provider.PRESET:
                        Log.logInfo("NewAccountsUI::providerClicked", "PRESET clicked");
                        WizardActions.setNewAccountType(provider.description, AccountConfig.IMAP)
                        WizardActions.wizardSetAccountPreset(provider)
                        break;
                    case Provider.IMAP:
                        Log.logInfo("NewAccountsUI::providerClicked", "IMAP clicked");
                        WizardActions.setNewAccountType(provider.description, AccountConfig.IMAP)
                        break;
                    case Provider.POP3:
                        Log.logInfo("NewAccountsUI::providerClicked", "POP3 clicked");
                        WizardActions.setNewAccountType(provider.description, AccountConfig.POP3)
                        break;
                    case Provider.SMTP:
                        Log.logInfo("NewAccountsUI::providerClicked", "SMTP clicked");
                        WizardActions.setNewAccountType(provider.description, AccountConfig.SMTP)
                        break;
                    }
                    WizardActions.wizardStepForward()
                }
            }
        }
    }
}

