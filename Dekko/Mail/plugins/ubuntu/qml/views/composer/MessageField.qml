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
import Dekko.Mail.API 1.0
import Dekko.Mail.Stores.Composer 1.0
import "../../constants"

FocusScope {
    id: root
    property alias placeholderText: textArea.placeholderText
    property alias text: textArea.text
    property alias textDocument: textArea.textDocument
    property alias __textArea: textArea

    signal keyEventOccurred()
    signal cursorPositionChanged()

    function getCursorPosition() {
        return textArea.positionToRectangle(textArea.cursorPosition)
    }
    height: textArea.height
    anchors {
//        top: parent.top
        left: parent.left
        right: parent.right
        leftMargin: Style.smallSpacing
        rightMargin: Style.smallSpacing
    }

    TextArea {
        id: textArea
        anchors {
            left: parent.left
            top: parent.top
            topMargin: Style.smallSpacing
            right: parent.right
        }
        autoSize: true
        maximumLineCount: 0
        style: TextAreaStyle {
            overlaySpacing: Style.smallSpacing
            frameSpacing: Style.smallSpacing
            background: Item{}
        }
        onCursorPositionChanged: root.cursorPositionChanged()
        Keys.onReleased: keyEventOccurred()
        onActiveFocusChanged: {
            if (activeFocus) {
                delayPositioning.start()
            } else {
                delayPositioning.stop()
            }
        }
    }

    Binding {
        target: ComposerStore
        property: "bodyDocument"
        value: textArea.textDocument
    }

    // This timer is used to position the view after
    // active focus has been gained so that the cursor doesn't get stuck down behind the keyboard
    Timer {
        id: delayPositioning
        interval: 750
        repeat: false
        onTriggered: {
            Log.logInfo("MessageField::delayPositioningTimer", "Repositiong view")
            if (!QuickUtils.mouseAttached && !Qt.inputMethod.visible) {
                // Hmm keyboards not up yet. Try again....
                Log.logInfo("MessageField::delayPositioningTimer", "Keyboards not visible yet")
                delayPositioning.start()
            } else {
                Log.logInfo("MessageField::delayPositioningTimer", "Ok let's position view")
                root.keyEventOccurred()
            }
        }
    }
}

