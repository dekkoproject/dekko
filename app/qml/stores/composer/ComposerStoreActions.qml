import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Components 1.0
import "../../actions/composer"

QtObject {

    property Action sendAction: Action {
        text: qsTr("Send")
        iconName: Icons.SendIcon
        iconSource: Paths.actionIconUrl(Icons.SendIcon)
        onTriggered: ComposerActions.sendMessage()
    }

    property Action saveDraftAction: Action {
        text: qsTr("Save draft")
        iconName: Icons.DraftIcon
        iconSource: Paths.actionIconUrl(Icons.DraftIcon)
        onTriggered: ComposerActions.saveDraft()
    }

    property Action discardMessageAction: Action {
        text: qsTr("Discard")
        iconName: Icons.DeleteIcon
        iconSource: Paths.actionIconUrl(Icons.DeleteIcon)
        onTriggered: ComposerActions.discardMessage()
    }
}

