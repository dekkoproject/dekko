import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.1
import Dekko.Theme 0.2

ActionSelectionPopover {
    id: actionPopover
    z: 100
    property string mboxName
    property var uid
    delegate: ListItem {
        height: layout.height
        ListItemLayout {
            id: layout
            title.text: action.text
            title.font.weight: Font.Normal
        }
        onClicked: {
            action.trigger()
            actionPopover.close()
        }
    }

    function close() {
        PopupUtils.close(actionPopover)
    }

    InverseMouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        onPressed: actionPopover.close()
    }
    //If after switching back to small form factor and the messageview
    // is displaying a valid index then this popover is now ontop of the messageview
    // which is just weird so just close this popover and let the user go back and re-open
    Connections {
        target: dekko.viewState
        onStateChanged: {
            if (dekko.viewState.isSmallFF && dekko.currentAccount.singleThreadModel.hasValidIndex) {
                actionPopover.close()
            }
        }
    }
}
