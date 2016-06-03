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
import Dekko.Controls 1.0
import QuickFlux 1.0
import "../../actions/views"

// A stage area is a root container used for an area of the MainStage
// A StageArea has it's own internal page stack and should be used
// by all child pages of this StageArea
Item {
    clip: true // Don't leak into another area
    // Set a page url to load on creation
    property var baseUrl: undefined
    // Set a base component to load on creation
    property Component baseComponent: null
    property alias stackCount: internalStack.depth
    property alias currentItem: internalStack.currentItem
    property bool immediatePush: false
    property alias delegate: internalStack.delegate

    property string stageID: ""

    anchors.fill: parent

    StackView {
        id: internalStack
        anchors.fill: parent // make sure our stack takes the full size
        initialItem: baseComponent !== null ? baseComponent : baseUrl
    }

    function pop() {
        internalStack.pop()
    }

    AppListener {
        Filter {
            type: ViewKeys.pushToStageArea
            onDispatched: {
                if (message.stageID === stageID) {
                    if (immediatePush) {
                        internalStack.push({item: message.page, immediate: true, properties: message.properties})
                    } else {
                        internalStack.push({item: message.page, properties: message.properties})
                    }
                }
            }
        }
        Filter {
            type: ViewKeys.replaceTopStageAreaItem
            onDispatched: {
                if (message.stageID === stageID) {
//                    internalStack.pop()
                    if (immediatePush) {
                        internalStack.push({item: message.page, replace: true, immediate: true, properties: message.properties})
                    } else {
                        internalStack.push({item: message.page, replace: true, properties: message.properties})
                    }
                }
            }
        }

        Filter {
            type: ViewKeys.popStageArea
            onDispatched: {
                if (message.stageID === stageID) {
                    internalStack.pop()
                }
            }
        }
    }
}

