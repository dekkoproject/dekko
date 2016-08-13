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
import Dekko.Settings 1.0
import "./actions/views"
import "./actions/logging"
import "./views/components"
import "./views/utils"
import "./views/stages"
import "./stores"
import "./stores/composer"

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
            anchors.fill: parent
            Component.onCompleted: {
                // this just ensures the ComposerStore instance has been created
                // before dispatching any actions. It's usually created when the composer is opened
                // for the first time but we may dispatch actions before it's open. i.e from the msglist context menus.
                var ignore = ComposerStore.sendInProgress
                ignore = PolicyManager.objectName // init the global policies so they are ready. This seems enough to get the singleton fired up
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

    Timer {
        interval: 120000
        repeat: true
        triggeredOnStart: true
        onTriggered: {
            Log.logStatus("Dekko::TrimCache", "Trimming component cache")
            dekkoapp.trimCache()
        }
        Component.onCompleted: start()
    }
}

