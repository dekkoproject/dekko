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
import Dekko.Mail.API 1.0
import Dekko.Mail.Stores.Accounts 1.0
import QuickFlux 1.0
import Dekko.Ubuntu.Components 1.0
import Dekko.Ubuntu.Constants 1.0

DekkoPage {
    objectName: "userInputUI"
    pageHeader.title: AccountSetup.accountDescripion

    PageFlickable{

        TitledTextField {
            id: name
            objectName: "userInputNameField"
            title: qsTr("Name")
            placeholderText: qsTr("Full name")
            inputMethodHints: Qt.ImhNoPredictiveText
        }

        TitledTextField {
            id: nickname
            objectName: "descriptionInput"
            title: qsTr("Description")
            placeholderText: qsTr("E.g Home, Work...")
            inputMethodHints: Qt.ImhNoPredictiveText
        }

        TitledTextField {
            id: email
            objectName: "userInputEmailField"
            title: qsTr("Email address")
            inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhEmailCharactersOnly | Qt.ImhNoPredictiveText
            placeholderText: qsTr("email@example.org")
        }

        TitledTextField {
            id: password
            objectName: "userInputPasswordField"
            title: qsTr("Password")
            inputMethodHints: Qt.ImhHiddenText | Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
            echoMode: showPassword.checked ? TextInput.Normal : TextInput.Password
            placeholderText: qsTr("Password")
        }

        CheckboxWithLabel {
            id: showPassword
            objectName: "showPwdCheckBox"
            text: qsTr("Show password")
            textColor: UbuntuColors.inkstone
        }

        WizardStepper {
            anchors {
                left: parent.left
                right: parent.right
            }
            previousAction: Action {
                text: qsTr("Cancel")
                onTriggered: WizardActions.wizardStepBack()
            }
            nextAction: Action {
                text: qsTr("Next")
                onTriggered: WizardActions.validateUserDetails()
            }
        }
    }

    AppScript {
        property var userDetails
        runWhen: WizardKeys.validateUserDetails
        script: {
            userDetails = {
                "name": name.text,
                "email": email.text,
                "password": password.text,
                "nickname": nickname.text
            };

            WizardActions.validateUser(userDetails)
            // It passed \o/. Now commit the details to the account object
            // and signal to go to the next stop
            once(WizardKeys.userDetailsValid, function() {

                Log.logInfo("UserInputUI", "User details valid")
                WizardActions.setUserDetails(userDetails)
                name.requiredField = false
                email.requiredField = false
                WizardActions.wizardStepForward()

            })
            // Somethings not right. Show the invalid fields.
            once(WizardKeys.userDetailsInvalid, function(message) {

                Log.logInfo("UserInputUI::userDetailsInvalid", "User details invalid")
                name.requiredField = message.result.indexOf("name") > -1
                email.requiredField = message.result.indexOf("email") > -1
                nickname.requiredField = message.result.indexOf("nickname") > -1

            })

            once(WizardKeys.noPasswordSet, function() {

                Log.logWarning("UserInputUI::noPasswordSet", "No password set, :-/ asking user if this is what they want")
                PopupActions.showConfirmationDialog("userinputpwd", qsTr("Password empty"), qsTr("Would you like to continue?"))

            }).then(PopupKeys.confirmationDialogConfirmed,function(message) {
                if (message.id !== "userinputpwd") {
                    return;
                }
                Log.logInfo("UserInputUI::confirmationDialogConfirmed", "Seems they do! Setting validation to allow empty password")
                WizardActions.setNoPasswordAllowed()
                WizardActions.validateUserDetails()
            })
            // Disconnect all callbacks if the user steps back or forward
            once(WizardKeys.wizardStepBack, exit.bind(this, 0))
            once(WizardKeys.wizardStepForward, exit.bind(this, 0))
        }
    }
}

