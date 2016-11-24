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
import QtQml.StateMachine 1.0 as DSM
import Ubuntu.Components.Popups 1.3
import QuickFlux 1.0
import "../../../actions/logging"
import "../../../actions/popups"
import "../../../actions/wizard"
import "../../../stores/accounts"

DSM.State {
    id: noAccountsState

    property alias nextTargetState: next.targetState

    onEntered: {
        Log.logStatus("AutoConfigState::onEntered", "Loading processing overlay");
        d.overlay = PopupUtils.open(Qt.resolvedUrl("qrc:/qml/views/setupwizard/components/ProcessingOverlay.qml"),
                                    dekko,
                                    {
                                        text: qsTr("Searching for configuration.")
                                    })
        WizardActions.lookForServerDetails()
    }

    onExited: {
        Log.logStatus("AutoConfigState::onExited", "Destroying overlay");
        if (d.overlay !== undefined) {
            PopupUtils.close(d.overlay)
        }
    }

    AppScript {
        runWhen: WizardKeys.askAboutImapInstead
        script: {
            WizardActions.showUseImapDialog()
            once(PopupKeys.confirmationDialogConfirmed, function (message) {
                if (message.id !== d.useImapId) {
                    return;
                }
                Log.logInfo("AutoConfigState::confirmationDialogConfirmed", "Seems they do! Using imap config instead")
                WizardActions.useImapInstead()
            })
            once(PopupKeys.confirmationDialogCancelled, function (message) {
                if (message.id !== d.useImapId) {
                    return;
                }
                Log.logInfo("AutoConfigState::confirmationDialogConfirmed", "Sticking with pop")
                WizardActions.stickWithPop()
            })
        }
    }

    AppListener {
        filter: WizardKeys.showUseImapDialog
        onDispatched: {
            if (d.overlay !== undefined) {
                PopupUtils.close(d.overlay)
            }
            PopupActions.showConfirmationDialog(
                        d.useImapId,
                        qsTr("IMAP server found"),
                        qsTr("A IMAP server configuration was found for you domain.\n\nWould you like to use this instead?"
                ))
        }
    }

    DSM.SignalTransition {
        id: next
        signal: AccountSetup.goNext
    }

    QtObject {
        id: d
        property var overlay: undefined
        readonly property string useImapId: "use-imap-dlg"
    }
}
