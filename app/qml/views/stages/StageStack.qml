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

