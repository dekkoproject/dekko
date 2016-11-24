/* Copyright (C) 2014-2016 Dan Chapman <dpniel@ubuntu.com>

   This file is part of Dekko email client for Ubuntu Devices/

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
import Dekko.Components 1.0
import "../components"

FocusScope {
    id: titledTextField

    height: column.height
    anchors {
        left: parent.left
        right: parent.right
    }

    property string title
    property alias text: textfield.text
    property alias inputMethodHints: textfield.inputMethodHints
    property alias echoMode: textfield.echoMode
    // TODO: There must be a more elegant way than this
    property alias font: textfield.font
    property alias textFieldFocusHandle: textfield
    property bool sendTabEventOnEnter
    property bool requiredField: false
    readonly property bool _inputValid: requiredField ? textfield.text : true
    property alias placeholderText: textfield.placeholderText
    property alias secondaryItem: textfield.secondaryItem
    property Action trailingAction

    Column {
        id: column
        spacing: units.gu(1)
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }

        Label {
            id: titleLabel
            anchors {
                left: parent.left
                leftMargin: units.gu(0.3)
                right: parent.right
            }
            text: !_inputValid ? title + qsTr(" (Required)") : title
            color: !_inputValid ? UbuntuColors.red : UbuntuColors.inkstone
        }
        StretchRow {
            height: textfield.height
            spacing: units.gu(1)
            anchors {
                left: parent.left
                right: parent.right
            }
            TextField {
                id: textfield
                iconName: "ok"
                implicitWidth: -1
                onAccepted: {
                   if (sendTabEventOnEnter) {
                        // TabEventSignaler.sendTabEvent()
                   }
                }
            }
            HeaderButton {
                height: units.gu(6)
                implicitWidth: width
                implicitHeight: height
                iconSize: units.gu(3)
                anchors.verticalCenter: textfield.verticalCenter
                visible: trailingAction
                action: trailingAction
            }
        }
    }
}
