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
import "./actions/views"
import "./actions/logging"
import "./views/components"
import "./views/utils"
import "./views/stages"
import "./stores"

Item {
    id: dekko
    property alias viewState: view
    anchors.fill: parent
    ViewState {
        id: view
        anchors.fill: parent
        onStateChanged: {
            ViewStore.formFactor = state
            Log.logStatus("ViewState::stateChanged", state)
        }
    }

    Item {
        id: dekkoContainer
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
            // anchor to the top of KeyboardRectangle
            // this ensures pages are always above the OSK
            // Basically the same as anchorToKeyboard
            bottom: kbdRect.top
        }
        StageStack {
            id: rootPageStack
            Component.onCompleted: {
                ViewActions.stageStackReady()
            }
        }
    }

    KeyboardRectangle {
        id: kbdRect
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }
}

