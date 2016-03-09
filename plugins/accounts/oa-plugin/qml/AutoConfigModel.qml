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

VisualItemModel {
    id: newAccount

    Label {
        text: i18n.tr("Username:")
        anchors.left: parent.left
        anchors.right: parent.right
    }

    TextField {
        id: usernameField
        objectName: "usernameField"
        placeholderText: i18n.tr("Your username")
        anchors.left: parent.left
        anchors.right: parent.right
        KeyNavigation.priority: KeyNavigation.BeforeItem
        KeyNavigation.tab: passwordField
        onTextChanged: imap.username = text
    }

    Label {
        text: i18n.tr("Password:")
        anchors.left: parent.left
        anchors.right: parent.right
    }

    TextField {
        id: passwordField
        objectName: "passwordField"
        placeholderText: i18n.tr("Your password")
        echoMode: TextInput.Password
        anchors.left: parent.left
        anchors.right: parent.right
        inputMethodHints: Qt.ImhSensitiveData
        onTextChanged: imap.password = text
    }

    Item {
        anchors.left: parent.left
        anchors.right: parent.right
        height: visible ? units.gu(5) : 0
        visible: ai.running
        ActivityIndicator {
            id: ai
            anchors.centerIn: parent
            running: autoDiscover.inProgress
        }
    }
}
