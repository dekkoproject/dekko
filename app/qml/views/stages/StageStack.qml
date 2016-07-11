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
import Ubuntu.Components.Popups 1.3
import QuickFlux 1.0
import "../../actions/views"
import "../../actions/logging"
import "../../stores"

PageStack {
    id: stageStack

    AppListener {
        waitFor: ViewStore.listenerId
        Filter {
            type: ViewKeys.pushStage
            onDispatched: {
                Log.logStatus("StageStack::pushStage", "Pushing stage: " + message.stage)
                stageStack.push(message.stage, message.properties)
            }
        }
        Filter {
            type: ViewKeys.popStage
            onDispatched: {
                Log.logStatus("StageStack::popStage", "Popping top stage: ")
                stageStack.pop()
            }
        }
        Filter {
            type: ViewKeys.clearStageStack
            onDispatched: {
                Log.logStatus("StageStack::clearStageStack", "Clearing stagestack ")
                stageStack.clear()
            }
        }
        Filter {
            type: ViewKeys.openMessageComposer
            onDispatched: {
                if (dekko.viewState.isLargeFF) {
                    PopupUtils.open("qrc:/qml/views/composer/ComposePanelPopup.qml", dekkoContainer, {})
                }
            }
        }
    }
}

