import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Accounts 1.0
import Dekko.Components 1.0
import QuickFlux 1.0
import "../../components"
import "../../actions"
import "../../stores"

DekkoPage {
    id: newAccountUI

    objectName: "newAccountUI"

    pageHeader.title: qsTr("New account")
    pageHeader.backAction: Action {
        iconName: "back"
        onTriggered: {
            DekkoActions.wizardStepBack()
        }
    }

    PresetProviderModel {
        id: providers
    }

    property bool setShowCompleted: TestStore.showCompletedTasks
    onSetShowCompletedChanged: console.log("We got the dispatched event")

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
                        DekkoActions.logInfo("NewAccountsUI::providerClicked", "PRESET clicked");
                        DekkoActions.setNewAccountType(provider.description, AccountConfig.IMAP)
                        DekkoActions.wizardSetAccountPreset(provider)
                        break;
                    case Provider.IMAP:
                        DekkoActions.logInfo("NewAccountsUI::providerClicked", "IMAP clicked");
                        DekkoActions.setNewAccountType(provider.description, AccountConfig.IMAP)
                        break;
                    case Provider.POP3:
                        DekkoActions.logInfo("NewAccountsUI::providerClicked", "POP3 clicked");
                        DekkoActions.setNewAccountType(provider.description, AccountConfig.POP3)
                        break;
                    case Provider.SMTP:
                        DekkoActions.logInfo("NewAccountsUI::providerClicked", "SMTP clicked");
                        DekkoActions.setNewAccountType(provider.description, AccountConfig.SMTP)
                        break;
                    }
                    DekkoActions.wizardStepForward()
                }
            }
        }
    }
}

