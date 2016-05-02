import QtQuick 2.4
import Ubuntu.Components 1.3
import QuickFlux 1.0
import "../../components"
import "../../../actions"
import "../../../stores"

DekkoPage {
    objectName: "userInputUI"

    pageHeader.title: AccountSetup.accountDescripion

    ScrollView {
        anchors {
            left: parent.left
            top: pageHeader.bottom
            right: parent.right
            bottom: parent.bottom

        }
        Flickable {
            anchors.fill: parent
            contentHeight: col.height + units.gu(5)
            Column {
                id: col
                anchors {
                    left: parent.left
                    leftMargin: units.gu(2)
                    top: parent.top
                    topMargin: units.gu(1)
                    right: parent.right
                    rightMargin: units.gu(2)
                }
                spacing: units.gu(1)

                TitledTextField {
                    id: name
                    objectName: "userInputNameField"
                    title: qsTr("Name")
                }

                TitledTextField {
                    id: email
                    objectName: "userInputEmailField"
                    title: qsTr("Email address")
                }

                TitledTextField {
                    id: password
                    objectName: "userInputPasswordField"
                    title: qsTr("Password")
                }

                WizardStepper {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    previousAction: Action {
                        text: qsTr("Cancel")
                        onTriggered: DekkoActions.wizardStepBack()
                    }
                    nextAction: Action {
                        text: qsTr("Next")
                        onTriggered: DekkoActions.validateUserDetails()
                    }
                }
            }
        }
    }

    AppScript {
        property var userDetails
        runWhen: ActionTypes.validateUserDetails
        script: {
            userDetails = {
                "name": name.text,
                "email": email.text,
                "password": password.text
            };

            DekkoActions.validateUser(userDetails)
            // It passed \o/. Now commit the details to the account object
            // and signal to go to the next stop
            once(ActionTypes.userDetailsValid, function() {

                DekkoActions.logInfo("UserInputUI", "User details valid")
                DekkoActions.setUserDetails(userDetails)
                name.requiredField = false
                email.requiredField = false
                DekkoActions.wizardStepForward()

            })
            // Somethings not right. Show the invalid fields.
            once(ActionTypes.userDetailsInvalid, function(message) {

                DekkoActions.logInfo("UserInputUI::userDetailsInvalid", "User details invalid")
                name.requiredField = message.result.indexOf("name") > -1
                email.requiredField = message.result.indexOf("email") > -1

            })

            once(ActionTypes.noPasswordSet, function() {

                DekkoActions.logWarning("UserInputUI::noPasswordSet", "No password set, :-/ asking user if this is what they want")
                DekkoActions.showConfirmationDialog("userinputpwd", qsTr("Password empty"), qsTr("Would you like to continue?"))

            }).then(ActionTypes.confirmationDialogConfirmed,function(message) {
                if (message.id !== "userinputpwd") {
                    return;
                }
                DekkoActions.logInfo("UserInputUI::confirmationDialogConfirmed", "Seems they do! Setting validation to allow empty password")
                DekkoActions.setNoPasswordAllowed()
                DekkoActions.validateUserDetails()
            })
            // Disconnect all callbacks if the user steps back or forward
            once(ActionTypes.wizardStepBack, exit.bind(this, 0))
            once(ActionTypes.wizardStepForward, exit.bind(this, 0))
        }
    }
}

