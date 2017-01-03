/* Copyright (C) 2016 Dan Chapman <dpniel@ubuntu.com>

   This file is part of Dekko email client for Ubuntu devices

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Mail.Accounts 1.0
import Dekko.Mail.API 1.0
import Dekko.Mail.Stores.Accounts 1.0
import Dekko.Components 1.0
import QuickFlux 1.0
import "../../components"

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

    PageContent {

        ScrollView {
            anchors.fill: parent

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
}

