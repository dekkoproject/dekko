/* Copyright (C) 2015 Dan Chapman <dpniel@ubuntu.com>

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

Row {
    id: buttons
    signal continueClicked()
    signal cancelClicked()
    height: units.gu(7)
    anchors.left: parent.left
    anchors.right: parent.right
    spacing: units.gu(1)
    Button {
        id: btnCancel
        objectName: "cancelButton"
        text: i18n.tr("Cancel")
        color: "#1c091a"
        height: units.gu(5)
        width: (parent.width / 2) - 0.5 * parent.spacing
        onClicked: cancelClicked()
        anchors.bottom: parent.bottom
    }
    Button {
        id: btnContinue
        objectName: "continueButton"
        text: i18n.tr("Continue")
        color: "#cc3300"
        height: units.gu(5)
        width: (parent.width / 2) - 0.5 * parent.spacing
        onClicked: continueClicked()
        anchors.bottom: parent.bottom
    }
}
