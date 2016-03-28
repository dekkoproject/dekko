import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Controls 1.0

// A stage area is a root container used for an area of the MainStage
// A StageArea has it's own internal page stack and should be used
// by all child pages of this StageArea and can be accessed by it's "internalStack" id
Item {
    clip: true // Don't leak into another area
    // Set a page url to load on creation
    property var baseUrl: undefined
    // Set a base component to load on creation
    property Component baseComponent: null
    property alias stackCount: internalStack.depth
    property alias currentItem: internalStack.currentItem
    // Function to push a page onto the stack
    function push(component, props) {
        internalStack.push({item: component, properties: props})
    }
    function pop() {
        internalStack.pop()
    }

    function hide() {
        opacity = 0
    }
    function show() {
        opacity = 1
    }

    anchors.fill: parent

    StackView {
        id: internalStack
        anchors.fill: parent // make sure our stack takes the full size
        initialItem: baseComponent !== null ? baseComponent : baseUrl
    }
}

