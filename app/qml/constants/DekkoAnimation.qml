pragma Singleton
import QtQuick 2.4
import Dekko.Controls 1.0

QtObject {

    property Transition listViewAddTransition: Transition {
        NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 300 }
        NumberAnimation { property: "scale"; easing.type: Easing.InOutSine; from: 0.4; to: 1.0; duration: 300 }
    }

    property Transition listViewAddDisplacedTransition: Transition {
        NumberAnimation { properties: "y"; duration: 400; easing.type: Easing.InBack }
    }

    property Transition listViewRemoveTransition: Transition {
        NumberAnimation { property: "opacity"; from: 1.0; to: 0; duration: 100 }
    }

    property Transition listViewRemoveDisplacedTransition: Transition {
        SequentialAnimation {
            PauseAnimation { duration: 300 }
            NumberAnimation { properties: "x,y"; duration: 400; easing.type: Easing.OutBack }
        }
    }
    // This animation fades and slides in from the bottom for a push and slides out to the right for pop
    property StackViewDelegate customStackViewDelegate1: StackViewDelegate {
        function transitionFinished(properties)
        {
            properties.exitItem.opacity = 1
        }

        pushTransition: StackViewTransition {
            PropertyAnimation {
                target: enterItem
                property: "y"
                from: target.height / 2
                to: 0
                duration: 400
                easing.type: Easing.OutCubic
            }
            PropertyAnimation {
                target: enterItem
                property: "opacity"
                from: 0
                to: 1
                duration: 400
            }
            PropertyAnimation {
                target: exitItem
                property: "opacity"
                from: 1
                to: 0
            }
        }
        popTransition: StackViewTransition {
            PropertyAnimation {
                target: enterItem
                property: "x"
                from: -target.width
                to: 0
                duration: 400
                easing.type: Easing.OutCubic
            }
            PropertyAnimation {
                target: exitItem
                property: "x"
                from: 0
                to: target.width
                duration: 400
                easing.type: Easing.OutCubic
            }
        }
        replaceTransition: pushTransition
    }


}

