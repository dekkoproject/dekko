pragma Singleton
import QtQuick 2.4

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
}

