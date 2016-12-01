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
import Dekko.Mail.API 1.0

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
