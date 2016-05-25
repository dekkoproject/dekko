import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Themes.Ambiance 1.3
import "../../actions/logging"
import "../../constants"
import "../../stores/composer"

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

