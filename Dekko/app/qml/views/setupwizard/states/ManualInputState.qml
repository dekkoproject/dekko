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
    id: manualInputState

    property alias nextTargetState: next.targetState
    property alias backTartgetState: back.targetState

    onEntered: {
        Log.logStatus("ManualInputState::onEntered", "Manual input state entered");
        if (stack.currentItem.objectName !== "manualInputUI") {
            Log.logStatus("NoAccountState::onEntered", "Pushing UserInputUI");
            WizardActions.wizardNavigateTo("qrc:/qml/views/setupwizard/components/ManualInputUI.qml", {})
        }
    }

    onExited: {
        Log.logStatus("NoAccountState::onExited", "Exited no accounts state");
    }

    DSM.SignalTransition {
        id: back
        signal: AccountSetup.goBack
    }

    DSM.SignalTransition {
        id: next
        signal: AccountSetup.goNext
    }

}
