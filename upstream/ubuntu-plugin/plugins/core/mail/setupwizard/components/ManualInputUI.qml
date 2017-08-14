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
import QuickFlux 1.0
import Dekko.Ubuntu.Components 1.0
import Dekko.Mail.API 1.0
import Dekko.Mail.Stores.Accounts 1.0

DekkoPage {
    objectName: "manualInputUI"

    pageHeader.title: qsTr("Server configuration")

    PageFlickable {
        id: flicker
        SectionHeader {
            text: qsTr("IMAP Server:")
            textMargin: 0
            visible: AccountSetup.isImap
        }

        ServerDetails {
            id: imap
            serviceType: "imap"
            visible: AccountSetup.isImap
            accountConfig: visible ? AccountSetup.account.incoming : null
        }

        SectionHeader {
            text: qsTr("POP3 Server:")
            textMargin: 0
            visible: AccountSetup.isPop3
        }

        ServerDetails {
            id: pop
            serviceType: "pop3"
            visible: AccountSetup.isPop3
            accountConfig: visible ? AccountSetup.account.incoming : null
        }

        SectionHeader {
            text: qsTr("SMTP Server:")
            textMargin: 0
        }

        ServerDetails {
            id: smtp
            serviceType: "smtp"
            accountConfig: visible ? AccountSetup.account.outgoing : null
        }

        WizardStepper {
            previousAction: Action {
                text: qsTr("Back")
                onTriggered: WizardActions.wizardStepBack()
            }
            nextAction: Action {
                text: qsTr("Next")
                onTriggered: WizardActions.validateServerConfiguration()
            }
        }
    }

    AppListener {
        Filter {
            type: WizardKeys.validateServerConfiguration
            onDispatched: {
                if (AccountSetup.isImap) {
                    WizardActions.validateServer(imap.toMap())
                } else if (AccountSetup.isPop3) {
                    WizardActions.validateServer(pop.toMap())
                } else {
                    WizardActions.validateServer(smtp.toMap())
                }
            }
        }

        Filter {
            type: WizardKeys.serverConfigValid
            onDispatched: {
                if (message.serviceType !== "smtp") {
                    WizardActions.validateServer(smtp.toMap())
                } else {
                    console.log("CONFIG VALID")
                    if (AccountSetup.isImap) {
                        WizardActions.applyServerConfig(imap.toMap())
                    } else if (AccountSetup.isPop3) {
                        WizardActions.applyServerConfig(pop.toMap())
                    }
                    WizardActions.applyServerConfig(smtp.toMap())
                    WizardActions.wizardStepForward()
                }
            }
        }

        Filter {
            type: WizardKeys.invalidServerConfig
            onDispatched: {
                flicker.positionAtBeginning()
                var result = message.result
                if (message.serviceType === "imap") {
                    imap.showInvalidFields(result)
                } else if (message.serviceType === "pop3") {
                    pop.showInvalidFields(result)
                } else if (message.serviceType === "smtp") {
                    smtp.showInvalidFields(result)
                } else {
                    Log.logError("ManualInputUI::InvalidServerConfig", "Unknow service type " + result.serviceType)
                }
            }
        }
    }

    AppScript {
        runWhen: WizardKeys.noServerPasswordSet
        script: {
            Log.logWarning("ManualInputUI::noPasswordSet", "No password set, :-/ asking user if this is what they want")
            PopupActions.showConfirmationDialog("serverinputpwd", qsTr("Password empty"), qsTr("Would you like to continue?"))

            once(PopupKeys.confirmationDialogConfirmed, function(message) {
                if (message.id !== "serverinputpwd") {
                    return;
                }
                Log.logInfo("ManualInputUI::confirmationDialogConfirmed", "Seems they do! Setting validation to allow empty password")
                WizardActions.setNoPasswordAllowed()
                WizardActions.validateServerConfiguration()
            })

            // Disconnect all callbacks if the user steps back or forward
            once(WizardKeys.wizardStepBack, exit.bind(this, 0))
            once(WizardKeys.wizardStepForward, exit.bind(this, 0))
        }
    }
}
