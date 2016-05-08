import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import "../../actions/popups"

DialogBase {
    id: confirmationDialog

    property Action confirmAction: Action {
        onTriggered: PopupActions.confirmationDialogConfirmed(id)
    }

    property Action cancelAction: Action {
        onTriggered: PopupActions.confirmationDialogCancelled(id)
    }
    property string id: "default"
    property alias confirmButtonText: confirmButton.text
    property alias cancelButtonText: cancelButton.text
    property alias confirmButton: confirmButton
    property alias cancelButton: cancelButton

    signal confirmClicked
    signal cancelClicked

    contents: [

        Row {
            anchors {left: parent.left; right: parent.right}
            spacing: units.gu(1)

            Button {
                id: cancelButton
                text: qsTr("Cancel")
                color: UbuntuColors.red
                width: parent.width / 2 - units.gu(0.5)
                onClicked: {
                    if (cancelAction) {
                        cancelAction.triggered(cancelButton)
                    }
                    PopupUtils.close(confirmationDialog)
                    cancelClicked()
                }
            }
            Button {
                id: confirmButton
                text: qsTr("Confirm")
                color: UbuntuColors.green
                width: parent.width / 2 - units.gu(0.5)
                onClicked: {
                    if (confirmAction) {
                        confirmAction.triggered(confirmButton)
                    }
                    PopupUtils.close(confirmationDialog)
                    confirmClicked()
                }
            }
        }
    ]
}
