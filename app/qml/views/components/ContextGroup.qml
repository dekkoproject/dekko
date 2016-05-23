import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Components 1.0
import "../delegates"

ListItem {
    height: group.height
    // Takes list of ContextAction
    property list<ContextAction> contextActions
    PixelPerfectItem {
        anchors.fill: parent

        Column {
            id: group
            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
            }

            Repeater {
                id: repeater
                model: contextActions
                delegate: ContextDelegate {
                    visible: model.visible
                    description: model.description
                    actionIcon: model.actionIcon
                    actionIconColor: model.actionIconColor

                    selectedIcon: model.selectedIcon
                    selected: model.selected
                    selectedIconColor: model.selectedIconColor
                    onClicked: contextActions[model.index].trigger()
                }
            }
        }
    }
}
