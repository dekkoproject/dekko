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
                    if (immediatePush) {
                        internalStack.push({item: message.page, immediate: true, replace: true, properties: message.properties})
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

