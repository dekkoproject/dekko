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
import Ubuntu.Components.Themes.Ambiance 1.3
import Dekko.Components 1.0
import Dekko.Mail.Stores.Composer 1.0
import "../components"
import "../../constants"

FocusScope {

    readonly property int defaultHeight: units.gu(4)
    readonly property alias text: input.text
    property alias textDocument: input.textDocument

    anchors {
        left: parent.left
        right: parent.right
    }

    clip: true
    height: input.lineCount > 1 ? input.height + Style.smallSpacing : defaultHeight
    implicitHeight: height
    implicitWidth: width

    StretchRow {
        anchors {
            fill: parent
            leftMargin: Style.defaultSpacing
            rightMargin: Style.defaultSpacing
        }
        spacing: Style.smallSpacing

        Label {
            anchors {
                left: parent.left
                top: parent.top
                topMargin: Style.smallSpacing
            }
            text: qsTr("Subject:")
        }

        Stretcher {
            height: input.height
            implicitHeight: height

            TextArea {
                id: input
                anchors {
                    left: parent.left
                    top: parent.top
                    right: parent.right
                    topMargin: Style.smallSpacing
                }
                maximumLineCount: 3
                wrapMode: TextEdit.WrapAnywhere
                autoSize: true
                height: defaultHeight
                style:TextAreaStyle {
                    overlaySpacing: 0
                    frameSpacing: 0
                    background: Item {}
                }
            }

            Binding {
                target: ComposerStore
                property: "subjectDocument"
                value: input.textDocument
            }
        }
    }

    Line {
        anchors {
            bottom: parent.bottom
            right: parent.right
            left: parent.left
        }
    }
}

