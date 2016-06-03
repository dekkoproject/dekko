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
import "../../../actions/logging"
import "../../../actions/wizard"
import "../../../stores/accounts"

DSM.State {
    id: noAccountsState


    property alias backTargetState: next.targetState
    property alias createTargetState: previous.targetState

    onEntered: {
        Log.logStatus("AutoConfigState::onEntered", "Loading processing overlay");
        if (stack.currentItem === null) {
            Log.logStatus("NoAccountState::onEntered", "Pushing NoAccountsUI");
            // TODO: either push a processing overlay or a new page with an activity indicator
        }
    }

    onExited: {
        console.log("Exited no accounts state")
    }

    DSM.SignalTransition {
        id: next
        signal: AccountSetup.createAccount
    }
    DSM.SignalTransition {
        id: previous
        signal: AccountSetup.goBack
    }
}
