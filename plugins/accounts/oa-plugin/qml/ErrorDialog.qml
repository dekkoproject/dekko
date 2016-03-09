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
import Ubuntu.Components.Popups 1.1

Dialog {
    id: dlg

    // Required by PopUpQueue
    // TODO: create a BaseDialog component to share this between
    // this dialog and the service validation dialog
    signal closing()
    Component.onDestruction: closing()

    contents: Button {
        width: parent.width
        text: i18n.tr("Close")
        onClicked: PopupUtils.close(dlg)
    }
}

